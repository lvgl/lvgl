/** @file lv_wayland_backend_egl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland_private.h"

#if LV_WAYLAND_USE_EGL == 1

#include "../../display/lv_display_private.h"
#include "../opengles/lv_opengles_texture_private.h"
#include "../opengles/lv_opengles_egl_private.h"
#include "../opengles/lv_opengles_debug.h"

#include <wayland-egl.h>

/* The DMA-BUF fast-path is a software-rendering optimization. lv_conf_internal.h
 * already derives LV_WAYLAND_USE_DMABUF internally and enables it only when
 * no GPU draw unit is active, so it maps directly to the local guard below. */
#if LV_WAYLAND_USE_DMABUF == 1
    #define LV_WL_EGL_DMABUF_ENABLED 1
#else
    #define LV_WL_EGL_DMABUF_ENABLED 0
#endif

#if LV_WL_EGL_DMABUF_ENABLED
    #include "lv_wayland_dmabuf.h"
    #include <fcntl.h>
    #include <string.h>
    #include <stdlib.h>
    #include LV_STDINT_INCLUDE
    #include <unistd.h>
    #include <poll.h>
    #include <gbm.h>
    #include <drm_fourcc.h>
    #include <xf86drm.h>
    #include <xf86drmMode.h>
    #include "../opengles/lv_opengles_private.h"
    #include <GLES2/gl2platform.h>
    #include <GLES2/gl2ext.h>
#endif /*LV_WL_EGL_DMABUF_ENABLED*/

/*********************
 *      DEFINES
 *********************/

#if LV_WL_EGL_DMABUF_ENABLED
    #define LV_WL_EGL_BUF_COUNT 2
#endif

/**********************
 *      TYPEDEFS
 **********************/

#if LV_WL_EGL_DMABUF_ENABLED

typedef void (GL_APIENTRYP PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)(GLenum target, GLeglImageOES image);

typedef struct {
    /* Shared DMA-BUF protocol state (wl_buffer + release tracking) */
    lv_wayland_dmabuf_buffer_t base;

    /* EGL/GBM allocator specific state */
    struct gbm_bo * bo;
    int dmabuf_fd;
    uint32_t stride;
    uint32_t offset;

    EGLImageKHR egl_image;
    GLuint texture_id;
} lv_wl_buffer_t;

#endif /*LV_WL_EGL_DMABUF_ENABLED*/

typedef struct {
    /* Used by the classic path (GPU render target, or software upload target). */
    lv_opengles_texture_t texture;

    struct wl_egl_window * egl_window;
    lv_opengles_egl_t * egl_ctx;

#if LV_WL_EGL_DMABUF_ENABLED
    lv_wl_buffer_t buffers[LV_WL_EGL_BUF_COUNT];
    /* Software draw buffers owned by this backend in DMA-BUF mode. */
    uint8_t * sw_buf[2];
    struct gbm_device * gbm_device;
    int drm_fd;
    int32_t width;
    int32_t height;
    uint8_t last_used;
    /* Set at init time once the DMA-BUF path has been confirmed available.
     * When false the classic software path above is used as a fallback. */
    bool use_dmabuf;
#endif
} lv_wl_egl_display_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t wl_egl_init(void ** backend_data);
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

#if LV_WL_EGL_DMABUF_ENABLED
static bool dmabuf_is_available(void);
static bool egl_dmabuf_setup(lv_wl_egl_display_data_t * ddata, lv_display_t * display, int32_t width, int32_t height);
static bool egl_dmabuf_reinit_buffers(lv_wl_egl_display_data_t * ddata, lv_display_t * display,
                                      int32_t width, int32_t height);
static void egl_dmabuf_teardown(lv_wl_egl_display_data_t * ddata);

static lv_wl_buffer_t * get_next_buffer(lv_wl_egl_display_data_t * ddata);

static uint32_t lv_drm_cf_to_gbm_cf(uint32_t drm_cf);
static void load_egl_extensions(void);
static bool init_buffer(lv_wayland_dmabuf_ctx_t * dmabuf_ctx, lv_wl_buffer_t * buffer, uint32_t width,
                        uint32_t height, lv_color_format_t cf, lv_wl_egl_display_data_t * ddata);
