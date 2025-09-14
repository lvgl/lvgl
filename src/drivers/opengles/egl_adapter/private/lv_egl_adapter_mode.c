/**
 * @file lv_egl_adapter_mode.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../../lv_conf_internal.h"
#include "lv_egl_adapter_mode.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/*********************
 *      DEFINES
 *********************/

#if  LV_ADAPTED_ON_WAYLAND
    #define GLMARK2_NATIVE_EGL_DISPLAY_ENUM EGL_PLATFORM_WAYLAND_KHR
#elif  LV_ADAPTED_ON_DRM
    #define GLMARK2_NATIVE_EGL_DISPLAY_ENUM EGL_PLATFORM_GBM_KHR
#else
    // Platforms not in the above platform enums (GBM) fall back to eglGetDisplay.
    #define GLMARK2_NATIVE_EGL_DISPLAY_ENUM 0
#endif

#if LV_EGL_ADAPTED_WITH_GLESV2
    #define DEFINED_EGL_OPENGL_BIT EGL_OPENGL_ES2_BIT
#elif LV_EGL_ADAPTED_WITH_GL
    #define DEFINED_EGL_OPENGL_BIT EGL_OPENGL_BIT
#else
    #define DEFINED_EGL_OPENGL_BIT 0
#endif

const int eglAttributes[] = {
    EGL_CONFIG_ID, EGL_CONFIG_CAVEAT, EGL_CONFORMANT, EGL_COLOR_BUFFER_TYPE, EGL_BUFFER_SIZE,
    EGL_RED_SIZE, EGL_GREEN_SIZE, EGL_BLUE_SIZE, EGL_LUMINANCE_SIZE, EGL_ALPHA_SIZE,
    EGL_ALPHA_MASK_SIZE, EGL_DEPTH_SIZE, EGL_STENCIL_SIZE, EGL_BIND_TO_TEXTURE_RGB,
    EGL_BIND_TO_TEXTURE_RGBA, EGL_LEVEL, EGL_MAX_PBUFFER_WIDTH, EGL_MAX_PBUFFER_HEIGHT,
    EGL_MAX_PBUFFER_PIXELS, EGL_MIN_SWAP_INTERVAL, EGL_MAX_SWAP_INTERVAL,
    EGL_NATIVE_RENDERABLE, EGL_NATIVE_VISUAL_TYPE, EGL_NATIVE_VISUAL_ID, EGL_SURFACE_TYPE,
    EGL_SAMPLE_BUFFERS, EGL_SAMPLES, EGL_TRANSPARENT_TYPE, EGL_TRANSPARENT_RED_VALUE,
    EGL_TRANSPARENT_GREEN_VALUE, EGL_TRANSPARENT_BLUE_VALUE
};

const char * eglAttributeNames[] = {
    "EGL_CONFIG_ID", "EGL_CONFIG_CAVEAT", "EGL_CONFORMANT", "EGL_COLOR_BUFFER_TYPE", "EGL_BUFFER_SIZE",
    "EGL_RED_SIZE", "EGL_GREEN_SIZE", "EGL_BLUE_SIZE", "EGL_LUMINANCE_SIZE", "EGL_ALPHA_SIZE",
    "EGL_ALPHA_MASK_SIZE", "EGL_DEPTH_SIZE", "EGL_STENCIL_SIZE", "EGL_BIND_TO_TEXTURE_RGB",
    "EGL_BIND_TO_TEXTURE_RGBA", "EGL_LEVEL", "EGL_MAX_PBUFFER_WIDTH", "EGL_MAX_PBUFFER_HEIGHT",
    "EGL_MAX_PBUFFER_PIXELS", "EGL_MIN_SWAP_INTERVAL", "EGL_MAX_SWAP_INTERVAL",
    "EGL_NATIVE_RENDERABLE", "EGL_NATIVE_VISUAL_TYPE", "EGL_NATIVE_VISUAL_ID", "EGL_SURFACE_TYPE",
    "EGL_SAMPLE_BUFFERS", "EGL_SAMPLES", "EGL_TRANSPARENT_TYPE", "EGL_TRANSPARENT_RED_VALUE",
    "EGL_TRANSPARENT_GREEN_VALUE", "EGL_TRANSPARENT_BLUE_VALUE"
};

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void add_egl_config_error(uint32_t * errorFlags, enum lv_egl_adapter_mode_error error);
static void unpack_egl_config_errors(uint32_t errorFlags);
static void get_egl_config_int(EGLDisplay dpy, EGLConfig handle, int index, uint32_t * errorFlags, EGLint * output);
static void get_egl_config_bool(EGLDisplay dpy, EGLConfig handle, int index, uint32_t * errorFlags, bool * output);
static void get_egl_config_ints(EGLDisplay dpy, EGLConfig handle, int startIndex, int endIndex, uint32_t * errorFlags,
                                ...);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_egl_adapter_mode_zero(void * config_ptr)
{
    lv_egl_adapter_mode_t adapter_mode = (lv_egl_adapter_mode_t)config_ptr;

    adapter_mode->handle_ = 0;//EGLConfig(config); // ??handle_(config)??,
    adapter_mode->buffer_size = 0;
    adapter_mode->red_size = 0;
    adapter_mode->green_size = 0;
    adapter_mode->blue_size = 0;
    adapter_mode->luminance_size = 0;
    adapter_mode->alpha_size = 0;
    adapter_mode->alpha_mask_size = 0;
    adapter_mode->bind_to_texture_rgb = false;
    adapter_mode->bind_to_texture_rgba = false;
    adapter_mode->buffer_type = EGL_RGB_BUFFER;
    adapter_mode->config_caveat = 0;
    adapter_mode->config_id = 0;
    adapter_mode->conformant = 0;
    adapter_mode->depth_size = 0;
    adapter_mode->level = 0;
    adapter_mode->max_pbuffer_width = 0;
    adapter_mode->max_pbuffer_height = 0;
    adapter_mode->max_pbuffer_pixels = 0;
    adapter_mode->min_swap_interval = 0;
    adapter_mode->max_swap_interval = 0;
    adapter_mode->native_visual_id = 0;
    adapter_mode->native_visual_type = 0;
    adapter_mode->native_renderable = false;
    adapter_mode->sample_buffers = 0;
    adapter_mode->samples = 0;
    adapter_mode->stencil_size = 0;
    adapter_mode->surface_type = 0;
    adapter_mode->transparent_type = 0;
    adapter_mode->transparent_red_value = 0;
    adapter_mode->transparent_green_value = 0;
    adapter_mode->transparent_blue_value = 0;
}

