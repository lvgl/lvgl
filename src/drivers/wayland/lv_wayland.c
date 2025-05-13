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

typedef int dummy_t;    /* Make GCC on windows happy, avoid empty translation unit */

#ifndef _WIN32

/*********************
 *      INCLUDES
 *********************/

#include "lv_wayland.h"
#include "lv_wl_keyboard.h"
#include "lv_wl_pointer_axis.h"
#include "lv_wl_pointer.h"
#include "lv_wl_touch.h"
#include "lv_wayland_smm.h"

#if LV_USE_WAYLAND

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
    #error [wayland] Unsupported LV_COLOR_DEPTH
#endif

/**********************
 *      TYPEDEFS
 **********************/




/*********************************
 *   STATIC VARIABLES and FUNCTIONS
 *********************************/

static struct application application;

static void color_fill(void * pixels, lv_color_t color, uint32_t width, uint32_t height);
static void color_fill_XRGB8888(void * pixels, lv_color_t color, uint32_t width, uint32_t height);
static void color_fill_RGB565(void * pixels, lv_color_t color, uint32_t width, uint32_t height);

static const struct wl_callback_listener wl_surface_frame_listener;
static bool resize_window(struct window * window, int width, int height);
static struct graphic_object * create_graphic_obj(struct application * app, struct window * window,
                                                  enum object_type type,
                                                  struct graphic_object * parent);

static uint32_t tick_get_cb(void);

static void wayland_init(void);
static void wayland_deinit(void);

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
static void graphic_obj_frame_done(void * data, struct wl_callback * cb, uint32_t time)
{
    struct graphic_object * obj;
    struct window * window;

    LV_UNUSED(time);

    wl_callback_destroy(cb);

    obj = (struct graphic_object *)data;
    window = obj->window;
    window->frame_counter++;

    LV_LOG_TRACE("frame: %d done, new frame: %d",
                 window->frame_counter - 1, window->frame_counter);

    window->frame_done = true;

}

static const struct wl_callback_listener wl_surface_frame_listener = {
    .done = graphic_obj_frame_done,
};

static inline bool _is_digit(char ch)
{
    return (ch >= '0') && (ch <= '9');
}

/*
 *  shm_format
 *  @description called by the compositor to advertise the supported
 *  color formats for SHM buffers, there is a call per supported format
 */
static void shm_format(void * data, struct wl_shm * wl_shm, uint32_t format)
{
    struct application * app = data;

    LV_UNUSED(wl_shm);

    LV_LOG_WARN("Supported color space fourcc.h code: %08X", format);

    if(LV_COLOR_DEPTH == 32 && format == WL_SHM_FORMAT_ARGB8888) {
        LV_LOG_WARN("Found WL_SHM_FORMAT_ARGB8888");

        /* Wayland compositors MUST support ARGB8888 */
        app->shm_format = format;

    }
    else if(LV_COLOR_DEPTH == 32 &&
            format == WL_SHM_FORMAT_XRGB8888 &&
            app->shm_format != WL_SHM_FORMAT_ARGB8888) {

        LV_LOG_WARN("Found WL_SHM_FORMAT_XRGB8888");
        /* Select XRGB only if the compositor doesn't support transprancy */
        app->shm_format = format;

    }
    else if(LV_COLOR_DEPTH == 16 && format == WL_SHM_FORMAT_RGB565) {

        app->shm_format = format;

    }
}

static const struct wl_shm_listener shm_listener = {
    .format = shm_format
};