static int open_drm_device(void);
static void delete_buffer(lv_opengles_egl_t * egl_ctx, lv_wl_buffer_t * buffer);
#endif /*LV_WL_EGL_DMABUF_ENABLED*/

/**********************
 *  STATIC VARIABLES
 **********************/


/* TODO: the opengl driver doesn't support multiple instances
 * so we can only handle one display per EGL instance
 * WIP: https://github.com/lvgl/lvgl/pull/9854 */
static bool has_display;
static const struct wl_callback_listener frame_listener = {
    .done = frame_done,
};

const lv_wayland_backend_ops_t wl_egl_ops = {
    .init = wl_egl_init,
    .deinit = wl_egl_deinit,
    .global_handler = wl_egl_global_handler,
};

const lv_wayland_backend_display_ops_t wl_egl_display_ops = {
    .init_display = wl_egl_init_display,
    .deinit_display = wl_egl_deinit_display,
    .resize_display = wl_egl_resize_display,
};

#if LV_WL_EGL_DMABUF_ENABLED

    static lv_wayland_dmabuf_ctx_t ctx;
    static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = NULL;

#endif /*LV_WL_EGL_DMABUF_ENABLED*/

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

static lv_result_t wl_egl_init(void ** backend_data)
{
    has_display = false;
#if LV_WL_EGL_DMABUF_ENABLED
    lv_wayland_dmabuf_ctx_init(&ctx);
    *backend_data = &ctx;
#else
    /* Without the DMA-BUF fast-path the backend keeps no global state */
    *backend_data = NULL;
#endif
    return LV_RESULT_OK;
}

static void wl_egl_deinit(void * backend_ctx)
{
#if LV_WL_EGL_DMABUF_ENABLED
    lv_wayland_dmabuf_ctx_deinit((lv_wayland_dmabuf_ctx_t *)backend_ctx);
#else
    LV_UNUSED(backend_ctx);
#endif
}

static lv_wl_egl_display_data_t * egl_create_display_data(lv_display_t * display,
                                                          int32_t width, int32_t height)
{
    lv_wl_egl_display_data_t * ddata = lv_zalloc(sizeof(*ddata));
    if(!ddata) {
        LV_LOG_WARN("Failed to allocate data for display");
        return NULL;
    }

#if LV_WL_EGL_DMABUF_ENABLED
    ddata->drm_fd = -1;
#endif

    /* Set the backend display data immediately as we will need it
     * in the EGL window creation callback */
    lv_wayland_set_backend_display_data(display, ddata);

    /* Create EGL context */
    lv_egl_interface_t egl_interface = wl_egl_get_interface(display);
    ddata->egl_ctx = lv_opengles_egl_context_create(&egl_interface);
    if(!ddata->egl_ctx) {
        LV_LOG_WARN("Failed to create EGL context");
        goto egl_ctx_err;
    }

#if LV_WL_EGL_DMABUF_ENABLED
    /* Try the zero-copy DMA-BUF software fast-path. On any failure fall back
     * transparently to the classic software path below. */
    if(egl_dmabuf_setup(ddata, display, width, height)) {
        ddata->use_dmabuf = true;
        return ddata;
    }
    LV_LOG_WARN("Wayland EGL: DMA-BUF unavailable, using the classic software path");
#endif

    /* Let the opengles texture driver handle the texture lifetime */
    ddata->texture.is_texture_owner = true;

    /*Initialize the draw buffers and texture*/
    lv_result_t res = lv_opengles_texture_reshape(&ddata->texture, display, width, height);
    if(res != LV_RESULT_OK) {
        LV_LOG_WARN("Failed to create draw buffers");
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

#if LV_WL_EGL_DMABUF_ENABLED
    if(ddata->use_dmabuf) {
        /* Buffers reference the EGL context, so tear them down before it. */
        egl_dmabuf_teardown(ddata);
    }
    else
#endif
    {
        lv_opengles_texture_deinit(&ddata->texture);
    }

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

#else /*Software rendering*/

static void egl_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
#if !LV_WL_EGL_DMABUF_ENABLED
    LV_UNUSED(px_map);
#endif

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

#if LV_WL_EGL_DMABUF_ENABLED
    if(ddata->use_dmabuf) {
        /* Zero-copy fast-path: upload the software framebuffer directly into a
         * DMA-BUF backed texture and attach its wl_buffer to the surface. No
         * full-screen quad pass and no eglSwapBuffers copy. */
        lv_wl_buffer_t * buf = get_next_buffer(ddata);
        if(!buf) {
            LV_LOG_WARN("Failed to acquire a wayland window body buffer");
            lv_display_flush_ready(disp);
            return;
        }

        lv_color_format_t cf = lv_display_get_color_format(disp);
        uint32_t stride = lv_draw_buf_width_to_stride(disp_width, cf);

        GL_CALL(glBindTexture(GL_TEXTURE_2D, buf->texture_id));
        GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
        GL_CALL(glPixelStorei(GL_UNPACK_ROW_LENGTH, stride / lv_color_format_get_size(cf)));
#if LV_COLOR_DEPTH == 16
        GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, disp_width, disp_height,
                                GL_RGB, GL_UNSIGNED_SHORT_5_6_5, px_map));
#elif LV_COLOR_DEPTH == 32
        GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, disp_width, disp_height,
                                GL_BGRA, GL_UNSIGNED_BYTE, px_map));
