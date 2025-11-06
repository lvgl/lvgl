
/**
 * @file lv_wl_g2d_backend.c
 *
 */


/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland_private.h"

#if LV_WAYLAND_USE_G2D

#if LV_USE_ROTATE_G2D && LV_WAYLAND_BUF_COUNT != 3
    #error "LV_WAYLAND_BUF_COUNT must be 3 when LV_USE_ROTATE_G2D is enabled"
#endif

// #if LV_WAYLAND_RENDER_MODE == 0
//     #error "LV_WAYLAND with G2D doesn't support LV_DISPLAY_RENDER_MODE_PARTIAL"
// #endif

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

/**********************
 *      TYPEDEFS
 **********************/

#define MAX_BUFFER_PLANES 4

typedef struct {
    struct wl_buffer * wl_buffer;
    void * buf_base[MAX_BUFFER_PLANES];
    lv_draw_buf_t * lv_draw_buf;

    int dmabuf_fds[MAX_BUFFER_PLANES];
    uint32_t strides[MAX_BUFFER_PLANES];
    uint32_t offsets[MAX_BUFFER_PLANES];
    uint32_t width;
    uint32_t height;
    int plane_count;
    bool busy;
} lv_wl_buffer_t;

typedef struct {
    lv_wl_buffer_t buffers[LV_WAYLAND_BUF_COUNT];
    uint32_t drm_cf;
    uint8_t last_used;
    bool flushing;
    uint32_t frame_count;
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
static void wl_g2d_deinit(void * backend_data);
static void wl_g2d_global_handler(void * backend_data, struct wl_registry * registry, uint32_t name,
                                  const char * interface, uint32_t version);


static void * wl_g2d_init_display(void * backend_data, lv_display_t * display, int32_t width, int32_t height);
static void * wl_g2d_resize_display(void * backend_data, lv_display_t * display);
static void wl_g2d_deinit_display(void * backend_data, lv_display_t * display);

static lv_wl_g2d_display_data_t * wl_g2d_create_display_data(lv_wl_g2d_ctx_t * ctx, lv_display_t * display,
                                                             int32_t width, int32_t height);

static void wl_g2d_delete_display_data(lv_wl_g2d_display_data_t * ddata);
static void set_display_buffers(lv_display_t * display, lv_wl_g2d_display_data_t * ddata);

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

static void dmabuf_wait_swap_buf(lv_display_t * disp);

static void buffer_release(void * data, struct wl_buffer * buffer);

static void create_succeeded(void * data, struct zwp_linux_buffer_params_v1 * params, struct wl_buffer * new_buffer);
static void create_failed(void * data, struct zwp_linux_buffer_params_v1 * params);

static uint32_t lv_cf_to_drm_cf(lv_color_format_t cf);

static void frame_done(void * data, struct wl_callback * callback, uint32_t time);

static void flush_wait_cb(lv_display_t * disp);

static lv_wl_buffer_t * get_next_buffer(lv_wl_g2d_display_data_t * ddata, uint8_t * color_p);

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
    LV_UNUSED(time);
    lv_display_t * display = data;
    lv_wl_g2d_display_data_t * ddata = lv_wayland_get_backend_display_data(display);
    ddata->frame_count++;
    wl_callback_destroy(callback);
    LV_LOG_USER("Frame done");
    lv_display_flush_ready(display);
}

static void set_display_buffers(lv_display_t * display, lv_wl_g2d_display_data_t * ddata)
{
    if(LV_USE_ROTATE_G2D == 1) {
        lv_display_set_draw_buffers(display, ddata->buffers[2].lv_draw_buf, NULL);
    }
    if(LV_WAYLAND_BUF_COUNT == 2) {
        lv_display_set_draw_buffers(display, ddata->buffers[0].lv_draw_buf, ddata->buffers[1].lv_draw_buf);
    }
    else if(LV_WAYLAND_BUF_COUNT == 1) {
        lv_display_set_draw_buffers(display, ddata->buffers[0].lv_draw_buf, NULL);
    }
}

