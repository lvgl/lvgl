
#include "lvgl/lvgl.h"

/***********************************************************************************
 * Size: 20 px
 * Bpp: 1 (or 2/4/8/compressed)
 * Fonts:
 *  DejaVuSans.ttf:     U+0020 ( ) .. U+007e (~) range
 *  FoneAwesome.ttf:    U+1e20 (?) .. U+1f22 (?), U+1f33 (?), U+1f66 (?)
 **********************************************************************************/

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static const uint8_t gylph_bitmap[] = {
    /*Unicode: U+0031 (1) */
    0x38, 0xf8, 0x98, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0xff, 0xff,

    /*Unicode: U+0033 (3) */
    0x7e, 0x00, 0xff, 0x00, 0x83, 0x80, 0x01, 0x80,
    0x03, 0x80, 0x3e, 0x00, 0x3f, 0x00, 0x03, 0x80,
    0x01, 0x80, 0x01, 0x80, 0x83, 0x80, 0xff, 0x00,
    0x7e, 0x00,

    /*Unicode: U+0041 (A) */
    0x06, 0x00, 0x06, 0x00, 0x0f, 0x00, 0x0f, 0x00,
    0x19, 0x80, 0x19, 0x80, 0x19, 0x80, 0x30, 0xc0,
    0x3f, 0xc0, 0x7f, 0xe0, 0x60, 0x60,  0x60, 0x60,
    0xc0, 0x30,

    /*Unicode: U+0061 (a) */
    0x3c, 0x7e, 0x47, 0x03, 0x3f, 0xff, 0xc3, 0xc7,
    0xff, 0x7b,
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static lv_font_glyph_dsc_fmt_txt_t glyph_dsc[] = {
        {.bitmap_index = 0,     .adv_w = 123,   .box_h = 13, .box_w = 9, .ofs_x = 2, .ofs_y = 5},
        {.bitmap_index = 32,    .adv_w = 234,   .box_h = 12, .box_w = 7, .ofs_x = 1, .ofs_y = 3},
        {.bitmap_index = 55,    .adv_w = 98,    .box_h = 11, .box_w = 8, .ofs_x = 0, .ofs_y = 4},
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static uint16_t unicode_list_1 = {0x234, 0x295, 0x456};
static uint16_t glyph_id_ofs_list_1 = {232, 545, 1466};

static uint8_t glyph_id_ofs_list_3 = {32, 45, 66};


/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_cmap_fmt_txt_t cmaps[] = {
        { .range_start = 0x280, .range_length = 124,  .glyph_id_start = 223, .unicode_list = unicode_list_1,  .glyph_id_ofs_list = glyph_id_ofs_list_1},
        { .range_start = 0x20,  .range_length = 112,  .glyph_id_start = 456, .unicode_list = NULL,            .glyph_id_ofs_list = NULL},
        { .range_start = 0x560, .range_length = 7654, .glyph_id_start = 756, .unicode_list = NULL,            .glyph_id_ofs_list = glyph_id_ofs_list_3},

};

/*-----------------
 *    KERNING
 *----------------*/

/*Left glyphs for kering*/
static uint16_t kern_left_gylph_ids[] =
{
    0x21, 0x42, 0x53
};

/*Left glyphs for kering*/
static uint16_t kern_right_gylph_ids[] =
{
    0x41, 0x342, 0x333
};

/*Kerning between the respective left and right glyphs*/
static uint8_t kern_pair_values[] =
{
    15, 52, 73
};

/*Collect the kern pair's data in one place*/
static lv_font_kern_pair_fmt_txt_t kern_pairs =
{
    .left_gylph_ids = kern_left_gylph_ids,
    .right_gylph_ids = kern_right_gylph_ids,
    .values = kern_pair_values,
    .pair_cnt = 3,
};

/***** OR ******/

/*Map glyph_ids to kern left classes*/
static uint8_t kern_left_class_mapping[] =
{
        1, 2, 1, 1, 1, 1, 1, 1
};

/*Map glyph_ids to kern right classes*/
static uint8_t kern_right_class_mapping[] =
{
        1, 2, 3, 2, 2, 3, 3, 1
};

/*Kern values between classes*/
static uint8_t kern_class_values =
{
        32, 33, 22, 11, 55, 33, 22, 44
};


/*Collect the kern class' data in one place*/
static lv_font_kern_classes_fmt_txt_t kern_classes =
{
        .class_pair_values = kern_class_values,
        .left_class_mapping = kern_left_class_mapping,
        .right_class_mapping = kern_right_class_mapping,
        .left_class_cnt = 2,
        .right_class_cnt = 3,
};


/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

/*Store all the custom data of the font*/
static lv_font_dsc_fmt_txt_t font_dsc = {
        .glyph_bitmap = gylph_bitmap,
        .glyph_dsc = glyph_dsc,
        .cmaps = cmaps,
        .cmap_num = 3,
        .bpp = 4,

        .kern_dsc = kern_classes,
        .kern_classes = 1,
        /*** OR ***/
        .kern_dsc = kern_pairs,
        .kern_classes = 0,
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
lv_font_t font_name = {
    .unicode_first = 0x20,     /*First Unicode letter in this font*/
    .unicode_last = 0x3F45,    /*Last Unicode letter in this font*/
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
    .get_glyph_bitmap = lv_font_get_bitmap_format_text_plain,    /*Function pointer to get glyph's bitmap*/
    .get_glyph_dsc = lv_font_get_glyph_dsc_format_text_plain,    /*Function pointer to get glyph's data*/
    .line_height = 20,          /*The maximum line height required by the font*/
    .base_line = 9,             /*Baseline measured from the bottom of the line*/
};
