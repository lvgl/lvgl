/**
 * @file lv_wayland_backend_dmabuf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland_private.h"

#if LV_WAYLAND_USE_DMABUF

#include "lv_wayland_dmabuf.h"
#include "../../display/lv_display_private.h"

#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <unistd.h>
#include <gbm.h>
#include <drm_fourcc.h>
#include <xf86drm.h>

/*********************
 *      DEFINES
 *********************/

#define LV_WL_DMABUF_BUF_COUNT 2

/* Bounds the wait for the compositor to hand a buffer back. A compositor that
 * misses the deadline costs a dropped frame instead of a stalled application. */
#define LV_WL_DMABUF_RELEASE_TIMEOUT_MS 500

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    /* Shared DMA-BUF protocol state (wl_buffer + release tracking) */
    lv_wayland_dmabuf_buffer_t base;

    /* GBM allocator specific state */
    struct gbm_bo * bo;
    int dmabuf_fd;
    uint32_t stride;
    uint32_t offset;
    uint64_t modifier;
    uint32_t cf;
} lv_wl_buffer_t;

typedef struct {
    lv_wayland_dmabuf_ctx_t dmabuf;
    struct gbm_device * gbm_device;
    int drm_fd;
} lv_wl_dmabuf_ctx_t;

typedef struct {
    lv_wl_buffer_t * buffers;
    uint8_t * render_buf;
    int32_t width;
    int32_t height;
    uint8_t last_used;
} lv_wl_dmabuf_display_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t wl_dmabuf_init(void ** backend_data);
static void wl_dmabuf_deinit(void * backend_ctx);
static void wl_dmabuf_global_handler(void * backend_ctx, struct wl_registry * registry, uint32_t name,
                                     const char * interface, uint32_t version);

static void * wl_dmabuf_init_display(void * backend_ctx, lv_display_t * display, int32_t width, int32_t height);
static void * wl_dmabuf_resize_display(void * backend_ctx, lv_display_t * display);
static void wl_dmabuf_deinit_display(void * backend_ctx, lv_display_t * display);

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void flush_wait_cb(lv_display_t * disp);
static void frame_done(void * data, struct wl_callback * callback, uint32_t time);

static bool is_available(lv_wl_dmabuf_ctx_t * ctx, lv_display_t * display);
static int open_drm_device(void);
static uint32_t lv_drm_cf_to_gbm_cf(uint32_t drm_cf);

static bool init_buffer(lv_wl_dmabuf_ctx_t * ctx, lv_wl_buffer_t * buffer, uint32_t width, uint32_t height,
                        lv_color_format_t cf);
static void delete_buffer(lv_wl_buffer_t * buffer);
static bool init_buffers(lv_wl_dmabuf_ctx_t * ctx, lv_wl_buffer_t * buffers, lv_display_t * display,
                         int32_t width, int32_t height);
static void delete_buffers(lv_wl_buffer_t * buffers);
static lv_wl_buffer_t * get_next_buffer(lv_wl_dmabuf_display_data_t * ddata);

static bool copy_frame_to_buffer(lv_wl_buffer_t * buffer, const uint8_t * px_map, lv_display_t * disp);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_wl_dmabuf_ctx_t ctx;

static const struct wl_callback_listener frame_listener = {
    .done = frame_done,
};

const lv_wayland_backend_ops_t wl_dmabuf_ops = {
    .init = wl_dmabuf_init,
    .deinit = wl_dmabuf_deinit,
    .global_handler = wl_dmabuf_global_handler,
};

