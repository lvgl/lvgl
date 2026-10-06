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
 * Controls when a property change is turned into a new barcode bitmap. Applies to the
 * data too. The colors are always a palette-only write, so they are never affected.
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
 * Set the length of a barcode along its bars, in pixels.
 * This is the width for `LV_DIR_HOR` and the height for `LV_DIR_VER`, and it becomes the
 * Widget's content size in that direction.
 * The thickness, the other direction, is not set here. It defaults to `LV_DPI_DEF`, and
 * the Widget itself carries it: use `lv_obj_set_height()` for `LV_DIR_HOR` or
 * `lv_obj_set_width()` for `LV_DIR_VER`.
 * @param obj  pointer to barcode object
 * @param size length along the bars in pixels; must be at least 1
 */
void lv_barcode_set_size(lv_obj_t * obj, int32_t size);

/**
 * Set the pixel width of a single bar.
 * @param obj   pointer to barcode object
 * @param scale bar width in pixels, or 0 to use the widest bar that fits the size set by
 *              `lv_barcode_set_size()` (the default)
 */
void lv_barcode_set_scale(lv_obj_t * obj, uint16_t scale);

/**
 * Set the direction of a barcode object, i.e. which way the bars run.
 * @param obj       pointer to barcode object
 * @param direction `LV_DIR_HOR` (the default) or `LV_DIR_VER`
 */
void lv_barcode_set_direction(lv_obj_t * obj, lv_dir_t direction);

/**
 * Set the encoding of a barcode object.
 * @param obj      pointer to barcode object
 * @param encoding encoding (default is `LV_BARCODE_ENCODING_CODE128_GS1`)
 */
void lv_barcode_set_encoding(lv_obj_t * obj, lv_barcode_encoding_t encoding);

/**
 * Set the dark color of a barcode object.
 * Only rewrites the palette, so it never regenerates the bars.
 * @param obj   pointer to barcode object
 * @param color dark color of the barcode
 */
void lv_barcode_set_dark_color(lv_obj_t * obj, lv_color_t color);

/**
 * Set the light color of a barcode object.
 * Only rewrites the palette, so it never regenerates the bars.
 * @param obj   pointer to barcode object
 * @param color light color of the barcode
 */
void lv_barcode_set_light_color(lv_obj_t * obj, lv_color_t color);

/**
 * Set the data of a barcode object and generate the bitmap.
 * A copy of the data is stored, so a later property change can regenerate it. The
 * properties may therefore be set before or after the data, in any order.
 * Use `lv_barcode_render()` to regenerate the stored data without passing it again.
 * @param obj  pointer to barcode object
 * @param data data to encode as a NUL terminated string
 * @return LV_RESULT_OK: if no error; LV_RESULT_INVALID: on error
 */
lv_result_t lv_barcode_set_data(lv_obj_t * obj, const char * data);

/**
 * (Re)generate the barcode bitmap from the data that is already stored.
 * Unlike `lv_barcode_set_data()` this needs no data, so it is the way to apply property
 * changes made in LV_BARCODE_UPDATE_MODE_DEFERRED: set the properties, then call this once
 * to generate them and get the result.
 * The bitmap is regenerated whether or not anything changed.
 * @param obj pointer to barcode object
 * @return LV_RESULT_OK: if no error; LV_RESULT_INVALID: on error (e.g. no data set, or the
 *         bars do not fit the current size)
 */
lv_result_t lv_barcode_render(lv_obj_t * obj);

/**
 * Set when a property change is turned into a new barcode bitmap.
 * With LV_BARCODE_UPDATE_MODE_IMMEDIATE (the default) a change regenerates the bars right
 * away. With LV_BARCODE_UPDATE_MODE_DEFERRED it only marks the bitmap as out of date and
 * several changes are collapsed into a single regeneration on the next redraw.
 * @note In deferred mode you are expected to call `lv_barcode_render()` yourself once the
 *       properties are set. It generates right away and returns the result, leaving the
 *       next redraw nothing to do. If it is forgotten, the work is done by the redraw
 *       instead: the bitmap is still correct, but it is charged to that refresh and its
 *       result cannot be reported to anyone, so a warning is logged. Prefer the explicit
 *       call.
 * @note Switching back to LV_BARCODE_UPDATE_MODE_IMMEDIATE while the bitmap is out of date
 *       also regenerates it, but this function returns void, so a failure can only be
 *       logged, not reported. Call `lv_barcode_render()` first and switch the mode
 *       afterwards to get the result.
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
 * Check whether the barcode bitmap is free of a known generation failure. Most generations
 * report their result directly: `lv_barcode_set_data()` and `lv_barcode_render()` return
 * it. The ones that cannot are the regenerations triggered by a property change - they
 * happen in a void setter or, in LV_BARCODE_UPDATE_MODE_DEFERRED, in the draw pass. Use
 * this to detect those, e.g. after setting a scale the size has no room for.
 * @note A failed generation leaves the bitmap marked as out of date, so it is never
 *       reported as current, and it is not retried on every redraw - only a property
 *       change makes the Widget try again.
 * @param obj pointer to barcode object
 * @return true: no generation attempt is known to have failed. A property change re-arms
 *               the Widget, so this is also true while a deferred regeneration is still
 *               pending;
 *         false: the last generation attempt failed, or no data has been set yet
 */
bool lv_barcode_is_render_valid(lv_obj_t * obj);

/**
 * Get the data of a barcode object.
 * @param obj pointer to barcode object
 * @return the stored NUL terminated data, or NULL if none has been set. It is owned by the
 *         Widget and is freed or replaced by the next `lv_barcode_set_data()`.
 */
const char * lv_barcode_get_data(lv_obj_t * obj);

/**
 * Get the length of a barcode along its bars.
 * @param obj pointer to barcode object
 * @return length along the bars in pixels
 */
int32_t lv_barcode_get_size(lv_obj_t * obj);

/**
 * Get the pixel width of a single bar.
 * @param obj pointer to barcode object
 * @return bar width in pixels, or 0 if it is fitted to the size
 */
uint16_t lv_barcode_get_scale(lv_obj_t * obj);

/**
 * Get the direction of a barcode object.
 * @param obj pointer to barcode object
 * @return `LV_DIR_HOR` or `LV_DIR_VER`
 */
lv_dir_t lv_barcode_get_direction(lv_obj_t * obj);

/**
 * Get the encoding of a barcode object.
 * @param obj pointer to barcode object
 * @return encoding
 */
lv_barcode_encoding_t lv_barcode_get_encoding(lv_obj_t * obj);

/**
 * Get the dark color of a barcode object.
 * @param obj pointer to barcode object
 * @return dark color of the barcode
 */
lv_color_t lv_barcode_get_dark_color(lv_obj_t * obj);

/**
 * Get the light color of a barcode object.
 * @param obj pointer to barcode object
 * @return light color of the barcode
 */
lv_color_t lv_barcode_get_light_color(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BARCODE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BARCODE_H*/
