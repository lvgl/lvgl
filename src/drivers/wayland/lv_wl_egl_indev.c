/**
 * @file lv_wl_egl_indev.c
 *
 */

/**
 * Modified by Renesas in 2025
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_wl_egl_private.h"

#if LV_USE_WAYLAND && LV_WAYLAND_USE_EGL
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <wayland-client-protocol.h>
#include <wayland-cursor.h>
#include <xkbcommon/xkbcommon.h>
#include <linux/input-event-codes.h>
#include "../../misc/lv_log.h"
#include "lv_wayland_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *      Seat
 **********************/

static void seat_handle_capabilities(void * data, struct wl_seat * wl_seat, enum wl_seat_capability caps);

/**********************
 *      Keyboard
 **********************/

static lv_indev_t * lv_wl_egl_keyboard_create(void);
static void keyboard_read(lv_indev_t * drv, lv_indev_data_t * data);
static void keyboard_handle_keymap(void * data, struct wl_keyboard * keyboard, uint32_t format, int fd, uint32_t size);
static void keyboard_handle_enter(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                  struct wl_surface * surface, struct wl_array * keys);
static void keyboard_handle_leave(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                  struct wl_surface * surface);
static void keyboard_handle_modifiers(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                      uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
                                      uint32_t group);
static void keyboard_handle_key(void * data, struct wl_keyboard * keyboard, uint32_t serial, uint32_t time,
                                uint32_t key, uint32_t state);
static lv_key_t keycode_xkb_to_lv(xkb_keysym_t xkb_key);

/**********************
 *      Pointer
 **********************/

static lv_indev_t * lv_wl_egl_pointer_create(void);
static lv_indev_t * lv_wl_egl_pointeraxis_create(void);
static void pointeraxis_read(lv_indev_t * drv, lv_indev_data_t * data);
static void pointer_read(lv_indev_t * drv, lv_indev_data_t * data);
static void pointer_handle_enter(void * data, struct wl_pointer * pointer, uint32_t serial, struct wl_surface * surface,
                                 wl_fixed_t sx, wl_fixed_t sy);
static void pointer_handle_leave(void * data, struct wl_pointer * pointer, uint32_t serial,
                                 struct wl_surface * surface);
static void pointer_handle_motion(void * data, struct wl_pointer * pointer, uint32_t time, wl_fixed_t sx,
                                  wl_fixed_t sy);
static void pointer_handle_button(void * data, struct wl_pointer * wl_pointer, uint32_t serial, uint32_t time,
                                  uint32_t button, uint32_t state);
static void pointer_handle_axis(void * data, struct wl_pointer * wl_pointer, uint32_t time, uint32_t axis,
                                wl_fixed_t value);

/**********************
 *      Touch
 **********************/

#if LV_USE_GESTURE_RECOGNITION
static lv_indev_t * lv_wl_egl_touch_create(void);
static void touch_read(lv_indev_t * drv, lv_indev_data_t * data);
static void touch_handle_down(void * data, struct wl_touch * wl_touch, uint32_t serial, uint32_t time,
                              struct wl_surface * surface, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w);
static void touch_handle_up(void * data, struct wl_touch * wl_touch, uint32_t serial, uint32_t time, int32_t id);
static void touch_handle_motion(void * data, struct wl_touch * wl_touch, uint32_t time, int32_t id, wl_fixed_t x_w,
                                wl_fixed_t y_w);
static void touch_handle_frame(void * data, struct wl_touch * wl_touch);
static void touch_handle_cancel(void * data, struct wl_touch * wl_touch);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      Seat
 **********************/

static const struct wl_seat_listener seat_listener = {
    .capabilities = seat_handle_capabilities,
};

/**********************
 *      Keyboard
 **********************/

static const struct wl_keyboard_listener keyboard_listener = {
    .keymap    = keyboard_handle_keymap,
    .enter     = keyboard_handle_enter,
    .leave     = keyboard_handle_leave,
    .key       = keyboard_handle_key,
    .modifiers = keyboard_handle_modifiers,
};

/**********************
 *      Pointer
 **********************/