static void dmabuf_wait_swap_buf(lv_display_t * disp)
{
    lv_wl_g2d_display_data_t * ddata = lv_wayland_get_backend_display_data(disp);

    if(ddata->frame_count == 0) {
        return;
    }

#if LV_USE_ROTATE_G2D
    int buf_nr = (ddata->last_used + 1) % (LV_WAYLAND_BUF_COUNT - 1);
#else
    int buf_nr = (ddata->last_used + 1) % LV_WAYLAND_BUF_COUNT;
#endif

    while(ddata->buffers[buf_nr].busy) {
        wl_display_roundtrip(lv_wl_ctx.wl_display);
        usleep(500); /* Sleep for 0.5ms to avoid busy waiting */
    }
}

static void flush_wait_cb(lv_display_t * disp)
{
    LV_LOG_USER("Flush wait cb");
    lv_wl_g2d_display_data_t * ddata = lv_wayland_get_backend_display_data(disp);
    if(ddata->frame_count == 0) {
        lv_display_flush_ready(disp);
        return;
    }
    while(disp->flushing) {
        wl_display_dispatch(lv_wl_ctx.wl_display);
    }
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, unsigned char * color_p)
{
    LV_LOG_USER("Flush!");
    lv_wl_g2d_display_data_t * ddata = lv_wayland_get_backend_display_data(disp);
    int32_t src_width = lv_area_get_width(area);
    int32_t src_height = lv_area_get_height(area);
    uint32_t rotation = lv_display_get_rotation(disp);
    lv_wl_buffer_t * buf = get_next_buffer(ddata, color_p);

#if LV_USE_ROTATE_G2D
    if(rotation == LV_DISPLAY_ROTATION_90 || rotation == LV_DISPLAY_ROTATION_270) {
        src_width  = lv_area_get_height(area);
        src_height = lv_area_get_width(area);
    }
#endif

    if(!buf) {
        LV_LOG_ERROR("Failed to acquire a wayland window body buffer");
        return;
    }

    lv_draw_buf_invalidate_cache(buf->lv_draw_buf, NULL);
#if LV_USE_ROTATE_G2D
    lv_draw_buf_invalidate_cache(ddata->buffers[2].lv_draw_buf, NULL);
#endif

    const bool force_full_flush = LV_WAYLAND_RENDER_MODE == LV_DISPLAY_RENDER_MODE_DIRECT &&
                                  rotation != LV_DISPLAY_ROTATION_0;
    struct wl_surface * surface = lv_wayland_get_window_surface(disp);
    /* Mark surface damage */
    if(!force_full_flush) {
        wl_surface_damage(surface, area->x1, area->y1, src_width, src_height);
    }

    if(lv_display_flush_is_last(disp)) {
        if(force_full_flush) {
            wl_surface_damage(surface, 0, 0, lv_display_get_original_horizontal_resolution(disp),
                              lv_display_get_original_vertical_resolution(disp));
        }
#if LV_USE_ROTATE_G2D
        g2d_rotate(ddata->buffers[2].lv_draw_buf, buf->lv_draw_buf,
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

        LV_LOG_USER("Wait swap buf");
        dmabuf_wait_swap_buf(disp);
        LV_LOG_USER("swap buf ok");
    }
    else {
        /* Not the last frame yet, so tell lvgl to keep going
         * For the last frame, we wait for the compositor instead */
        buf->busy = false;
        lv_display_flush_ready(disp);
    }

    return;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * wl_g2d_init(void)
{
    LV_LOG_USER("wl_g2d_init");
    lv_memset(&ctx, 0, sizeof(ctx));
    return &ctx;
}

static void wl_g2d_deinit(void * backend_data)
{
    LV_LOG_USER("wl_g2d_deinit");
    lv_wl_g2d_ctx_t * ctx = (lv_wl_g2d_ctx_t *)backend_data;
    if(!ctx) {
        return;
    }
    if(ctx->handler) {
        zwp_linux_dmabuf_v1_destroy(ctx->handler);

    }
}


static void wl_g2d_global_handler(void * backend_data, struct wl_registry * registry, uint32_t name,
                                  const char * interface, uint32_t version)
{

    LV_LOG_USER("wl_g2d_global_handler");
    LV_UNUSED(version);
    lv_wl_g2d_ctx_t * ctx = (lv_wl_g2d_ctx_t *)backend_data;

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

static lv_wl_g2d_display_data_t * wl_g2d_create_display_data(lv_wl_g2d_ctx_t * ctx, lv_display_t * display,
                                                             int32_t width, int32_t height)
{
    LV_LOG_USER("create display data");
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
    ddata->drm_cf = lv_cf_to_drm_cf(cf);

    for(int i = 0; i < LV_WAYLAND_BUF_COUNT; i++) {
        uint32_t w = width;
        uint32_t h = height;
        uint32_t rotation = lv_display_get_rotation(display);
        if(LV_USE_ROTATE_G2D && i == 2 && (rotation == LV_DISPLAY_ROTATION_90 || rotation == LV_DISPLAY_ROTATION_270)) {
            w = height;
            h = width;
        }

        uint32_t stride = lv_draw_buf_width_to_stride(w, cf);

        ddata->buffers[i].lv_draw_buf = lv_draw_buf_create(w, h, cf, stride);
        ddata->buffers[i].strides[0] = stride;
        ddata->buffers[i].dmabuf_fds[0] = g2d_get_buf_fd(ddata->buffers[i].lv_draw_buf);
        ddata->buffers[i].buf_base[0] = ddata->buffers[i].lv_draw_buf->data;

        struct zwp_linux_buffer_params_v1 * params = zwp_linux_dmabuf_v1_create_params(ctx->handler);

        zwp_linux_buffer_params_v1_add(params,
                                       ddata->buffers[i].dmabuf_fds[0],
                                       0,
                                       ddata->buffers[i].offsets[0],
                                       ddata->buffers[i].strides[0],
                                       0,
                                       0);

        zwp_linux_buffer_params_v1_add_listener(params, &params_listener, &ddata->buffers[i]);
        zwp_linux_buffer_params_v1_create(params, w, h, ddata->drm_cf, 0);
    }
    return ddata;
}

static void wl_g2d_delete_display_data(lv_wl_g2d_display_data_t * ddata)
{

    LV_LOG_USER("delete display data");
    for(int i = 0; i < LV_WAYLAND_BUF_COUNT; i++) {
        lv_wl_buffer_t * buf = ddata->buffers + i;
        if(buf->wl_buffer) {
            wl_buffer_destroy(buf->wl_buffer);
        }

        if(buf->lv_draw_buf) {
            lv_draw_buf_destroy(buf->lv_draw_buf);
        }
    }
}

static void * wl_g2d_init_display(void * backend_data, lv_display_t * display, int32_t width, int32_t height)
{

    LV_LOG_USER("init display");
    lv_wl_g2d_ctx_t * ctx = (lv_wl_g2d_ctx_t *)backend_data;
    lv_wl_g2d_display_data_t * ddata = wl_g2d_create_display_data(ctx, display, width, height);
    if(!ddata) {
        LV_LOG_ERROR("Failed to create display data");
        return NULL;
    }

    set_display_buffers(display, ddata);
    lv_display_set_flush_cb(display, flush_cb);
    lv_display_set_flush_wait_cb(display, flush_wait_cb);
    lv_display_set_render_mode(display, LV_WAYLAND_RENDER_MODE);
    return ddata;
}

static void buffer_release(void * data, struct wl_buffer * buffer)
{
    LV_UNUSED(buffer);
    LV_LOG_USER("Buffer release");
    lv_wl_buffer_t * buf = data;
    buf->busy = false;
}


static void create_succeeded(void * data, struct zwp_linux_buffer_params_v1 * params, struct wl_buffer * new_buffer)
{
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

static void * wl_g2d_resize_display(void * backend_data, lv_display_t * disp)
{
    LV_LOG_USER("resize display");
    lv_wl_g2d_ctx_t * ctx = (lv_wl_g2d_ctx_t *)backend_data;
    int32_t width = lv_display_get_horizontal_resolution(disp);
    int32_t height = lv_display_get_vertical_resolution(disp);

    lv_wl_g2d_display_data_t * ddata = wl_g2d_create_display_data(ctx, disp, width, height);
    if(!ddata) {
        LV_LOG_ERROR("Failed to create DMABUF buffers for %dx%d", width, height);
        return NULL;
    }
    set_display_buffers(disp, ddata);

    lv_wl_g2d_display_data_t * old_ddata = lv_wayland_get_backend_display_data(disp);
    wl_g2d_delete_display_data(old_ddata);

    return ddata;
}

static void wl_g2d_deinit_display(void * backend_data, lv_display_t * display)
{
    LV_LOG_USER("deinit display");

    LV_UNUSED(backend_data);
    lv_wl_g2d_display_data_t * ddata = lv_wayland_get_backend_display_data(display);
    if(!ddata) {
        return;
    }
    wl_g2d_delete_display_data(ddata);
}

#if 0
lv_result_t lv_wayland_dmabuf_resize_window(lv_wl_g2d_ctx_t * context, lv_wl_window_t * window, int width, int height)
{
    /* Don't attempt to create buffers with invalid dimensions */
    if(width <= 0 || height <= 0) {
        LV_LOG_ERROR("DMABUF resize failed: invalid dimensions %dx%d", width, height);
        return LV_RESULT_INVALID;
    }

    lv_wayland_dmabuf_destroy_draw_buffers(context, window);

    struct buffer * buffers = lv_wayland_dmabuf_create_draw_buffers_internal(window, width, height);
    if(!buffers) {
        LV_LOG_ERROR("Failed to create DMABUF buffers for %dx%d", width, height);
        return LV_RESULT_INVALID;
    }

    context->buffers = buffers;
    lv_wayland_dmabuf_set_draw_buffers(context, window->lv_disp);

    /* Clear DMABUF resize pending flag and acknowledge XDG configure if needed */
    window->dmabuf_resize_pending = false;

    if(window->surface_configured && window->configure_serial > 0 && !window->configure_acknowledged) {
        lv_wayland_xdg_shell_ack_configure(window, window->configure_serial);
        window->configure_acknowledged = true;
        window->configure_serial = 0;  /* Reset after acknowledgment */
    }
    else if(window->configure_acknowledged) {
        LV_LOG_TRACE("XDG configure already acknowledged, skipping duplicate acknowledgment");
        window->configure_serial = 0;  /* Reset the serial */
    }

    LV_LOG_TRACE("DMABUF resize completed successfully: %dx%d", width, height);
    return LV_RESULT_OK;
}
#endif



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

    LV_LOG_TRACE("DMABUF feedback done - format is %u", ctx->format);

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

static lv_wl_buffer_t * get_next_buffer(lv_wl_g2d_display_data_t * ddata, uint8_t * color_p)
{
    if(LV_USE_ROTATE_G2D) {

        int next_buf = (ddata->last_used + 1) % (LV_WAYLAND_BUF_COUNT - 1);
        ddata->buffers[next_buf].busy = 1;
        ddata->last_used = next_buf;
        return &ddata->buffers[next_buf];

    }

    for(int i = 0; i < LV_WAYLAND_BUF_COUNT; i++) {
        lv_wl_buffer_t * buffer = &ddata->buffers[i];
        if(buffer->buf_base[0] == color_p && buffer->busy == 0) {
            ddata->last_used = i;
            buffer->busy = 1;
            return buffer;
        }
    }

    while(1) {
        wl_display_roundtrip(lv_wl_ctx.wl_display);

        for(int i = 0; i < LV_WAYLAND_BUF_COUNT; i++) {
            lv_wl_buffer_t * buffer = &ddata->buffers[i];
            if(buffer->buf_base[0] == color_p && buffer->busy == 0) {
                ddata->last_used = i;
                buffer->busy = 1;
                return buffer;
            }
        }
    }
    return NULL;

}

#endif /*LV_USE_WAYLAND_G2D*/
