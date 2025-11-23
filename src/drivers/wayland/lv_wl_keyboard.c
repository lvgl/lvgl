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

static struct xkb_context * xkb_context;

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

    lv_indev_t * indev = lv_indev_create();
    if(!indev) {
        return NULL;
    }
    lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(indev, keyboard_read);
    lv_indev_set_driver_data(indev, lv_wl_ctx.seat.keyboard);

    return indev;
}

lv_indev_t * lv_wayland_get_keyboard(lv_display_t * display)
{
    lv_wl_window_t * window = lv_display_get_driver_data(display);
    if(!window) {
        return NULL;
    }
    return window->lv_indev_keyboard;
}

/**********************
 *   PRIVATE FUNCTIONS
 **********************/

lv_wl_seat_keyboard_t * lv_wayland_seat_keyboard_create(struct wl_seat * wl_seat)
{

    struct wl_keyboard * keyboard = wl_seat_get_keyboard(wl_seat);
    if(!keyboard) {
        LV_LOG_WARN("Failed to get seat keyboard");
        return NULL;
    }
    if(!xkb_context && !(xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS))) {
        LV_LOG_WARN("Failed to create xkb context");
        return NULL;
    }

    lv_wl_seat_keyboard_t * wl_seat_keyboard = lv_zalloc(sizeof(*wl_seat_keyboard));
    LV_ASSERT_MALLOC(wl_seat_keyboard);
    if(!wl_seat_keyboard) {
        LV_LOG_WARN("Failed to allocate memory for wayland keyboard");
        return NULL;
    }
    wl_keyboard_add_listener(keyboard, &keyboard_listener, NULL);
    wl_keyboard_set_user_data(keyboard, wl_seat_keyboard);

    wl_seat_keyboard->wl_keyboard = keyboard;
    lv_wayland_update_indevs(keyboard_read, wl_seat_keyboard);

    return wl_seat_keyboard;
}
void lv_wayland_seat_keyboard_delete(lv_wl_seat_keyboard_t * seat_keyboard)
{
    lv_wayland_update_indevs(keyboard_read, NULL);
    lv_free(seat_keyboard);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void keyboard_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_wl_seat_keyboard_t * kbdata = lv_indev_get_driver_data(indev);
    if(!kbdata) {
        return;
    }
    data->key = kbdata->key;
    data->state = kbdata->state;
}

static void keyboard_handle_keymap(void * data, struct wl_keyboard * keyboard, uint32_t format, int fd, uint32_t size)
{
    LV_UNUSED(data);
    lv_wl_seat_keyboard_t * kbdata = wl_keyboard_get_user_data(keyboard);

    if(format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        LV_LOG_WARN("Can't handle formats other than WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1");
        close(fd);
        return;
    }

    char * map_str = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(map_str == MAP_FAILED) {
        LV_LOG_WARN("Failed to mmap keyboard keymap file");
        close(fd);
        return;
    }

    /* Set up XKB keymap */
    struct xkb_keymap * keymap = xkb_keymap_new_from_string(xkb_context, map_str, XKB_KEYMAP_FORMAT_TEXT_V1, 0);
    munmap(map_str, size);
    close(fd);

    if(!keymap) {
        LV_LOG_WARN("Failed to compile keymap");
        return;
    }

    /* Set up XKB state */
    struct xkb_state * state = xkb_state_new(keymap);
    if(!state) {
        LV_LOG_WARN("Failed to create XKB state");
        xkb_keymap_unref(keymap);
        return;
    }

    xkb_keymap_unref(kbdata->xkb_keymap);
    xkb_state_unref(kbdata->xkb_state);

    kbdata->xkb_keymap = keymap;
    kbdata->xkb_state = state;
}

static void keyboard_handle_enter(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                  struct wl_surface * surface, struct wl_array * keys)
{

    LV_UNUSED(data);
    LV_UNUSED(keyboard);
    LV_UNUSED(serial);
    LV_UNUSED(keys);
    LV_UNUSED(surface);
}

static void keyboard_handle_leave(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                  struct wl_surface * surface)
{
    LV_UNUSED(serial);
    LV_UNUSED(keyboard);
    LV_UNUSED(data);
    LV_UNUSED(surface);
}

static void keyboard_handle_key(void * data, struct wl_keyboard * keyboard, uint32_t serial, uint32_t time,
                                uint32_t key, uint32_t state)
{
    LV_UNUSED(data);
    LV_UNUSED(serial);
    LV_UNUSED(time);

    lv_wl_seat_keyboard_t * kbdata = wl_keyboard_get_user_data(keyboard);
    const uint32_t code = (key + 8);

    if(!kbdata->xkb_state) {
        return;
    }

    const xkb_keysym_t * syms = XKB_KEY_NoSymbol;
    if(xkb_state_key_get_syms(kbdata->xkb_state, code, &syms) != 1) {
        return;
    }

    const lv_key_t lv_key = keycode_xkb_to_lv(syms[0]);
    const lv_indev_state_t lv_state =
        (state == WL_KEYBOARD_KEY_STATE_PRESSED) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;

    if(lv_key != 0) {
        kbdata->key = lv_key;
        kbdata->state = lv_state;
    }
}

static void keyboard_handle_modifiers(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                      uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
                                      uint32_t group)
{
    LV_UNUSED(serial);
    LV_UNUSED(data);
    lv_wl_seat_keyboard_t * kbdata = wl_keyboard_get_user_data(keyboard);

    /* If we're not using a keymap, then we don't handle PC-style modifiers */
    if(!kbdata->xkb_keymap) {
        return;
    }

    xkb_state_update_mask(kbdata->xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
}
static lv_key_t keycode_xkb_to_lv(xkb_keysym_t xkb_key)
{

    if(xkb_key >= XKB_KEY_space && xkb_key <= XKB_KEY_asciitilde) {
        return xkb_key;
    }
    if(xkb_key >= XKB_KEY_KP_0 && xkb_key <= XKB_KEY_KP_9) {
        return (xkb_key & 0x003f);
    }
    switch(xkb_key) {
        case XKB_KEY_BackSpace:
            return LV_KEY_BACKSPACE;
            break;
        case XKB_KEY_Return:
        case XKB_KEY_KP_Enter:
            return LV_KEY_ENTER;
            break;
        case XKB_KEY_Escape:
            return LV_KEY_ESC;
            break;
        case XKB_KEY_Delete:
        case XKB_KEY_KP_Delete:
            return LV_KEY_DEL;
            break;
        case XKB_KEY_Home:
        case XKB_KEY_KP_Home:
            return LV_KEY_HOME;
            break;
        case XKB_KEY_Left:
        case XKB_KEY_KP_Left:
            return LV_KEY_LEFT;
            break;
        case XKB_KEY_Up:
        case XKB_KEY_KP_Up:
            return LV_KEY_UP;
            break;
        case XKB_KEY_Right:
        case XKB_KEY_KP_Right:
            return LV_KEY_RIGHT;
            break;
        case XKB_KEY_Down:
        case XKB_KEY_KP_Down:
            return LV_KEY_DOWN;
            break;
        case XKB_KEY_Prior:
        case XKB_KEY_KP_Prior:
            return LV_KEY_PREV;
            break;
        case XKB_KEY_Next:
        case XKB_KEY_KP_Next:
        case XKB_KEY_Tab:
        case XKB_KEY_KP_Tab:
            return LV_KEY_NEXT;
            break;
        case XKB_KEY_End:
        case XKB_KEY_KP_End:
            return LV_KEY_END;
            break;
        default:
            return 0;
    }
}

#endif /* LV_WAYLAND */
