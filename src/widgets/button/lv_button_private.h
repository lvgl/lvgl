/**
 * @file lv_button_private.h
 *
 */

#ifndef LV_BUTTON_PRIVATE_H
#define LV_BUTTON_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_button.h"

#if LV_USE_BUTTON != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_button_t {
    lv_obj_t obj;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_BUTTON != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BUTTON_PRIVATE_H*/
