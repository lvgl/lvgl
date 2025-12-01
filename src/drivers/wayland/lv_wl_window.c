/**
 * @file lv_wl_window.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_wl_window.h"

#if LV_USE_WAYLAND

#include "../../lv_init.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "lv_wayland_private.h"
#include "lv_wayland_private.h"
#include "lv_wl_pointer.h"
#include "lv_wl_pointer_axis.h"
#include "lv_wl_touch.h"
#include "lv_wl_keyboard.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void refr_start_event(lv_event_t * e);
static void refr_end_event(lv_event_t * e);
static void res_changed_event(lv_event_t * e);
static void delete_event(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_wayland_window_create(uint32_t hor_res, uint32_t ver_res, char * title,
                                        lv_wayland_display_close_cb_t close_cb)
{
    lv_wayland_init();
    if(close_cb) {
        LV_LOG_WARN("'lv_wayland_display_close_cb_t' is deprecated and will be removed in the next release. Instead bind an LV_EVENT_DELETE to the display\
		      Bind an LV_EVENT_DELETE to the display returned by `lv_wayland_window_create` instead.");
    }

    lv_wl_window_t * window = lv_ll_ins_tail(&lv_wl_ctx.window_ll);
    LV_ASSERT_MALLOC(window);
    if(!window) {
        LV_LOG_ERROR("Failed to allocate memory fo window");
        goto alloc_window_err;
    }

    lv_memset(window, 0, sizeof(*window));

    window->close_cb = close_cb;

    window->lv_disp = lv_display_create(hor_res, ver_res);
    if(!window->lv_disp) {
        LV_LOG_ERROR("failed to create lvgl display");
        goto create_display_error;
    }

    window->body = wl_compositor_create_surface(lv_wl_ctx.wl_compositor);
    if(!window->body) {
        LV_LOG_ERROR("Failed to create window body");
        goto create_surface_err;
    }

    if(lv_wl_xdg_create_window(lv_wl_ctx.xdg_wm, window, title) != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to create window");
        goto create_window_err;
    }

    /* Initialize display driver */
    window->backend_display_data = wl_backend_ops.init_display(lv_wl_ctx.backend_data, window->lv_disp, hor_res, ver_res);

    lv_display_set_driver_data(window->lv_disp, window);

    lv_wayland_xdg_configure_surface(window);

    lv_display_add_event_cb(window->lv_disp, res_changed_event, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_display_add_event_cb(window->lv_disp, refr_start_event, LV_EVENT_REFR_START, NULL);
    lv_display_add_event_cb(window->lv_disp, refr_end_event, LV_EVENT_REFR_READY, NULL);
    lv_display_add_event_cb(window->lv_disp, delete_event, LV_EVENT_DELETE, NULL);

    /* Register input */
    window->lv_indev_pointer = lv_wayland_pointer_create();
    lv_indev_set_display(window->lv_indev_pointer, window->lv_disp);

    if(!window->lv_indev_pointer) {
        LV_LOG_ERROR("failed to register pointer indev");
    }

    window->lv_indev_pointeraxis = lv_wayland_pointer_axis_create();
    lv_indev_set_display(window->lv_indev_pointeraxis, window->lv_disp);

    if(!window->lv_indev_pointeraxis) {
        LV_LOG_ERROR("failed to register pointeraxis indev");
    }

    window->lv_indev_touch = lv_wayland_touch_create();
    lv_indev_set_display(window->lv_indev_touch, window->lv_disp);

    if(!window->lv_indev_touch) {
        LV_LOG_ERROR("failed to register touch indev");
    }

    window->lv_indev_keyboard = lv_wayland_keyboard_create();
    lv_indev_set_display(window->lv_indev_keyboard, window->lv_disp);

    if(!window->lv_indev_keyboard) {
        LV_LOG_ERROR("failed to register keyboard indev");
    }
    return window->lv_disp;

create_window_err:
    wl_surface_destroy(window->body);
create_surface_err:
    lv_display_delete(window->lv_disp);
create_display_error:
    lv_ll_remove(&lv_wl_ctx.window_ll, window);
    lv_free(window);
alloc_window_err:
    return NULL;
}

void * lv_wayland_get_backend_display_data(lv_display_t * display)
{
    LV_ASSERT_NULL(display);
    lv_wl_window_t * window = lv_display_get_driver_data(display);
    LV_ASSERT_NULL(window);
    return window->backend_display_data;
}

struct wl_surface * lv_wayland_get_window_surface(lv_display_t * display)
{
    LV_ASSERT_NULL(display);
    lv_wl_window_t * window = lv_display_get_driver_data(display);
    LV_ASSERT_NULL(window);
    return window->body;
}

void lv_wayland_window_close(lv_display_t * display)
{
    LV_ASSERT_NULL(display);
    lv_wl_window_t * window = lv_display_get_driver_data(display);
    if(!window) {
        return;
    }
    window->close_cb = NULL;
    lv_wayland_window_delete(window);
    lv_wayland_deinit();
}

