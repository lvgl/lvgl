/**
 * @file lv_wl_dmabuf.c
 *
 */

#include "lv_wayland.h"

#if LV_WAYLAND_USE_DMABUF

#include "lv_wayland_private.h"
#include <wayland_linux_dmabuf.h>
#include <drm/drm_fourcc.h>
#include <stdlib.h>
#include <unistd.h>
#include <src/misc/lv_types.h>
#include <string.h>
#include "../../draw/nxp/g2d/lv_g2d_utils.h"
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

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
static struct buffer * dmabuf_acquire_buffer(dmabuf_ctx_t * context, unsigned char * color_p);
static struct buffer * lv_wayland_dmabuf_create_draw_buffers_internal(struct window * window, int width, int height);
static void buffer_free(struct buffer * buf);
static void dmabuf_wait_swap_buf(lv_display_t * disp);

/**********************
 *  STATIC VARIABLES
 **********************/

static const struct zwp_linux_dmabuf_feedback_v1_listener dmabuf_listener_v5 = {
    .done          = dmabuf_done,
    .format_table  = dmabuf_format_table,
    .main_device   = dmabuf_main_device,
    .tranche_done  = dmabuf_tranche_done,
    .tranche_target_device = dmabuf_tranche_target_device,
    .tranche_formats = dmabuf_tranche_formats,
    .tranche_flags = dmabuf_tranche_flags,
};

static const struct zwp_linux_dmabuf_v1_listener dmabuf_listener_v3 = {.format   = dmabuf_format,
           .modifier = dmabuf_modifiers
};
static const struct zwp_linux_dmabuf_v1_listener dmabuf_listener    = {.format = dmabuf_format};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

void lv_wayland_dmabuf_initalize_context(dmabuf_ctx_t * context)
{
    memset(context, 0, sizeof(*context));
    context->format = DRM_FORMAT_INVALID;
}
lv_result_t lv_wayland_dmabuf_set_draw_buffers(dmabuf_ctx_t * context, lv_display_t * display)
{
    if(LV_WAYLAND_BUF_COUNT == 2) {
        lv_display_set_draw_buffers(display, context->buffers[0].lv_draw_buf, context->buffers[1].lv_draw_buf);
        return LV_RESULT_OK;
    }
    else if(LV_WAYLAND_BUF_COUNT == 1) {
        lv_display_set_draw_buffers(display, context->buffers[0].lv_draw_buf, NULL);
        return LV_RESULT_OK;
    }
    return LV_RESULT_INVALID;
}

void lv_wayland_dmabuf_set_interface(dmabuf_ctx_t * context, struct wl_registry * registry, uint32_t name,
                                     const char * interface, uint32_t version)
{
    LV_UNUSED(interface);

    context->handler = wl_registry_bind(registry, name, &zwp_linux_dmabuf_v1_interface, version);

    if(version >= 4) {
        struct zwp_linux_dmabuf_feedback_v1 * feedback = zwp_linux_dmabuf_v1_get_default_feedback(context->handler);
        zwp_linux_dmabuf_feedback_v1_add_listener(feedback, &dmabuf_listener_v5, context);
    }
    else if(version < 3) {
        zwp_linux_dmabuf_v1_add_listener(context->handler, &dmabuf_listener, context);
    }
    else if(version == 3) {
        zwp_linux_dmabuf_v1_add_listener(context->handler, &dmabuf_listener_v3, context);
    }
}

