#include "import.h"

#include "mdg.h"
#include "utils/binfile.h"
#include "utils/json.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

// clang-format off

const std::vector<uint8_t> SIZETBL_BYTES{
    0x13, 0x16, 0x00,
};

std::vector<uint8_t> make_header_volume_descriptor(uint32_t sectors)
{
    return {
        static_cast<uint8_t>(sectors & 0xff),
        static_cast<uint8_t>((sectors >> 8) & 0xff),
        static_cast<uint8_t>((sectors >> 16) & 0xff),
        static_cast<uint8_t>((sectors >> 24) & 0xff),

        static_cast<uint8_t>((sectors >> 24) & 0xff),
        static_cast<uint8_t>((sectors >> 16) & 0xff),
        static_cast<uint8_t>((sectors >> 8) & 0xff),
        static_cast<uint8_t>(sectors & 0xff),
    };
}

std::vector<uint8_t> make_volume_descriptor(uint32_t sectors)
{
    uint8_t v = sectors - 1;
    return {
        0x02, 0x00, 0x02, 0x00,
        0x14, 0x00, 0x00, 0x00,
        0x1e, 0xB5, 0xF0, 0x07,
        static_cast<uint8_t>(v & 0xff),
        static_cast<uint8_t>((v >> 8) & 0xff),
        static_cast<uint8_t>((v >> 16) & 0xff),
        static_cast<uint8_t>((v >> 24) & 0xff),
        0x00, 0x80, 0x00, 0x00,
        0x20, 0x00, 0x00, 0x00,
        0x00, 0x80, 0x00, 0x00,
        0x30, 0x00, 0x00, 0x00,
    };
}

// clang-format on

