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
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_EVDEV_TYPE_REL,   /**< mice */
    LV_EVDEV_TYPE_ABS,   /**< touch screens, mousepads */
    LV_EVDEV_TYPE_KEY    /**< keyboards, keypads, buttons */
} lv_evdev_type_t;

/**
 * @param indev      the indev created for the newly discovered evdev
 * @param type       the type of the evdev
 * @param user_data  a custom parameter
 */
typedef void (*lv_evdev_discovery_cb_t)(lv_indev_t * indev, lv_evdev_type_t type, void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create evdev input device from a given path.
 * @param type LV_INDEV_TYPE_POINTER or LV_INDEV_TYPE_KEYPAD
 * @param dev_path device path, e.g., /dev/input/event0
 * @return pointer to input device or NULL if opening failed
 */
lv_indev_t * lv_evdev_create(lv_indev_type_t indev_type, const char * dev_path);

/**
 * Create evdev input device, taking ownership of the given file descriptor.
 * @param type LV_INDEV_TYPE_POINTER or LV_INDEV_TYPE_KEYPAD
 * @param fd file descriptor of the evdev device
 * @return pointer to input device or NULL if opening failed
 */
lv_indev_t * lv_evdev_create_fd(lv_indev_type_t indev_type, int fd);

/**
 * Begin automatically creating evdev indevs for all new and existing
 * evdev devices found in /dev/input/
 * @param cb         function to call when a new evdev indev is discovered, or `NULL`
 * @param user_data  parameter to pass to the callback
 * @return           the success or failure status. It will fail if it's
 *                   already running or resources could not be initialized.
 */
lv_result_t lv_evdev_discovery_start(lv_evdev_discovery_cb_t cb, void * user_data);

/**
 * Stop automatically creating evdev indevs. Safe to call from the
 * discovery callback.
 * @return  the success or failure status. It will fail if it's already running.
 */
lv_result_t lv_evdev_discovery_stop(void);

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
