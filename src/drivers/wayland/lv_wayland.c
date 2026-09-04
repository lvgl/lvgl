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
        It's now the user's responsibility to generate their own window decorations. See `lv_win`
    #endif
#endif

#include LV_STDDEF_INCLUDE
#include LV_STDINT_INCLUDE
#include LV_STDBOOL_INCLUDE
#include "lv_wayland_private.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>
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

#ifdef WL_OUTPUT_NAME_SINCE_VERSION
    #define LV_WAYLAND_WL_OUTPUT_VERSION 4
#else
    #define LV_WAYLAND_WL_OUTPUT_VERSION 2
#endif


/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/

/* Timer callback to process Wayland compositor events without blocking the UI.
 * We use an independent timer so that we always read and flush compositor events even if LVGL
 * doesn't need to redraw anything.
 */
static void read_compositor_events_timer_cb(lv_timer_t * timer);

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

#ifdef WL_OUTPUT_NAME_SINCE_VERSION
    static void output_name(void * data, struct wl_output * output, const char * name);
    static void output_description(void * data, struct wl_output * output, const char * description);
#endif

static void xdg_output_logical_position(void * data, struct zxdg_output_v1 * xdg_output, int32_t x, int32_t y);
static void xdg_output_logical_size(void * data, struct zxdg_output_v1 * xdg_output, int32_t width, int32_t height);
static void xdg_output_done(void * data, struct zxdg_output_v1 * xdg_output);
static void xdg_output_name(void * data, struct zxdg_output_v1 * xdg_output, const char * name);
static void xdg_output_description(void * data, struct zxdg_output_v1 * xdg_output, const char * description);

/* Attaches an zxdg_output_v1 to 'info' so that the compositor reports its
 * connector name and logical size. No-op if the compositor doesn't support it. */
#ifdef WL_OUTPUT_NAME_SINCE_VERSION

static void output_name(void * data, struct wl_output * output, const char * name)
{
    LV_UNUSED(output);
    lv_wl_output_info_t * info = data;

    /* The core protocol is authoritative, xdg-output reports the same name */
    snprintf(info->name, sizeof(info->name), "%s", name);
}

static void output_description(void * data, struct wl_output * output, const char * description)
{
    /* A human readable description of the output,
     * Skipped since we identify outputs by their connector name */
    LV_UNUSED(data);
    LV_UNUSED(output);
    LV_UNUSED(description);
}

#endif /*WL_OUTPUT_NAME_SINCE_VERSION*/

static void output_bind_xdg_output(lv_wl_output_info_t * info);

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
    .scale = output_scale,
#ifdef WL_OUTPUT_NAME_SINCE_VERSION
    .name = output_name,
    .description = output_description,
#endif
};

