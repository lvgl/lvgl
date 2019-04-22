
#include "../lv_misc/lv_font.h"

#if USE_LV_FONT_DEJAVU_20 != 0  /*Can be enabled in lv_conf.h*/

/***********************************************************************************
 * DejaVuSans.ttf 20 px Font in U+0020 ( ) .. U+007e (~)  range with 1 bpp
***********************************************************************************/

/*Store the image of the letters (glyph)*/
static const uint8_t lv_font_dejavu_20_glyph_bitmap[] = {
    /*Unicode: U+0020 ( ) , Width: 6 */

    /*Unicode: U+0031 (1) , Width: 8 */

    /*3 rows*/
    0x38,  //.+@@@...
    0xf8,  //@@@@@...
    0x98,  //@+.@@...
    0x18,  //...@@...
    0x18,  //...@@...
    0x18,  //...@@...
    0x18,  //...@@...
    0x18,  //...@@...
    0x18,  //...@@...
    0x18,  //...@@...
    0x18,  //...@@...
    0xff,  //@@@@@@@@
    0xff,  //@@@@@@@@
    /*4 rows*/


    /*Unicode: U+0033 (3) , Width: 9 */
    /*3 rows*/
    0x7e, 0x00,  //.%@@@@%..
    0xff, 0x00,  //@@@@@@@@+
    0x83, 0x80,  //@+....%@@
    0x01, 0x80,  //.......@@
    0x03, 0x80,  //......%@%
    0x3e, 0x00,  //..@@@@@+.
    0x3f, 0x00,  //..@@@@@%.
    0x03, 0x80,  //.....+@@%
    0x01, 0x80,  //.......@@
    0x01, 0x80,  //.......@@
    0x83, 0x80,  //%+...+@@%
    0xff, 0x00,  //@@@@@@@@.
    0x7e, 0x00,  //.%@@@@%..
    /*4 rows*/


    /*Unicode: U+0041 (A) , Width: 12 */
    /*3 rows*/
    0x06, 0x00,  //.....@@.....
    0x06, 0x00,  //....+@@+....
    0x0f, 0x00,  //....@@@@....
    0x0f, 0x00,  //....@@@@....
    0x19, 0x80,  //...%@++@%...
    0x19, 0x80,  //...@@..@@...
    0x19, 0x80,  //..+@%..%@+..
    0x30, 0xc0,  //..@@....@@..
    0x3f, 0xc0,  //..@@@@@@@@..
    0x7f, 0xe0,  //.%@@@@@@@@%.
    0x60, 0x60,  //.@@......@@.
    0x60, 0x60,  //+@%......%@+
    0xc0, 0x30,  //@@+......+@@
    /*4 rows*/


    /*Unicode: U+0061 (a) , Width: 8 */
    /*6 rows*/
    0x3c,  //.+%@@@+.
    0x7e,  //.@@@@@@.
    0x47,  //.%...%@%
    0x03,  //......@@
    0x3f,  //.+@@@@@@
    0xff,  //%@@@@@@@
    0xc3,  //@@+...@@
    0xc7,  //@@+..%@@
    0xff,  //%@@@@%@@
    0x7b,  //.%@@%.@@
    /*4 rows*/
};

uint32_t advance_w :8;          /*The glyph need this space. Draw the next glyph after this width */
uint32_t advance_w_fract :4;    /*Fractional part of `advance_width` in 1/16 unit*/
uint32_t box_w :8;              /*Width of the glyph's bounding box*/
uint32_t box_h :8;              /*Height of the glyph's bounding box*/
uint32_t ofs_x :8;              /*x offset of the bounding box*/
uint32_t ofs_y :8;              /*y offset of the bounding box*/
uint32_t glyph_index : 20;      /* Start index of the bitmap. A font can be max 1 MB. */

/*Store the glyph descriptions*/
static const lv_font_glyph_dsc_t lv_font_dejavu_20_glyph_dsc[] = {
    {.adv_w = 6,  .adv_w_fract = 0,  .box_w = 6, .box_h = 0, .ofs_x = 0, .ofs_y = 0, .glyph_index = 0},  /*Unicode: U+0020 ( )*/
    {.adv_w = 8,  .adv_w_fract = 0,  .box_w = 8, .box_h = 13,.ofs_x = 0, .ofs_y = 3, .glyph_index = 0},  /*Unicode: U+0031 (1)*/
    {.adv_w = 9,  .adv_w_fract = 0,  .box_w = 9, .box_h = 13,.ofs_x = 0, .ofs_y = 3, .glyph_index = 13}, /*Unicode: U+0033 (3)*/
    {.adv_w = 12, .adv_w_fract = 0,  .box_w = 12,.box_h = 13,.ofs_x = 0, .ofs_y = 3, .glyph_index = 26}, /*Unicode: U+0041 (A)*/
    {.adv_w = 8,  .adv_w_fract = 0,  .box_w = 8, .box_h = 10,.ofs_x = 0, .ofs_y = 6, .glyph_index = 39}, /*Unicode: U+0061 (a)*/
};

static const uint16_t lv_font_dejavu_20_unicode_map[] = {
    32,   /*Unicode: U+0020 ( )*/
    49,   /*Unicode: U+0031 (1)*/
    51,   /*Unicode: U+0033 (3)*/
    65,   /*Unicode: U+0041 (A)*/
    97,   /*Unicode: U+0061 (a)*/
};

lv_font_t lv_font_dejavu_20 = {
    .unicode_first = 32,    /*First Unicode letter in this font*/
    .unicode_last = 126,    /*Last Unicode letter in this font*/
    .h_px = 20,             /*Font height in pixels*/
    .glyph_bitmap = lv_font_dejavu_20_glyph_bitmap, /*Bitmap of glyphs*/
    .glyph_dsc = lv_font_dejavu_20_glyph_dsc,       /*Description of glyphs*/
    .glyph_cnt = 5,            /*Number of glyphs in the font*/
    .unicode_list = NULL,   /*Every character in the font from 'unicode_first' to 'unicode_last'*/
    .get_bitmap = lv_font_get_glyph_bitmap_plain,    /*Function pointer to get glyph's bitmap*/
    .get_dsc = lv_font_get_glyph_dsc_plain,  /*Function pointer to get glyph's width*/
    .bpp = 1,               /*Bit per pixel*/
    .monospace = 0,
    .next_page = NULL,      /*Pointer to a font extension*/
};

#endif /*USE_LV_FONT_DEJAVU_20*/
