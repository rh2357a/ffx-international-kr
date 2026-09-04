#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <span>
#include <stdexcept>
#include <vector>

namespace ffx::japanese_wd
{
    using Bytes = std::vector<uint8_t>;

    struct BankChange
    {
        uint32_t id;
        uint32_t slots;
        uint32_t old_spu_bytes;
        uint32_t new_spu_bytes;
    };

    struct Result
    {
        Bytes archive;
        Bytes index;
        std::vector<BankChange> changes;
    };

    namespace detail
    {
        inline void require(bool condition, const char *message)
        {
            if (!condition)
                throw std::runtime_error(message);
        }

        inline uint32_t read32(std::span<const uint8_t> b, size_t p)
        {
            require(p <= b.size() && b.size() - p >= 4, "Truncated WD data.");
            return uint32_t(b[p]) | (uint32_t(b[p + 1]) << 8) | (uint32_t(b[p + 2]) << 16) | (uint32_t(b[p + 3]) << 24);
        }

        inline void write32(Bytes &b, size_t p, uint32_t value)
        {
            require(p <= b.size() && b.size() - p >= 4, "Invalid WD output field.");
            for (size_t i = 0; i < 4; ++i)
                b[p + i] = static_cast<uint8_t>(value >> (8 * i));
        }

        // Compatibility fingerprints, not an authentication mechanism.
        inline uint64_t fingerprint(std::span<const uint8_t> b)
        {
            uint64_t hash = 14695981039346656037ULL;
            for (auto byte : b)
                hash = (hash ^ byte) * 1099511628211ULL;
            return hash;
        }

        struct Range
        {
            uint32_t id;
            size_t offset;
            size_t size;
        };

        inline std::vector<Range> read_index(std::span<const uint8_t> archive, std::span<const uint8_t> index)
        {
            require(!archive.empty() && archive.size() % 2048 == 0 && index.size() % 4 == 0,
                "Invalid WD archive/index size.");
            std::vector<Range> ranges;
            for (size_t p = 0; p < index.size(); p += 4)
            {
                const auto sector = read32(index, p);
                if (sector == UINT32_MAX)
                    continue;
                const uint64_t offset = uint64_t(sector) * 2048;
                require(offset < archive.size() && p / 4 <= 65535, "WD index points outside archive.");
                require(read32(archive, static_cast<size_t>(offset)) == (0x4457u | (uint32_t(p / 4) << 16)),
                    "WD bank ID does not match index.");
                ranges.push_back({static_cast<uint32_t>(p / 4), static_cast<size_t>(offset), 0});
            }
            std::sort(ranges.begin(), ranges.end(), [](const auto &a, const auto &b) { return a.offset < b.offset; });
            require(!ranges.empty() && ranges.front().offset == 0, "Invalid WD archive start.");
            for (size_t i = 0; i < ranges.size(); ++i)
            {
                const size_t end = i + 1 < ranges.size() ? ranges[i + 1].offset : archive.size();
                require(end > ranges[i].offset, "Overlapping WD bank entries.");
                ranges[i].size = end - ranges[i].offset;
            }
            return ranges;
        }

        struct Sample
        {
            std::array<uint8_t, 32> region;
            std::span<const uint8_t> audio;
        };

        struct Bank
        {
            size_t table;
            size_t header;
            std::vector<Sample> samples;
        };

