
/**
 * @file lv_wl_g2d_backend.c
 *
 */


/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland_private.h"

#if LV_WAYLAND_USE_G2D

#include "../../display/lv_display_private.h"
#include <wayland_linux_dmabuf.h>
#include <drm/drm_fourcc.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../misc/lv_types.h"
#include <string.h>
#include "../../draw/nxp/g2d/lv_g2d_utils.h"
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

/*********************
 *      DEFINES
 *********************/

#define LV_WL_G2D_BUF_COUNT 2

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    struct wl_buffer * wl_buffer;
    lv_draw_buf_t * lv_draw_buf;
    int dmabuf_fd;
    uint32_t stride;
    uint32_t offset;
    bool busy;
} lv_wl_buffer_t;

typedef struct {
    lv_wl_buffer_t buffers[LV_WL_G2D_BUF_COUNT];
#if LV_USE_ROTATE_G2D
    lv_wl_buffer_t rotate_buffer;
#endif
    uint32_t drm_cf;
    uint8_t last_used;
    bool flushing;
} lv_wl_g2d_display_data_t;

typedef struct {
    struct zwp_linux_dmabuf_v1 * handler;
    /* XRBG888 and ARGB8888 are always supported*/
    bool supports_rgb565;
} lv_wl_g2d_ctx_t;

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

static lv_wl_g2d_display_data_t * wl_g2d_create_display_data(lv_wl_g2d_ctx_t * ctx, lv_display_t * display,
                                                             int32_t width, int32_t height);

static void wl_g2d_delete_display_data(lv_wl_g2d_display_data_t * ddata);

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);


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

static void frame_done(void * data, struct wl_callback * callback, uint32_t time);

static void init_buffer(lv_wl_g2d_ctx_t * ctx, lv_wl_buffer_t * buffer, uint32_t width, uint32_t height,
                        lv_color_format_t cf);

static void delete_buffer(lv_wl_buffer_t * buffer);
static void flush_wait_cb(lv_display_t * disp);

static lv_wl_buffer_t * get_next_buffer(lv_wl_g2d_display_data_t * ddata);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_wl_g2d_ctx_t ctx;