static const struct wl_pointer_listener pointer_listener = {
    .enter  = pointer_handle_enter,
    .leave  = pointer_handle_leave,
    .motion = pointer_handle_motion,
    .button = pointer_handle_button,
    .axis   = pointer_handle_axis,
};

/**********************
 *      Touch
 **********************/

#if LV_USE_GESTURE_RECOGNITION
static const struct wl_touch_listener touch_listener = {
    .down   = touch_handle_down,
    .up     = touch_handle_up,
    .motion = touch_handle_motion,
    .frame  = touch_handle_frame,
    .cancel = touch_handle_cancel,
};
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_wl_egl_indev_init(lv_wl_egl_window_t * window)
{
    if(!window) {
        LV_LOG_ERROR("Window is NULL");
        return false;
    }

    LV_LOG_INFO("Initializing input devices");

    // Initialize input state
    memset(&window->indev, 0, sizeof(lv_wl_egl_indev_t));

    window->indev.pointer.x = 0;
    window->indev.pointer.y = 0;
    window->indev.pointer.left_button = LV_INDEV_STATE_RELEASED;
    window->indev.pointer.right_button = LV_INDEV_STATE_RELEASED;
    window->indev.pointer.wheel_button = LV_INDEV_STATE_RELEASED;
    window->indev.pointer.wheel_diff = 0;

    window->indev.keyboard.key = 0;
    window->indev.keyboard.state = LV_INDEV_STATE_RELEASED;

#if LV_USE_GESTURE_RECOGNITION
    window->indev.touch_event_cnt = 0;
    memset(window->indev.touches, 0, sizeof(window->indev.touches));
#endif

    // Create LVGL input devices
    window->lv_indev_keyboard = lv_wl_egl_keyboard_create();
    if(!window->lv_indev_keyboard) {
        LV_LOG_ERROR("failed to register keyboard indev");
        return false;
    }
    lv_indev_set_display(window->lv_indev_keyboard, window->lv_display);
    lv_indev_set_user_data(window->lv_indev_keyboard, window);

    window->lv_indev_pointer = lv_wl_egl_pointer_create();
    if(!window->lv_indev_pointer) {
        LV_LOG_ERROR("failed to register pointer indev");
        return false;
    }
    lv_indev_set_display(window->lv_indev_pointer, window->lv_display);
    lv_indev_set_user_data(window->lv_indev_pointer, window);

    window->lv_indev_pointeraxis = lv_wl_egl_pointeraxis_create();
    if(!window->lv_indev_pointeraxis) {
        LV_LOG_ERROR("failed to register pointeraxis indev");
        return false;
    }
    lv_indev_set_display(window->lv_indev_pointeraxis, window->lv_display);
    lv_indev_set_user_data(window->lv_indev_pointeraxis, window);

#if LV_USE_GESTURE_RECOGNITION
    window->lv_indev_touch = lv_wl_egl_touch_create();
    if(!window->lv_indev_touch) {
        LV_LOG_ERROR("failed to register touch indev");
        return false;
    }
    lv_indev_set_display(window->lv_indev_touch, window->lv_display);
    lv_indev_set_user_data(window->lv_indev_touch, window);
#endif

    LV_LOG_INFO("Input devices initialized successfully");
    return true;
}

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

/**********************
 *      Seat
 **********************/

