/**
 * @file lv_wayland.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland.h"

#if LV_USE_WAYLAND

#if LV_WAYLAND_BUF_COUNT < 1 || LV_WAYLAND_BUF_COUNT > 2
    #error "Invalid LV_WAYLAND_BUF_COUNT. Expected either 1 or 2"
#endif

#if !LV_WAYLAND_USE_DMABUF && LV_WAYLAND_BUF_COUNT != 1
    #error "Wayland doesn't support more than 1 LV_WAYLAND_BUF_COUNT without DMABUF"
#endif

#if LV_WAYLAND_USE_DMABUF && LV_WAYLAND_BUF_COUNT != 2
    #error "Wayland with DMABUF only supports 2 LV_WAYLAND_BUF_COUNT"
#endif

#if LV_WAYLAND_USE_DMABUF && !LV_USE_DRAW_G2D
    #error "LV_WAYLAND_USE_DMABUF requires LV_USE_DRAW_G2D"
#endif

#ifndef LV_DISPLAY_RENDER_MODE_PARTIAL
    /* FIXME: Hacky fix else building fails with -Wundef=error*/
    #define LV_DISPLAY_RENDER_MODE_PARTIAL 0
    #define LV_DISPLAY_RENDER_MODE_DIRECT 1
    #define LV_DISPLAY_RENDER_MODE_FULL 2
#endif

#if LV_WAYLAND_USE_DMABUF && LV_WAYLAND_RENDER_MODE == LV_DISPLAY_RENDER_MODE_PARTIAL
    #error "LV_WAYLAND_USE_DMABUF doesn't support LV_DISPLAY_RENDER_MODE_PARTIAL"
#endif

#if !LV_WAYLAND_USE_DMABUF && LV_WAYLAND_RENDER_MODE != LV_DISPLAY_RENDER_MODE_PARTIAL
    #error "Wayland without DMABUF only supports LV_DISPLAY_RENDER_MODE_PARTIAL"
#endif

#if (LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1)
    #error[wayland] Unsupported LV_COLOR_DEPTH
#endif

#include "lv_wayland_private.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <poll.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <wayland-client.h>
#include <wayland-cursor.h>
#include <xkbcommon/xkbcommon.h>

#if LV_WAYLAND_USE_DMABUF
    #include <wayland_linux_dmabuf.h>
#endif

#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

struct lv_wayland_context lv_wl_ctx;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void handle_global(void * data, struct wl_registry * registry, uint32_t name, const char * interface,
                          uint32_t version);
static void handle_global_remove(void * data, struct wl_registry * registry, uint32_t name);
static void handle_input(void);
static void handle_output(void);

static uint32_t tick_get_cb(void);

/**********************
 *  STATIC VARIABLES
 **********************/

static bool is_wayland_initialized                         = false;
static const struct wl_registry_listener registry_listener = {.global        = handle_global,
           .global_remove = handle_global_remove
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
    return wl_display_get_fd(lv_wl_ctx.display);
}

uint32_t lv_wayland_timer_handler(void)
{
    struct window * window;

    /* Wayland input handling - it will also trigger the frame done handler */
    handle_input();

    /* Ready input timers (to probe for any input received) */
    LV_LL_READ(&lv_wl_ctx.window_ll, window) {
        LV_LOG_TRACE("handle timer frame: %d", window->frame_counter);

        if(window != NULL && window->resize_pending) {
#if LV_WAYLAND_USE_DMABUF
            /* Check surface configuration state before resizing */
            if(!window->surface_configured) {
                LV_LOG_TRACE("Deferring resize - surface not configured yet");
                continue;
            }
#endif
            LV_LOG_TRACE("Processing resize: %dx%d -> %dx%d",
                         window->width, window->height,
                         window->resize_width, window->resize_height);

            if(lv_wayland_window_resize(window, window->resize_width, window->resize_height) == LV_RESULT_OK) {
                window->resize_width   = window->width;
                window->resize_height  = window->height;
                window->resize_pending = false;
#if LV_WAYLAND_USE_DMABUF
                /* Reset synchronization flags after successful resize */
                window->surface_configured = false;
                window->dmabuf_resize_pending = false;
#endif
                LV_LOG_TRACE("Window resize completed successfully: %dx%d",
                             window->width, window->height);
            }
            else {
                LV_LOG_ERROR("Failed to resize window frame: %d", window->frame_counter);
            }
        }
        else if(window->shall_close == true) {

            /* Destroy graphical context and execute close_cb */
            handle_output();
            lv_wayland_deinit();
            return 0;
        }
    }

    /* LVGL handling */
    uint32_t idle_time = lv_timer_handler();

    /* Wayland output handling */
    handle_output();

    /* Set 'errno' if a Wayland flush is outstanding (i.e. data still needs to
     * be sent to the compositor, but the compositor pipe/connection is unable
     * to take more data at this time).
     */
    LV_LL_READ(&lv_wl_ctx.window_ll, window) {
        if(window->flush_pending) {
            errno = EAGAIN;
            break;
        }
    }

    return idle_time;
}

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

