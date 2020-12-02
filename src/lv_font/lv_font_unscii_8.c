#include "../../lvgl.h"

/*******************************************************************************
 * Size: 8 px
 * Bpp: 1
 * Opts:
 ******************************************************************************/

#ifndef LV_FONT_UNSCII_8
    #define LV_FONT_UNSCII_8 1
#endif

#if LV_FONT_UNSCII_8

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t gylph_bitmap[] = {
    /* U+20 " " */
    0x0,

    /* U+21 "!" */
    0xff, 0xcc,

    /* U+22 "\"" */
    0xcf, 0x3c, 0xc0,

    /* U+23 "#" */
    0x6c, 0xdb, 0xfb, 0x6f, 0xed, 0x9b, 0x0,

    /* U+24 "$" */
    0x31, 0xfc, 0x1e, 0xf, 0xe3, 0x0,

    /* U+25 "%" */
    0xc7, 0x98, 0x61, 0x86, 0x78, 0xc0,

    /* U+26 "&" */
    0x38, 0xd8, 0xe3, 0xbd, 0xd9, 0x9d, 0x80,

    /* U+27 "'" */
    0x6f, 0x0,

    /* U+28 "(" */
    0x36, 0xcc, 0xc6, 0x30,

    /* U+29 ")" */
    0xc6, 0x33, 0x36, 0xc0,

    /* U+2A "*" */
    0x66, 0x3c, 0xff, 0x3c, 0x66,

    /* U+2B "+" */
    0x30, 0xcf, 0xcc, 0x30,

    /* U+2C "," */
    0x6f, 0x0,

    /* U+2D "-" */
    0xfc,

    /* U+2E "." */
    0xf0,

    /* U+2F "/" */
    0x3, 0x6, 0xc, 0x18, 0x30, 0x60, 0xc0,

    /* U+30 "0" */
    0x7b, 0x3d, 0xfb, 0xcf, 0x37, 0x80,

    /* U+31 "1" */
    0x31, 0xc3, 0xc, 0x30, 0xcf, 0xc0,

    /* U+32 "2" */
    0x7b, 0x31, 0x8c, 0x63, 0xf, 0xc0,

    /* U+33 "3" */
    0x7b, 0x30, 0xce, 0xf, 0x37, 0x80,

    /* U+34 "4" */
    0x1c, 0x79, 0xb6, 0x6f, 0xe1, 0x83, 0x0,

    /* U+35 "5" */
    0xff, 0xf, 0x83, 0xf, 0x37, 0x80,

    /* U+36 "6" */
    0x39, 0x8c, 0x3e, 0xcf, 0x37, 0x80,

    /* U+37 "7" */
    0xfc, 0x30, 0xc6, 0x30, 0xc3, 0x0,

    /* U+38 "8" */
    0x7b, 0x3c, 0xde, 0xcf, 0x37, 0x80,

    /* U+39 "9" */
    0x7b, 0x3c, 0xdf, 0xc, 0x67, 0x0,

    /* U+3A ":" */
    0xf0, 0xf0,

    /* U+3B ";" */
    0x6c, 0x6, 0xf0,

    /* U+3C "<" */
    0x19, 0x99, 0x86, 0x18, 0x60,

    /* U+3D "=" */
    0xfc, 0xf, 0xc0,

    /* U+3E ">" */
    0xc3, 0xc, 0x33, 0x33, 0x0,

    /* U+3F "?" */
    0x7b, 0x30, 0xc6, 0x30, 0x3, 0x0,

    /* U+40 "@" */
    0x7d, 0x8f, 0x7e, 0xfd, 0xf8, 0x1f, 0x0,

    /* U+41 "A" */
    0x31, 0xec, 0xf3, 0xff, 0x3c, 0xc0,

    /* U+42 "B" */
    0xfb, 0x3c, 0xfe, 0xcf, 0x3f, 0x80,

    /* U+43 "C" */
    0x7b, 0x3c, 0x30, 0xc3, 0x37, 0x80,

    /* U+44 "D" */
    0xf3, 0x6c, 0xf3, 0xcf, 0x6f, 0x0,

    /* U+45 "E" */
    0xff, 0xc, 0x3e, 0xc3, 0xf, 0xc0,

    /* U+46 "F" */
    0xff, 0xc, 0x3e, 0xc3, 0xc, 0x0,

    /* U+47 "G" */
    0x7b, 0x3c, 0x37, 0xcf, 0x37, 0xc0,

    /* U+48 "H" */
    0xcf, 0x3c, 0xff, 0xcf, 0x3c, 0xc0,

    /* U+49 "I" */
    0xfc, 0xc3, 0xc, 0x30, 0xcf, 0xc0,

    /* U+4A "J" */
    0xc, 0x30, 0xc3, 0xf, 0x37, 0x80,

    /* U+4B "K" */
    0xc7, 0x9b, 0x67, 0x8d, 0x99, 0xb1, 0x80,

    /* U+4C "L" */
    0xc3, 0xc, 0x30, 0xc3, 0xf, 0xc0,

    /* U+4D "M" */
    0xc7, 0xdf, 0xfe, 0xbc, 0x78, 0xf1, 0x80,

    /* U+4E "N" */
    0xc7, 0xcf, 0xde, 0xfc, 0xf8, 0xf1, 0x80,

    /* U+4F "O" */
    0x7b, 0x3c, 0xf3, 0xcf, 0x37, 0x80,

    /* U+50 "P" */
    0xfb, 0x3c, 0xfe, 0xc3, 0xc, 0x0,

    /* U+51 "Q" */
    0x7b, 0x3c, 0xf3, 0xcf, 0x66, 0xc0,

    /* U+52 "R" */
    0xfb, 0x3c, 0xfe, 0xdb, 0x3c, 0xc0,

    /* U+53 "S" */
    0x7b, 0x3c, 0x1e, 0xf, 0x37, 0x80,

    /* U+54 "T" */
    0xfc, 0xc3, 0xc, 0x30, 0xc3, 0x0,

    /* U+55 "U" */
    0xcf, 0x3c, 0xf3, 0xcf, 0x37, 0x80,

    /* U+56 "V" */
    0xcf, 0x3c, 0xf3, 0xcd, 0xe3, 0x0,

    /* U+57 "W" */
    0xc7, 0x8f, 0x1e, 0xbf, 0xfd, 0xf1, 0x80,

    /* U+58 "X" */
    0xc3, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0xc3,

    /* U+59 "Y" */
    0xc3, 0x66, 0x3c, 0x18, 0x18, 0x18, 0x18,

    /* U+5A "Z" */
    0xfc, 0x31, 0x8c, 0x63, 0xf, 0xc0,

    /* U+5B "[" */
    0xfc, 0xcc, 0xcc, 0xf0,

    /* U+5C "\\" */
    0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0x3,

    /* U+5D "]" */
    0xf3, 0x33, 0x33, 0xf0,

    /* U+5E "^" */
    0x10, 0x71, 0xb6, 0x30,

    /* U+5F "_" */
    0xff,

    /* U+60 "`" */
    0xc6, 0x30,

    /* U+61 "a" */
    0x78, 0x37, 0xf3, 0x7c,

    /* U+62 "b" */
    0xc3, 0xf, 0xb3, 0xcf, 0x3f, 0x80,

    /* U+63 "c" */
    0x7e, 0x31, 0x87, 0x80,

    /* U+64 "d" */
    0xc, 0x37, 0xf3, 0xcf, 0x37, 0xc0,

    /* U+65 "e" */
    0x7b, 0x3f, 0xf0, 0x78,

    /* U+66 "f" */
    0x3b, 0x3e, 0xc6, 0x31, 0x80,

    /* U+67 "g" */
    0x7f, 0x3c, 0xdf, 0xf, 0xe0,

    /* U+68 "h" */
    0xc3, 0xf, 0xb3, 0xcf, 0x3c, 0xc0,

    /* U+69 "i" */
    0x60, 0x38, 0xc6, 0x31, 0xe0,

    /* U+6A "j" */
    0x18, 0x6, 0x31, 0x8c, 0x7e,

    /* U+6B "k" */
    0xc3, 0xc, 0xf6, 0xf3, 0x6c, 0xc0,

    /* U+6C "l" */
    0xe3, 0x18, 0xc6, 0x31, 0xe0,

    /* U+6D "m" */
    0xcd, 0xff, 0x5e, 0xbc, 0x60,

    /* U+6E "n" */
    0xfb, 0x3c, 0xf3, 0xcc,

    /* U+6F "o" */
    0x7b, 0x3c, 0xf3, 0x78,

    /* U+70 "p" */
    0xfb, 0x3c, 0xfe, 0xc3, 0x0,

    /* U+71 "q" */
    0x7f, 0x3c, 0xdf, 0xc, 0x30,

    /* U+72 "r" */
    0xfb, 0x3c, 0x30, 0xc0,

    /* U+73 "s" */
    0x7f, 0x7, 0x83, 0xf8,

    /* U+74 "t" */
    0x61, 0x8f, 0xd8, 0x61, 0x83, 0xc0,

    /* U+75 "u" */
    0xcf, 0x3c, 0xf3, 0x7c,

    /* U+76 "v" */
    0xcf, 0x3c, 0xde, 0x30,

    /* U+77 "w" */
    0xc7, 0x8f, 0x5b, 0xe6, 0xc0,

    /* U+78 "x" */
    0xc6, 0xd8, 0xe3, 0x6c, 0x60,

    /* U+79 "y" */
    0xcf, 0x3c, 0xdf, 0xd, 0xe0,

    /* U+7A "z" */
    0xfc, 0x63, 0x18, 0xfc,

    /* U+7B "{" */
    0x1c, 0xc3, 0x38, 0x30, 0xc1, 0xc0,

    /* U+7C "|" */
    0xff, 0xfc,

    /* U+7D "}" */
    0xe0, 0xc3, 0x7, 0x30, 0xce, 0x0,

    /* U+7E "~" */
    0x77, 0xb8,

    /* U+7F "" */
    0xc1, 0x42, 0xbd, 0x2c, 0x40, 0x81, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 128, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 8},
    {.bitmap_index = 1, .adv_w = 128, .box_w = 2, .box_h = 7, .ofs_x = 3, .ofs_y = 1},
    {.bitmap_index = 3, .adv_w = 128, .box_w = 6, .box_h = 3, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 6, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 13, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 19, .adv_w = 128, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 25, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 32, .adv_w = 128, .box_w = 3, .box_h = 3, .ofs_x = 2, .ofs_y = 5},
    {.bitmap_index = 34, .adv_w = 128, .box_w = 4, .box_h = 7, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 38, .adv_w = 128, .box_w = 4, .box_h = 7, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 42, .adv_w = 128, .box_w = 8, .box_h = 5, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 47, .adv_w = 128, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 51, .adv_w = 128, .box_w = 3, .box_h = 3, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 53, .adv_w = 128, .box_w = 6, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 54, .adv_w = 128, .box_w = 2, .box_h = 2, .ofs_x = 3, .ofs_y = 1},
    {.bitmap_index = 55, .adv_w = 128, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 62, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 68, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 74, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 80, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 86, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 93, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 99, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 105, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 111, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 117, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 123, .adv_w = 128, .box_w = 2, .box_h = 6, .ofs_x = 3, .ofs_y = 1},
    {.bitmap_index = 125, .adv_w = 128, .box_w = 3, .box_h = 7, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 128, .adv_w = 128, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 133, .adv_w = 128, .box_w = 6, .box_h = 3, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 136, .adv_w = 128, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 141, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 147, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 154, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 160, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 166, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 172, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 178, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 184, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 190, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 196, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 202, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 208, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 214, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 221, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 227, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 234, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 241, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 247, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 253, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 259, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 265, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 271, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 277, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 283, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 289, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 296, .adv_w = 128, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 303, .adv_w = 128, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 310, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 316, .adv_w = 128, .box_w = 4, .box_h = 7, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 320, .adv_w = 128, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 327, .adv_w = 128, .box_w = 4, .box_h = 7, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 331, .adv_w = 128, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 335, .adv_w = 128, .box_w = 8, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 336, .adv_w = 128, .box_w = 4, .box_h = 3, .ofs_x = 3, .ofs_y = 5},
    {.bitmap_index = 338, .adv_w = 128, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 342, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 348, .adv_w = 128, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 352, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 358, .adv_w = 128, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 362, .adv_w = 128, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 367, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 372, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 378, .adv_w = 128, .box_w = 5, .box_h = 7, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 383, .adv_w = 128, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 388, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 394, .adv_w = 128, .box_w = 5, .box_h = 7, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 399, .adv_w = 128, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 404, .adv_w = 128, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 408, .adv_w = 128, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 412, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 417, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 422, .adv_w = 128, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 426, .adv_w = 128, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 430, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 436, .adv_w = 128, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 440, .adv_w = 128, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 444, .adv_w = 128, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 449, .adv_w = 128, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 454, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 459, .adv_w = 128, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 463, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 469, .adv_w = 128, .box_w = 2, .box_h = 7, .ofs_x = 3, .ofs_y = 1},
    {.bitmap_index = 471, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 477, .adv_w = 128, .box_w = 7, .box_h = 2, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 479, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] = {
    {
        .range_start = 32, .range_length = 96, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

/*Store all the custom data of the font*/
static lv_font_fmt_txt_dsc_t font_dsc = {
    .glyph_bitmap = gylph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
lv_font_t lv_font_unscii_8 = {
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 9,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0)
    .underline_position = 0,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if LV_FONT_UNSCII_8*/
