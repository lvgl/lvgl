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
#include "../lv_conf_internal.h"

#if LV_USE_SLIDER != 0

/*Testing of dependencies*/
#if LV_USE_BAR == 0
#error "lv_slider: lv_bar is required. Enable it in lv_conf.h (LV_USE_BAR 1)"
#endif

#include "../lv_core/lv_obj.h"
#include "lv_bar.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_SLIDER_TYPE_NORMAL,
    LV_SLIDER_TYPE_SYMMETRICAL,
    LV_SLIDER_TYPE_RANGE
};
typedef uint8_t lv_slider_type_t;

/*Data of slider*/
typedef struct {
    lv_bar_ext_t bar; /*Ext. of ancestor*/
    /*New data for this type */
    lv_style_list_t style_knob; /*Style of the knob*/
    lv_area_t left_knob_area;
    lv_area_t right_knob_area;
    int16_t * value_to_set; /* Which bar value to set */
    uint8_t dragging : 1;       /*1: the slider is being dragged*/
    uint8_t left_knob_focus : 1; /*1: with encoder now the right knob can be adjusted*/
} lv_slider_ext_t;

/** Built-in styles of slider*/
enum {
    LV_SLIDER_PART_BG, /** Slider background style. */
    LV_SLIDER_PART_INDIC, /** Slider indicator (filled area) style. */
    LV_SLIDER_PART_KNOB, /** Slider knob style. */
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a slider objects
 * @param par pointer to an object, it will be the parent of the new slider
 * @param copy pointer to a slider object, if not NULL then the new object will be copied from it
 * @return pointer to the created slider
 */
lv_obj_t * lv_slider_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the slider
 * @param slider pointer to a slider object
 * @param value new value
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
static inline void lv_slider_set_value(lv_obj_t * slider, int16_t value, lv_anim_enable_t anim)
{
    lv_bar_set_value(slider, value, anim);
}

/**
 * Set a new value for the left knob of a slider
 * @param slider pointer to a slider object
 * @param left_value new value
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
static inline void lv_slider_set_left_value(lv_obj_t * slider, int16_t left_value, lv_anim_enable_t anim)
{
    lv_bar_set_start_value(slider, left_value, anim);
}

/**
 * Set minimum and the maximum values of a bar
 * @param slider pointer to the slider object
 * @param min minimum value
 * @param max maximum value
 */
static inline void lv_slider_set_range(lv_obj_t * slider, int16_t min, int16_t max)
{
    lv_bar_set_range(slider, min, max);
}

/**
 * Set the animation time of the slider
 * @param slider pointer to a bar object
 * @param anim_time the animation time in milliseconds.
 */
static inline void lv_slider_set_anim_time(lv_obj_t * slider, uint16_t anim_time)
{
    lv_bar_set_anim_time(slider, anim_time);
}

/**
 * Make the slider symmetric to zero. The indicator will grow from zero instead of the minimum
 * position.
 * @param slider pointer to a slider object
 * @param en true: enable disable symmetric behavior; false: disable
 */
static inline void lv_slider_set_type(lv_obj_t * slider, lv_slider_type_t type)
{
    if(type == LV_SLIDER_TYPE_NORMAL)
        lv_bar_set_type(slider, LV_BAR_TYPE_NORMAL);
    else if(type == LV_SLIDER_TYPE_SYMMETRICAL)
        lv_bar_set_type(slider, LV_BAR_TYPE_SYMMETRICAL);
    else if(type == LV_SLIDER_TYPE_RANGE)
        lv_bar_set_type(slider, LV_BAR_TYPE_CUSTOM);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of the main knob of a slider
 * @param slider pointer to a slider object
 * @return the value of the main knob of the slider
 */
int16_t lv_slider_get_value(const lv_obj_t * slider);

/**
 * Get the value of the left knob of a slider
 * @param slider pointer to a slider object
 * @return the value of the left knob of the slider
 */
static inline int16_t lv_slider_get_left_value(const lv_obj_t * slider)
{
    return lv_bar_get_start_value(slider);
}

/**
 * Get the minimum value of a slider
 * @param slider pointer to a slider object
 * @return the minimum value of the slider
 */
static inline int16_t lv_slider_get_min_value(const lv_obj_t * slider)
{
    return lv_bar_get_min_value(slider);
}

/**
 * Get the maximum value of a slider
 * @param slider pointer to a slider object
 * @return the maximum value of the slider
 */
static inline int16_t lv_slider_get_max_value(const lv_obj_t * slider)
{
    return lv_bar_get_max_value(slider);
}

/**
 * Give the slider is being dragged or not
 * @param slider pointer to a slider object
 * @return true: drag in progress false: not dragged
 */
bool lv_slider_is_dragged(const lv_obj_t * slider);

/**
 * Get the animation time of the slider
 * @param slider pointer to a slider object
 * @return the animation time in milliseconds.
 */
static inline uint16_t lv_slider_get_anim_time(lv_obj_t * slider)
{
    return lv_bar_get_anim_time(slider);
}

/**
 * Get whether the slider is symmetric or not.
 * @param slider pointer to a bar object
 * @return true: symmetric is enabled; false: disable
 */
static inline lv_slider_type_t lv_slider_get_type(lv_obj_t * slider)
{
    lv_bar_type_t type = lv_bar_get_type(slider);
    if(type == LV_BAR_TYPE_SYMMETRICAL)
        return LV_SLIDER_TYPE_SYMMETRICAL;
    else if(type == LV_BAR_TYPE_CUSTOM)
        return LV_SLIDER_TYPE_RANGE;
    else
        return LV_SLIDER_TYPE_NORMAL;
}

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SLIDER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_SLIDER_H*/
