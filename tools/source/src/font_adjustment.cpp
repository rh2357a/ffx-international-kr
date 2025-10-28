#include "utils/bitmap_image.hpp"
#include "utils/binfile.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <format>
#include <array>
#include <vector>

// clang-format off

const std::array<rgb_t, 4> FONT_COLORS{
	rgb_t{0, 0, 0},
	rgb_t{112, 112, 112},
	rgb_t{190, 190, 190},
	rgb_t{255, 255, 255},
};

constexpr int PATTERN_WIDTH = 4;
constexpr int PATTERN_HEIGHT = 4;

const std::array<rgb_t, PATTERN_WIDTH * PATTERN_HEIGHT> REPLACE_FROM_PATTERN{
	FONT_COLORS[3], FONT_COLORS[1], FONT_COLORS[0], FONT_COLORS[0],
	FONT_COLORS[3], FONT_COLORS[3], FONT_COLORS[3], FONT_COLORS[2],
	FONT_COLORS[3], FONT_COLORS[2], FONT_COLORS[1], FONT_COLORS[1],
	FONT_COLORS[3], FONT_COLORS[1], FONT_COLORS[0], FONT_COLORS[0],
};

const std::array<rgb_t, PATTERN_WIDTH * PATTERN_HEIGHT> REPLACE_TO_PATTERN{
	FONT_COLORS[3], FONT_COLORS[1], FONT_COLORS[0], FONT_COLORS[0],
	FONT_COLORS[3], FONT_COLORS[3], FONT_COLORS[2], FONT_COLORS[0],
	FONT_COLORS[3], FONT_COLORS[2], FONT_COLORS[1], FONT_COLORS[0],
	FONT_COLORS[3], FONT_COLORS[1], FONT_COLORS[0], FONT_COLORS[0],
};

// clang-format on

int main(int argc, char *argv[])
{
	constexpr uint32_t glyph_count = 2704;

	constexpr auto name = "font_kr.bmp";
	constexpr auto new_name = "font_kr_new.bmp";

	std::string font_bitmap_path(name);
	bitmap_image bmp(font_bitmap_path);

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

		for (int j = 0; j <= 18 - PATTERN_HEIGHT; j++)
		{
			for (int k = 0; k <= 16 - PATTERN_WIDTH; k++)
			{
				bool match = true;
				for (int pj = 0; pj < PATTERN_HEIGHT && match; pj++)
				{
					for (int pk = 0; pk < PATTERN_WIDTH; pk++)
					{
						rgb_t color;
						bmp.get_pixel(col + k + pk, row + j + pj, color);

						if (color != REPLACE_FROM_PATTERN[pj * PATTERN_WIDTH + pk])
						{
							match = false;
							break;
						}
					}
				}

				if (match)
				{
					for (int pj = 0; pj < PATTERN_HEIGHT; pj++)
					{
						for (int pk = 0; pk < PATTERN_WIDTH; pk++)
						{
							bmp.set_pixel(
								col + k + pk,
								row + j + pj,
								REPLACE_TO_PATTERN[pj * PATTERN_WIDTH + pk]);
						}
					}
				}
			}
		}
	}

	if (std::filesystem::exists(new_name))
		std::filesystem::remove(new_name);
	bmp.save_image(new_name);

	return 0;
}
