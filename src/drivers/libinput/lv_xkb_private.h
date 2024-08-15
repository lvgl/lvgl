/**
 * @file lv_xkb_private.h
 *
 */

#ifndef LV_XKB_PRIVATE_H
#define LV_XKB_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_xkb.h"

#if defined(LV_LIBINPUT_XKB) && LV_LIBINPUT_XKB

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_xkb_t {
    struct xkb_keymap * keymap;
    struct xkb_state * state;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* defined(LV_LIBINPUT_XKB) && LV_LIBINPUT_XKB */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XKB_PRIVATE_H*/
