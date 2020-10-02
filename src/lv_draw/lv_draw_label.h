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
#include "../lv_misc/lv_bidi.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_core/lv_style.h"

/*********************
 *      DEFINES
 *********************/
#define LV_DRAW_LABEL_NO_TXT_SEL (0xFFFF)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_color_t color;
    lv_color_t sel_color;
    lv_color_t sel_bg_color;
    const lv_font_t * font;
    lv_opa_t opa;
    lv_style_int_t line_space;
    lv_style_int_t letter_space;
    uint32_t sel_start;
    uint32_t sel_end;
    lv_coord_t ofs_x;
    lv_coord_t ofs_y;
    lv_bidi_dir_t bidi_dir;
    lv_txt_flag_t flag;
    lv_text_decor_t decor;
    lv_blend_mode_t blend_mode;
} lv_draw_label_dsc_t;

/** Store some info to speed up drawing of very large texts
 * It takes a lot of time to get the first visible character because
 * all the previous characters needs to be checked to calculate the positions.
 * This structure stores an earlier (e.g. at -1000 px) coordinate and the index of that line.
 * Therefore the calculations can start from here.*/
typedef struct {
    /** Index of the line at `y` coordinate*/
    int32_t line_start;

    /** Give the `y` coordinate of the first letter at `line start` index. Relative to the label's coordinates*/
    int32_t y;

    /** The 'y1' coordinate of the label when the hint was saved.
     * Used to invalidate the hint if the label has moved too much. */
    int32_t coord_y;
} lv_draw_label_hint_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

//! @cond Doxygen_Suppress

LV_ATTRIBUTE_FAST_MEM void lv_draw_label_dsc_init(lv_draw_label_dsc_t * dsc);

/**
 * Write a text
 * @param coords coordinates of the label
 * @param mask the label will be drawn only in this area
 * @param dsc pointer to draw descriptor
 * @param txt `\0` terminated text to write
 * @param hint pointer to a `lv_draw_label_hint_t` variable.
 * It is managed by the drawer to speed up the drawing of very long texts (thousands of lines).
 */
LV_ATTRIBUTE_FAST_MEM void lv_draw_label(const lv_area_t * coords, const lv_area_t * mask,
                                         const lv_draw_label_dsc_t * dsc,
                                         const char * txt, lv_draw_label_hint_t * hint);

//! @endcond
/***********************
 * GLOBAL VARIABLES
 ***********************/
extern const uint8_t _lv_bpp2_opa_table[];
extern const uint8_t _lv_bpp3_opa_table[];
extern const uint8_t _lv_bpp1_opa_table[];
extern const uint8_t _lv_bpp4_opa_table[];
extern const uint8_t _lv_bpp8_opa_table[];

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DRAW_LABEL_H*/
