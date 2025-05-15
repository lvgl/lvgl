/**
 * @file lv_wl_pointer_axis.h
 *
 */

#ifndef LV_WL_POINTER_AXIS_H
#define LV_WL_POINTER_AXIS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../indev/lv_indev.h"
#include "../../indev/lv_indev_gesture.h"

#if LV_USE_WAYLAND

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_indev_t * lv_wayland_pointer_axis_create(void);

/**
 * Obtains the input device of the encoder
 * @note It is used to create an input group on application start
 * @param display Reference to the LVGL display associated to the window
 * @return The input device
 */
lv_indev_t * lv_wayland_get_pointeraxis(lv_display_t * display);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WL_POINTER_AXIS_H*/