static void seat_handle_capabilities(void * data, struct wl_seat * wl_seat, enum wl_seat_capability caps)
{
    struct application * app = data;
    struct seat * seat = &app->seat;

    if((caps & WL_SEAT_CAPABILITY_POINTER) && !seat->wl_pointer) {
        seat->wl_pointer = wl_seat_get_pointer(wl_seat);
        wl_pointer_add_listener(seat->wl_pointer, lv_wayland_pointer_get_listener(), app);
        app->cursor_surface = wl_compositor_create_surface(app->compositor);
        if(!app->cursor_surface) {
            LV_LOG_WARN("failed to create cursor surface");
        }
    }
    else if(!(caps & WL_SEAT_CAPABILITY_POINTER) && seat->wl_pointer) {
        wl_pointer_destroy(seat->wl_pointer);
        if(app->cursor_surface) {
            wl_surface_destroy(app->cursor_surface);
        }
        seat->wl_pointer = NULL;
    }

    if((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !seat->wl_keyboard) {
        seat->wl_keyboard = wl_seat_get_keyboard(wl_seat);
        wl_keyboard_add_listener(seat->wl_keyboard, lv_wayland_keyboard_get_listener(), app);
    }
    else if(!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && seat->wl_keyboard) {
        wl_keyboard_destroy(seat->wl_keyboard);
        seat->wl_keyboard = NULL;
    }

#if LV_USE_GESTURE_RECOGNITION
    if((caps & WL_SEAT_CAPABILITY_TOUCH) && !seat->wl_touch) {
        seat->wl_touch = wl_seat_get_touch(wl_seat);
        wl_touch_add_listener(seat->wl_touch, lv_wayland_touch_get_listener(), app);
    }
#endif
    else if(!(caps & WL_SEAT_CAPABILITY_TOUCH) && seat->wl_touch) {
        wl_touch_destroy(seat->wl_touch);
        seat->wl_touch = NULL;
    }
}

static const struct wl_seat_listener seat_listener = {
    .capabilities = seat_handle_capabilities,
};

static void draw_window(struct window * window, uint32_t width, uint32_t height)
{

#if LV_WAYLAND_WINDOW_DECORATIONS
    if(application.opt_disable_decorations == false) {
        int d;
        for(d = 0; d < NUM_DECORATIONS; d++) {
            window->decoration[d] = create_graphic_obj(&application, window, (FIRST_DECORATION + d), window->body);
            if(!window->decoration[d]) {
                LV_LOG_ERROR("Failed to create decoration %d", d);
            }
        }
    }
#endif

    /* First resize */
    if(!resize_window(window, width, height)) {
        LV_LOG_ERROR("Failed to resize window");
#if LV_WAYLAND_XDG_SHELL
        if(window->xdg_toplevel) {
            xdg_toplevel_destroy(window->xdg_toplevel);
        }
#endif
    }

    lv_refr_now(window->lv_disp);

}

#if LV_WAYLAND_WL_SHELL
static void wl_shell_handle_ping(void * data, struct wl_shell_surface * shell_surface, uint32_t serial)
{
    return wl_shell_surface_pong(shell_surface, serial);
}

static void wl_shell_handle_configure(void * data, struct wl_shell_surface * shell_surface,
                                      uint32_t edges, int32_t width, int32_t height)
{
    struct window * window = (struct window *)data;

    LV_UNUSED(edges);

    if((width <= 0) || (height <= 0)) {
        return;
    }
    else if((width != window->width) || (height != window->height)) {
        window->resize_width = width;
        window->resize_height = height;
        window->resize_pending = true;
    }
}

static const struct wl_shell_surface_listener shell_surface_listener = {
    .ping       = wl_shell_handle_ping,
    .configure  =  wl_shell_handle_configure,
};
#endif

#if LV_WAYLAND_XDG_SHELL
static void xdg_surface_handle_configure(void * data, struct xdg_surface * xdg_surface, uint32_t serial)
{
    struct window * window = (struct window *)data;

    xdg_surface_ack_configure(xdg_surface, serial);

    if(window->body->surface_configured == false) {
        /* This branch is executed at launch */
        if(window->resize_pending == false) {
            /* Use the size passed to the create_window function */
            draw_window(window, window->width, window->height);
        }
        else {

            /* Handle early maximization or fullscreen, */
            /* by using the size communicated by the compositor */
            /* when the initial xdg configure event arrives  */
            draw_window(window, window->resize_width, window->resize_height);
            window->width = window->resize_width;
            window->height = window->resize_height;
            window->resize_pending = false;
        }
    }
    window->body->surface_configured = true;
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_handle_configure,
};

static void xdg_toplevel_handle_configure(void * data, struct xdg_toplevel * xdg_toplevel,
                                          int32_t width, int32_t height, struct wl_array * states)
{
    struct window * window = (struct window *)data;

    LV_UNUSED(xdg_toplevel);
    LV_UNUSED(states);
    LV_UNUSED(width);
    LV_UNUSED(height);

    LV_LOG_TRACE("w:%d h:%d", width, height);
    LV_LOG_TRACE("current body w:%d h:%d", window->body->width, window->body->height);
    LV_LOG_TRACE("window w:%d h:%d", window->width, window->height);


    if((width <= 0) || (height <= 0)) {
        LV_LOG_TRACE("will not resize to w:%d h:%d", width, height);
        return;
    }

    if((width != window->width) || (height != window->height)) {
        window->resize_width = width;
        window->resize_height = height;
        window->resize_pending = true;
        LV_LOG_TRACE("resize_pending is set, will resize to w:%d h:%d", width, height);
    }
    else {
        LV_LOG_TRACE("resize_pending not set w:%d h:%d", width, height);
    }
}

static void xdg_toplevel_handle_close(void * data, struct xdg_toplevel * xdg_toplevel)
{
    struct window * window = (struct window *)data;
    window->shall_close = true;

    LV_UNUSED(xdg_toplevel);
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_handle_configure,
    .close = xdg_toplevel_handle_close,
};

static void xdg_wm_base_ping(void * data, struct xdg_wm_base * xdg_wm_base, uint32_t serial)
{
    LV_UNUSED(data);

    xdg_wm_base_pong(xdg_wm_base, serial);

    return;
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping
};
#endif


static void handle_global(void * data, struct wl_registry * registry,
                          uint32_t name, const char * interface, uint32_t version)
{
    struct application * app = data;

    LV_UNUSED(version);
    LV_UNUSED(data);

    if(strcmp(interface, wl_compositor_interface.name) == 0) {
        LV_LOG_WARN("Found compositor, %s", interface);

        app->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
    }
    else if(strcmp(interface, wl_subcompositor_interface.name) == 0) {
        LV_LOG_WARN("Found subcompositor, %s", interface);
        app->subcompositor = wl_registry_bind(registry, name, &wl_subcompositor_interface, 1);
    }
    else if(strcmp(interface, wl_shm_interface.name) == 0) {
        LV_LOG_WARN("Found shm, %s", interface);
        app->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
        wl_shm_add_listener(app->shm, &shm_listener, app);
        app->cursor_theme = wl_cursor_theme_load(NULL, 32, app->shm);
    }
    else if(strcmp(interface, wl_seat_interface.name) == 0) {
        LV_LOG_WARN("Found seat, %s", interface);
        app->wl_seat = wl_registry_bind(app->registry, name, &wl_seat_interface, 1);
        wl_seat_add_listener(app->wl_seat, &seat_listener, app);
    }
#if LV_WAYLAND_WL_SHELL
    else if(strcmp(interface, wl_shell_interface.name) == 0) {
        app->wl_shell = wl_registry_bind(registry, name, &wl_shell_interface, 1);
    }
#endif
#if LV_WAYLAND_XDG_SHELL
    else if(strcmp(interface, xdg_wm_base_interface.name) == 0) {
        LV_LOG_WARN("Found xdg wm base interface, %s", interface);
        /* supporting version 2 of the XDG protocol - ensures greater compatibility */
        app->xdg_wm = wl_registry_bind(app->registry, name, &xdg_wm_base_interface, 2);
        xdg_wm_base_add_listener(app->xdg_wm, &xdg_wm_base_listener, app);
    }
#endif
}

static void handle_global_remove(void * data, struct wl_registry * registry, uint32_t name)
{

    LV_UNUSED(data);
    LV_UNUSED(registry);
    LV_UNUSED(name);

}

static const struct wl_registry_listener registry_listener = {
    .global         = handle_global,
    .global_remove  = handle_global_remove
};

static void handle_wl_buffer_release(void * data, struct wl_buffer * wl_buffer)
{
    const struct smm_buffer_properties * props;
    struct graphic_object * obj;
    struct window * window;
    smm_buffer_t * buf;

    buf = (smm_buffer_t *)data;
    props = SMM_BUFFER_PROPERTIES(buf);
    obj = SMM_GROUP_PROPERTIES(props->group)->tag[TAG_LOCAL];
    window = obj->window;

    LV_LOG_TRACE("releasing buffer %p wl_buffer %p w:%d h:%d frame: %d", (smm_buffer_t *)data, (void *)wl_buffer,
                 obj->width,
                 obj->height, window->frame_counter);
    smm_release((smm_buffer_t *)data);
}

static const struct wl_buffer_listener wl_buffer_listener = {
    .release = handle_wl_buffer_release,
};

static void cache_clear(struct window * window)
{
    window->dmg_cache.start = window->dmg_cache.end;
    window->dmg_cache.size = 0;
}

static void cache_purge(struct window * window, smm_buffer_t * buf)
{
    lv_area_t * next_dmg;
    smm_buffer_t * next_buf = smm_next(buf);

    /* Remove all damage areas up until start of next buffers damage */
    if(next_buf == NULL) {
        cache_clear(window);
    }
    else {
        next_dmg = SMM_BUFFER_PROPERTIES(next_buf)->tag[TAG_BUFFER_DAMAGE];
        while((window->dmg_cache.cache + window->dmg_cache.start) != next_dmg) {
            window->dmg_cache.start++;
            window->dmg_cache.start %= DMG_CACHE_CAPACITY;
            window->dmg_cache.size--;
        }
    }
}

static void cache_add_area(struct window * window, smm_buffer_t * buf, const lv_area_t * area)
{
    if(SMM_BUFFER_PROPERTIES(buf)->tag[TAG_BUFFER_DAMAGE] == NULL) {
        /* Buffer damage beyond cache capacity */
        goto done;
    }

    if((window->dmg_cache.start == window->dmg_cache.end) &&
       (window->dmg_cache.size)) {
        /* This buffer has more damage then the cache's capacity, so
         * clear cache and leave buffer damage unrecorded
         */
        cache_clear(window);
        SMM_TAG(buf, TAG_BUFFER_DAMAGE, NULL);
        goto done;
    }

    /* Add damage area to cache */
    memcpy(window->dmg_cache.cache + window->dmg_cache.end,
           area,
           sizeof(lv_area_t));
    window->dmg_cache.end++;
    window->dmg_cache.end %= DMG_CACHE_CAPACITY;
    window->dmg_cache.size++;

done:
    return;
}

static void cache_apply_areas(struct window * window, void * dest, void * src, smm_buffer_t * src_buf)
{
    unsigned long offset;
    unsigned char start;
    int32_t y;
    lv_area_t * dmg;
    lv_area_t * next_dmg;
    smm_buffer_t * next_buf = smm_next(src_buf);
    const struct smm_buffer_properties * props = SMM_BUFFER_PROPERTIES(src_buf);
    struct graphic_object * obj = SMM_GROUP_PROPERTIES(props->group)->tag[TAG_LOCAL];
    uint8_t bpp;

    if(next_buf == NULL) {
        next_dmg = (window->dmg_cache.cache + window->dmg_cache.end);
    }
    else {
        next_dmg = SMM_BUFFER_PROPERTIES(next_buf)->tag[TAG_BUFFER_DAMAGE];
    }

    bpp = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);

    /* Apply all buffer damage areas */
    start = ((lv_area_t *)SMM_BUFFER_PROPERTIES(src_buf)->tag[TAG_BUFFER_DAMAGE] - window->dmg_cache.cache);
    while((window->dmg_cache.cache + start) != next_dmg) {
        /* Copy an area from source to destination (line-by-line) */
        dmg = (window->dmg_cache.cache + start);
        for(y = dmg->y1; y <= dmg->y2; y++) {
            offset = (dmg->x1 + (y * obj->width)) * bpp;

            memcpy(((char *)dest) + offset,
                   ((char *)src) + offset,
                   ((dmg->x2 - dmg->x1 + 1) * bpp));
        }


        start++;
        start %= DMG_CACHE_CAPACITY;
    }

}

