#include <utils/binfile.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

size_t to_size(const std::vector<uint8_t> bytes)
{
    // clang-format off
    return (static_cast<size_t>(bytes[0])) |
           (static_cast<size_t>(bytes[1]) <<  8) |
           (static_cast<size_t>(bytes[2]) << 16) |
           (static_cast<size_t>(bytes[3]) << 24);
    // clang-format on
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Usage:\n"
                  << "    ffxname -e name iso_build_dir\n"
                  << "    ffxname -i name iso_build_dir\n"
                  << "    ffxname -c name iso_build_dir\n";
        return -1;
    }

    std::string method(argv[1]);
    std::string name(argv[2]);
    std::filesystem::path iso_build_dir(argv[3]);

    const auto &target_filename = iso_build_dir / (name + ".bin");
    if (!std::filesystem::exists(target_filename))
    {
        std::cout << "Cannot find '" << target_filename << "'\n";
        return -2;
    }

    const std::vector<std::filesystem::path> name_files{
        iso_build_dir / (name + ".06.bin"),
        iso_build_dir / (name + ".07.bin"),
        iso_build_dir / (name + ".08.bin"),
        iso_build_dir / (name + ".09.bin"),
        iso_build_dir / (name + ".11.bin"),
        iso_build_dir / (name + ".13.bin"),
    };

    if (method == "-e")
    {
        using namespace binfile;

        const std::vector<uint64_t> file_offsets{
            /* 06 */ static_cast<uint64_t>(to_size(read_bytes(target_filename, 0x18, 4))),
            /* 07 */ static_cast<uint64_t>(to_size(read_bytes(target_filename, 0x1c, 4))),
            /* 08 */ static_cast<uint64_t>(to_size(read_bytes(target_filename, 0x20, 4))),
            /* 09 */ static_cast<uint64_t>(to_size(read_bytes(target_filename, 0x24, 4))),
            /* 11 */ static_cast<uint64_t>(to_size(read_bytes(target_filename, 0x2c, 4))),
            /* 13 */ static_cast<uint64_t>(to_size(read_bytes(target_filename, 0x34, 4))),
        };

        const std::vector<size_t> file_sizes{
            /* 06 */ to_size(read_bytes(target_filename, 0x1c, 4)) - to_size(read_bytes(target_filename, 0x18, 4)),
            /* 07 */ to_size(read_bytes(target_filename, 0x20, 4)) - to_size(read_bytes(target_filename, 0x1c, 4)),
            /* 08 */ to_size(read_bytes(target_filename, 0x24, 4)) - to_size(read_bytes(target_filename, 0x20, 4)),
            /* 09 */ to_size(read_bytes(target_filename, 0x2c, 4)) - to_size(read_bytes(target_filename, 0x24, 4)),
            /* 11 */ to_size(read_bytes(target_filename, 0x34, 4)) - to_size(read_bytes(target_filename, 0x2c, 4)),
            /* 13 */ read_all_bytes(target_filename).size() - to_size(read_bytes(target_filename, 0x34, 4)),
        };

        for (int i = 0; i < static_cast<int>(file_sizes.size()); i++)
        {
            const auto &bytes = read_bytes(target_filename, file_offsets[i], static_cast<uint64_t>(file_sizes[i]));
            if (std::filesystem::exists(name_files[i]))
                std::filesystem::remove(name_files[i]);
            write_byte_to_file(name_files[i], bytes);
        }
    }
    else if (method == "-i")
    {
        const std::vector<uint64_t> offset_ptrs{
            /* 06 */ 0x18,
            /* 07 */ 0x1c,
            /* 08 */ 0x20,
            /* 09 */ 0x24,
            /* 11 */ 0x2c,
            /* 13 */ 0x34,
        };

        uint64_t offset = 0x40;
        std::vector<uint8_t> header_bytes(0x40);
        std::vector<uint8_t> data_bytes;

        for (int i = 0; i < static_cast<int>(name_files.size()); i++)
        {
            if (!std::filesystem::exists(name_files[i]))
            {
                std::cout << "Cannot find '" << name_files[i] << "'\n";
                return -3;
            }

            auto data = binfile::read_all_bytes(name_files[i]);
            data_bytes.insert(data_bytes.end(), data.begin(), data.end());

            uint64_t padding = 0;
            uint64_t remainder = static_cast<uint64_t>(data.size()) % 0x10;
            if (remainder != 0)
            {
                padding = 0x10 - remainder;
                data_bytes.insert(data_bytes.end(), padding, 0);
            }

            header_bytes[offset_ptrs[i] + 0] = static_cast<uint8_t>(offset & 0xff);
            header_bytes[offset_ptrs[i] + 1] = static_cast<uint8_t>((offset >> 8) & 0xff);
            header_bytes[offset_ptrs[i] + 2] = static_cast<uint8_t>((offset >> 16) & 0xff);
            header_bytes[offset_ptrs[i] + 3] = static_cast<uint8_t>((offset >> 24) & 0xff);

            offset += padding;
            offset += static_cast<uint64_t>(data.size());
        }

        if (std::filesystem::exists(target_filename))
            std::filesystem::remove(target_filename);

        std::ofstream output(target_filename, std::ios::binary | std::ios::app);
        binfile::append_bytes(output, header_bytes);
        binfile::append_bytes(output, data_bytes);
    }
    else
    {
        for (const auto &path : name_files)
        {
            if (std::filesystem::exists(path))
                std::filesystem::remove(path);
        }
    }

    return 0;
}
