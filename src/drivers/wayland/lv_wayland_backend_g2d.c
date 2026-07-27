/**
 * @file lv_wayland_backend_g2d.c
 *
 */


/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland_private.h"

#if LV_WAYLAND_USE_G2D

#include "lv_wayland_dmabuf.h"
#include "../../display/lv_display_private.h"
#include <stdlib.h>
#include <unistd.h>
#include "../../draw/nxp/g2d/lv_g2d_utils.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/

#define LV_WL_G2D_BUF_COUNT 2

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    /* Shared DMA-BUF protocol state (wl_buffer + release tracking) */
    lv_wayland_dmabuf_buffer_t base;

    /* G2D allocator specific state */
    lv_draw_buf_t * lv_draw_buf;
    int dmabuf_fd;
    uint32_t stride;
    uint32_t offset;
} lv_wl_buffer_t;

typedef struct {
    lv_wl_buffer_t buffers[LV_WL_G2D_BUF_COUNT];
    /* Use a separate buffer if rotation is enabled
     * Let LVGL render to this buffer and then copy it
     * and rotate it to one off the two main buffers*/
    lv_wl_buffer_t rotate_buffer;
    uint32_t drm_cf;
    uint8_t last_used;
    bool flushing;
} lv_wl_g2d_display_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * wl_g2d_init(void);
static void wl_g2d_deinit(void * backend_ctx);
static void wl_g2d_global_handler(void * backend_ctx, struct wl_registry * registry, uint32_t name,
                                  const char * interface, uint32_t version);


static void * wl_g2d_init_display(void * backend_ctx, lv_display_t * display, int32_t width, int32_t height);
static void * wl_g2d_resize_display(void * backend_ctx, lv_display_t * display);
static void wl_g2d_deinit_display(void * backend_ctx, lv_display_t * display);

static lv_wl_g2d_display_data_t * wl_g2d_create_display_data(lv_wayland_dmabuf_ctx_t * ctx, lv_display_t * display,
                                                             int32_t width, int32_t height);

static void wl_g2d_delete_display_data(lv_wl_g2d_display_data_t * ddata);

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);

static void frame_done(void * data, struct wl_callback * callback, uint32_t time);

static void init_buffer(lv_wayland_dmabuf_ctx_t * ctx, lv_wl_buffer_t * buffer, uint32_t width, uint32_t height,
                        lv_color_format_t cf);

static void delete_buffer(lv_wl_buffer_t * buffer);
static void flush_wait_cb(lv_display_t * disp);

static lv_wl_buffer_t * get_next_buffer(lv_wl_g2d_display_data_t * ddata);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_wayland_dmabuf_ctx_t ctx;

