/**
 * @file lv_spinner.h
 *
 */

#ifndef LV_SPINNER_H
#define LV_SPINNER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_SPINNER

#include "../../misc/lv_types.h"
#include "../../core/lv_obj_property.h"

/*Testing of dependencies*/
#if LV_USE_ARC == 0
#error "lv_spinner: lv_arc is required. Enable it in lv_conf.h (LV_USE_ARC  1) "
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_spinner_class;

#if LV_USE_OBJ_PROPERTY
enum _lv_property_spinner_id_t {
    LV_PROPERTY_ID(SPINNER, ANIM_DURATION, LV_PROPERTY_TYPE_INT, 0),
    LV_PROPERTY_ID(SPINNER, ARC_SWEEP,     LV_PROPERTY_TYPE_INT, 1),
    LV_PROPERTY_SPINNER_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a spinner widget
 * @param parent    pointer to an object, it will be the parent of the new spinner.
 * @return          the created spinner
 */
lv_obj_t * lv_spinner_create(lv_obj_t * parent);

/**
 * Set the animation time and arc length of the spinner
 * The animation is suited for angle values between 180 and 360.
 * @param obj       pointer to a spinner
 * @param t         the animation time in milliseconds
 * @param angle     the angle of the arc in degrees
 */
void lv_spinner_set_anim_params(lv_obj_t * obj, uint32_t t, uint32_t angle);

/**
 * Set the animation time of the spinner
 * @param obj       pointer to a spinner
 * @param t         the animation time in milliseconds
 */
void lv_spinner_set_anim_duration(lv_obj_t * obj, uint32_t t);

/**
 * Set the animation arc length of the spinner.
 * The animation is suited to values between 180 and 360.
 * @param obj       pointer to a spinner
 * @param angle     the angle of the arc in degrees
 */
void lv_spinner_set_arc_sweep(lv_obj_t * obj, uint32_t angle);

/**
 * Get the animation duration of the spinner
 * @param obj       pointer to a spinner
 * @return          the animation time in milliseconds
 */
uint32_t lv_spinner_get_anim_duration(lv_obj_t * obj);

/**
 * Get the animation arc length of the spinner
 * @param obj       pointer to a spinner
 * @return          the angle of the arc in degrees
 */
uint32_t lv_spinner_get_arc_sweep(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SPINNER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SPINNER_H*/
