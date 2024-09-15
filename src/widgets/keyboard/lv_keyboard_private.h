/**
 * @file lv_keyboard_private.h
 *
 */

#ifndef LV_KEYBOARD_PRIVATE_H
#define LV_KEYBOARD_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../buttonmatrix/lv_buttonmatrix_private.h"
#include "lv_keyboard.h"

#if LV_USE_KEYBOARD

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Data of keyboard */
struct _lv_keyboard_t {
    lv_buttonmatrix_t btnm;
    lv_obj_t * ta;              /**< Pointer to the assigned text area */
    lv_keyboard_mode_t mode;    /**< Key map type */
    uint8_t popovers : 1;       /**< Show button titles in popovers on press */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_KEYBOARD */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_KEYBOARD_PRIVATE_H*/
