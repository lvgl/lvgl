/**
 * @file lv_wl_window.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_wl_window.h"

#if LV_USE_WAYLAND

#include <string.h>
#include "lv_wayland_private.h"
#include "lv_wayland_private.h"
#include "lv_wl_pointer.h"
#include "lv_wl_pointer_axis.h"
#include "lv_wl_touch.h"
#include "lv_wl_keyboard.h"

#include "../../core/lv_refr.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static struct graphic_object * create_graphic_obj(struct window * window, enum object_type type,
                                                  struct graphic_object * parent);
static void destroy_graphic_obj(struct window * window, struct graphic_object * obj);

/* Create a window
 * @description Creates the graphical context for the window body, and then create a toplevel
 * wayland surface and commit it to obtain an XDG configuration event
 * @param width the height of the window w/decorations
 * @param height the width of the window w/decorations
 */
static struct window * create_window(struct lv_wayland_context * app, int width, int height, const char * title);

/**
 * The frame callback called when the compositor has finished rendering
 * a frame.It increments the frame counter and sets up the callback
 * for the next frame the frame counter is used to avoid needlessly
 * committing frames too fast on a slow system
 *
 * NOTE: this function is invoked by the wayland-server library within the compositor
 * the event is added to the queue, and then upon the next timer call it's
 * called indirectly from _lv_wayland_handle_input (via wl_display_dispatch_queue)
 * @param void data the user object defined that was tied to this event during
 * the configuration of the callback
 * @param struct wl_callback The callback that needs to be destroyed and re-created
 * @param time Timestamp of the event (unused)
 */
static void lv_window_graphic_obj_flush_done(void * data, struct wl_callback * cb, uint32_t time);

/**********************
 *  STATIC VARIABLES
 **********************/