void lv_wayland_init(void)
{

    if(is_wayland_initialized) {
        return;
    }

    // Create XKB context
    lv_wl_ctx.xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    LV_ASSERT_MSG(lv_wl_ctx.xkb_context, "failed to create XKB context");
    if(lv_wl_ctx.xkb_context == NULL) {
        return;
    }

    // Connect to Wayland display
    lv_wl_ctx.display = wl_display_connect(NULL);
    LV_ASSERT_MSG(lv_wl_ctx.display, "failed to connect to Wayland server");
    if(lv_wl_ctx.display == NULL) {
        return;
    }

#if LV_WAYLAND_USE_DMABUF
    lv_wayland_dmabuf_initalize_context(&lv_wl_ctx.dmabuf_ctx);
#endif
    lv_wayland_shm_initalize_context(&lv_wl_ctx.shm_ctx);

    /* Add registry listener and wait for registry reception */
    lv_wl_ctx.registry = wl_display_get_registry(lv_wl_ctx.display);
    wl_registry_add_listener(lv_wl_ctx.registry, &registry_listener, &lv_wl_ctx);
    wl_display_dispatch(lv_wl_ctx.display);
    wl_display_roundtrip(lv_wl_ctx.display);

    LV_ASSERT_MSG(lv_wl_ctx.compositor, "Wayland compositor not available");
    if(lv_wl_ctx.compositor == NULL) {
        return;
    }

    bool shm_ready = lv_wayland_shm_is_ready(&lv_wl_ctx.shm_ctx);
    LV_ASSERT_MSG(shm_ready, "Couldn't initialize wayland SHM");
    if(!shm_ready) {
        LV_LOG_ERROR("Couldn't initialize wayland SHM");
        return;
    }
    lv_wl_ctx.cursor_theme = lv_wayland_shm_load_cursor_theme(&lv_wl_ctx.shm_ctx);
    if(!lv_wl_ctx.cursor_theme) {
        LV_LOG_WARN("Failed to initialize the cursor theme");
    }

#if LV_WAYLAND_USE_DMABUF
    bool dmabuf_ready = lv_wayland_dmabuf_is_ready(&lv_wl_ctx.dmabuf_ctx);
    LV_ASSERT_MSG(dmabuf_ready, "Couldn't initialize wayland DMABUF");
    if(!dmabuf_ready) {
        LV_LOG_ERROR("Couldn't initialize wayland DMABUF");
        return;
    }
#endif

#ifdef LV_WAYLAND_WINDOW_DECORATIONS
    const char * env_disable_decorations = getenv("LV_WAYLAND_DISABLE_WINDOWDECORATION");
    lv_wl_ctx.opt_disable_decorations  = ((env_disable_decorations != NULL) && (env_disable_decorations[0] != '0'));
#endif

    lv_ll_init(&lv_wl_ctx.window_ll, sizeof(struct window));

    lv_tick_set_cb(tick_get_cb);

    /* Used to wait for events when the window is minimized or hidden */
    lv_wl_ctx.wayland_pfd.fd     = wl_display_get_fd(lv_wl_ctx.display);
    lv_wl_ctx.wayland_pfd.events = POLLIN;

    is_wayland_initialized = true;
}

void lv_wayland_deinit(void)
{
    struct window * window = NULL;

    LV_LL_READ(&lv_wl_ctx.window_ll, window) {
        if(!window->closed) {
            lv_wayland_window_destroy(window);
        }

        /* TODO: This should probably be moved inside lv_wayland_window_destroy but not sure about the if condition */
#if LV_WAYLAND_USE_DMABUF
        lv_wayland_dmabuf_destroy_draw_buffers(&lv_wl_ctx.dmabuf_ctx, window);
#else
        lv_wayland_shm_delete_draw_buffers(&lv_wl_ctx.shm_ctx, window);
#endif
        lv_display_delete(window->lv_disp);
    }

    lv_wayland_shm_deinit(&lv_wl_ctx.shm_ctx);
#if LV_WAYLAND_USE_DMABUF
    lv_wayland_dmabuf_deinit(&lv_wl_ctx.dmabuf_ctx);
#endif

#if LV_WAYLAND_WL_SHELL
    lv_wayland_wl_shell_deinit();
#elif LV_WAYLAND_XDG_SHELL
    lv_wayland_xdg_shell_deinit();
#endif

    if(lv_wl_ctx.wl_seat) {
        wl_seat_destroy(lv_wl_ctx.wl_seat);
    }

    if(lv_wl_ctx.subcompositor) {
        wl_subcompositor_destroy(lv_wl_ctx.subcompositor);
    }

    if(lv_wl_ctx.compositor) {
        wl_compositor_destroy(lv_wl_ctx.compositor);
    }

    wl_registry_destroy(lv_wl_ctx.registry);
    wl_display_flush(lv_wl_ctx.display);
    wl_display_disconnect(lv_wl_ctx.display);

    lv_ll_clear(&lv_wl_ctx.window_ll);
}

