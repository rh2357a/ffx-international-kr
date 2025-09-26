#ifndef _FFXISO_IMPORT_H_
#define _FFXISO_IMPORT_H_

#include <filesystem>

namespace ffxiso {

bool import(std::filesystem::path import_dir, std::filesystem::path output_path);

}

#endif