#endif
        GL_CALL(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

        wl_surface_attach(surface, buf->base.wl_buffer, 0, 0);
        wl_surface_damage(surface, 0, 0, disp_width, disp_height);

        struct wl_callback * callback = wl_surface_frame(surface);
        wl_callback_add_listener(callback, &frame_listener, disp);

        wl_surface_commit(surface);

        buf->base.busy = true;
        wl_display_flush(lv_wl_ctx.wl_display);
        return;
    }
#endif /*LV_WL_EGL_DMABUF_ENABLED*/

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
    if(has_display) {
        LV_LOG_INFO("The EGL backend can only handle one display at a time");
        return NULL;
    }
    lv_wl_egl_display_data_t * ddata = egl_create_display_data(display, width, height);
    if(!ddata) {
        LV_LOG_WARN("Failed to create display data");
        return NULL;
    }

#if LV_WL_EGL_DMABUF_ENABLED
    if(ddata->use_dmabuf) {
        lv_color_format_t cf = lv_display_get_color_format(display);
        uint32_t stride = lv_draw_buf_width_to_stride(width, cf);
        uint32_t buf_size = stride * height;

        uint8_t * buf1 = lv_malloc(buf_size);
        uint8_t * buf2 = lv_malloc(buf_size);
        LV_ASSERT_MALLOC(buf1);
        LV_ASSERT_MALLOC(buf2);
        if(!buf1 || !buf2) {
            LV_LOG_WARN("Failed to allocate display buffer");
            lv_free(buf1);
            lv_free(buf2);
            egl_destroy_display_data(ddata);
            return NULL;
        }

        ddata->sw_buf[0] = buf1;
        ddata->sw_buf[1] = buf2;

        lv_display_set_buffers(display, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_DIRECT);
        lv_display_set_flush_cb(display, egl_flush_cb);
        lv_display_set_flush_wait_cb(display, flush_wait_cb);
        has_display = true;
        return ddata;
    }
#endif /*LV_WL_EGL_DMABUF_ENABLED*/

    lv_display_set_flush_cb(display, egl_flush_cb);
    lv_display_set_flush_wait_cb(display, flush_wait_cb);
    lv_display_set_render_mode(display, LV_USE_DRAW_NANOVG ? LV_DISPLAY_RENDER_MODE_FULL : LV_DISPLAY_RENDER_MODE_DIRECT);

    has_display = true;
    return ddata;
}

