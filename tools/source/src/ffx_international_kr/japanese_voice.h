#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace ffx::japanese_voice
{
    namespace detail
    {
        using Bytes = std::vector<uint8_t>;

        inline void require(bool condition, const char *message)
        {
            if (!condition)
                throw std::runtime_error(message);
        }

        inline uint32_t read32(std::span<const uint8_t> bytes, size_t offset)
        {
            require(offset <= bytes.size() && bytes.size() - offset >= 4, "Truncated voice file.");
            return uint32_t(bytes[offset]) | (uint32_t(bytes[offset + 1]) << 8) | (uint32_t(bytes[offset + 2]) << 16) | (uint32_t(bytes[offset + 3]) << 24);
        }

        inline void write32(Bytes &bytes, size_t offset, uint32_t value)
        {
            require(offset <= bytes.size() && bytes.size() - offset >= 4, "Invalid voice field.");
            for (size_t i = 0; i < 4; ++i)
                bytes[offset + i] = static_cast<uint8_t>(value >> (8 * i));
        }

        // FFX LZ1: literal runs, RLE, and 3..18-byte matches within the previous 2048 bytes.
        inline Bytes decompress_lz1(std::span<const uint8_t> input, size_t expected_size)
        {
            require(input.size() >= 6 && input[0] == 1 && read32(input, 1) == expected_size,
                "Unsupported voice file version or compression.");
            Bytes output;
            output.reserve(expected_size);
            size_t pos = 5;
            auto take = [&]() -> uint8_t {
                require(pos < input.size(), "Truncated LZ1 stream.");
                return input[pos++];
            };
            for (;;)
            {
                const auto flag = take();
                if (flag == 0)
                    break;
                size_t count;
                if (flag < 0x7e)
                {
                    count = flag;
                    require(count <= input.size() - pos && count <= expected_size - output.size(),
                        "Invalid LZ1 literal run.");
                    output.insert(output.end(), input.begin() + pos, input.begin() + pos + count);
                    pos += count;
                }
                else if (flag < 0x80)
                {
                    count = take();
                    if (flag == 0x7e)
                        count += 4;
                    else
                        count |= size_t(take()) << 8;
                    const auto value = take();
                    require(count != 0 && count <= expected_size - output.size(), "Invalid LZ1 RLE run.");
                    output.insert(output.end(), count, value);
                }
                else
                {
                    const size_t distance = ((size_t(flag & 7) << 8) | take()) + 1;
                    count = ((flag & 0x7f) >> 3) + 3;
                    require(count <= expected_size - output.size(),
                        "Invalid LZ1 back reference.");
                    // FFX initializes the preceding dictionary to zero. The original
                    // Japanese 9193 stream uses it before 2048 bytes have been decoded.
                    for (size_t i = 0; i < count; ++i)
                        output.push_back(distance > output.size() ? 0 : output[output.size() - distance]);
                }
            }
            require(output.size() == expected_size, "Incomplete LZ1 output.");
            return output;
        }

        inline Bytes compress_lz1(std::span<const uint8_t> input)
        {
            require(input.size() <= UINT32_MAX, "Voice file is too large.");
            Bytes output(5);
            output[0] = 1;
            write32(output, 1, static_cast<uint32_t>(input.size()));
            std::array<int, 65536> head;
            head.fill(-1);
            std::vector<int> previous(input.size(), -1);
            auto hash = [&](size_t pos) {
                return ((uint32_t(input[pos]) * 251 + input[pos + 1]) * 251 + input[pos + 2]) & 65535;
            };
            auto remember = [&](size_t pos) {
                if (pos + 2 < input.size())
                {
                    const auto key = hash(pos);
                    previous[pos] = head[key];
                    head[key] = static_cast<int>(pos);
                }
            };
            size_t literal_start = 0, literal_count = 0;
            auto flush = [&]() {
                if (literal_count)
                {
                    output.push_back(static_cast<uint8_t>(literal_count));
                    output.insert(output.end(), input.begin() + literal_start,
                        input.begin() + literal_start + literal_count);
                    literal_count = 0;
                }
            };
            for (size_t pos = 0; pos < input.size();)
            {
                size_t run = 1, match = 0, distance = 0;
                while (run < 65535 && pos + run < input.size() && input[pos + run] == input[pos])
                    ++run;
                if (pos + 2 < input.size() && run < 18)
                {
                    int candidate = head[hash(pos)];
                    for (int attempts = 0; candidate >= 0 && attempts < 128; ++attempts)
                    {
                        const size_t source = static_cast<size_t>(candidate);
                        if (pos - source > 2048)
                            break;
                        size_t length = 0;
                        while (length < 18 && pos + length < input.size() && input[source + length] == input[pos + length])
                            ++length;
                        if (length > match)
                        {
                            match = length;
                            distance = pos - source;
                        }
                        if (match == 18)
                            break;
                        candidate = previous[source];
                    }
                }
                size_t count = 1;
                if (run >= 4 && run >= match)
                {
                    flush();
                    count = run;
                    if (run <= 259)
                    {
                        output.push_back(0x7e);
                        output.push_back(static_cast<uint8_t>(run - 4));
                    }
                    else
                    {
                        output.push_back(0x7f);
                        output.push_back(static_cast<uint8_t>(run));
                        output.push_back(static_cast<uint8_t>(run >> 8));
                    }
                    output.push_back(input[pos]);
                }
                else if (match >= 3)
                {
                    flush();
                    count = match;
                    output.push_back(static_cast<uint8_t>(0x80 | ((match - 3) << 3) | ((distance - 1) >> 8)));
                    output.push_back(static_cast<uint8_t>(distance - 1));
                }
                else
                {
                    if (!literal_count)
                        literal_start = pos;
                    if (++literal_count == 125)
                        flush();
                }
                for (size_t i = 0; i < count; ++i)
                    remember(pos + i);
                pos += count;
            }
            flush();
            output.push_back(0);
            output.resize((output.size() + 7) & ~size_t(7), 0);
            return output;
        }

        struct Clip
        {
            size_t descriptor;
            size_t offset;
            size_t size;
        };

        // The resource table's +8 field becomes an absolute pointer in 0x002B4E00.
        // The separate relative-pointer table controls placement, so shrinking a clip
        // must not move that table, the following resources, or any overlay code.
        inline std::array<Clip, 3> read_clips(std::span<const uint8_t> bytes, size_t base, bool japanese)
        {
            require(read32(bytes, 0) == (japanese ? 0x01b00de0u : 0x01b00df0u) && read32(bytes, 0x5c) == (japanese ? 0x002790d8u : 0x00283ff0u),
                "Unexpected executable overlay in voice file.");
            require(read32(bytes, base + 0x18) == 0x360 && read32(bytes, base + 0x3c) == 0x410,
                "Unexpected voice resource layout.");
            const size_t data = base + read32(bytes, base + 0x3c);
            require(read32(bytes, data + 0x18) == 0x60 && read32(bytes, data + 0x4c) == 9,
                "Unexpected voice resource pointer table.");
            std::array<Clip, 3> clips;
            constexpr std::array<uint32_t, 3> offsets{0x1a3f0, 0x1cbf0, 0x1f3f0};
            for (size_t i = 0; i < clips.size(); ++i)
            {
                const size_t descriptor = base + 0x360 + (6 + i) * 16;
                const auto relative = read32(bytes, data + 0x60 + (6 + i) * 4);
                const auto size = read32(bytes, descriptor + 4);
                require(relative == offsets[i] && read32(bytes, descriptor) == 2 && read32(bytes, descriptor + 8) == size && read32(bytes, descriptor + 12) == 0 && (size == 0x2800 || (!japanese && i == 2 && size == 0x4000)),
                    "Unexpected VS resource descriptor.");
                const size_t offset = data + relative;
                require(offset <= bytes.size() && size <= bytes.size() - offset, "VS resource exceeds file.");
                const auto mode = read32(bytes, offset + 4);
                require(mode == 0 || mode == 0x100, "Unsupported VS mode.");
                const auto pitch = read32(bytes, offset + 16);
                require(pitch > 0 && pitch <= 0x4000, "Invalid VS pitch.");
                for (size_t block = 0; block < size / 2048; ++block)
                {
                    const size_t start = offset + block * 2048;
                    require(read32(bytes, start) == 0x5356 && read32(bytes, start + 4) == mode && read32(bytes, start + 8) == block && read32(bytes, start + 12) == size / 2048 - block && std::equal(bytes.begin() + offset + 16, bytes.begin() + offset + 32, bytes.begin() + start + 16),
                        "Invalid VS block sequence.");
                    for (size_t frame = start + 32; frame < start + 2048; frame += 16)
                        require((bytes[frame] >> 4) <= 4 && (bytes[frame] & 15) <= 12 && bytes[frame + 1] <= 7,
                            "Invalid VS ADPCM frame.");
                }
                clips[i] = {descriptor, offset, size};
            }
            return clips;
        }
    }

    // SLPM_675.13 file 9514 <- SLPS_250.50 file 9193. No game audio is embedded.
    inline std::vector<uint8_t> replace_9514(std::span<const uint8_t> international,
        std::span<const uint8_t> japanese)
    {
        using namespace detail;
        auto target = decompress_lz1(international, 0x100a80);
        const auto source = decompress_lz1(japanese, 0x0fefc0);
        const auto destinations = read_clips(target, 0x12470, false);
        const auto sources = read_clips(source, 0x12190, true);
        constexpr std::array<size_t, 3> capacities{0x2800, 0x2800, 0x4000};
        for (size_t i = 0; i < sources.size(); ++i)
        {
            const auto &dst = destinations[i];
            const auto &src = sources[i];
            require(src.size <= capacities[i] && capacities[i] <= target.size() - dst.offset,
                "Japanese VS exceeds original slot.");
            std::fill_n(target.begin() + dst.offset, capacities[i], uint8_t(0));
            std::copy_n(source.begin() + src.offset, src.size, target.begin() + dst.offset);
            write32(target, dst.descriptor + 4, static_cast<uint32_t>(src.size));
            write32(target, dst.descriptor + 8, static_cast<uint32_t>(src.size));
        }
        auto compressed = compress_lz1(target);
        require(decompress_lz1(compressed, target.size()) == target, "Voice recompression verification failed.");
        return compressed;
    }
}
