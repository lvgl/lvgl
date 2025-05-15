/*******************************************************************
 *
 * @file lv_wayland.c - The Wayland client for LVGL applications
 * Based on the original file from the repository.
 *
 * Porting to LVGL 9.1
 * EDGEMTech Ltd. by Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 * See LICENCE.txt for details
 *
 ******************************************************************/

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland.h"

#if LV_USE_WAYLAND

#include "lv_wayland_smm.h"
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

#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

#define SHM_FORMAT_UNKNOWN 0xFFFFFF

#if (LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1)
    #error[wayland] Unsupported LV_COLOR_DEPTH
#endif

/**********************
 *      TYPEDEFS
 **********************/

struct application application;

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

static const struct wl_registry_listener registry_listener = {
    .global        = handle_global,
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
    return wl_display_get_fd(application.display);
}

bool lv_wayland_timer_handler(void)
{
    struct window * window;

    /* Wayland input handling - it will also trigger the frame done handler */
    handle_input();

    /* Ready input timers (to probe for any input received) */
    LV_LL_READ(&application.window_ll, window) {
        LV_LOG_TRACE("handle timer frame: %d", window->frame_counter);

        if(window != NULL && window->frame_done == false && window->frame_counter > 0) {
            /* The last frame was not rendered */
            LV_LOG_TRACE("The window is hidden or minimized");

            /* Simply blocks until a frame done message arrives */
            poll(&application.wayland_pfd, 1, -1);

            /* Resume lvgl on the next cycle */
            return false;

        }
        else if(window != NULL && window->body->surface_configured == false) {
            /* Initial commit to trigger the configure event */
            /* Manually dispatching the queue is necessary, */
            /* to emit the configure event straight away */
            wl_surface_commit(window->body->surface);
            wl_display_dispatch(application.display);
        }
        else if(window != NULL && window->resize_pending) {
            if(lv_wayland_window_resize(window, window->resize_width, window->resize_height)) {
                window->resize_width   = window->width;
                window->resize_height  = window->height;
                window->resize_pending = false;

            }
            else {

                LV_LOG_TRACE("Failed to resize window frame: %d", window->frame_counter);
            }
        }
        else if(window->shall_close == true) {

            /* Destroy graphical context and execute close_cb */
            handle_output();
            lv_wayland_deinit();
            return false;
        }
    }

    /* LVGL handling */
    lv_timer_handler();

    /* Wayland output handling */
    handle_output();

    /* Set 'errno' if a Wayland flush is outstanding (i.e. data still needs to
     * be sent to the compositor, but the compositor pipe/connection is unable
     * to take more data at this time).
     */
    LV_LL_READ(&application.window_ll, window) {
        if(window->flush_pending) {
            errno = EAGAIN;
            break;
        }
    }

    return true;
}

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

void lv_wayland_init(void)
{
    // Create XKB context
    application.xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    LV_ASSERT_MSG(application.xkb_context, "failed to create XKB context");
    if(application.xkb_context == NULL) {
        return;
    }

    // Connect to Wayland display
    application.display = wl_display_connect(NULL);
    LV_ASSERT_MSG(application.display, "failed to connect to Wayland server");
    if(application.display == NULL) {
        return;
    }

    /* Add registry listener and wait for registry reception */
    application.shm_format = SHM_FORMAT_UNKNOWN;
    application.registry   = wl_display_get_registry(application.display);
    wl_registry_add_listener(application.registry, &registry_listener, &application);
    wl_display_dispatch(application.display);
    wl_display_roundtrip(application.display);

    LV_ASSERT_MSG(application.compositor, "Wayland compositor not available");
    if(application.compositor == NULL) {
        return;
    }

    LV_ASSERT_MSG(application.shm, "Wayland SHM not available");
    if(application.shm == NULL) {
        return;
    }

    LV_ASSERT_MSG((application.shm_format != SHM_FORMAT_UNKNOWN), "WL_SHM_FORMAT not available");
    if(application.shm_format == SHM_FORMAT_UNKNOWN) {
        LV_LOG_TRACE("Unable to match a suitable SHM format for selected LVGL color depth");
        return;
    }

    smm_init(lv_wayland_sme_get_events());
    smm_setctx(&application);

#ifdef LV_WAYLAND_WINDOW_DECORATIONS
    const char * env_disable_decorations = getenv("LV_WAYLAND_DISABLE_WINDOWDECORATION");
    application.opt_disable_decorations  = ((env_disable_decorations != NULL) && (env_disable_decorations[0] != '0'));
#endif

    lv_ll_init(&application.window_ll, sizeof(struct window));

    lv_tick_set_cb(tick_get_cb);

    /* Used to wait for events when the window is minimized or hidden */
    application.wayland_pfd.fd     = wl_display_get_fd(application.display);
    application.wayland_pfd.events = POLLIN;
}