static void * wl_egl_resize_display(void * backend_ctx, lv_display_t * display)
{
    LV_UNUSED(backend_ctx);
    lv_wl_egl_display_data_t * ddata = lv_wayland_get_backend_display_data(display);

#if LV_WL_EGL_DMABUF_ENABLED
    if(ddata->use_dmabuf) {
        int32_t width = lv_display_get_original_horizontal_resolution(display);
        int32_t height = lv_display_get_original_vertical_resolution(display);

        lv_color_format_t cf = lv_display_get_color_format(display);
        uint32_t stride = lv_draw_buf_width_to_stride(width, cf);
        uint32_t buf_size = stride * height;

        uint8_t * buf1 = lv_malloc(buf_size);
        uint8_t * buf2 = lv_malloc(buf_size);
        LV_ASSERT_MALLOC(buf1);
        LV_ASSERT_MALLOC(buf2);
        if(!buf1 || !buf2) {
            LV_LOG_ERROR("Failed to allocate display buffer for %dx%d", width, height);
            lv_free(buf1);
            lv_free(buf2);
            lv_display_set_resolution(display, ddata->width, ddata->height);
            return ddata;
        }

        if(!egl_dmabuf_reinit_buffers(ddata, display, width, height)) {
            LV_LOG_ERROR("Failed to recreate DMA-BUF buffers for %dx%d", width, height);
            lv_free(buf1);
            lv_free(buf2);
            lv_display_set_resolution(display, ddata->width, ddata->height);
            return ddata;
        }

        lv_free(ddata->sw_buf[0]);
        lv_free(ddata->sw_buf[1]);
        ddata->sw_buf[0] = buf1;
        ddata->sw_buf[1] = buf2;

        lv_display_set_buffers(display, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_DIRECT);
        wl_egl_window_resize(ddata->egl_window, width, height, 0, 0);
        return ddata;
    }
#endif /*LV_WL_EGL_DMABUF_ENABLED*/

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

#if LV_WL_EGL_DMABUF_ENABLED
    if(ddata && ddata->use_dmabuf) {
        lv_free(ddata->sw_buf[0]);
        lv_free(ddata->sw_buf[1]);
        ddata->sw_buf[0] = NULL;
        ddata->sw_buf[1] = NULL;
    }
#endif

    egl_destroy_display_data(ddata);
    has_display = false;
}

