/**
 * @file lv_obj_scroll_private.h
 *
 */

#ifndef LV_OBJ_SCROLL_PRIVATE_H
#define LV_OBJ_SCROLL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lvgl_public.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Low level function to scroll by given x and y coordinates.
 * `LV_EVENT_SCROLL` is sent.
 * @param obj       pointer to an object to scroll
 * @param x         pixels to scroll horizontally
 * @param y         pixels to scroll vertically
 * @return          `LV_RESULT_INVALID`: to object was deleted in `LV_EVENT_SCROLL`;
 *                  `LV_RESULT_OK`: if the object is still valid
 */
lv_result_t lv_obj_scroll_by_raw(lv_obj_t * obj, int32_t x, int32_t y);

/**
 * Stop the vertical scroll animation of an object.
 * @param obj       pointer to an object
 */
void lv_obj_stop_scroll_anim_y(const lv_obj_t * obj);

/**
 * Readjust only the vertical scroll position of an object.
 * @param obj       pointer to an object
 * @param anim_en   LV_ANIM_ON/OFF
 */
void lv_obj_readjust_scroll_y(lv_obj_t * obj, lv_anim_enable_t anim_en);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_SCROLL_PRIVATE_H*/
