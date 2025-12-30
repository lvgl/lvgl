/**
 * @file lv_wayland.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland_private.h"

#if LV_USE_WAYLAND

#if (LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1)
    #error[wayland] Unsupported LV_COLOR_DEPTH
#endif

#ifdef LV_WAYLAND_WINDOW_DECORATIONS
    #if LV_WAYLAND_WINDOW_DECORATIONS == 1
        #warning LV_WAYLAND_WINDOW_DECORATIONS has been removed for v9.5. \
        It's now the user's responsability to generate their own window decorations. Server side window decorations will be \
        added before the v9.5 release.
    #endif
#endif

#include "lv_wayland_private.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <poll.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <wayland-client.h>
#include <wayland-cursor.h>
#include <xkbcommon/xkbcommon.h>

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/

static void handle_global(void * data, struct wl_registry * registry, uint32_t name, const char * interface,
                          uint32_t version);
static void handle_global_remove(void * data, struct wl_registry * registry, uint32_t name);

static uint32_t tick_get_cb(void);

static void output_scale(void * data, struct wl_output * output, int32_t factor);
static void output_mode(void * data, struct wl_output * output, uint32_t flags, int32_t width, int32_t height,
                        int32_t refresh);
static void output_done(void * data, struct wl_output * output);
static void output_geometry(void * data, struct wl_output * output, int32_t x, int32_t y, int32_t physical_width,
                            int32_t physical_height, int32_t subpixel, const char * make, const char * model, int32_t transform);

/**********************
 *  STATIC VARIABLES
 **********************/

static bool is_wayland_initialized = false;
lv_wl_ctx_t lv_wl_ctx;

static const struct wl_registry_listener registry_listener = {
    .global = handle_global,
    .global_remove = handle_global_remove
};

