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
 * Controls when a change is turned into a new barcode bitmap. Applies to the data too.
 * The colors are always a palette-only write, so they are never affected.
 */
typedef enum {
    LV_BARCODE_UPDATE_MODE_IMMEDIATE = 0,   /**< Re-generate as soon as a property changes (default) */
    LV_BARCODE_UPDATE_MODE_DEFERRED,        /**< Only mark the bitmap out of date and re-generate once, on the next redraw */
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
 * Only rewrites the palette, so it takes effect before or after the data and never
 * regenerates the bars.
 * @param obj pointer to barcode object
 * @param color dark color of the barcode
 */
void lv_barcode_set_dark_color(lv_obj_t * obj, lv_color_t color);

/**
 * Set the light color of a barcode object.
 * Only rewrites the palette, so it takes effect before or after the data and never
 * regenerates the bars.
 * @param obj pointer to barcode object
 * @param color light color of the barcode
 */
void lv_barcode_set_light_color(lv_obj_t * obj, lv_color_t color);

/**
 * Set the scale of a barcode object, i.e. the pixel width of a single bar.
 * The stored data is re-generated, so this may be called before or after the data.
 * @param obj pointer to barcode object
 * @param scale scale factor; must be at least 1
 */
void lv_barcode_set_scale(lv_obj_t * obj, uint16_t scale);

/**
 * Set the direction of a barcode object.
 * The stored data is re-generated, so this may be called before or after the data.
 * @param obj pointer to barcode object
 * @param direction draw direction (`LV_DIR_HOR` or `LV_DIR_VER`)
 */
void lv_barcode_set_direction(lv_obj_t * obj, lv_dir_t direction);

/**
 * Set the tiled mode of a barcode object.
 * The stored data is re-generated, so this may be called before or after the data.
 * @param obj pointer to barcode object
 * @param tiled true: tiled mode, false: normal mode (default)
 */
void lv_barcode_set_tiled(lv_obj_t * obj, bool tiled);

/**
 * Set the encoding of a barcode object.
 * The stored data is re-generated, so this may be called before or after the data.
 * @param obj pointer to barcode object
 * @param encoding encoding (default is `LV_BARCODE_ENCODING_CODE128_GS1`)
 */
void lv_barcode_set_encoding(lv_obj_t * obj, lv_barcode_encoding_t encoding);

/**
 * Set the text a barcode object encodes, and generate the bitmap.
 * A copy is stored, so a later property change or resize can regenerate it; the properties
 * may be set before or after the text, in any order.
 * @note Obeys the update mode. In DEFERRED the canvas is only resized here, so the return
 *       value reports that resize, not the bars.
 * @param obj  pointer to barcode object
 * @param text text to encode, as a non-empty NUL terminated string
 * @return LV_RESULT_OK: if no error; LV_RESULT_INVALID: on error
 */
lv_result_t lv_barcode_set_text(lv_obj_t * obj, const char * text);

/**
 * Get the text a barcode object encodes.
 * @param obj pointer to barcode object
 * @return the stored text, or NULL if none is set. Owned by the barcode object and
 *         invalidated by the next `lv_barcode_set_text()`.
 */
const char * lv_barcode_get_text(lv_obj_t * obj);

/**
 * (Re)generate the barcode bitmap from the stored data, whether or not anything changed.
 * Needs no data argument, which is how deferred changes are applied.
 * @param obj pointer to barcode object
 * @return LV_RESULT_OK: if no error; LV_RESULT_INVALID: on error (e.g. no text set, or
 *         the bars do not fit the current object size)
 */
lv_result_t lv_barcode_render(lv_obj_t * obj);

/**
 * Set when a change is turned into a new barcode bitmap. Applies to the data and to the
 * scale, direction, tiled mode and encoding; the colors are never affected.
 * @note Only the fill is deferred. The canvas is resized in the setter in both modes,
 *       because the draw pass cannot reallocate it.
 * @note Deferred mode expects an explicit `lv_barcode_render()`, which returns the result.
 *       Forgetting it still gives the right bitmap, but the redraw does the work and warns,
 *       and no caller is left to see a failure.
 * @note Switching back to IMMEDIATE while out of date also regenerates, but this returns
 *       void, so a failure is only logged. Render first to get the result.
 * @param obj  pointer to barcode object
 * @param mode the mode to use
 */
void lv_barcode_set_update_mode(lv_obj_t * obj, lv_barcode_update_mode_t mode);

/**
 * Get when a property change is turned into a new barcode bitmap.
 * @param obj pointer to barcode object
 * @return the update mode currently in use
 */
lv_barcode_update_mode_t lv_barcode_get_update_mode(lv_obj_t * obj);

/**
 * Get whether the last attempt to generate the bitmap failed. `lv_barcode_render()`, and
 * `lv_barcode_set_text()` in IMMEDIATE mode, return their result directly; the rest cannot -
 * they run in a void setter, the resize handler, or a deferred fill in the draw pass. Use
 * this for those, e.g. after shrinking the object below the size its data needs.
 * @note A failure is not retried every redraw; only a change makes the Widget try again.
 * @param obj pointer to barcode object
 * @return true: the last generation attempt failed, or no text has been set yet;
 *         false: the bitmap holds a valid barcode
 */
bool lv_barcode_get_render_failed(lv_obj_t * obj);

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
