#include "extract.h"

#include "mdg.h"
#include "../utils/strings.h"
#include "../utils/binfile.h"
#include "../utils/json.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <format>

uint64_t seek_compressed_size(const std::vector<uint8_t> &data)
{
	if (data.size() < 5)
		return 0;

	uint64_t pos = 5;
	while (true)
	{
		uint8_t flag = data[pos++];

		if (flag == 0x00)
			break;
		else if (flag < 0x7e)
			pos += flag;
		else if (flag == 0x7e)
			pos += 2;
		else if (flag == 0x7f)
			pos += 3;
		else if (flag >= 0x80)
			pos += 1;
	}

	return (pos + 7) & ~static_cast<uint64_t>(7);
}

bool ffxiso::extract(std::filesystem::path iso_path, std::filesystem::path output_dir)
{
	if (!std::filesystem::exists(iso_path))
	{
		std::cout << "Cannot find '" << iso_path << "'.\n";
		return false;
	}

	if (!std::filesystem::create_directories(output_dir))
	{
		std::cout << "Folder '" << output_dir << "' already exists.\n";
		return false;
	}

	auto iso_header_bytes = binfile::read_bytes(iso_path, 0, 0x8c000);
	binfile::write_byte_to_file(output_dir / "iso_header", iso_header_bytes);

	auto fid_bytes = binfile::read_bytes(iso_path, 0xac000, 0x800);
	binfile::write_byte_to_file(output_dir / "cdrom.fid", fid_bytes);

	std::vector<mdg> cdrom_mdg;
	{
		auto mdg_bytes = binfile::read_bytes(iso_path, 0x8c000, 0x20000);

		for (int i = 0; i < CDROM_MDG_SIZE; i++)
		{
			std::vector<uint8_t> current_mdg_bytes{
				mdg_bytes[i * 4],
				mdg_bytes[i * 4 + 1],
				mdg_bytes[i * 4 + 2],
				mdg_bytes[i * 4 + 3],
			};

			uint8_t flag = 0;
			if ((current_mdg_bytes[2] >> 6) & 1)
				flag |= MDG_FLAG_COMPRESSED_FILE;

			if ((current_mdg_bytes[2] >> 7) & 1)
				flag |= MDG_FLAG_DUMMY_FILE;

			uint64_t addr = static_cast<uint32_t>(current_mdg_bytes[0])
							| (static_cast<uint32_t>(current_mdg_bytes[1]) << 8)
							| ((static_cast<uint32_t>(current_mdg_bytes[2]) & 0x3f) << 16);

			cdrom_mdg.push_back({
				addr * 2048,
				flag,
				static_cast<uint32_t>(current_mdg_bytes[3] * 8),
			});
		}
	}

	nlohmann::json output_json = nlohmann::json::array();
	{
		int i = 0;
		for (const auto &e : cdrom_mdg)
		{
			nlohmann::json mdg_json;
			mdg_json["index"] = i++;
			mdg_json["is_compressed"] = (e.flag & MDG_FLAG_COMPRESSED_FILE) != 0;
			mdg_json["is_dummy_file"] = (e.flag & MDG_FLAG_DUMMY_FILE) != 0;
			output_json.push_back(mdg_json);
		}
	}

	std::filesystem::create_directories(output_dir / "files");

	uint64_t begin_size_addr = cdrom_mdg[15].address;
	for (int i = 0; i < CDROM_MDG_SIZE; i++)
	{
		// sizetbl
		if (i == 15)
			continue;

		if ((cdrom_mdg[i].flag & MDG_FLAG_DUMMY_FILE) != 0)
			continue;

		auto size_bytes = binfile::read_bytes(iso_path, begin_size_addr + (i * 3), 3);
		uint64_t file_size = (static_cast<uint64_t>(size_bytes[0])
							  | (static_cast<uint64_t>(size_bytes[1]) << 8)
							  | (static_cast<uint64_t>(size_bytes[2]) << 16))
							 * 8ULL;

		uint64_t align_file_size = cdrom_mdg[i + 1].address - cdrom_mdg[i].address;
		uint64_t real_file_size = ((cdrom_mdg[i].flag & MDG_FLAG_COMPRESSED_FILE) != 0)
									  ? file_size
								  : align_file_size == file_size
									  ? file_size
									  : align_file_size - (2048 - (file_size % 2048));

		if (i == 7826)
			real_file_size = 0x808;

		auto file_bytes = binfile::read_bytes(iso_path, cdrom_mdg[i].address, real_file_size);

		std::string ext;
		if (binfile::has_bytes(file_bytes, 5, {0x01, 0x01, 0xc0}) || binfile::has_bytes(file_bytes, 5, {0x01, 0x01, 0xb0}))
			ext = "mt";
		else if (binfile::has_bytes(file_bytes, 0, {0x56, 0x53, 0x00, 0x00}))
			ext = "vs";
		else if (binfile::has_bytes(file_bytes, 0, {0x7f, 0x45, 0x4c, 0x46}))
			ext = "elf";
		else if (binfile::has_bytes(file_bytes, 6, {0x42, 0x47, 0x4d, 0x20}) || binfile::has_bytes(file_bytes, 0, {0x42, 0x47, 0x4d, 0x20}))
			ext = "bgm";
		else if (binfile::has_bytes(file_bytes, 5, {0x01, 0x08, 0x80, 0x03, 0x01, 0x30}))
			ext = "bt";
		else if (binfile::has_bytes(file_bytes, 0, {0x08, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00}))
			ext = "bts";
		else if (binfile::has_bytes(file_bytes, 6, {0x45, 0x56, 0x30, 0x31, 0x40}))
			ext = "ev";
		else if (binfile::has_bytes(file_bytes, 6, {0x4d, 0x41, 0x50, 0x31}))
			ext = "map";
		else if (binfile::has_bytes(file_bytes, 0, {0x46, 0x54, 0x43, 0x58}) || binfile::has_bytes(file_bytes, 6, {0x46, 0x54, 0x43, 0x58}))
			ext = "ftcx";
		else
			ext = "bin";

		if ((cdrom_mdg[i].flag & MDG_FLAG_COMPRESSED_FILE) != 0)
		{
			auto comp_type = binfile::read_bytes(iso_path, cdrom_mdg[i].address, 1)[0];

			std::string lz;
			if (comp_type == 0)
				lz = "lz0";
			else if (comp_type == 1)
				lz = "lz1";
			else
				lz = "lz2";

			if (comp_type == 0)
			{
				file_bytes.resize(0x808);

				auto filename = output_dir / "files" / std::format("file_{:05}.{}.{}", i, ext, lz);
				binfile::write_byte_to_file(filename, file_bytes);

				output_json[i]["filename"] = std::format("file_{:05}.{}.{}", i, ext, lz);
			}
			else
			{
				uint64_t size = seek_compressed_size(file_bytes);
				file_bytes.resize(size);

				auto filename = output_dir / "files" / std::format("file_{:05}.{}.{}", i, ext, lz);
				binfile::write_byte_to_file(filename, file_bytes);

				output_json[i]["filename"] = std::format("file_{:05}.{}.{}", i, ext, lz);
			}
		}
		else
		{
			auto filename = output_dir / "files" / std::format("file_{:05}.{}", i, ext);
			binfile::write_byte_to_file(filename, file_bytes);

			output_json[i]["filename"] = std::format("file_{:05}.{}", i, ext);
		}
	}

	std::ofstream output(output_dir / "files.json");
	output << output_json.dump(2);

	return true;
}