static const struct zxdg_output_v1_listener xdg_output_listener = {
    .logical_position = xdg_output_logical_position,
    .logical_size = xdg_output_logical_size,
    .done = xdg_output_done,
    .name = xdg_output_name,
    .description = xdg_output_description
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

uint8_t lv_wayland_get_output_count(void)
{
    return lv_wl_ctx.wl_output_count;
}

const char * lv_wayland_get_output_name(uint8_t output)
{
    LV_CHECK_ARG_FORMAT_MSG(output < lv_wl_ctx.wl_output_count, return NULL,
                            "Invalid output '%d'. Expected '0'..'%d'",
                            output, lv_wl_ctx.wl_output_count - 1);

    const lv_wl_output_info_t * info = lv_wl_ctx.physical_outputs[output];

    return info->name;
}

bool lv_wayland_get_output_size(uint8_t output, int32_t * width, int32_t * height)
{
    LV_CHECK_ARG_FORMAT_MSG(output < lv_wl_ctx.wl_output_count, return false,
                            "Invalid output '%d'. Expected '0'..'%d'",
                            output, lv_wl_ctx.wl_output_count - 1);

    const lv_wl_output_info_t * info = lv_wl_ctx.physical_outputs[output];

    int32_t w, h;
    /* Prefer the logical size over the screen resolution */
    if(info->logical_width > 0 && info->logical_height > 0) {
        w = info->logical_width;
        h = info->logical_height;
    }
    else {
        w = info->width;
        h = info->height;
    }

    if(width) {
        *width = w;
    }
    if(height) {
        *height = h;
    }
    return w > 0 && h > 0;
}

int32_t lv_wayland_get_display_size(const char * name, int32_t * width, int32_t * height)
{
    LV_CHECK_ARG(name != NULL, return -1);

    for(uint8_t i = 0; i < lv_wl_ctx.wl_output_count; i++) {
        const char * output_name = lv_wayland_get_output_name(i);
        if(output_name == NULL || strcmp(name, output_name) != 0) {
            continue;
        }
        lv_wayland_get_output_size(i, width, height);
        return i;
    }

    LV_LOG_WARN("No output named '%s'", name);
    return -1;
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
    lv_wayland_backend_init_all();

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
    lv_wl_ctx.read_compositor_events_timer = lv_timer_create(read_compositor_events_timer_cb, LV_DEF_REFR_PERIOD, NULL);

    if(lv_wl_ctx.xdg_output_mgr) {
        for(uint8_t i = 0; i < lv_wl_ctx.wl_output_count; i++) {
            output_bind_xdg_output(lv_wl_ctx.physical_outputs[i]);
        }
        /* Wait for the name and logical size of every output to arrive, so that
         * `lv_wayland_get_display_size` can be used right after initialization */
        wl_display_roundtrip(lv_wl_ctx.wl_display);
    }
    else {
        LV_LOG_WARN("zxdg_output_manager_v1 is not available, "
                    "outputs can't be identified by their connector name");
    }

    is_wayland_initialized = true;
    return LV_RESULT_OK;
}

void lv_wayland_deinit(void)
{
    if(!is_wayland_initialized) {
        return;
    }

    lv_wayland_xdg_deinit();

    if(is_wayland_initialized) {
        lv_wayland_backend_deinit_all();
    }

    if(lv_wl_ctx.seat.wl_seat) {
        lv_wayland_seat_deinit(&lv_wl_ctx.seat);
        lv_wl_ctx.seat.wl_seat = NULL;
    }

    if(lv_wl_ctx.wl_registry) {
        wl_registry_destroy(lv_wl_ctx.wl_registry);
        lv_wl_ctx.wl_registry = NULL;
    }

    if(lv_wl_ctx.wl_shm) {
        wl_shm_destroy(lv_wl_ctx.wl_shm);
        lv_wl_ctx.wl_shm = NULL;
    }

    for(uint8_t i = 0; i < lv_wl_ctx.wl_output_count; ++i) {
        lv_wl_output_info_t * info = lv_wl_ctx.physical_outputs[i];
        if(info->xdg_output) {
            zxdg_output_v1_destroy(info->xdg_output);
        }
        if(info->wl_output) {
            wl_output_destroy(info->wl_output);
        }
        lv_free(info);
    }
    lv_free(lv_wl_ctx.physical_outputs);
    lv_wl_ctx.physical_outputs = NULL;
    lv_wl_ctx.wl_output_count = 0;

    if(lv_wl_ctx.xdg_output_mgr) {
        zxdg_output_manager_v1_destroy(lv_wl_ctx.xdg_output_mgr);
        lv_wl_ctx.xdg_output_mgr = NULL;
    }

    if(lv_wl_ctx.wl_compositor) {
        wl_compositor_destroy(lv_wl_ctx.wl_compositor);
        lv_wl_ctx.wl_compositor = NULL;
    }
    if(lv_wl_ctx.wl_display) {
        wl_display_disconnect(lv_wl_ctx.wl_display);
        lv_wl_ctx.wl_display = NULL;
    }

    if(lv_wl_ctx.read_compositor_events_timer) {
        lv_timer_delete(lv_wl_ctx.read_compositor_events_timer);
        lv_wl_ctx.read_compositor_events_timer = NULL;
    }

    lv_ll_clear(&lv_wl_ctx.window_ll);
    is_wayland_initialized = false;
}

void lv_wayland_flush(void)
{
    int ret;
    while((ret = wl_display_flush(lv_wl_ctx.wl_display)) == -1 && errno == EAGAIN) {
        struct pollfd pfd = {
            .fd = wl_display_get_fd(lv_wl_ctx.wl_display),
            .events = POLLOUT,
        };

        if(poll(&pfd, 1, -1) == -1) {
            LV_LOG_ERROR("poll failed: %s", strerror(errno));
            break;
        }
        /* Socket is writable now, loop back and try flush again */
    }
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

static void read_compositor_events_timer_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);

    lv_wayland_flush();

    while(wl_display_prepare_read(lv_wl_ctx.wl_display) != 0) {
        wl_display_dispatch_pending(lv_wl_ctx.wl_display);
    }

    struct pollfd fds = {
        .fd = wl_display_get_fd(lv_wl_ctx.wl_display),
        .events = POLLIN,
    };
    const bool is_event_ready = poll(&fds, 1, 0) > 0;
    if(!is_event_ready) {
        wl_display_cancel_read(lv_wl_ctx.wl_display);
        return;
    }
    wl_display_read_events(lv_wl_ctx.wl_display);
    wl_display_dispatch_pending(lv_wl_ctx.wl_display);
}

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

    LV_UNUSED(data);
    LV_UNUSED(model);
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