        // Only the single-region-per-instrument layout used by the mapped banks.
        // Unmapped banks (including other layouts) are copied byte-for-byte.
        inline Bank read_bank(std::span<const uint8_t> b)
        {
            const size_t groups = read32(b, 8), regions = read32(b, 12), pcm_size = read32(b, 4);
            require(groups > 0 && groups == regions && regions <= 256 && pcm_size > 0 && pcm_size <= 0x80000,
                "Unsupported WD region layout or sample memory size.");
            Bank bank{32 + ((groups + 3) & ~size_t(3)) * 4, 0, {}};
            bank.header = bank.table + regions * 32;
            require(bank.header < 0x8000 && bank.header <= b.size() && pcm_size <= b.size() - bank.header,
                "WD sample data exceeds bank.");
            std::vector<uint32_t> offsets{static_cast<uint32_t>(pcm_size)};
            for (size_t i = 0; i < regions; ++i)
            {
                require(read32(b, 32 + i * 4) == bank.table + i * 32, "Unexpected WD instrument pointer.");
                const auto start = read32(b, bank.table + i * 32 + 4);
                require(start < pcm_size && start % 16 == 0, "Invalid WD sample offset.");
                require((read32(b, bank.table + i * 32) & 0x10) == 0, "Streaming WD regions are not supported.");
                offsets.push_back(start);
            }
            std::sort(offsets.begin(), offsets.end());
            offsets.erase(std::unique(offsets.begin(), offsets.end()), offsets.end());
            for (size_t i = 0; i < regions; ++i)
            {
                const size_t p = bank.table + i * 32;
                const auto start = read32(b, p + 4);
                const auto end = *std::upper_bound(offsets.begin(), offsets.end(), start);
                require((end - start) % 16 == 0, "Unaligned WD ADPCM sample.");
                Sample sample{};
                std::copy_n(b.begin() + p, 32, sample.region.begin());
                sample.audio = b.subspan(bank.header + start, end - start);
                bank.samples.push_back(sample);
            }
            return bank;
        }

        struct Mapping
        {
            uint32_t id;
            uint64_t international_hash;
            uint64_t japanese_hash;
            uint32_t first_slot;
            uint32_t last_slot;
        };

        // Filled from the two supported original discs; no audio is embedded.
        // Same-number region pairs only. Unmatched/additional INT regions stay INT.
        inline constexpr std::array<Mapping, 13> mappings{{
            {1112, 0xcad469d018adf569ULL, 0xae6dd125e6d65ba4ULL, 1, 2},
            {1134, 0xeb171755a361eb55ULL, 0x37b46295f95adc30ULL, 0, 1},
            {1135, 0x5818c7adfe73e6feULL, 0x9938dde4198207a7ULL, 0, 1},
            {1197, 0x783b67ae5bd51328ULL, 0xaa984e2a4aa18d50ULL, 0, 1},
            {1199, 0xda17e57b389ecbb0ULL, 0xf94e530e94d0a8a5ULL, 0, 1},
            {1213, 0x8aadabf6cf0d816aULL, 0xe0a393e18c0bed40ULL, 0, 1},
            {3999, 0xc9c666d4bdc7073bULL, 0x1977026d99a504afULL, 0, 3},
            {4009, 0xdd2bd314e73ec9f8ULL, 0xa1229139ca361dc9ULL, 9, 10},
            {4010, 0xe37e0fb8e4483057ULL, 0x283cecdcd9a47878ULL, 4, 8},
            {4037, 0xdb023e50ed219e28ULL, 0x9f284032c5b1be34ULL, 0, 0},
            {4042, 0x446c855621e86619ULL, 0xb58488b6f2441471ULL, 3, 9},
            {4087, 0x422333e06978f7c6ULL, 0x419786835b74f106ULL, 4, 4},
            {4088, 0x34ecd08ce49517ffULL, 0x911fda2a579811d1ULL, 0, 4},
        }};

