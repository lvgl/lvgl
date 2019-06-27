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
#include "../../../lv_conf.h"
#endif

#if LV_USE_SW != 0

/*Testing of dependencies*/
#if LV_USE_SLIDER == 0
#error "lv_sw: lv_slider is required. Enable it in lv_conf.h (LV_USE_SLIDER  1)"
#endif

#include "../lv_core/lv_obj.h"
#include "lv_slider.h"

/*********************
 *      DEFINES
 *********************/
#define LV_SW_MAX_VALUE 100

/**********************
 *      TYPEDEFS
 **********************/
/*Data of switch*/
typedef struct
{
    lv_slider_ext_t slider; /*Ext. of ancestor*/
    /*New data for this type */
    const lv_style_t * style_knob_off; /*Style of the knob when the switch is OFF*/
    const lv_style_t * style_knob_on;  /*Style of the knob when the switch is ON (NULL to use the same as OFF)*/
    lv_coord_t start_x;
    uint8_t changed : 1; /*Indicates the switch state explicitly changed by drag*/
    uint8_t slided : 1;
#if LV_USE_ANIMATION
    uint16_t anim_time; /*switch animation time */
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
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_sw_on(lv_obj_t * sw, lv_anim_enable_t anim);

/**
 * Turn OFF the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_sw_off(lv_obj_t * sw, lv_anim_enable_t anim);

/**
 * Toggle the position of the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 * @return resulting state of the switch.
 */
bool lv_sw_toggle(lv_obj_t * sw, lv_anim_enable_t anim);

/**
 * Set a style of a switch
 * @param sw pointer to a switch object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_sw_set_style(lv_obj_t * sw, lv_sw_style_t type, const lv_style_t * style);

/**
 * Set the animation time of the switch
 * @param sw pointer to a  switch object
 * @param anim_time animation time
 * @return style pointer to a style
 */
void lv_sw_set_anim_time(lv_obj_t * sw, uint16_t anim_time);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the state of a switch
 * @param sw pointer to a switch object
 * @return false: OFF; true: ON
 */
static inline bool lv_sw_get_state(const lv_obj_t * sw)
{
    return lv_bar_get_value(sw) < LV_SW_MAX_VALUE / 2 ? false : true;
}

/**
 * Get a style of a switch
 * @param sw pointer to a  switch object
 * @param type which style should be get
 * @return style pointer to a style
 */
const lv_style_t * lv_sw_get_style(const lv_obj_t * sw, lv_sw_style_t type);

/**
 * Get the animation time of the switch
 * @param sw pointer to a  switch object
 * @return style pointer to a style
 */
uint16_t lv_sw_get_anim_time(const lv_obj_t * sw);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SW*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_SW_H*/
