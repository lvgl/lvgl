/**
 * @file lv_egl_adapter_interface.h
 */

#ifndef LV_EGL_ADAPTER_INTERFACE_H_
#define LV_EGL_ADAPTER_INTERFACE_H_

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#include "../../../misc/lv_array.h"

#if defined(LV_LINUX_DRM_USE_EGL) || defined(LV_LINUX_WAYLAND_USE_EGL)

/* Negotiate the appropriate output module */
#ifndef __LV_OUTMOD_SETUP_COMPLETE
    #if LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL
        #define __LV_OUTMOD_CLASS_T lv_egl_adapter_outmod_drm_t
        #define __LV_OUTMOD_CLASS_INCLUDE_T "../../display/drm/egl_adapter_outmod/lv_egl_adapter_outmod_drm.h"
        #define __LV_OUTMOD_SETUP_COMPLETE 1
    #elif LV_USE_LINUX_WAYLAND && LV_LINUX_WAYLAND_USE_EGL
        #define __LV_OUTMOD_CLASS_T lv_egl_adapter_outmod_wayland_t
        #define __LV_OUTMOD_CLASS_INCLUDE_T "../../wayland/egl_adapter_outmod/lv_egl_adapter_outmod_wayland.h"
        #define __LV_OUTMOD_SETUP_COMPLETE 1
    #endif
#endif /* __LV_OUTMOD_SETUP_COMPLETE */

/* Only continue if an output module was successfully negotiated */
#ifdef __LV_OUTMOD_SETUP_COMPLETE

#include "private/lv_egl_adapter_typedefs.h"
#include "../egl_adapter_runtime_defaults.h"
#include "../egl_adapter_runtime_defaults_internal.h"

#include "common/opengl_headers.h"
#include "lv_egl_adapter_protected.h"
#include "lv_egl_adapter_config.h"
#include <stdint.h>
#include <stdio.h>

#include __LV_OUTMOD_CLASS_INCLUDE_T

/**********************
 *      TYPEDEFS
 **********************/

typedef struct lv_egl_adapter_fbo lv_egl_adapter_fbo_t;

struct lv_egl_adapter_interface {
    lv_egl_adapter_t * egl_adapter;
    __LV_OUTMOD_CLASS_T * output_module;
    int width;
    int height;
    float refresh_rate;
    unsigned int offscreen_fbo_count;
    int offscreen_fbo_index;
    bool is_sync_supported;
    bool is_window_initialized;
    void * output_internal_data;
    lv_array_t fbos_data;
    lv_array_t fbos_syncs_data;
    lv_array_t * fbos;
    lv_array_t * fbos_syncs;
    GLuint depthTexture;
    GLenum format_color;
    GLenum format_depth;
    bool owns_adapter;
};

