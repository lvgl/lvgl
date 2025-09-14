
/**
 * @file lv_egl_adapter_outmod_drm-helpers.h
 *
 */

#ifndef LV_EGL_ADAPTER_OUTMOD_DRM_HELPERS_H
#define LV_EGL_ADAPTER_OUTMOD_DRM_HELPERS_H

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>
#include <stddef.h>
#include <stdint.h>
#include <xf86drmMode.h>
//#include <libudev.h>

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef int (*udev_test_fn_t)(struct udev * udev, struct udev_device * device, const char * syspath);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* Return a malloc'd C string. Caller must free(). */
static char * TEMP_get_default_drm_device_path(void * nativedrm_ptr);

/* Return 1 and set *out_value when found, otherwise 0.
   Ownership: out_value is set by value; no heap ownership issues.
*/
static int drm_get_prop_value(int drm_fd, drmModeObjectProperties * props, const char * name, uint64_t * out_value);

/* Caller returns a dynamically allocated array of uint64_t and the count via out_count.
   On success returns a pointer to the array (caller must free()). On failure returns NULL and *out_count = 0.
*/
static uint64_t * drm_get_format_mods(int drm_fd, uint32_t format, uint32_t crtc_index,
                                      size_t * out_count);

static char * find_drm_device_path(void);

/* Fallback: try drmOpen on known modules; returns fd or -1 on failure. */
static int open_drm_with_module_checking(void);

#ifdef __cplusplus
}
#endif

/**********************
 *      MACROS
 **********************/

#endif /* LV_EGL_ADAPTER_OUTMOD_DRM_HELPERS_H */
