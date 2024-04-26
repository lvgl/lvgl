/**
 * @file lv_slider.h
 *
 */

#ifndef LV_SLIDER_H
#define LV_SLIDER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../bar/lv_bar.h"

#if LV_USE_SLIDER != 0

/*Testing of dependencies*/
#if LV_USE_BAR == 0
#error "lv_slider: lv_bar is required. Enable it in lv_conf.h (LV_USE_BAR 1)"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_SLIDER_MODE_NORMAL = LV_BAR_MODE_NORMAL,
    LV_SLIDER_MODE_SYMMETRICAL = LV_BAR_MODE_SYMMETRICAL,
    LV_SLIDER_MODE_RANGE = LV_BAR_MODE_RANGE
} lv_slider_mode_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_slider_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a slider object
 * @param parent    pointer to an object, it will be the parent of the new slider.
 * @return          pointer to the created slider
 */
lv_obj_t * lv_slider_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the slider
 * @param obj       pointer to a slider object
 * @param value     the new value
 * @param anim      LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_slider_set_value(lv_obj_t * obj, int32_t value, lv_anim_enable_t anim);

/**
 * Set a new value for the left knob of a slider
 * @param obj       pointer to a slider object
 * @param value     new value
 * @param anim      LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_slider_set_left_value(lv_obj_t * obj, int32_t value, lv_anim_enable_t anim);

/**
 * Set minimum and the maximum values of a bar
 * @param obj       pointer to the slider object
 * @param min       minimum value
 * @param max       maximum value
 */
void lv_slider_set_range(lv_obj_t * obj, int32_t min, int32_t max);

/**
 * Set the mode of slider.
 * @param obj       pointer to a slider object
 * @param mode      the mode of the slider. See ::lv_slider_mode_t
 */
void lv_slider_set_mode(lv_obj_t * obj, lv_slider_mode_t mode);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of the main knob of a slider
 * @param obj       pointer to a slider object
 * @return          the value of the main knob of the slider
 */
int32_t lv_slider_get_value(const lv_obj_t * obj);

/**
 * Get the value of the left knob of a slider
 * @param obj       pointer to a slider object
 * @return          the value of the left knob of the slider
 */
int32_t lv_slider_get_left_value(const lv_obj_t * obj);

/**
 * Get the minimum value of a slider
 * @param obj       pointer to a slider object
 * @return          the minimum value of the slider
 */
int32_t lv_slider_get_min_value(const lv_obj_t * obj);

/**
 * Get the maximum value of a slider
 * @param obj       pointer to a slider object
 * @return          the maximum value of the slider
 */
int32_t lv_slider_get_max_value(const lv_obj_t * obj);

/**
 * Give the slider is being dragged or not
 * @param obj       pointer to a slider object
 * @return          true: drag in progress false: not dragged
 */
bool lv_slider_is_dragged(const lv_obj_t * obj);

/**
 * Get the mode of the slider.
 * @param slider       pointer to a slider object
 * @return          see ::lv_slider_mode_t
 */
lv_slider_mode_t lv_slider_get_mode(lv_obj_t * slider);

/**
 * Give the slider is in symmetrical mode or not
 * @param obj       pointer to slider object
 * @return          true: in symmetrical mode false : not in
*/
bool lv_slider_is_symmetrical(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SLIDER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SLIDER_H*/