bool lv_wayland_window_is_open(lv_display_t * disp)
{
    LV_UNUSED(disp);
    return true;
}

void lv_wayland_window_set_maximized(lv_display_t * disp, bool maximized)
{
    lv_wl_window_t * window = lv_display_get_driver_data(disp);
    if(!window) {
        return;
    }
    if(window->maximized != maximized) {
        lv_wayland_xdg_set_maximized(&window->xdg, maximized);
    }

    window->maximized = maximized;
}
void lv_wayland_window_set_minimized(lv_display_t * disp)
{
    lv_wl_window_t * window = lv_display_get_driver_data(disp);
    if(!window) {
        return;
    }
    lv_wayland_xdg_set_minimized(&window->xdg);
}

void lv_wayland_assign_physical_display(lv_display_t * disp, uint8_t display_number)
{
    if(!disp) {
        LV_LOG_ERROR("Invalid display");
        return;
    }

    lv_wl_window_t * window = lv_display_get_driver_data(disp);

    if(!window) {
        LV_LOG_ERROR("Invalid window");
        return;
    }

    if(display_number >= lv_wl_ctx.wl_output_count) {
        LV_LOG_WARN("Invalid display number '%d'. Expected '0'..'%d'", display_number, lv_wl_ctx.wl_output_count - 1);
        return;
    }
    window->physical_output = lv_wl_ctx.physical_outputs[display_number].wl_output;
}

void lv_wayland_unassign_physical_display(lv_display_t * disp)
{

    if(!disp) {
        LV_LOG_ERROR("Invalid display");
        return;
    }

    lv_wl_window_t * window = lv_display_get_user_data(disp);
    if(!window) {
        LV_LOG_ERROR("Invalid window");
        return;
    }
    window->physical_output = NULL;
}

void lv_wayland_window_set_fullscreen(lv_display_t * disp, bool fullscreen)
{
    lv_wl_window_t * window = lv_display_get_driver_data(disp);
    if(!window) {
        return;
    }

    if(window->fullscreen == fullscreen) {
        return;
    }
    lv_wayland_xdg_set_fullscreen(&window->xdg, fullscreen, window->physical_output);
    window->fullscreen = fullscreen;
}

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

int32_t lv_wayland_window_get_width(lv_wl_window_t * window)
{
    return lv_display_get_horizontal_resolution(window->lv_disp);
}
int32_t lv_wayland_window_get_height(lv_wl_window_t * window)
{
    return lv_display_get_vertical_resolution(window->lv_disp);
}

void lv_wayland_window_delete(lv_wl_window_t * window)
{
    if(!window) {
        return;
    }

    if(window->close_cb) {
        window->close_cb(window->lv_disp);
    }
    lv_wayland_xdg_delete_window(&window->xdg);

    /* Commit a NULL buffer to the body surface so that we release buffers*/
    wl_surface_attach(window->body, NULL, 0, 0);
    wl_surface_commit(window->body);
    wl_display_roundtrip(lv_wl_ctx.wl_display);

    wl_surface_destroy(window->body);
    window->body = NULL;

    /* Make sure buffer is correctly released*/
    wl_display_roundtrip(lv_wl_ctx.wl_display);

    wl_backend_ops.deinit_display(window->backend_display_data, window->lv_disp);
    window->backend_display_data = NULL;

    /* Set the driver data to NULL before calling display delete
     * so that the delete event doesn't do anything*/
    lv_display_set_driver_data(window->lv_disp, NULL);
    lv_display_delete(window->lv_disp);


    lv_ll_remove(&lv_wl_ctx.window_ll, window);

    if(LV_WAYLAND_DIRECT_EXIT && lv_ll_is_empty(&lv_wl_ctx.window_ll)) {
        /* lv_deinit will deinit the wayland driver*/
        lv_deinit();
        exit(0);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void delete_event(lv_event_t * e)
{
    lv_display_t * display = lv_event_get_target(e);
    lv_wl_window_t * window = lv_display_get_driver_data(display);
    lv_wayland_window_delete(window);
}

static void refr_start_event(lv_event_t * e)
{
    lv_display_t * display = lv_event_get_target(e);
    lv_wl_window_t * window = lv_display_get_driver_data(display);

    while(wl_display_prepare_read(lv_wl_ctx.wl_display) != 0) {
        wl_display_dispatch_pending(lv_wl_ctx.wl_display);
    }

    wl_display_read_events(lv_wl_ctx.wl_display);
    wl_display_dispatch_pending(lv_wl_ctx.wl_display);


    if(lv_wayland_xdg_is_resize_pending(window)) {
        lv_wayland_xdg_resize(window);
    }
}

static void refr_end_event(lv_event_t * e)
{
    LV_UNUSED(e);
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
static void res_changed_event(lv_event_t * e)
{
    lv_display_t * display = (lv_display_t *) lv_event_get_target(e);
    lv_wl_window_t * window = lv_display_get_driver_data(display);
    window->backend_display_data = wl_backend_ops.resize_display(lv_wl_ctx.backend_data, display);
}

#endif /* LV_USE_WAYLAND */
