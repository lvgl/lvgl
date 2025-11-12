/**
 * @file lv_wl_shm_backend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland_private.h"

#if LV_WAYLAND_USE_SHM

#include "../../display/lv_display_private.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

/*********************
 *      DEFINES
 *********************/

#define LV_WL_SHM_BUF_COUNT 2

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    struct wl_shm * shm;
} lv_wl_shm_ctx_t;

typedef struct {
    struct wl_buffer * wl_buffer;
    bool busy;
} lv_wl_buffer_t;

typedef struct {
    void * mmap_ptr;
    size_t mmap_size;
    struct wl_shm_pool * pool;
    lv_wl_buffer_t buffers[LV_WL_SHM_BUF_COUNT];
    size_t curr_wl_buffer_idx;
    uint32_t shm_cf;
    int fd;
    bool delete_on_release;
} lv_wl_shm_display_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * shm_init(void);
static void shm_deinit(void *);
static void * shm_init_display(void * backend_ctx, lv_display_t * display, int32_t width, int32_t height);
static void * shm_resize_display(void * backend_ctx, lv_display_t * display);
static void shm_deinit_display(void * backend_ctx, lv_display_t * display);
static void shm_global_handler(void * backend_ctx, struct wl_registry * registry, uint32_t name,
                               const char * interface, uint32_t version);

static int create_shm_file(size_t size);
static void shm_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

static lv_wl_shm_display_data_t * shm_create_display_data(lv_wl_shm_ctx_t * ctx, lv_display_t * display, int32_t width,
                                                          int32_t height, size_t buf_count);
static void shm_destroy_display_data(lv_wl_shm_display_data_t * ddata);

static void frame_done(void * data, struct wl_callback * callback, uint32_t time);
static void buffer_release(void * data, struct wl_buffer * wl_buffer);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_wl_shm_ctx_t shm_ctx;

static const struct wl_callback_listener frame_listener = {
    .done = frame_done,
};

static const struct wl_buffer_listener buffer_listener = {
    .release = buffer_release
};

const lv_wayland_backend_ops_t wl_backend_ops = {
    .init = shm_init,
    .deinit = shm_deinit,
    .global_handler = shm_global_handler,
    .init_display = shm_init_display,
    .deinit_display = shm_deinit_display,
    .resize_display = shm_resize_display,
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

static void buffer_release(void * data, struct wl_buffer * wl_buffer)
{
    lv_wl_shm_display_data_t * ddata = data;
    for(size_t i = 0; i < LV_WL_SHM_BUF_COUNT; ++i) {
        if(wl_buffer == ddata->buffers[i].wl_buffer) {
            ddata->buffers[i].busy = false;
        }
    }

    if(ddata->delete_on_release) {
        shm_destroy_display_data(ddata);
    }
}

static void frame_done(void * data, struct wl_callback * callback, uint32_t time)
{
    LV_UNUSED(time);
    lv_display_t * display = data;
    wl_callback_destroy(callback);
    lv_display_flush_ready(display);
}

static uint32_t lv_cf_to_shm_cf(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
        case LV_COLOR_FORMAT_ARGB8888:
            return WL_SHM_FORMAT_ARGB8888;
        case LV_COLOR_FORMAT_XRGB8888:
            return WL_SHM_FORMAT_XRGB8888;
        case LV_COLOR_FORMAT_RGB565:
            return WL_SHM_FORMAT_RGB565;
        default:
            return 0;
    }
}

static void * shm_init(void)
{
    lv_memzero(&shm_ctx, sizeof(shm_ctx));
    return &shm_ctx;
}

static void shm_deinit(void * backend_ctx)
{
    lv_wl_shm_ctx_t * ctx = backend_ctx;
    if(ctx->shm) {
        wl_shm_destroy(ctx->shm);
    }
}

