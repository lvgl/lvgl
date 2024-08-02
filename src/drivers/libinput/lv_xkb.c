/**
 * @file lv_xkb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_xkb_private.h"

#if defined(LV_LIBINPUT_XKB) && LV_LIBINPUT_XKB

#include "../../core/lv_group.h"
#include "../../misc/lv_log.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool _set_keymap(lv_xkb_t * dsc, struct xkb_rule_names names);

/**********************
 *  STATIC VARIABLES
 **********************/

static struct xkb_context * context = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_xkb_init(lv_xkb_t * dsc, struct xkb_rule_names names)
{
    if(!context) {
        context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
        if(!context) {
            LV_LOG_ERROR("xkb_context_new failed: %s", strerror(errno));
            return false;
        }
    }

    return _set_keymap(dsc, names);
}

void lv_xkb_deinit(lv_xkb_t * dsc)
{
    if(dsc->state) {
        xkb_state_unref(dsc->state);
        dsc->state = NULL;
    }

    if(dsc->keymap) {
        xkb_keymap_unref(dsc->keymap);
        dsc->keymap = NULL;
    }
}

uint32_t lv_xkb_process_key(lv_xkb_t * dsc, uint32_t scancode, bool down)
{
    /* Offset the evdev scancode by 8, see https://xkbcommon.org/doc/current/xkbcommon_8h.html#ac29aee92124c08d1953910ab28ee1997 */
    xkb_keycode_t keycode = scancode + 8;

    uint32_t result = 0;

    switch(xkb_state_key_get_one_sym(dsc->state, keycode)) {
        case XKB_KEY_BackSpace:
            result = LV_KEY_BACKSPACE;
            break;
        case XKB_KEY_Return:
        case XKB_KEY_KP_Enter:
            result = LV_KEY_ENTER;
            break;
        case XKB_KEY_Prior:
        case XKB_KEY_KP_Prior:
            result = LV_KEY_PREV;
            break;
        case XKB_KEY_Next:
        case XKB_KEY_KP_Next:
            result = LV_KEY_NEXT;
            break;
        case XKB_KEY_Up:
        case XKB_KEY_KP_Up:
            result = LV_KEY_UP;
            break;
        case XKB_KEY_Left:
        case XKB_KEY_KP_Left:
            result = LV_KEY_LEFT;
            break;
        case XKB_KEY_Right:
        case XKB_KEY_KP_Right:
            result = LV_KEY_RIGHT;
            break;
        case XKB_KEY_Down:
        case XKB_KEY_KP_Down:
            result = LV_KEY_DOWN;
            break;
        case XKB_KEY_Tab:
        case XKB_KEY_KP_Tab:
            result = LV_KEY_NEXT;
            break;
        case XKB_KEY_ISO_Left_Tab: /* Sent on SHIFT + TAB */
            result = LV_KEY_PREV;
            break;
        case XKB_KEY_Home:
        case XKB_KEY_KP_Home:
            result = LV_KEY_HOME;
            break;
        case XKB_KEY_End:
        case XKB_KEY_KP_End:
            result = LV_KEY_END;
            break;
        default:
            break;
    }

    if(result == 0) {
        char buffer[4] = { 0, 0, 0, 0 };
        int size = xkb_state_key_get_utf8(dsc->state, keycode, NULL, 0) + 1;
        if(size > 1) {
            xkb_state_key_get_utf8(dsc->state, keycode, buffer, size);
            memcpy(&result, buffer, 4);
        }
    }

    xkb_state_update_key(dsc->state, keycode, down ? XKB_KEY_DOWN : XKB_KEY_UP);

    return result;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool _set_keymap(lv_xkb_t * dsc, struct xkb_rule_names names)
{
    if(dsc->keymap) {
        xkb_keymap_unref(dsc->keymap);
        dsc->keymap = NULL;
    }

    dsc->keymap = xkb_keymap_new_from_names(context, &names, XKB_KEYMAP_COMPILE_NO_FLAGS);
    if(!dsc->keymap) {
        LV_LOG_ERROR("xkb_keymap_new_from_names failed: %s", strerror(errno));
        return false;
    }

    if(dsc->state) {
        xkb_state_unref(dsc->state);
        dsc->state = NULL;
    }

    dsc->state = xkb_state_new(dsc->keymap);
    if(!dsc->state) {
        LV_LOG_ERROR("xkb_state_new failed: %s", strerror(errno));
        return false;
    }

    return true;
}

#endif /* defined(LV_LIBINPUT_XKB) && LV_LIBINPUT_XKB */
