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
#include "lv_conf.h"
#if USE_LV_SLIDER != 0

#include "../lv_obj/lv_obj.h"
#include "lv_bar.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of slider*/
typedef struct
{
    lv_bar_ext_t bar;       /*Ext. of ancestor*/
    /*New data for this type */
    lv_action_t cb;             /*Function to call when a new value is set*/
    lv_style_t * style_knob;    /*Style of the knob*/
    int16_t tmp_value;          /*Store a temporal value during press until release (Handled by the library)*/
}lv_slider_ext_t;

/*Built-in styles of slider*/
typedef enum
{
	LV_SLIDERS_DEF,
}lv_sliders_builtin_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a slider objects
 * @param par pointer to an object, it will be the parent of the new slider
 * @param copy pointer to a slider object, if not NULL then the new object will be copied from it
 * @return pointer to the created slider
 */
lv_obj_t * lv_slider_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the slider
 * @param slider pointer to a slider object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_slider_signal(lv_obj_t * slider, lv_signal_t sign, void * param);

/**
 * Set a function which will be called when a new value is set on the slider
 * @param slider pointer to slider object
 * @param cb a callback function
 */
void lv_slider_set_action(lv_obj_t * slider, lv_action_t cb);

/**
 * Set the style of knob on a slider
 * @param slider pointer to slider object
 * @param style pointer the new knob style
 */
void lv_slider_set_style_knob(lv_obj_t * slider, lv_style_t * style);

/**
 * Get the slider callback function
 * @param slider pointer to slider object
 * @return the callback function
 */
lv_action_t lv_slider_get_action(lv_obj_t * slider);

/**
 * Get the style of knob on a slider
 * @param slider pointer to slider object
 * @return pointer the new knob style
 */
lv_style_t *  lv_slider_get_style_knob(lv_obj_t * slider);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_SLIDER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_SLIDER_H*/
