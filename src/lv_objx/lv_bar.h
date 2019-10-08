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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#if LV_USE_BAR != 0

#include "../lv_core/lv_obj.h"
#include "../lv_misc/lv_anim.h"
#include "lv_cont.h"
#include "lv_btn.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/** Bar animation start value. (Not the real value of the Bar just indicates process animation)*/
#define LV_BAR_ANIM_STATE_START 0

/** Bar animation end value.  (Not the real value of the Bar just indicates process animation)*/
#define LV_BAR_ANIM_STATE_END 256

/** Mark no animation is in progress */
#define LV_BAR_ANIM_STATE_INV -1

/** log2(LV_BAR_ANIM_STATE_END) used to normalize data*/
#define LV_BAR_ANIM_STATE_NORM 8

LV_EXPORT_CONST_INT(LV_BAR_ANIM_STATE_START);
LV_EXPORT_CONST_INT(LV_BAR_ANIM_STATE_END);
LV_EXPORT_CONST_INT(LV_BAR_ANIM_STATE_INV);
LV_EXPORT_CONST_INT(LV_BAR_ANIM_STATE_NORM);

/**********************
 *      TYPEDEFS
 **********************/

/** Data of bar*/
typedef struct
{
    /*No inherited ext, derived from the base object */

    /*New data for this type */
    int16_t cur_value; /*Current value of the bar*/
    int16_t min_value; /*Minimum value of the bar*/
    int16_t max_value; /*Maximum value of the bar*/
#if LV_USE_ANIMATION
    lv_anim_value_t anim_start;
    lv_anim_value_t anim_end;
    lv_anim_value_t anim_state;
    lv_anim_value_t anim_time;
#endif
    uint8_t sym : 1;                /*Symmetric: means the center is around zero value*/
    const lv_style_t * style_indic; /*Style of the indicator*/
} lv_bar_ext_t;

/** Bar styles. */
enum {
    LV_BAR_STYLE_BG, /** Bar background style. */
    LV_BAR_STYLE_INDIC, /** Bar fill area style. */
};
typedef uint8_t lv_bar_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a bar objects
 * @param par pointer to an object, it will be the parent of the new bar
 * @param copy pointer to a bar object, if not NULL then the new object will be copied from it
 * @return pointer to the created bar
 */
lv_obj_t * lv_bar_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the bar
 * @param bar pointer to a bar object
 * @param value new value
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_bar_set_value(lv_obj_t * bar, int16_t value, lv_anim_enable_t anim);

/**
 * Set minimum and the maximum values of a bar
 * @param bar pointer to the bar object
 * @param min minimum value
 * @param max maximum value
 */
void lv_bar_set_range(lv_obj_t * bar, int16_t min, int16_t max);

/**
 * Make the bar symmetric to zero. The indicator will grow from zero instead of the minimum
 * position.
 * @param bar pointer to a bar object
 * @param en true: enable disable symmetric behavior; false: disable
 */
void lv_bar_set_sym(lv_obj_t * bar, bool en);

/**
 * Set the animation time of the bar
 * @param bar pointer to a bar object
 * @param anim_time the animation time in milliseconds.
 */
void lv_bar_set_anim_time(lv_obj_t * bar, uint16_t anim_time);

/**
 * Set a style of a bar
 * @param bar pointer to a bar object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_bar_set_style(lv_obj_t * bar, lv_bar_style_t type, const lv_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a bar
 * @param bar pointer to a bar object
 * @return the value of the bar
 */
int16_t lv_bar_get_value(const lv_obj_t * bar);

/**
 * Get the minimum value of a bar
 * @param bar pointer to a bar object
 * @return the minimum value of the bar
 */
int16_t lv_bar_get_min_value(const lv_obj_t * bar);

/**
 * Get the maximum value of a bar
 * @param bar pointer to a bar object
 * @return the maximum value of the bar
 */
int16_t lv_bar_get_max_value(const lv_obj_t * bar);

/**
 * Get whether the bar is symmetric or not.
 * @param bar pointer to a bar object
 * @return true: symmetric is enabled; false: disable
 */
bool lv_bar_get_sym(lv_obj_t * bar);

/**
 * Get the animation time of the bar
 * @param bar pointer to a bar object
 * @return the animation time in milliseconds.
 */
uint16_t lv_bar_get_anim_time(lv_obj_t * bar);

/**
 * Get a style of a bar
 * @param bar pointer to a bar object
 * @param type which style should be get
 * @return style pointer to a style
 */
const lv_style_t * lv_bar_get_style(const lv_obj_t * bar, lv_bar_style_t type);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BAR*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BAR_H*/
