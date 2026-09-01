/**
 * @file lv_sdl_egl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl_public.h"

#if LV_SDL_USE_EGL

#include <SDL2/SDL_syswm.h>
#include "lv_sdl_private.h"
#include "../opengles/lv_opengles_egl_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_opengles_egl_t * egl_ctx;
    lv_opengles_texture_t opengles_texture;
} lv_sdl_egl_display_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * create_window_cb(void * driver_data, const lv_egl_native_window_properties_t * props);
static void destroy_window_cb(void * driver_data, void * native_window);
static void flip_cb(void * driver_data, bool vsync);
static size_t select_config_cb(void * driver_data, const lv_egl_config_t * configs, size_t config_count);
static lv_egl_interface_t lv_sdl_get_egl_interface(lv_display_t * display);
static void flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map);

static lv_result_t init_display(lv_display_t * display);
static lv_result_t resize_display(lv_display_t * display);
static void deinit_display(lv_display_t * display);
static SDL_Renderer * get_renderer(lv_display_t * display);
static lv_result_t redraw(lv_display_t * display);

const lv_sdl_backend_ops_t lv_sdl_backend_ops = {
    .init_display = init_display,
    .resize_display = resize_display,
    .deinit_display = deinit_display,
    .redraw = redraw,
    .get_renderer = get_renderer,
};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_result_t init_display(lv_display_t * display)
{
    lv_egl_interface_t ifc = lv_sdl_get_egl_interface(display);
    lv_sdl_egl_display_data_t * ddata = lv_malloc_zeroed(sizeof(*ddata));
    if(!ddata) {
        LV_LOG_WARN("Failed to allocate memory for display data");
        return LV_RESULT_INVALID;
    }
    ddata->egl_ctx = lv_opengles_egl_context_create(&ifc);
    if(!ddata->egl_ctx) {
        LV_LOG_ERROR("Failed to initialize EGL context.");
        LV_LOG_ERROR("This probably means you're trying to use an unsupported SDL_VIDEO_DRIVER.");
        LV_LOG_ERROR("Try setting SDL_VIDEODRIVER=x11 via an environment variable");
        lv_free(ddata);
        return LV_RESULT_INVALID;
    }

    lv_sdl_backend_set_display_data(display, ddata);

    if(LV_USE_DRAW_NANOVG) {
        static lv_draw_buf_t draw_buf;
        static uint8_t dummy_buf;
        lv_draw_buf_init(&draw_buf, 4096, 4096, LV_COLOR_FORMAT_ARGB8888, 4096 * 4, &dummy_buf, 4096 * 4096 * 4);

        lv_display_set_draw_buffers(display, &draw_buf, NULL);
        lv_display_set_render_mode(display, LV_DISPLAY_RENDER_MODE_FULL);
    }
    else {
        lv_result_t res = resize_display(display);
        if(res != LV_RESULT_OK) {
            LV_LOG_ERROR("Failed to create draw buffers");
            lv_opengles_egl_context_destroy(ddata->egl_ctx);
            lv_free(ddata);
            lv_sdl_backend_set_display_data(display, NULL);
            return LV_RESULT_INVALID;
        }
        lv_display_set_render_mode(display, LV_DISPLAY_RENDER_MODE_DIRECT);
    }
    lv_display_set_flush_cb(display, flush_cb);

    return LV_RESULT_OK;

}
static lv_result_t resize_display(lv_display_t * display)
{
    if(!LV_USE_DRAW_OPENGLES) {
        return LV_RESULT_OK;
    }

    lv_sdl_egl_display_data_t * ddata = lv_sdl_backend_get_display_data(display);
    LV_ASSERT(ddata != NULL);

    int32_t hor_res = lv_sdl_window_get_horizontal_resolution(display);
    int32_t ver_res = lv_sdl_window_get_vertical_resolution(display);
    ddata->opengles_texture.is_texture_owner = true;
    return lv_opengles_texture_reshape(&ddata->opengles_texture, display, hor_res, ver_res);

}
static void deinit_display(lv_display_t * display)
{
    lv_sdl_egl_display_data_t * ddata = lv_sdl_backend_get_display_data(display);
    if(ddata->egl_ctx) {
        lv_opengles_egl_context_destroy(ddata->egl_ctx);
        ddata->egl_ctx = NULL;
    }

    if(LV_USE_DRAW_OPENGLES) {
        lv_opengles_texture_deinit(&ddata->opengles_texture);
    }

    lv_free(ddata);
    lv_sdl_backend_set_display_data(display, NULL);
}


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
    lv_display_t * display = (lv_display_t *)driver_data;
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if(SDL_GetWindowWMInfo(lv_sdl_window_get_window(display), &wmInfo) != SDL_TRUE) {
        LV_LOG_ERROR("SDL_GetWindowWMInfo failed: %s", SDL_GetError());
        return NULL;
    }

    EGLNativeWindowType native_window;
    const char * driver = SDL_GetCurrentVideoDriver();

    if(driver && strcmp(driver, "x11") == 0) {
#if defined(SDL_VIDEO_DRIVER_X11)
        native_window = (EGLNativeWindowType)wmInfo.info.x11.window;
#else
        LV_LOG_ERROR("SDL built without X11 support");
        return NULL;
#endif
    }
    else {
        LV_LOG_ERROR("Unsupported SDL video driver: (%s)", driver ? driver : "(null)");
        return NULL;
    }
    return (void *)native_window;
}

static void flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{

    LV_UNUSED(area);
    LV_UNUSED(px_map);
    lv_sdl_egl_display_data_t * ddata = lv_sdl_backend_get_display_data(display);
    LV_ASSERT(ddata != NULL);

    if(lv_display_flush_is_last(display)) {
#if LV_USE_DRAW_OPENGLES
        lv_opengles_viewport(0, 0,
                             lv_display_get_original_horizontal_resolution(display),
                             lv_display_get_original_vertical_resolution(display));
        lv_opengles_render_display_texture_internal(display, false, true);
#endif /*LV_USE_DRAW_OPENGLES*/
        lv_opengles_egl_update(ddata->egl_ctx);
    }
    lv_display_flush_ready(display);
    return;
}

static size_t select_config_cb(void * driver_data, const lv_egl_config_t * configs, size_t config_count)
{
    lv_display_t * display = (lv_display_t *)driver_data;
    return lv_opengles_egl_display_select_config(display, configs, config_count);
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


static SDL_Renderer * get_renderer(lv_display_t * display)
{
    LV_UNUSED(display);
    return NULL;
}
static lv_result_t redraw(lv_display_t * display)
{
    LV_UNUSED(display);
    return LV_RESULT_OK;
}

#endif /*LV_SDL_USE_EGL*/
