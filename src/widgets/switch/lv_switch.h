/**
 * @file lv_switch.h
 *
 */

#ifndef LV_SWITCH_H
#define LV_SWITCH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_SWITCH != 0

#include "../../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/** Switch knob extra area correction factor */
#define _LV_SWITCH_KNOB_EXT_AREA_CORRECTION 2

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t obj;
    int32_t anim_state;
} lv_switch_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_switch_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a switch object
 * @param parent    pointer to an object, it will be the parent of the new switch
 * @return          pointer to the created switch
 */
lv_obj_t * lv_switch_create(lv_obj_t * parent);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SWITCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SWITCH_H*/
