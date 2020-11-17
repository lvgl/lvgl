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
#include "lv_obj.h"

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
 * @param proc pointer to an input device's proc field
 */
void _lv_scroll_handler(lv_indev_proc_t * proc);


/**
 * Handle throwing after scrolling. Called by LVGL during input device processing
 * @param proc pointer to an input device's proc field
 */
void _lv_scroll_throw_handler(lv_indev_proc_t * proc);

/**
 * Predict where would a scroll throw end
 * @param indev pointer to an input device
 * @param dir `LV_DIR_VER` or `LV_DIR_HOR`
 * @return the difference compared to the current position when the throw would be finished
 */
lv_coord_t _lv_scroll_throw_predict(lv_indev_t * indev, lv_dir_t dir);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_INDEV_SCROLL_H*/
