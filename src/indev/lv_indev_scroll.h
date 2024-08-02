/**
 * @file lv_indev_scroll.h
 *
 */

#ifndef LV_INDEV_SCROLL_H
#define LV_INDEV_SCROLL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"

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
 * Handle scrolling. Called by LVGL during input device processing
 * @param indev      pointer to an input device
 */
void lv_indev_scroll_handler(lv_indev_t * indev);

/**
 * Handle throwing after scrolling. Called by LVGL during input device processing
 * @param indev      pointer to an input device
 */
void lv_indev_scroll_throw_handler(lv_indev_t * indev);

/**
 * Predict where would a scroll throw end
 * @param indev     pointer to an input device
 * @param dir `     LV_DIR_VER` or `LV_DIR_HOR`
 * @return          the difference compared to the current position when the throw would be finished
 */
int32_t lv_indev_scroll_throw_predict(lv_indev_t * indev, lv_dir_t dir);

/**
 * Get the distance of the nearest snap point
 * @param obj       the object on which snap points should be found
 * @param p         save the distance of the found snap point there
 */
void lv_indev_scroll_get_snap_dist(lv_obj_t * obj, lv_point_t * p);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_INDEV_SCROLL_H*/
