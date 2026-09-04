#pragma once

#include <map>

#include "japanese_voice.h"
#include "japanese_wd.h"

namespace ffx::japanese_event_sound
{
    using Bytes = std::vector<uint8_t>;
    using japanese_voice::detail::read32;
    using japanese_voice::detail::require;
    using japanese_voice::detail::write32;

    struct Mapping
    {
        uint32_t international_file, japanese_file, sound_id, bank_id;
        uint64_t original_hash, japanese_hash;
    };

    inline constexpr std::array<Mapping, 26> mappings{{
        {5530, 5453, 81067, 4037, 0x81f61d28eed47abfULL, 0xcc45fc04d6dd504fULL},
        {5602, 5525, 81067, 4037, 0x81f61d28eed47abfULL, 0xcc45fc04d6dd504fULL},
        {4504, 4427, 81067, 4037, 0x81f61d28eed47abfULL, 0xcc45fc04d6dd504fULL},
        {6016, 5939, 81067, 4037, 0x81f61d28eed47abfULL, 0xcc45fc04d6dd504fULL},
        {5710, 5633, 81067, 4037, 0x81f61d28eed47abfULL, 0xcc45fc04d6dd504fULL},
        {5512, 5435, 81067, 4037, 0x81f61d28eed47abfULL, 0xcc45fc04d6dd504fULL},
        {2902, 2825, 81067, 4037, 0x81f61d28eed47abfULL, 0xcc45fc04d6dd504fULL},
        {5314, 5237, 85102, 4042, 0xce22a458ffbf7288ULL, 0x12751282a515f3deULL},
        {5062, 4985, 84029, 3999, 0x62fe99022de123a2ULL, 0x29d091cb3e50c916ULL},
        {5062, 4985, 84030, 3999, 0xcb572d14b0e1cc06ULL, 0x4b69e43e71eeb20dULL},
        {2794, 2717, 84032, 3999, 0x104585a1314867c6ULL, 0x6c40d90e6daf05e0ULL},
        {2794, 2717, 84033, 3999, 0x859918f7a25e5acdULL, 0x51e28e93a01d7bcbULL},
        {2794, 2717, 84043, 3999, 0x5928be97c7591193ULL, 0xfe92d4986ab4b62bULL},
        {2830, 2753, 84032, 3999, 0x104585a1314867c6ULL, 0x6c40d90e6daf05e0ULL},
        {2830, 2753, 84033, 3999, 0x859918f7a25e5acdULL, 0x51e28e93a01d7bcbULL},
        {2830, 2753, 84043, 3999, 0x5928be97c7591193ULL, 0xfe92d4986ab4b62bULL},
        {1858, 1781, 84033, 3999, 0x859918f7a25e5acdULL, 0x51e28e93a01d7bcbULL},
        {5224, 5147, 89243, 4087, 0xad9cf13f963e9b0aULL, 0x91f54e95a37af9f4ULL},
        {1660, 1583, 85055, 4009, 0x3b4a25b50f0c4d7dULL, 0x2a2d97701e242983ULL},
        {1660, 1583, 85056, 4009, 0x33947dd7e02a240aULL, 0xbad79384919c02f0ULL},
        {5998, 5921, 85049, 4010, 0x6f7826fb19f816f8ULL, 0x1a4d1c5d9f07122eULL},
        {2254, 2177, 85049, 4010, 0x6f7826fb19f816f8ULL, 0x1a4d1c5d9f07122eULL},
        {7168, 7091, 88025, 4088, 0x0eae8ec0594880efULL, 0x9a678ba6d0849b5dULL},
        {7168, 7091, 88026, 4088, 0x53b26c2d72d4ad6dULL, 0x70f885d3387b7b08ULL},
        {7168, 7091, 88027, 4088, 0xb84d64aff5f32f79ULL, 0x5ec6eb6cc9c70d3aULL},
        {7168, 7091, 88028, 4088, 0x45e5b9d2ecaa4593ULL, 0x849ba5dcf43d6da6ULL},
    }};

