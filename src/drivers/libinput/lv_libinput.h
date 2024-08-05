/**
 * @file lv_libinput.h
 *
 */

#ifndef LV_LIBINPUT_H
#define LV_LIBINPUT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../indev/lv_indev.h"

#if LV_USE_LIBINPUT

#include <poll.h>
#include <pthread.h>

#if LV_LIBINPUT_XKB
#include "lv_xkb.h"
#endif /* LV_LIBINPUT_XKB */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_LIBINPUT_CAPABILITY_NONE     = 0,
    LV_LIBINPUT_CAPABILITY_KEYBOARD = 1U << 0,
    LV_LIBINPUT_CAPABILITY_POINTER  = 1U << 1,
    LV_LIBINPUT_CAPABILITY_TOUCH    = 1U << 2
} lv_libinput_capability;

struct libinput_device;

#define LV_LIBINPUT_MAX_EVENTS 32

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Determine the capabilities of a specific libinput device.
 * @param device the libinput device to query
 * @return the supported input capabilities
 */
lv_libinput_capability lv_libinput_query_capability(struct libinput_device * device);

/**
 * Find connected input device with specific capabilities
 * @param capabilities required device capabilities
 * @param force_rescan erase the device cache (if any) and rescan the file system for available devices
 * @return device node path (e.g. /dev/input/event0) for the first matching device or NULL if no device was found.
 *         The pointer is safe to use until the next forceful device search.
 */
char * lv_libinput_find_dev(lv_libinput_capability capabilities, bool force_rescan);

/**
 * Find connected input devices with specific capabilities
 * @param capabilities required device capabilities
 * @param devices pre-allocated array to store the found device node paths (e.g. /dev/input/event0). The pointers are
 *                safe to use until the next forceful device search.
 * @param count maximum number of devices to find (the devices array should be at least this long)
 * @param force_rescan erase the device cache (if any) and rescan the file system for available devices
 * @return number of devices that were found
 */
size_t lv_libinput_find_devs(lv_libinput_capability capabilities, char ** found, size_t count, bool force_rescan);

/**
 * Create a new libinput input device
 * @param type LV_INDEV_TYPE_POINTER or LV_INDEV_TYPE_KEYPAD
 * @param dev_path device path, e.g. /dev/input/event0
 * @return pointer to input device or NULL if opening failed
 */
lv_indev_t * lv_libinput_create(lv_indev_type_t indev_type, const char * dev_path);

/**
 * Delete a libinput input device
 * @param indev pointer to input device
 */
void lv_libinput_delete(lv_indev_t * indev);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_LIBINPUT */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_LIBINPUT_H */
