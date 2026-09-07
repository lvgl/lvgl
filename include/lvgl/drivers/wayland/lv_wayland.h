/**
 * @file lv_wayland.h
 */

#ifndef LV_WAYLAND_H
#define LV_WAYLAND_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../config/lv_conf_internal.h"

#if LV_USE_WAYLAND

#include "lv_wayland_keyboard.h"
#include "lv_wayland_pointer.h"
#include "lv_wayland_touch.h"
#include "lv_wayland_window.h"
#include "lv_wayland_pointer_axis.h"

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
 * Retrieves the file descriptor of the wayland socket
 */
int lv_wayland_get_fd(void);

/**
 * Get the number of physical outputs (screens) advertised by the compositor
 * @return the number of outputs, 0 if the driver isn't initialized yet
 */
uint8_t lv_wayland_get_output_count(void);

/**
 * Get the name of a physical output
 * @param output   index of the output, `0`..`lv_wayland_get_output_count() - 1`
 * @return the connector name, e.g. "HDMI-A-1", or NULL if `output` is out of range.
 *         The string is owned by the driver and stays valid until `lv_wayland_deinit`
 */
const char * lv_wayland_get_output_name(uint8_t output);

/**
 * Get the size of a physical output
 * @param output   index of the output, `0`..`lv_wayland_get_output_count() - 1`
 * @param width    pointer to store the width in pixels @nullable
 * @param height   pointer to store the height in pixels @nullable
 * @return true if the size is known, false if `output` is out of range or the
 *         compositor hasn't reported a size yet
 */
bool lv_wayland_get_output_size(uint8_t output, int32_t * width, int32_t * height);

/**
 * Look up a physical output by name and get its size
 * @param name     connector name of the output, e.g. "DSI-1" or "HDMI-A-1"
 * @param width    pointer to store the width in pixels @nullable
 * @param height   pointer to store the height in pixels @nullable
 * @return the index of the output, to be passed to `lv_wayland_window_set_physical_display`,
 *         or -1 if no output goes by that name
 */
int32_t lv_wayland_get_display_size(const char * name, int32_t * width, int32_t * height);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_WAYLAND */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_WAYLAND_H */