void lv_egl_adapter_mode_init(void * config_ptr, EGLDisplay D, EGLConfig config)
{
    lv_egl_adapter_mode_t adapter_mode = (lv_egl_adapter_mode_t)config_ptr;

    uint32_t E = 0;
    adapter_mode->handle_ = config;//EGLConfig(config); // ??handle_(config)??,
    EGLConfig H = adapter_mode->handle_;

    get_egl_config_ints(D, H, 0, 4, &E,
                        &adapter_mode->config_id,
                        &adapter_mode->config_caveat,
                        &adapter_mode->conformant,
                        &adapter_mode->buffer_type,
                        &adapter_mode->buffer_size);
    (adapter_mode->buffer_type == EGL_RGB_BUFFER)
    ? get_egl_config_ints(D, H, 5, 7, &E,
                          &adapter_mode->red_size,
                          &adapter_mode->green_size,
                          &adapter_mode->blue_size)
    : get_egl_config_int(D, H, 8, &E,
                         &adapter_mode->luminance_size);
    get_egl_config_ints(D, H, 9, 12, &E,
                        &adapter_mode->alpha_size,
                        &adapter_mode->alpha_mask_size,
                        &adapter_mode->depth_size,
                        &adapter_mode->stencil_size);
    get_egl_config_bool(D, H, 13, &E,
                        &adapter_mode->bind_to_texture_rgb);
    get_egl_config_bool(D, H, 14, &E,
                        &adapter_mode->bind_to_texture_rgba);
    get_egl_config_ints(D, H, 15, 20, &E,
                        &adapter_mode->level,
                        &adapter_mode->max_pbuffer_width,
                        &adapter_mode->max_pbuffer_height,
                        &adapter_mode->max_pbuffer_pixels,
                        &adapter_mode->min_swap_interval,
                        &adapter_mode->max_swap_interval);
    get_egl_config_bool(D, H, 21, &E,
                        &adapter_mode->native_renderable);
    get_egl_config_ints(D, H, 22, 25, &E,
                        &adapter_mode->native_visual_type,
                        &adapter_mode->native_visual_id,
                        &adapter_mode->surface_type,
                        &adapter_mode->sample_buffers);
    if(adapter_mode->sample_buffers)
        get_egl_config_int(D, H, 26, &E,
                           &adapter_mode->samples);
    get_egl_config_int(D, H, 27, &E,
                       &adapter_mode->transparent_type);
    if(adapter_mode->transparent_type != EGL_NONE)
        get_egl_config_ints(D, H, 28, 30, &E,
                            &adapter_mode->transparent_red_value,
                            &adapter_mode->transparent_green_value,
                            &adapter_mode->transparent_blue_value);

    if(E) unpack_egl_config_errors(E);
}

lv_egl_adapter_mode_t lv_egl_adapter_mode_blank()
{
    lv_egl_adapter_mode_t local_config = (lv_egl_adapter_mode_t)malloc(sizeof(struct lv_egl_adapter_mode));
    if(local_config) {
        lv_egl_adapter_mode_zero(local_config);
    }
    return local_config;
}

lv_egl_adapter_mode_t lv_egl_adapter_mode_create(EGLDisplay dpy, EGLConfig config)
{
    lv_egl_adapter_mode_t local_config = (lv_egl_adapter_mode_t)malloc(sizeof(struct lv_egl_adapter_mode));
    if(local_config) {
        lv_egl_adapter_mode_zero(local_config);
        lv_egl_adapter_mode_init(local_config, dpy, config);
    }
    return local_config;
}