std::vector<uint8_t> ffxiso::get_file_bytes(std::filesystem::path iso_path, int index)
{
	std::vector<mdg> cdrom_mdg;
	{
		auto mdg_bytes = binfile::read_bytes(iso_path, 0x8c000, 0x20000);

		for (int i = 0; i < CDROM_MDG_SIZE; i++)
		{
			std::vector<uint8_t> current_mdg_bytes{
				mdg_bytes[i * 4],
				mdg_bytes[i * 4 + 1],
				mdg_bytes[i * 4 + 2],
				mdg_bytes[i * 4 + 3],
			};

			uint8_t flag = 0;
			if ((current_mdg_bytes[2] >> 6) & 1)
				flag |= MDG_FLAG_COMPRESSED_FILE;

			if ((current_mdg_bytes[2] >> 7) & 1)
				flag |= MDG_FLAG_DUMMY_FILE;

			uint64_t addr = static_cast<uint32_t>(current_mdg_bytes[0])
							| (static_cast<uint32_t>(current_mdg_bytes[1]) << 8)
							| ((static_cast<uint32_t>(current_mdg_bytes[2]) & 0x3f) << 16);

			cdrom_mdg.push_back({
				addr * 2048,
				flag,
				static_cast<uint32_t>(current_mdg_bytes[3] * 8),
			});
		}
	}

	uint64_t begin_size_addr = cdrom_mdg[15].address;
	auto size_bytes = binfile::read_bytes(iso_path, begin_size_addr + (index * 3), 3);
	uint64_t file_size = (static_cast<uint64_t>(size_bytes[0])
						  | (static_cast<uint64_t>(size_bytes[1]) << 8)
						  | (static_cast<uint64_t>(size_bytes[2]) << 16))
						 * 8ULL;

	uint64_t align_file_size = cdrom_mdg[index + 1].address - cdrom_mdg[index].address;
	uint64_t real_file_size = ((cdrom_mdg[index].flag & MDG_FLAG_COMPRESSED_FILE) != 0)
								  ? file_size
							  : align_file_size == file_size
								  ? file_size
								  : align_file_size - (2048 - (file_size % 2048));

	return binfile::read_bytes(iso_path, cdrom_mdg[index].address, real_file_size);
}
