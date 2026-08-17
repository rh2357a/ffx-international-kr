#include <utils/binfile.h>
#include <utils/bitmap_image.hpp>

#include <array>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// clang-format off

const std::vector<uint8_t> FTCX_HEADER{
    0x46, 0x54, 0x43, 0x58, 0xc8, 0x00, 0x14, 0x06,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const std::vector<uint8_t> DEFAULT_FONT_WIDTH{
    0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x07, 0x0a, 0x0c, 0x0d, 0x0c, 0x0c, 0x0b, 0x08,
    0x04, 0x04, 0x0a, 0x0a, 0x03, 0x09, 0x02, 0x0a, 0x0c, 0x03, 0x0c, 0x09, 0x0a, 0x09, 0x0d, 0x04, 0x05, 0x05,
    0x04, 0x0c, 0x0b, 0x0b, 0x0c, 0x07, 0x0d, 0x0d, 0x0a, 0x0a, 0x0a, 0x0a, 0x0c, 0x09, 0x0b, 0x08, 0x0b, 0x0a,
    0x0c, 0x09, 0x0c, 0x0c, 0x0e, 0x0c, 0x0e, 0x08, 0x0d, 0x0b, 0x0e, 0x0b, 0x0e, 0x0c, 0x0e, 0x0a, 0x0a, 0x0c,
    0x0e, 0x0d, 0x0e, 0x0c, 0x0e, 0x0c, 0x0c, 0x0c, 0x0d, 0x09, 0x0c, 0x0d, 0x0c, 0x0d, 0x0a, 0x0d, 0x0c, 0x0b,
    0x0c, 0x0c, 0x0c, 0x0c, 0x0e, 0x0e, 0x0d, 0x0e, 0x0d, 0x0c, 0x0e, 0x0d, 0x0d, 0x0e, 0x0e, 0x0c, 0x0e, 0x0e,
    0x0b, 0x0c, 0x0c, 0x0c, 0x0c, 0x0a, 0x0c, 0x09, 0x0c, 0x09, 0x0b, 0x0b, 0x09, 0x0b, 0x0d, 0x0b, 0x0c, 0x0c,
    0x0c, 0x08, 0x0c, 0x09, 0x0a, 0x09, 0x0b, 0x0a, 0x0c, 0x09, 0x0b, 0x0b, 0x0d, 0x0c, 0x0d, 0x0a, 0x0d, 0x0c,
    0x0e, 0x0b, 0x0d, 0x0d, 0x0e, 0x0c, 0x0e, 0x0b, 0x0e, 0x0b, 0x0d, 0x0a, 0x0e, 0x0a, 0x0d, 0x0c, 0x0e, 0x09,
    0x0c, 0x0e, 0x0c, 0x0e, 0x0c, 0x0e, 0x0c, 0x0c, 0x0b, 0x0c, 0x0a, 0x0b, 0x0e, 0x0e, 0x0a, 0x0d, 0x0d, 0x0b,
    0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0d, 0x0e, 0x0e, 0x0c, 0x08, 0x0c, 0x0b, 0x0b, 0x09, 0x0c, 0x0a, 0x0c, 0x07,
    // 0x0a, 0x0b, 0x09, 0x0e, 0x0a, 0x0a, 0x0b, 0x0b, 0x0c, 0x0e,
    0x0a, 0x0b, 0x09, 0x0e, 0x0a, 0x0a, 0x0b, 0x0b, 0x0c, 0x03,
};

const std::vector<uint8_t> DEFAULT_LATIN_FONT_WIDTH{
    0x0b, 0x0a, 0x0a, 0x0a, 0x09, 0x0a, 0x0a, 0x0a, 0x08, 0x0a, 0x0a, 0x09, 0x0b, 0x0a, 0x0a, 0x0a, 0x0b, 0x0a,
    0x0a, 0x0a, 0x0a, 0x0b, 0x0d, 0x09, 0x0a, 0x09, 0x0a, 0x09, 0x08, 0x09, 0x09, 0x08, 0x09, 0x08, 0x02, 0x06,
    0x08, 0x02, 0x0c, 0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x08, 0x08, 0x09, 0x0e, 0x08, 0x09, 0x08,
};

// clang-format on

const std::array<rgb_t, 4> FONT_COLORS{
    rgb_t{0, 0, 0},
    rgb_t{112, 112, 112},
    rgb_t{190, 190, 190},
    rgb_t{255, 255, 255},
};

void print_usage()
{
    std::cout << "Usage:\n"
              << "    ffxftcx FONT_BITMAP_FILE OUTPUT_DIR PREFIX_NAME\n";
}

uint8_t get_pixel_byte(const bool &is_odd, const rgb_t &first, const rgb_t &second)
{
    if (is_odd)
    {
        // clang-format off
        uint8_t b = 0;
        if ( first == FONT_COLORS[3]) b |= 0b00001100;
        if ( first == FONT_COLORS[2]) b |= 0b00001000;
        if ( first == FONT_COLORS[1]) b |= 0b00000100;
        if (second == FONT_COLORS[3]) b |= 0b11000000;
        if (second == FONT_COLORS[2]) b |= 0b10000000;
        if (second == FONT_COLORS[1]) b |= 0b01000000;
        // clang-format on

        return b;
    }
    else
    {
        // clang-format off
        uint8_t b = 0;
        if ( first == FONT_COLORS[3]) b |= 0b00000011;
        if ( first == FONT_COLORS[2]) b |= 0b00000010;
        if ( first == FONT_COLORS[1]) b |= 0b00000001;
        if (second == FONT_COLORS[3]) b |= 0b00110000;
        if (second == FONT_COLORS[2]) b |= 0b00100000;
        if (second == FONT_COLORS[1]) b |= 0b00010000;
        // clang-format on

        return b;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        print_usage();
        return -1;
    }

    constexpr uint32_t glyph_count = 2704;
    constexpr uint32_t font_bytes_size = 0x2a780;

    std::string font_bitmap_path(argv[1]);
    bitmap_image bmp(font_bitmap_path);

    std::vector<uint8_t> width_bytes(glyph_count, 0xe);
    std::vector<uint8_t> font_bytes(font_bytes_size, 0);

    for (int i = 0; i < static_cast<int>(glyph_count); i++)
    {
        int inner = i % 0xd0;
        int group = i / 0xd0;

        int row, col;
        if (inner < 180)
        {
            row = (group * 6 + inner / 36) * 18;
            col = (inner % 36) * 16;
        }
        else
        {
            row = (group * 6 + 5) * 18;
            col = (inner - 180) * 16;
        }

        uint32_t offset = ((i / 18) * 0x480) + ((i % 18) / 2) * 7;
        bool is_odd = i & 1;

        uint8_t max_width = 0;

        rgb_t first, second;
        for (int j = 0; j < 18; j++)
        {
            for (int k = 0; k < 7; k++)
            {
                int x = col + (k * 2);
                int y = row + j;
                bmp.get_pixel(x, y, first);
                bmp.get_pixel(x + 1, y, second);

                if (first != FONT_COLORS[0])
                    max_width = std::max<uint8_t>(max_width, (uint8_t)((k * 2) + 1));

                if (second != FONT_COLORS[0])
                    max_width = std::max<uint8_t>(max_width, (uint8_t)((k * 2) + 2));

                uint8_t b = get_pixel_byte(is_odd, first, second);
                font_bytes[offset + (j * 0x40) + k] |= b;
            }
        }

        if (max_width >= 0xe)
            max_width = 0xe;
        width_bytes[i] = max_width;
    }

    for (int i = 0; i < 0xd0; i++)
        width_bytes[i] = DEFAULT_FONT_WIDTH[i];
    for (int i = 0; i < 26 * 2; i++)
        width_bytes[i + (0xd0 * 9)] = DEFAULT_LATIN_FONT_WIDTH[i];

    std::filesystem::path output_dir(argv[2]);
    if (std::filesystem::exists(output_dir))
        std::filesystem::remove_all(output_dir);
    std::filesystem::create_directories(output_dir);

    const auto &output_path = output_dir / std::format("{}.ftcx", argv[3]);
    if (std::filesystem::exists(output_path))
        std::filesystem::remove(output_path);

    std::ofstream output(output_path, std::ios::binary | std::ios::app);
    binfile::append_bytes(output, FTCX_HEADER);
    binfile::append_uint32(output, glyph_count);

    // glyph size
    binfile::append_uint16(output, 0xe);
    binfile::append_uint16(output, 0x12);
    binfile::append_zero(output, 8);

    // data address, size
    binfile::append_uint32(output, 0x40);
    binfile::append_uint32(output, 0);

    // image size
    binfile::append_uint16(output, 0x80);
    binfile::append_uint16(output, 0x3f0);
    binfile::append_zero(output, 4);

    // font width address, size
    binfile::append_uint32(output, 0);
    binfile::append_uint32(output, glyph_count);
    binfile::append_zero(output, 8);

    const auto &font_data_path = output_dir / std::format("{}.data.bin", argv[3]);
    if (std::filesystem::exists(font_data_path))
        std::filesystem::remove(font_data_path);

    std::ofstream font_data_output(font_data_path, std::ios::binary | std::ios::app);
    binfile::append_bytes(font_data_output, font_bytes);

    const auto &width_data_path = output_dir / std::format("{}.width.bin", argv[3]);
    if (std::filesystem::exists(width_data_path))
        std::filesystem::remove(width_data_path);

    std::ofstream width_data_output(width_data_path, std::ios::binary | std::ios::app);
    binfile::append_bytes(width_data_output, width_bytes);

    return 0;
}
