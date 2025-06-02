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
#include <src/misc/lv_types.h>
#include <string.h>
#include "../../draw/nxp/g2d/lv_g2d_utils.h"

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#define MAX_BUFFER_PLANES 4

struct buffer {
    int busy;

#if LV_WAYLAND_USE_DMABUF
    struct window * window;
    int plane_count;

    int dmabuf_fds[MAX_BUFFER_PLANES];
    uint32_t strides[MAX_BUFFER_PLANES];
    uint32_t offsets[MAX_BUFFER_PLANES];
    struct wl_buffer * buffer;
#endif

    void * buf_base[MAX_BUFFER_PLANES];
    lv_draw_buf_t * lv_draw_buf;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void dmabuf_modifiers(void * data, struct zwp_linux_dmabuf_v1 * zwp_linux_dmabuf, uint32_t format,
                             uint32_t modifier_hi, uint32_t modifier_lo);
static void dmabuf_format(void * data, struct zwp_linux_dmabuf_v1 * zwp_linux_dmabuf, uint32_t format);
static struct buffer * dmabuf_acquire_buffer(dmabuf_ctx_t * context, unsigned char * color_p);
static struct buffer * lv_wayland_dmabuf_create_draw_buffers_internal(struct window * window);
static void buffer_free(struct buffer * buf);

/**********************
 *  STATIC VARIABLES
 **********************/

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
    if(version > 3) {
        LV_LOG_WARN("Unsupported DMABUF version %d. Using version 3 instead", version);
        version = 3;
    }

    context->handler = wl_registry_bind(registry, name, &zwp_linux_dmabuf_v1_interface, version);
    if(version < 3) {
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

        buf->busy             = 1;
        window->flush_pending = true;
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

lv_result_t lv_wayland_dmabuf_resize_window(dmabuf_ctx_t * context, struct window * window)
{
    struct buffer * buffers = lv_wayland_dmabuf_create_draw_buffers_internal(window);
    if(!buffers) {
        return LV_RESULT_INVALID;
    }
    lv_wayland_dmabuf_destroy_draw_buffers(context, window);

    context->buffers = buffers;
    lv_wayland_dmabuf_set_draw_buffers(context, window->lv_disp);
    return LV_RESULT_OK;
}

lv_result_t lv_wayland_dmabuf_create_draw_buffers(dmabuf_ctx_t * context, struct window * window)
{
    struct buffer * buffers = lv_wayland_dmabuf_create_draw_buffers_internal(window);
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
        return;
    }
    free(context->buffers);
    context->buffers = NULL;
}

static struct buffer * lv_wayland_dmabuf_create_draw_buffers_internal(struct window * window)
{
    const uint32_t flags = 0;
    struct zwp_linux_buffer_params_v1 * params;
    const int stride        = lv_draw_buf_width_to_stride(window->width, lv_display_get_color_format(window->lv_disp));
    struct buffer * buffers = (struct buffer *)calloc(LV_WAYLAND_BUF_COUNT, sizeof(struct buffer));
    LV_ASSERT_MALLOC(buffers);

    for(int i = 0; i < LV_WAYLAND_BUF_COUNT; i++) {
        uint32_t drmcf = 0;

        buffers[i].window = window;
        buffers[i].lv_draw_buf =
            lv_draw_buf_create(window->width, window->height, lv_display_get_color_format(window->lv_disp), stride);
        buffers[i].strides[0]    = stride;
        buffers[i].dmabuf_fds[0] = g2d_get_buf_fd(buffers[i].lv_draw_buf);
        buffers[i].buf_base[0]   = buffers[i].lv_draw_buf->data;
        params                   = zwp_linux_dmabuf_v1_create_params(window->wl_ctx->dmabuf_ctx.handler);

        switch(lv_display_get_color_format(window->lv_disp)) {
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

        zwp_linux_buffer_params_v1_add(params, buffers[i].dmabuf_fds[0], 0, buffers[i].offsets[0], buffers[i].strides[0], 0,
                                       0);

        zwp_linux_buffer_params_v1_add_listener(params, &params_listener, &buffers[i]);
        zwp_linux_buffer_params_v1_create(params, window->width, window->height, drmcf, flags);
    }

    wl_display_roundtrip(lv_wl_ctx.display);

    return buffers;
}

static void buffer_free(struct buffer * buf)
{
    if(buf->buffer) wl_buffer_destroy(buf->buffer);

    if(buf->lv_draw_buf) lv_draw_buf_destroy(buf->lv_draw_buf);
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
            return buffer;
        }
    }

    while(1) {
        wl_display_roundtrip(lv_wl_ctx.display);

        for(int i = 0; i < LV_WAYLAND_BUF_COUNT; i++) {
            struct buffer * buffer = &context->buffers[i];
            if(buffer->buf_base[0] == color_p && buffer->busy == 0) {
                return buffer;
            }
        }
    }

    return NULL;
}

#endif /* LV_WAYLAND_DMABUF */
