/**
 * @file lv_sdl_egl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_SDL && LV_USE_EGL

#include "../../display/lv_display_private.h"
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
static lv_egl_interface_t lv_sdl_get_egl_interface(lv_display_t * display);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_sdl_egl_init(lv_display_t * disp)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    int32_t hor_res = (int32_t)((float)(disp->hor_res) * dsc->zoom);
    int32_t ver_res = (int32_t)((float)(disp->ver_res) * dsc->zoom);
    lv_egl_interface_t ifc = lv_sdl_get_egl_interface(disp);
    dsc->egl_ctx = lv_opengles_egl_context_create(&ifc);
    if(!dsc->egl_ctx) {
        LV_LOG_ERROR("Failed to initialize EGL context");
        return LV_RESULT_INVALID;
    }

#if LV_USE_DRAW_OPENGLES
    lv_result_t res = lv_sdl_egl_resize(disp);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to create draw buffers");
        lv_opengles_egl_context_destroy(dsc->egl_ctx);
        dsc->egl_ctx = NULL;
        return LV_RESULT_INVALID;
    }
#endif
    return LV_RESULT_OK;
}
void lv_sdl_egl_deinit(lv_display_t * disp)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    if(dsc->egl_ctx) {
        lv_opengles_egl_context_destroy(dsc->egl_ctx);
        dsc->egl_ctx = NULL;
    }

#if LV_USE_DRAW_OPENGLES
    lv_opengles_texture_deinit(&dsc->opengles_texture);
#endif
}

lv_result_t lv_sdl_egl_resize(lv_display_t * disp)
{
#if LV_USE_DRAW_OPENGLES
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    int32_t hor_res = (int32_t)((float)(disp->hor_res) * dsc->zoom);
    int32_t ver_res = (int32_t)((float)(disp->ver_res) * dsc->zoom);
    dsc->opengles_texture.is_texture_owner = true;
    return lv_opengles_texture_reshape(&dsc->opengles_texture, disp, hor_res, ver_res);
#endif
    return LV_RESULT_OK;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_egl_interface_t lv_sdl_get_egl_interface(lv_display_t * display)
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
        LV_LOG_TRACE("Got config %zu %#x %dx%d %d %d %d %d buffer size %d depth %d  samples %d stencil %d surface type %d renderable type %d",
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
            LV_LOG_INFO("Choosing config %zu", i);
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