        inline Bytes transplant(std::span<const uint8_t> original, std::span<const uint8_t> japanese, const Mapping &mapping)
        {
            require(fingerprint(original) == mapping.international_hash && fingerprint(japanese) == mapping.japanese_hash,
                "Unsupported or already modified WD bank; original discs are required.");
            const auto target = read_bank(original), source = read_bank(japanese);
            require(mapping.first_slot <= mapping.last_slot && mapping.last_slot < target.samples.size() && mapping.last_slot < source.samples.size(), "WD slot mapping exceeds bank.");
            Bytes out(original.begin(), original.begin() + target.header);
            Bytes pcm;
            // Preserve original shared samples; also share identical read-only data.
            std::vector<std::pair<std::span<const uint8_t>, uint32_t>> stored;
            for (size_t i = 0; i < target.samples.size(); ++i)
            {
                const auto &sample = i >= mapping.first_slot && i <= mapping.last_slot ? source.samples[i] : target.samples[i];
                auto found = std::find_if(stored.begin(), stored.end(), [&](const auto &e) {
                    return e.first.size() == sample.audio.size() && std::equal(e.first.begin(), e.first.end(), sample.audio.begin());
                });
                uint32_t start;
                if (found != stored.end())
                    start = found->second;
                else
                {
                    require(pcm.size() + sample.audio.size() <= 0x80000, "Rebuilt WD bank exceeds sample memory ceiling.");
                    start = static_cast<uint32_t>(pcm.size());
                    pcm.insert(pcm.end(), sample.audio.begin(), sample.audio.end());
                    stored.emplace_back(sample.audio, start);
                }
                const size_t p = target.table + i * 32;
                std::copy(sample.region.begin(), sample.region.end(), out.begin() + p);
                write32(out, p + 4, start);
                // Region +8 is relative to the sample start (IOPSOUND 0x2224..0x2230).
                // Keep it, pitch, envelopes and other playback settings from the chosen region.
            }
            pcm.resize((pcm.size() + 63) & ~size_t(63), 0);
            require(pcm.size() <= 0x80000, "Rebuilt WD sample memory size is too large.");
            write32(out, 4, static_cast<uint32_t>(pcm.size()));
            out.insert(out.end(), pcm.begin(), pcm.end());
            out.resize((out.size() + 2047) & ~size_t(2047), 0);
            return out;
        }
    }

    // INT files 547/548 <- JP files 470/471. Keep the INT 0x5000-byte ID table.
    inline Result replace(std::span<const uint8_t> original, std::span<const uint8_t> original_index,
        std::span<const uint8_t> japanese, std::span<const uint8_t> japanese_index)
    {
        using namespace detail;
        require(original_index.size() == 0x5000 && japanese_index.size() == 0xa000, "Unsupported WD index version.");
        const auto targets = read_index(original, original_index);
        const auto sources = read_index(japanese, japanese_index);
        require(targets.size() == 834 && sources.size() == 834, "Unexpected WD bank count.");
        std::map<uint32_t, Range> source_by_id;
        for (const auto &range : sources)
            source_by_id.emplace(range.id, range);
        Result result{{}, Bytes(original_index.begin(), original_index.end()), {}};
        result.archive.reserve(original.size() + 0x100000);
        for (const auto &range : targets)
        {
            require(source_by_id.contains(range.id), "Japanese WD bank ID is missing.");
            const auto bank = original.subspan(range.offset, range.size);
            const auto mapping = std::find_if(mappings.begin(), mappings.end(), [&](const auto &m) { return m.id == range.id; });
            require(result.archive.size() / 2048 <= UINT32_MAX, "WD archive is too large.");
            write32(result.index, range.id * 4, static_cast<uint32_t>(result.archive.size() / 2048));
            if (mapping == mappings.end())
                result.archive.insert(result.archive.end(), bank.begin(), bank.end());
            else
            {
                const auto &source = source_by_id.at(range.id);
                const auto replacement = transplant(bank, japanese.subspan(source.offset, source.size), *mapping);
                result.changes.push_back({range.id, mapping->last_slot - mapping->first_slot + 1,
                    read32(bank, 4), read32(replacement, 4)});
                result.archive.insert(result.archive.end(), replacement.begin(), replacement.end());
            }
        }
        require(result.changes.size() == mappings.size(), "Not all WD mappings were applied.");
        const auto checked = read_index(result.archive, result.index);
        require(checked.size() == targets.size(), "Rebuilt WD index verification failed.");
        return result;
    }
}