static lv_wl_shm_display_data_t * shm_create_display_data(lv_wl_shm_ctx_t * ctx, lv_display_t * display, int32_t width,
                                                          int32_t height, size_t buf_count)
{
    lv_wl_shm_display_data_t * ddata = lv_zalloc(sizeof(*ddata));
    if(!ddata) {
        LV_LOG_ERROR("Failed to allocate data for display");
        return NULL;
    }

    lv_color_format_t cf = lv_display_get_color_format(display);
    ddata->shm_cf = lv_cf_to_shm_cf(cf);

    if(!ddata->shm_cf) {
        LV_LOG_WARN("Unsupported color format %d. Falling back to XRGB8888", cf);
        cf = LV_COLOR_FORMAT_XRGB8888;
        lv_display_set_color_format(display, cf);
        ddata->shm_cf = WL_SHM_FORMAT_XRGB8888;
    }

    const uint32_t stride = lv_draw_buf_width_to_stride(width, cf);
    const size_t buf_size = stride * height;

    ddata->mmap_size = buf_size * buf_count;

    ddata->fd = create_shm_file(ddata->mmap_size);
    if(ddata->fd < 0) {
        LV_LOG_ERROR("Failed to create shm file");
        goto shm_file_err;
    }
    ddata->mmap_ptr = mmap(NULL, ddata->mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, ddata->fd, 0);
    if(ddata->mmap_ptr == MAP_FAILED) {
        LV_LOG_ERROR("Failed to map shm file: %s", strerror(errno));
        goto mmap_err;
    }
    ddata->pool = wl_shm_create_pool(ctx->shm, ddata->fd, ddata->mmap_size);
    if(!ddata->pool) {
        LV_LOG_ERROR("Failed to create wl_shm_pool");
        goto shm_pool_err;
    }

    for(size_t i = 0; i < buf_count; ++i) {
        size_t offset = i * buf_size;
        ddata->buffers[i].wl_buffer = wl_shm_pool_create_buffer(ddata->pool, offset, width, height, stride, ddata->shm_cf);
        if(!ddata->buffers[i].wl_buffer) {
            LV_LOG_ERROR("Failed to create wl_buffer %zu", i);
            goto pool_buffer_err;
        }
        wl_buffer_add_listener(ddata->buffers[i].wl_buffer, &buffer_listener, ddata);
        ddata->buffers[i].busy = false;
    }

    lv_display_set_buffers(display, ddata->mmap_ptr, (uint8_t *)ddata->mmap_ptr + buf_size,
                           buf_size, LV_DISPLAY_RENDER_MODE_DIRECT);

    return ddata;

pool_buffer_err:
    wl_shm_pool_destroy(ddata->pool);
shm_pool_err:
    munmap(ddata->mmap_ptr, ddata->mmap_size);
mmap_err:
    close(ddata->fd);
shm_file_err:
    lv_free(ddata);
    return NULL;
}

static void shm_destroy_display_data(lv_wl_shm_display_data_t * ddata)
{
    for(size_t i = 0; i < LV_WL_SHM_BUF_COUNT; ++i) {
        lv_wl_buffer_t * buffer = &ddata->buffers[i];
        if(!buffer->wl_buffer) {
            continue;
        }
        if(buffer->busy) {
            /* Defer the deletion of this display data until the buffers are released */
            LV_LOG_INFO("Buffer is still busy, deferring deletion to when its released");
            ddata->delete_on_release = true;
            return;
        }
        wl_buffer_destroy(ddata->buffers[i].wl_buffer);
        ddata->buffers[i].wl_buffer = NULL;
    }

    if(ddata->pool) {
        wl_shm_pool_destroy(ddata->pool);
        ddata->pool = NULL;
    }


    if(ddata->mmap_ptr != MAP_FAILED) {
        munmap(ddata->mmap_ptr, ddata->mmap_size);
        ddata->mmap_ptr = MAP_FAILED;
    }

    if(ddata->fd >= 0) {
        close(ddata->fd);
        ddata->fd = -1;
    }

    LV_LOG_INFO("Deleted buffers and display data");
    lv_free(ddata);
}

static void flush_wait_cb(lv_display_t * disp)
{
    while(disp->flushing) {
        wl_display_dispatch(lv_wl_ctx.wl_display);
    }
}

