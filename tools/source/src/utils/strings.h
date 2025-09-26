#ifndef _UTILS_STRINGS_H_
#define _UTILS_STRINGS_H_

#include <chrono>
#include <string>
#include <format>

namespace strings {

inline std::string datetime_str()
{
	using namespace std::chrono;
	auto now = floor<seconds>(system_clock::now());
	zoned_time zt{locate_zone("Asia/Seoul"), now};
	return std::format("{:%Y%m%d_%H%M%S}", zt);
}

} // namespace strings

#endif
