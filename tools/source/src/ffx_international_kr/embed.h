#pragma once

#include <cmrc/cmrc.hpp>

#include <cstdint>
#include <span>

CMRC_DECLARE(embed);

#define EMBED(name, path)                                                                              \
    inline const std::span<const uint8_t> name = [] {                                                  \
        const auto file = cmrc::embed::get_filesystem().open(path);                                    \
        return std::span<const uint8_t>(reinterpret_cast<const uint8_t *>(file.begin()), file.size()); \
    }();

namespace embed
{
    EMBED(kThirdPartyNotices, "assets/ThirdPartyNotices.txt")

    EMBED(kPatchData, "assets/ffx_international_kr.xdelta")
    EMBED(kJpVoicePatchData, "assets/ffx_international_kr.jp_voice.xdelta")
}

#undef EMBED