static void * shm_init_display(void * backend_ctx, lv_display_t * display, int32_t width, int32_t height)
{
    lv_wl_shm_ctx_t * ctx = (lv_wl_shm_ctx_t *)backend_ctx;
    if(!ctx->shm) {
        LV_LOG_ERROR("wl_shm not available");
        return NULL;
    }

    lv_wl_shm_display_data_t * ddata = shm_create_display_data(ctx, display, width, height, LV_WL_SHM_BUF_COUNT);
    if(!ddata) {
        LV_LOG_ERROR("Failed to allocate data for display");
        return NULL;
    }

    lv_display_set_flush_cb(display, shm_flush_cb);
    lv_display_set_flush_wait_cb(display, flush_wait_cb);

    return ddata;
}

static void * shm_resize_display(void * backend_ctx, lv_display_t * display)
{
    lv_wl_shm_ctx_t * ctx = (lv_wl_shm_ctx_t *)backend_ctx;

    const int32_t new_width = lv_display_get_horizontal_resolution(display);
    const int32_t new_height = lv_display_get_vertical_resolution(display);

    lv_wl_shm_display_data_t * ddata = shm_create_display_data(ctx, display, new_width, new_height, LV_WL_SHM_BUF_COUNT);

    if(!ddata) {
        LV_LOG_ERROR("Failed to allocate data for new display resolution");
        return NULL;
    }

    lv_wl_shm_display_data_t * curr_ddata = lv_wayland_get_backend_display_data(display);
    shm_destroy_display_data(curr_ddata);
    return ddata;
}


static void shm_deinit_display(void * backend_ctx, lv_display_t * display)
{
    LV_UNUSED(backend_ctx);
    lv_wl_shm_display_data_t * ddata = lv_wayland_get_backend_display_data(display);
    if(!ddata) {
        return;
    }
    shm_destroy_display_data(ddata);
}

static int create_shm_file(size_t size)
{
    int fd = -1;
    char name[255];
    snprintf(name, sizeof(name), "/lvgl-wayland-%d-%ld", getpid(), (long)lv_tick_get());

    fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
    if(fd < 0) {
        LV_LOG_ERROR("shm_open failed: %s", strerror(errno));
        return -1;
    }

    shm_unlink(name);

    if(ftruncate(fd, size) < 0) {
        LV_LOG_ERROR("ftruncate failed: %s", strerror(errno));
        close(fd);
        return -1;
    }
    return fd;
}

static void shm_global_handler(void * backend_ctx, struct wl_registry * registry, uint32_t name,
                               const char * interface, uint32_t version)
{
    LV_UNUSED(version);
    lv_wl_shm_ctx_t * ctx = (lv_wl_shm_ctx_t *)backend_ctx;

    if(lv_streq(interface, wl_shm_interface.name)) {
        ctx->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    }
}

static void shm_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(px_map);
    lv_wl_shm_display_data_t * ddata = lv_wayland_get_backend_display_data(disp);
    struct wl_surface * surface = lv_wayland_get_window_surface(disp);
    if(!surface) {
        lv_display_flush_ready(disp);
        return;
    }

    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);
    lv_color_format_t cf = lv_display_get_color_format(disp);
    /* When using ARGB8888, the compositor expects premultiplied ARGB8888 so premultiply it here*/
    if(ddata->shm_cf == WL_SHM_FORMAT_ARGB8888 && cf != LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED) {
        size_t index = 0;
        for(int32_t y = area->y1; y <= area->y2; ++y) {
            for(int32_t x = area->x1; x <= area->x2; ++x) {
                lv_color_premultiply((lv_color32_t *) px_map + (index++));
            }
        }
    }

    wl_surface_damage(surface, area->x1, area->y1, w, h);
    if(!lv_display_flush_is_last(disp)) {
        lv_display_flush_ready(disp);
        return;
    }

    struct wl_callback * callback = wl_surface_frame(surface);
    wl_callback_add_listener(callback, &frame_listener, disp);

    lv_wl_buffer_t * buffer = &ddata->buffers[ddata->curr_wl_buffer_idx];
    if(buffer->busy) {
        LV_LOG_WARN("Failed to acquire a non-busy buffer");
    }
    wl_surface_attach(surface, buffer->wl_buffer, 0, 0);
    wl_surface_commit(surface);

    buffer->busy = true;
    ddata->curr_wl_buffer_idx = (ddata->curr_wl_buffer_idx + 1) % LV_WL_SHM_BUF_COUNT;
}

#endif /*LV_WAYLAND_USE_SHM*/
