/**
 * @file lv_wl_egl_dmabuf_backend.c
 *
 */

/**
 * Copyright 2025 Renesas
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland_private.h"

#if LV_WAYLAND_USE_EGL == 1 && LV_USE_DRAW_OPENGLES == 0

#include <fcntl.h>
#include <string.h>
#include <xf86drmMode.h>
#include <stdlib.h>
#include <stdint.h>
#include <gbm.h>
#include <drm_fourcc.h>
#include <xf86drm.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include "../../display/lv_display_private.h"
#include "../opengles/lv_opengles_driver.h"
#include "../opengles/lv_opengles_texture_private.h"
#include "../opengles/lv_opengles_egl_private.h"
#include "../opengles/lv_opengles_debug.h"
#include "../opengles/lv_opengles_private.h"
#include <GLES2/gl2platform.h>
#include <GLES2/gl2ext.h>

#include <wayland-egl.h>
#include <wayland_linux_dmabuf.h>

/*********************
 *      DEFINES
 *********************/

#define LV_WL_EGL_BUF_COUNT 2

/**********************
 *      TYPEDEFS
 **********************/

typedef void (GL_APIENTRYP PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)(GLenum target, GLeglImageOES image);

typedef struct {
    struct gbm_bo * bo;
    int dmabuf_fd;
    uint32_t stride;
    uint32_t offset;

    EGLImageKHR egl_image;
    GLuint texture_id;

    struct wl_buffer * wl_buffer;
    bool busy;
} lv_wl_buffer_t;

typedef struct {
    struct zwp_linux_dmabuf_v1 * handler;
    /* XRBG888 and ARGB8888 are always supported*/
    bool supports_rgb565;
} lv_wl_egl_ctx_t;