static const struct wl_output_listener output_listener = {
    .geometry = output_geometry,
    .mode = output_mode,
    .done = output_done,
    .scale = output_scale
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Get Wayland display file descriptor
 * @return Wayland display file descriptor
 */
int lv_wayland_get_fd(void)
{
    if(!is_wayland_initialized) {
        LV_LOG_ERROR("Wayland is not initialized");
        return -1;
    }
    return wl_display_get_fd(lv_wl_ctx.wl_display);
}

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

lv_result_t lv_wayland_init(void)
{

    if(is_wayland_initialized) {
        return LV_RESULT_OK;
    }
    lv_memset(&lv_wl_ctx, 0, sizeof(lv_wl_ctx));

    /* Connect to Wayland display */
    lv_wl_ctx.wl_display = wl_display_connect(NULL);
    if(!lv_wl_ctx.wl_display) {
        LV_LOG_ERROR("failed to connect to Wayland server");
        return LV_RESULT_INVALID;
    }

    lv_wl_ctx.backend_data = wl_backend_ops.init();

    /* Add registry listener and wait for registry reception */
    lv_wl_ctx.wl_registry = wl_display_get_registry(lv_wl_ctx.wl_display);
    wl_registry_add_listener(lv_wl_ctx.wl_registry, &registry_listener, &lv_wl_ctx);
    wl_display_dispatch(lv_wl_ctx.wl_display);
    wl_display_roundtrip(lv_wl_ctx.wl_display);

    LV_ASSERT_MSG(lv_wl_ctx.wl_compositor, "Wayland compositor not available");
    if(!lv_wl_ctx.wl_compositor) {
        LV_LOG_ERROR("Wayland compositor is not available");
        wl_display_disconnect(lv_wl_ctx.wl_display);
        lv_wl_ctx.wl_display = NULL;
        return LV_RESULT_INVALID;

    }

    lv_ll_init(&lv_wl_ctx.window_ll, sizeof(lv_wl_window_t));

    lv_tick_set_cb(tick_get_cb);

    is_wayland_initialized = true;
    return LV_RESULT_OK;
}

void lv_wayland_deinit(void)
{
    lv_wl_window_t * window = NULL;

    LV_LL_READ(&lv_wl_ctx.window_ll, window) {
        lv_wayland_window_delete(window);
    }

    lv_wayland_xdg_deinit();

    if(is_wayland_initialized) {
        wl_backend_ops.deinit(lv_wl_ctx.backend_data);
    }

    if(lv_wl_ctx.seat.wl_seat) {
        lv_wayland_seat_deinit(&lv_wl_ctx.seat);
        lv_wl_ctx.seat.wl_seat = NULL;
    }

    if(lv_wl_ctx.wl_registry) {
        wl_registry_destroy(lv_wl_ctx.wl_registry);
        lv_wl_ctx.wl_registry = NULL;
    }

    if(lv_wl_ctx.wl_compositor) {
        wl_compositor_destroy(lv_wl_ctx.wl_compositor);
        lv_wl_ctx.wl_compositor = NULL;
    }
    if(lv_wl_ctx.wl_display) {
        wl_display_disconnect(lv_wl_ctx.wl_display);
        lv_wl_ctx.wl_display = NULL;
    }

    lv_ll_clear(&lv_wl_ctx.window_ll);
    is_wayland_initialized = false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void output_geometry(void * data, struct wl_output * output, int32_t x, int32_t y, int32_t physical_width,
                            int32_t physical_height,
                            int32_t subpixel, const char * make, const char * model, int32_t transform)
{
    LV_UNUSED(output);
    LV_UNUSED(x);
    LV_UNUSED(y);
    LV_UNUSED(physical_width);
    LV_UNUSED(physical_height);
    LV_UNUSED(subpixel);
    LV_UNUSED(make);
    LV_UNUSED(transform);

    lv_wl_output_info_t * info = data;
    snprintf(info->name, sizeof(info->name), "%s", model);
}

static void output_mode(void * data, struct wl_output * wl_output, uint32_t flags, int32_t width, int32_t height,
                        int32_t refresh)
{
    LV_UNUSED(wl_output);

    lv_wl_output_info_t * info = data;

    if(flags & WL_OUTPUT_MODE_CURRENT) {
        info->height = height;
        info->width = width;
        info->refresh = refresh;
        info->flags = flags;
    }
}

static void output_done(void * data, struct wl_output * output)
{
    /* Called when all geometry/mode info for this output has been sent */
    LV_UNUSED(data);
    LV_UNUSED(output);
}

static void output_scale(void * data, struct wl_output * output, int32_t factor)
{
    LV_UNUSED(output);
    lv_wl_output_info_t * info = data;
    info->scale = factor;
}

static uint32_t tick_get_cb(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    uint64_t time_ms = t.tv_sec * 1000 + (t.tv_nsec / 1000000);
    return time_ms;
}
static void handle_global(void * data, struct wl_registry * registry, uint32_t name, const char * interface,
                          uint32_t version)
{
    lv_wl_ctx_t * ctx = data;

    LV_UNUSED(data);

    if(strcmp(interface, wl_compositor_interface.name) == 0) {
        ctx->wl_compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
    }
    else if(strcmp(interface, wl_shm_interface.name) == 0) {
        /* Regardless of the backend, we always need SHM for the pointer cursor*/
        ctx->wl_shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    }
    else if(strcmp(interface, wl_seat_interface.name) == 0) {
        lv_wayland_seat_init(&ctx->seat, registry, name, version);
    }
    else if(strcmp(interface, xdg_wm_base_interface.name) == 0) {
        ctx->xdg_wm = wl_registry_bind(ctx->wl_registry, name, &xdg_wm_base_interface, LV_MIN(version, 2));
        xdg_wm_base_add_listener(ctx->xdg_wm, lv_wayland_xdg_get_wm_base_listener(), ctx);
    }
    else if(strcmp(interface, wl_output_interface.name) == 0) {
        if(ctx->wl_output_count < LV_WAYLAND_MAX_OUTPUTS) {
            memset(&ctx->physical_outputs[ctx->wl_output_count], 0, sizeof(lv_wl_output_info_t));
            struct wl_output * out = wl_registry_bind(registry, name, &wl_output_interface, 1);
            ctx->physical_outputs[ctx->wl_output_count].wl_output = out;
            wl_output_add_listener(out, &output_listener, &ctx->physical_outputs[ctx->wl_output_count].wl_output);
            ctx->wl_output_count++;
        }
    }

    wl_backend_ops.global_handler(lv_wl_ctx.backend_data, registry, name, interface, version);
}

static void handle_global_remove(void * data, struct wl_registry * registry, uint32_t name)
{
    LV_UNUSED(data);
    LV_UNUSED(registry);
    LV_UNUSED(name);
}

#endif /* LV_USE_WAYLAND */
