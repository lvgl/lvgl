
#include "lvgl/lvgl.h"

/***********************************************************************************
 * Copy/Paste the command line instruction
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

static lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] =
{
    {.bitmap_index = 0,     .adv_w = 23,    .box_h = 13, .box_w = 9, .ofs_x = 2, .ofs_y = 5},
    {.bitmap_index = 32,    .adv_w = 44,    .box_h = 12, .box_w = 7, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 55,    .adv_w = 29,    .box_h = 11, .box_w = 8, .ofs_x = 0, .ofs_y = 4},
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static uint16_t unicode_list_1 = {0x234, 0x295, 0x456};
static uint16_t glyph_id_ofs_list_1 = {232, 545, 1466};

static uint8_t glyph_id_ofs_list_3 = {32, 45, 66};


/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 0x280,   .range_length = 124,        .type = LV_FONT_FMT_TXT_CMAP_SPARSE_FULL,
        .glyph_id_start = 223,  .glyph_id_ofs_list = glyph_id_ofs_list_1,    .glyph_id_ofs_list = 23,
        .unicode_list = unicode_list_1,  .unicode_list_length = 8,
    },
    {
        .range_start = 0x20,    .range_length = 112,        .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY,
        .glyph_id_start = 456,  .glyph_id_ofs_list = NULL,  .glyph_id_ofs_list_length = 0,
        .unicode_list = NULL,   .unicode_list_length = 22,
    },
    {
        .range_start = 0x560,   .range_length = 7654,       .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL,
        .glyph_id_start = 756,  .glyph_id_ofs_list = glyph_id_ofs_list_3, .glyph_id_ofs_list_length = 23,
        .unicode_list = NULL,   .unicode_list_length = 0,
    },
};

/*-----------------
 *    KERNING
 *----------------*/

/*Pair left and right glyphs for kerning*/
static lv_font_fmt_txt_kern_pair_id_t kern_pair_gylph_ids[] =
{
    {.pair.left = 0x23,     .pair.right = 0x283},
    {.pair.left = 0x43,     .pair.right = 0x383},
    {.pair.left = 0x73,     .pair.right = 0x583},
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static int8_t kern_pair_values[] =
{
    23, -33, 55
};

/*Collect the kern pair's data in one place*/
static lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_gylph_ids,
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

/* Kern values between classes
 * 4.4 format which needs to scaled with `kern_scale`*/
static int8_t kern_class_values =
{
    44, -66, 24, -23, 56, 23
};

/*Collect the kern class' data in one place*/
static lv_font_fmt_txt_kern_classes_t kern_classes =
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
static lv_font_fmt_txt_dsc_t font_dsc = {
    .glyph_bitmap = gylph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .cmap_num = 3,
    .bpp = 4,
    .bitmap_format = LV_FONT_FMT_TXT_PLAIN,

    .kerning_scale = 324,

    .kern_dsc = kern_classes,
    .kern_classes = 1,
    /*** OR ***/
    .kern_dsc = kern_pairs,
    .kern_classes = 0,
    /*** OR ***/
    .kern_dsc = NULL,
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
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt
    ,    /*Function pointer to get glyph's data*/
    .line_height = 20,          /*The maximum line height required by the font*/
    .base_line = 9,             /*Baseline measured from the bottom of the line*/
};
