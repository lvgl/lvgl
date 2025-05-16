/**
 * @file lv_wl_window.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_wl_window.h"
#include <src/misc/lv_types.h>

#if LV_USE_WAYLAND

#include <string.h>
#include "lv_wayland_private.h"
#include "lv_wl_pointer.h"
#include "lv_wl_pointer_axis.h"
#include "lv_wl_touch.h"
#include "lv_wl_keyboard.h"
#include "lv_wayland_smm.h"

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

static void _lv_wayland_flush(lv_display_t * disp, const lv_area_t * area, unsigned char * color_p);

static struct graphic_object * create_graphic_obj(struct application * app, struct window * window,
                                                  enum object_type type, struct graphic_object * parent);
static void destroy_graphic_obj(struct graphic_object * obj);

/* Create a window
 * @description Creates the graphical context for the window body, and then create a toplevel
 * wayland surface and commit it to obtain an XDG configuration event
 * @param width the height of the window w/decorations
 * @param height the width of the window w/decorations
 */
static struct window * create_window(struct application * app, int width, int height, const char * title);

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
static void graphic_obj_frame_done(void * data, struct wl_callback * cb, uint32_t time);

/**********************
 *  STATIC VARIABLES
 **********************/

static const struct wl_callback_listener wl_surface_frame_listener = {
    .done = graphic_obj_frame_done,
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
    int32_t stride;

    lv_wayland_init();

    window_width  = hor_res;
    window_height = ver_res;

#if LV_WAYLAND_WINDOW_DECORATIONS
    if(!application.opt_disable_decorations) {
        window_width  = hor_res + (2 * BORDER_SIZE);
        window_height = ver_res + (TITLE_BAR_HEIGHT + (2 * BORDER_SIZE));
    }
#endif

    window = create_window(&application, window_width, window_height, title);
    if(!window) {
        LV_LOG_ERROR("failed to create wayland window");
        return NULL;
    }

    window->close_cb = close_cb;

    /* Initialize display driver */
    window->lv_disp = lv_display_create(hor_res, ver_res);
    if(window->lv_disp == NULL) {
        LV_LOG_ERROR("failed to create lvgl display");
        return NULL;
    }

    stride = lv_draw_buf_width_to_stride(hor_res, lv_display_get_color_format(window->lv_disp));

    window->lv_disp_draw_buf = lv_draw_buf_create(hor_res, ver_res / LVGL_DRAW_BUFFER_DIV,
                                                  lv_display_get_color_format(window->lv_disp), stride);

    lv_display_set_draw_buffers(window->lv_disp, window->lv_disp_draw_buf, NULL);
    lv_display_set_render_mode(window->lv_disp, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(window->lv_disp, _lv_wayland_flush);
    lv_display_set_user_data(window->lv_disp, window);

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

#if LV_USE_GESTURE_RECOGNITION

    window->lv_indev_touch = lv_wayland_touch_create();
    lv_indev_set_display(window->lv_indev_touch, window->lv_disp);

    if(!window->lv_indev_touch) {
        LV_LOG_ERROR("failed to register touch indev");
    }

#endif /* END LV_USE_GESTURE_RECOGNITION */

    window->lv_indev_keyboard = lv_wayland_keyboard_create();
    lv_indev_set_display(window->lv_indev_keyboard, window->lv_disp);

    if(!window->lv_indev_keyboard) {
        LV_LOG_ERROR("failed to register keyboard indev");
    }
    return window->lv_disp;
}

void lv_wayland_window_close(lv_display_t * disp)
{
    struct window * window = lv_display_get_user_data(disp);
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
        LV_LL_READ(&application.window_ll, window)
        {
            if(!window->closed) {
                open = true;
                break;
            }
        }
    } else {
        window = lv_display_get_user_data(disp);
        open   = (!window->closed);
    }

    return open;
}

