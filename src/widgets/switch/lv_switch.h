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
#define LV_SWITCH_KNOB_EXT_AREA_CORRECTION 2

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_switch_class;

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_SWITCH_ORIENTATION_AUTO,
    LV_SWITCH_ORIENTATION_HORIZONTAL,
    LV_SWITCH_ORIENTATION_VERTICAL
} lv_switch_orientation_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a switch object
 * @param parent    pointer to an object, it will be the parent of the new switch
 * @return          pointer to the created switch
 */
lv_obj_t * lv_switch_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the orientation of switch.
 * @param obj           pointer to switch object
 * @param orientation   switch orientation from `lv_switch_orientation_t`
 */
void lv_switch_set_orientation(lv_obj_t * obj, lv_switch_orientation_t orientation);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the orientation of switch.
 * @param obj       pointer to switch object
 * @return          switch orientation from ::lv_switch_orientation_t
 */
lv_switch_orientation_t lv_switch_get_orientation(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SWITCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SWITCH_H*/
