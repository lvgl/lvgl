/**
 * @file lv_draw_label.h
 *
 */

#ifndef LV_DRAW_LABEL_H
#define LV_DRAW_LABEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "../misc/lv_bidi.h"
#include "../misc/lv_txt.h"
#include "../misc/lv_color.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/
#define LV_DRAW_LABEL_NO_TXT_SEL (0xFFFF)

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_draw_ctx_t;
/** Store some info to speed up drawing of very large texts
 * It takes a lot of time to get the first visible character because
 * all the previous characters needs to be checked to calculate the positions.
 * This structure stores an earlier (e.g. at -1000 px) coordinate and the index of that line.
 * Therefore the calculations can start from here.*/
typedef struct _lv_draw_label_hint_t {
    /** Index of the line at `y` coordinate*/
    int32_t line_start;

    /** Give the `y` coordinate of the first letter at `line start` index. Relative to the label's coordinates*/
    int32_t y;

    /** The 'y1' coordinate of the label when the hint was saved.
     * Used to invalidate the hint if the label has moved too much.*/
    int32_t coord_y;
} lv_draw_label_hint_t;


typedef struct {
    const char * text;
    const lv_font_t * font;
    uint32_t sel_start;
    uint32_t sel_end;
    lv_color_t color;
    lv_color_t sel_color;
    lv_color_t sel_bg_color;
    lv_coord_t line_space;
    lv_coord_t letter_space;
    lv_coord_t ofs_x;
    lv_coord_t ofs_y;
    lv_opa_t opa;
    lv_base_dir_t bidi_dir;
    lv_text_align_t align;
    lv_text_flag_t flag;
    lv_text_decor_t decor : 3;
    lv_blend_mode_t blend_mode: 3;
    lv_draw_label_hint_t * hint;
} lv_draw_label_dsc_t;

typedef enum {
    LV_DRAW_LETTER_BITMAP_FORMAT_A8,
    LV_DRAW_LETTER_BITMAP_FORMAT_SUBPX,
    LV_DRAW_LETTER_BITMAP_FORMAT_IMAGE,
} lv_draw_letter_bitmap_format_t;

typedef struct {
    const uint8_t * bitmap;
    lv_draw_letter_bitmap_format_t format;

    const lv_area_t * bg_coords;
    const lv_area_t * letter_coords;
    lv_coord_t line1_y_ofs;
    lv_coord_t line2_y_ofs;
    lv_coord_t line_width;
    lv_text_decor_t decor;
    lv_blend_mode_t blend_mode;

    lv_color_t bg_color;
    lv_color_t letter_color;
    lv_color_t line_color;

    lv_opa_t letter_opa;
    lv_opa_t bg_opa;
    lv_opa_t line_opa;
} lv_draw_letter_dsc_t;


typedef void(*lv_draw_letter_cb_t)(lv_draw_unit_t * draw_unit, lv_draw_letter_dsc_t * dsc);

void lv_draw_label_interate_letters(lv_draw_unit_t * draw_unit, lv_draw_label_dsc_t * dsc, const lv_area_t * coords,
                                    lv_draw_letter_cb_t cb);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_draw_label_dsc_init(lv_draw_label_dsc_t * dsc);

/**
 * Write a text
 * @param coords coordinates of the label
 * @param mask the label will be drawn only in this area
 * @param dsc pointer to draw descriptor
 * @param txt `\0` terminated text to write
 * @param hint pointer to a `lv_draw_label_hint_t` variable.
 * It is managed by the draw to speed up the drawing of very long texts (thousands of lines).
 */
LV_ATTRIBUTE_FAST_MEM void lv_draw_label(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,
                                         const lv_area_t * coords);

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_LABEL_H*/