static bool sme_new_pool(void * ctx, smm_pool_t * pool)
{
    struct wl_shm_pool * wl_pool;
    struct application * app = ctx;
    const struct smm_pool_properties * props = SMM_POOL_PROPERTIES(pool);

    LV_UNUSED(ctx);

    wl_pool = wl_shm_create_pool(app->shm,
                                 props->fd,
                                 props->size);

    SMM_TAG(pool, TAG_LOCAL, wl_pool);
    return (wl_pool == NULL);
}

static void sme_expand_pool(void * ctx, smm_pool_t * pool)
{
    const struct smm_pool_properties * props = SMM_POOL_PROPERTIES(pool);

    LV_UNUSED(ctx);

    wl_shm_pool_resize(props->tag[TAG_LOCAL], props->size);
}

static void sme_free_pool(void * ctx, smm_pool_t * pool)
{
    struct wl_shm_pool * wl_pool = SMM_POOL_PROPERTIES(pool)->tag[TAG_LOCAL];

    LV_UNUSED(ctx);

    wl_shm_pool_destroy(wl_pool);
}

static bool sme_new_buffer(void * ctx, smm_buffer_t * buf)
{
    struct wl_buffer * wl_buf;
    bool fail_alloc = true;
    const struct smm_buffer_properties * props = SMM_BUFFER_PROPERTIES(buf);
    struct wl_shm_pool * wl_pool = SMM_POOL_PROPERTIES(props->pool)->tag[TAG_LOCAL];
    struct application * app = ctx;
    struct graphic_object * obj = SMM_GROUP_PROPERTIES(props->group)->tag[TAG_LOCAL];
    uint8_t bpp;

    LV_LOG_TRACE("create new buffer of width %d height %d", obj->width, obj->height);

    bpp = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);
    wl_buf = wl_shm_pool_create_buffer(wl_pool,
                                       props->offset,
                                       obj->width,
                                       obj->height,
                                       obj->width * bpp,
                                       app->shm_format);

    if(wl_buf != NULL) {
        wl_buffer_add_listener(wl_buf, &wl_buffer_listener, buf);
        SMM_TAG(buf, TAG_LOCAL, wl_buf);
        SMM_TAG(buf, TAG_BUFFER_DAMAGE, NULL);
        fail_alloc = false;
    }

    return fail_alloc;
}