static void output_bind_xdg_output(lv_wl_output_info_t * info)
{
    if(!lv_wl_ctx.xdg_output_mgr || info->xdg_output || !info->wl_output) {
        return;
    }

    info->xdg_output = zxdg_output_manager_v1_get_xdg_output(lv_wl_ctx.xdg_output_mgr, info->wl_output);
    if(!info->xdg_output) {
        LV_LOG_WARN("Failed to get the xdg_output of an output");
        return;
    }

    zxdg_output_v1_add_listener(info->xdg_output, &xdg_output_listener, info);
}

static void xdg_output_logical_position(void * data, struct zxdg_output_v1 * xdg_output, int32_t x, int32_t y)
{
    LV_UNUSED(data);
    LV_UNUSED(xdg_output);
    LV_UNUSED(x);
    LV_UNUSED(y);
}

static void xdg_output_logical_size(void * data, struct zxdg_output_v1 * xdg_output, int32_t width, int32_t height)
{
    LV_UNUSED(xdg_output);
    lv_wl_output_info_t * info = data;
    info->logical_width = width;
    info->logical_height = height;
}

static void xdg_output_done(void * data, struct zxdg_output_v1 * xdg_output)
{
    /* Deprecated since version 3, the events above are applied immediately */
    LV_UNUSED(data);
    LV_UNUSED(xdg_output);
}

static void xdg_output_name(void * data, struct zxdg_output_v1 * xdg_output, const char * name)
{
    LV_UNUSED(xdg_output);
    lv_wl_output_info_t * info = data;

    /* Only useful below wl_output version 4, which reports the same name */
    if(info->name[0] == '\0') {
        snprintf(info->name, sizeof(info->name), "%s", name);
    }
}

static void xdg_output_description(void * data, struct zxdg_output_v1 * xdg_output, const char * description)
{
    /* A human readable description of the output, not stored since the driver
     * identifies outputs by their connector name */
    LV_UNUSED(data);
    LV_UNUSED(xdg_output);
    LV_UNUSED(description);
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
        lv_wl_output_info_t ** outputs = lv_realloc(ctx->physical_outputs,
                                                    (ctx->wl_output_count + 1) * sizeof(lv_wl_output_info_t *));
        if(!outputs) {
            LV_LOG_WARN("Failed to allocate memory for output");
            return;
        }
        ctx->physical_outputs = outputs;

        lv_wl_output_info_t * info = lv_zalloc(sizeof(lv_wl_output_info_t));
        if(!info) {
            LV_LOG_WARN("Failed to allocate memory for output");
            return;
        }

        info->wl_output = wl_registry_bind(registry, name, &wl_output_interface,
                                           LV_MIN(version, LV_WAYLAND_WL_OUTPUT_VERSION));
        ctx->physical_outputs[ctx->wl_output_count] = info;
        ctx->wl_output_count++;
        wl_output_add_listener(info->wl_output, &output_listener, info);

        output_bind_xdg_output(info);
    }
    else if(strcmp(interface, zxdg_output_manager_v1_interface.name) == 0) {
        ctx->xdg_output_mgr = wl_registry_bind(registry, name, &zxdg_output_manager_v1_interface, LV_MIN(version, 3));
    }

    lv_wayland_backend_global_handler(registry, name, interface, version);
}

static void handle_global_remove(void * data, struct wl_registry * registry, uint32_t name)
{
    LV_UNUSED(data);
    LV_UNUSED(registry);
    LV_UNUSED(name);
}

#endif /* LV_USE_WAYLAND */