void lv_egl_adapter_mode_print_bar()
{
    printf("  +------+----+-----+---+---+---+---+----+---+------+------+--------+--------+------+---+---+\n");
}

void lv_egl_adapter_mode_print_header()
{
    //lv_egl_adapter_mode* adapter_mode = (lv_egl_adapter_mode_t)(lv_egl_adapter_mode_ptr);
    printf(" ________ ____ _____ _______________ ____ ___ ______ ______ ________ ________ ______ _________ \n");
    printf("| config  bufr  rgb/   colorbuffer   dep  stn config native   max      max   surface  samples |\n");
    printf("|    id   size  lum   r   g   b   a   th  clr caveat render  width    height   type  buf  ns  |\n");
    printf(" `+------+----+-----+---+---+---+---+----+---+------+------+--------+--------+------+---+---+'\n");
}

void lv_egl_adapter_mode_print(void * lv_egl_adapter_mode_ptr, bool is_active)
{
    lv_egl_adapter_mode_t adapter_mode = (lv_egl_adapter_mode_t)lv_egl_adapter_mode_ptr;
    const char * caveat = "None";

    switch(adapter_mode->config_caveat) {
        case EGL_SLOW_CONFIG:
            caveat = "Slow";
            break;
        case EGL_NON_CONFORMANT_CONFIG:
            caveat = "Ncon";
            break;
        case EGL_NONE:
            // Initialized to none.
            break;
    }

    const char * doNative = adapter_mode->native_renderable ? "true" : "false";

    printf("%3s%#5x |%3d | %3s |%2d |%2d |%2d |%2d |%3d |%2d | %4s |%5s |%6d  |%6d  |%#5x |%2d |%2d |\n",
           is_active ? ">] " : " |",
           adapter_mode->config_id,
           adapter_mode->buffer_size,
           (adapter_mode->buffer_type == EGL_RGB_BUFFER) ? "rgb" : "lum",
           (adapter_mode->buffer_type == EGL_RGB_BUFFER) ? adapter_mode->red_size : adapter_mode->luminance_size,
           (adapter_mode->buffer_type == EGL_RGB_BUFFER) ? adapter_mode->green_size : 0,
           (adapter_mode->buffer_type == EGL_RGB_BUFFER) ? adapter_mode->blue_size : 0,
           adapter_mode->alpha_size,
           adapter_mode->depth_size,
           adapter_mode->stencil_size,
           caveat,
           doNative,
           adapter_mode->max_pbuffer_width,
           adapter_mode->max_pbuffer_height,
           adapter_mode->surface_type,
           adapter_mode->sample_buffers,
           adapter_mode->samples);
}

bool lv_egl_adapter_mode_is_window(void * lv_egl_adapter_mode_ptr)
{
    lv_egl_adapter_mode_t adapter_mode = (lv_egl_adapter_mode_t)(lv_egl_adapter_mode_ptr);
    return adapter_mode->surface_type & EGL_WINDOW_BIT;
}

EGLint lv_egl_adapter_mode_get_id(void * lv_egl_adapter_mode_ptr)
{
    lv_egl_adapter_mode_t adapter_mode = (lv_egl_adapter_mode_t)(lv_egl_adapter_mode_ptr);
    return adapter_mode->config_id;
}

lv_egl_adapter_mode_t lv_egl_adapter_mode_cleanup(void * config_ptr)
{
    if(config_ptr != NULL) {
        free(config_ptr);
    }
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void add_egl_config_error(uint32_t * errorFlags, enum lv_egl_adapter_mode_error error)
{
    *errorFlags |= error;
}

static void unpack_egl_config_errors(uint32_t errorFlags)
{
    for(int i = 0; i < 31; ++i) if(errorFlags & (1 << i)) printf("Error: %s\n", eglAttributeNames[i]);
}

static void get_egl_config_int(EGLDisplay dpy, EGLConfig handle, int index, uint32_t * errorFlags, EGLint * output)
{
    if(!eglGetConfigAttrib(dpy, handle, eglAttributes[index], output)) add_egl_config_error(errorFlags,
                                                                                                (enum lv_egl_adapter_mode_error)(1 << index));
}

static void get_egl_config_bool(EGLDisplay dpy, EGLConfig handle, int index, uint32_t * errorFlags, bool * output)
{
    EGLint temp_bool = EGL_FALSE;
    get_egl_config_int(dpy, handle, index, errorFlags, &temp_bool);
    *output = (temp_bool == EGL_TRUE);
}

static void get_egl_config_ints(EGLDisplay dpy, EGLConfig handle, int startIndex, int endIndex, uint32_t * errorFlags,
                                ...)
{
    va_list args;
    va_start(args, errorFlags);
    for(int i = startIndex; i <= endIndex; ++i)
        get_egl_config_int(dpy, handle, i, errorFlags, va_arg(args, EGLint *));
    va_end(args);
}
