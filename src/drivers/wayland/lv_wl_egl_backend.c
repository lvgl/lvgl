/** @file lv_wl_egl_backend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland_private.h"

#if LV_WAYLAND_USE_EGL

#include "../../display/lv_display_private.h"
#include "../opengles/lv_opengles_driver.h"
#include "../opengles/lv_opengles_texture_private.h"
#include "../opengles/lv_opengles_egl_private.h"
#include "../opengles/lv_opengles_debug.h"

#include <wayland-egl.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/


typedef struct {
    lv_opengles_texture_t texture;
    struct wl_egl_window * egl_window;
    lv_opengles_egl_t * egl_ctx;
} lv_wl_egl_display_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * wl_egl_init(void);
static void wl_egl_deinit(void * backend_ctx);
static void * wl_egl_init_display(void * backend_ctx, lv_display_t * display, int32_t width, int32_t height);
static void * wl_egl_resize_display(void * backend_ctx, lv_display_t * display);
static void wl_egl_deinit_display(void * backend_ctx, lv_display_t * display);
static void wl_egl_global_handler(void * backend_ctx, struct wl_registry * registry, uint32_t name,
                                  const char * interface, uint32_t version);

static void egl_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void flush_wait_cb(lv_display_t * disp);
static void frame_done(void * data, struct wl_callback * callback, uint32_t time);

static lv_wl_egl_display_data_t * egl_create_display_data(lv_display_t * display,
                                                          int32_t width, int32_t height);
static void egl_destroy_display_data(lv_wl_egl_display_data_t * ddata);

static lv_egl_interface_t wl_egl_get_interface(lv_display_t * display);
static void * wl_egl_create_window(void * driver_data, const lv_egl_native_window_properties_t * properties);
static void wl_egl_destroy_window(void * driver_data, void * native_window);
static size_t wl_egl_select_config_cb(void * driver_data, const lv_egl_config_t * configs, size_t config_count);
static void wl_egl_flip_cb(void * driver_data, bool vsync);

/**********************
 *  STATIC VARIABLES
 **********************/


static const struct wl_callback_listener frame_listener = {
    .done = frame_done,
};

const lv_wayland_backend_ops_t wl_backend_ops = {
    .init = wl_egl_init,
    .deinit = wl_egl_deinit,
    .global_handler = wl_egl_global_handler,
    .init_display = wl_egl_init_display,
    .deinit_display = wl_egl_deinit_display,
    .resize_display = wl_egl_resize_display,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void frame_done(void * data, struct wl_callback * callback, uint32_t time)
{
    LV_UNUSED(time);
    lv_display_t * display = data;
    wl_callback_destroy(callback);
    lv_display_flush_ready(display);
}

static void * wl_egl_init(void)
{
    return NULL;
}

static void wl_egl_deinit(void * backend_ctx)
{
    LV_UNUSED(backend_ctx);
}

static lv_wl_egl_display_data_t * egl_create_display_data(lv_display_t * display,
                                                          int32_t width, int32_t height)
{
    lv_wl_egl_display_data_t * ddata = lv_zalloc(sizeof(*ddata));
    if(!ddata) {
        LV_LOG_ERROR("Failed to allocate data for display");
        return NULL;
    }

    /* Set the backend display data immediately as we will need it
     * in the EGL window creation callback */
    lv_wayland_set_backend_display_data(display, ddata);


    /* Create EGL context */
    lv_egl_interface_t egl_interface = wl_egl_get_interface(display);
    ddata->egl_ctx = lv_opengles_egl_context_create(&egl_interface);
    if(!ddata->egl_ctx) {
        LV_LOG_ERROR("Failed to create EGL context");
        goto egl_ctx_err;
    }

    /* Let the opengles texture driver handle the texture lifetime */
    ddata->texture.is_texture_owner = true;

    /*Initialize the draw buffers and texture*/
    lv_result_t res = lv_opengles_texture_reshape(&ddata->texture, display, width, height);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to create draw buffers");
        goto texture_err;
    }
    return ddata;

texture_err:
    lv_opengles_egl_context_destroy(ddata->egl_ctx);
egl_ctx_err:
    lv_wayland_set_backend_display_data(display, NULL);
    lv_free(ddata);
    return NULL;
}

static void egl_destroy_display_data(lv_wl_egl_display_data_t * ddata)
{
    if(!ddata) {
        return;
    }

    lv_opengles_texture_deinit(&ddata->texture);

    if(ddata->egl_ctx) {
        lv_opengles_egl_context_destroy(ddata->egl_ctx);
        ddata->egl_ctx = NULL;
    }

    LV_LOG_INFO("Deleted EGL display data");
    lv_free(ddata);
}