static bool sme_init_buffer(void * ctx, smm_buffer_t * buf)
{
    smm_buffer_t * src;
    void * src_base;
    bool fail_init = true;
    bool dmg_missing = false;
    void * buf_base = smm_map(buf);
    const struct smm_buffer_properties * props = SMM_BUFFER_PROPERTIES(buf);
    struct graphic_object * obj = SMM_GROUP_PROPERTIES(props->group)->tag[TAG_LOCAL];
    uint8_t bpp;

    LV_UNUSED(ctx);

    if(buf_base == NULL) {
        LV_LOG_ERROR("cannot map in buffer to initialize");
        goto done;
    }

    /* Determine if all subsequent buffers damage is recorded */
    for(src = smm_next(buf); src != NULL; src = smm_next(src)) {
        if(SMM_BUFFER_PROPERTIES(src)->tag[TAG_BUFFER_DAMAGE] == NULL) {
            dmg_missing = true;
            break;
        }
    }

    bpp = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);

    if((smm_next(buf) == NULL) || dmg_missing) {
        /* Missing subsequent buffer damage, initialize by copying the most
         * recently acquired buffers data
         */
        src = smm_latest(props->group);
        if((src != NULL) &&
           (src != buf)) {
            /* Map and copy latest buffer data */
            src_base = smm_map(src);
            if(src_base == NULL) {
                LV_LOG_ERROR("cannot map most recent buffer to copy");
                goto done;
            }

            memcpy(buf_base,
                   src_base,
                   (obj->width * bpp) * obj->height);
        }
    }
    else {
        /* All subsequent buffers damage is recorded, initialize by applying
         * their damage to this buffer
         */
        for(src = smm_next(buf); src != NULL; src = smm_next(src)) {
            src_base = smm_map(src);
            if(src_base == NULL) {
                LV_LOG_ERROR("cannot map source buffer to copy from");
                goto done;
            }

            cache_apply_areas(obj->window, buf_base, src_base, src);
        }

        /* Purge out-of-date cached damage (up to and including next buffer) */
        src = smm_next(buf);
        if(src == NULL) {
            cache_purge(obj->window, src);
        }
    }

    fail_init = false;
done:
    return fail_init;
}

static void sme_free_buffer(void * ctx, smm_buffer_t * buf)
{
    struct wl_buffer * wl_buf = SMM_BUFFER_PROPERTIES(buf)->tag[TAG_LOCAL];

    LV_UNUSED(ctx);

    wl_buffer_destroy(wl_buf);
}

static struct graphic_object * create_graphic_obj(struct application * app, struct window * window,
                                                  enum object_type type,
                                                  struct graphic_object * parent)
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

    obj->window = window;
    obj->type = type;
    obj->surface_configured = true;
    obj->pending_buffer = NULL;
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

#if LV_WAYLAND_WINDOW_DECORATIONS
static bool attach_decoration(struct window * window, struct graphic_object * decoration,
                              smm_buffer_t * decoration_buffer, struct graphic_object * parent)
{
    struct wl_buffer * wl_buf = SMM_BUFFER_PROPERTIES(decoration_buffer)->tag[TAG_LOCAL];

    int pos_x, pos_y;

    switch(decoration->type) {
        case OBJECT_TITLEBAR:
            pos_x = 0;
            pos_y = -TITLE_BAR_HEIGHT;
            break;
        case OBJECT_BUTTON_CLOSE:
            pos_x = parent->width - 1 * (BUTTON_MARGIN + BUTTON_SIZE);
            pos_y = -1 * (BUTTON_MARGIN + BUTTON_SIZE + (BORDER_SIZE / 2));
            break;
#if LV_WAYLAND_XDG_SHELL
        case OBJECT_BUTTON_MAXIMIZE:
            pos_x = parent->width - 2 * (BUTTON_MARGIN + BUTTON_SIZE);
            pos_y = -1 * (BUTTON_MARGIN + BUTTON_SIZE + (BORDER_SIZE / 2));
            break;
        case OBJECT_BUTTON_MINIMIZE:
            pos_x = parent->width - 3 * (BUTTON_MARGIN + BUTTON_SIZE);
            pos_y = -1 * (BUTTON_MARGIN + BUTTON_SIZE + (BORDER_SIZE / 2));
            break;
#endif
        case OBJECT_BORDER_TOP:
            pos_x = -BORDER_SIZE;
            pos_y = -(BORDER_SIZE + TITLE_BAR_HEIGHT);
            break;
        case OBJECT_BORDER_BOTTOM:
            pos_x = -BORDER_SIZE;
            pos_y = parent->height;
            break;
        case OBJECT_BORDER_LEFT:
            pos_x = -BORDER_SIZE;
            pos_y = -TITLE_BAR_HEIGHT;
            break;
        case OBJECT_BORDER_RIGHT:
            pos_x = parent->width;
            pos_y = -TITLE_BAR_HEIGHT;
            break;
        default:
            LV_ASSERT_MSG(0, "Invalid object type");
            return false;
    }

    /* Enable this, to make it function on weston 10.0.2 */
    /* It's not elegant but it forces weston to size the surfaces before */
    /* the conversion to a subsurface takes place */

    /* Likely related to this issue, some patches were merged into 10.0.0 */
    /* https://gitlab.freedesktop.org/wayland/weston/-/issues/446 */
    /* Moreover, it crashes on GNOME */

#if 0
    wl_surface_attach(decoration->surface, wl_buf, 0, 0);
    wl_surface_commit(decoration->surface);
#endif

    if(decoration->subsurface == NULL) {
        /* Create the subsurface only once */

        decoration->subsurface = wl_subcompositor_get_subsurface(window->application->subcompositor,
                                                                 decoration->surface,
                                                                 parent->surface);
        if(!decoration->subsurface) {
            LV_LOG_ERROR("cannot get subsurface for decoration");
            goto err_destroy_surface;
        }
    }

    wl_subsurface_set_position(decoration->subsurface, pos_x, pos_y);
    wl_surface_attach(decoration->surface, wl_buf, 0, 0);
    wl_surface_commit(decoration->surface);

    return true;

err_destroy_surface:
    wl_surface_destroy(decoration->surface);
    decoration->surface = NULL;

    return false;
}

/*
 * Fills a buffer with a color
 * @description Used to draw the decorations, by writing directly to the SHM buffer,
 * most wayland compositors support the ARGB8888, XRGB8888, RGB565 formats
 *
 * For color depths usually not natively supported by wayland i.e RGB332, Grayscale
 * A conversion is performed to match the format of the SHM buffer read by the compositor.
 *
 * This function can also be used as a visual debugging aid to see how damage is applied
 *
 * @param pixels pointer to the buffer to fill
 * @param lv_color_t color the color that will be used for the fill
 * @param width width of the filled area
 * @param height height of the filled area
 *
 */
static void color_fill(void * pixels, lv_color_t color, uint32_t width, uint32_t height)
{

    switch(application.shm_format) {
        case WL_SHM_FORMAT_ARGB8888:
            color_fill_XRGB8888(pixels, color, width, height);
            break;
        case WL_SHM_FORMAT_RGB565:
            color_fill_RGB565(pixels, color, width, height);
            break;
        default:
            LV_ASSERT_MSG(0, "Unsupported WL_SHM_FORMAT");
            break;
    }
}

