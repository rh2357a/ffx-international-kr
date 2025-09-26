#ifndef _FFXISO_EXTRACT_H_
#define _FFXISO_EXTRACT_H_

#include <filesystem>

namespace ffxiso {

bool extract(std::filesystem::path iso_path, std::filesystem::path output_dir);

}

#endif
