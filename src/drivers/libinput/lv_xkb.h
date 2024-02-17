/**
 * @file lv_xkb.h
 *
 */

#ifndef LV_XKB_H
#define LV_XKB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if defined(LV_LIBINPUT_XKB) && LV_LIBINPUT_XKB

#include <stdbool.h>
#include <xkbcommon/xkbcommon.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    struct xkb_keymap * keymap;
    struct xkb_state * state;
} lv_xkb_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialise an XKB descriptor.
 * @return true if the initialisation was successful
 */
bool lv_xkb_init(lv_xkb_t * dsc, struct xkb_rule_names names);

/**
 * De-initialise an XKB descriptor.
 * @param dsc Pointer to descriptor
 */
void lv_xkb_deinit(lv_xkb_t * dsc);

/**
 * Process an evdev scancode using a specific XKB descriptor.
 * @param state XKB descriptor to use
 * @param scancode evdev scancode to process
 * @param down true if the key was pressed, false if it was releases
 * @return the (first) UTF-8 character produced by the event or 0 if no output was produced
 */
uint32_t lv_xkb_process_key(lv_xkb_t * dsc, uint32_t scancode, bool down);

/**********************
 *      MACROS
 **********************/

#endif /* LV_LIBINPUT_XKB */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* defined(LV_LIBINPUT_XKB) && LV_LIBINPUT_XKB */
