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
    lv_action_t action;             /*Function to call when a new value is set*/
    lv_style_t *style_knob;    /*Style of the knob*/
    int16_t drag_value;          /*Store a temporal value during press until release (Handled by the library)*/
    uint8_t knob_in     :1;     /*1: Draw the knob inside the bar*/
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
 * Set a function which will be called when a new value is set on the slider
 * @param slider pointer to slider object
 * @param cb a callback function
 */
void lv_slider_set_action(lv_obj_t * slider, lv_action_t cb);

/**
 * Set the styles of a slider
 * @param bar pointer to a bar object
 * @param bg pointer to the background's style
 * @param indic pointer to the indicator's style
 * @param knob pointer to the knob's style
 */
void lv_slider_set_style(lv_obj_t * slider, lv_style_t *bg, lv_style_t *indic, lv_style_t *knob);

/**
 * Set the 'knob in' attribute of a slider
 * @param slider pointer to slider object
 * @param in true: the knob is drawn always in the slider;
 *           false: the knob can be out on the edges
 */
void lv_slider_set_knob_in(lv_obj_t * slider, bool in);

/**
 * Get the slider callback function
 * @param slider pointer to slider object
 * @return the callback function
 */
lv_action_t lv_slider_get_action(lv_obj_t * slider);


/**
 * Set the styles of a slider
 * @param slider pointer to a bar object
 * @return pointer to the knob's style
 */
lv_style_t * lv_slider_get_style_knob(lv_obj_t * slider);

/**
 * Get the 'knob in' attribute of a slider
 * @param slider pointer to slider object
 * @return true: the knob is drawn always in the slider;
 *         false: the knob can be out on the edges
 */
bool lv_slider_get_knob_in(lv_obj_t * slider);

/******************************
 *  TRANSPARENT API FUNCTIONS
 ******************************/

/**
 * Set a new value on the slider
 * @param slider pointer to a slider object
 * @param value new value
 */
static inline void lv_slider_set_value(lv_obj_t * slider, int16_t value)
{
    lv_bar_set_value(slider, value);
}

/**
 * Set a new value with animation on a slider
 * @param slider pointer to a slider object
 * @param value new value
 * @param anim_time animation time in milliseconds
 */
static inline void lv_slider_set_value_anim(lv_obj_t * slider, int16_t value, uint16_t anim_time)
{
    lv_bar_set_value_anim(slider, value, anim_time);
}

/**
 * Set minimum and the maximum values of a bar
 * @param slider pointer to the slider object
 * @param min minimum value
 * @param max maximum value
 */
static inline void lv_slider_set_range(lv_obj_t *slider, int16_t min, int16_t max)
{
    lv_bar_set_range(slider, min, max);
}

/**
 * Get the value of a slider
 * @param slider pointer to a slider object
 * @return the value of the slider
 */
static inline int16_t lv_slider_get_value(lv_obj_t * slider)
{
    return lv_bar_get_value(slider);
}

/**
 * Get the minimum value of a slider
 * @param slider pointer to a slider object
 * @return the minimum value of the slider
 */
static inline int16_t lv_slider_get_min_value(lv_obj_t * slider)
{
    return lv_bar_get_min_value(slider);
}

/**
 * Get the maximum value of a slider
 * @param slider pointer to a slider object
 * @return the maximum value of the slider
 */
static inline int16_t lv_slider_get_max_value(lv_obj_t * slider)
{
    return lv_bar_get_max_value(slider);
}


/**
 * Get the style of the slider's background
 * @param slider pointer to a slider object
 * @return pointer to the slider's background style
 */
static inline lv_style_t * lv_slider_get_style_bg(lv_obj_t * slider)
{
    return lv_bar_get_style_bg(slider);
}

/**
 * Get the style of slider indicator
 * @param bar pointer to a slider object
 * @return pointer to the slider indicator style
 */
static inline lv_style_t * lv_slider_get_style_indicator(lv_obj_t * slider)
{
    return lv_bar_get_style_indicator(slider);
}

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_SLIDER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_SLIDER_H*/
