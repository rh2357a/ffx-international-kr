#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

namespace ffxiso
{
    /**
     * @brief ISO 해제
     *
     * @param iso_path ISO 경로
     * @param output_dir 출력 경로
     * @return bool 성공 유무
     */
    bool extract(std::filesystem::path iso_path, std::filesystem::path output_dir);

    /**
     * @brief ISO 파일 추출
     *
     * @param iso_path ISO 경로
     * @param index 파일 번호
     * @return std::vector<uint8_t> 바이너리 데이터
     */
    std::vector<uint8_t> get_file_bytes(std::filesystem::path iso_path, int index);
}