void lv_wayland_window_set_maximized(lv_display_t * disp, bool maximized)
{
    struct window * window = lv_display_get_user_data(disp);
    lv_result_t err        = LV_RESULT_INVALID;
    if(!window || window->closed) {
        return;
    }

    if(window->maximized != maximized) {
#if LV_WAYLAND_WL_SHELL
        err = lv_wayland_wl_shell_set_maximized(window, maximized);
#elif LV_WAYLAND_XDG_SHELL
        err = lv_wayland_xdg_shell_set_maximized(window, maximized);
#endif
    }

    if(err == LV_RESULT_INVALID) {
        LV_LOG_WARN("Failed to maximize wayland window");
        return;
    }

    window->maximized     = maximized;
    window->flush_pending = true;
}

void lv_wayland_window_set_fullscreen(lv_display_t * disp, bool fullscreen)
{
    struct window * window = lv_display_get_user_data(disp);
    lv_result_t err        = LV_RESULT_INVALID;
    if(!window || window->closed) {
        return;
    }

    if(window->fullscreen == fullscreen) {
        return;
    }
#if LV_WAYLAND_WL_SHELL
    err = lv_wayland_wl_shell_set_fullscreen(window, fullscreen);
#elif LV_WAYLAND_XDG_SHELL
    err = lv_wayland_xdg_shell_set_fullscreen(window, fullscreen);
#endif

    if(err == LV_RESULT_INVALID) {
        LV_LOG_WARN("Failed to set wayland window to fullscreen");
        return;
    }

    window->fullscreen    = fullscreen;
    window->flush_pending = true;
}

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

void lv_wayland_window_draw(struct window * window, uint32_t width, uint32_t height)
{

#if LV_WAYLAND_WINDOW_DECORATIONS
    if(application.opt_disable_decorations == false) {
        for(size_t i = 0; i < NUM_DECORATIONS; i++) {
            window->decoration[i] = create_graphic_obj(&application, window, (FIRST_DECORATION + i), window->body);
            if(!window->decoration[i]) {
                LV_LOG_ERROR("Failed to create decoration %zu", i);
            }
        }
    }
#endif

    /* First resize */
    if(!lv_wayland_window_resize(window, width, height)) {
        LV_LOG_ERROR("Failed to resize window");
#if LV_WAYLAND_XDG_SHELL
        lv_wayland_xdg_shell_destroy_window_toplevel(window);
#endif
    }

    lv_refr_now(window->lv_disp);
}

bool lv_wayland_window_resize(struct window * window, int width, int height)
{
    struct smm_buffer_t * body_buf1;
    struct smm_buffer_t * body_buf2;
    uint32_t stride;
    uint8_t bpp;

    window->width  = width;
    window->height = height;

#if LV_WAYLAND_WINDOW_DECORATIONS
    if(!window->application->opt_disable_decorations && !window->fullscreen) {
        width -= (2 * BORDER_SIZE);
        height -= (TITLE_BAR_HEIGHT + (2 * BORDER_SIZE));
    }
#endif

    bpp = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);

    /* Update size for newly allocated buffers */
    smm_resize(window->body->buffer_group, ((width * bpp) * height) * 2);

    window->body->width  = width;
    window->body->height = height;

    /* Pre-allocate two buffers for the window body here */
    body_buf1 = smm_acquire(window->body->buffer_group);
    body_buf2 = smm_acquire(window->body->buffer_group);

    if(smm_map(body_buf2) == NULL) {
        LV_LOG_ERROR("Cannot pre-allocate backing buffers for window body");
        wl_surface_destroy(window->body->surface);
        return false;
    }

    /* Moves the buffers to the the unused list of the group */
    smm_release(body_buf1);
    smm_release(body_buf2);

#if LV_WAYLAND_WINDOW_DECORATIONS
    if(!window->application->opt_disable_decorations && !window->fullscreen) {
        lv_wayland_window_decoration_create_all(window);
    } else if(!window->application->opt_disable_decorations) {
        /* Entering fullscreen, detach decorations to prevent xdg_wm_base error 4 */
        /* requested geometry larger than the configured fullscreen state */
        lv_wayland_window_decoration_detach_all(window);
    }