const struct wl_seat_listener * lv_wl_egl_seat_get_listener(void)
{
    return &seat_listener;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**********************
 *      Seat
 **********************/

static void seat_handle_capabilities(void * data, struct wl_seat * wl_seat, enum wl_seat_capability caps)
{
    lv_wl_egl_window_t * window = (lv_wl_egl_window_t *)data;
    lv_wl_egl_indev_t * indev;

    if(!window) {
        LV_LOG_ERROR("Window is NULL in seat_handle_capabilities");
        return;
    }

    if(!wl_seat) {
        LV_LOG_ERROR("wl_seat is NULL in seat_handle_capabilities");
        return;
    }

    indev = &window->indev;

    if((caps & WL_SEAT_CAPABILITY_POINTER) && !indev->wl_pointer) {
        indev->wl_pointer = wl_seat_get_pointer(wl_seat);
        if(!indev->wl_pointer) {
            LV_LOG_ERROR("Failed to get pointer from seat");
        }
        else {
            wl_pointer_add_listener(indev->wl_pointer, &pointer_listener, window);

            if(!window->cursor_surface && window->compositor) {
                window->cursor_surface = wl_compositor_create_surface(window->compositor);
                if(!window->cursor_surface) {
                    LV_LOG_ERROR("Failed to create cursor surface");
                }
            }

            if(!window->cursor_theme && window->shm) {
                window->cursor_theme = wl_cursor_theme_load(NULL, 24, window->shm);
                if(!window->cursor_theme) {
                    LV_LOG_ERROR("Failed to load cursor theme");
                }
            }
        }
    }
    else if(!(caps & WL_SEAT_CAPABILITY_POINTER) && indev->wl_pointer) {
        wl_pointer_destroy(indev->wl_pointer);
        if(window->cursor_surface) {
            wl_surface_destroy(window->cursor_surface);
            window->cursor_surface = NULL;
        }
        indev->wl_pointer = NULL;
    }

    if((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !indev->wl_keyboard) {
        if(!window->xkb_context) {
            LV_LOG_ERROR("XKB context is NULL! Cannot create keyboard.");
            LV_LOG_ERROR("This should have been created in init_wayland()");
            return;
        }

        indev->wl_keyboard = wl_seat_get_keyboard(wl_seat);
        if(!indev->wl_keyboard) {
            LV_LOG_ERROR("Failed to get keyboard from seat");
        }
        else {
            wl_keyboard_add_listener(indev->wl_keyboard, &keyboard_listener, window);
        }
    }
    else if(!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && indev->wl_keyboard) {
        wl_keyboard_destroy(indev->wl_keyboard);
        indev->wl_keyboard = NULL;
    }

#if LV_USE_GESTURE_RECOGNITION
    if((caps & WL_SEAT_CAPABILITY_TOUCH) && !indev->wl_touch) {
        indev->wl_touch = wl_seat_get_touch(wl_seat);
        if(!indev->wl_touch) {
            LV_LOG_ERROR("Failed to get touch from seat");
        }
        else {
            wl_touch_add_listener(indev->wl_touch, &touch_listener, window);
        }
    }
    else if(!(caps & WL_SEAT_CAPABILITY_TOUCH) && indev->wl_touch) {
        wl_touch_destroy(indev->wl_touch);
        indev->wl_touch = NULL;
    }
#endif
}

/**********************
 *      Keyboard
 **********************/

static lv_indev_t * lv_wl_egl_keyboard_create(void)
{
    lv_indev_t * keyboard = lv_indev_create();
    lv_indev_set_type(keyboard, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(keyboard, keyboard_read);

    return keyboard;
}

static void keyboard_read(lv_indev_t * drv, lv_indev_data_t * data)
{
    lv_display_t * disp = lv_indev_get_display(drv);
    lv_wl_egl_window_t * window;

    if(!disp) {
        LV_LOG_ERROR("Display is NULL in keyboard_read");
        return;
    }

    window = (lv_wl_egl_window_t *)lv_display_get_user_data(disp);
    if(!window) {
        LV_LOG_ERROR("Window is NULL in keyboard_read");
        return;
    }

    data->key   = window->indev.keyboard.key;
    data->state = window->indev.keyboard.state;

    if(window->indev.keyboard.state == LV_INDEV_STATE_RELEASED) {
        window->indev.keyboard.key = 0;
    }
}

static void keyboard_handle_keymap(void * data, struct wl_keyboard * keyboard, uint32_t format, int fd, uint32_t size)
{
    lv_wl_egl_window_t * window = (lv_wl_egl_window_t *)data;
    struct xkb_keymap * keymap;
    struct xkb_state * state;
    char * map_str;

    LV_UNUSED(keyboard);

    if(!window) {
        LV_LOG_ERROR("Window is NULL in keyboard_handle_keymap");
        close(fd);
        return;
    }

    if(!window->xkb_context) {
        LV_LOG_ERROR("XKB context is NULL");
        close(fd);
        return;
    }

    if(format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        LV_LOG_ERROR("Unsupported keymap format: %u (expected %u)",
                     format, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);
        close(fd);
        return;
    }

    map_str = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(map_str == MAP_FAILED) {
        LV_LOG_ERROR("Failed to mmap keymap");
        close(fd);
        return;
    }

    /* Set up XKB keymap */
    keymap = xkb_keymap_new_from_string(window->xkb_context, map_str,
                                        XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
    munmap(map_str, size);
    close(fd);

    if(!keymap) {
        LV_LOG_ERROR("Failed to compile XKB keymap");
        return;
    }

    /* Set up XKB state */
    state = xkb_state_new(keymap);
    if(!state) {
        LV_LOG_ERROR("Failed to create XKB state");
        xkb_keymap_unref(keymap);
        return;
    }

    /* Clean up old keymap and state */
    if(window->indev.xkb.state) {
        xkb_state_unref(window->indev.xkb.state);
    }
    if(window->indev.xkb.keymap) {
        xkb_keymap_unref(window->indev.xkb.keymap);
    }

    window->indev.xkb.keymap = keymap;
    window->indev.xkb.state  = state;
}

static void keyboard_handle_enter(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                  struct wl_surface * surface, struct wl_array * keys)
{
    lv_wl_egl_window_t * window = (lv_wl_egl_window_t *)data;

    LV_UNUSED(keyboard);
    LV_UNUSED(serial);
    LV_UNUSED(surface);
    LV_UNUSED(keys);

    if(!window) {
        LV_LOG_ERROR("Window is NULL in keyboard_handle_enter");
        return;
    }

    if(!window->indev.xkb.keymap || !window->indev.xkb.state) {
        if(!window->xkb_context) {
            LV_LOG_ERROR("XKB context is NULL, cannot create default keymap");
            return;
        }

        struct xkb_rule_names names = {
            .rules = NULL,
            .model = NULL,
            .layout = "us",
            .variant = NULL,
            .options = NULL
        };

        struct xkb_keymap * keymap = xkb_keymap_new_from_names(window->xkb_context, &names,
                                                               XKB_KEYMAP_COMPILE_NO_FLAGS);

        if(!keymap) {
            LV_LOG_ERROR("Failed to create default keymap");
            return;
        }

        struct xkb_state * state = xkb_state_new(keymap);
        if(!state) {
            LV_LOG_ERROR("Failed to create XKB state from default keymap");
            xkb_keymap_unref(keymap);
            return;
        }

        if(window->indev.xkb.state) {
            xkb_state_unref(window->indev.xkb.state);
        }
        if(window->indev.xkb.keymap) {
            xkb_keymap_unref(window->indev.xkb.keymap);
        }

        window->indev.xkb.keymap = keymap;
        window->indev.xkb.state = state;
    }
}

static void keyboard_handle_leave(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                  struct wl_surface * surface)
{
    lv_wl_egl_window_t * window = (lv_wl_egl_window_t *)data;

    LV_UNUSED(keyboard);
    LV_UNUSED(serial);
    LV_UNUSED(surface);

    if(!window) {
        return;
    }

    window->indev.keyboard.key = 0;
    window->indev.keyboard.state = LV_INDEV_STATE_RELEASED;
}

static void keyboard_handle_key(void * data, struct wl_keyboard * keyboard, uint32_t serial, uint32_t time,
                                uint32_t key, uint32_t state)
{
    lv_wl_egl_window_t * window = (lv_wl_egl_window_t *)data;
    const uint32_t code = (key + 8);
    const xkb_keysym_t * syms;
    xkb_keysym_t sym = XKB_KEY_NoSymbol;
    int nsyms;

    LV_UNUSED(keyboard);
    LV_UNUSED(serial);
    LV_UNUSED(time);

    if(!window) {
        LV_LOG_ERROR("Window is NULL in keyboard_handle_key");
        return;
    }

    if(!window->indev.xkb.state) {
        LV_LOG_ERROR("XKB state not initialized, cannot process key");
        return;
    }

    nsyms = xkb_state_key_get_syms(window->indev.xkb.state, code, &syms);

    if(nsyms >= 1) {
        sym = syms[0];
    }
    else {
        LV_LOG_WARN("No symbol for keycode %u", code);
    }

    const lv_key_t lv_key = keycode_xkb_to_lv(sym);
    const lv_indev_state_t lv_state =
        (state == WL_KEYBOARD_KEY_STATE_PRESSED) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;

    if(lv_key != 0) {
        window->indev.keyboard.key   = lv_key;
        window->indev.keyboard.state = lv_state;
    }
}

static void keyboard_handle_modifiers(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                      uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
                                      uint32_t group)
{
    lv_wl_egl_window_t * window = (lv_wl_egl_window_t *)data;

    LV_UNUSED(keyboard);
    LV_UNUSED(serial);

    if(!window) {
        return;
    }

    if(!window->indev.xkb.keymap || !window->indev.xkb.state) {
        return;
    }

    xkb_state_update_mask(window->indev.xkb.state, mods_depressed, mods_latched,
                          mods_locked, 0, 0, group);
}

static lv_key_t keycode_xkb_to_lv(xkb_keysym_t xkb_key)
{
    lv_key_t key = 0;

    if(((xkb_key >= XKB_KEY_space) && (xkb_key <= XKB_KEY_asciitilde))) {
        key = xkb_key;
    }
    else if(((xkb_key >= XKB_KEY_KP_0) && (xkb_key <= XKB_KEY_KP_9))) {
        key = (xkb_key & 0x003f);
    }
    else {
        switch(xkb_key) {
            case XKB_KEY_BackSpace:
                key = LV_KEY_BACKSPACE;
                break;
            case XKB_KEY_Return:
            case XKB_KEY_KP_Enter:
                key = LV_KEY_ENTER;
                break;
            case XKB_KEY_Escape:
                key = LV_KEY_ESC;
                break;
            case XKB_KEY_Delete:
            case XKB_KEY_KP_Delete:
                key = LV_KEY_DEL;
                break;
            case XKB_KEY_Home:
            case XKB_KEY_KP_Home:
                key = LV_KEY_HOME;
                break;
            case XKB_KEY_Left:
            case XKB_KEY_KP_Left:
                key = LV_KEY_LEFT;
                break;
            case XKB_KEY_Up:
            case XKB_KEY_KP_Up:
                key = LV_KEY_UP;
                break;
            case XKB_KEY_Right:
            case XKB_KEY_KP_Right:
                key = LV_KEY_RIGHT;
                break;
            case XKB_KEY_Down:
            case XKB_KEY_KP_Down:
                key = LV_KEY_DOWN;
                break;
            case XKB_KEY_Prior:
            case XKB_KEY_KP_Prior:
                key = LV_KEY_PREV;
                break;
            case XKB_KEY_Next:
            case XKB_KEY_KP_Next:
            case XKB_KEY_Tab:
            case XKB_KEY_KP_Tab:
                key = LV_KEY_NEXT;
                break;
            case XKB_KEY_End:
            case XKB_KEY_KP_End:
                key = LV_KEY_END;
                break;
            default:
                break;
        }
    }
    return key;
}

/**********************
 *      Pointer
 **********************/

static lv_indev_t * lv_wl_egl_pointer_create(void)
{
    lv_indev_t * pointer = lv_indev_create();
    lv_indev_set_type(pointer, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(pointer, pointer_read);
    return pointer;
}

static lv_indev_t * lv_wl_egl_pointeraxis_create(void)
{
    lv_indev_t * pointeraxis = lv_indev_create();
    lv_indev_set_type(pointeraxis, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(pointeraxis, pointeraxis_read);
    return pointeraxis;
}

static void pointeraxis_read(lv_indev_t * drv, lv_indev_data_t * data)
{
    lv_wl_egl_window_t * window = lv_display_get_user_data(lv_indev_get_display(drv));

    if(!window) {
        return;
    }

    data->state    = window->indev.pointer.wheel_button;
    data->enc_diff = window->indev.pointer.wheel_diff;

    window->indev.pointer.wheel_diff = 0;
}

static void pointer_read(lv_indev_t * drv, lv_indev_data_t * data)
{
    lv_wl_egl_window_t * window = lv_display_get_user_data(lv_indev_get_display(drv));

    if(!window) {
        return;
    }

    data->point.x = window->indev.pointer.x;
    data->point.y = window->indev.pointer.y;
    data->state   = window->indev.pointer.left_button;
}

static void pointer_handle_enter(void * data, struct wl_pointer * pointer, uint32_t serial,
                                 struct wl_surface * surface, wl_fixed_t sx, wl_fixed_t sy)
{
    lv_wl_egl_window_t * window = (lv_wl_egl_window_t *)data;
    const char * cursor_name = "left_ptr";

    LV_UNUSED(surface);

    if(!window) {
        LV_LOG_ERROR("Window is NULL in pointer_handle_enter");
        return;
    }

    if(!pointer) {
        LV_LOG_ERROR("Pointer is NULL in pointer_handle_enter");
        return;
    }

    window->indev.pointer.x = wl_fixed_to_int(sx);
    window->indev.pointer.y = wl_fixed_to_int(sy);

    if(window->cursor_theme && window->cursor_surface) {
        struct wl_cursor * cursor = wl_cursor_theme_get_cursor(window->cursor_theme, cursor_name);

        if(!cursor) {
            cursor = wl_cursor_theme_get_cursor(window->cursor_theme, "default");
        }

        if(cursor && cursor->image_count > 0) {
            struct wl_cursor_image * image = cursor->images[0];
            struct wl_buffer * buffer = wl_cursor_image_get_buffer(image);

            if(buffer) {
                wl_pointer_set_cursor(pointer, serial, window->cursor_surface,
                                      image->hotspot_x, image->hotspot_y);
                wl_surface_attach(window->cursor_surface, buffer, 0, 0);
                wl_surface_damage(window->cursor_surface, 0, 0,
                                  image->width, image->height);
                wl_surface_commit(window->cursor_surface);
                window->cursor_flush_pending = true;
            }
            else {
                LV_LOG_ERROR("Failed to get cursor buffer");
            }
        }
        else {
            LV_LOG_ERROR("No cursor images available");
        }
    }
    else {
        wl_pointer_set_cursor(pointer, serial, NULL, 0, 0);
    }
}

static void pointer_handle_leave(void * data, struct wl_pointer * pointer, uint32_t serial, struct wl_surface * surface)
{
    lv_wl_egl_window_t * window = data;

    LV_UNUSED(pointer);
    LV_UNUSED(serial);
    LV_UNUSED(surface);

    window->indev.pointer.left_button = LV_INDEV_STATE_RELEASED;
    window->indev.pointer.right_button = LV_INDEV_STATE_RELEASED;
    window->indev.pointer.wheel_button = LV_INDEV_STATE_RELEASED;
}

static void pointer_handle_motion(void * data, struct wl_pointer * pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    lv_wl_egl_window_t * window = data;

    LV_UNUSED(pointer);
    LV_UNUSED(time);

    window->indev.pointer.x = wl_fixed_to_int(sx);
    window->indev.pointer.y = wl_fixed_to_int(sy);
}

static void pointer_handle_button(void * data, struct wl_pointer * wl_pointer, uint32_t serial, uint32_t time,
                                  uint32_t button, uint32_t state)
{
    lv_wl_egl_window_t * window = data;

    LV_UNUSED(serial);
    LV_UNUSED(wl_pointer);
    LV_UNUSED(time);

    const lv_indev_state_t lv_state =
        (state == WL_POINTER_BUTTON_STATE_PRESSED) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;


    switch(button) {
        case BTN_LEFT:
            window->indev.pointer.left_button = lv_state;
            break;
        case BTN_RIGHT:
            window->indev.pointer.right_button = lv_state;
            break;
        case BTN_MIDDLE:
            window->indev.pointer.wheel_button = lv_state;
            break;
        default:
            break;
    }
}

static void pointer_handle_axis(void * data, struct wl_pointer * wl_pointer, uint32_t time, uint32_t axis,
                                wl_fixed_t value)
{
    lv_wl_egl_window_t * window = data;
    const int diff           = wl_fixed_to_int(value);

    LV_UNUSED(time);
    LV_UNUSED(wl_pointer);

    if(axis == 0) {
        if(diff > 0) {
            window->indev.pointer.wheel_diff++;
        }
        else if(diff < 0) {
            window->indev.pointer.wheel_diff--;
        }
    }
}

/**********************
 *      Touch
 **********************/

#if LV_USE_GESTURE_RECOGNITION
static lv_indev_t * lv_wl_egl_touch_create(void)
{
    lv_indev_t * touch = lv_indev_create();
    lv_indev_set_type(touch, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(touch, touch_read);
    return touch;
}

static void touch_read(lv_indev_t * drv, lv_indev_data_t * data)
{

    lv_wl_egl_window_t * window = lv_display_get_user_data(lv_indev_get_display(drv));

    if(!window) {
        return;
    }

    lv_indev_gesture_recognizers_update(drv, &window->indev.touches[0], window->indev.touch_event_cnt);

    LV_LOG_TRACE("collected touch events: %d", window->indev.touch_event_cnt);

    window->indev.touch_event_cnt = 0;

    lv_indev_gesture_recognizers_set_data(drv, data);
}

static void touch_handle_down(void * data, struct wl_touch * wl_touch, uint32_t serial, uint32_t time,
                              struct wl_surface * surface, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
    lv_wl_egl_window_t * window = data;
    uint8_t i;

    LV_UNUSED(id);
    LV_UNUSED(time);
    LV_UNUSED(serial);
    LV_UNUSED(wl_touch);

    if(id > 10) {
        LV_LOG_WARN("Touch ID exceeds maximum supported touches");
        return;
    }

    i = window->indev.touch_event_cnt;

    window->indev.touches[i].point.x   = wl_fixed_to_int(x_w);
    window->indev.touches[i].point.y   = wl_fixed_to_int(y_w);
    window->indev.touches[i].id        = id;
    window->indev.touches[i].timestamp = time;
    window->indev.touches[i].state     = LV_INDEV_STATE_PRESSED;
    window->indev.touch_event_cnt++;
}

static void touch_handle_up(void * data, struct wl_touch * wl_touch, uint32_t serial, uint32_t time, int32_t id)
{
    lv_wl_egl_window_t * window = data;
    uint8_t i;

    LV_UNUSED(serial);
    LV_UNUSED(time);
    LV_UNUSED(id);
    LV_UNUSED(wl_touch);

    i = window->indev.touch_event_cnt;

    window->indev.touches[i].point.x   = 0;
    window->indev.touches[i].point.y   = 0;
    window->indev.touches[i].id        = id;
    window->indev.touches[i].timestamp = time;
    window->indev.touches[i].state     = LV_INDEV_STATE_RELEASED;

    window->indev.touch_event_cnt++;
}

static void touch_handle_motion(void * data, struct wl_touch * wl_touch, uint32_t time, int32_t id, wl_fixed_t x_w,
                                wl_fixed_t y_w)
{
    lv_wl_egl_window_t * window = data;
    lv_indev_touch_data_t * touch;
    lv_indev_touch_data_t * cur;
    uint8_t i;

    LV_UNUSED(time);
    LV_UNUSED(id);
    LV_UNUSED(wl_touch);

    /* Update the contact point of the corresponding id with the latest coordinate */
    touch = &window->indev.touches[0];
    cur   = NULL;

    for(i = 0; i < window->indev.touch_event_cnt; i++) {
        if(touch->id == id) {
            cur = touch;
        }
        touch++;
    }

    if(cur == NULL) {

        i = window->indev.touch_event_cnt;
        window->indev.touches[i].point.x   = wl_fixed_to_int(x_w);
        window->indev.touches[i].point.y   = wl_fixed_to_int(y_w);
        window->indev.touches[i].id        = id;
        window->indev.touches[i].timestamp = time;
        window->indev.touches[i].state     = LV_INDEV_STATE_PRESSED;
        window->indev.touch_event_cnt++;

    }
    else {

        cur->point.x   = wl_fixed_to_int(x_w);
        cur->point.y   = wl_fixed_to_int(y_w);
        cur->id        = id;
        cur->timestamp = time;
    }
}

static void touch_handle_frame(void * data, struct wl_touch * wl_touch)
{
    LV_UNUSED(wl_touch);
    LV_UNUSED(data);
}

static void touch_handle_cancel(void * data, struct wl_touch * wl_touch)
{
    LV_UNUSED(wl_touch);
    LV_UNUSED(data);
}
#endif

#endif /* LV_USE_WAYLAND && LV_WAYLAND_USE_EGL */
