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
#include "lv_draw_rect.h"
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

typedef struct {
    lv_draw_dsc_base_t base;

    /**The text to draw*/
    const char * text;

    /**The size of the text*/
    lv_point_t text_size;

    /**The font to use. Fallback fonts are also handled.*/
    const lv_font_t * font;

    /**Color of the text*/
    lv_color_t color;

    /**Extra space between the lines*/
    int32_t line_space;

    /**Extra space between the characters*/
    int32_t letter_space;

    /**Offset the text with this value horizontally*/
    int32_t ofs_x;

    /**Offset the text with this value vertically*/
    int32_t ofs_y;

    /**Rotation of the letters in 0.1 degree unit*/
    int32_t rotation;

    /**The first characters index for selection (not byte index). `LV_DRAW_LABEL_NO_TXT_SEL` for no selection*/
    uint32_t sel_start;

    /**The last characters's index for selection (not byte index). `LV_DRAW_LABEL_NO_TXT_SEL` for no selection*/
    uint32_t sel_end;

    /**Color of the selected characters*/
    lv_color_t sel_color;

    /**Background color of the selected characters*/
    lv_color_t sel_bg_color;

    /**The number of characters to render. 0: means render until reaching the `\0` termination.*/
    uint32_t text_length;

    /**The alignment of the text `LV_TEXT_ALIGN_LEFT/RIGHT/CENTER`*/
    lv_text_align_t align;

    /**The base direction. Used when type setting Right-to-left (e.g. Arabic) texts*/
    lv_base_dir_t bidi_dir;

    /**Opacity of the text in 0...255 range.
     * LV_OPA_TRANSP, LV_OPA_10, LV_OPA_20, .. LV_OPA_COVER can be used as well*/
    lv_opa_t opa;

    /**Letter outline stroke opacity */
    lv_opa_t outline_stroke_opa;

    /**Text decoration, e.g. underline*/
    lv_text_decor_t decor : 3;

    /**Some flags to control type setting*/
    lv_text_flag_t flag : 5;

    /**1: malloc a buffer and copy `text` there.
     * 0: `text` will be valid during rendering.*/
    uint8_t text_local : 1;

    /**Indicate that the text is constant and its pointer can be safely saved e.g. in a cache.*/
    uint8_t text_static : 1;

    /**1: already executed lv_bidi_process_paragraph.
     * 0: has not been executed lv_bidi_process_paragraph.*/
    uint8_t has_bided : 1;

    /**Pointer to an externally stored struct where some data can be cached to speed up rendering*/
    lv_draw_label_hint_t * hint;

    /* Properties of the letter outlines */
    lv_color_t outline_stroke_color;
    int32_t outline_stroke_width;

} lv_draw_label_dsc_t;

typedef struct {
    lv_draw_dsc_base_t base;

    uint32_t unicode;
    const lv_font_t * font;
    lv_color_t color;

    int32_t rotation;
    int32_t scale_x;
    int32_t scale_y;
    int32_t skew_x;
    int32_t skew_y;
    lv_point_t pivot;

    lv_opa_t opa;
    lv_text_decor_t decor : 3;
    lv_blend_mode_t blend_mode : 4;

    /* Properties of the letter outlines */
    lv_opa_t outline_stroke_opa;
    int32_t outline_stroke_width;
    lv_color_t outline_stroke_color;

} lv_draw_letter_dsc_t;

/**
 * Passed as a parameter to `lv_draw_label_iterate_characters` to
 * draw the characters one by one
 * @param t             pointer to a draw task
 * @param dsc           pointer to `lv_draw_glyph_dsc_t` to describe the character to draw
 *                      if NULL don't draw character
 * @param fill_dsc      pointer to a fill descriptor to draw a background for the character or
 *                      underline or strike through
 *                      if NULL do not fill anything
 * @param fill_area     the area to fill
 *                      if NULL do not fill anything
 */
typedef void(*lv_draw_glyph_cb_t)(lv_draw_task_t * t, lv_draw_glyph_dsc_t * dsc, lv_draw_fill_dsc_t * fill_dsc,
                                  const lv_area_t * fill_area);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_letter_dsc_init(lv_draw_letter_dsc_t * dsc);

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
 * Create a draw task to render a text
 * @param layer         pointer to a layer
 * @param dsc           pointer to draw descriptor
 * @param coords        coordinates of the character
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_label(lv_layer_t * layer, const lv_draw_label_dsc_t * dsc,
                                               const lv_area_t * coords);

/**
 * Create a draw task to render a single character
 * @param layer          pointer to a layer
 * @param dsc            pointer to draw descriptor
 * @param point          position of the label
 * @param unicode_letter the letter to draw
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_character(lv_layer_t * layer, lv_draw_label_dsc_t * dsc,
                                                   const lv_point_t * point, uint32_t unicode_letter);

/**
 * Draw a single letter
 * @param layer          pointer to a layer
 * @param dsc            pointer to draw descriptor
 * @param point          position of the label
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_letter(lv_layer_t * layer, lv_draw_letter_dsc_t * dsc,
                                                const lv_point_t * point);

/**
 * Should be used during rendering the characters to get the position and other
 * parameters of the characters
 * @param t             pointer to a draw task
 * @param dsc           pointer to draw descriptor
 * @param coords        coordinates of the label
 * @param cb            a callback to call to draw each glyphs one by one
 */
void lv_draw_label_iterate_characters(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc,
                                      const lv_area_t * coords, lv_draw_glyph_cb_t cb);

/**
 * @brief Draw a single letter using the provided draw unit, glyph descriptor, position, font, and callback.
 *
 * This function is responsible for rendering a single character from a text string,
 * applying the necessary styling described by the glyph descriptor (`dsc`). It handles
 * the retrieval of the glyph's description, checks its visibility within the clipping area,
 * and invokes the callback (`cb`) to render the glyph at the specified position (`pos`)
 * using the given font (`font`).
 *
 * @param t             Pointer to the drawing task.
 * @param dsc           Pointer to the descriptor containing styling for the glyph to be drawn.
 * @param pos           Pointer to the point coordinates where the letter should be drawn.
 * @param font          Pointer to the font containing the glyph.
 * @param letter        The Unicode code point of the letter to be drawn.
 * @param cb            Callback function to execute the actual rendering of the glyph.
 */
void lv_draw_unit_draw_letter(lv_draw_task_t * t, lv_draw_glyph_dsc_t * dsc,  const lv_point_t * pos,
                              const lv_font_t * font, uint32_t letter, lv_draw_glyph_cb_t cb);

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
