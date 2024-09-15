/**
 * @file lv_draw_label_private.h
 *
 */

#ifndef LV_DRAW_LABEL_PRIVATE_H
#define LV_DRAW_LABEL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Store some info to speed up drawing of very large texts
 * It takes a lot of time to get the first visible character because
 * all the previous characters needs to be checked to calculate the positions.
 * This structure stores an earlier (e.g. at -1000 px) coordinate and the index of that line.
 * Therefore the calculations can start from here.*/
struct _lv_draw_label_hint_t {
    /** Index of the line at `y` coordinate*/
    int32_t line_start;

    /** Give the `y` coordinate of the first letter at `line start` index. Relative to the label's coordinates*/
    int32_t y;

    /** The 'y1' coordinate of the label when the hint was saved.
     * Used to invalidate the hint if the label has moved too much.*/
    int32_t coord_y;
};

struct _lv_draw_glyph_dsc_t {
    void * glyph_data;  /**< Depends on `format` field, it could be image source or draw buf of bitmap or vector data. */
    lv_font_glyph_format_t format;
    const lv_area_t * letter_coords;
    const lv_area_t * bg_coords;
    const lv_font_glyph_dsc_t * g;
    lv_color_t color;
    lv_opa_t opa;
    lv_draw_buf_t * _draw_buf; /**< a shared draw buf for get_bitmap, do not use it directly, use glyph_data instead */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_LABEL_PRIVATE_H*/
