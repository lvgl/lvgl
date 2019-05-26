
#include "../lv_misc/lv_font_fmt/lv_font_fmt_txt.h"

#if LV_USE_FONT_DEJAVU_20 != 0  /*Can be enabled in lv_conf.h*/

/***********************************************************************************
 * DejaVuSans.ttf 20 px Font in U+0020 ( ) .. U+007e (~)  range with 1 bpp
***********************************************************************************/

/*Store the image of the letters (glyph)*/
static const uint8_t gylph_bitmap[] = {
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

static uint16_t kern_left_gylph_ids[] =
{
        1, 2, 3
};

static uint16_t kern_right_gylph_ids[] =
{
        1, 2, 3
};
static uint8_t kern_pair_values[] =
{
        15, 52, 73
};

static lv_font_kern_pair_fmt_txt_t kern_pairs =
{
    .left_gylph_ids = kern_left_gylph_ids,
    .right_gylph_ids = kern_right_gylph_ids,
    .values = kern_pair_values,
    .pair_cnt = 3,
};

static uint8_t kern_left_class_mapping[] =
{
        1, 2, 1, 1, 1, 1, 1, 1
};

static uint8_t kern_right_class_mapping[] =
{
        1, 2, 3, 2, 2, 3, 3, 1
};

static uint8_t kern_class_values =
{
        32, 33, 22, 11, 55, 33, 22, 44
};


static lv_font_kern_classes_fmt_txt_t kern_classes =
{
        .class_pair_values = kern_class_values,
        .left_class_mapping = kern_left_class_mapping,
        .right_class_mapping = kern_right_class_mapping,
        .left_class_cnt = 2,
        .right_class_cnt = 3,
};


static uint16_t uniocde_list_1 = {0x234, 0x295, 0x456};
static uint16_t glyph_id_ofs_list_1 = {232, 545, 1466};

static uint8_t glyph_id_ofs_list_3 = {32, 45, 66};


/*Store the glyph descriptions*/
static const lv_font_cmap_fmt_txt_t cmaps[] = {
        { .range_start = 0x280, .range_length = 124,  .glyph_id_start = 223, .unicode_list = uniocde_list_1,  .glyph_id_ofs_list = glyph_id_ofs_list_1},
        { .range_start = 0x20,  .range_length = 112,  .glyph_id_start = 456, .unicode_list = NULL,            .glyph_id_ofs_list = NULL},
        { .range_start = 0x560, .range_length = 7654, .glyph_id_start = 756, .unicode_list = NULL,            .glyph_id_ofs_list = glyph_id_ofs_list_3},

};

static lv_font_glyph_dsc_fmt_txt_t glyph_dsc[] = {
        {.bitmap_index = 0,     .adv_w = 123,   .box_h = 13, .box_w = 9, .ofs_x = 2, .ofs_y = 5},
        {.bitmap_index = 32,    .adv_w = 234,   .box_h = 12, .box_w = 7, .ofs_x = 1, .ofs_y = 3},
        {.bitmap_index = 55,    .adv_w = 98,    .box_h = 11, .box_w = 8, .ofs_x = 0, .ofs_y = 4},
};

static lv_font_dsc_fmt_txt_t font_dsc = {
        .glyph_bitmap = gylph_bitmap,
        .glyph_dsc = glyph_dsc,
        .cmaps = cmaps,
        .cmap_num = 3,
        .bpp = 4,

        .kern_dsc = kern_classes,
        .kern_classes = 1,
        /*or*/
        .kern_dsc = kern_pairs,
        .kern_classes = 0,


};


lv_font_t lv_font_dejavu_20 = {
    .unicode_first = 0x20,     /*First Unicode letter in this font*/
    .unicode_last = 0x3F45,    /*Last Unicode letter in this font*/
    .dsc = &font_dsc,
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .get_glyph_dsc = lv_font_get_glyph_dsc_format_text_plain,    /*Function pointer to get glyph's data*/
    .line_height = 20,          /*The maximum line height required by the font*/
    .base_line = 9,             /*Baseline measured from the bottom of the line*/
};

#endif /*USE_LV_FONT_DEJAVU_20*/