static void color_fill_XRGB8888(void * pixels, lv_color_t color, uint32_t width, uint32_t height)
{
    unsigned char * buf = pixels;
    unsigned char * buf_end;

    buf_end = (unsigned char *)((uint32_t *)buf + width * height);

    while(buf < buf_end) {
        *(buf++) = color.blue;
        *(buf++) = color.green;
        *(buf++) = color.red;
        *(buf++) = 0xFF;
    }

}

static void color_fill_RGB565(void * pixels, lv_color_t color, uint32_t width, uint32_t height)
{
    uint16_t * buf = pixels;
    uint16_t * buf_end;

    buf_end = (uint16_t *)buf + width * height;

    while(buf < buf_end) {
        *(buf++) = lv_color_to_u16(color);
    }
}

static bool create_decoration(struct window * window,
                              struct graphic_object * decoration,
                              int window_width, int window_height)
{
    smm_buffer_t * buf;
    void * buf_base;
    int x, y;
    lv_color_t * pixel;
    uint8_t bpp;

    switch(decoration->type) {
        case OBJECT_TITLEBAR:
            decoration->width = window_width;
            decoration->height = TITLE_BAR_HEIGHT;
            break;
        case OBJECT_BUTTON_CLOSE:
            decoration->width = BUTTON_SIZE;
            decoration->height = BUTTON_SIZE;
            break;
#if LV_WAYLAND_XDG_SHELL
        case OBJECT_BUTTON_MAXIMIZE:
            decoration->width = BUTTON_SIZE;
            decoration->height = BUTTON_SIZE;
            break;
        case OBJECT_BUTTON_MINIMIZE:
            decoration->width = BUTTON_SIZE;
            decoration->height = BUTTON_SIZE;
            break;
#endif
        case OBJECT_BORDER_TOP:
            decoration->width = window_width + 2 * (BORDER_SIZE);
            decoration->height = BORDER_SIZE;
            break;
        case OBJECT_BORDER_BOTTOM:
            decoration->width = window_width + 2 * (BORDER_SIZE);
            decoration->height = BORDER_SIZE;
            break;
        case OBJECT_BORDER_LEFT:
            decoration->width = BORDER_SIZE;
            decoration->height = window_height + TITLE_BAR_HEIGHT;
            break;
        case OBJECT_BORDER_RIGHT:
            decoration->width = BORDER_SIZE;
            decoration->height = window_height + TITLE_BAR_HEIGHT;
            break;
        default:
            LV_ASSERT_MSG(0, "Invalid object type");
            return false;
    }

    bpp = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);

    LV_LOG_TRACE("decoration window %dx%d", decoration->width, decoration->height);

    smm_resize(decoration->buffer_group,
               (decoration->width * bpp) * decoration->height);

    buf = smm_acquire(decoration->buffer_group);

    if(buf == NULL) {
        LV_LOG_ERROR("cannot allocate buffer for decoration");
        return false;
    }

    buf_base = smm_map(buf);
    if(buf_base == NULL) {
        LV_LOG_ERROR("cannot map in allocated decoration buffer");
        smm_release(buf);
        return false;
    }

    switch(decoration->type) {
        case OBJECT_TITLEBAR:
            color_fill(buf_base, lv_color_make(0x66, 0x66, 0x66), decoration->width, decoration->height);
            break;
        case OBJECT_BUTTON_CLOSE:
            color_fill(buf_base, lv_color_make(0xCC, 0xCC, 0xCC), decoration->width, decoration->height);
            for(y = 0; y < decoration->height; y++) {
                for(x = 0; x < decoration->width; x++) {
                    pixel = (lv_color_t *)((unsigned char *)buf_base + (y * (decoration->width * bpp)) + x * bpp);
                    if((x >= BUTTON_PADDING) && (x < decoration->width - BUTTON_PADDING)) {
                        if((x == y) || (x == decoration->width - 1 - y)) {
                            color_fill(pixel, lv_color_make(0x33, 0x33, 0x33), 1, 1);
                        }
                        else if((x == y - 1) || (x == decoration->width - y)) {
                            color_fill(pixel, lv_color_make(0x66, 0x66, 0x66), 1, 1);
                        }
                    }
                }
            }
            break;
#if LV_WAYLAND_XDG_SHELL
        case OBJECT_BUTTON_MAXIMIZE:
            color_fill(buf_base, lv_color_make(0xCC, 0xCC, 0xCC), decoration->width, decoration->height);
            for(y = 0; y < decoration->height; y++) {
                for(x = 0; x < decoration->width; x++) {
                    pixel = (lv_color_t *)((unsigned char *)buf_base + (y * (decoration->width * bpp)) + x * bpp);
                    if(((x == BUTTON_PADDING) && (y >= BUTTON_PADDING) && (y < decoration->height - BUTTON_PADDING)) ||
                       ((x == (decoration->width - BUTTON_PADDING)) && (y >= BUTTON_PADDING) && (y <= decoration->height - BUTTON_PADDING)) ||
                       ((y == BUTTON_PADDING) && (x >= BUTTON_PADDING) && (x < decoration->width - BUTTON_PADDING)) ||
                       ((y == (BUTTON_PADDING + 1)) && (x >= BUTTON_PADDING) && (x < decoration->width - BUTTON_PADDING)) ||
                       ((y == (decoration->height - BUTTON_PADDING)) && (x >= BUTTON_PADDING) && (x < decoration->width - BUTTON_PADDING))) {
                        color_fill(pixel, lv_color_make(0x33, 0x33, 0x33), 1, 1);
                    }
                }
            }
            break;
        case OBJECT_BUTTON_MINIMIZE:
            color_fill(buf_base, lv_color_make(0xCC, 0xCC, 0xCC), decoration->width, decoration->height);
            for(y = 0; y < decoration->height; y++) {
                for(x = 0; x < decoration->width; x++) {
                    pixel = (lv_color_t *)((unsigned char *)buf_base + (y * (decoration->width * bpp)) + x * bpp);
                    if((x >= BUTTON_PADDING) && (x < decoration->width - BUTTON_PADDING) &&
                       (y > decoration->height - (2 * BUTTON_PADDING)) && (y < decoration->height - BUTTON_PADDING)) {
                        color_fill(pixel, lv_color_make(0x33, 0x33, 0x33), 1, 1);
                    }
                }
            }
            break;
#endif
        case OBJECT_BORDER_TOP:
        /* fallthrough */
        case OBJECT_BORDER_BOTTOM:
        /* fallthrough */
        case OBJECT_BORDER_LEFT:
        /* fallthrough */
        case OBJECT_BORDER_RIGHT:
            color_fill(buf_base, lv_color_make(0x66, 0x66, 0x66), decoration->width, decoration->height);
            break;
        default:
            LV_ASSERT_MSG(0, "Invalid object type");
            return false;
    }

    return attach_decoration(window, decoration, buf, window->body);
}

