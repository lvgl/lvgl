/**
 * @file lv_barcode.h
 *
 */

#ifndef LV_BARCODE_H
#define LV_BARCODE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../config/lv_conf_internal.h"
#include "../lv_types.h"
#include "../draw/lv_color.h"
#include "lv_canvas.h"

#if LV_USE_BARCODE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    /**
     * Code 128 with GS1 encoding. Strips `[FCN1]` and spaces.
     */
    LV_BARCODE_ENCODING_CODE128_GS1,
    /**
     * Code 128 with raw encoding.
     */
    LV_BARCODE_ENCODING_CODE128_RAW,
} lv_barcode_encoding_t;

/**
 * When a change generates the barcode bitmap again. Applies to the text too.
 * A color change only writes the palette.
 */
typedef enum {
    LV_BARCODE_UPDATE_MODE_IMMEDIATE = 0,   /**< Generate in the setter (default) */
    LV_BARCODE_UPDATE_MODE_DEFERRED,        /**< Only mark the bitmap as out of date */
} lv_barcode_update_mode_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_barcode_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an empty barcode (an `lv_canvas`) object.
 * @param parent    pointer to a parent widget @nullable. When NULL, the widget
 *                  is created as a screen on the active display.
 * @return pointer to the created barcode object
 */
lv_obj_t * lv_barcode_create(lv_obj_t * parent);

/**
 * Set the dark color of a barcode object.
 * Only writes the palette. It does not generate the bars again.
 * You can call this before or after `lv_barcode_set_text()`.
 * @param obj pointer to barcode object
 * @param color dark color of the barcode
 */
void lv_barcode_set_dark_color(lv_obj_t * obj, lv_color_t color);

/**
 * Set the light color of a barcode object.
 * Only writes the palette. It does not generate the bars again.
 * You can call this before or after `lv_barcode_set_text()`.
 * @param obj pointer to barcode object
 * @param color light color of the barcode
 */
void lv_barcode_set_light_color(lv_obj_t * obj, lv_color_t color);

/**
 * Set the scale of a barcode object, i.e. the pixel width of a single bar.
 * Generates the bars again. You can call this before or after `lv_barcode_set_text()`.
 * @param obj pointer to barcode object
 * @param scale scale factor; must be at least 1
 */
void lv_barcode_set_scale(lv_obj_t * obj, uint16_t scale);

/**
 * Set the direction of a barcode object.
 * Generates the bars again. You can call this before or after `lv_barcode_set_text()`.
 * @param obj pointer to barcode object
 * @param direction draw direction (`LV_DIR_HOR` or `LV_DIR_VER`)
 */
void lv_barcode_set_direction(lv_obj_t * obj, lv_dir_t direction);

/**
 * Set the tiled mode of a barcode object.
 * Generates the bars again. You can call this before or after `lv_barcode_set_text()`.
 * @param obj pointer to barcode object
 * @param tiled true: tiled mode, false: normal mode (default)
 */
void lv_barcode_set_tiled(lv_obj_t * obj, bool tiled);

/**
 * Set the encoding of a barcode object.
 * Generates the bars again. You can call this before or after `lv_barcode_set_text()`.
 * @param obj pointer to barcode object
 * @param encoding encoding (default is `LV_BARCODE_ENCODING_CODE128_GS1`)
 */
void lv_barcode_set_encoding(lv_obj_t * obj, lv_barcode_encoding_t encoding);

/**
 * Set the text to encode and generate the bitmap.
 * The Widget keeps a copy, so a property change or a resize can generate the bars again.
 * You can set the properties before or after the text, in any order.
 * @note Obeys the update mode. In deferred mode, this function only resizes the canvas.
 *       The return value is then the result of the resize.
 * @param obj  pointer to barcode object
 * @param text text to encode, as a non-empty NUL terminated string
 * @return LV_RESULT_OK: if no error; LV_RESULT_INVALID: on error
 */
lv_result_t lv_barcode_set_text(lv_obj_t * obj, const char * text);

/**
 * Get the text set with `lv_barcode_set_text()`.
 * @param obj pointer to barcode object
 * @return the text, or NULL if no text is set. The barcode object owns the string.
 *         The next `lv_barcode_set_text()` makes the pointer invalid.
 */
const char * lv_barcode_get_text(lv_obj_t * obj);

/**
 * Generate the bitmap from the stored text. The bitmap is always generated again.
 * In deferred mode, call this after you set the properties.
 * @param obj pointer to barcode object
 * @return LV_RESULT_OK: if no error; LV_RESULT_INVALID: on error (e.g. no text set, or
 *         the bars do not fit the current object size)
 */
lv_result_t lv_barcode_render(lv_obj_t * obj);

/**
 * Set when a change generates the barcode bitmap again. Applies to the text, the scale,
 * the direction, the tiled mode and the encoding. A color change only writes the palette.
 * LV_BARCODE_UPDATE_MODE_IMMEDIATE (the default) generates the bitmap in the setter.
 * LV_BARCODE_UPDATE_MODE_DEFERRED only marks the bitmap as out of date.
 * @note Only the fill of the bars is deferred. The setter resizes the canvas in both
 *       modes, because the redraw cannot reallocate it.
 * @note In deferred mode, call `lv_barcode_render()` after you set the properties.
 *       If you do not, the next redraw generates the bitmap and logs a warning. A failure
 *       in the redraw is only logged.
 * @note A switch to LV_BARCODE_UPDATE_MODE_IMMEDIATE generates a bitmap that is out of
 *       date and logs a warning. This function cannot return the result. To get it, call
 *       `lv_barcode_render()` before you switch the mode.
 * @param obj  pointer to barcode object
 * @param mode the mode to use
 */
void lv_barcode_set_update_mode(lv_obj_t * obj, lv_barcode_update_mode_t mode);

/**
 * Get when a change generates the barcode bitmap again.
 * @param obj pointer to barcode object
 * @return the update mode currently in use
 */
lv_barcode_update_mode_t lv_barcode_get_update_mode(lv_obj_t * obj);

/**
 * Check if the last generation of the bitmap failed. Use this after a property change,
 * after a resize, or after a redraw in LV_BARCODE_UPDATE_MODE_DEFERRED. These generations
 * cannot return a result. `lv_barcode_render()` returns its result, and so does
 * `lv_barcode_set_text()` in LV_BARCODE_UPDATE_MODE_IMMEDIATE.
 * @note The Widget does not retry a failed generation on each redraw. The next change of
 *       the text, the scale, the direction, the tiled mode or the encoding, or a resize,
 *       starts a new attempt.
 * @param obj pointer to barcode object
 * @return true: no generation failed since the last change of the text or a property,
 *               or the last resize.
 *               This is also true while a deferred generation is pending;
 *         false: the last generation failed, or no text is set
 */
bool lv_barcode_is_render_valid(lv_obj_t * obj);

/**
 * Get the dark color of a barcode object
 * @param obj pointer to barcode object
 * @return dark color of the barcode
 */
lv_color_t lv_barcode_get_dark_color(lv_obj_t * obj);

/**
 * Get the light color of a barcode object
 * @param obj pointer to barcode object
 * @return light color of the barcode
 */
lv_color_t lv_barcode_get_light_color(lv_obj_t * obj);

/**
 * Get the scale of a barcode object
 * @param obj pointer to barcode object
 * @return scale factor
 */
uint16_t lv_barcode_get_scale(lv_obj_t * obj);

/**
 * Get the encoding of a barcode object
 * @param obj pointer to barcode object
 * @return encoding
 */
lv_barcode_encoding_t lv_barcode_get_encoding(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BARCODE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BARCODE_H*/