#endif

    LV_LOG_TRACE("resize window:%dx%d body:%dx%d frame: %d rendered: %d", window->width, window->height,
                 window->body->width, window->body->height, window->frame_counter, window->frame_done);

    width  = window->body->width;
    height = window->body->height;

    if(window->lv_disp != NULL) {
        /* Resize draw buffer */
        stride = lv_draw_buf_width_to_stride(width, lv_display_get_color_format(window->lv_disp));

        window->lv_disp_draw_buf =
            lv_draw_buf_reshape(window->lv_disp_draw_buf, lv_display_get_color_format(window->lv_disp), width,
                                height / LVGL_DRAW_BUFFER_DIV, stride);

        lv_display_set_resolution(window->lv_disp, width, height);

        window->body->input.pointer.x = LV_MIN((int32_t)window->body->input.pointer.x, (width - 1));
        window->body->input.pointer.y = LV_MIN((int32_t)window->body->input.pointer.y, (height - 1));
    }

    return true;
}

void lv_wayland_window_destroy(struct window * window)
{
    if(!window) {
        return;
    }

#if LV_WAYLAND_WL_SHELL
    lv_wayland_wl_shell_destroy_window(window);
#elif LV_WAYLAND_XDG_SHELL
    lv_wayland_xdg_shell_destroy_window_toplevel(window);
    lv_wayland_xdg_shell_destroy_window_surface(window);
#endif

#if LV_WAYLAND_WINDOW_DECORATIONS
    for(size_t i = 0; i < NUM_DECORATIONS; i++) {
        if(window->decoration[i]) {
            destroy_graphic_obj(window->decoration[i]);
            window->decoration[i] = NULL;
        }
    }
#endif

    destroy_graphic_obj(window->body);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _lv_wayland_flush(lv_display_t * disp, const lv_area_t * area, unsigned char * color_p)
{
    void * buf_base;
    struct wl_buffer * wl_buf;
    int32_t src_width;
    int32_t src_height;
    struct window * window;
    struct application * app;
    smm_buffer_t * buf;
    struct wl_callback * cb;
    lv_display_rotation_t rot;
    uint8_t bpp;
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    int32_t hres;
    int32_t vres;

    window     = lv_display_get_user_data(disp);
    app        = window->application;
    buf        = window->body->pending_buffer;
    src_width  = lv_area_get_width(area);
    src_height = lv_area_get_height(area);
    bpp        = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);

    rot = lv_display_get_rotation(disp);
    w   = lv_display_get_horizontal_resolution(disp);
    h   = lv_display_get_vertical_resolution(disp);

    /* TODO actually test what happens if the rotation is 90 or 270 or 180 ? */
    hres = (rot == LV_DISPLAY_ROTATION_0) ? w : h;
    vres = (rot == LV_DISPLAY_ROTATION_0) ? h : w;

    /* If window has been / is being closed, or is not visible, skip flush */
    if(window->closed || window->shall_close) {
        goto skip;
    }
    /* Skip if the area is out the screen */
    else if((area->x2 < 0) || (area->y2 < 0) || (area->x1 > hres - 1) || (area->y1 > vres - 1)) {
        goto skip;
    }

    /* Acquire and map a buffer to attach/commit to surface */
    if(buf == NULL) {
        buf = smm_acquire(window->body->buffer_group);
        if(buf == NULL) {
            LV_LOG_ERROR("cannot acquire a window body buffer");
            goto skip;
        }

        window->body->pending_buffer = buf;
        SMM_TAG(buf, TAG_BUFFER_DAMAGE, window->dmg_cache.cache + window->dmg_cache.end);
    }

    buf_base = smm_map(buf);
    if(buf_base == NULL) {
        LV_LOG_ERROR("cannot map in window body buffer");
        goto skip;
    }

    /* Modify specified area in buffer */
    for(y = 0; y < src_height; ++y) {
        if(app->shm_format == WL_SHM_FORMAT_ARGB8888) {
            for(x = 0; x < src_width; ++x) {
                lv_color_premultiply((lv_color32_t *)color_p + x);
            }
        }
        memcpy(((char *)buf_base) + ((((area->y1 + y) * hres) + area->x1) * bpp), color_p, src_width * bpp);
        color_p += src_width * bpp;
    }

    /* Mark surface damage */
    wl_surface_damage(window->body->surface, area->x1, area->y1, src_width, src_height);

    lv_wayland_cache_add_area(window, buf, area);

    if(lv_display_flush_is_last(disp)) {
        /* Finally, attach buffer and commit to surface */
        wl_buf = SMM_BUFFER_PROPERTIES(buf)->tag[TAG_LOCAL];
        wl_surface_attach(window->body->surface, wl_buf, 0, 0);
        wl_surface_commit(window->body->surface);
        window->body->pending_buffer = NULL;
        window->frame_done           = false;

        cb = wl_surface_frame(window->body->surface);
        wl_callback_add_listener(cb, &wl_surface_frame_listener, window->body);
        LV_LOG_TRACE("last flush frame: %d", window->frame_counter);

        window->flush_pending = true;
    }

    lv_display_flush_ready(disp);
    return;
skip:
    if(buf != NULL) {
        /* Cleanup any intermediate state (in the event that this flush being
         * skipped is in the middle of a flush sequence)
         */
        lv_wayland_cache_clear(window);
        SMM_TAG(buf, TAG_BUFFER_DAMAGE, NULL);
        smm_release(buf);
        window->body->pending_buffer = NULL;
    }
}

