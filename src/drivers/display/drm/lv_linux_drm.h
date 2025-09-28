/**
 * @file lv_linux_drm.h
 *
 */

#ifndef LV_LINUX_DRM_H
#define LV_LINUX_DRM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../display/lv_display.h"

#if LV_USE_LINUX_DRM

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
 * @brief Create a new Linux DRM display
 *
 * Creates and initializes a new LVGL display using the Linux DRM (Direct Rendering Manager)
 * subsystem for hardware-accelerated graphics output.
 *
 * @return Pointer to the created display object, or NULL on failure
 */
lv_display_t * lv_linux_drm_create(void);

/**
 * @brief Configure the DRM device file and connector for a display
 *
 * Sets the DRM device file path and connector ID to use for the specified display.
 * The DRM device file is typically located at /dev/dri/cardN where N is the card number.
 * The connector ID specifies which physical output (HDMI, VGA, etc.) to use.
 *
 * @param disp         Pointer to the display object created with lv_linux_drm_create()
 * @param file         Path to the DRM device file (e.g., "/dev/dri/card0")
 * @param connector_id ID of the DRM connector to use, or -1 to auto-select the first available
 */
void lv_linux_drm_set_file(lv_display_t * disp, const char * file, int64_t connector_id);

/**
 * @brief Automatically find a suitable DRM device path
 *
 * Scans the system for available DRM devices and returns the path to a suitable
 * device file that can be used with lv_linux_drm_set_file().
 *
 * @return Dynamically allocated string containing the device path (must be freed with lv_free()),
 *         or NULL if no suitable device is found
 */
char * lv_linux_drm_find_device_path(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_LINUX_DRM */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_LINUX_DRM_H */