void lv_wayland_deinit(void)
{
    struct window * window = NULL;

    LV_LL_READ(&application.window_ll, window) {
        if(!window->closed) {
            lv_wayland_window_destroy(window);
        }

        lv_draw_buf_destroy(window->lv_disp_draw_buf);
        lv_display_delete(window->lv_disp);
    }

    smm_deinit();

    if(application.shm) {
        wl_shm_destroy(application.shm);
    }

#if LV_WAYLAND_XDG_SHELL
    if(application.xdg_wm) {
        xdg_wm_base_destroy(application.xdg_wm);
    }
#endif

#if LV_WAYLAND_WL_SHELL
    if(application.wl_shell) {
        wl_shell_destroy(application.wl_shell);
    }
#endif

    if(application.wl_seat) {
        wl_seat_destroy(application.wl_seat);
    }

    if(application.subcompositor) {
        wl_subcompositor_destroy(application.subcompositor);
    }

    if(application.compositor) {
        wl_compositor_destroy(application.compositor);
    }

    wl_registry_destroy(application.registry);
    wl_display_flush(application.display);
    wl_display_disconnect(application.display);

    lv_ll_clear(&application.window_ll);
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
    struct application * app = data;

    LV_UNUSED(version);
    LV_UNUSED(data);

    if(strcmp(interface, wl_compositor_interface.name) == 0) {
        app->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
    }
    else if(strcmp(interface, wl_subcompositor_interface.name) == 0) {
        app->subcompositor = wl_registry_bind(registry, name, &wl_subcompositor_interface, 1);
    }
    else if(strcmp(interface, wl_shm_interface.name) == 0) {
        app->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
        wl_shm_add_listener(app->shm, lv_wayland_shm_get_listener(), app);
        app->cursor_theme = wl_cursor_theme_load(NULL, 32, app->shm);
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
        wl_display_dispatch_pending(application.display);
        prepare_read = wl_display_prepare_read(application.display);
    }
    wl_display_read_events(application.display);
    wl_display_dispatch_pending(application.display);
}

static void handle_output(void)
{
    struct window * window;
    bool shall_flush = application.cursor_flush_pending;

    LV_LL_READ(&application.window_ll, window) {
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
            if(window->application->pointer_obj == window->body) {
                window->application->pointer_obj = NULL;
            }

            window->body->input.keyboard.key   = 0;
            window->body->input.keyboard.state = LV_INDEV_STATE_RELEASED;
            if(window->application->keyboard_obj == window->body) {
                window->application->keyboard_obj = NULL;
            }
            lv_wayland_window_destroy(window);
        }

        shall_flush |= window->flush_pending;
    }

    if(shall_flush) {
        if(wl_display_flush(application.display) == -1) {
            if(errno != EAGAIN) {
                LV_LOG_ERROR("failed to flush wayland display");
            }
        }
        else {
            /* All data flushed */
            application.cursor_flush_pending = false;
            LV_LL_READ(&application.window_ll, window) {
                window->flush_pending = false;
            }
        }
    }
}

#endif /* LV_USE_WAYLAND */
