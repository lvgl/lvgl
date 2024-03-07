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
#include "../misc/lv_text.h"
#include "../misc/lv_color.h"
#include "../misc/lv_style.h"

/*********************
 *      DEFINES
 *********************/
#define LV_DRAW_LABEL_NO_TXT_SEL (0xFFFF)

/**********************
 *      TYPEDEFS
 **********************/

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
    lv_draw_dsc_base_t base;

    const char * text;
    const lv_font_t * font;
    uint32_t sel_start;
    uint32_t sel_end;
    lv_color_t color;
    lv_color_t sel_color;
    lv_color_t sel_bg_color;
    int32_t line_space;
    int32_t letter_space;
    int32_t ofs_x;
    int32_t ofs_y;
    lv_opa_t opa;
    lv_base_dir_t bidi_dir;
    lv_text_align_t align;
    lv_text_flag_t flag;
    lv_text_decor_t decor : 3;
    lv_blend_mode_t blend_mode : 3;
    /**
     * < 1: malloc buffer and copy `text` there.
     * 0: `text` is const and it's pointer will be valid during rendering.*/
    uint8_t text_local : 1;
    lv_draw_label_hint_t * hint;
} lv_draw_label_dsc_t;

typedef struct {
    void * glyph_data;  /*Depends on `format` field, it could be image source or draw buf of bitmap or vector data.*/
    lv_font_glyph_format_t format;
    const lv_area_t * letter_coords;
    const lv_area_t * bg_coords;
    const lv_font_glyph_dsc_t * g;
    lv_color_t color;
    lv_opa_t opa;
    lv_draw_buf_t * _draw_buf; /*a shared draw buf for get_bitmap, do not use it directly, use glyph_data instead*/
} lv_draw_glyph_dsc_t;

/**
 * Passed as a parameter to `lv_draw_label_iterate_characters` to
 * draw the characters one by one
 * @param draw_unit     pointer to a draw unit
 * @param dsc           pointer to `lv_draw_glyph_dsc_t` to describe the character to draw
 *                      if NULL don't draw character
 * @param fill_dsc      pointer to a fill descriptor to draw a background for the character or
 *                      underline or strike through
 *                      if NULL do not fill anything
 * @param fill_area     the area to fill
 *                      if NULL do not fill anything
 */
typedef void(*lv_draw_glyph_cb_t)(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * dsc, lv_draw_fill_dsc_t * fill_dsc,
                                  const lv_area_t * fill_area);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a label draw descriptor
 * @param dsc       pointer to a draw descriptor
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_label_dsc_init(lv_draw_label_dsc_t * dsc);

/**
 * Try to get a label draw descriptor from a draw task.
 * @param task      draw task
 * @return          the task's draw descriptor or NULL if the task is not of type LV_DRAW_TASK_TYPE_LABEL
 */
lv_draw_label_dsc_t * lv_draw_task_get_label_dsc(lv_draw_task_t * task);

/**
 * Initialize a glyph draw descriptor.
 * Used internally.
 * @param dsc       pointer to a draw descriptor
 */
void lv_draw_glyph_dsc_init(lv_draw_glyph_dsc_t * dsc);

/**
 * Crate a draw task to render a text
 * @param layer         pointer to a layer
 * @param dsc           pointer to draw descriptor
 * @param coords        coordinates of the character
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_label(lv_layer_t * layer, const lv_draw_label_dsc_t * dsc,
                                               const lv_area_t * coords);

/**
 * Crate a draw task to render a single character
 * @param layer          pointer to a layer
 * @param dsc            pointer to draw descriptor
 * @param point          position of the label
 * @param unicode_letter the letter to draw
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_character(lv_layer_t * layer, lv_draw_label_dsc_t * dsc,
                                                   const lv_point_t * point, uint32_t unicode_letter);

/**
 * Should be used during rendering the characters to get the position and other
 * parameters of the characters
 * @param draw_unit     pointer to a draw unit
 * @param dsc           pointer to draw descriptor
 * @param coords        coordinates of the label
 * @param cb            a callback to call to draw each glyphs one by one
 */
void lv_draw_label_iterate_characters(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc,
                                      const lv_area_t * coords, lv_draw_glyph_cb_t cb);

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
