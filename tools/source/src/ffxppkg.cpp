#include "utils/xdelta3/xdelta3_wrapper.h"
#include "utils/binfile.h"

#include <iostream>
#include <fstream>
#include <filesystem>

void print_usage()
{
	std::cout << "Usage:\n"
			  << "    ffxppkg BASE_ISO_PATH KOREAN_ISO_PATH MULTILANG_ISO_PATH SOURCE_PATH\n";
}

constexpr auto &KOREAN_NAME = "korean.xdelta";
constexpr auto &MULTILANG_NAME = "multilang.xdelta";

int main(int argc, char *argv[])
{
	if (argc != 5)
	{
		print_usage();
		return -1;
	}

	const std::filesystem::path &base_iso_path(argv[1]);
	if (!std::filesystem::exists(base_iso_path))
	{
		std::cout << "Cannot find '" << base_iso_path << "'\n";
		return -2;
	}

	const std::filesystem::path &korean_iso_path(argv[2]);
	if (!std::filesystem::exists(korean_iso_path))
	{
		std::cout << "Cannot find '" << korean_iso_path << "'\n";
		return -2;
	}

	const std::filesystem::path &multilang_iso_path(argv[3]);
	if (!std::filesystem::exists(multilang_iso_path))
	{
		std::cout << "Cannot find '" << multilang_iso_path << "'\n";
		return -2;
	}

	xd3_main_exec({"-e", "-n", "-s", base_iso_path.string(), korean_iso_path.string(), KOREAN_NAME});
	xd3_main_exec({"-e", "-n", "-s", base_iso_path.string(), multilang_iso_path.string(), MULTILANG_NAME});

	const std::filesystem::path &source_path(argv[4]);
	if (std::filesystem::exists(source_path))
		std::filesystem::remove(source_path);

	const auto &korean_xdelta = binfile::read_all_bytes(KOREAN_NAME);
	const auto &multilang_xdelta = binfile::read_all_bytes(MULTILANG_NAME);

	std::ofstream fout(source_path);
	fout << "#include <array>\n";
	fout << "#include <cstdint>\n";
	fout << "\n";
	fout << "const std::array<uint8_t, " << korean_xdelta.size() << "> korean_xdelta_bytes = {";
	for (size_t i = 0; i < korean_xdelta.size(); ++i)
	{
		if (i % 24 == 0)
			fout << "\n";
		fout << "0x"
			 << std::hex << std::setw(2) << std::setfill('0')
			 << std::nouppercase << (int)korean_xdelta[i]
			 << ",";
	}
	fout << "\n";
	fout << "};\n";
	fout << "\n" << std::dec;
	fout << "const std::array<uint8_t, " << multilang_xdelta.size() << "> multilang_xdelta_bytes = {";
	for (size_t i = 0; i < multilang_xdelta.size(); ++i)
	{
		if (i % 24 == 0)
			fout << "\n";
		fout << "0x"
			 << std::hex << std::setw(2) << std::setfill('0')
			 << std::nouppercase << (int)multilang_xdelta[i]
			 << ",";
	}
	fout << "\n";
	fout << "};\n";
	fout.close();

	std::filesystem::remove(KOREAN_NAME);
	std::filesystem::remove(MULTILANG_NAME);

	return 0;
}
