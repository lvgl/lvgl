
/**
 * @file lv_egl_adapter_mode.h
 *
 */

#ifndef LV_EGL_ADAPTER_MODE_PRIVATE_H_
#define LV_EGL_ADAPTER_MODE_PRIVATE_H_

/*********************
 *      INCLUDES
 *********************/

#include <stdbool.h>
#include "glad/include/glad/egl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

enum lv_egl_adapter_mode_error {
    EGL_CONFIG_ID_ERROR                 = 1 << 0,  // 1
    EGL_CONFIG_CAVEAT_ERROR             = 1 << 1,  // 2
    EGL_CONFORMANT_ERROR                = 1 << 2,  // 4
    EGL_COLOR_BUFFER_TYPE_ERROR         = 1 << 3,  // 8
    EGL_BUFFER_SIZE_ERROR               = 1 << 4,  // 16
    EGL_RED_SIZE_ERROR                  = 1 << 5,  // 32
    EGL_GREEN_SIZE_ERROR                = 1 << 6,  // 64
    EGL_BLUE_SIZE_ERROR                 = 1 << 7,  // 128
    EGL_LUMINANCE_SIZE_ERROR            = 1 << 8,  // 256
    EGL_ALPHA_SIZE_ERROR                = 1 << 9,  // 512
    EGL_ALPHA_MASK_SIZE_ERROR           = 1 << 10, // 1024
    EGL_DEPTH_SIZE_ERROR                = 1 << 11, // 2048
    EGL_STENCIL_SIZE_ERROR              = 1 << 12, // 4096
    EGL_BIND_TO_TEXTURE_RGB_ERROR       = 1 << 13, // 8192
    EGL_BIND_TO_TEXTURE_RGBA_ERROR      = 1 << 14, // 16384
    EGL_LEVEL_ERROR                     = 1 << 15, // 32768
    EGL_MAX_PBUFFER_WIDTH_ERROR         = 1 << 16, // 65536
    EGL_MAX_PBUFFER_HEIGHT_ERROR        = 1 << 17, // 131072
    EGL_MAX_PBUFFER_PIXELS_ERROR        = 1 << 18, // 262144
    EGL_MIN_SWAP_INTERVAL_ERROR         = 1 << 19, // 524288
    EGL_MAX_SWAP_INTERVAL_ERROR         = 1 << 20, // 1048576
    EGL_NATIVE_RENDERABLE_ERROR         = 1 << 21, // 2097152
    EGL_NATIVE_VISUAL_TYPE_ERROR        = 1 << 22, // 4194304
    EGL_NATIVE_VISUAL_ID_ERROR          = 1 << 23, // 8388608
    EGL_SURFACE_TYPE_ERROR              = 1 << 24, // 16777216
    EGL_SAMPLE_BUFFERS_ERROR            = 1 << 25, // 33554432
    EGL_SAMPLES_ERROR                   = 1 << 26, // 67108864
    EGL_TRANSPARENT_TYPE_ERROR          = 1 << 27, // 134217728
    EGL_TRANSPARENT_RED_VALUE_ERROR     = 1 << 28, // 268435456
    EGL_TRANSPARENT_GREEN_VALUE_ERROR   = 1 << 29, // 536870912
    EGL_TRANSPARENT_BLUE_VALUE_ERROR    = 1 << 30  // 1073741824
};

struct lv_egl_adapter_mode {
    //public:
    EGLConfig handle_;
    // Color buffer attributes.
    EGLint buffer_size;
    EGLint red_size;
    EGLint green_size;
    EGLint blue_size;
    EGLint luminance_size;
    EGLint alpha_size;
    EGLint alpha_mask_size;
    bool bind_to_texture_rgb;
    bool bind_to_texture_rgba;
    EGLint buffer_type;
    // Base config attributes
    EGLint config_caveat;
    EGLint config_id;
    EGLint conformant;
    // Depth buffer
    EGLint depth_size;
    // Framebuffer level
    EGLint level;
    // Pbuffers
    EGLint max_pbuffer_width;
    EGLint max_pbuffer_height;
    EGLint max_pbuffer_pixels;
    // Swap interval
    EGLint min_swap_interval;
    EGLint max_swap_interval;
    // Native window system attributes.
    EGLint native_visual_id;
    EGLint native_visual_type;
    bool native_renderable;
    // Multisample support
    EGLint sample_buffers;
    EGLint samples;
    // Stencil buffer
    EGLint stencil_size;
    EGLint surface_type;
    // Transparency
    EGLint transparent_type; // Should be RGB or NONE
    EGLint transparent_red_value;
    EGLint transparent_green_value;
    EGLint transparent_blue_value;

};

typedef struct lv_egl_adapter_mode lv_egl_adapter_mode_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_EGL_ADAPTER_MODE_PRIVATE_H_ */