static void detach_decoration(struct window * window,
                              struct graphic_object * decoration)
{

    LV_UNUSED(window);

    if(decoration->subsurface) {
        wl_subsurface_destroy(decoration->subsurface);
        decoration->subsurface = NULL;
    }
}
#endif

static bool resize_window(struct window * window, int width, int height)
{
    struct smm_buffer_t * body_buf1;
    struct smm_buffer_t * body_buf2;
    uint32_t stride;
    uint8_t bpp;
#if LV_WAYLAND_WINDOW_DECORATIONS
    int b;
#endif


    window->width = width;
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

    window->body->width = width;
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
        for(b = 0; b < NUM_DECORATIONS; b++) {
            if(!create_decoration(window, window->decoration[b],
                                  window->body->width, window->body->height)) {
                LV_LOG_ERROR("failed to create decoration %d", b);
            }
        }

    }
    else if(!window->application->opt_disable_decorations) {
        /* Entering fullscreen, detach decorations to prevent xdg_wm_base error 4 */
        /* requested geometry larger than the configured fullscreen state */
        for(b = 0; b < NUM_DECORATIONS; b++) {
            detach_decoration(window, window->decoration[b]);
        }

    }
#endif

    LV_LOG_TRACE("resize window:%dx%d body:%dx%d frame: %d rendered: %d",
                 window->width, window->height,
                 window->body->width, window->body->height,
                 window->frame_counter, window->frame_done);

    width = window->body->width;
    height = window->body->height;

    if(window->lv_disp != NULL) {
        /* Resize draw buffer */
        stride = lv_draw_buf_width_to_stride(width,
                                             lv_display_get_color_format(window->lv_disp));

        window->lv_disp_draw_buf = lv_draw_buf_reshape(
                                       window->lv_disp_draw_buf,
                                       lv_display_get_color_format(window->lv_disp),
                                       width, height / LVGL_DRAW_BUFFER_DIV, stride);

        lv_display_set_resolution(window->lv_disp, width, height);

        window->body->input.pointer.x = LV_MIN((int32_t)window->body->input.pointer.x, (width - 1));
        window->body->input.pointer.y = LV_MIN((int32_t)window->body->input.pointer.y, (height - 1));
    }

    return true;
}

/* Create a window
 * @description Creates the graphical context for the window body, and then create a toplevel
 * wayland surface and commit it to obtain an XDG configuration event
 * @param width the height of the window w/decorations
 * @param height the width of the window w/decorations
*/
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
    window->body = create_graphic_obj(app, window, OBJECT_WINDOW, NULL);
    window->width = width;
    window->height = height;

    if(!window->body) {
        LV_LOG_ERROR("cannot create window body");
        goto err_free_window;
    }

    // Create shell surface
    if(0) {
        // Needed for #if madness below
    }
#if LV_WAYLAND_XDG_SHELL
    else if(app->xdg_wm) {
        window->xdg_surface = xdg_wm_base_get_xdg_surface(app->xdg_wm, window->body->surface);
        if(!window->xdg_surface) {
            LV_LOG_ERROR("cannot create XDG surface");
            goto err_destroy_surface;
        }

        xdg_surface_add_listener(window->xdg_surface, &xdg_surface_listener, window);

        window->xdg_toplevel = xdg_surface_get_toplevel(window->xdg_surface);
        if(!window->xdg_toplevel) {
            LV_LOG_ERROR("cannot get XDG toplevel surface");
            goto err_destroy_shell_surface;
        }

        xdg_toplevel_add_listener(window->xdg_toplevel, &xdg_toplevel_listener, window);
        xdg_toplevel_set_title(window->xdg_toplevel, title);
        xdg_toplevel_set_app_id(window->xdg_toplevel, title);

        // XDG surfaces need to be configured before a buffer can be attached.
        // An (XDG) surface commit (without an attached buffer) triggers this
        // configure event
        window->body->surface_configured = false;
    }
#endif
#if LV_WAYLAND_WL_SHELL
    else if(app->wl_shell) {
        window->wl_shell_surface = wl_shell_get_shell_surface(app->wl_shell, window->body->surface);
        if(!window->wl_shell_surface) {
            LV_LOG_ERROR("cannot create WL shell surface");
            goto err_destroy_surface;
        }

        wl_shell_surface_add_listener(window->wl_shell_surface, &shell_surface_listener, window);
        wl_shell_surface_set_toplevel(window->wl_shell_surface);
        wl_shell_surface_set_title(window->wl_shell_surface, title);

        /* For wl_shell, just draw the window, weston doesn't send it */
        draw_window(window, window->width, window->height);
    }
#endif
    else {
        LV_LOG_ERROR("No shell available");
        goto err_destroy_surface;
    }


    return window;

err_destroy_shell_surface:
#if LV_WAYLAND_WL_SHELL
    if(window->wl_shell_surface) {
        wl_shell_surface_destroy(window->wl_shell_surface);
    }
#endif
#if LV_WAYLAND_XDG_SHELL
    if(window->xdg_surface) {
        xdg_surface_destroy(window->xdg_surface);
    }
#endif

err_destroy_surface:
    wl_surface_destroy(window->body->surface);

err_free_window:
    lv_ll_remove(&app->window_ll, window);
    lv_free(window);
    return NULL;
}

