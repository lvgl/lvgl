/**
 * @file lv_wl_keyboard.c
 *
 */

#include "lv_wl_keyboard.h"

#if LV_USE_WAYLAND

#include "lv_wayland_private.h"
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <xkbcommon/xkbcommon.h>
#include "../../misc/lv_log.h"

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

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
 *  STATIC VARIABLES
 **********************/

static const struct wl_keyboard_listener keyboard_listener = {
    .keymap    = keyboard_handle_keymap,
    .enter     = keyboard_handle_enter,
    .leave     = keyboard_handle_leave,
    .key       = keyboard_handle_key,
    .modifiers = keyboard_handle_modifiers,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_indev_t * lv_wayland_keyboard_create(void)
{

    lv_indev_t * keyboard = lv_indev_create();
    lv_indev_set_type(keyboard, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(keyboard, keyboard_read);

    return keyboard;
}

lv_indev_t * lv_wayland_get_keyboard(lv_display_t * display)
{
    struct window * window = lv_display_get_driver_data(display);
    if(!window) {
        return NULL;
    }
    return window->lv_indev_keyboard;
}

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

const struct wl_keyboard_listener * lv_wayland_keyboard_get_listener(void)
{
    return &keyboard_listener;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void keyboard_read(lv_indev_t * drv, lv_indev_data_t * data)
{
    struct window * window = lv_display_get_driver_data(lv_indev_get_display(drv));
    if(!window || window->closed) {
        return;
    }

    data->key   = window->body->input.keyboard.key;
    data->state = window->body->input.keyboard.state;
}

static void keyboard_handle_keymap(void * data, struct wl_keyboard * keyboard, uint32_t format, int fd, uint32_t size)
{
    struct lv_wayland_context * app = data;

    struct xkb_keymap * keymap;
    struct xkb_state * state;
    char * map_str;

    LV_UNUSED(keyboard);

    if(format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        close(fd);
        return;
    }

    map_str = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(map_str == MAP_FAILED) {
        close(fd);
        return;
    }

    /* Set up XKB keymap */
    keymap = xkb_keymap_new_from_string(app->xkb_context, map_str, XKB_KEYMAP_FORMAT_TEXT_V1, 0);
    munmap(map_str, size);
    close(fd);

    if(!keymap) {
        LV_LOG_ERROR("failed to compile keymap");
        return;
    }

    /* Set up XKB state */
    state = xkb_state_new(keymap);
    if(!state) {
        LV_LOG_ERROR("failed to create XKB state");
        xkb_keymap_unref(keymap);
        return;
    }

    xkb_keymap_unref(app->seat.xkb.keymap);
    xkb_state_unref(app->seat.xkb.state);
    app->seat.xkb.keymap = keymap;
    app->seat.xkb.state  = state;
}

static void keyboard_handle_enter(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                  struct wl_surface * surface, struct wl_array * keys)
{
    struct lv_wayland_context * app = data;

    LV_UNUSED(keyboard);
    LV_UNUSED(serial);
    LV_UNUSED(keys);

    if(!surface) {
        app->keyboard_obj = NULL;
    }
    else {
        app->keyboard_obj = wl_surface_get_user_data(surface);
    }
}

static void keyboard_handle_leave(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                  struct wl_surface * surface)
{
    struct lv_wayland_context * app = data;

    LV_UNUSED(serial);
    LV_UNUSED(keyboard);

    if(!surface || (app->keyboard_obj == wl_surface_get_user_data(surface))) {
        app->keyboard_obj = NULL;
    }
}

static void keyboard_handle_key(void * data, struct wl_keyboard * keyboard, uint32_t serial, uint32_t time,
                                uint32_t key, uint32_t state)
{
    struct lv_wayland_context * app = data;
    const uint32_t code      = (key + 8);
    const xkb_keysym_t * syms;
    xkb_keysym_t sym = XKB_KEY_NoSymbol;

    LV_UNUSED(serial);
    LV_UNUSED(time);
    LV_UNUSED(keyboard);

    if(!app->keyboard_obj || !app->seat.xkb.state) {
        return;
    }

    if(xkb_state_key_get_syms(app->seat.xkb.state, code, &syms) == 1) {
        sym = syms[0];
    }

    const lv_key_t lv_key = keycode_xkb_to_lv(sym);
    const lv_indev_state_t lv_state =
        (state == WL_KEYBOARD_KEY_STATE_PRESSED) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;

    if(lv_key != 0) {
        app->keyboard_obj->input.keyboard.key   = lv_key;
        app->keyboard_obj->input.keyboard.state = lv_state;
    }
}

static void keyboard_handle_modifiers(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                      uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
                                      uint32_t group)
{
    struct lv_wayland_context * app = data;

    LV_UNUSED(serial);
    LV_UNUSED(keyboard);

    /* If we're not using a keymap, then we don't handle PC-style modifiers */
    if(!app->seat.xkb.keymap) {
        return;
    }

    xkb_state_update_mask(app->seat.xkb.state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
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

#endif /* LV_WAYLAND */