typedef struct {
    struct wl_egl_window * egl_window;
    lv_opengles_egl_t * egl_ctx;
    bool delete_pending;

    lv_wl_buffer_t buffers[LV_WL_EGL_BUF_COUNT];
    uint32_t drm_cf;
    int drm_fd;
    struct gbm_device * gbm_device;
    uint8_t last_used;
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

static lv_egl_interface_t wl_egl_get_interface(lv_display_t * display);
static void * wl_egl_create_window(void * driver_data, const lv_egl_native_window_properties_t * properties);
static void wl_egl_destroy_window(void * driver_data, void * native_window);
static size_t wl_egl_select_config_cb(void * driver_data, const lv_egl_config_t * configs, size_t config_count);
static void wl_egl_flip_cb(void * driver_data, bool vsync);

static void egl_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void flush_wait_cb(lv_display_t * disp);
static void frame_done(void * data, struct wl_callback * callback, uint32_t time);

static lv_wl_egl_display_data_t * egl_create_display_data(lv_wl_egl_ctx_t * ctx, lv_display_t * display,
                                                          int32_t width, int32_t height);
static void egl_destroy_display_data(lv_wl_egl_display_data_t * ddata);

static inline void set_viewport(lv_display_t * display);

static void dmabuf_done(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback);
static void dmabuf_format_table(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback,
                                int32_t fd, uint32_t size);
static void dmabuf_main_device(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback,
                               struct wl_array * device);
static void dmabuf_tranche_done(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback);
static void dmabuf_tranche_target_device(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback,
                                         struct wl_array * device);
static void dmabuf_tranche_formats(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback,
                                   struct wl_array * indices);
static void dmabuf_tranche_flags(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback,
                                 uint32_t flags);
static void dmabuf_modifiers(void * data, struct zwp_linux_dmabuf_v1 * zwp_linux_dmabuf, uint32_t format,
                             uint32_t modifier_hi, uint32_t modifier_lo);
static void dmabuf_format(void * data, struct zwp_linux_dmabuf_v1 * zwp_linux_dmabuf, uint32_t format);

static void buffer_release(void * data, struct wl_buffer * buffer);

static void create_succeeded(void * data, struct zwp_linux_buffer_params_v1 * params, struct wl_buffer * new_buffer);
static void create_failed(void * data, struct zwp_linux_buffer_params_v1 * params);

static uint32_t lv_cf_to_drm_cf(lv_color_format_t cf);
static uint32_t lv_drm_cf_to_gbm_cf(uint32_t drm_cf);
static void load_egl_extensions(void);
static void init_buffer(lv_wl_egl_ctx_t * ctx, lv_wl_buffer_t * buffer, uint32_t width, uint32_t height,
                        lv_color_format_t cf, lv_wl_egl_display_data_t * ddata);
static int open_drm_device(void);

static void delete_buffer(lv_opengles_egl_t * egl_ctx, lv_wl_buffer_t * buffer);
static lv_wl_buffer_t * get_next_buffer(lv_wl_egl_display_data_t * ddata);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_wl_egl_ctx_t ctx;
static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = NULL;

const lv_wayland_backend_ops_t wl_backend_ops = {
    .init = wl_egl_init,
    .deinit = wl_egl_deinit,
    .global_handler = wl_egl_global_handler,
    .init_display = wl_egl_init_display,
    .deinit_display = wl_egl_deinit_display,
    .resize_display = wl_egl_resize_display,
};

static const struct zwp_linux_dmabuf_feedback_v1_listener dmabuf_listener_v5 = {
    .done          = dmabuf_done,
    .format_table  = dmabuf_format_table,
    .main_device   = dmabuf_main_device,
    .tranche_done  = dmabuf_tranche_done,
    .tranche_target_device = dmabuf_tranche_target_device,
    .tranche_formats = dmabuf_tranche_formats,
    .tranche_flags = dmabuf_tranche_flags,
};

static const struct wl_buffer_listener buffer_listener = {
    .release = buffer_release
};

static const struct zwp_linux_dmabuf_v1_listener dmabuf_listener_v3 = {
    .format   = dmabuf_format,
    .modifier = dmabuf_modifiers
};

static const struct zwp_linux_dmabuf_v1_listener dmabuf_listener = {
    .format = dmabuf_format
};

static const struct zwp_linux_buffer_params_v1_listener params_listener = {
    .created = create_succeeded,
    .failed  = create_failed
};

static const struct wl_callback_listener frame_listener = {
    .done = frame_done,
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
    lv_memset(&ctx, 0, sizeof(ctx));
    return &ctx;
}

static void wl_egl_deinit(void * backend_ctx)
{
    lv_wl_egl_ctx_t * ctx = (lv_wl_egl_ctx_t *)backend_ctx;
    if(!ctx) {
        return;
    }
    if(ctx->handler) {
        zwp_linux_dmabuf_v1_destroy(ctx->handler);
    }
}

static lv_wl_egl_display_data_t * egl_create_display_data(lv_wl_egl_ctx_t * ctx, lv_display_t * display,
                                                          int32_t width, int32_t height)
{
    lv_wl_egl_display_data_t * ddata = lv_zalloc(sizeof(*ddata));
    LV_ASSERT_MALLOC(ddata);
    if(!ddata) {
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

    /* Load glEGLimage */
    if(!glEGLImageTargetTexture2DOES) {
        load_egl_extensions();
    }

    ddata->drm_fd = open_drm_device();
    if(ddata->drm_fd < 0) {
        LV_LOG_ERROR("Failed to open DRM device");
        goto texture_err;
    }

    ddata->gbm_device = gbm_create_device(ddata->drm_fd);
    if(!ddata->gbm_device) {
        LV_LOG_ERROR("Failed to create GBM device");
        close(ddata->drm_fd);
        ddata->drm_fd = -1;
        goto texture_err;
    }

    lv_color_format_t cf = lv_display_get_color_format(display);
    ddata->drm_cf = lv_cf_to_drm_cf(cf);
    for(size_t i = 0; i < LV_WL_EGL_BUF_COUNT; i++) {
        init_buffer(ctx, &ddata->buffers[i], width, height, cf, ddata);
    }

    ddata->last_used = 0;

    wl_display_flush(lv_wl_ctx.wl_display);
    wl_display_roundtrip(lv_wl_ctx.wl_display);
    for(size_t i = 0; i < LV_WL_EGL_BUF_COUNT; ++i) {
        if(!ddata->buffers[i].wl_buffer) {
            egl_destroy_display_data(ddata);
            LV_LOG_ERROR("DMABUF creation failed");
            return NULL;
        }
    }

    return ddata;

texture_err:
    lv_opengles_egl_context_destroy(ddata->egl_ctx);
egl_ctx_err:
    lv_free(ddata);
    return NULL;
}

static void egl_destroy_display_data(lv_wl_egl_display_data_t * ddata)
{
    if(!ddata) {
        return;
    }

    for(int i = 0; i < LV_WL_EGL_BUF_COUNT; i++) {
        delete_buffer(ddata->egl_ctx, &ddata->buffers[i]);
    }

    if(ddata->egl_ctx) {
        lv_opengles_egl_context_destroy(ddata->egl_ctx);
        ddata->egl_ctx = NULL;
    }

    if(ddata->gbm_device) {
        gbm_device_destroy(ddata->gbm_device);
        ddata->gbm_device = NULL;
    }
    if(ddata->drm_fd >= 0) {
        close(ddata->drm_fd);
        ddata->drm_fd = -1;
    }

    LV_LOG_INFO("Deleted EGL display data");
    lv_free(ddata);
}

static lv_wl_buffer_t * get_next_buffer(lv_wl_egl_display_data_t * ddata)
{
    /* First, try to find a non-busy buffer */
    for(int i = 0; i < LV_WL_EGL_BUF_COUNT; i++) {
        int index = (ddata->last_used + i) % LV_WL_EGL_BUF_COUNT;
        if(!ddata->buffers[index].busy) {
            ddata->last_used = (index + 1) % LV_WL_EGL_BUF_COUNT;
            return &ddata->buffers[index];
        }
    }

    lv_wl_buffer_t * ret = &ddata->buffers[ddata->last_used];
    ddata->last_used = (ddata->last_used + 1) % LV_WL_EGL_BUF_COUNT;
    return ret;
}

static void flush_wait_cb(lv_display_t * disp)
{
    while(disp->flushing) {
        wl_display_dispatch(lv_wl_ctx.wl_display);
    }
}

static inline void set_viewport(lv_display_t * display)
{
    lv_opengles_viewport(0, 0, lv_display_get_original_horizontal_resolution(display),
                         lv_display_get_original_vertical_resolution(display));
}

static void egl_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);

    if(!lv_display_flush_is_last(disp)) {
        lv_display_flush_ready(disp);
        return;
    }

    lv_wl_egl_display_data_t * ddata = lv_wayland_get_backend_display_data(disp);
    int32_t disp_width = lv_display_get_horizontal_resolution(disp);
    int32_t disp_height = lv_display_get_vertical_resolution(disp);
    lv_wl_buffer_t * buf = get_next_buffer(ddata);
    if(!buf) {
        LV_LOG_ERROR("Failed to acquire a wayland window body buffer");
        lv_display_flush_ready(disp);
        return;
    }

    struct wl_surface * surface = lv_wayland_get_window_surface(disp);

    GL_CALL(glBindTexture(GL_TEXTURE_2D, buf->texture_id));

#if LV_COLOR_DEPTH == 16
    GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, disp_width, disp_height,
                            GL_RGB, GL_UNSIGNED_SHORT_5_6_5, px_map));
