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
#include "../../lv_conf_internal.h"
#include "../../misc/lv_types.h"
#include "../../misc/lv_color.h"
#include "../../widgets/canvas/lv_canvas.h"

#if LV_USE_BARCODE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_barcode_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an empty barcode (an `lv_canvas`) object.
 * @param parent point to an object where to create the barcode
 * @return pointer to the created barcode object
 */
lv_obj_t * lv_barcode_create(lv_obj_t * parent);

/**
 * Set the dark color of a barcode object
 * @param obj pointer to barcode object
 * @param color dark color of the barcode
 */
void lv_barcode_set_dark_color(lv_obj_t * obj, lv_color_t color);

/**
 * Set the light color of a barcode object
 * @param obj pointer to barcode object
 * @param color light color of the barcode
 */
void lv_barcode_set_light_color(lv_obj_t * obj, lv_color_t color);

/**
 * Set the scale of a barcode object
 * @param obj pointer to barcode object
 * @param scale scale factor
 */
void lv_barcode_set_scale(lv_obj_t * obj, uint16_t scale);

/**
 * Set the direction of a barcode object
 * @param obj pointer to barcode object
 * @param direction draw direction (`LV_DIR_HOR` or `LB_DIR_VER`)
 */
void lv_barcode_set_direction(lv_obj_t * obj, lv_dir_t direction);

/**
 * Set the tiled mode of a barcode object
 * @param obj pointer to barcode object
 * @param tiled true: tiled mode, false: normal mode (default)
 */
void lv_barcode_set_tiled(lv_obj_t * obj, bool tiled);

/**
 * Set the data of a barcode object
 * @param obj pointer to barcode object
 * @param data data to display
 * @return LV_RESULT_OK: if no error; LV_RESULT_INVALID: on error
 */
lv_result_t lv_barcode_update(lv_obj_t * obj, const char * data);

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

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BARCODE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BARCODE_H*/
