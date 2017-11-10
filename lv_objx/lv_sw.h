/**
 * @file lv_sw.h
 * 
 */

#ifndef LV_SW_H
#define LV_SW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_SW != 0

/*Testing of dependencies*/
#if USE_LV_SW == 0
#error "lv_sw: lv_slider is required. Enable it in lv_conf.h (USE_LV_SLIDER  1)"
#endif

#include "../lv_obj/lv_obj.h"
#include "lv_slider.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of switch*/
typedef struct
{
    lv_slider_ext_t slider;         /*Ext. of ancestor*/
    /*New data for this type */
    lv_style_t *knob_off_style;       /*Style of the knob when the switch is OFF*/
    lv_style_t *knob_on_style;       /*Style of the knob when the switch is ON (NULL to use the same as OFF)*/
    uint8_t changed   :1;           /*Indicates the switch explicitly changed by drag*/
}lv_sw_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a switch objects
 * @param par pointer to an object, it will be the parent of the new switch
 * @param copy pointer to a switch object, if not NULL then the new object will be copied from it
 * @return pointer to the created switch
 */
lv_obj_t * lv_sw_create(lv_obj_t * par, lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Turn ON the switch
 * @param sw pointer to a switch object
 */
void lv_sw_set_on(lv_obj_t *sw);

/**
 * Turn OFF the switch
 * @param sw pointer to a switch object
 */
void lv_sw_set_off(lv_obj_t *sw);

/**
 * Set a function which will be called when the switch is toggled by the user
 * @param sw pointer to switch object
 * @param action a callback function
 */
static inline void lv_sw_set_action(lv_obj_t * sw, lv_action_t action)
{
    lv_slider_set_action(sw, action);
}

/**
 * Set the styles of a switch
 * @param sw pointer to a switch object
 * @param bg pointer to the background's style
 * @param indic pointer to the indicator's style
 * @param knob_off pointer to the knob's style when the switch is OFF
 * @param knob_on pointer to the knob's style when the switch is ON
 */
void lv_sw_set_style(lv_obj_t * sw, lv_style_t *bg, lv_style_t *indic, lv_style_t *knob_off, lv_style_t *knob_on);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the state of a switch
 * @param sw pointer to a switch object
 * @return false: OFF; true: ON
 */
static inline bool lv_sw_get_state(lv_obj_t *sw)
{
    return lv_bar_get_value(sw) == 0 ? false : true;
}

/**
 * Get the switch action function
 * @param slider pointer to a switch object
 * @return the callback function
 */
static inline lv_action_t lv_sw_get_action(lv_obj_t * slider)
{
    return lv_slider_get_action(slider);
}

/**
 * Get the style of the switch's background
 * @param sw pointer to a switch object
 * @return pointer to the switch's background style
 */
static inline lv_style_t * lv_sw_get_style_bg(lv_obj_t *sw)
{
    return lv_slider_get_style_bg(sw);
}

/**
 * Get the style of the switch's indicator
 * @param sw pointer to a switch object
 * @return pointer to the switch's indicator style
 */
static inline lv_style_t * lv_sw_get_style_indicator(lv_obj_t *sw)
{
    return lv_slider_get_style_indicator(sw);
}

/**
 * Get the style of the switch's knob when the switch is OFF
 * @param sw pointer to a switch object
 * @return pointer to the switch's knob OFF style
 */
lv_style_t * lv_sw_get_style_knob_off(lv_obj_t *sw);

/**
 * Get the style of the switch's knob when the switch is ON
 * @param sw pointer to a switch object
 * @return pointer to the switch's knob ON style
 */
lv_style_t * lv_sw_get_style_knob_on(lv_obj_t *sw);


/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_SW*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_SW_H*/
