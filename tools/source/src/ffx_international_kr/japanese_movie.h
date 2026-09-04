#pragma once

#include <algorithm>
#include <cstdint>
#include <istream>
#include <ostream>
#include <span>
#include <stdexcept>
#include <vector>

namespace ffx::japanese_movie
{
    using Bytes = std::vector<uint8_t>;

    inline void require(bool ok, const char *message)
    {
        if (!ok)
            throw std::runtime_error(message);
    }

    inline uint32_t read32(std::span<const uint8_t> b, size_t p)
    {
        require(p <= b.size() && b.size() - p >= 4, "Truncated movie data.");
        return uint32_t(b[p]) | uint32_t(b[p + 1]) << 8 | uint32_t(b[p + 2]) << 16 | uint32_t(b[p + 3]) << 24;
    }

    inline void write32(Bytes &b, size_t p, uint32_t v)
    {
        for (size_t i = 0; i < 4; ++i)
            b.at(p + i) = static_cast<uint8_t>(v >> (i * 8));
    }

    struct Audio
    {
        size_t begin, end;
        bool present;
    };

    inline Audio audio(std::span<const uint8_t> frame)
    {
        const size_t p = size_t(read32(frame, 0)) + 16;
        const auto code = read32(frame, p);
        const auto raw = read32(frame, p + 4);
        const auto stored = read32(frame, p + 8);
        const auto extra = read32(frame, p + 12);
        require(p <= frame.size() && frame.size() - p >= 32 && stored == frame.size() - p - 32 && extra <= stored, "Unexpected movie audio extent.");
        require(code == 0 || code == 0x40 || code == 0x51 || code == 0x6400 || code == 0x8000 || code == 0x8700,
            "Unexpected movie audio format.");
        if (!code)
            require(!raw && !stored && !extra, "Unexpected empty movie audio block.");
        if (code >= 0x6400)
            require(raw == code && stored == code && extra == code,
                "Unexpected movie PCM layout.");
        if (code == 0x40)
            require(raw > 0 && raw <= stored - extra, "Invalid movie DPCM size.");
        if (code == 0x51)
            require(stored - extra >= 6 && frame[p + 16] == 2 && read32(frame, p + 17) == raw,
                "Invalid movie LZ2 header.");
        return {p, p + 16 + stored, code != 0};
    }

    // Decode only the single block containing the 13-second kiss-scene cut.
    // PCM uses successive 256-sample left/right groups, not interleaved samples.
    inline Bytes pcm(std::span<const uint8_t> frame, Audio a)
    {
        const auto code = read32(frame, a.begin), raw = read32(frame, a.begin + 4);
        auto data = frame.subspan(a.begin + 16, a.end - a.begin - 16);
        if (code >= 0x6400)
            return Bytes(data.begin(), data.begin() + 25600);
        Bytes unpacked;
        if (code == 0x51)
        {
            size_t pos = 5;
            auto take = [&]() { require(pos < data.size(), "Truncated movie LZ2."); return data[pos++]; };
            for (;;)
            {
                auto flag = take();
                if (!flag)
                    break;
                if (flag & 128)
                {
                    size_t count = ((flag >> 4) & 7) + 3, distance = ((flag & 15) << 8) + take() + 1;
                    require(unpacked.size() + count <= raw, "Movie LZ2 overflow.");
                    while (count--)
                        unpacked.push_back(distance > unpacked.size() ? 0 : unpacked[unpacked.size() - distance]);
                }
                else if (flag >= 126)
                {
                    size_t count = take();
                    if (flag == 126)
                        count += 4;
                    else
                        count |= size_t(take()) << 8;
                    const auto value = take();
                    require(count && unpacked.size() + count <= raw, "Movie LZ2 repeat overflow.");
                    unpacked.insert(unpacked.end(), count, value);
                }
                else
                {
                    require(unpacked.size() + flag <= raw, "Movie LZ2 literal overflow.");
                    while (flag--)
                        unpacked.push_back(take());
                }
            }
            require(unpacked.size() == raw, "Incomplete movie LZ2.");
            data = unpacked;
        }

        require(raw <= data.size(), "Truncated movie DPCM.");

        Bytes result;
        int previous = 0;
        for (size_t pos = 0; pos < raw;)
        {
            const auto c = data[pos++];
            int delta = c & 127;
            if (delta & 64)
                delta -= 128;
            if (!(c & 128))
            {
                require(pos < raw, "Truncated movie DPCM sample.");
                delta = delta * 256 + data[pos++];
            }
            previous = (previous + delta) & 65535;
            result.push_back(static_cast<uint8_t>(previous));
            result.push_back(static_cast<uint8_t>(previous >> 8));
            require(result.size() <= 25600, "Movie PCM overflow.");
        }

        require(result.size() == 25600, "Unexpected movie PCM sample count.");
        return result;
    }

