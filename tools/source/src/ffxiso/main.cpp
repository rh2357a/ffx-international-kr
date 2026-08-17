#include <ffxiso/extract.h>
#include <ffxiso/import.h>

#include <iostream>
#include <string>

void print_usage()
{
    std::cout << "Usage:\n"
              << "    ffxiso -e INPUT_ISO OUTPUT_DIR\n"
              << "    ffxiso -i INPUT_DIR OUTPUT_ISO\n";
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        print_usage();
        return -1;
    }

    if (std::string(argv[1]) == "-e")
        return ffxiso::extract(argv[2], argv[3]) ? 0 : -2;

    if (std::string(argv[1]) == "-i")
        return ffxiso::import(argv[2], argv[3]) ? 0 : -3;

    print_usage();
    return -1;
}