#ifdef __cplusplus
extern "C" {
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Create and allocate a canvas instance bound to an lv_egl_adapter context.
 * @param egl_adapter The lv_egl_adapter context this canvas will be associated with.
 * @param width Initial canvas width in pixels.
 * @param height Initial canvas height in pixels.
 * @param refresh_rate Target refresh rate (Hz) for the canvas presentation; may be 0 for default.
 * @return Opaque pointer to the newly created canvas instance, or NULL on failure.
 *
 * The returned pointer should be used with the other lv_egl_adapter_interface_* APIs.
 * The canvas will be configured for the provided size and refresh rate; the caller
 * is responsible for cleaning up the canvas via the appropriate adapter destroy
 * or reset API (if provided).
 */
lv_egl_adapter_interface_t * lv_egl_adapter_interface_create(lv_egl_adapter_t * egl_adapter, int width, int height,
                                                             float refresh_rate);

lv_egl_adapter_interface_t * lv_egl_adapter_interface_auto(void);

/**
 * @brief Initialize resources for an existing canvas instance.
 * @param cnvs_ptr Opaque pointer to the canvas instance.
 * @return true on successful initialization, false on failure.
 *
 * Perform any platform- or driver-specific initialization required before using
 * the canvas (GL/EGL resources, textures, FBOs, etc.). This may be called
 * after lv_egl_adapter_interface_create() or after a reset.
 */
bool lv_egl_adapter_interface_init(void * cnvs_ptr);

/**
 * @brief Reset the canvas to an uninitialized or default state.
 * @param cnvs_ptr Opaque pointer to the canvas instance.
 * @return true if reset succeeded, false on failure.
 *
 * This releases or reinitializes internal resources so the canvas can be
 * reconfigured or safely destroyed. After a successful reset, callers will
 * normally call lv_egl_adapter_interface_init() again before use.
 */
bool lv_egl_adapter_interface_reset(void * cnvs_ptr);

/**
 * @brief Set the canvas visibility state.
 * @param cnvs_ptr Opaque pointer to the canvas instance.
 * @param visible true to make the canvas visible/presented, false to hide it.
 *
 * Visibility may control whether the canvas contents are presented to the
 * display or remain off-screen. Exact behavior depends on the platform.
 */
void lv_egl_adapter_interface_visible(void * cnvs_ptr, bool visible);

/**
 * @brief Clear the canvas contents to a default (transparent/black) color.
 *
 * Clears the current render target or backbuffer associated with the canvas.
 * This does not destroy resources; use lv_egl_adapter_interface_reset() for that.
 */
void lv_egl_adapter_interface_clear();

/**
 * @brief Trigger an update / render pass for the canvas.
 * @param cnvs_ptr Opaque pointer to the canvas instance.
 *
 * This function should be called when the canvas contents need to be drawn
 * and presented according to the adapter's refresh policy. It performs any
 * required rendering and swap/present operations.
 */
void lv_egl_adapter_interface_update(void * cnvs_ptr);

/**
 * @brief Switch the current GL context/bindings to the primary FBO for the canvas.
 * @param cnvs_ptr Opaque pointer to the canvas instance.
 *
 * Ensures subsequent GL operations target the canvas's primary framebuffer
 * object (FBO). Useful when performing direct GL rendering into the canvas
 * outside of the adapter's normal update flow.
 */
void lv_egl_adapter_interface_switch_to_primary_fbo(void * cnvs_ptr);

/**
 * @brief Print diagnostic or runtime information about the canvas to stdout.
 * @param cnvs_ptr Opaque pointer to the canvas instance.
 *
 * This helper prints internal state useful for debugging (sizes, GL resource
 * IDs, refresh rate, visibility, etc.). The exact content is implementation-defined.
 */
void lv_egl_adapter_interface_print_info(void * cnvs_ptr);

/**
 * @brief Get the canvas current width in pixels.
 * @param cnvs_ptr Opaque pointer to the canvas instance.
 * @return Width in pixels, or 0 if cnvs_ptr is invalid.
 */
int lv_egl_adapter_interface_width(void * cnvs_ptr);

/**
 * @brief Get the canvas current height in pixels.
 * @param cnvs_ptr Opaque pointer to the canvas instance.
 * @return Height in pixels, or 0 if cnvs_ptr is invalid.
 */
int lv_egl_adapter_interface_height(void * cnvs_ptr);

/**
 * @brief Read the RGBA color of a pixel from the canvas.
 * @param x X coordinate of the pixel (0..width-1).
 * @param y Y coordinate of the pixel (0..height-1).
 * @param r Pointer to receive red channel (0-255);
 * @param g Pointer to receive green channel (0-255);
 * @param b Pointer to receive blue channel (0-255);
 * @param a Pointer to receive alpha channel (0-255);
 *
 * Reads pixel data from the current or primary framebuffer associated with
 * the canvas. Coordinates outside the canvas area are handled in an
 * implementation-defined manner (typically clamped or ignored).
 */
void lv_egl_adapter_interface_read_pixel(int x, int y, uint8_t * r, uint8_t * g, uint8_t * b, uint8_t * a);

/**
 * @brief Read the depth buffer value at a given pixel location.
 * @param x X coordinate of the pixel (0..width-1).
 * @param y Y coordinate of the pixel (0..height-1).
 * @return Depth value as a float in the normalized range [0.0, 1.0], or -1.0 on error.
 *
 * The returned depth corresponds to the value stored in the canvas's depth
 * buffer at the specified pixel. A negative return value indicates an error
 * (for example, invalid coordinates or unavailable depth buffer).
 */
float lv_egl_adapter_interface_read_depth(int x, int y);

/**
 * @brief Enable or disable off-screen backing for the canvas.
 * @param cnvs_ptr Opaque pointer to the canvas instance.
 * @param offscreen Non-zero to back the canvas with an off-screen surface, zero to use an on-screen surface.
 *
 * When enabled the canvas will render into an off-screen surface (for example
 * an FBO or offscreen EGL surface). The change does not take effect immediately;
 * it will be applied the next time lv_egl_adapter_interface_init() or
 * lv_egl_adapter_interface_reset() is called. */
void lv_egl_adapter_interface_offscreen(void * cnvs_ptr, unsigned int offscreen);

/*
 * Call this destructor when you created the canvas with the auto configuration
 * constructor, and it will also free up it's managed variables
 */
void lv_egl_adapter_interface_destroy(void ** cnvs_ptr);

// only referenced locally / internally
void interface_destroy_internal(void ** cnvs_ptr);
lv_egl_adapter_fbo_t * lv_egl_adapter_fbo_create(GLsizei width, GLsizei height, GLuint color_format,
                                                 GLuint depth_format);
void lv_egl_adapter_fbo_destroy(void ** fbostruct_ptr);

#ifdef __cplusplus
}
#endif

/**********************
 *      MACROS
 **********************/

#endif /* __LV_OUTMOD_SETUP_COMPLETE */
#endif /* LV_LINUX_DRM_USE_EGL || LV_LINUX_WAYLAND_USE_EGL */
#endif /* LV_EGL_ADAPTER_INTERFACE_H_ */