#elif LV_COLOR_DEPTH == 32
    GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, disp_width, disp_height,
                            GL_BGRA, GL_UNSIGNED_BYTE, px_map));
#endif
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

    wl_surface_attach(surface, buf->wl_buffer, 0, 0);
    wl_surface_damage(surface, 0, 0, disp_width, disp_height);

    struct wl_callback * callback = wl_surface_frame(surface);
    wl_callback_add_listener(callback, &frame_listener, disp);

    wl_surface_commit(surface);

    buf->busy = true;
    wl_display_flush(lv_wl_ctx.wl_display);
    return;
}

static void * wl_egl_init_display(void * backend_ctx, lv_display_t * display, int32_t width, int32_t height)
{
    lv_wl_egl_ctx_t * ctx = (lv_wl_egl_ctx_t *)backend_ctx;
    lv_wl_egl_display_data_t * ddata = egl_create_display_data(ctx, display, width, height);
    if(!ddata) {
        LV_LOG_ERROR("Failed to create display data");
        return NULL;
    }

    lv_color_format_t cf = lv_display_get_color_format(display);
    uint32_t stride = lv_draw_buf_width_to_stride(width, cf);
    uint32_t buf_size = stride * height;

    uint8_t * buf1 = lv_malloc(buf_size);
    uint8_t * buf2 = lv_malloc(buf_size);
    LV_ASSERT_MALLOC(buf1);
    LV_ASSERT_MALLOC(buf2);
    if(!buf1 || !buf2) {
        LV_LOG_ERROR("Failed to allocate display buffer");
        egl_destroy_display_data(ddata);
        return NULL;
    }

    lv_display_set_buffers(display, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(display, egl_flush_cb);
    lv_display_set_flush_wait_cb(display, flush_wait_cb);

    return ddata;
}

static void * wl_egl_resize_display(void * backend_ctx, lv_display_t * display)
{
    lv_wl_egl_ctx_t * ctx = (lv_wl_egl_ctx_t *)backend_ctx;
    int32_t width = lv_display_get_original_horizontal_resolution(display);
    int32_t height = lv_display_get_original_vertical_resolution(display);

    lv_wl_egl_display_data_t * ddata = egl_create_display_data(ctx, display, width, height);
    if(!ddata) {
        LV_LOG_ERROR("Failed to create DMABUF buffers for %dx%d", width, height);
        return NULL;
    }

    lv_color_format_t cf = lv_display_get_color_format(display);
    uint32_t stride = lv_draw_buf_width_to_stride(width, cf);
    uint32_t buf_size = stride * height;

    uint8_t * buf1 = lv_malloc(buf_size);
    uint8_t * buf2 = lv_malloc(buf_size);
    LV_ASSERT_MALLOC(buf1);
    LV_ASSERT_MALLOC(buf2);
    if(!buf1 || !buf2) {
        LV_LOG_ERROR("Failed to allocate display buffer");
        egl_destroy_display_data(ddata);
        return NULL;
    }

    void * old_buf1 = lv_display_get_buf_active(display);
    void * old_buf2 = display->buf_2;
    lv_free(old_buf1);
    lv_free(old_buf2);

    lv_display_set_buffers(display, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_DIRECT);

    lv_wl_egl_display_data_t * old_ddata = lv_wayland_get_backend_display_data(display);
    egl_destroy_display_data(old_ddata);

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
    lv_wl_egl_ctx_t * ctx = (lv_wl_egl_ctx_t *)backend_ctx;

    if(lv_streq(interface, zwp_linux_dmabuf_v1_interface.name)) {
        ctx->handler = wl_registry_bind(registry, name, &zwp_linux_dmabuf_v1_interface, version);

        if(version >= 4) {
            struct zwp_linux_dmabuf_feedback_v1 * feedback = zwp_linux_dmabuf_v1_get_default_feedback(ctx->handler);
            zwp_linux_dmabuf_feedback_v1_add_listener(feedback, &dmabuf_listener_v5, ctx);
        }
        else if(version < 3) {
            zwp_linux_dmabuf_v1_add_listener(ctx->handler, &dmabuf_listener, ctx);
        }
        else if(version == 3) {
            zwp_linux_dmabuf_v1_add_listener(ctx->handler, &dmabuf_listener_v3, ctx);
        }
        wl_display_roundtrip(lv_wl_ctx.wl_display);
    }
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
        if(configs[i].max_width >= target_w &&
           configs[i].max_height >= target_h &&
           config_cf == target_cf &&
           configs[i].surface_type & EGL_WINDOW_BIT) {
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

static uint32_t lv_cf_to_drm_cf(lv_color_format_t cf)
{
    if(cf == LV_COLOR_FORMAT_UNKNOWN) {
        return DRM_FORMAT_ARGB8888; /* Default to ARGB8888 */
    }

    switch(cf) {
        case LV_COLOR_FORMAT_XRGB8888:
            return DRM_FORMAT_XRGB8888;
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            return DRM_FORMAT_ARGB8888;
        case LV_COLOR_FORMAT_RGB565:
            return DRM_FORMAT_RGB565;
        default:
            return DRM_FORMAT_ARGB8888;
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
        LV_LOG_ERROR("Failed to load glEGLImageTargetTexture2DOES extension");
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
        LV_LOG_ERROR("drmGetDevices2 failed: %s", strerror(-num_devices));
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
        LV_LOG_ERROR("Failed to open DRM device");
    }

    return fd;
}

static void init_buffer(lv_wl_egl_ctx_t * ctx, lv_wl_buffer_t * buffer, uint32_t width, uint32_t height,
                        lv_color_format_t cf, lv_wl_egl_display_data_t * ddata)
{
    uint32_t gbm_flags = GBM_BO_USE_RENDERING | GBM_BO_USE_LINEAR;
    uint32_t drm_cf = lv_cf_to_drm_cf(cf);
    uint32_t gbm_cf = lv_drm_cf_to_gbm_cf(drm_cf);

    buffer->bo = gbm_bo_create(ddata->gbm_device, width, height, gbm_cf, gbm_flags);
    buffer->stride = gbm_bo_get_stride_for_plane(buffer->bo, 0);
    buffer->offset = gbm_bo_get_offset(buffer->bo, 0);
    buffer->dmabuf_fd = gbm_bo_get_fd(buffer->bo);
    buffer->busy = false;

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

    /* Create texture from EGL image */
    GL_CALL(glGenTextures(1, &buffer->texture_id));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, buffer->texture_id));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, buffer->egl_image);
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

    /* Will be set on the dmabuf callback if the creation is successful*/
    buffer->wl_buffer = NULL;

    struct zwp_linux_buffer_params_v1 * params = zwp_linux_dmabuf_v1_create_params(ctx->handler);

    zwp_linux_buffer_params_v1_add(params, buffer->dmabuf_fd, 0,
                                   buffer->offset, buffer->stride, 0, 0);

    zwp_linux_buffer_params_v1_add_listener(params, &params_listener, buffer);
    zwp_linux_buffer_params_v1_create(params, width, height, drm_cf, 0);
}

