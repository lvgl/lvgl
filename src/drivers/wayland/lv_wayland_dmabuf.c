/**
 * @file lv_wayland_dmabuf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland_dmabuf.h"

#if LV_USE_WAYLAND && LV_WAYLAND_USE_DMABUF_PROTOCOL

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <drm/drm_fourcc.h>
#include <wayland_linux_dmabuf.h>

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void dmabuf_done(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback);
static void dmabuf_format_table(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback,
                                int32_t fd, uint32_t size);
static void dmabuf_main_device(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback,
                               struct wl_array * device);
static void dmabuf_tranche_done(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback);
static void dmabuf_tranche_target_device(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback,
                                         struct wl_array * device);
static void dmabuf_tranche_formats(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback,
                                   struct wl_array * indices);
static void dmabuf_tranche_flags(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback,
                                 uint32_t flags);
static void dmabuf_modifiers(void * data, struct zwp_linux_dmabuf_v1 * zwp_linux_dmabuf, uint32_t format,
                             uint32_t modifier_hi, uint32_t modifier_lo);
static void dmabuf_format(void * data, struct zwp_linux_dmabuf_v1 * zwp_linux_dmabuf, uint32_t format);

static void buffer_release(void * data, struct wl_buffer * buffer);
static void create_succeeded(void * data, struct zwp_linux_buffer_params_v1 * params, struct wl_buffer * new_buffer);
static void create_failed(void * data, struct zwp_linux_buffer_params_v1 * params);

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

static const struct wl_buffer_listener buffer_listener = {
    .release = buffer_release
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_wayland_dmabuf_ctx_init(lv_wayland_dmabuf_ctx_t * ctx)
{
    lv_memset(ctx, 0, sizeof(*ctx));
}

void lv_wayland_dmabuf_ctx_deinit(lv_wayland_dmabuf_ctx_t * ctx)
{
    if(!ctx) {
        return;
    }
    if(ctx->feedback) {
        zwp_linux_dmabuf_feedback_v1_destroy(ctx->feedback);
        ctx->feedback = NULL;
    }
    if(ctx->handler) {
        zwp_linux_dmabuf_v1_destroy(ctx->handler);
        ctx->handler = NULL;
    }
}

bool lv_wayland_dmabuf_registry_handle_global(lv_wayland_dmabuf_ctx_t * ctx, struct wl_registry * registry,
                                              uint32_t name, const char * interface, uint32_t version)
{
    if(!lv_streq(interface, zwp_linux_dmabuf_v1_interface.name)) {
        return false;
    }

    version = LV_MIN(version, (uint32_t)zwp_linux_dmabuf_v1_interface.version);
    ctx->handler = wl_registry_bind(registry, name, &zwp_linux_dmabuf_v1_interface, version);

    if(version >= 4) {
        ctx->feedback = zwp_linux_dmabuf_v1_get_default_feedback(ctx->handler);
        zwp_linux_dmabuf_feedback_v1_add_listener(ctx->feedback, &dmabuf_listener_v5, ctx);
    }
    else if(version < 3) {
        zwp_linux_dmabuf_v1_add_listener(ctx->handler, &dmabuf_listener, ctx);
    }
    else if(version == 3) {
        zwp_linux_dmabuf_v1_add_listener(ctx->handler, &dmabuf_listener_v3, ctx);
    }
    wl_display_roundtrip(lv_wl_ctx.wl_display);

    return true;
}

bool lv_wayland_dmabuf_commit_creation(lv_wayland_dmabuf_ctx_t * ctx)
{
    LV_UNUSED(ctx);
    int flush_ret = wl_display_flush(lv_wl_ctx.wl_display);
    if(flush_ret < 0 && errno != EAGAIN) {
        LV_LOG_ERROR("wl_display_flush failed: %s", strerror(errno));
        return false;
    }
    int roundtrip_ret = wl_display_roundtrip(lv_wl_ctx.wl_display);
    if(roundtrip_ret < 0) {
        LV_LOG_ERROR("wl_display_roundtrip failed: %s", strerror(errno));
        return false;
    }
    return true;
}

bool lv_wayland_dmabuf_buffer_is_valid(const lv_wayland_dmabuf_buffer_t * buf)
{
    LV_ASSERT(buf != NULL);
    return buf->wl_buffer != NULL;
}

bool lv_wayland_dmabuf_buffer_is_ready(const lv_wayland_dmabuf_buffer_t * buf)
{
    LV_ASSERT(buf != NULL);
    return !buf->busy;
}

void lv_wayland_dmabuf_buffer_attach(lv_wayland_dmabuf_ctx_t * ctx, struct wl_surface * surface,
                                     lv_wayland_dmabuf_buffer_t * buf,
                                     uint32_t width, uint32_t height)
{
    LV_ASSERT(buf != NULL);
    LV_ASSERT(surface != NULL);
    LV_ASSERT(width > 0);
    LV_ASSERT(height > 0);
    LV_UNUSED(ctx);
    wl_surface_attach(surface, buf->wl_buffer, 0, 0);
    wl_surface_damage(surface, 0, 0, width, height);
    wl_surface_commit(surface);
    buf->busy = true;
}

uint32_t lv_wayland_dmabuf_cf_to_drm(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_XRGB8888:
            return DRM_FORMAT_XRGB8888;
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            return DRM_FORMAT_ARGB8888;
        case LV_COLOR_FORMAT_RGB565:
            return DRM_FORMAT_RGB565;
        default:
            return 0;
    }
}

void lv_wayland_dmabuf_create_buffer(lv_wayland_dmabuf_ctx_t * ctx, lv_wayland_dmabuf_buffer_t * buf,
                                     int dmabuf_fd, uint32_t width, uint32_t height,
                                     uint32_t stride, uint32_t offset, uint32_t drm_cf,
                                     uint64_t modifier)
{
    /* Will be set on the create callback if the creation is successful */
    buf->wl_buffer = NULL;

    struct zwp_linux_buffer_params_v1 * params = zwp_linux_dmabuf_v1_create_params(ctx->handler);

    zwp_linux_buffer_params_v1_add(params, dmabuf_fd, 0, offset, stride,
                                   (uint32_t)(modifier >> 32), (uint32_t)(modifier & 0xFFFFFFFF));

    zwp_linux_buffer_params_v1_add_listener(params, &params_listener, buf);
    zwp_linux_buffer_params_v1_create(params, width, height, drm_cf, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void buffer_release(void * data, struct wl_buffer * buffer)
{
    LV_LOG_TRACE("Buffer released");
    LV_UNUSED(buffer);
    lv_wayland_dmabuf_buffer_t * buf = data;
    buf->busy = false;
}

static void create_succeeded(void * data, struct zwp_linux_buffer_params_v1 * params, struct wl_buffer * new_buffer)
{
    LV_LOG_TRACE("Buffer created successfully");
    lv_wayland_dmabuf_buffer_t * buf = data;
    buf->wl_buffer = new_buffer;

    /* When not using explicit synchronization listen to wl_buffer.release
     * for release notifications, otherwise we are going to use
     * zwp_linux_buffer_release_v1. */
    wl_buffer_add_listener(buf->wl_buffer, &buffer_listener, buf);

    zwp_linux_buffer_params_v1_destroy(params);
}

static void create_failed(void * data, struct zwp_linux_buffer_params_v1 * params)
{
    lv_wayland_dmabuf_buffer_t * buf = data;
    buf->wl_buffer = NULL;
    zwp_linux_buffer_params_v1_destroy(params);
    LV_LOG_ERROR("Failed to create dmabuf buffer");
}

static void dmabuf_format_table(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback,
                                int32_t fd, uint32_t size)
{
    lv_wayland_dmabuf_ctx_t * ctx = data;

    LV_UNUSED(feedback);

    if(fd < 0 || size == 0) {
        LV_LOG_ERROR("Invalid format table fd=%d size=%u", fd, size);
        if(fd >= 0) {
            close(fd);
        }
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

static void dmabuf_done(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback)
{
    LV_UNUSED(data);
    LV_UNUSED(feedback);

    LV_LOG_TRACE("DMABUF feedback done");

    /* This event marks the end of a feedback round. The client has received
     * all the format and modifier pairs from all tranches. This allows
     * the client to proceed with buffer allocation. */
}

static void dmabuf_main_device(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback,
                               struct wl_array * device)
{
    LV_UNUSED(data);
    LV_UNUSED(feedback);
    LV_UNUSED(device);

    LV_LOG_TRACE("DMABUF main device received (size: %zu)", device->size);

    /* This event advertises the main device that the server-side allocator
     * will use for scanout. It should be used by clients as a hint for
     * buffer allocation. */
}

static void dmabuf_tranche_done(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback)
{
    LV_UNUSED(data);
    LV_UNUSED(feedback);

    LV_LOG_TRACE("DMABUF tranche done");

    /* This event marks the end of a tranche. This allows the client to
     * process the formats and modifiers it has received for this tranche. */
}

static void dmabuf_tranche_target_device(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback,
                                         struct wl_array * device)
{
    LV_UNUSED(data);
    LV_UNUSED(feedback);
    LV_UNUSED(device);

    LV_LOG_TRACE("DMABUF tranche target device (size: %zu)", device->size);

    /* This event advertises the target device that the following tranche
     * will apply to. */
}

static void dmabuf_tranche_formats(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback,
                                   struct wl_array * indices)
{
    LV_UNUSED(data);
    LV_UNUSED(feedback);

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

static void dmabuf_tranche_flags(void * data, struct zwp_linux_dmabuf_feedback_v1 * feedback,
                                 uint32_t flags)
{
    LV_UNUSED(data);
    LV_UNUSED(feedback);
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
    lv_wayland_dmabuf_ctx_t * ctx = data;

    LV_UNUSED(zwp_linux_dmabuf);
    if(format == DRM_FORMAT_RGB565) {
        ctx->supports_rgb565 = true;
    }
}

#endif /*LV_USE_WAYLAND && LV_WAYLAND_USE_DMABUF_PROTOCOL*/