void lv_wayland_wait_flush_cb(lv_display_t * disp)
{
    struct window * window = lv_display_get_user_data(disp);
    /* TODO: Figure out why we need this */
    if(window->frame_counter == 0) {
        return;
    }
    uint32_t initial_frame_counter = window->frame_counter;
    while(initial_frame_counter == window->frame_counter) {
        poll(&lv_wl_ctx.wayland_pfd, 1, -1);
        handle_input();
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

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
    struct lv_wayland_context * app = data;

    LV_UNUSED(version);
    LV_UNUSED(data);

    if(strcmp(interface, wl_compositor_interface.name) == 0) {
        app->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
    }
    else if(strcmp(interface, wl_subcompositor_interface.name) == 0) {
        app->subcompositor = wl_registry_bind(registry, name, &wl_subcompositor_interface, 1);
    }
    else if(strcmp(interface, wl_shm_interface.name) == 0) {

        lv_wayland_shm_set_interface(&app->shm_ctx, app->registry, name, interface, version);

    }
    else if(strcmp(interface, wl_seat_interface.name) == 0) {
        app->wl_seat = wl_registry_bind(app->registry, name, &wl_seat_interface, 1);
        wl_seat_add_listener(app->wl_seat, lv_wayland_seat_get_listener(), app);
    }
#if LV_WAYLAND_WL_SHELL
    else if(strcmp(interface, wl_shell_interface.name) == 0) {
        app->wl_shell = wl_registry_bind(registry, name, &wl_shell_interface, 1);
    }
#endif
#if LV_WAYLAND_XDG_SHELL
    else if(strcmp(interface, xdg_wm_base_interface.name) == 0) {
        /* supporting version 2 of the XDG protocol - ensures greater compatibility */
        app->xdg_wm = wl_registry_bind(app->registry, name, &xdg_wm_base_interface, 2);
        xdg_wm_base_add_listener(app->xdg_wm, lv_wayland_xdg_shell_get_wm_base_listener(), app);
    }
#endif
#if LV_WAYLAND_USE_DMABUF
    else if(strcmp(interface, zwp_linux_dmabuf_v1_interface.name) == 0) {
        lv_wayland_dmabuf_set_interface(&app->dmabuf_ctx, app->registry, name, interface, version);

        wl_display_roundtrip(app->display);
    }
#endif
}

static void handle_global_remove(void * data, struct wl_registry * registry, uint32_t name)
{

    LV_UNUSED(data);
    LV_UNUSED(registry);
    LV_UNUSED(name);
}

static void handle_input(void)
{
    int prepare_read = -1;

    while(prepare_read != 0) {
        wl_display_dispatch_pending(lv_wl_ctx.display);
        prepare_read = wl_display_prepare_read(lv_wl_ctx.display);
    }
    wl_display_read_events(lv_wl_ctx.display);
    wl_display_dispatch_pending(lv_wl_ctx.display);
}

static void handle_output(void)
{
    struct window * window;
    bool shall_flush = lv_wl_ctx.cursor_flush_pending;

    LV_LL_READ(&lv_wl_ctx.window_ll, window) {
        if((window->shall_close) && (window->close_cb != NULL)) {
            window->shall_close = window->close_cb(window->lv_disp);
        }

        if(window->closed) {
            continue;
        }
        else if(window->shall_close) {
            window->closed      = true;
            window->shall_close = false;
            shall_flush         = true;

            window->body->input.pointer.x            = 0;
            window->body->input.pointer.y            = 0;
            window->body->input.pointer.left_button  = LV_INDEV_STATE_RELEASED;
            window->body->input.pointer.right_button = LV_INDEV_STATE_RELEASED;
            window->body->input.pointer.wheel_button = LV_INDEV_STATE_RELEASED;
            window->body->input.pointer.wheel_diff   = 0;
            if(window->wl_ctx->pointer_obj == window->body) {
                window->wl_ctx->pointer_obj = NULL;
            }

            window->body->input.keyboard.key   = 0;
            window->body->input.keyboard.state = LV_INDEV_STATE_RELEASED;
            if(window->wl_ctx->keyboard_obj == window->body) {
                window->wl_ctx->keyboard_obj = NULL;
            }
            lv_wayland_window_destroy(window);
        }

        shall_flush |= window->flush_pending;
    }

    if(shall_flush) {
        if(wl_display_flush(lv_wl_ctx.display) == -1) {
            if(errno != EAGAIN) {
                LV_LOG_ERROR("failed to flush wayland display");
            }
        }
        else {
            /* All data flushed */
            lv_wl_ctx.cursor_flush_pending = false;
            LV_LL_READ(&lv_wl_ctx.window_ll, window) {
                window->flush_pending = false;
            }
        }
    }
}

#endif /* LV_USE_WAYLAND */