static void wl_egl_global_handler(void * backend_ctx, struct wl_registry * registry, uint32_t name,
                                  const char * interface, uint32_t version)
{
#if LV_WL_EGL_DMABUF_ENABLED
    lv_wayland_dmabuf_registry_handle_global((lv_wayland_dmabuf_ctx_t *)backend_ctx,
                                             registry, name, interface, version);
#else
    LV_UNUSED(backend_ctx);
    LV_UNUSED(registry);
    LV_UNUSED(name);
    LV_UNUSED(interface);
    LV_UNUSED(version);

    /* No specific Wayland globals needed for basic EGL support */
#endif
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
        LV_LOG_WARN("Failed to get Wayland surface");
        return NULL;
    }

    ddata->egl_window = wl_egl_window_create(wl_surface,
                                             lv_display_get_horizontal_resolution(display),
                                             lv_display_get_vertical_resolution(display));
    if(!ddata->egl_window) {
        LV_LOG_WARN("Failed to create wl_egl_window");
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

/*==================================================================
 * DMA-BUF software fast-path
 *==================================================================*/

#if LV_WL_EGL_DMABUF_ENABLED

static bool dmabuf_is_available(void)
{
    if(!ctx.handler) {
        LV_LOG_WARN("Compositor does not support zwp_linux_dmabuf_v1");
        return false;
    }

#if LV_COLOR_DEPTH == 16
    if(!ctx.supports_rgb565) {
        LV_LOG_WARN("Compositor does not advertise DMA-BUF RGB565 support");
        return false;
    }
#endif

    return true;
}

static bool egl_dmabuf_setup(lv_wl_egl_display_data_t * ddata, lv_display_t * display, int32_t width, int32_t height)
{
    if(!dmabuf_is_available()) {
        return false;
    }

    /* Load glEGLImageTargetTexture2DOES */
    if(!glEGLImageTargetTexture2DOES) {
        load_egl_extensions();
    }
    if(!glEGLImageTargetTexture2DOES) {
        return false;
    }

    ddata->drm_fd = open_drm_device();
    if(ddata->drm_fd < 0) {
        LV_LOG_WARN("Failed to open DRM device");
        return false;
    }

    ddata->gbm_device = gbm_create_device(ddata->drm_fd);
    if(!ddata->gbm_device) {
        LV_LOG_WARN("Failed to create GBM device");
        close(ddata->drm_fd);
        ddata->drm_fd = -1;
        return false;
    }

    /* Buffers not yet reached by init_buffer() must still report a closed fd,
     * otherwise a teardown after a partial failure below would close fd 0. */
    for(size_t i = 0; i < LV_WL_EGL_BUF_COUNT; i++) {
        ddata->buffers[i].dmabuf_fd = -1;
    }

    lv_color_format_t cf = lv_display_get_color_format(display);
    bool ok = true;
    for(size_t i = 0; i < LV_WL_EGL_BUF_COUNT; i++) {
        if(!init_buffer(&ctx, &ddata->buffers[i], width, height, cf, ddata)) {
            ok = false;
        }
    }
    ddata->last_used = 0;

    wl_display_flush(lv_wl_ctx.wl_display);
    wl_display_roundtrip(lv_wl_ctx.wl_display);
    for(size_t i = 0; i < LV_WL_EGL_BUF_COUNT; ++i) {
        if(!ddata->buffers[i].base.wl_buffer) {
            ok = false;
        }
    }

    if(!ok) {
        LV_LOG_WARN("DMA-BUF creation failed");
        egl_dmabuf_teardown(ddata);
        return false;
    }

    ddata->width = width;
    ddata->height = height;
    return true;
}

static bool egl_dmabuf_reinit_buffers(lv_wl_egl_display_data_t * ddata, lv_display_t * display,
                                      int32_t width, int32_t height)
{
    lv_wl_buffer_t new_buffers[LV_WL_EGL_BUF_COUNT];
    lv_memset(new_buffers, 0, sizeof(new_buffers));
    for(size_t i = 0; i < LV_WL_EGL_BUF_COUNT; i++) {
        new_buffers[i].dmabuf_fd = -1;
    }

    lv_color_format_t cf = lv_display_get_color_format(display);
    bool ok = true;
    for(size_t i = 0; i < LV_WL_EGL_BUF_COUNT; i++) {
        if(!init_buffer(&ctx, &new_buffers[i], width, height, cf, ddata)) {
            ok = false;
        }
    }

    wl_display_flush(lv_wl_ctx.wl_display);
    wl_display_roundtrip(lv_wl_ctx.wl_display);
    for(size_t i = 0; i < LV_WL_EGL_BUF_COUNT; i++) {
        if(!new_buffers[i].base.wl_buffer) {
            ok = false;
        }
    }

    if(!ok) {
        for(int i = 0; i < LV_WL_EGL_BUF_COUNT; i++) {
            delete_buffer(ddata->egl_ctx, &new_buffers[i]);
        }
        return false;
    }

    /* All new buffers are ready: it is now safe to release the old ones. */
    for(int i = 0; i < LV_WL_EGL_BUF_COUNT; i++) {
        delete_buffer(ddata->egl_ctx, &ddata->buffers[i]);
    }
    lv_memcpy(ddata->buffers, new_buffers, sizeof(new_buffers));
    ddata->last_used = 0;
    ddata->width = width;
    ddata->height = height;

    for(int i = 0; i < LV_WL_EGL_BUF_COUNT; i++) {
        if(ddata->buffers[i].base.wl_buffer) {
            wl_buffer_set_user_data(ddata->buffers[i].base.wl_buffer, &ddata->buffers[i].base);
        }
    }

    return true;
}

static void egl_dmabuf_teardown(lv_wl_egl_display_data_t * ddata)
{
    for(int i = 0; i < LV_WL_EGL_BUF_COUNT; i++) {
        delete_buffer(ddata->egl_ctx, &ddata->buffers[i]);
    }

    if(ddata->gbm_device) {
        gbm_device_destroy(ddata->gbm_device);
        ddata->gbm_device = NULL;
    }
    if(ddata->drm_fd >= 0) {
        close(ddata->drm_fd);
        ddata->drm_fd = -1;
    }
}

static lv_wl_buffer_t * get_next_buffer(lv_wl_egl_display_data_t * ddata)
{
    /* Try to find a non-busy buffer. If all buffers are still owned by the
     * compositor (busy), wait for a wl_buffer.release event to clear one.
     * Bounded to 500ms so a compositor that never releases one drops this frame. */
    struct pollfd pfd = { .fd = wl_display_get_fd(lv_wl_ctx.wl_display), .events = POLLIN };
    uint32_t remaining_ms = 500;

    for(;;) {
        for(int i = 0; i < LV_WL_EGL_BUF_COUNT; i++) {
            int index = (ddata->last_used + i) % LV_WL_EGL_BUF_COUNT;
            if(!ddata->buffers[index].base.busy) {
                ddata->last_used = (index + 1) % LV_WL_EGL_BUF_COUNT;
                return &ddata->buffers[index];
            }
        }

        if(remaining_ms == 0) {
            LV_LOG_WARN("Timed out waiting for a free DMA-BUF buffer, dropping frame");
            return NULL;
        }

        /* All buffers are busy: flush pending requests and wait (bounded) for
         * the compositor to release one via a wl_buffer.release event. */
        wl_display_flush(lv_wl_ctx.wl_display);
        if(wl_display_prepare_read(lv_wl_ctx.wl_display) == 0) {
            int step_ms = remaining_ms < 20 ? (int)remaining_ms : 20;
            int ret = poll(&pfd, 1, step_ms);
            remaining_ms -= step_ms;
            if(ret > 0 && (pfd.revents & POLLIN)) {
                wl_display_read_events(lv_wl_ctx.wl_display);
            }
            else {
                wl_display_cancel_read(lv_wl_ctx.wl_display);
            }
        }
        wl_display_dispatch_pending(lv_wl_ctx.wl_display);
    }
}

static uint32_t lv_drm_cf_to_gbm_cf(uint32_t drm_cf)
{
    switch(drm_cf) {
        case DRM_FORMAT_XRGB8888:
            return GBM_FORMAT_XRGB8888;
        case DRM_FORMAT_ARGB8888:
            return GBM_FORMAT_ARGB8888;
        case DRM_FORMAT_RGB565:
            return GBM_FORMAT_RGB565;
        default:
            return GBM_FORMAT_ARGB8888;
    }
}

static void load_egl_extensions(void)
{
    glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)
                                   eglGetProcAddress("glEGLImageTargetTexture2DOES");

    if(!glEGLImageTargetTexture2DOES) {
        LV_LOG_WARN("Failed to load glEGLImageTargetTexture2DOES extension");
    }
    else {
        LV_LOG_INFO("Loaded glEGLImageTargetTexture2DOES extension");
    }
}