    // Preserve Japanese trailers. Switch existing stereo audio at decoded
    // time 13 seconds (kiss) or 420 seconds (ending). Only the partial kiss
    // boundary block is converted to PCM; all other audio blocks are copied.
    // Ending video also switches to International at that same audio transition.
    inline Bytes preserve_song(std::span<const uint8_t> international_index,
        std::istream &international_data, std::span<const uint8_t> japanese_index,
        std::span<const uint8_t> japanese_data, std::ostream &output, bool kiss = false)
    {
        const size_t index_size = kiss ? 16032 : 99448;
        const size_t frame_count = kiss ? 3975 : 24828, block_count = kiss ? 996 : 6216;
        const size_t cut_sample = (kiss ? 13 : 420) * 48000;

        require(international_index.size() == index_size && japanese_index.size() == index_size,
            "Unsupported song movie index.");
        require(std::equal(international_index.begin(), international_index.begin() + 128,
                    japanese_index.begin()),
            "Movie quantization tables differ.");

        Bytes index(japanese_index.begin(), japanese_index.end());

        // Some 3000-frame segment starts prebuffer audio on different video frames.
        // Match audio blocks by decoded sample order, not by video-frame number.
        struct Source
        {
            size_t offset, size;
            Audio extent;
        };
        std::vector<Source> source;
        std::vector<Source> video_source;
        size_t int_pos = 0, int_frames = 0;
        for (size_t p = 128; p < international_index.size(); p += 4)
        {
            const auto size = read32(international_index, p);
            if (!size)
                continue;
            require(size <= 0x20000, "Invalid original movie frame size.");
            Bytes frame(size);
            international_data.read(reinterpret_cast<char *>(frame.data()), size);
            require(bool(international_data), "Truncated original song movie.");
            const auto extent = audio(frame);
            video_source.push_back({int_pos, size, extent});
            if (extent.present)
                source.push_back({int_pos, size, extent});
            int_pos += size;
            ++int_frames;
        }
        require(international_data.peek() == std::char_traits<char>::eof() && !international_data.bad() && int_frames == frame_count && source.size() == block_count, "Unexpected original movie length.");
        international_data.clear();
        size_t jp_pos = 0, frames = 0, blocks = 0;
        bool international_video = false;
        auto write = [&](std::span<const uint8_t> b) {
            output.write(reinterpret_cast<const char *>(b.data()), static_cast<std::streamsize>(b.size()));
            require(bool(output), "Could not write song movie.");
        };
        for (size_t p = 128; p < index.size(); p += 4)
        {
            const auto int_size = read32(international_index, p), jp_size = read32(japanese_index, p);
            require((int_size == 0) == (jp_size == 0), "Movie frame counts differ.");
            if (!jp_size)
                continue;

            require(int_size <= 0x20000 && jp_size <= 0x20000 && jp_pos <= japanese_data.size() && jp_size <= japanese_data.size() - jp_pos,
                "Invalid song movie frame size.");
            const auto jp = japanese_data.subspan(jp_pos, jp_size);
            const auto ja = audio(jp);
            if (!kiss && ja.present && blocks * 6400 >= cut_sample)
                international_video = true;
            Bytes original_video;
            auto video = jp.first(ja.begin);
            if (international_video)
            {
                const auto &src = video_source.at(frames);
                original_video.resize(src.size);
                international_data.seekg(static_cast<std::streamoff>(src.offset));
                international_data.read(reinterpret_cast<char *>(original_video.data()),
                    static_cast<std::streamsize>(original_video.size()));
                require(bool(international_data), "Could not read International credits video.");
                video = std::span<const uint8_t>(original_video).first(src.extent.begin);
            }
            if (ja.present && (blocks + 1) * 6400 > cut_sample)
            {
                require(blocks < source.size(), "Too many Japanese movie audio blocks.");
                const auto &src = source[blocks];
                const auto ia = src.extent;
                Bytes original(src.size);
                international_data.seekg(static_cast<std::streamoff>(src.offset));
                international_data.read(reinterpret_cast<char *>(original.data()), static_cast<std::streamsize>(original.size()));
                require(bool(international_data), "Could not read original movie audio.");
                Bytes boundary;
                auto replacement = std::span<const uint8_t>(original).subspan(ia.begin, ia.end - ia.begin);
                if (blocks * 6400 < cut_sample)
                {
                    const auto keep = cut_sample - blocks * 6400;
                    auto mixed = pcm(original, ia);
                    const auto japanese_pcm = pcm(jp, ja);
                    for (size_t sample = 0; sample < keep; ++sample)
                        for (size_t channel = 0; channel < 2; ++channel)
                        {
                            const size_t offset = (sample / 256) * 1024 + channel * 512 + (sample % 256) * 2;
                            mixed[offset] = japanese_pcm[offset];
                            mixed[offset + 1] = japanese_pcm[offset + 1];
                        }
                    boundary.resize(16 + 25600);
                    for (size_t h = 0; h < 16; h += 4)
                        write32(boundary, h, 25600);
                    std::copy(mixed.begin(), mixed.end(), boundary.begin() + 16);
                    replacement = boundary;
                }
                const size_t new_size = video.size() + replacement.size() + (jp.size() - ja.end);
                require(new_size <= 0x20000, "Rebuilt song movie frame is too large.");
                write(video);
                write(replacement);
                write(jp.subspan(ja.end));
                write32(index, p, static_cast<uint32_t>(new_size));
            }
            else
            {
                const size_t new_size = video.size() + jp.size() - ja.begin;
                require(new_size <= 0x20000, "Rebuilt credits frame is too large.");
                write(video);
                write(jp.subspan(ja.begin));
                write32(index, p, static_cast<uint32_t>(new_size));
            }

            if (ja.present)
                ++blocks;

            jp_pos += jp_size;
            ++frames;
        }

        require(jp_pos == japanese_data.size() && frames == frame_count && blocks == block_count,
            "Unexpected song movie length.");
        return index;
    }
}
