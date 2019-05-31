#include "lvgl/lvgl.h"

/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --no-compress --no-prefilter --bpp 1 --size 16 --font ./Roboto-Regular.ttf -r 0x20-0x7F --format lvgl -o ./lvgl_sample.c
 ******************************************************************************/

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static const uint8_t gylph_bitmap[] = {
    /* U+20 " " */

    /* U+21 "!" */
    0xd0, 0xff, 0xfc,

    /* U+22 "\"" */
    0x42, 0xde, 0xf0, 0x0,

    /* U+23 "#" */
    0x24, 0x9, 0x2, 0x61, 0xfe, 0x32, 0x4, 0x81,
    0x21, 0xfe, 0x19, 0x2, 0x40, 0x90, 0x0,

    /* U+24 "$" */
    0x10, 0x10, 0x7c, 0xc6, 0xc2, 0x2, 0x6, 0x3c,
    0x70, 0x40, 0xc2, 0x46, 0x7c, 0x18, 0x10, 0x0,

    /* U+25 "%" */
    0x0, 0x0, 0x1c, 0x13, 0x21, 0xa2, 0xb, 0x20,
    0x5c, 0x4, 0x3, 0x20, 0x49, 0x4, 0x90, 0x48,
    0x87, 0x80, 0x0, 0x0,

    /* U+26 "&" */
    0x0, 0xf, 0xe6, 0x31, 0xe, 0x46, 0x9b, 0x23,
    0x80, 0xe0, 0x34, 0x8, 0x82, 0x20, 0xf8, 0x8,
    0x0,

    /* U+27 "'" */
    0x9, 0x30,

    /* U+28 "(" */
    0x0, 0x88, 0xc4, 0x63, 0x18, 0xc6, 0x31, 0x84,
    0x30, 0x86, 0x10,

    /* U+29 ")" */
    0x6, 0x10, 0x43, 0x18, 0x42, 0x10, 0x84, 0x63,
    0x11, 0x98, 0x0,

    /* U+2A "*" */
    0x28, 0x70, 0xe3, 0xe1, 0x0, 0x0,

    /* U+2B "+" */
    0x8, 0x4, 0x2, 0x1, 0x7, 0xf0, 0x40, 0x20,
    0x10, 0x0, 0x0,

    /* U+2C "," */
    0x49, 0x20,

    /* U+2D "-" */
    0xf8, 0x0,

    /* U+2E "." */
    0xc0,

    /* U+2F "/" */
    0xc0, 0x81, 0x3, 0x2, 0x6, 0x4, 0x8, 0x18,
    0x10, 0x20, 0x20, 0x0,

    /* U+30 "0" */
    0x0, 0x1f, 0x8, 0x8c, 0x64, 0x12, 0x9, 0x4,
    0x82, 0x41, 0x31, 0x98, 0xc7, 0xc0, 0x80,

    /* U+31 "1" */
    0x18, 0xc6, 0x31, 0x8c, 0x63, 0x18, 0xde, 0x0,

    /* U+32 "2" */
    0xfe, 0x40, 0x60, 0x30, 0x18, 0xc, 0x4, 0x6,
    0xc6, 0xc6, 0x7c, 0x10,

    /* U+33 "3" */
    0x0, 0x1f, 0x18, 0xcc, 0x60, 0x10, 0x18, 0x78,
    0xc, 0x3, 0x21, 0x98, 0xc7, 0xc0, 0x80,

    /* U+34 "4" */
    0x6, 0x3, 0x1, 0x8f, 0xe6, 0x61, 0x30, 0xd8,
    0x2c, 0xe, 0x7, 0x1, 0x80, 0x0,

    /* U+35 "5" */
    0x0, 0x7c, 0x46, 0xc6, 0x2, 0x6, 0xc6, 0x7c,
    0x40, 0x40, 0x40, 0x7e, 0x0,

    /* U+36 "6" */
    0x0, 0x3c, 0x66, 0xc2, 0xc2, 0xc2, 0xc6, 0xfc,
    0xc0, 0xc0, 0x40, 0x7c, 0x0,

    /* U+37 "7" */
    0x18, 0xc, 0x6, 0x3, 0x0, 0x80, 0x40, 0x30,
    0x8, 0x2, 0x1, 0x9f, 0xc0, 0x0,

    /* U+38 "8" */
    0x0, 0x1f, 0x18, 0xc8, 0x24, 0x13, 0x18, 0xf8,
    0x7c, 0x63, 0x31, 0x98, 0xc7, 0xc0, 0x80,

    /* U+39 "9" */
    0x0, 0x3c, 0x6, 0x3, 0x3, 0x3f, 0x63, 0x43,
    0x43, 0x43, 0x63, 0x3e, 0x0,

    /* U+3A ":" */
    0xc0, 0x3, 0x0,

    /* U+3B ";" */
    0x49, 0x20, 0x0, 0xc, 0x0,

    /* U+3C "<" */
    0x0, 0x4, 0x7b, 0x86, 0x7, 0x3, 0x81,

    /* U+3D "=" */
    0xfe, 0x0, 0x3, 0xef, 0xe0,

    /* U+3E ">" */
    0x1, 0x81, 0xc0, 0xe0, 0xe7, 0x38, 0x40,

    /* U+3F "?" */
    0x10, 0x20, 0x0, 0x81, 0x81, 0x3, 0x3, 0x6,
    0x8d, 0xf0, 0x80,

    /* U+40 "@" */
    0x0, 0x0, 0x73, 0x3, 0x0, 0x8, 0x0, 0x47,
    0x71, 0x33, 0x24, 0xcc, 0x93, 0x32, 0x4c, 0x45,
    0x11, 0x26, 0x6c, 0x88, 0x62, 0x30, 0x18, 0x60,
    0xc0, 0x7c, 0x0, 0x0,

    /* U+41 "A" */
    0x40, 0xd0, 0x26, 0x18, 0xfe, 0x31, 0xc, 0xc1,
    0x20, 0x68, 0x1e, 0x3, 0x0, 0xc0, 0x0,

    /* U+42 "B" */
    0xfe, 0x61, 0xb0, 0x58, 0x2c, 0x17, 0xf3, 0xf9,
    0x86, 0xc3, 0x61, 0xbf, 0x80, 0x0,

    /* U+43 "C" */
    0x0, 0x7, 0xc3, 0x19, 0x82, 0x60, 0x90, 0x4,
    0x1, 0x0, 0x60, 0x18, 0x23, 0x18, 0x7c, 0x4,
    0x0,

    /* U+44 "D" */
    0xfc, 0x63, 0xb0, 0x58, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0x61, 0xbf, 0x80, 0x0,

    /* U+45 "E" */
    0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0,
    0xc0, 0xc0, 0xfe, 0x0,

    /* U+46 "F" */
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc0, 0xc0,
    0xc0, 0xc0, 0xfe, 0x0,

    /* U+47 "G" */
    0x0, 0x7, 0xc3, 0x19, 0x83, 0x60, 0xd8, 0xf6,
    0x39, 0x80, 0x60, 0x18, 0x23, 0x18, 0x7c, 0x4,
    0x0,

    /* U+48 "H" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1f, 0xff, 0x7,
    0x83, 0xc1, 0xe0, 0xf0, 0x60, 0x0,

    /* U+49 "I" */
    0xdb, 0x6d, 0xb6, 0xdb, 0x0,

    /* U+4A "J" */
    0x0, 0x3c, 0x66, 0x42, 0x2, 0x2, 0x2, 0x2,
    0x2, 0x2, 0x2, 0x2, 0x0,

    /* U+4B "K" */
    0xc1, 0xe1, 0xb1, 0x99, 0x8d, 0x87, 0xc3, 0x61,
    0x90, 0xc4, 0x63, 0x30, 0xc0, 0x0,

    /* U+4C "L" */
    0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0x0,

    /* U+4D "M" */
    0xc6, 0x3c, 0x63, 0xce, 0x3c, 0xb3, 0xc9, 0x3d,
    0x9b, 0xd0, 0xbf, 0xf, 0xe0, 0xfe, 0x7, 0xe0,
    0x70, 0x0,

    /* U+4E "N" */
    0xc1, 0xe1, 0xf0, 0xf8, 0xfc, 0x5e, 0x6f, 0x67,
    0xa3, 0xf1, 0xf0, 0xf0, 0x60, 0x0,

    /* U+4F "O" */
    0x0, 0x3, 0xe0, 0xc6, 0x30, 0x66, 0xc, 0x81,
    0x90, 0x32, 0x6, 0x60, 0xcc, 0x18, 0xc6, 0xf,
    0x80, 0x40,

    /* U+50 "P" */
    0xc0, 0x60, 0x30, 0x18, 0xc, 0x7, 0xf3, 0xd,
    0x82, 0xc1, 0x61, 0xbf, 0x80, 0x0,

    /* U+51 "Q" */
    0x0, 0x0, 0x8, 0x7f, 0x18, 0xc6, 0xc, 0xc1,
    0x90, 0x32, 0x6, 0x40, 0xcc, 0x19, 0x83, 0x18,
    0xc1, 0xf0, 0x8, 0x0,

    /* U+52 "R" */
    0xc1, 0x60, 0xb0, 0x58, 0x6c, 0x37, 0xf3, 0xd,
    0x82, 0xc1, 0x61, 0xbf, 0x80, 0x0,

    /* U+53 "S" */
    0x0, 0x7, 0x83, 0x19, 0x86, 0x1, 0x80, 0xe0,
    0xe0, 0xe0, 0x20, 0x18, 0x62, 0x18, 0xfc, 0x0,
    0x0,

    /* U+54 "T" */
    0xc, 0x3, 0x0, 0xc0, 0x30, 0xc, 0x3, 0x0,
    0xc0, 0x30, 0xc, 0x3, 0xf, 0xf8, 0x0,

    /* U+55 "U" */
    0x0, 0x1f, 0x18, 0xd8, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xf0, 0x78, 0x30, 0x0,

    /* U+56 "V" */
    0xc, 0x3, 0x1, 0xe0, 0x68, 0x12, 0xc, 0xc3,
    0x10, 0x86, 0x61, 0x90, 0x2c, 0xc, 0x0,

    /* U+57 "W" */
    0x18, 0x60, 0x61, 0x83, 0x86, 0xe, 0x3c, 0x2c,
    0xf0, 0x92, 0x46, 0x59, 0x19, 0xe6, 0x67, 0x19,
    0xc, 0x64, 0x30, 0x80, 0x0,

    /* U+58 "X" */
    0x60, 0x98, 0x63, 0x30, 0x48, 0x1e, 0x3, 0x0,
    0xc0, 0x78, 0x33, 0xc, 0x46, 0x18, 0x0,

    /* U+59 "Y" */
    0xc, 0x3, 0x0, 0xc0, 0x30, 0xc, 0x3, 0x81,
    0xa0, 0xcc, 0x31, 0x18, 0x66, 0x8, 0x0,

    /* U+5A "Z" */
    0x7f, 0xb0, 0x8, 0x6, 0x1, 0x80, 0x40, 0x30,
    0xc, 0x2, 0x1, 0x9f, 0xe0, 0x0,

    /* U+5B "[" */
    0xe, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xce,

    /* U+5C "\\" */
    0x4, 0x18, 0x30, 0x41, 0x82, 0x4, 0x18, 0x20,
    0xc1, 0x2, 0x0, 0x0,

    /* U+5D "]" */
    0xe, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x6e,

    /* U+5E "^" */
    0x0, 0x99, 0xa1, 0x43, 0x86, 0x0, 0x0,

    /* U+5F "_" */
    0x0, 0xfe,

    /* U+60 "`" */
    0x2, 0x60,

    /* U+61 "a" */
    0x0, 0x3b, 0x67, 0x43, 0x63, 0x3f, 0x3, 0x63,
    0x3e, 0x8,

    /* U+62 "b" */
    0x0, 0xbc, 0xc6, 0xc2, 0xc2, 0xc2, 0xc2, 0xc6,
    0xfc, 0xd8, 0xc0, 0xc0, 0xc0, 0x0,

    /* U+63 "c" */
    0x0, 0x3c, 0x62, 0x62, 0x40, 0x40, 0x40, 0x62,
    0x3e, 0x8,

    /* U+64 "d" */
    0x0, 0x3d, 0x63, 0x63, 0x43, 0x43, 0x63, 0x63,
    0x3f, 0x1b, 0x3, 0x3, 0x3, 0x0,

    /* U+65 "e" */
    0x0, 0x3e, 0x60, 0x60, 0x40, 0x7f, 0x63, 0x62,
    0x3e, 0x8,

    /* U+66 "f" */
    0x20, 0x82, 0x8, 0x20, 0x82, 0x3c, 0x70, 0x82,
    0xe, 0x0,

    /* U+67 "g" */
    0x0, 0x3e, 0x3, 0x3, 0x3f, 0x63, 0x63, 0x43,
    0x43, 0x63, 0x63, 0x3f, 0x18,

    /* U+68 "h" */
    0xc7, 0x8f, 0x1e, 0x3c, 0x78, 0xf1, 0xfe, 0xd9,
    0x83, 0x6, 0x0, 0x0,

    /* U+69 "i" */
    0xff, 0xff, 0x3, 0x0,

    /* U+6A "j" */
    0x6, 0x33, 0x33, 0x33, 0x33, 0x33, 0x0, 0x3,
    0x0,

    /* U+6B "k" */
    0xc6, 0xcc, 0xc8, 0xd8, 0xf0, 0xd8, 0xc8, 0xcc,
    0xc0, 0xc0, 0xc0, 0xc0, 0x0,

    /* U+6C "l" */
    0xff, 0xff, 0xff, 0x0,

    /* U+6D "m" */
    0xc6, 0x3c, 0x63, 0xc6, 0x3c, 0x63, 0xc6, 0x3c,
    0x63, 0xc6, 0x3f, 0xde, 0x18, 0xc0,

    /* U+6E "n" */
    0xc7, 0x8f, 0x1e, 0x3c, 0x78, 0xf1, 0xfe, 0x18,

    /* U+6F "o" */
    0x0, 0x1f, 0x18, 0xcc, 0x64, 0x12, 0x9, 0x4,
    0xc6, 0x3e, 0x4, 0x0,

    /* U+70 "p" */
    0x0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc6, 0xc2, 0xc2,
    0xc2, 0xc2, 0xc6, 0xfc, 0x18,

    /* U+71 "q" */
    0x0, 0x3, 0x3, 0x3, 0x3f, 0x63, 0x63, 0x43,
    0x43, 0x63, 0x63, 0x3f, 0x18,

    /* U+72 "r" */
    0xc6, 0x31, 0x8c, 0x63, 0x1e, 0x10,

    /* U+73 "s" */
    0x0, 0x3c, 0x62, 0x43, 0x6, 0x3c, 0x60, 0x62,
    0x3e, 0x8,

    /* U+74 "t" */
    0x1, 0xc8, 0x42, 0x10, 0x84, 0xf3, 0x88, 0x40,

    /* U+75 "u" */
    0x0, 0xff, 0x1e, 0x3c, 0x78, 0xf1, 0xe3, 0xc6,
    0x0,

    /* U+76 "v" */
    0x18, 0x18, 0x1c, 0x34, 0x24, 0x66, 0x62, 0x42,
    0x0,

    /* U+77 "w" */
    0x10, 0x83, 0x8c, 0x39, 0xc2, 0x94, 0x29, 0x66,
    0xf6, 0x46, 0x24, 0x62, 0x0, 0x0,

    /* U+78 "x" */
    0x42, 0x66, 0x3c, 0x18, 0x18, 0x3c, 0x24, 0x66,
    0x0,

    /* U+79 "y" */
    0x0, 0x70, 0x10, 0x18, 0x18, 0x18, 0x3c, 0x3c,
    0x24, 0x66, 0x66, 0x42, 0x0,

    /* U+7A "z" */
    0x7e, 0x60, 0x30, 0x10, 0x18, 0xc, 0x6, 0x7e,
    0x0,

    /* U+7B "{" */
    0x8, 0x63, 0xc, 0x30, 0xc2, 0x18, 0x60, 0xc3,
    0xc, 0x30, 0xc1, 0x80,

    /* U+7C "|" */
    0x3f, 0xff, 0xff, 0xf0,

    /* U+7D "}" */
    0x3, 0x8, 0x42, 0x10, 0xc3, 0x31, 0x8, 0x42,
    0x33, 0x0,

    /* U+7E "~" */
    0x7, 0x66, 0xdc, 0x20, 0x0
};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_h = 0, .box_w = 0, .ofs_x = 0, .ofs_y = 0}, /* id = 0 reserved */
    {.bitmap_index = 0, .adv_w = 64, .box_h = 0, .box_w = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 67, .box_h = 12, .box_w = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 92, .box_h = 5, .box_w = 5, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 7, .adv_w = 160, .box_h = 12, .box_w = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 22, .adv_w = 149, .box_h = 16, .box_w = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 38, .adv_w = 187, .box_h = 13, .box_w = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 58, .adv_w = 160, .box_h = 13, .box_w = 10, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 75, .adv_w = 56, .box_h = 5, .box_w = 3, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 77, .adv_w = 85, .box_h = 17, .box_w = 5, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 88, .adv_w = 86, .box_h = 17, .box_w = 5, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 99, .adv_w = 111, .box_h = 6, .box_w = 7, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 105, .adv_w = 145, .box_h = 9, .box_w = 9, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 116, .adv_w = 57, .box_h = 4, .box_w = 3, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 118, .adv_w = 115, .box_h = 2, .box_w = 5, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 120, .adv_w = 69, .box_h = 2, .box_w = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 121, .adv_w = 106, .box_h = 13, .box_w = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 133, .adv_w = 144, .box_h = 13, .box_w = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 148, .adv_w = 144, .box_h = 12, .box_w = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 156, .adv_w = 144, .box_h = 12, .box_w = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 168, .adv_w = 144, .box_h = 13, .box_w = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 183, .adv_w = 144, .box_h = 12, .box_w = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 197, .adv_w = 144, .box_h = 13, .box_w = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 210, .adv_w = 144, .box_h = 13, .box_w = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 223, .adv_w = 144, .box_h = 12, .box_w = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 237, .adv_w = 144, .box_h = 13, .box_w = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 252, .adv_w = 144, .box_h = 13, .box_w = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 265, .adv_w = 65, .box_h = 9, .box_w = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 268, .adv_w = 66, .box_h = 11, .box_w = 3, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 273, .adv_w = 130, .box_h = 8, .box_w = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 280, .adv_w = 144, .box_h = 5, .box_w = 7, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 285, .adv_w = 134, .box_h = 8, .box_w = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 292, .adv_w = 122, .box_h = 12, .box_w = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 303, .adv_w = 229, .box_h = 16, .box_w = 14, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 331, .adv_w = 162, .box_h = 12, .box_w = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 346, .adv_w = 162, .box_h = 12, .box_w = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 360, .adv_w = 162, .box_h = 13, .box_w = 10, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 377, .adv_w = 173, .box_h = 12, .box_w = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 391, .adv_w = 140, .box_h = 12, .box_w = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 403, .adv_w = 140, .box_h = 12, .box_w = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 415, .adv_w = 173, .box_h = 13, .box_w = 10, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 432, .adv_w = 180, .box_h = 12, .box_w = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 446, .adv_w = 72, .box_h = 12, .box_w = 3, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 451, .adv_w = 140, .box_h = 13, .box_w = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 464, .adv_w = 162, .box_h = 12, .box_w = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 478, .adv_w = 140, .box_h = 12, .box_w = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 490, .adv_w = 221, .box_h = 12, .box_w = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 508, .adv_w = 180, .box_h = 12, .box_w = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 522, .adv_w = 175, .box_h = 13, .box_w = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 540, .adv_w = 162, .box_h = 12, .box_w = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 554, .adv_w = 178, .box_h = 14, .box_w = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 574, .adv_w = 162, .box_h = 12, .box_w = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 588, .adv_w = 157, .box_h = 13, .box_w = 10, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 605, .adv_w = 153, .box_h = 12, .box_w = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 620, .adv_w = 173, .box_h = 13, .box_w = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 635, .adv_w = 162, .box_h = 12, .box_w = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 650, .adv_w = 220, .box_h = 12, .box_w = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 671, .adv_w = 162, .box_h = 12, .box_w = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 686, .adv_w = 162, .box_h = 12, .box_w = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 701, .adv_w = 153, .box_h = 12, .box_w = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 715, .adv_w = 69, .box_h = 16, .box_w = 4, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 723, .adv_w = 106, .box_h = 13, .box_w = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 735, .adv_w = 69, .box_h = 16, .box_w = 4, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 743, .adv_w = 107, .box_h = 7, .box_w = 7, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 750, .adv_w = 116, .box_h = 2, .box_w = 8, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 752, .adv_w = 80, .box_h = 3, .box_w = 4, .ofs_x = 0, .ofs_y = 9},
    {.bitmap_index = 754, .adv_w = 141, .box_h = 10, .box_w = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 764, .adv_w = 146, .box_h = 14, .box_w = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 778, .adv_w = 134, .box_h = 10, .box_w = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 788, .adv_w = 146, .box_h = 14, .box_w = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 802, .adv_w = 134, .box_h = 10, .box_w = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 812, .adv_w = 78, .box_h = 13, .box_w = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 822, .adv_w = 146, .box_h = 13, .box_w = 8, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 835, .adv_w = 146, .box_h = 13, .box_w = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 847, .adv_w = 65, .box_h = 13, .box_w = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 851, .adv_w = 66, .box_h = 17, .box_w = 4, .ofs_x = -1, .ofs_y = -4},
    {.bitmap_index = 860, .adv_w = 131, .box_h = 13, .box_w = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 873, .adv_w = 65, .box_h = 13, .box_w = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 877, .adv_w = 224, .box_h = 9, .box_w = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 891, .adv_w = 146, .box_h = 9, .box_w = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 899, .adv_w = 146, .box_h = 10, .box_w = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 911, .adv_w = 146, .box_h = 13, .box_w = 8, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 924, .adv_w = 146, .box_h = 13, .box_w = 8, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 937, .adv_w = 90, .box_h = 9, .box_w = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 943, .adv_w = 134, .box_h = 10, .box_w = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 953, .adv_w = 82, .box_h = 12, .box_w = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 961, .adv_w = 146, .box_h = 10, .box_w = 7, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 970, .adv_w = 129, .box_h = 9, .box_w = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 979, .adv_w = 194, .box_h = 9, .box_w = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 993, .adv_w = 129, .box_h = 9, .box_w = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1002, .adv_w = 129, .box_h = 13, .box_w = 8, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1015, .adv_w = 129, .box_h = 9, .box_w = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1024, .adv_w = 87, .box_h = 16, .box_w = 6, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1036, .adv_w = 63, .box_h = 15, .box_w = 2, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1040, .adv_w = 87, .box_h = 16, .box_w = 5, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1050, .adv_w = 174, .box_h = 4, .box_w = 9, .ofs_x = 1, .ofs_y = 3}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY,
        .glyph_id_start = 1, .unicode_list = NULL, .glyph_id_ofs_list = NULL
    }
};