const lv_wayland_backend_ops_t wl_backend_ops = {
    .init = wl_g2d_init,
    .deinit = wl_g2d_deinit,
    .global_handler = wl_g2d_global_handler,
    .init_display =   wl_g2d_init_display,
    .deinit_display = wl_g2d_deinit_display,
    .resize_display = wl_g2d_resize_display,
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


static void * wl_g2d_init(void)
{
    lv_wayland_dmabuf_ctx_init(&ctx);
    return &ctx;
}

static void wl_g2d_deinit(void * backend_ctx)
{
    lv_wayland_dmabuf_ctx_deinit((lv_wayland_dmabuf_ctx_t *)backend_ctx);
}


static void wl_g2d_global_handler(void * backend_ctx, struct wl_registry * registry, uint32_t name,
                                  const char * interface, uint32_t version)
{
    lv_wayland_dmabuf_registry_handle_global((lv_wayland_dmabuf_ctx_t *)backend_ctx,
                                             registry, name, interface, version);
}

static void init_buffer(lv_wayland_dmabuf_ctx_t * ctx, lv_wl_buffer_t * buffer, uint32_t width, uint32_t height,
                        lv_color_format_t cf)
{
    uint32_t drm_cf = lv_wayland_dmabuf_cf_to_drm(cf);
    uint32_t stride = lv_draw_buf_width_to_stride(width, cf);
    buffer->lv_draw_buf = lv_draw_buf_create(width, height, cf, stride);
    buffer->dmabuf_fd = g2d_get_buf_fd(buffer->lv_draw_buf);
    buffer->stride = stride;
    buffer->offset = 0;
    buffer->base.busy = false;

    /* The wl_buffer is shared with the compositor through the common DMA-BUF
     * helper; buffer->base.wl_buffer is filled in asynchronously. */
    lv_wayland_dmabuf_create_buffer(ctx, &buffer->base, buffer->dmabuf_fd,
                                    width, height, buffer->stride, buffer->offset, drm_cf);
}

static void delete_buffer(lv_wl_buffer_t * buffer)
{
    if(buffer->base.wl_buffer) {
        wl_buffer_destroy(buffer->base.wl_buffer);
        buffer->base.wl_buffer = NULL;
    }
    if(buffer->lv_draw_buf) {
        lv_draw_buf_destroy(buffer->lv_draw_buf);
        buffer->lv_draw_buf = NULL;
    }
}

static lv_wl_g2d_display_data_t * wl_g2d_create_display_data(lv_wayland_dmabuf_ctx_t * ctx, lv_display_t * display,
                                                             int32_t width, int32_t height)
{
    lv_wl_g2d_display_data_t * ddata = lv_zalloc(sizeof(*ddata));
    LV_ASSERT_MALLOC(ddata);
    if(!ddata) {
        return NULL;
    }

    lv_color_format_t cf = lv_display_get_color_format(display);
    if(cf == LV_COLOR_FORMAT_RGB565 && !ctx->supports_rgb565) {
        LV_LOG_WARN("RGB565 is not supported by the wayland compositor. Falling back to XRGB8888");
        cf = LV_COLOR_FORMAT_XRGB8888;
        lv_display_set_color_format(display, cf);
    }

    ddata->drm_cf = lv_wayland_dmabuf_cf_to_drm(cf);
    for(size_t i = 0; i < LV_WL_G2D_BUF_COUNT; i++) {
        init_buffer(ctx, &ddata->buffers[i], width, height, cf);
    }

    lv_display_rotation_t rotation = lv_display_get_rotation(display);
    if(rotation == LV_DISPLAY_ROTATION_90 || rotation == LV_DISPLAY_ROTATION_270) {
        init_buffer(ctx, &ddata->rotate_buffer, height, width, cf);
    }
    else if(rotation == LV_DISPLAY_ROTATION_180) {
        init_buffer(ctx, &ddata->rotate_buffer, width, height, cf);
    }

    wl_display_flush(lv_wl_ctx.wl_display);
    wl_display_roundtrip(lv_wl_ctx.wl_display);
    for(size_t i = 0; i < LV_WL_G2D_BUF_COUNT; ++i) {
        if(!ddata->buffers[i].base.wl_buffer) {
            wl_g2d_delete_display_data(ddata);
            LV_LOG_ERROR("DMABUF creation failed");
            return NULL;
        }
    }

    if(rotation == LV_DISPLAY_ROTATION_0) {
        lv_display_set_draw_buffers(display, ddata->buffers[0].lv_draw_buf, ddata->buffers[1].lv_draw_buf);
        return ddata;
    }

    /*rotation != 0 so use a separate buffer for rendering and two other for flushing*/
    if(!ddata->rotate_buffer.base.wl_buffer) {
        wl_g2d_delete_display_data(ddata);
        LV_LOG_ERROR("DMABUF creation failed");
        return NULL;
    }
    lv_display_set_draw_buffers(display, ddata->rotate_buffer.lv_draw_buf, NULL);

    return ddata;
}

static void wl_g2d_delete_display_data(lv_wl_g2d_display_data_t * ddata)
{
    for(int i = 0; i < LV_WL_G2D_BUF_COUNT; i++) {
        delete_buffer(ddata->buffers + i);
    }

    delete_buffer(&ddata->rotate_buffer);

    lv_free(ddata);
}

static void * wl_g2d_init_display(void * backend_ctx, lv_display_t * display, int32_t width, int32_t height)
{

    lv_wayland_dmabuf_ctx_t * ctx = (lv_wayland_dmabuf_ctx_t *)backend_ctx;
    lv_wl_g2d_display_data_t * ddata = wl_g2d_create_display_data(ctx, display, width, height);
    if(!ddata) {
        LV_LOG_ERROR("Failed to create display data");
        return NULL;
    }

    lv_display_set_flush_cb(display, flush_cb);
    lv_display_set_flush_wait_cb(display, flush_wait_cb);
    lv_display_set_render_mode(display, LV_DISPLAY_RENDER_MODE_DIRECT);
    return ddata;
}

static void frame_done(void * data, struct wl_callback * callback, uint32_t time)
{
    LV_LOG_TRACE("Frame done");
    LV_UNUSED(time);
    lv_display_t * display = data;
    wl_callback_destroy(callback);
    lv_display_flush_ready(display);
}

static void * wl_g2d_resize_display(void * backend_ctx, lv_display_t * disp)
{
    lv_wayland_dmabuf_ctx_t * ctx = (lv_wayland_dmabuf_ctx_t *)backend_ctx;
    int32_t width = lv_display_get_original_horizontal_resolution(disp);
    int32_t height = lv_display_get_original_vertical_resolution(disp);

    lv_wl_g2d_display_data_t * ddata = wl_g2d_create_display_data(ctx, disp, width, height);
    if(!ddata) {
        LV_LOG_ERROR("Failed to create DMABUF buffers for %dx%d", width, height);
        return NULL;
    }

    lv_wl_g2d_display_data_t * old_ddata = lv_wayland_get_backend_display_data(disp);
    wl_g2d_delete_display_data(old_ddata);
    return ddata;
}

static void wl_g2d_deinit_display(void * backend_ctx, lv_display_t * display)
{
    LV_UNUSED(backend_ctx);
    lv_wl_g2d_display_data_t * ddata = lv_wayland_get_backend_display_data(display);
    if(!ddata) {
        return;
    }
    wl_g2d_delete_display_data(ddata);
}

static lv_wl_buffer_t * get_next_buffer(lv_wl_g2d_display_data_t * ddata)
{
    lv_wl_buffer_t * ret =  &ddata->buffers[ddata->last_used];
    if(ret->base.busy) {
        /* In theory this should never happen, log a warning in case it does */
        LV_LOG_WARN("Failed to acquire a non-busy buffer");
    }
    ddata->last_used = (ddata->last_used + 1) % (LV_WL_G2D_BUF_COUNT);
    return ret;
}

static void flush_wait_cb(lv_display_t * disp)
{
    while(disp->flushing) {
        wl_display_dispatch(lv_wl_ctx.wl_display);
    }
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, unsigned char * color_p)
{

    LV_UNUSED(color_p);
    lv_wl_g2d_display_data_t * ddata = lv_wayland_get_backend_display_data(disp);
    int32_t src_width = lv_area_get_width(area);
    int32_t src_height = lv_area_get_height(area);
    lv_display_rotation_t rotation = lv_display_get_rotation(disp);

    struct wl_surface * surface = lv_wayland_get_window_surface(disp);
    /* Mark surface damage */
    wl_surface_damage(surface, area->x1, area->y1, src_width, src_height);

    if(!lv_display_flush_is_last(disp)) {
        lv_display_flush_ready(disp);
        return;
    }

    lv_wl_buffer_t * buf = get_next_buffer(ddata);

    if(!buf) {
        LV_LOG_ERROR("Failed to acquire a wayland window body buffer");
        return;
    }

    lv_draw_buf_invalidate_cache(buf->lv_draw_buf, NULL);

    /*Rerender the whole surface if we're using rotation*/
    if(rotation != LV_DISPLAY_ROTATION_0) {
        wl_surface_damage(surface, 0, 0,
                          lv_display_get_original_horizontal_resolution(disp),
                          lv_display_get_original_vertical_resolution(disp));

        lv_draw_buf_invalidate_cache(ddata->rotate_buffer.lv_draw_buf, NULL);
        g2d_rotate(ddata->rotate_buffer.lv_draw_buf, buf->lv_draw_buf,
                   lv_display_get_original_horizontal_resolution(disp),
                   lv_display_get_original_vertical_resolution(disp),
                   lv_display_get_rotation(disp),
                   lv_display_get_color_format(disp));

    }

    /* Finally, attach buffer and commit to surface */
    struct wl_callback * cb = wl_surface_frame(surface);
    wl_callback_add_listener(cb, &frame_listener, disp);

    wl_surface_attach(surface, buf->base.wl_buffer, 0, 0);
    wl_surface_commit(surface);

    buf->base.busy = true;
    return;
}

#endif /*LV_USE_WAYLAND_G2D*/
