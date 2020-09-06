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

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_INDEV_SCROLL_H*/