    inline std::vector<uint32_t> files()
    {
        std::vector<uint32_t> out;
        for (const auto &m : mappings)
            if (std::find(out.begin(), out.end(), m.international_file) == out.end())
                out.push_back(m.international_file);
        return out;
    }

    struct Section
    {
        size_t start, end;
    };

    inline Section section(std::span<const uint8_t> ev)
    {
        require(ev.size() >= 64 && read32(ev, 0) == 0x31305645, "Unsupported event container.");
        const size_t start = read32(ev, 12);
        require(start >= 64 && start <= ev.size() && ev.size() - start >= 32, "Missing event sound section.");
        size_t end = ev.size();
        bool terminated = false;
        for (size_t p = 4; p < 64; p += 4)
        {
            const auto v = read32(ev, p);
            if (v == UINT32_MAX)
            {
                terminated = true;
                break;
            }
            require(v == 0 || (v >= 64 && v <= ev.size()), "Invalid event section pointer.");
            if (v > start)
                end = std::min(end, size_t(v));
        }
        require(terminated && end - start >= 32, "Invalid event section table.");
        require((read32(ev, start) & 0xc0000000u) == 0x40000000u, "Unsupported event sound block.");
        require((read32(ev, start) & 0x3fffffffu) <= end - start - 32, "Event sound block exceeds section.");
        return {start, end};
    }

    struct Record
    {
        size_t offset, size;
        uint32_t id;
    };

    inline std::vector<Record> records(std::span<const uint8_t> ev, Section sec)
    {
        constexpr std::array<uint8_t, 8> magic{'S', 'e', 'S', 'e', 'p', ' ', ' ', ' '};
        std::vector<Record> out;
        for (size_t p = sec.start + 32; p + 16 <= sec.end;)
        {
            if (!std::equal(magic.begin(), magic.end(), ev.begin() + p))
            {
                ++p;
                continue;
            }
            const auto size = read32(ev, p + 12);
            require(size >= 24 && size % 4 == 0 && size <= sec.end - p, "Invalid SeSep record size.");
            const size_t count = ev[p + 16];
            require(count && 21 + count * 2 <= size, "Invalid SeSep track table.");
            size_t track_end = 21 + count * 2;
            for (size_t i = 0; i < count; ++i)
                track_end += (uint32_t(ev[p + 21 + i * 2]) | (uint32_t(ev[p + 22 + i * 2]) << 8)) & 0x7fff;
            require(track_end <= size, "SeSep tracks exceed record.");
            out.push_back({p, size, read32(ev, p + 8)});
            p += size;
        }
        return out;
    }

