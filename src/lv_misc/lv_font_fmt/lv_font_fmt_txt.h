/**
 * @file lv_font.h
 *
 */

#ifndef LV_FONT_FMT_TXT_H
#define LV_FONT_FMT_TXT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "../lv_font.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Describe a glyph*/
typedef struct
{
    uint32_t bitmap_index : 20;     /* Start index of the bitmap. A font can be max 1 MB. */
    uint32_t adv_w :12;             /*The glyph needs this space. Draw the next glyph after this width. 8.4 format. */

    uint8_t box_w;                  /*Width of the glyph's bounding box*/
    uint8_t box_h;                  /*Height of the glyph's bounding box*/
    int8_t ofs_x;                   /*x offset of the bounding box*/
    uint8_t ofs_y;                  /*y offset of the bounding box. Measured from the top of the line*/
}lv_font_glyph_dsc_fmt_txt_t;

/* Map codepoints to a `glyph_dsc`s
 * Several formats are supported to optimize memory usage
 * See https://github.com/littlevgl/lv_font_conv/blob/master/doc/font_spec.md
 */
typedef struct {
    /* First Unicode character for this range */
    uint32_t range_start;

    /* Number of Unicode characters related to this range.
     * Last Unicode character = range_start + range_length - 1*/
    uint16_t range_length;

    /* First glyph ID (array index of `glyph_dsc`) for this range */
    uint16_t glyph_id_start;

    /*
    According the specification there are 4 formats:
        https://github.com/littlevgl/lv_font_conv/blob/master/doc/font_spec.md

    For simplicity introduce "relative code point":
        rcp = codepoint - range_start

    and a search function:
        search a "value" in an "array" and returns the index of "value".

    Format 0 tiny
        unicode_list == NULL && glyph_id_ofs_list == NULL
        glyph_id = glyph_id_start + rcp

    Format 0 full
        unicode_list == NULL && glyph_id_ofs_list != NULL
        glyph_id = glyph_id_start + glyph_id_ofs_list[rcp]

    Sparse tiny
        unicode_list != NULL && glyph_id_ofs_list == NULL
        glyph_id = glyph_id_start + search(unicode_list, rcp)

    Sparse full
        unicode_list != NULL && glyph_id_ofs_list != NULL
        glyph_id = glyph_id_start + glyph_id_ofs_list[search(unicode_list, rcp)]
    */

    uint16_t * unicode_list;

    /* if unicode_list == NULL uint8_t *
     * else uint16_t *
     */
    const void * glyph_id_ofs_list;
}lv_font_cmap_fmt_txt_t;

/*A simple mapping of kern values from pairs*/
typedef struct {
    /*To get a kern value of two code points:
       1. Get the `glyph_id_left` and `glyph_id_right` from `lv_font_cmap_built_in_t
       2  for(i = 0; i < pair_cnt; i++)
             if(left_gylph_ids[i] == glyph_id_left &&
                right_gylph_ids[i] == glyph_id_right)
                 return values[i];
     */
    uint16_t * left_gylph_ids;
    uint16_t * right_gylph_ids;
    uint8_t * values;
    uint16_t pair_cnt;
}lv_font_kern_pair_fmt_txt_t;

/*More complex but more optimal class based kern value storage*/
typedef struct {
    /*To get a kern value of two code points:
          1. Get the `glyph_id_left` and `glyph_id_right` from `lv_font_cmap_built_in_t
          2  Get the class of the left and right glyphs as `left_class` and `right_class`
              for(i = 0; i < left_class_num; i++)
                if(left_class_mapping[i] == glyph_id_left)
                    return i;
          3. value = class_pair_values[(left_class-1)*right_class_cnt + (righ_class-1)]
        */

    uint8_t * class_pair_values;    /*left_class_num * right_class_num value*/
    uint8_t * left_class_mapping;   /*Map the glyph_ids to classes: index -> glyph_id -> class_id*/
    uint8_t * right_class_mapping;  /*Map the glyph_ids to classes: index -> glyph_id -> class_id*/
    uint8_t left_class_cnt;
    uint8_t right_class_cnt;
}lv_font_kern_classes_fmt_txt_t;

/*Describe store additional data for fonts */
typedef struct {
    /*The bitmaps os all glyphs*/
    const uint8_t * glyph_bitmap;

    /*Describe the glyphs*/
    const lv_font_glyph_dsc_fmt_txt_t * glyph_dsc;

    /* Map the glyphs to Unicode characters.
     * Array of `lv_font_cmap_fmt_txt_t` variables*/
    const lv_font_cmap_fmt_txt_t * cmaps;

    /* Store kerning values.
     * Can be  `lv_font_kern_pair_fmt_txt_t *  or `lv_font_kern_classes_fmt_txt_t *`
     * depending on `kern_classes`
     */
    const void * kern_dsc;

    /*Scale kern values in 12.4 format*/
    uint16_t kern_scale;

    /*Number of cmap tables*/
    uint16_t cmap_num       :10;

    /*Bit per pixel: 1, 2, 4 or 8*/
    uint16_t bpp            :3;

    /*Type of `kern_dsc`*/
    uint16_t kern_classes   :1;

    /*
     * storage format of the bitmap
     * 0: plain
     * 1: compressed: RLE with XOR pre-filter
     * 2: reserved
     * 3: reserved
     */
    uint16_t bitmap_format  :2;
}lv_font_dsc_fmt_txt_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Used as `get_glyph_bitmap` callback in LittelvGL's native font format if the font is uncompressed.
 * @param font pointer to font
 * @param unicode_letter an unicode letter which bitmap should be get
 * @return pointer to the bitmap or NULL if not found
 */
const uint8_t * lv_font_get_bitmap_format_text_plain(const lv_font_t * font, uint32_t letter);

/**
 * Used as `get_glyph_dsc` callback in LittelvGL's native font format if the font is uncompressed.
 * @param font_p pointer to font
 * @param dsc_out store the result descriptor here
 * @param letter an UNICODE letter code
 * @return true: descriptor is successfully loaded into `dsc_out`.
 *         false: the letter was not found, no data is loaded to `dsc_out`
 */
bool lv_font_get_glyph_dsc_format_text_plain(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter);

/**********************
 *      MACROS
 **********************/
#define LV_FONT_X4_SET(_int, _fract)  ((_int << 4) + _fract)
#define LV_FONT_X4_INT(_num)    (_num >> 4)
#define LV_FONT_X4_FRACT(_num)  (_num & 0xF)

/**********************
 * ADD BUILT IN FONTS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_FONT_FMT_TXT_H*/
