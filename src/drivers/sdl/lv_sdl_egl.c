/**
 * @file lv_sdl_egl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_SDL && LV_USE_EGL

#include "../../display/lv_display.h"
#include <SDL2/SDL_syswm.h>
#include "lv_sdl_private.h"
#include "../opengles/lv_opengles_egl_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * create_window_cb(void * driver_data, const lv_egl_native_window_properties_t * props);
static void destroy_window_cb(void * driver_data, void * native_window);
static void flip_cb(void * driver_data, bool vsync);
static size_t select_config_cb(void * driver_data, const lv_egl_config_t * configs, size_t config_count);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_egl_interface_t lv_sdl_get_egl_interface(lv_display_t * display)
{
    return (lv_egl_interface_t) {
        .driver_data = display,
        .create_window_cb = create_window_cb,
        .destroy_window_cb = destroy_window_cb,
        .egl_platform = 0,
        .native_display = EGL_DEFAULT_DISPLAY,
        .flip_cb = flip_cb,
        .select_config = select_config_cb,
    };

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * create_window_cb(void * driver_data, const lv_egl_native_window_properties_t * props)
{
    LV_UNUSED(props);
    lv_sdl_window_t * dsc = lv_display_get_driver_data(driver_data);
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(dsc->window, &wmInfo);

    EGLNativeWindowType native_window;
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
    native_window = wmInfo.info.win.window;
#elif defined(SDL_VIDEO_DRIVER_X11)
    native_window = wmInfo.info.x11.window;
#elif defined(SDL_VIDEO_DRIVER_WAYLAND)
    native_window = wmInfo.info.wl.surface;
#else
    LV_LOG_ERROR("Unsupported platform for EGL");
    return false;
#endif
    return (void *)native_window;
}

static size_t select_config_cb(void * driver_data, const lv_egl_config_t * configs, size_t config_count)
{
    lv_display_t * display = (lv_display_t *)driver_data;
    int32_t target_w = lv_display_get_horizontal_resolution(display);
    int32_t target_h = lv_display_get_vertical_resolution(display);

#if LV_COLOR_DEPTH == 16
    lv_color_format_t target_cf = LV_COLOR_FORMAT_RGB565;
#elif LV_COLOR_DEPTH == 32
    lv_color_format_t target_cf = LV_COLOR_FORMAT_ARGB8888;
#else
#error("Unsupported color format")
#endif


    for(size_t i = 0; i < config_count; ++i) {
        LV_LOG_USER("Got config %zu %#x %dx%d %d %d %d %d buffer size %d depth %d  samples %d stencil %d surface type %d renderable type %d",
                    i, configs[i].id,
                    configs[i].max_width, configs[i].max_height, configs[i].r_bits, configs[i].g_bits, configs[i].b_bits, configs[i].a_bits,
                    configs[i].buffer_size, configs[i].depth, configs[i].samples, configs[i].stencil,
                    configs[i].surface_type & EGL_WINDOW_BIT, configs[i].renderable_type & EGL_OPENGL_ES2_BIT);
    }

    for(size_t i = 0; i < config_count; ++i) {
        lv_color_format_t config_cf = lv_opengles_egl_color_format_from_egl_config(&configs[i]);
        if(configs[i].max_width >= target_w &&
           configs[i].max_height >= target_h &&
           config_cf == target_cf &&
           configs[i].surface_type & EGL_WINDOW_BIT &&
           configs[i].renderable_type & EGL_OPENGL_ES2_BIT &&
           configs[i].stencil == 8 &&
           configs[i].samples == 4

          ) {
            LV_LOG_USER("Choosing config %zu", i);
            return i;
        }
    }
    return config_count;
}
static void destroy_window_cb(void * driver_data, void * native_window)
{
    LV_UNUSED(driver_data);
    LV_UNUSED(native_window);
}

static void flip_cb(void * driver_data, bool vsync)
{

    LV_UNUSED(driver_data);
    LV_UNUSED(vsync);
}

#endif