bool ffxiso::import(std::filesystem::path import_dir, std::filesystem::path output_path)
{
    if (std::filesystem::exists(output_path))
        std::filesystem::remove(output_path);

    std::ifstream json_file(import_dir / "files.json");
    nlohmann::json files_json = nlohmann::json::parse(json_file);

    std::ofstream output(output_path, std::ios::binary | std::ios::app);
    std::vector<char> buffer(16 * 1024 * 1024);
    output.rdbuf()->pubsetbuf(buffer.data(), buffer.size());

    // 0
    {
        auto iso_header_bytes = binfile::read_all_bytes(import_dir / "iso_header");
        iso_header_bytes = binfile::pad_sector_bytes(iso_header_bytes);
        binfile::append_bytes(output, iso_header_bytes);
    }

    std::vector<uint8_t> sizetbl;

    // 0x8c000
    {
        uint64_t current_addr = 0xac800;
        std::vector<uint8_t> cdrom_mdg;

        for (int i = 0; i < CDROM_MDG_SIZE; i++)
        {
            auto entry = files_json[i];

            uint64_t current_sector = current_addr / 0x800;
            std::vector<uint8_t> current_mdg_bytes{
                static_cast<uint8_t>(current_sector & 0xff),
                static_cast<uint8_t>((current_sector >> 8) & 0xff),
                static_cast<uint8_t>((current_sector >> 16) & 0x3f),
                0,
            };

            bool is_compressed = entry["is_compressed"].get<bool>();
            if (is_compressed)
                current_mdg_bytes[2] |= (1 << 6);

            bool is_dummy_file = entry["is_dummy_file"].get<bool>();
            if (is_dummy_file)
                current_mdg_bytes[2] |= (1 << 7);

            if (is_dummy_file)
            {
                current_mdg_bytes[3] = 0;
                sizetbl.push_back(0);
                sizetbl.push_back(0);
                sizetbl.push_back(0);
            }
            else
            {
                if (i == 15)
                {
                    uint64_t size = CDROM_MDG_SIZE * 3;
                    uint64_t padding = (2048 - (size % 2048)) % 2048;
                    current_mdg_bytes[3] = 0x1e;
                    current_addr += padding + size;

                    sizetbl.push_back(SIZETBL_BYTES[0]);
                    sizetbl.push_back(SIZETBL_BYTES[1]);
                    sizetbl.push_back(SIZETBL_BYTES[2]);
                }
                else
                {
                    auto file_path = import_dir / "files" / entry["filename"].get<std::string>();
                    auto file_bytes = binfile::read_all_bytes(file_path);

                    if (is_compressed)
                    {
                        uint64_t decomp_size = file_bytes[1]
                                               | (file_bytes[2] << 8)
                                               | (file_bytes[3] << 16)
                                               | (file_bytes[4] << 24);
                        decomp_size = (decomp_size / 8) + (decomp_size % 8 == 0 ? 0 : 1);

                        sizetbl.push_back(static_cast<uint8_t>(decomp_size & 0xff));
                        sizetbl.push_back(static_cast<uint8_t>((decomp_size >> 8) & 0xff));
                        sizetbl.push_back(static_cast<uint8_t>((decomp_size >> 16) & 0xff));
                    }
                    else
                    {
                        uint64_t size = static_cast<uint64_t>(file_bytes.size() / 8);
                        size = size & 0xffffff;
                        sizetbl.push_back(static_cast<uint8_t>(size & 0xff));
                        sizetbl.push_back(static_cast<uint8_t>((size >> 8) & 0xff));
                        sizetbl.push_back(static_cast<uint8_t>((size >> 16) & 0xff));
                    }

                    uint64_t padding = (2048 - (static_cast<uint64_t>(file_bytes.size()) % 2048)) % 2048;
                    current_mdg_bytes[3] = static_cast<uint8_t>(padding / 8);
                    current_addr += padding + static_cast<uint64_t>(file_bytes.size());
                }
            }

            cdrom_mdg.insert(cdrom_mdg.end(), current_mdg_bytes.begin(), current_mdg_bytes.end());
        }

        cdrom_mdg = binfile::pad_sector_bytes(cdrom_mdg);
        binfile::append_bytes(output, cdrom_mdg);

        std::vector<uint8_t> free_sectors(0x20 * 0x800, 0);
        binfile::append_bytes(output, free_sectors);
    }

    // 0xac000
    {
        auto cdrom_fid_bytes = binfile::read_all_bytes(import_dir / "cdrom.fid");
        cdrom_fid_bytes = binfile::pad_sector_bytes(cdrom_fid_bytes);
        binfile::append_bytes(output, cdrom_fid_bytes);
    }

    // 0xac800
    {
        for (int i = 0; i < CDROM_MDG_SIZE; i++)
        {
            if (i == 15)
            {
                sizetbl = binfile::pad_sector_bytes(sizetbl);
                binfile::append_bytes(output, sizetbl);
                continue;
            }

            auto entry = files_json[i];

            if (entry["is_dummy_file"].get<bool>())
                continue;

            auto file_path = import_dir / "files" / entry["filename"].get<std::string>();
            auto file_bytes = binfile::read_all_bytes(file_path);
            file_bytes = binfile::pad_sector_bytes(file_bytes);
            binfile::append_bytes(output, file_bytes);
        }

        uint64_t current_addr = output.tellp();
        uint64_t sectors = current_addr / 0x800;
        uint64_t total_sectors = sectors + 1;
        uint64_t pad_sectors = (16 - (total_sectors % 16)) % 16;

        std::vector<uint8_t> empty_sector(0x800, 0);
        for (uint64_t i = 0; i < pad_sectors; i++)
            binfile::append_bytes(output, empty_sector);

        auto volume_descriptor_bytes = make_volume_descriptor(total_sectors + pad_sectors);
        volume_descriptor_bytes = binfile::pad_sector_bytes(volume_descriptor_bytes);
        binfile::append_bytes(output, volume_descriptor_bytes);

        output.close();

        std::fstream output(output_path, std::ios::binary | std::ios::in | std::ios::out);
        auto header_volume_descriptor_bytes = make_header_volume_descriptor(total_sectors + pad_sectors);
        output.seekp(static_cast<std::streamoff>(0x8050));
        output.write(reinterpret_cast<const char *>(header_volume_descriptor_bytes.data()), header_volume_descriptor_bytes.size());
        output.close();
    }

    return true;
}
