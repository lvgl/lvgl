/**
 * @file lv_bar.h
 *
 */

#ifndef LV_BAR_H
#define LV_BAR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_BAR != 0

#include "../lv_core/lv_obj.h"
#include "../lv_misc/lv_anim.h"
#include "lv_btn.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_BAR_TYPE_NORMAL,
    LV_BAR_TYPE_SYMMETRICAL,
    LV_BAR_TYPE_RANGE
};
typedef uint8_t lv_bar_type_t;

typedef struct {
    lv_obj_t * bar;
    lv_anim_value_t anim_start;
    lv_anim_value_t anim_end;
    lv_anim_value_t anim_state;
} lv_bar_anim_t;

typedef struct {
    lv_obj_t obj;
    int16_t cur_value;          /**< Current value of the bar*/
    int16_t min_value;          /**< Minimum value of the bar*/
    int16_t max_value;          /**< Maximum value of the bar*/
    int16_t start_value;        /**< Start value of the bar*/
    lv_area_t indic_area;       /**< Save the indicator area. Might be used by derived types*/
    lv_bar_anim_t cur_value_anim;
    lv_bar_anim_t start_value_anim;
    lv_bar_type_t type : 2;     /**< Type of bar*/
}lv_bar_t;

extern const lv_obj_class_t lv_bar_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a bar objects
 * @param parent    pointer to an object, it will be the parent of the new bar
 * @param copy      DEPRECATED, will be removed in v9.
 *                  Pointer to an other bar to copy.
 * @return          pointer to the created bar
 */
lv_obj_t * lv_bar_create(lv_obj_t * parent, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the bar
 * @param bar       pointer to a bar object
 * @param value     new value
 * @param anim      LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_bar_set_value(lv_obj_t * obj, int16_t value, lv_anim_enable_t anim);

/**
 * Set a new start value on the bar
 * @param obj       pointer to a bar object
 * @param value     new start value
 * @param anim      LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_bar_set_start_value(lv_obj_t * obj, int16_t start_value, lv_anim_enable_t anim);

/**
 * Set minimum and the maximum values of a bar
 * @param obj       pointer to the bar object
 * @param min       minimum value
 * @param max       maximum value
 */
void lv_bar_set_range(lv_obj_t * obj, int16_t min, int16_t max);

/**
 * Set the type of bar.
 * @param obj       pointer to bar object
 * @param type      bar type from ::lv_bar_type_t
 */
void lv_bar_set_type(lv_obj_t * obj, lv_bar_type_t type);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a bar
 * @param obj       pointer to a bar object
 * @return          the value of the bar
 */
int16_t lv_bar_get_value(const lv_obj_t * obj);

/**
 * Get the start value of a bar
 * @param obj       pointer to a bar object
 * @return          the start value of the bar
 */
int16_t lv_bar_get_start_value(const lv_obj_t * obj);

/**
 * Get the minimum value of a bar
 * @param obj       pointer to a bar object
 * @return          the minimum value of the bar
 */
int16_t lv_bar_get_min_value(const lv_obj_t * obj);

/**
 * Get the maximum value of a bar
 * @param obj       pointer to a bar object
 * @return          the maximum value of the bar
 */
int16_t lv_bar_get_max_value(const lv_obj_t * obj);

/**
 * Get the type of bar.
 * @param obj       pointer to bar object
 * @return          bar type from ::lv_bar_type_t
 */
lv_bar_type_t lv_bar_get_type(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BAR*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BAR_H*/
