/**
 * @file lv_switch_private.h
 *
 */

#ifndef LV_SWITCH_PRIVATE_H
#define LV_SWITCH_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_switch.h"

#if LV_USE_SWITCH != 0
#include "../../core/lv_obj_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_switch_t {
    lv_obj_t obj;
    int32_t anim_state;
    lv_switch_orientation_t orientation : 3; /**< Orientation of switch*/
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_SWITCH != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SWITCH_PRIVATE_H*/
