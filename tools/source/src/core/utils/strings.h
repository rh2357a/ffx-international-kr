#pragma once

#include <chrono>
#include <format>
#include <string>

namespace strings
{
    inline std::string datetime_str()
    {
        using namespace std::chrono;
        auto now = floor<seconds>(system_clock::now());
        zoned_time zt{locate_zone("Asia/Seoul"), now};
        return std::format("{:%Y%m%d_%H%M%S}", zt);
    }
}