static void flush_wait_cb(lv_display_t * disp)
{
    while(disp->flushing) {
        wl_display_dispatch(lv_wl_ctx.wl_display);
    }
}

#if LV_USE_DRAW_OPENGLES || LV_USE_DRAW_NANOVG

static void egl_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
    LV_UNUSED(px_map);

    int32_t disp_width = lv_display_get_horizontal_resolution(disp);
    int32_t disp_height = lv_display_get_vertical_resolution(disp);
    if(!lv_display_flush_is_last(disp)) {
        lv_display_flush_ready(disp);
        return;
    }

    lv_wl_egl_display_data_t * ddata = lv_wayland_get_backend_display_data(disp);
    struct wl_surface * surface = lv_wayland_get_window_surface(disp);

    if(!surface) {
        lv_display_flush_ready(disp);
        return;
    }

#if LV_USE_DRAW_OPENGLES
    lv_opengles_viewport(0, 0, lv_display_get_original_horizontal_resolution(disp),
                         lv_display_get_original_vertical_resolution(disp));
    lv_opengles_render_display_texture(disp, false, true);
#endif /*LV_USE_DRAW_OPENGLES*/

    /* Swap buffers through EGL */
    lv_opengles_egl_update(ddata->egl_ctx);

    /* Request frame callback for vsync */
    struct wl_callback * callback = wl_surface_frame(surface);
    wl_callback_add_listener(callback, &frame_listener, disp);
    wl_surface_damage(surface, 0, 0, disp_width, disp_height);
    wl_surface_commit(surface);
}

#else

static void egl_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(px_map);
    LV_UNUSED(area);

    if(!lv_display_flush_is_last(disp)) {
        lv_display_flush_ready(disp);
        return;
    }

    lv_wl_egl_display_data_t * ddata = lv_wayland_get_backend_display_data(disp);
    struct wl_surface * surface = lv_wayland_get_window_surface(disp);

    if(!surface) {
        lv_display_flush_ready(disp);
        return;
    }

    int32_t disp_width = lv_display_get_horizontal_resolution(disp);
    int32_t disp_height = lv_display_get_vertical_resolution(disp);

    lv_opengles_viewport(0, 0, lv_display_get_original_horizontal_resolution(disp),
                         lv_display_get_original_vertical_resolution(disp));

    lv_color_format_t cf = lv_display_get_color_format(disp);
    uint32_t stride = lv_draw_buf_width_to_stride(disp_width, cf);

    GL_CALL(glBindTexture(GL_TEXTURE_2D, ddata->texture.texture_id));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CALL(glPixelStorei(GL_UNPACK_ROW_LENGTH, stride / lv_color_format_get_size(cf)));

#if LV_COLOR_DEPTH == 16
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, disp_width, disp_height, 0, GL_RGB,
                         GL_UNSIGNED_SHORT_5_6_5, ddata->texture.fb1));
#elif LV_COLOR_DEPTH == 32
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, disp_width, disp_height, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, ddata->texture.fb1));
#else
#error("Unsupported color format")
#endif
    lv_opengles_render_params_t params = {
        .h_flip = false,
        .v_flip = false,
        .rb_swap = LV_COLOR_DEPTH == 32,
    };
    lv_opengles_render_display(disp, &params);
    lv_opengles_egl_update(ddata->egl_ctx);

    struct wl_callback * callback = wl_surface_frame(surface);
    wl_callback_add_listener(callback, &frame_listener, disp);

    wl_surface_damage(surface, 0, 0, disp_width, disp_height);
    wl_surface_commit(surface);
}

#endif

static void * wl_egl_init_display(void * backend_ctx, lv_display_t * display, int32_t width, int32_t height)
{
    LV_UNUSED(backend_ctx);
    lv_wl_egl_display_data_t * ddata = egl_create_display_data(display, width, height);
    if(!ddata) {
        LV_LOG_ERROR("Failed to create display data");
        return NULL;
    }

    lv_display_set_flush_cb(display, egl_flush_cb);
    lv_display_set_flush_wait_cb(display, flush_wait_cb);
    lv_display_set_render_mode(display, LV_USE_DRAW_NANOVG ? LV_DISPLAY_RENDER_MODE_FULL : LV_DISPLAY_RENDER_MODE_DIRECT);

    return ddata;
}

