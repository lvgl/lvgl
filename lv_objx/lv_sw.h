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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#if USE_LV_SW != 0

/*Testing of dependencies*/
#if USE_LV_SLIDER == 0
#error "lv_sw: lv_slider is required. Enable it in lv_conf.h (USE_LV_SLIDER  1)"
#endif

#include "../lv_core/lv_obj.h"
#include "lv_slider.h"

/*********************
 *      DEFINES
 *********************/
#define LV_SWITCH_SLIDER_ANIM_MAX 1000

/**********************
 *      TYPEDEFS
 **********************/
/*Data of switch*/
typedef struct
{
    lv_slider_ext_t slider;         /*Ext. of ancestor*/
    /*New data for this type */
    lv_style_t *style_knob_off;     /*Style of the knob when the switch is OFF*/
    lv_style_t *style_knob_on;      /*Style of the knob when the switch is ON (NULL to use the same as OFF)*/
    lv_coord_t start_x;
    uint8_t changed   :1;           /*Indicates the switch state explicitly changed by drag*/
    uint8_t slided  :1;
#if USE_LV_ANIMATION
    uint16_t anim_time;				/*switch animation time */
#endif
} lv_sw_ext_t;

enum {
    LV_SW_STYLE_BG,
    LV_SW_STYLE_INDIC,
    LV_SW_STYLE_KNOB_OFF,
    LV_SW_STYLE_KNOB_ON,
};
typedef uint8_t lv_sw_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a switch objects
 * @param par pointer to an object, it will be the parent of the new switch
 * @param copy pointer to a switch object, if not NULL then the new object will be copied from it
 * @return pointer to the created switch
 */
lv_obj_t * lv_sw_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Turn ON the switch
 * @param sw pointer to a switch object
 */
void lv_sw_on(lv_obj_t *sw);

/**
 * Turn OFF the switch
 * @param sw pointer to a switch object
 */
void lv_sw_off(lv_obj_t *sw);

/**
 * Toggle the position of the switch
 * @param sw pointer to a switch object
 * @return resulting state of the switch.
 */
bool lv_sw_toggle(lv_obj_t *sw);

/**
 * Turn ON the switch with an animation
 * @param sw pointer to a switch object
 */
void lv_sw_on_anim(lv_obj_t * sw);

/**
 * Turn OFF the switch with an animation
 * @param sw pointer to a switch object
 */
void lv_sw_off_anim(lv_obj_t * sw);

/**
 * Toggle the position of the switch with an animation
 * @param sw pointer to a switch object
 * @return resulting state of the switch.
 */
bool lv_sw_toggle_anim(lv_obj_t *sw);

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
 * Set a style of a switch
 * @param sw pointer to a switch object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_sw_set_style(lv_obj_t *sw, lv_sw_style_t type, lv_style_t *style);

#if USE_LV_ANIMATION
/**
 * Set the animation time of the switch
 * @param sw pointer to a  switch object
 * @param anim_time animation time
 * @return style pointer to a style
 */
void lv_sw_set_anim_time(lv_obj_t *sw, uint16_t anim_time);
#endif

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the state of a switch
 * @param sw pointer to a switch object
 * @return false: OFF; true: ON
 */
static inline bool lv_sw_get_state(const lv_obj_t *sw)
{
    return lv_bar_get_value(sw) < LV_SWITCH_SLIDER_ANIM_MAX / 2 ? false : true;
}

/**
 * Get the switch action function
 * @param slider pointer to a switch object
 * @return the callback function
 */
static inline lv_action_t lv_sw_get_action(const lv_obj_t * slider)
{
    return lv_slider_get_action(slider);
}

/**
 * Get a style of a switch
 * @param sw pointer to a  switch object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_sw_get_style(const lv_obj_t *sw, lv_sw_style_t type);

/**
 * Get the animation time of the switch
 * @param sw pointer to a  switch object
 * @return style pointer to a style
 */
uint16_t lv_sw_get_anim_time(const lv_obj_t *sw);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_SW*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_SW_H*/