static void destroy_window(struct window * window)
{
    if(!window) {
        return;
    }

#if LV_WAYLAND_WL_SHELL
    if(window->wl_shell_surface) {
        wl_shell_surface_destroy(window->wl_shell_surface);
    }
#endif
#if LV_WAYLAND_XDG_SHELL
    if(window->xdg_toplevel) {
        xdg_toplevel_destroy(window->xdg_toplevel);
        xdg_surface_destroy(window->xdg_surface);
    }
#endif

#if LV_WAYLAND_WINDOW_DECORATIONS
    int b;
    for(b = 0; b < NUM_DECORATIONS; b++) {
        if(window->decoration[b]) {
            destroy_graphic_obj(window->decoration[b]);
            window->decoration[b] = NULL;
        }
    }
#endif

    destroy_graphic_obj(window->body);
}

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

    window = lv_display_get_user_data(disp);
    app = window->application;
    buf = window->body->pending_buffer;
    src_width = lv_area_get_width(area);
    src_height = lv_area_get_height(area);
    bpp = lv_color_format_get_size(LV_COLOR_FORMAT_NATIVE);

    rot = lv_display_get_rotation(disp);
    w = lv_display_get_horizontal_resolution(disp);
    h = lv_display_get_vertical_resolution(disp);

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
        SMM_TAG(buf,
                TAG_BUFFER_DAMAGE,
                window->dmg_cache.cache + window->dmg_cache.end);
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
        memcpy(((char *)buf_base) + ((((area->y1 + y) * hres) + area->x1) * bpp),
               color_p, src_width * bpp);
        color_p += src_width * bpp;
    }

    /* Mark surface damage */
    wl_surface_damage(window->body->surface,
                      area->x1,
                      area->y1,
                      src_width,
                      src_height);

    cache_add_area(window, buf, area);


    if(lv_display_flush_is_last(disp)) {
        /* Finally, attach buffer and commit to surface */
        wl_buf = SMM_BUFFER_PROPERTIES(buf)->tag[TAG_LOCAL];
        wl_surface_attach(window->body->surface, wl_buf, 0, 0);
        wl_surface_commit(window->body->surface);
        window->body->pending_buffer = NULL;
        window->frame_done = false;

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
        cache_clear(window);
        SMM_TAG(buf, TAG_BUFFER_DAMAGE, NULL);
        smm_release(buf);
        window->body->pending_buffer = NULL;
    }
}

static void _lv_wayland_handle_input(void)
{
    int prepare_read = wl_display_prepare_read(application.display);

    while(prepare_read != 0) {
        wl_display_dispatch_pending(application.display);
    }

    wl_display_read_events(application.display);
    wl_display_dispatch_pending(application.display);
}

static void _lv_wayland_handle_output(void)
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
            window->closed = true;
            window->shall_close = false;
            shall_flush = true;

            window->body->input.pointer.x = 0;
            window->body->input.pointer.y = 0;
            window->body->input.pointer.left_button = LV_INDEV_STATE_RELEASED;
            window->body->input.pointer.right_button = LV_INDEV_STATE_RELEASED;
            window->body->input.pointer.wheel_button = LV_INDEV_STATE_RELEASED;
            window->body->input.pointer.wheel_diff = 0;
            if(window->application->pointer_obj == window->body) {
                window->application->pointer_obj = NULL;
            }

            window->body->input.keyboard.key = 0;
            window->body->input.keyboard.state = LV_INDEV_STATE_RELEASED;
            if(window->application->keyboard_obj == window->body) {
                window->application->keyboard_obj = NULL;
            }
            destroy_window(window);
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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize Wayland driver
 */
static void wayland_init(void)
{
    struct smm_events evs = {
        NULL,
        sme_new_pool,
        sme_expand_pool,
        sme_free_pool,
        sme_new_buffer,
        sme_init_buffer,
        sme_free_buffer
    };

    // Create XKB context
    application.xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    LV_ASSERT_MSG(application.xkb_context, "failed to create XKB context");
    if(application.xkb_context == NULL) {
        return;
    }

    LV_LOG_USER("---- Display Connect");
    // Connect to Wayland display
    application.display = wl_display_connect(NULL);
    LV_ASSERT_MSG(application.display, "failed to connect to Wayland server");
    if(application.display == NULL) {
        return;
    }

    LV_LOG_USER("---- Display Connected");
    /* Add registry listener and wait for registry reception */
    application.shm_format = SHM_FORMAT_UNKNOWN;
    LV_LOG_USER("---- Display Get Registry");
    application.registry = wl_display_get_registry(application.display);
    LV_LOG_USER("---- Display Adding listener");
    wl_registry_add_listener(application.registry, &registry_listener, &application);
    LV_LOG_USER("---- Display Dispatching");
    wl_display_dispatch(application.display);
    LV_LOG_USER("---- Roundtrip");
    wl_display_roundtrip(application.display);
    LV_LOG_USER("---- Roundtrip done");

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

    smm_init(&evs);
    smm_setctx(&application);

#ifdef LV_WAYLAND_WINDOW_DECORATIONS
    const char * env_disable_decorations = getenv("LV_WAYLAND_DISABLE_WINDOWDECORATION");
    application.opt_disable_decorations = ((env_disable_decorations != NULL) &&
                                           (env_disable_decorations[0] != '0'));
#endif

    lv_ll_init(&application.window_ll, sizeof(struct window));

    lv_tick_set_cb(tick_get_cb);

    /* Used to wait for events when the window is minimized or hidden */
    application.wayland_pfd.fd = wl_display_get_fd(application.display);
    application.wayland_pfd.events = POLLIN;

}

/**
 * De-initialize Wayland driver
 */