static int open_drm_device(void)
{
    drmDevicePtr devices[64];
    int num_devices, fd = -1;

    num_devices = drmGetDevices2(0, devices, 64);
    if(num_devices < 0) {
        LV_LOG_WARN("drmGetDevices2 failed: %s", strerror(-num_devices));
        return -1;
    }

    /* Try to find a primary GPU device */
    for(int i = 0; i < num_devices; i++) {
        drmDevicePtr device = devices[i];
        if(!(device->available_nodes & (1 << DRM_NODE_PRIMARY)))
            continue;

        fd = open(device->nodes[DRM_NODE_PRIMARY], O_RDWR);
        if(fd >= 0) {
            LV_LOG_INFO("Opened DRM device: %s", device->nodes[DRM_NODE_PRIMARY]);
            break;
        }
    }

    drmFreeDevices(devices, num_devices);
    if(fd < 0) {
        LV_LOG_WARN("Failed to open DRM device");
    }

    return fd;
}

static bool init_buffer(lv_wayland_dmabuf_ctx_t * dmabuf_ctx, lv_wl_buffer_t * buffer, uint32_t width,
                        uint32_t height, lv_color_format_t cf, lv_wl_egl_display_data_t * ddata)
{
    uint32_t gbm_flags = GBM_BO_USE_RENDERING | GBM_BO_USE_LINEAR;
    uint32_t drm_cf = lv_wayland_dmabuf_cf_to_drm(cf);
    uint32_t gbm_cf = lv_drm_cf_to_gbm_cf(drm_cf);

    buffer->base.busy = false;
    buffer->dmabuf_fd = -1;

    buffer->bo = gbm_bo_create(ddata->gbm_device, width, height, gbm_cf, gbm_flags);
    if(!buffer->bo) {
        LV_LOG_WARN("Failed to create GBM buffer object");
        return false;
    }

    buffer->stride = gbm_bo_get_stride_for_plane(buffer->bo, 0);
    buffer->offset = gbm_bo_get_offset(buffer->bo, 0);
    buffer->dmabuf_fd = gbm_bo_get_fd(buffer->bo);
    if(buffer->dmabuf_fd < 0) {
        LV_LOG_WARN("Failed to export GBM buffer object as a DMA-BUF fd");
        return false;
    }

    EGLint attribs[] = {
        EGL_WIDTH, width,
        EGL_HEIGHT, height,
        EGL_LINUX_DRM_FOURCC_EXT, (EGLint)drm_cf,
        EGL_DMA_BUF_PLANE0_FD_EXT, buffer->dmabuf_fd,
        EGL_DMA_BUF_PLANE0_OFFSET_EXT, (EGLint)buffer->offset,
        EGL_DMA_BUF_PLANE0_PITCH_EXT, (EGLint)buffer->stride,
        EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT, (EGLint)(DRM_FORMAT_MOD_LINEAR & 0xFFFFFFFF),
        EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT, (EGLint)(DRM_FORMAT_MOD_LINEAR >> 32),
        EGL_NONE
    };
    buffer->egl_image = eglCreateImageKHR(ddata->egl_ctx->egl_display, EGL_NO_CONTEXT,
                                          EGL_LINUX_DMA_BUF_EXT, NULL, attribs);
    if(buffer->egl_image == EGL_NO_IMAGE_KHR) {
        LV_LOG_WARN("Failed to create EGL image from DMA-BUF");
        return false;
    }

    /* Create texture from EGL image */
    GL_CALL(glGenTextures(1, &buffer->texture_id));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, buffer->texture_id));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, buffer->egl_image);
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

    /* The wl_buffer is shared with the compositor through the common DMA-BUF
     * helper; buffer->base.wl_buffer is filled in asynchronously. */
    lv_wayland_dmabuf_create_buffer(dmabuf_ctx, &buffer->base, buffer->dmabuf_fd,
                                    width, height, buffer->stride, buffer->offset, drm_cf);
    return true;
}

static void delete_buffer(lv_opengles_egl_t * egl_ctx, lv_wl_buffer_t * buffer)
{
    if(buffer->base.wl_buffer) {
        wl_buffer_destroy(buffer->base.wl_buffer);
        buffer->base.wl_buffer = NULL;
    }
    if(buffer->texture_id) {
        glDeleteTextures(1, &buffer->texture_id);
        buffer->texture_id = 0;
    }
    if(buffer->egl_image) {
        eglDestroyImageKHR(egl_ctx->egl_display, buffer->egl_image);
        buffer->egl_image = NULL;
    }
    if(buffer->bo) {
        gbm_bo_destroy(buffer->bo);
        buffer->bo = NULL;
    }
    if(buffer->dmabuf_fd >= 0) {
        close(buffer->dmabuf_fd);
        buffer->dmabuf_fd = -1;
    }
}

#endif /*LV_WL_EGL_DMABUF_ENABLED*/

#endif /*LV_WAYLAND_USE_EGL*/
