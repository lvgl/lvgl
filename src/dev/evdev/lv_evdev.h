/**
 * @file lv_evdev.h
 *
 */

#ifndef LV_EVDEV_H
#define LV_EVDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../indev/lv_indev.h"

#if LV_USE_EVDEV

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create evdev input device.
 * @param type LV_INDEV_TYPE_POINTER or LV_INDEV_TYPE_KEYPAD
 * @param dev_path device path, e.g., /dev/input/event0
 * @return pointer to input device or NULL if opening failed
 */
lv_indev_t * lv_evdev_create(lv_indev_type_t indev_type, const char * dev_path);

/**
 * Set whether coordinates of pointer device should be swapped. Defaults to
 * false.
 * @param indev evdev input device
 * @param swap_axes whether to swap x and y axes
 */
void lv_evdev_set_swap_axes(lv_indev_t * indev, bool swap_axes);

/**
 * Configure a coordinate transformation for pointer devices. Applied after
 * axis swap, if any. Defaults to apply no transformation.
 * @param indev evdev input device
 * @param min_x pointer coordinate mapped to min x of display
 * @param min_y pointer coordinate mapped to min y of display
 * @param max_x pointer coordinate mapped to max x of display
 * @param max_y pointer coordinate mapped to max y of display
 */
void lv_evdev_set_calibration(lv_indev_t * indev, int min_x, int min_y, int max_x, int max_y);

/**
 * Remove evdev input device.
 * @param indev evdev input device to close and free
 */
void lv_evdev_delete(lv_indev_t * indev);

#endif /*LV_USE_EVDEV*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_EVDEV_H*/