static void wayland_deinit(void)
{
    struct window * window = NULL;

    LV_LL_READ(&application.window_ll, window) {
        if(!window->closed) {
            destroy_window(window);
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

static uint32_t tick_get_cb(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    uint64_t time_ms = t.tv_sec * 1000 + (t.tv_nsec / 1000000);
    return time_ms;
}

/**
 * Get Wayland display file descriptor
 * @return Wayland display file descriptor
 */
int lv_wayland_get_fd(void)
{
    return wl_display_get_fd(application.display);
}

/**
 * Create wayland window
 * @param hor_res initial horizontal window body size in pixels
 * @param ver_res initial vertical window body size in pixels
 * @param title window title
 * @param close_cb function to be called when the window gets closed by the user (optional)
 * @return new display backed by a Wayland window, or NULL on error
 */
lv_display_t * lv_wayland_window_create(uint32_t hor_res, uint32_t ver_res, char * title,
                                        lv_wayland_display_close_f_t close_cb)
{
    struct window * window;
    int32_t window_width;
    int32_t window_height;
    int32_t stride;

    wayland_init();

    window_width = hor_res;
    window_height = ver_res;

#if LV_WAYLAND_WINDOW_DECORATIONS

    /* Decorations are enabled, calculate the body size */
    if(!application.opt_disable_decorations) {
        window_width = hor_res + (2 * BORDER_SIZE);
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

    stride = lv_draw_buf_width_to_stride(hor_res,
                                         lv_display_get_color_format(window->lv_disp));

    window->lv_disp_draw_buf = lv_draw_buf_create(
                                   hor_res,
                                   ver_res / LVGL_DRAW_BUFFER_DIV,
                                   lv_display_get_color_format(window->lv_disp),
                                   stride);


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

/**
 * Close wayland window
 * @param disp LVGL display using window to be closed
 */
void lv_wayland_window_close(lv_display_t * disp)
{
    struct window * window = lv_display_get_user_data(disp);
    if(!window || window->closed) {
        return;
    }
    window->shall_close = true;
    window->close_cb = NULL;
    wayland_deinit();
}

/**
 * Check if a Wayland window is open on the specified display. Otherwise (if
 * argument is NULL), check if any Wayland window is open.
 * @return true if window open, false otherwise
 */
bool lv_wayland_window_is_open(lv_display_t * disp)
{
    struct window * window;
    bool open = false;

    if(disp == NULL) {
        LV_LL_READ(&application.window_ll, window) {
            if(!window->closed) {
                open = true;
                break;
            }
        }
    }
    else {
        window = lv_display_get_user_data(disp);
        open = (!window->closed);
    }

    return open;
}
/**
 * Set/unset window maximization mode
 * @description Maximization is nearly the same as fullscreen, except
 * window decorations and the compositor's panels must remain visible
 * @param disp LVGL display using window to be set/unset maximization
 * @param Maximization requested status (true = maximized)
 */
void lv_wayland_window_set_maximized(lv_display_t * disp, bool maximized)
{
    struct window * window = lv_display_get_user_data(disp);
    if(!window || window->closed) {
        return;
    }

    if(window->maximized != maximized) {

#if LV_WAYLAND_WL_SHELL
        if(window->wl_shell_surface) {
            if(maximized) {
                /* Maximizing the wl_shell is possible, but requires binding to wl_output */
                /* The wl_shell has been deperacted */
                //wl_shell_surface_set_maximized(window->wl_shell_surface);
            }
            else {
                wl_shell_surface_set_toplevel(window->wl_shell_surface);
            }
            window->maximized = maximized;
            window->flush_pending = true;
        }
#endif

#if LV_WAYLAND_XDG_SHELL
        if(window->xdg_toplevel) {
            if(maximized) {
                xdg_toplevel_set_maximized(window->xdg_toplevel);
            }
            else {
                xdg_toplevel_unset_maximized(window->xdg_toplevel);
            }

            window->maximized = maximized;
            window->flush_pending = true;
        }
#endif
    }
}

/**
 * Set/unset window fullscreen mode
 * @param disp LVGL display using window to be set/unset fullscreen
 * @param fullscreen requested status (true = fullscreen)
 */
void lv_wayland_window_set_fullscreen(lv_display_t * disp, bool fullscreen)
{
    struct window * window = lv_display_get_user_data(disp);
    if(!window || window->closed) {
        return;
    }

    if(window->fullscreen != fullscreen) {
        if(0) {
            // Needed for #if madness below
        }
#if LV_WAYLAND_XDG_SHELL
        else if(window->xdg_toplevel) {
            if(fullscreen) {
                xdg_toplevel_set_fullscreen(window->xdg_toplevel, NULL);
            }
            else {
                xdg_toplevel_unset_fullscreen(window->xdg_toplevel);
            }
            window->fullscreen = fullscreen;
            window->flush_pending = true;
        }
#endif
#if LV_WAYLAND_WL_SHELL
        else if(window->wl_shell_surface) {
            if(fullscreen) {
                wl_shell_surface_set_fullscreen(window->wl_shell_surface,
                                                WL_SHELL_SURFACE_FULLSCREEN_METHOD_SCALE,
                                                0, NULL);
            }
            else {
                wl_shell_surface_set_toplevel(window->wl_shell_surface);
            }
            window->fullscreen = fullscreen;
            window->flush_pending = true;
        }
#endif
        else {
            LV_LOG_WARN("Wayland fullscreen mode not supported");
        }
    }
}

/**
 * Get pointer input device for given LVGL display
 * @param disp LVGL display
 * @return input device connected to pointer events, or NULL on error
 */
lv_indev_t * lv_wayland_get_pointer(lv_display_t * disp)
{
    struct window * window = lv_display_get_user_data(disp);
    if(!window) {
        return NULL;
    }
    return window->lv_indev_pointer;
}

/**
 * Get pointer axis input device for given LVGL display
 * @param disp LVGL display
 * @return input device connected to pointer axis events, or NULL on error
 */
lv_indev_t * lv_wayland_get_pointeraxis(lv_display_t * disp)
{
    struct window * window = lv_display_get_user_data(disp);
    if(!window) {
        return NULL;
    }
    return window->lv_indev_pointeraxis;
}

/**
 * Get keyboard input device for given LVGL display
 * @param disp LVGL display
 * @return input device connected to keyboard, or NULL on error
 */
lv_indev_t * lv_wayland_get_keyboard(lv_display_t * disp)
{
    struct window * window = lv_display_get_user_data(disp);
    if(!window) {
        return NULL;
    }
    return window->lv_indev_keyboard;
}

/**
 * Get touchscreen input device for given LVGL display
 * @param disp LVGL display
 * @return input device connected to touchscreen, or NULL on error
 */
lv_indev_t * lv_wayland_get_touchscreen(lv_display_t * disp)
{
    struct window * window = lv_display_get_user_data(disp);
    if(!window) {
        return NULL;
    }
    return window->lv_indev_touch;
}

/**
 * Wayland specific timer handler (use in place of LVGL lv_timer_handler)
 */
bool lv_wayland_timer_handler(void)
{
    struct window * window;

    /* Wayland input handling - it will also trigger the frame done handler */
    _lv_wayland_handle_input();

    /* Ready input timers (to probe for any input received) */
    LV_LL_READ(&application.window_ll, window) {
        LV_LOG_TRACE("handle timer frame: %d", window->frame_counter);

        if(window != NULL && window->frame_done == false
           && window->frame_counter > 0) {
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
            if(resize_window(window, window->resize_width, window->resize_height)) {
                window->resize_width = window->width;
                window->resize_height = window->height;
                window->resize_pending = false;

            }
            else {

                LV_LOG_TRACE("Failed to resize window frame: %d",
                             window->frame_counter);
            }
        }
        else if(window->shall_close == true) {

            /* Destroy graphical context and execute close_cb */
            _lv_wayland_handle_output();
            wayland_deinit();
            return false;
        }
    }

    /* LVGL handling */
    lv_timer_handler();

    /* Wayland output handling */
    _lv_wayland_handle_output();

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

#endif /* LV_USE_WAYLAND */
#endif /* _WIN32 */
