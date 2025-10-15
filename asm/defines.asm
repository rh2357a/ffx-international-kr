.definelabel    get_language_config, 0x1bd3a0
.definelabel get_font_locale_config, 0x20a828

.definelabel                draw_text_ja, 0x1f57d8
.definelabel                draw_text_en, 0x1f3810
.definelabel          get_font_width_ptr, 0x20b960
.definelabel      apply_font_texture_dma, 0x20c528
.definelabel draw_scan_status_background, 0x2199a0
.definelabel         return_lab_00218ec8, 0x218ec8

.definelabel      memcpy, 0x2f0dd4
.definelabel     std_log, 0x2f10f8
.definelabel      strcat, 0x2f21e4
.definelabel      strcpy, 0x2f25e4
.definelabel flush_cache, 0x2fa520

.definelabel font_data_buffer_ptr, 0x66d330
.definelabel     font_data_buffer, 0x67c3a0


MULTIBYTE_BASE      equ 0x24
TEXTURE_GLYPH_COUNT equ 998

MON_NAME_LENGTH equ 24

ALBHED_UNTRANSLATED_COLOR   equ 0x60
ALBHED_UNTRANSLATED_COLOR_2 equ 0x61
ALBHED_TRANSLATED_COLOR     equ 0x56