const lv_wayland_backend_display_ops_t wl_dmabuf_display_ops = {
    .init_display = wl_dmabuf_init_display,
    .deinit_display = wl_dmabuf_deinit_display,
    .resize_display = wl_dmabuf_resize_display,
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

static lv_result_t wl_dmabuf_init(void ** backend_data)
{
    lv_memzero(&ctx, sizeof(ctx));
    lv_wayland_dmabuf_ctx_init(&ctx.dmabuf);
    ctx.drm_fd = -1;

    ctx.drm_fd = open_drm_device();
    if(ctx.drm_fd < 0) {
        LV_LOG_WARN("Failed to open a DRM device");
        return LV_RESULT_INVALID;
    }

    ctx.gbm_device = gbm_create_device(ctx.drm_fd);
    if(!ctx.gbm_device) {
        LV_LOG_WARN("Failed to create a GBM device");
        close(ctx.drm_fd);
        ctx.drm_fd = -1;
        return LV_RESULT_INVALID;
    }

    *backend_data = &ctx;
    return LV_RESULT_OK;
}

static void wl_dmabuf_deinit(void * backend_ctx)
{
    lv_wl_dmabuf_ctx_t * c = backend_ctx;
    if(!c) {
        return;
    }

    lv_wayland_dmabuf_ctx_deinit(&c->dmabuf);

    if(c->gbm_device) {
        gbm_device_destroy(c->gbm_device);
        c->gbm_device = NULL;
    }
    if(c->drm_fd >= 0) {
        close(c->drm_fd);
        c->drm_fd = -1;
    }
}

static void wl_dmabuf_global_handler(void * backend_ctx, struct wl_registry * registry, uint32_t name,
                                     const char * interface, uint32_t version)
{
    lv_wl_dmabuf_ctx_t * c = backend_ctx;
    lv_wayland_dmabuf_registry_handle_global(&c->dmabuf, registry, name, interface, version);
}

static bool is_available(lv_wl_dmabuf_ctx_t * c, lv_display_t * display)
{
    if(!c->dmabuf.handler) {
        LV_LOG_WARN("Compositor does not support zwp_linux_dmabuf_v1");
        return false;
    }

    const lv_color_format_t cf = lv_display_get_color_format(display);
    if(cf == LV_COLOR_FORMAT_RGB565 && !c->dmabuf.supports_rgb565) {
        LV_LOG_WARN("Compositor does not advertise DMA-BUF RGB565 support");
        return false;
    }

    return true;
}

static void * wl_dmabuf_init_display(void * backend_ctx, lv_display_t * display, int32_t width, int32_t height)
{
    lv_wl_dmabuf_ctx_t * c = backend_ctx;

    if(!is_available(c, display)) {
        return NULL;
    }

    lv_wl_dmabuf_display_data_t * ddata = lv_zalloc(sizeof(*ddata));
    LV_ASSERT_MALLOC(ddata);
    if(!ddata) {
        LV_LOG_ERROR("Failed to allocate data for display");
        return NULL;
    }

    ddata->buffers = lv_malloc(LV_WL_DMABUF_BUF_COUNT * sizeof(*ddata->buffers));
    if(!ddata->buffers) {
        LV_LOG_WARN("Failed to allocate wl buffers");
        lv_free(ddata);
        return NULL;
    }
    if(!init_buffers(c, ddata->buffers, display, width, height)) {
        delete_buffers(ddata->buffers);
        lv_free(ddata);
        return NULL;
    }

    const lv_color_format_t cf = lv_display_get_color_format(display);
    const size_t buf_size = (size_t)lv_draw_buf_width_to_stride(width, cf) * height;


    ddata->render_buf = lv_malloc(buf_size);
    if(!ddata->render_buf) {
        delete_buffers(ddata->buffers);
        lv_free(ddata);
        return NULL;
    }

    ddata->width = width;
    ddata->height = height;

    lv_display_set_buffers(display, ddata->render_buf, NULL, buf_size,
                           LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(display, flush_cb);
    lv_display_set_flush_wait_cb(display, flush_wait_cb);

    return ddata;
}

static void * wl_dmabuf_resize_display(void * backend_ctx, lv_display_t * display)
{
    lv_wl_dmabuf_ctx_t * c = backend_ctx;
    lv_wl_dmabuf_display_data_t * ddata = lv_wayland_get_backend_display_data(display);

    const int32_t width = lv_display_get_horizontal_resolution(display);
    const int32_t height = lv_display_get_vertical_resolution(display);
    const lv_color_format_t cf = lv_display_get_color_format(display);
    const size_t buf_size = (size_t)lv_draw_buf_width_to_stride(width, cf) * height;

    uint8_t * render_buf = lv_malloc(buf_size);
    if(!render_buf) {
        LV_LOG_ERROR("Failed to allocate render buffer for %" LV_PRId32 "x%" LV_PRId32, width, height);
        return NULL;
    }

    lv_wl_buffer_t * buffers = lv_malloc(LV_WL_DMABUF_BUF_COUNT * sizeof(*buffers));
    if(!buffers) {
        LV_LOG_WARN("failed to allocate memory for new wl buffers");
        lv_free(render_buf);
        return NULL;
    }
    if(!init_buffers(c, buffers, display, width, height)) {
        LV_LOG_ERROR("Failed to recreate DMA-BUFs for %" LV_PRId32 "x%" LV_PRId32, width, height);
        delete_buffers(buffers);
        lv_free(render_buf);
        return NULL;
    }

    delete_buffers(ddata->buffers);
    lv_free(ddata->render_buf);
    ddata->buffers = buffers;
    ddata->render_buf = render_buf;
    ddata->last_used = 0;
    ddata->width = width;
    ddata->height = height;

    lv_display_set_buffers(display, ddata->render_buf, NULL, buf_size,
                           LV_DISPLAY_RENDER_MODE_DIRECT);
    return ddata;
}

static void wl_dmabuf_deinit_display(void * backend_ctx, lv_display_t * display)
{
    LV_UNUSED(backend_ctx);
    lv_wl_dmabuf_display_data_t * ddata = lv_wayland_get_backend_display_data(display);
    if(!ddata) {
        return;
    }

    delete_buffers(ddata->buffers);
    lv_free(ddata->render_buf);

    LV_LOG_INFO("Deleted DMA-BUF display data");
    lv_free(ddata);
}

static void frame_done(void * data, struct wl_callback * callback, uint32_t time)
{
    LV_UNUSED(time);
    lv_display_t * display = data;
    wl_callback_destroy(callback);
    lv_display_flush_ready(display);
}

static void flush_wait_cb(lv_display_t * disp)
{
    while(disp->flushing) {
        wl_display_dispatch(lv_wl_ctx.wl_display);
    }
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
    lv_color_format_t cf = lv_display_get_color_format(disp);

    if(!lv_display_flush_is_last(disp)) {
        lv_display_flush_ready(disp);
        return;
    }

    lv_wl_dmabuf_display_data_t * ddata = lv_wayland_get_backend_display_data(disp);
    struct wl_surface * surface = lv_wayland_get_window_surface(disp);
    if(!surface) {
        lv_display_flush_ready(disp);
        return;
    }

    /* When using ARGB8888, the compositor expects premultiplied ARGB8888 so premultiply it here*/
    if(ddata->buffers[0].cf == DRM_FORMAT_ARGB8888 && cf != LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED) {
        const int32_t w = lv_area_get_width(area);
        const int32_t h = lv_area_get_height(area);
        const int32_t buf_w = lv_display_get_horizontal_resolution(disp);
        lv_color32_t * start_pos = (lv_color32_t *)px_map + (area->y1 * buf_w) + area->x1;

        for(int32_t y = 0; y < h; ++y) {
            size_t index = y * buf_w;
            for(int32_t x = 0; x < w; ++x) {
                lv_color_premultiply(start_pos + (index++));
            }
        }
    }

    lv_wl_buffer_t * buf = get_next_buffer(ddata);
    if(!buf) {
        LV_LOG_ERROR("Failed to acquire a wayland window body buffer");
        lv_display_flush_ready(disp);
        return;
    }

    if(!copy_frame_to_buffer(buf, px_map, disp)) {
        LV_LOG_ERROR("Failed to map the DMA-BUF for CPU access");
        lv_display_flush_ready(disp);
        return;
    }

    const int32_t phy_width = lv_display_get_original_horizontal_resolution(disp);
    const int32_t phy_height = lv_display_get_original_vertical_resolution(disp);

    struct wl_callback * callback = wl_surface_frame(surface);
    wl_callback_add_listener(callback, &frame_listener, disp);
    lv_wayland_dmabuf_buffer_attach(&ctx.dmabuf, surface, &buf->base, phy_width, phy_height);
}


static bool init_buffers(lv_wl_dmabuf_ctx_t * c, lv_wl_buffer_t * buffers, lv_display_t * display,
                         int32_t width, int32_t height)
{
    lv_memzero(buffers, sizeof(lv_wl_buffer_t) * LV_WL_DMABUF_BUF_COUNT);
    /* A buffer that init_buffer() never reaches must still report a closed fd,
     * otherwise cleaning up after a partial failure would close fd 0. */
    for(size_t i = 0; i < LV_WL_DMABUF_BUF_COUNT; i++) {
        buffers[i].dmabuf_fd = -1;
    }

    /* Under rotation LVGL renders at the rotated resolution while the compositor
     * is handed the physical one, so the DMA-BUFs follow the physical size and
     * the flush callback rotates into them. */
    const lv_display_rotation_t rotation = lv_display_get_rotation(display);
    const bool swap_axes = rotation == LV_DISPLAY_ROTATION_90 || rotation == LV_DISPLAY_ROTATION_270;
    const uint32_t buf_width = swap_axes ? (uint32_t)height : (uint32_t)width;
    const uint32_t buf_height = swap_axes ? (uint32_t)width : (uint32_t)height;

    const lv_color_format_t cf = lv_display_get_color_format(display);

    for(size_t i = 0; i < LV_WL_DMABUF_BUF_COUNT; i++) {
        if(!init_buffer(c, &buffers[i], buf_width, buf_height, cf)) {
            return false;
        }
    }
    lv_wayland_dmabuf_commit_creation(&c->dmabuf);

    for(size_t i = 0; i < LV_WL_DMABUF_BUF_COUNT; i++) {
        if(!lv_wayland_dmabuf_buffer_is_valid(&buffers[i].base)) {
            return false;
        }
    }

    return true;
}

static void delete_buffers(lv_wl_buffer_t * buffers)
{
    for(size_t i = 0; i < LV_WL_DMABUF_BUF_COUNT; i++) {
        delete_buffer(&buffers[i]);
    }
    lv_free(buffers);
}

static bool init_buffer(lv_wl_dmabuf_ctx_t * c, lv_wl_buffer_t * buffer, uint32_t width, uint32_t height,
                        lv_color_format_t cf)
{

    const uint32_t drm_cf = lv_wayland_dmabuf_cf_to_drm(cf);
    const uint32_t gbm_cf = lv_drm_cf_to_gbm_cf(drm_cf);
    if(drm_cf == 0 || gbm_cf == 0) {
        LV_LOG_WARN("unsupported color format 0x%02x", cf);
        return false;
    }
    const uint64_t linear_modifier = DRM_FORMAT_MOD_LINEAR;

    buffer->base.busy = false;
    buffer->dmabuf_fd = -1;
    buffer->cf = drm_cf;

    /*
     * We require a linear buffer so that we can memcpy to it in the flush callback
     * First try to request it by providing a modifier list
     * */
    buffer->bo = gbm_bo_create_with_modifiers(c->gbm_device, width, height, gbm_cf, &linear_modifier, 1);
    if(!buffer->bo) {
        /* Two reasons this could've failed:
        * 1. the driver doesn't support linear buffers
        * 2. the driver doesn't support modifier lists
        * Try getting on without providing a modifier list just in case
        * */
        buffer->bo = gbm_bo_create(c->gbm_device, width, height, gbm_cf,
                                   GBM_BO_USE_RENDERING | GBM_BO_USE_LINEAR);
    }
    if(!buffer->bo) {
        LV_LOG_WARN("Failed to create a linear GBM buffer object");
        return false;
    }

    /* Ensure we actually got a linear buffer */
    buffer->modifier = gbm_bo_get_modifier(buffer->bo);
    if(buffer->modifier != linear_modifier) {
        LV_LOG_WARN("GBM returned modifier 0x%" LV_PRIx64 " for a linear allocation", buffer->modifier);
        return false;
    }

    buffer->stride = gbm_bo_get_stride_for_plane(buffer->bo, 0);
    buffer->offset = gbm_bo_get_offset(buffer->bo, 0);
    buffer->dmabuf_fd = gbm_bo_get_fd(buffer->bo);
    if(buffer->dmabuf_fd < 0) {
        LV_LOG_ERROR("Failed to export the GBM buffer object as a DMA-BUF fd");
        return false;
    }

    LV_LOG_INFO("DMA-BUF %" LV_PRIu32 "x%" LV_PRIu32 " modifier 0x%" LV_PRIx64 " stride %" LV_PRIu32
                " offset %" LV_PRIu32, width, height, buffer->modifier, buffer->stride, buffer->offset);

    /* dummy map to ensure we can actually use this buffer afterwards */
    uint32_t map_stride = 0;
    void * map_data = NULL;
    void * map = gbm_bo_map(buffer->bo, 0, 0, width, height, GBM_BO_TRANSFER_WRITE, &map_stride, &map_data);
    if(!map) {
        LV_LOG_WARN("Failed to map the GBM buffer object for CPU access");
        return false;
    }
    gbm_bo_unmap(buffer->bo, map_data);

    lv_wayland_dmabuf_create_buffer(&c->dmabuf, &buffer->base, buffer->dmabuf_fd,
                                    width, height, buffer->stride, buffer->offset, drm_cf,
                                    buffer->modifier);
    return true;
}

static void delete_buffer(lv_wl_buffer_t * buffer)
{
    if(buffer->base.wl_buffer) {
        wl_buffer_destroy(buffer->base.wl_buffer);
        buffer->base.wl_buffer = NULL;
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

static lv_wl_buffer_t * get_next_buffer(lv_wl_dmabuf_display_data_t * ddata)
{
    struct pollfd pfd = { .fd = wl_display_get_fd(lv_wl_ctx.wl_display), .events = POLLIN };
    uint32_t remaining_ms = LV_WL_DMABUF_RELEASE_TIMEOUT_MS;

    for(;;) {
        for(size_t i = 0; i < LV_WL_DMABUF_BUF_COUNT; i++) {
            size_t index = (ddata->last_used + i) % LV_WL_DMABUF_BUF_COUNT;
            if(lv_wayland_dmabuf_buffer_is_ready(&ddata->buffers[index].base)) {
                ddata->last_used = (index + 1) % LV_WL_DMABUF_BUF_COUNT;
                return &ddata->buffers[index];
            }
        }

        if(remaining_ms == 0) {
            LV_LOG_WARN("Timed out waiting for a free DMA-BUF, dropping frame");
            return NULL;
        }

        /* All buffers are still owned by the compositor: flush pending requests
         * and wait (bounded) for it to release one via wl_buffer.release. */
        wl_display_flush(lv_wl_ctx.wl_display);
        if(wl_display_prepare_read(lv_wl_ctx.wl_display) == 0) {
            int step_ms = remaining_ms < 20 ? (int)remaining_ms : 20;
            int ret = poll(&pfd, 1, step_ms);
            remaining_ms -= (uint32_t)step_ms;
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

static bool copy_frame_to_buffer(lv_wl_buffer_t * buffer, const uint8_t * px_map, lv_display_t * disp)
{
    const uint32_t bo_width = gbm_bo_get_width(buffer->bo);
    const uint32_t bo_height = gbm_bo_get_height(buffer->bo);
    const lv_color_format_t cf = lv_display_get_color_format(disp);
    const lv_display_rotation_t rotation = lv_display_get_rotation(disp);

    const int32_t src_width = lv_display_get_horizontal_resolution(disp);
    const int32_t src_height = lv_display_get_vertical_resolution(disp);
    const uint32_t src_stride = lv_draw_buf_width_to_stride(src_width, cf);

    if(lv_display_get_original_horizontal_resolution(disp) > (int32_t)bo_width ||
       lv_display_get_original_vertical_resolution(disp) > (int32_t)bo_height) {
        LV_LOG_ERROR("Frame is larger than the DMA-BUF it is copied into");
        return false;
    }

    uint32_t dst_stride = 0;
    void * map_data = NULL;

    /* Map and unmap every frame to ensure the buffer is written and visible by the compositor */
    uint8_t * dst = gbm_bo_map(buffer->bo, 0, 0, bo_width, bo_height, GBM_BO_TRANSFER_WRITE,
                               &dst_stride, &map_data);
    if(!dst) {
        return false;
    }

    if(rotation != LV_DISPLAY_ROTATION_0) {
        lv_draw_sw_rotate(px_map, dst, src_width, src_height, src_stride, dst_stride, rotation, cf);
    }
    else if(dst_stride == src_stride) {
        /* The whole frame is copied rather than just the flushed area: in DIRECT
         * mode the render buffer LVGL keeps up to date is not the one this
         * DMA-BUF was last filled from, so a partial copy could leave stale
         * regions behind. */
        lv_memcpy(dst, px_map, (size_t)src_stride * src_height);
    }
    else {
        const uint32_t row_size = LV_MIN(src_stride, dst_stride);
        for(int32_t y = 0; y < src_height; y++) {
            lv_memcpy(dst + (size_t)y * dst_stride, px_map + (size_t)y * src_stride, row_size);
        }
    }

    gbm_bo_unmap(buffer->bo, map_data);
    return true;
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
            return 0;
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
        LV_LOG_ERROR("Failed to open a DRM device");
    }

    return fd;
}

#endif /*LV_WAYLAND_USE_DMABUF*/