    inline Bytes replace_raw(std::span<const uint8_t> original, std::span<const uint8_t> japanese, uint32_t file)
    {
        const auto ts = section(original), js = section(japanese);
        const auto tr = records(original, ts), jr = records(japanese, js);
        std::map<size_t, std::pair<Record, Record>> edits;
        const size_t old_decl = read32(original, ts.start) & 0x3fffffffu;
        size_t last_record = ts.start + 32;
        for (const auto &r : tr)
            if (r.offset < ts.start + 32 + old_decl)
                last_record = std::max(last_record, r.offset + r.size);
        int64_t delta = 0;
        for (const auto &m : mappings)
        {
            if (m.international_file != file)
                continue;
            const auto find_record = [&](const auto &rs) {
                auto it = std::find_if(rs.begin(), rs.end(), [&](const auto &r) { return r.id == m.sound_id; });
                require(it != rs.end() && std::count_if(rs.begin(), rs.end(), [&](const auto &r) { return r.id == m.sound_id; }) == 1, "Missing or duplicate mapped SeSep ID.");
                return *it;
            };
            const auto a = find_record(tr), b = find_record(jr);
            const auto av = original.subspan(a.offset, a.size), bv = japanese.subspan(b.offset, b.size);
            require(japanese_wd::detail::fingerprint(av) == m.original_hash && japanese_wd::detail::fingerprint(bv) == m.japanese_hash,
                "Unsupported or already modified event sound record.");
            require((uint32_t(av[17]) | (uint32_t(av[18]) << 8)) == m.bank_id && (uint32_t(bv[17]) | (uint32_t(bv[18]) << 8)) == m.bank_id, "Unexpected SeSep WD bank.");
            require(a.offset + a.size <= ts.start + 32 + old_decl || a.size == b.size, "Cannot resize a trailing SeSep record.");
            require(edits.emplace(a.offset, std::pair{a, b}).second, "Duplicate event sound edit.");
            if (a.offset + a.size <= ts.start + 32 + old_decl)
                delta += int64_t(b.size) - int64_t(a.size);
        }

        require(!edits.empty(), "Event has no sound mapping.");

        // All supported originals have enough zero padding: keep every EV section pointer,
        // ATEL instruction offset, text section and total uncompressed size unchanged.
        Bytes block;
        size_t cursor = ts.start;
        for (const auto &[offset, pair] : edits)
        {
            block.insert(block.end(), original.begin() + cursor, original.begin() + offset);
            block.insert(block.end(), japanese.begin() + pair.second.offset, japanese.begin() + pair.second.offset + pair.second.size);
            cursor = offset + pair.first.size;
        }
        block.insert(block.end(), original.begin() + cursor, original.begin() + ts.end);
        const size_t capacity = ts.end - ts.start;
        if (block.size() > capacity)
            require(std::all_of(block.begin() + capacity, block.end(), [](uint8_t x) { return x == 0; }), "Insufficient event sound padding.");
        block.resize(capacity, 0);
        const int64_t used = int64_t(last_record - ts.start - 32) + delta;
        require(used >= 0, "Invalid rebuilt sound block size.");
        const size_t declared = std::max(old_decl, (size_t(used) / 16 + 1) * 16);
        require(declared <= capacity - 32 && declared <= 0xffff, "Rebuilt sound block exceeds loader capacity.");
        write32(block, 0, 0x40000000u | static_cast<uint32_t>(declared));
        Bytes out(original.begin(), original.end());
        std::copy(block.begin(), block.end(), out.begin() + ts.start);
        const auto checked = records(out, ts);
        require(checked.size() == tr.size(), "Event sound record count changed.");
        for (const auto &before : tr)
        {
            auto it = std::find_if(checked.begin(), checked.end(), [&](const auto &r) { return r.id == before.id; });
            require(it != checked.end(), "Event sound ID lost.");
            auto edit = edits.find(before.offset);
            const auto expected = edit == edits.end() ? original.subspan(before.offset, before.size) : japanese.subspan(edit->second.second.offset, edit->second.second.size);
            require(expected.size() == it->size && std::equal(expected.begin(), expected.end(), out.begin() + it->offset), "Event sound preservation check failed.");
        }
        return out;
    }

    inline Bytes replace(std::span<const uint8_t> original, std::span<const uint8_t> japanese, uint32_t file)
    {
        using namespace japanese_voice::detail;
        require(original.size() >= 6 && japanese.size() >= 6 && original[0] == 1 && japanese[0] == 1, "Unsupported mapped event compression.");
        const auto as = read32(original, 1), bs = read32(japanese, 1);
        require(as >= 64 && bs >= 64 && as <= 0x400000 && bs <= 0x400000, "Invalid event decompressed size.");
        const auto a = decompress_lz1(original, as), b = decompress_lz1(japanese, bs);
        const auto raw = replace_raw(a, b, file);
        auto out = compress_lz1(raw);
        require(decompress_lz1(out, raw.size()) == raw, "Event sound recompression failed.");
        return out;
    }
}