static const struct wl_callback_listener wl_surface_frame_listener = {
    .done = lv_window_graphic_obj_flush_done,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_wayland_window_create(uint32_t hor_res, uint32_t ver_res, char * title,
                                        lv_wayland_display_close_f_t close_cb)
{
    struct window * window;
    int32_t window_width;
    int32_t window_height;

    uint32_t width = hor_res;
    uint32_t height = ver_res;

    lv_wayland_init();

    window_width  = hor_res;
    window_height = ver_res;

#if LV_WAYLAND_WINDOW_DECORATIONS
    if(!lv_wl_ctx.opt_disable_decorations) {
        window_width  = hor_res + (2 * BORDER_SIZE);
        window_height = ver_res + (TITLE_BAR_HEIGHT + (2 * BORDER_SIZE));
    }
#endif

    window = create_window(&lv_wl_ctx, window_width, window_height, title);
    if(!window) {
        LV_LOG_ERROR("failed to create wayland window");
        return NULL;
    }

    window->close_cb = close_cb;

    /* Initialize display driver */
    window->lv_disp = lv_display_create(width, height);
    if(window->lv_disp == NULL) {
        LV_LOG_ERROR("failed to create lvgl display");
        return NULL;
    }

#if LV_WAYLAND_USE_DMABUF
    if(lv_wayland_dmabuf_create_draw_buffers(&lv_wl_ctx.dmabuf_ctx, window) != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to create draw buffers");
        return NULL;
    }
#else
    if(lv_wayland_shm_create_draw_buffers(&lv_wl_ctx.shm_ctx, window) != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to create window buffers");
        return NULL;
    }
#endif

    lv_wayland_xdg_shell_configure_surface(window);

    lv_display_set_driver_data(window->lv_disp, window);

    lv_display_set_render_mode(window->lv_disp, LV_WAYLAND_RENDER_MODE);
    lv_display_set_flush_wait_cb(window->lv_disp, lv_wayland_wait_flush_cb);

#if LV_WAYLAND_USE_DMABUF
    lv_wayland_dmabuf_set_draw_buffers(&lv_wl_ctx.dmabuf_ctx, window->lv_disp);
    lv_display_set_flush_cb(window->lv_disp, lv_wayland_dmabuf_flush_full_mode);
#else
    lv_wayland_shm_set_draw_buffers(&lv_wl_ctx.shm_ctx, window->lv_disp, window);
    lv_display_set_flush_cb(window->lv_disp, lv_wayland_shm_flush_partial_mode);
#endif

    lv_display_add_event_cb(window->lv_disp, lv_wayland_event_cb, LV_EVENT_RESOLUTION_CHANGED, window);

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
}

void lv_wayland_window_close(lv_display_t * disp)
{
    struct window * window = lv_display_get_driver_data(disp);
    if(!window || window->closed) {
        return;
    }
    window->shall_close = true;
    window->close_cb    = NULL;
    lv_wayland_deinit();
}

bool lv_wayland_window_is_open(lv_display_t * disp)
{
    struct window * window;
    bool open = false;

    if(disp == NULL) {
        LV_LL_READ(&lv_wl_ctx.window_ll, window) {
            if(!window->closed) {
                open = true;
                break;
            }
        }
    }
    else {
        window = lv_display_get_driver_data(disp);
        open   = (!window->closed);
    }

    return open;
}

void lv_wayland_window_set_maximized(lv_display_t * disp, bool maximized)
{
    struct window * window = lv_display_get_driver_data(disp);
    lv_result_t err        = LV_RESULT_INVALID;
    if(!window || window->closed) {
        return;
    }

    if(window->maximized != maximized) {
        err = lv_wayland_xdg_shell_set_maximized(window, maximized);
    }

    if(err == LV_RESULT_INVALID) {
        LV_LOG_WARN("Failed to maximize wayland window");
        return;
    }

    window->maximized     = maximized;
    window->flush_pending = true;
}

void lv_wayland_assign_physical_display(lv_display_t * disp, uint8_t display_number)
{
    if(!disp) {
        LV_LOG_ERROR("Invalid display");
        return;
    }

    struct window * window = lv_display_get_driver_data(disp);

    if(!window || window->closed) {
        LV_LOG_ERROR("Invalid window");
        return;
    }

    if(display_number >= window->wl_ctx->wl_output_count) {
        LV_LOG_WARN("Invalid display number '%d'. Expected '0'..'%d'", display_number, window->wl_ctx->wl_output_count - 1);
        return;
    }
    window->assigned_output = lv_wl_ctx.outputs[display_number].wl_output;
}

void lv_wayland_unassign_physical_display(lv_display_t * disp)
{

    if(!disp) {
        LV_LOG_ERROR("Invalid display");
        return;
    }

    struct window * window = lv_display_get_user_data(disp);

    if(!window || window->closed) {
        LV_LOG_ERROR("Invalid window");
        return;
    }
    window->assigned_output = NULL;
}

void lv_wayland_window_set_fullscreen(lv_display_t * disp, bool fullscreen)
{
    struct window * window = lv_display_get_driver_data(disp);
    lv_result_t err        = LV_RESULT_INVALID;
    if(!window || window->closed) {
        return;
    }

    if(window->fullscreen == fullscreen) {
        return;
    }
    err = lv_wayland_xdg_shell_set_fullscreen(window, fullscreen, window->assigned_output);

    if(err == LV_RESULT_INVALID) {
        LV_LOG_WARN("Failed to set wayland window to fullscreen");
        return;
    }

    window->fullscreen = fullscreen;
    window->flush_pending = true;
}

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

void lv_wayland_window_draw(struct window * window, uint32_t width, uint32_t height)
{

#if LV_WAYLAND_WINDOW_DECORATIONS
    if(lv_wl_ctx.opt_disable_decorations == false) {
        for(size_t i = 0; i < NUM_DECORATIONS; i++) {
            window->decoration[i] = create_graphic_obj(window, (FIRST_DECORATION + i), window->body);
            if(!window->decoration[i]) {
                LV_LOG_ERROR("Failed to create decoration %zu", i);
            }
        }
    }
#endif

    LV_LOG_TRACE("Resizing to %d %d", width, height);
    /* First resize */
    if(lv_wayland_window_resize(window, width, height) != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to resize window");
        lv_wayland_xdg_shell_destroy_window_toplevel(window);
    }

    lv_refr_now(window->lv_disp);
}

lv_result_t lv_wayland_window_resize(struct window * window, int width, int height)
{

#if LV_WAYLAND_WINDOW_DECORATIONS
    if(!window->wl_ctx->opt_disable_decorations && !window->fullscreen) {
        width -= (2 * BORDER_SIZE);
        height -= (TITLE_BAR_HEIGHT + (2 * BORDER_SIZE));
    }
#endif
    if(window->lv_disp) {
        lv_display_set_resolution(window->lv_disp, width, height);
        window->body->input.pointer.x = LV_MIN((int32_t)window->body->input.pointer.x, (width - 1));
        window->body->input.pointer.y = LV_MIN((int32_t)window->body->input.pointer.y, (height - 1));
    }

    /* On the first resize call, the resolution of the display is already set, so there won't be a trigger on the resolution changed event.*/
    if(!window->is_window_configured) {
#if LV_WAYLAND_USE_DMABUF
        lv_result_t err = lv_wayland_dmabuf_resize_window(&window->wl_ctx->dmabuf_ctx, window, width, height);
        if(err != LV_RESULT_OK) {
            return err;
        }
#else
        lv_result_t err = lv_wayland_shm_resize_window(&window->wl_ctx->shm_ctx, window, width, height);
        if(err != LV_RESULT_OK) {
            return err;
        }
#endif
    }

#if LV_WAYLAND_WINDOW_DECORATIONS
    if(!window->wl_ctx->opt_disable_decorations && !window->fullscreen) {
        lv_wayland_window_decoration_create_all(window);
    }
    else if(!window->wl_ctx->opt_disable_decorations) {
        /* Entering fullscreen, detach decorations to prevent xdg_wm_base error 4 */
        /* requested geometry larger than the configured fullscreen state */
        lv_wayland_window_decoration_detach_all(window);
    }
#endif

    window->width  = width;
    window->height = height;
    return LV_RESULT_OK;
}

void lv_wayland_window_destroy(struct window * window)
{
    if(!window) {
        return;
    }

    lv_wayland_xdg_shell_destroy_window_toplevel(window);
    lv_wayland_xdg_shell_destroy_window_surface(window);

#if LV_WAYLAND_WINDOW_DECORATIONS
    for(size_t i = 0; i < NUM_DECORATIONS; i++) {
        if(window->decoration[i]) {
            destroy_graphic_obj(window, window->decoration[i]);
            window->decoration[i] = NULL;
        }
    }
#endif

    destroy_graphic_obj(window, window->body);
}

const struct wl_callback_listener * lv_wayland_window_get_wl_surface_frame_listener(void)
{
    return &wl_surface_frame_listener;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static struct window * create_window(struct lv_wayland_context * app, int width, int height, const char * title)
{
    struct window * window;

    window = lv_ll_ins_tail(&app->window_ll);
    LV_ASSERT_MALLOC(window);
    if(!window) {
        return NULL;
    }

    lv_memset(window, 0x00, sizeof(struct window));

    window->wl_ctx = app;

    // Create wayland buffer and surface
    window->body   = create_graphic_obj(window, OBJECT_WINDOW, NULL);
    window->width  = width;
    window->height = height;

    if(!window->body) {
        LV_LOG_ERROR("cannot create window body");
        goto err_free_window;
    }

    if(lv_wayland_xdg_shell_create_window(app, window, title) != LV_RESULT_OK) {
        goto err_destroy_surface;
    }

    return window;

err_destroy_surface:
    wl_surface_destroy(window->body->surface);

err_free_window:
    lv_ll_remove(&app->window_ll, window);
    lv_free(window);
    return NULL;
}

static struct graphic_object * create_graphic_obj(struct window * window, enum object_type type,
                                                  struct graphic_object * parent)
{

    /* For now creating a graphical object is similar enough for both shm and dmabuf
     * so the heavylifting is done in this function directly but we still
     * call the shm and dmabuf specific functions at the end to make sure they can add additional
     * attributes if needed
     */
    struct graphic_object * obj = NULL;

    LV_UNUSED(parent);

    obj = lv_malloc(sizeof(*obj));
    LV_ASSERT_MALLOC(obj);
    if(!obj) {
        LV_LOG_ERROR("Failed to create graphic object");
        return NULL;
    }

    lv_memset(obj, 0, sizeof(*obj));

    obj->surface = wl_compositor_create_surface(window->wl_ctx->compositor);
    if(!obj->surface) {
        LV_LOG_ERROR("Failed to create surface for graphic object");
        lv_free(obj);
        return NULL;
    }
    wl_surface_set_user_data(obj->surface, obj);

    obj->window = window;
    obj->type   = type;

#if LV_WAYLAND_USE_DMABUF
    return lv_wayland_dmabuf_on_graphical_object_creation(&window->wl_ctx->dmabuf_ctx, obj);
#else
    return lv_wayland_shm_on_graphical_object_creation(&window->wl_ctx->shm_ctx, obj);
#endif
}

static void destroy_graphic_obj(struct window * window, struct graphic_object * obj)
{
    if(obj->subsurface) {
        wl_subsurface_destroy(obj->subsurface);
    }

    wl_surface_destroy(obj->surface);

#if LV_WAYLAND_USE_DMABUF
    lv_wayland_dmabuf_on_graphical_object_destruction(&window->wl_ctx->dmabuf_ctx, obj);
#else
    lv_wayland_shm_on_graphical_object_destruction(&window->wl_ctx->shm_ctx, obj);
#endif
    lv_free(obj);
}

static void lv_window_graphic_obj_flush_done(void * data, struct wl_callback * cb, uint32_t time)
{
    struct graphic_object * obj;
    struct window * window;

    LV_UNUSED(time);

    wl_callback_destroy(cb);

    obj    = (struct graphic_object *)data;
    window = obj->window;
    window->frame_counter++;

    LV_LOG_TRACE("frame: %d done, new frame: %d", window->frame_counter - 1, window->frame_counter);

    lv_display_flush_ready(window->lv_disp);
}

#endif /* LV_USE_WAYLAND */
