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
#include <xf86drmMode.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef drmModeModeInfo lv_linux_drm_mode_t;

/**
 * Callback function type for selecting a DRM display mode
 * @param disp pointer to the display object
 * @param modes array of available DRM modes
 * @param mode_count number of modes in the array
 * @return index of the selected mode from the modes array
 */
typedef size_t (*lv_linux_drm_select_mode_cb_t)(lv_display_t * disp,
                                                const lv_linux_drm_mode_t * modes,
                                                size_t mode_count);

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

/**
 * Set a callback function for custom DRM mode selection to override the default mode selection behavior
 *
 * The default mode selection behavior is selecting the native mode
 *
 * @param disp pointer to the display object
 * @param callback function to be called when a display mode needs to be selected,
 *                 or NULL to use the default mode selection behavior
 */
void lv_linux_drm_set_mode_cb(lv_display_t * disp, lv_linux_drm_select_mode_cb_t callback);

/**
 * Get the horizontal resolution of a DRM mode
 * @param mode pointer to the DRM mode object
 * @return horizontal resolution in pixels, or 0 if mode is invalid
 */
int32_t lv_linux_drm_mode_get_horizontal_resolution(const lv_linux_drm_mode_t * mode);

/**
 * Get the vertical resolution of a DRM mode
 * @param mode pointer to the DRM mode object
 * @return vertical resolution in pixels, or 0 if mode is invalid
 */
int32_t lv_linux_drm_mode_get_vertical_resolution(const lv_linux_drm_mode_t * mode);

/**
 * Get the refresh rate of a DRM mode
 * @param mode pointer to the DRM mode object
 * @return refresh rate in Hz, or 0 if mode is invalid
 */
int32_t lv_linux_drm_mode_get_refresh_rate(const lv_linux_drm_mode_t * mode);

/**
 * Check if a DRM mode is the preferred mode for the display
 * @param mode pointer to the DRM mode object
 * @return true if this is the preferred/native mode, false otherwise
 */
bool lv_linux_drm_mode_is_preferred(const lv_linux_drm_mode_t * mode);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_LINUX_DRM */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_LINUX_DRM_H */