/*-----------------
 *    KERNING
 *----------------*/

/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    9, 43,
    9, 55,
    9, 56,
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -12, 5, 5, 6, 1, -4, 0, 1,
    0, 1, -5, 1, -1, -1, -1, 0,
    -2, -2, 1, -2, -2, -3, -2, 0,
    -2, 0, 1, 2, 1, 0, -3, 1,
    0, 0, -4, 4, 2, 0, -10, 2,
    0, -3, 1, 2, 1, 0, 0, 1,
    -3, 0, -5, -3, 3, 1, 0, -5,
    1, 5, -20, -4, -5, 5, -3, -1,
    0, -3, 1, 3, 1, 0, 0, -5,
    -1, 1, 0, 0, -6, 1, -5, -5,
    -5, -5, -19, -6, -18, -12, -20, 1,
    -2, -3, -3, -2, -3, -2, 0, -8,
    -3, -10, -8, 3, -20, 1, 0, 0,
    1, 0, 1, 1, -1, -1, -1, -1,
    -1, -1, -1, 1, -1, -5, -3, 0,
    1, 1, 1, 2, 2, 2, -1, -5,
    -5, -5, -5, -4, -1, -1, -1, -1,
    -2, -2, -5, -2, -4, -2, -5, -4,
    -7, -6, 1, -6, 1, -38, -38, -15,
    -9, -5, -1, -1, -5, -7, -6, -6,
    0, 1, 0, 0, 0, 0, 0, -1,
    -1, -1, -1, 0, -1, -1, -2, -1,
    -1, -1, -1, -1, 0, -1, -1, -1,
    -1, -1, 0, -1, -1, -1, -7, -7,
    -8, -7, -1, -7, -1, -7, -1, -6,
    -10, -10, 5, -5, -6, -6, -6, -19,
    -6, -23, -14, -22, 0, -4, -11, -14,
    -1, -1, -1, -1, -1, -1, 0, -1,
    -1, 0, -5, -7, -6, -3, -6, -7,
    0, 0, 0, 0, 0, -1, 0, 0,
    1, 0, 1, 0, -1, 1, -2, -42,
    -42, -14, -1, -1, -1, -3, -3, 0,
    -1, -1, -1, -3, 0, -1, 4, 4,
    4, -8, -2, -7, -5, 3, -9, 0,
    0, -1, -1, -2, -1, -5, -2, -5,
    -4, -12, 0, -2, -2, -1, 0, 0,
    0, -1, -1, -1, 0, 0, 0, 0,
    1, 1, -20, -20, -20, -19, -19, -20,
    -6, -7, -7, -7, -4, 4, 4, 4,
    3, 4, -20, -20, -1, -17, -20, -17,
    -19, -17, -12, -12, -15, -6, -2, 0,
    -1, -1, -1, -1, -1, -1, 0, -2,
    -2, 5, -22, -9, -22, -8, -8, -19,
    -5, -5, -6, -5, 4, -12, -11, -12,
    -8, -7, -3, 5, 4, -15, -5, -15,
    -5, -6, -14, -3, -3, -4, -3, 4,
    3, -8, -8, -8, -5, -5, -1, 4,
    -6, -6, -6, -6, -7, -5, -8, 5,
    -23, -13, -23, -11, -11, -21, -7, -7,
    -7, -7, 4, 5, 4, 3, 5, 5,
    -16, -17, -17, -16, -6, -10, -5, 5,
    3, -6, -6, -7, -6, 0, -5, -1,
    -5, -5, -7, -7, -5, -3, -2, -3,
    -3, 4, 4, 5, 5, -6, 5, -5,
    -4, -2, -5, -4, -2, -16, -16, -5,
    -4, -5, 4, 0, -5, -4, 4, 0,
    5, 4, 2, 5, 1, -15, -15, -4,
    -3, -3, -3, -4, -3, -12, -11, -2,
    -2, -3, -2, -5, -5, -5, -5, -5,
    -17, -16, -4, -4, -4, -4, -5, -4,
    -4, -4, -4, -5
};

/*Collect the kern pair's data in one place*/
static lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 484,
    .glyph_ids_size = 1,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

/*Store all the custom data of the font*/
static lv_font_fmt_txt_dsc_t font_dsc = {
    .glyph_bitmap = gylph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .cmap_num = 1,
    .bpp = 1,

    .kern_scale = 16,
    //.kern_dsc = &kern_classes,
    //.kern_classes = 1,
    .kern_dsc = &kern_pairs,
    .kern_classes = 0,
};

/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
lv_font_t lvgl_sample = {
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .line_height = 19,          /*The maximum line height required by the font*/
    .base_line = 4,             /*Baseline measured from the bottom of the line*/
};
