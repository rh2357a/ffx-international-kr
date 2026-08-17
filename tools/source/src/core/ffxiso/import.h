#pragma once

#include <filesystem>

namespace ffxiso
{
    /**
     * @brief ISO 리패킹
     *
     * @param import_dir 데이터 경로
     * @param output_path 출력 경로
     * @return bool 성공 유무
     */
    bool import(std::filesystem::path import_dir, std::filesystem::path output_path);
}
