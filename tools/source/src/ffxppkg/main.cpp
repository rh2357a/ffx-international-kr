#include <utils/binfile.h>
#include <xdelta3_wrapper.h>

#include <filesystem>
#include <iostream>

void print_usage()
{
    std::cout << "Usage:\n"
              << "    ffxppkg BASE_ISO_PATH KOREAN_ISO_PATH MULTILANG_ISO_PATH SOURCE_DIR\n";
}

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

    const std::filesystem::path &source_path(argv[4]);
    xd3_main_exec({"-e", "-f", "-A=.", "-n", "-s", base_iso_path.string(), korean_iso_path.string(), (source_path / "patch_korean.g.xdelta").string()});
    xd3_main_exec({"-e", "-f", "-A=.", "-n", "-s", base_iso_path.string(), multilang_iso_path.string(), (source_path / "patch_multilang.g.xdelta").string()});

    return 0;
}