lv_result_t lv_wayland_dmabuf_is_ready(dmabuf_ctx_t * context)
{
    return (context->handler && context->format != DRM_FORMAT_INVALID) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

void lv_wayland_dmabuf_destroy_window(dmabuf_ctx_t * context, struct window * window)
{
    LV_UNUSED(context);
    LV_ASSERT_NULL(window);
}

void lv_wayland_dmabuf_deinit(dmabuf_ctx_t * context)
{
    LV_UNUSED(context);
}

struct graphic_object * lv_wayland_dmabuf_on_graphical_object_creation(dmabuf_ctx_t * context,
                                                                       struct graphic_object * obj)
{
    LV_UNUSED(context);
    return obj;
}

void lv_wayland_dmabuf_on_graphical_object_destruction(dmabuf_ctx_t * context, struct graphic_object * obj)
{

    LV_UNUSED(context);
    LV_UNUSED(obj);
}

static void dmabuf_wait_swap_buf(lv_display_t * disp)
{
    struct window * window = lv_display_get_user_data(disp);

    if(window->frame_counter == 0) {
        return;
    }

    int buf_nr = (window->wl_ctx->dmabuf_ctx.last_used + 1) % LV_WAYLAND_BUF_COUNT;

    while(window->wl_ctx->dmabuf_ctx.buffers[buf_nr].busy) {
        wl_display_roundtrip(lv_wl_ctx.display);
        usleep(500); /* Sleep for 0.5ms to avoid busy waiting */
    }
}

void lv_wayland_dmabuf_flush_full_mode(lv_display_t * disp, const lv_area_t * area, unsigned char * color_p)
{
    struct window * window = lv_display_get_user_data(disp);
    struct buffer * buf    = dmabuf_acquire_buffer(&window->wl_ctx->dmabuf_ctx, color_p);
    if(!buf) {
        LV_LOG_ERROR("Failed to acquire a wayland window body buffer");
        return;
    }

    int32_t src_width  = lv_area_get_width(area);
    int32_t src_height = lv_area_get_height(area);

    lv_draw_buf_invalidate_cache(buf->lv_draw_buf, NULL);

    /* Mark surface damage */
    wl_surface_damage(window->body->surface, area->x1, area->y1, src_width, src_height);

    if(lv_display_flush_is_last(disp)) {
        /* Finally, attach buffer and commit to surface */
        wl_surface_attach(window->body->surface, buf->buffer, 0, 0);
        wl_surface_commit(window->body->surface);

        struct wl_callback * cb = wl_surface_frame(window->body->surface);
        wl_callback_add_listener(cb, lv_wayland_window_get_wl_surface_frame_listener(), window->body);

        window->flush_pending = true;
        dmabuf_wait_swap_buf(disp);
    }
    else {
        /* Not the last frame yet, so tell lvgl to keep going
         * For the last frame, we wait for the compositor instead */
        buf->busy = 0;
        lv_display_flush_ready(disp);
    }

    return;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

static void buffer_release(void * data, struct wl_buffer * buffer)
{
    LV_UNUSED(buffer);
    struct buffer * buf = data;
    buf->busy           = 0;
}

static const struct wl_buffer_listener buffer_listener = {.release = buffer_release};

static void create_succeeded(void * data, struct zwp_linux_buffer_params_v1 * params, struct wl_buffer * new_buffer)
{
    struct buffer * buffer = data;
    buffer->buffer         = new_buffer;
    /* When not using explicit synchronization listen to wl_buffer.release
     * for release notifications, otherwise we are going to use
     * zwp_linux_buffer_release_v1. */
    wl_buffer_add_listener(buffer->buffer, &buffer_listener, buffer);

    zwp_linux_buffer_params_v1_destroy(params);
}

static void create_failed(void * data, struct zwp_linux_buffer_params_v1 * params)
{
    struct buffer * buffer = data;

    buffer->buffer = NULL;
    zwp_linux_buffer_params_v1_destroy(params);
    LV_LOG_ERROR("Failed to create dmabuf buffer\n");
}

static const struct zwp_linux_buffer_params_v1_listener params_listener = {.created = create_succeeded,
           .failed  = create_failed
};

lv_result_t lv_wayland_dmabuf_resize_window(dmabuf_ctx_t * context, struct window * window, int width, int height)
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

lv_result_t lv_wayland_dmabuf_create_draw_buffers(dmabuf_ctx_t * context, struct window * window)
{
    struct buffer * buffers = lv_wayland_dmabuf_create_draw_buffers_internal(window, window->width, window->height);
    if(!buffers) {
        return LV_RESULT_INVALID;
    }

    context->buffers = buffers;
    return LV_RESULT_OK;
}

void lv_wayland_dmabuf_destroy_draw_buffers(dmabuf_ctx_t * context, struct window * window)
{
    LV_UNUSED(window);
    if(context->buffers == NULL) {
        return;
    }
    for(int i = 0; i < LV_WAYLAND_BUF_COUNT; i++) {
        buffer_free(&context->buffers[i]);
    }
    free(context->buffers);
    context->buffers = NULL;
}

uint32_t lv_wayland_dmabuf_get_format(struct window * window)
{
    uint32_t drmcf = 0;
    lv_color_format_t format = lv_display_get_color_format(window->lv_disp);
    if(format == LV_COLOR_FORMAT_UNKNOWN) {
        return DRM_FORMAT_ARGB8888; /* Default to ARGB8888 */
    }

    switch(format) {
        case LV_COLOR_FORMAT_XRGB8888:
            drmcf = DRM_FORMAT_XRGB8888;
            break;
        case LV_COLOR_FORMAT_ARGB8888:
            drmcf = DRM_FORMAT_ARGB8888;
            break;
        case LV_COLOR_FORMAT_RGB565:
            drmcf = DRM_FORMAT_RGB565;
            break;
        default:
            drmcf = DRM_FORMAT_ARGB8888;
    }

    return drmcf;
}

static struct buffer * lv_wayland_dmabuf_create_draw_buffers_internal(struct window * window, int width, int height)
{
    const uint32_t flags = 0;
    struct zwp_linux_buffer_params_v1 * params;
    uint32_t drmcf = lv_wayland_dmabuf_get_format(window);
    const int stride        = lv_draw_buf_width_to_stride(width, lv_display_get_color_format(window->lv_disp));
    struct buffer * buffers = (struct buffer *)calloc(LV_WAYLAND_BUF_COUNT, sizeof(struct buffer));
    LV_ASSERT_MALLOC(buffers);

    for(int i = 0; i < LV_WAYLAND_BUF_COUNT; i++) {
        buffers[i].window = window;
        buffers[i].lv_draw_buf =
            lv_draw_buf_create(width, height, lv_display_get_color_format(window->lv_disp), stride);
        buffers[i].strides[0]    = stride;
        buffers[i].dmabuf_fds[0] = g2d_get_buf_fd(buffers[i].lv_draw_buf);
        buffers[i].buf_base[0]   = buffers[i].lv_draw_buf->data;
        params                   = zwp_linux_dmabuf_v1_create_params(window->wl_ctx->dmabuf_ctx.handler);

        zwp_linux_buffer_params_v1_add(params, buffers[i].dmabuf_fds[0], 0, buffers[i].offsets[0], buffers[i].strides[0], 0,
                                       0);

        zwp_linux_buffer_params_v1_add_listener(params, &params_listener, &buffers[i]);
        zwp_linux_buffer_params_v1_create(params, width, height, drmcf, flags);
    }

    wl_display_roundtrip(lv_wl_ctx.display);

    window->body->width = width;
    window->body->height = height;

    return buffers;
}

static void buffer_free(struct buffer * buf)
{
    if(buf->buffer) wl_buffer_destroy(buf->buffer);

    if(buf->lv_draw_buf) lv_draw_buf_destroy(buf->lv_draw_buf);
}

static void dmabuf_format_table(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback,
                                int32_t fd, uint32_t size)
{
    dmabuf_ctx_t * ctx = data;

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
        uint32_t format = formats[i * 4]; /* Each entry is 4 uint32_t words */

        if(LV_COLOR_DEPTH == 32) {
            if(format == DRM_FORMAT_ARGB8888) {
                ctx->format = format;
                break;
            }
            else if(format == DRM_FORMAT_XRGB8888 && ctx->format == DRM_FORMAT_INVALID) {
                ctx->format = format;
                break;
            }
        }
        else if(LV_COLOR_DEPTH == 16 && format == DRM_FORMAT_RGB565) {
            ctx->format = format;
            break;
        }
    }

    LV_ASSERT(ctx->format != DRM_FORMAT_INVALID);

    /* Clean up */
    munmap(table, size);
    close(fd);
}

static void dmabuf_done(void * data, struct zwp_linux_dmabuf_feedback_v1 * zwp_linux_dmabuf_feedback)
{
    dmabuf_ctx_t * ctx = data;

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
    dmabuf_ctx_t * ctx = data;

    LV_UNUSED(zwp_linux_dmabuf_feedback);

    LV_LOG_TRACE("DMABUF tranche formats (count: %zu)", indices->size / sizeof(uint16_t));

    /* This event advertises the format + modifier pairs that the compositor
     * supports for the current tranche. The indices are offsets into the
     * format table sent earlier. */

    if(ctx->format == DRM_FORMAT_INVALID && indices->size > 0) {
        /* If we don't have a format yet, we could parse the indices here
         * to find a suitable format from the format table, but for now
         * we rely on the format_table callback to set a format directly */
        LV_LOG_TRACE("Format indices received but format already set or no format table");
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
    dmabuf_ctx_t * ctx = data;

    LV_UNUSED(zwp_linux_dmabuf);

    if(LV_COLOR_DEPTH == 32 && format == DRM_FORMAT_ARGB8888) {

        /* Wayland compositors MUST support ARGB8888 */
        ctx->format = format;

    }
    else if(LV_COLOR_DEPTH == 32 && format == DRM_FORMAT_XRGB8888 && ctx->format != DRM_FORMAT_ARGB8888) {
        /* Select XRGB only if the compositor doesn't support transprancy */
        ctx->format = format;

    }
    else if(LV_COLOR_DEPTH == 16 && format == DRM_FORMAT_RGB565) {
        ctx->format = format;
    }
}

static struct buffer * dmabuf_acquire_buffer(dmabuf_ctx_t * context, unsigned char * color_p)
{

    for(int i = 0; i < LV_WAYLAND_BUF_COUNT; i++) {
        struct buffer * buffer = &context->buffers[i];
        if(buffer->buf_base[0] == color_p && buffer->busy == 0) {
            context->last_used = i;
            buffer->busy = 1;
            return buffer;
        }
    }

    while(1) {
        wl_display_roundtrip(lv_wl_ctx.display);

        for(int i = 0; i < LV_WAYLAND_BUF_COUNT; i++) {
            struct buffer * buffer = &context->buffers[i];
            if(buffer->buf_base[0] == color_p && buffer->busy == 0) {
                context->last_used = i;
                buffer->busy = 1;
                return buffer;
            }
        }
    }

    return NULL;
}

#if LV_WAYLAND_WINDOW_DECORATIONS
static void create_decorators_buf(struct window * window, struct graphic_object * decoration)
{
    struct zwp_linux_buffer_params_v1 * params;
    const uint32_t flags = 0;
    uint8_t id = decoration->type;

    window->decorators_buf[id] = (struct buffer *)calloc(1, sizeof(struct buffer));
    LV_ASSERT_MALLOC(window->decorators_buf[id]);

    const int stride        = lv_draw_buf_width_to_stride(decoration->width, lv_display_get_color_format(window->lv_disp));
    window->decorators_buf[id]->window = window;
    window->decorators_buf[id]->lv_draw_buf =
        lv_draw_buf_create(decoration->width, decoration->height, lv_display_get_color_format(window->lv_disp), stride);

    window->decorators_buf[id]->strides[0]    = stride;
    window->decorators_buf[id]->width        = decoration->width;
    window->decorators_buf[id]->height       = decoration->height;
    window->decorators_buf[id]->dmabuf_fds[0] = g2d_get_buf_fd(window->decorators_buf[id]->lv_draw_buf);
    window->decorators_buf[id]->buf_base[0]   = window->decorators_buf[id]->lv_draw_buf->data;
    params                   = zwp_linux_dmabuf_v1_create_params(window->wl_ctx->dmabuf_ctx.handler);

    zwp_linux_buffer_params_v1_add(params, window->decorators_buf[id]->dmabuf_fds[0], 0,
                                   window->decorators_buf[id]->offsets[0],
                                   window->decorators_buf[id]->strides[0], 0,
                                   0);

    zwp_linux_buffer_params_v1_add_listener(params, &params_listener, window->decorators_buf[id]);
    zwp_linux_buffer_params_v1_create(params, decoration->width, decoration->height, lv_wayland_dmabuf_get_format(window),
                                      flags);

    wl_display_roundtrip(lv_wl_ctx.display);
}

void destroy_decorators_buf(struct window * window, struct graphic_object * decoration)
{
    uint8_t id = decoration->type;

    if(window->decorators_buf[id] != NULL) {
        buffer_free(window->decorators_buf[id]);
        free(window->decorators_buf[id]);
        window->decorators_buf[id] = NULL;
    }
}

struct buffer * dmabuf_acquire_pool_buffer(struct window * window, struct graphic_object * decoration)
{
    uint8_t id = decoration->type;

    if(window->decorators_buf[id] == NULL || (window->decorators_buf[id]->width == (uint32_t)decoration->width &&
                                              window->decorators_buf[id]->height == (uint32_t)decoration->height)) {
        create_decorators_buf(window, decoration);

        return window->decorators_buf[id];
    }
    else {
        destroy_decorators_buf(window, decoration);
        create_decorators_buf(window, decoration);

        return window->decorators_buf[id];
    }
}
#endif

#endif /* LV_WAYLAND_DMABUF */
