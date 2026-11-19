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
 * Set the pixel width of a single bar.
 * @param obj   pointer to barcode object
 * @param scale bar width in pixels, at least 1 (default 1)
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
 * An empty string clears the barcode.
 * @param obj  pointer to barcode object
 * @param data data to encode as a NUL terminated string
 */
void lv_barcode_set_data(lv_obj_t * obj, const char * data);

/**
 * Get the data of a barcode object.
 * @param obj pointer to barcode object
 * @return the stored NUL terminated data, or NULL if none has been set. It is owned by the
 *         Widget and is freed or replaced by the next `lv_barcode_set_data()`.
 */
const char * lv_barcode_get_data(lv_obj_t * obj);

/**
 * Get the pixel width of a single bar.
 * @param obj pointer to barcode object
 * @return bar width in pixels
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