static void * wl_egl_resize_display(void * backend_ctx, lv_display_t * display)
{
    LV_UNUSED(backend_ctx);
    lv_wl_egl_display_data_t * ddata = lv_wayland_get_backend_display_data(display);

    int32_t width = lv_display_get_horizontal_resolution(display);
    int32_t height = lv_display_get_vertical_resolution(display);
    lv_result_t res = lv_opengles_texture_reshape(&ddata->texture, display, width, height);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to resize display");
        return ddata;
    }

    wl_egl_window_resize(ddata->egl_window, width, height, 0, 0);
    return ddata;
}

static void wl_egl_deinit_display(void * backend_ctx, lv_display_t * display)
{
    LV_UNUSED(backend_ctx);
    lv_wl_egl_display_data_t * ddata = lv_wayland_get_backend_display_data(display);
    egl_destroy_display_data(ddata);
}

static void wl_egl_global_handler(void * backend_ctx, struct wl_registry * registry, uint32_t name,
                                  const char * interface, uint32_t version)
{
    LV_UNUSED(backend_ctx);
    LV_UNUSED(registry);
    LV_UNUSED(name);
    LV_UNUSED(interface);
    LV_UNUSED(version);

    /* No specific Wayland globals needed for basic EGL support */
}

static lv_egl_interface_t wl_egl_get_interface(lv_display_t * display)
{
    return (lv_egl_interface_t) {
        .driver_data = display,
        .native_display = lv_wl_ctx.wl_display,
        .egl_platform = EGL_PLATFORM_WAYLAND_KHR,
        .select_config = wl_egl_select_config_cb,
        .flip_cb = wl_egl_flip_cb,
        .create_window_cb = wl_egl_create_window,
        .destroy_window_cb = wl_egl_destroy_window,
    };
}

static size_t wl_egl_select_config_cb(void * driver_data, const lv_egl_config_t * configs, size_t config_count)
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
        LV_LOG_TRACE("Got config %zu %#x %dx%d %d %d %d %d buffer size %d depth %d samples %d stencil %d surface type %d",
                     i, configs[i].id,
                     configs[i].max_width, configs[i].max_height,
                     configs[i].r_bits, configs[i].g_bits, configs[i].b_bits, configs[i].a_bits,
                     configs[i].buffer_size, configs[i].depth, configs[i].samples,
                     configs[i].stencil, configs[i].surface_type);
    }

    for(size_t i = 0; i < config_count; ++i) {
        lv_color_format_t config_cf = lv_opengles_egl_color_format_from_egl_config(&configs[i]);
        const bool resolution_matches = configs[i].max_width >= target_w &&
                                        configs[i].max_height >= target_h;
        const bool is_nanovg_compatible = (configs[i].renderable_type & EGL_OPENGL_ES2_BIT) != 0 &&
                                          configs[i].stencil == 8 && configs[i].samples == 4;
        const bool is_window = (configs[i].surface_type & EGL_WINDOW_BIT) != 0;
        const bool is_compatible_with_draw_unit = is_nanovg_compatible || !LV_USE_DRAW_NANOVG;

        if(is_window && resolution_matches && config_cf == target_cf && is_compatible_with_draw_unit) {
            LV_LOG_TRACE("Choosing config %zu", i);
            return i;
        }
    }

    return config_count;
}

static void * wl_egl_create_window(void * driver_data, const lv_egl_native_window_properties_t * properties)
{
    LV_UNUSED(properties);
    lv_display_t * display = (lv_display_t *)driver_data;

    struct wl_surface * wl_surface = lv_wayland_get_window_surface(display);
    lv_wl_egl_display_data_t * ddata = lv_wayland_get_backend_display_data(display);

    if(!wl_surface) {
        LV_LOG_ERROR("Failed to get Wayland surface");
        return NULL;
    }

    ddata->egl_window = wl_egl_window_create(wl_surface,
                                             lv_display_get_horizontal_resolution(display),
                                             lv_display_get_vertical_resolution(display));
    if(!ddata->egl_window) {
        LV_LOG_ERROR("Failed to create wl_egl_window");
        return NULL;
    }

    return ddata->egl_window;
}

static void wl_egl_destroy_window(void * driver_data, void * native_window)
{
    lv_display_t * display = (lv_display_t *)driver_data;
    lv_wl_egl_display_data_t * ddata = lv_wayland_get_backend_display_data(display);

    LV_ASSERT(ddata->egl_window == native_window);
    if(ddata->egl_window) {
        wl_egl_window_destroy(ddata->egl_window);
        ddata->egl_window = NULL;
    }
}

static void wl_egl_flip_cb(void * driver_data, bool vsync)
{
    LV_UNUSED(driver_data);
    LV_UNUSED(vsync);

    /* For Wayland, buffer swapping is handled by the compositor
     * through wl_surface_commit() which is called in the flush callback */
}

#endif /*LV_WAYLAND_USE_EGL*/