static void delete_buffer(lv_opengles_egl_t * egl_ctx, lv_wl_buffer_t * buffer)
{
    if(buffer->wl_buffer) {
        wl_buffer_destroy(buffer->wl_buffer);
        buffer->wl_buffer = NULL;
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

static void buffer_release(void * data, struct wl_buffer * buffer)
{
    LV_LOG_TRACE("Buffer released");
    LV_UNUSED(buffer);
    lv_wl_buffer_t * buf = data;
    buf->busy = false;
}


static void create_succeeded(void * data, struct zwp_linux_buffer_params_v1 * params, struct wl_buffer * new_buffer)
{
    LV_LOG_TRACE("Buffer created successfuly");
    lv_wl_buffer_t * buffer = data;
    buffer->wl_buffer = new_buffer;

    /* When not using explicit synchronization listen to wl_buffer.release
     * for release notifications, otherwise we are going to use
     * zwp_linux_buffer_release_v1. */
    wl_buffer_add_listener(buffer->wl_buffer, &buffer_listener, buffer);

    zwp_linux_buffer_params_v1_destroy(params);
}

static void create_failed(void * data, struct zwp_linux_buffer_params_v1 * params)
{
    lv_wl_buffer_t * buffer = data;
    buffer->wl_buffer = NULL;
    zwp_linux_buffer_params_v1_destroy(params);
    LV_LOG_ERROR("Failed to create dmabuf buffer\n");
}

static void dmabuf_format_table(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback,
                                int32_t fd, uint32_t size)
{
    lv_wl_egl_ctx_t * ctx = data;

    LV_UNUSED(zwp_linux_dmabuf_feedback);

    if(fd < 0 || size == 0) {
        LV_LOG_ERROR("Invalid format table fd=%d size=%u", fd, size);
        return;
    }

    /* Map the format table file descriptor */
    void * table = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(table == MAP_FAILED) {
        LV_LOG_ERROR("Failed to mmap format table: %s", strerror(errno));
        close(fd);
        return;
    }

    LV_LOG_TRACE("Received format table with fd %d and size %u", fd, size);

    /* Parse the format table - each entry is 16 bytes: 4 bytes format + 4 bytes padding + 8 bytes modifier */
    size_t num_formats = size / 16;
    uint32_t * formats = (uint32_t *)table;

    for(size_t i = 0; i < num_formats; i++) {
        /* Each entry is 4 uint32_t words */
        uint32_t format = formats[i * 4];
        if(format == DRM_FORMAT_RGB565) {
            ctx->supports_rgb565 = true;
        }
    }

    /* Clean up */
    munmap(table, size);
    close(fd);
}

static void dmabuf_done(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback)
{
    lv_wl_egl_ctx_t * ctx = data;

    LV_UNUSED(zwp_linux_dmabuf_feedback);
    LV_UNUSED(ctx);

    LV_LOG_TRACE("DMABUF feedback done");

    /* This event marks the end of a feedback round. The client has received
     * all the format and modifier pairs from all tranches. This allows
     * the client to proceed with buffer allocation. */
}

static void dmabuf_main_device(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback,
                               struct wl_array * device)
{
    LV_UNUSED(data);
    LV_UNUSED(zwp_linux_dmabuf_feedback);
    LV_UNUSED(device);

    LV_LOG_TRACE("DMABUF main device received (size: %zu)", device->size);

    /* This event advertises the main device that the server-side allocator
     * will use for scanout. It should be used by clients as a hint for
     * buffer allocation. */
}

static void dmabuf_tranche_done(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback)
{
    LV_UNUSED(data);
    LV_UNUSED(zwp_linux_dmabuf_feedback);

    LV_LOG_TRACE("DMABUF tranche done");

    /* This event marks the end of a tranche. This allows the client to
     * process the formats and modifiers it has received for this tranche. */
}

static void dmabuf_tranche_target_device(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback,
                                         struct wl_array * device)
{
    LV_UNUSED(data);
    LV_UNUSED(zwp_linux_dmabuf_feedback);
    LV_UNUSED(device);

    LV_LOG_TRACE("DMABUF tranche target device (size: %zu)", device->size);

    /* This event advertises the target device that the following tranche
     * will apply to. */
}

static void dmabuf_tranche_formats(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback,
                                   struct wl_array * indices)
{
    LV_UNUSED(data);
    LV_UNUSED(zwp_linux_dmabuf_feedback);

    LV_LOG_TRACE("DMABUF tranche formats (count: %zu)", indices->size / sizeof(uint16_t));

    /* This event advertises the format + modifier pairs that the compositor
     * supports for the current tranche. The indices are offsets into the
     * format table sent earlier. */

    if(indices->size > 0) {
        /* If we don't have a format yet, we could parse the indices here
         * to find a suitable format from the format table, but for now
         * we rely on the format_table callback to set a format directly */
        LV_LOG_TRACE("Format indices received");
    }
}

static void dmabuf_tranche_flags(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback,
                                 uint32_t flags)
{
    LV_UNUSED(data);
    LV_UNUSED(zwp_linux_dmabuf_feedback);
    LV_UNUSED(flags);

    LV_LOG_TRACE("DMABUF tranche flags: 0x%x", flags);

    /* This event advertises the flags for the current tranche.
     * Flags can indicate special properties like scanout support. */
}

static void dmabuf_modifiers(void * data, struct zwp_linux_dmabuf_v1 * zwp_linux_dmabuf, uint32_t format,
                             uint32_t modifier_hi, uint32_t modifier_lo)
{
    LV_UNUSED(modifier_hi);
    LV_UNUSED(modifier_lo);
    dmabuf_format(data, zwp_linux_dmabuf, format);
}

static void dmabuf_format(void * data, struct zwp_linux_dmabuf_v1 * zwp_linux_dmabuf, uint32_t format)
{
    lv_wl_egl_ctx_t * ctx = data;

    LV_UNUSED(zwp_linux_dmabuf);
    if(format == DRM_FORMAT_RGB565) {
        ctx->supports_rgb565 = true;
    }
}

#endif /*LV_WAYLAND_USE_EGL*/