const lv_wayland_backend_ops_t wl_backend_ops = {
    .init = wl_g2d_init,
    .deinit = wl_g2d_deinit,
    .global_handler = wl_g2d_global_handler,
    .init_display =   wl_g2d_init_display,
    .deinit_display = wl_g2d_deinit_display,
    .resize_display = wl_g2d_resize_display,
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


static void * wl_g2d_init(void)
{
    lv_memset(&ctx, 0, sizeof(ctx));
    return &ctx;
}

static void wl_g2d_deinit(void * backend_ctx)
{
    lv_wl_g2d_ctx_t * ctx = (lv_wl_g2d_ctx_t *)backend_ctx;
    if(!ctx) {
        return;
    }
    if(ctx->handler) {
        zwp_linux_dmabuf_v1_destroy(ctx->handler);
    }
}


static void wl_g2d_global_handler(void * backend_ctx, struct wl_registry * registry, uint32_t name,
                                  const char * interface, uint32_t version)
{

    LV_UNUSED(version);
    lv_wl_g2d_ctx_t * ctx = (lv_wl_g2d_ctx_t *)backend_ctx;

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
static void init_buffer(lv_wl_g2d_ctx_t * ctx, lv_wl_buffer_t * buffer, uint32_t width, uint32_t height,
                        lv_color_format_t cf)
{
    uint32_t drm_cf = lv_cf_to_drm_cf(cf);
    uint32_t stride = lv_draw_buf_width_to_stride(width, cf);
    buffer->lv_draw_buf = lv_draw_buf_create(width, height, cf, stride);
    buffer->dmabuf_fd = g2d_get_buf_fd(buffer->lv_draw_buf);
    buffer->stride = stride;
    buffer->offset = 0;
    buffer->busy = false;

    /* Will be set on the dmabuf callback if the creation is successful*/
    buffer->wl_buffer = NULL;

    struct zwp_linux_buffer_params_v1 * params = zwp_linux_dmabuf_v1_create_params(ctx->handler);

    zwp_linux_buffer_params_v1_add(params,
                                   buffer->dmabuf_fd,
                                   0,
                                   buffer->offset,
                                   buffer->stride,
                                   0,
                                   0);

    zwp_linux_buffer_params_v1_add_listener(params, &params_listener, buffer);
    zwp_linux_buffer_params_v1_create(params, width, height, drm_cf, 0);
}

static void delete_buffer(lv_wl_buffer_t * buffer)
{
    if(buffer->wl_buffer) {
        wl_buffer_destroy(buffer->wl_buffer);
        buffer->wl_buffer = NULL;
    }
    if(buffer->lv_draw_buf) {
        lv_draw_buf_destroy(buffer->lv_draw_buf);
        buffer->lv_draw_buf = NULL;
    }
}

static lv_wl_g2d_display_data_t * wl_g2d_create_display_data(lv_wl_g2d_ctx_t * ctx, lv_display_t * display,
                                                             int32_t width, int32_t height)
{
    lv_wl_g2d_display_data_t * ddata = lv_zalloc(sizeof(*ddata));
    LV_ASSERT_MALLOC(ddata);
    if(!ddata) {
        return NULL;
    }

    lv_display_rotation_t rotation = lv_display_get_rotation(display);
    lv_color_format_t cf = lv_display_get_color_format(display);
    if(cf == LV_COLOR_FORMAT_RGB565 && !ctx->supports_rgb565) {
        LV_LOG_WARN("RGB565 is not supported by the wayland compositor. Falling back to XRGB8888");
        cf = LV_COLOR_FORMAT_XRGB8888;
        lv_display_set_color_format(display, cf);
    }

    ddata->drm_cf = lv_cf_to_drm_cf(cf);
    for(size_t i = 0; i < LV_WL_G2D_BUF_COUNT; i++) {
        init_buffer(ctx, &ddata->buffers[i], width, height, cf);
    }

#if LV_USE_ROTATE_G2D
    if(rotation == LV_DISPLAY_ROTATION_90 || rotation == LV_DISPLAY_ROTATION_270) {
        init_buffer(ctx, &ddata->rotate_buffer, height, width, cf);
    }
    else {
        init_buffer(ctx, &ddata->rotate_buffer, width, height, cf);
    }
#endif

    wl_display_flush(lv_wl_ctx.wl_display);
    wl_display_roundtrip(lv_wl_ctx.wl_display);
    for(size_t i = 0; i < LV_WL_G2D_BUF_COUNT; ++i) {
        if(!ddata->buffers[i].wl_buffer) {
            wl_g2d_delete_display_data(ddata);
            LV_LOG_ERROR("DMABUF creation failed");
            return NULL;
        }
    }

#if LV_USE_ROTATE_G2D
    if(!ddata->rotate_buffer.wl_buffer) {
        wl_g2d_delete_display_data(ddata);
        LV_LOG_ERROR("DMABUF creation failed");
        return NULL;
    }
    lv_display_set_draw_buffers(display, ddata->rotate_buffer.lv_draw_buf, NULL);
#else
    lv_display_set_draw_buffers(display, ddata->buffers[0].lv_draw_buf, ddata->buffers[1].lv_draw_buf);
#endif

    return ddata;
}

static void wl_g2d_delete_display_data(lv_wl_g2d_display_data_t * ddata)
{
    for(int i = 0; i < LV_WL_G2D_BUF_COUNT; i++) {
        delete_buffer(ddata->buffers + i);
    }

#if LV_USE_ROTATE_G2D
    delete_buffer(&ddata->rotate_buffer);
#endif

    lv_free(ddata);
}

static void * wl_g2d_init_display(void * backend_ctx, lv_display_t * display, int32_t width, int32_t height)
{

    lv_wl_g2d_ctx_t * ctx = (lv_wl_g2d_ctx_t *)backend_ctx;
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

static uint32_t lv_cf_to_drm_cf(lv_color_format_t cf)
{
    if(cf == LV_COLOR_FORMAT_UNKNOWN) {
        return DRM_FORMAT_ARGB8888; /* Default to ARGB8888 */
    }

    switch(cf) {
        case LV_COLOR_FORMAT_XRGB8888:
            return DRM_FORMAT_XRGB8888;
            break;
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            return DRM_FORMAT_ARGB8888;
            break;
        case LV_COLOR_FORMAT_RGB565:
            return DRM_FORMAT_RGB565;
            break;
        default:
            return DRM_FORMAT_ARGB8888;
    }
}

static void frame_done(void * data, struct wl_callback * callback, uint32_t time)
{
    LV_LOG_TRACE("Frame done");
    LV_UNUSED(time);
    lv_display_t * display = data;
    wl_callback_destroy(callback);
    lv_display_flush_ready(display);
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

static void * wl_g2d_resize_display(void * backend_ctx, lv_display_t * disp)
{
    lv_wl_g2d_ctx_t * ctx = (lv_wl_g2d_ctx_t *)backend_ctx;
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

static void dmabuf_format_table(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback,
                                int32_t fd, uint32_t size)
{
    lv_wl_g2d_ctx_t * ctx = data;

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
    lv_wl_g2d_ctx_t * ctx = data;

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
    lv_wl_g2d_ctx_t * ctx = data;

    LV_UNUSED(zwp_linux_dmabuf);
    if(format == DRM_FORMAT_RGB565) {
        ctx->supports_rgb565 = true;
    }
}

static lv_wl_buffer_t * get_next_buffer(lv_wl_g2d_display_data_t * ddata)
{
    lv_wl_buffer_t * ret =  &ddata->buffers[ddata->last_used];
    if(ret->busy) {
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
    uint32_t rotation = lv_display_get_rotation(disp);
    lv_wl_buffer_t * buf = get_next_buffer(ddata);

    if(!buf) {
        LV_LOG_ERROR("Failed to acquire a wayland window body buffer");
        return;
    }

    lv_draw_buf_invalidate_cache(buf->lv_draw_buf, NULL);
#if LV_USE_ROTATE_G2D
    lv_draw_buf_invalidate_cache(ddata->rotate_buffer.lv_draw_buf, NULL);
#endif

    struct wl_surface * surface = lv_wayland_get_window_surface(disp);
    /* Mark surface damage */
    wl_surface_damage(surface, area->x1, area->y1, src_width, src_height);

    if(!lv_display_flush_is_last(disp)) {
        lv_display_flush_ready(disp);
        return;
    }

    /*Rerender the whole surface if we're using rotation*/
    if(rotation != LV_DISPLAY_ROTATION_0) {
        wl_surface_damage(surface, 0, 0,
                          lv_display_get_original_horizontal_resolution(disp),
                          lv_display_get_original_vertical_resolution(disp));
    }

#if LV_USE_ROTATE_G2D
    g2d_rotate(ddata->rotate_buffer.lv_draw_buf, buf->lv_draw_buf,
               lv_display_get_original_horizontal_resolution(disp),
               lv_display_get_original_vertical_resolution(disp),
               lv_display_get_rotation(disp),
               lv_display_get_color_format(disp));
#endif
    /* Finally, attach buffer and commit to surface */
    struct wl_callback * cb = wl_surface_frame(surface);
    wl_callback_add_listener(cb, &frame_listener, disp);

    wl_surface_attach(surface, buf->wl_buffer, 0, 0);
    wl_surface_commit(surface);

    buf->busy = true;
    return;
}

#endif /*LV_USE_WAYLAND_G2D*/