static struct window * create_window(struct application * app, int width, int height, const char * title)
{
    struct window * window;

    window = lv_ll_ins_tail(&app->window_ll);
    LV_ASSERT_MALLOC(window);
    if(!window) {
        return NULL;
    }

    lv_memset(window, 0x00, sizeof(struct window));

    window->application = app;

    // Create wayland buffer and surface
    window->body   = create_graphic_obj(app, window, OBJECT_WINDOW, NULL);
    window->width  = width;
    window->height = height;

    if(!window->body) {
        LV_LOG_ERROR("cannot create window body");
        goto err_free_window;
    }

#if LV_WAYLAND_WL_SHELL
    if(lv_wayland_wl_shell_create_window(app, window, title) != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to create wl shell window");
        goto err_destroy_surface;
    }
#elif LV_WAYLAND_XDG_SHELL
    if(lv_wayland_xdg_shell_create_window(app, window, title) != LV_RESULT_OK) {
        goto err_destroy_surface;
    }
#endif

    return window;

err_destroy_surface:
    wl_surface_destroy(window->body->surface);

err_free_window:
    lv_ll_remove(&app->window_ll, window);
    lv_free(window);
    return NULL;
}

static void graphic_obj_frame_done(void * data, struct wl_callback * cb, uint32_t time)
{
    struct graphic_object * obj;
    struct window * window;

    LV_UNUSED(time);

    wl_callback_destroy(cb);

    obj    = (struct graphic_object *)data;
    window = obj->window;
    window->frame_counter++;

    LV_LOG_TRACE("frame: %d done, new frame: %d", window->frame_counter - 1, window->frame_counter);

    window->frame_done = true;
}

static struct graphic_object * create_graphic_obj(struct application * app, struct window * window,
                                                  enum object_type type, struct graphic_object * parent)
{
    struct graphic_object * obj;

    LV_UNUSED(parent);

    obj = lv_malloc(sizeof(*obj));
    LV_ASSERT_MALLOC(obj);
    if(!obj) {
        goto err_out;
    }

    lv_memset(obj, 0x00, sizeof(struct graphic_object));

    obj->surface = wl_compositor_create_surface(app->compositor);
    if(!obj->surface) {
        LV_LOG_ERROR("cannot create surface for graphic object");
        goto err_free;
    }

    obj->buffer_group = smm_create();
    if(obj->buffer_group == NULL) {
        LV_LOG_ERROR("cannot create buffer group for graphic object");
        goto err_destroy_surface;
    }

    obj->window             = window;
    obj->type               = type;
    obj->surface_configured = true;
    obj->pending_buffer     = NULL;
    wl_surface_set_user_data(obj->surface, obj);
    SMM_TAG(obj->buffer_group, TAG_LOCAL, obj);

    return obj;

err_destroy_surface:
    wl_surface_destroy(obj->surface);

err_free:
    lv_free(obj);

err_out:
    return NULL;
}

static void destroy_graphic_obj(struct graphic_object * obj)
{
    if(obj->subsurface) {
        wl_subsurface_destroy(obj->subsurface);
    }

    wl_surface_destroy(obj->surface);
    smm_destroy(obj->buffer_group);
    lv_free(obj);
}

#endif /* LV_USE_WAYLAND */
