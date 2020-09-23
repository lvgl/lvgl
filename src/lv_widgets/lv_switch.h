/**
 * @file lv_sw.h
 *
 */

#ifndef LV_SWITCH_H
#define LV_SWITCH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_SWITCH != 0

/*Testing of dependencies*/
#if LV_USE_SLIDER == 0
#error "lv_sw: lv_slider is required. Enable it in lv_conf.h (LV_USE_SLIDER  1)"
#endif

#include "../lv_core/lv_obj.h"
#include "lv_bar.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of switch*/
typedef struct {
    lv_bar_ext_t bar; /*Ext. of ancestor*/
    /*New data for this type */
    lv_style_list_t style_knob; /*Style of the knob*/
} lv_switch_ext_t;

/**
 * Switch parts.
 */
enum {
    LV_SWITCH_PART_BG = LV_BAR_PART_BG,                 /**< Switch background. */
    LV_SWITCH_PART_INDIC = LV_BAR_PART_INDIC,           /**< Switch fill area. */
    LV_SWITCH_PART_KNOB = _LV_BAR_PART_VIRTUAL_LAST,    /**< Switch knob. */
    _LV_SWITCH_PART_VIRTUAL_LAST
};

typedef uint8_t lv_switch_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a switch objects
 * @param par pointer to an object, it will be the parent of the new switch
 * @param copy pointer to a switch object, if not NULL then the new object will be copied from it
 * @return pointer to the created switch
 */
lv_obj_t * lv_switch_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Turn ON the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_switch_on(lv_obj_t * sw, lv_anim_enable_t anim);

/**
 * Turn OFF the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_switch_off(lv_obj_t * sw, lv_anim_enable_t anim);

/**
 * Toggle the position of the switch
 * @param sw pointer to a switch object
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 * @return resulting state of the switch.
 */
bool lv_switch_toggle(lv_obj_t * sw, lv_anim_enable_t anim);

/**
 * Set the animation time of the switch
 * @param sw pointer to a  switch object
 * @param anim_time animation time
 * @return style pointer to a style
 */
static inline void lv_switch_set_anim_time(lv_obj_t * sw, uint16_t anim_time)
{
    lv_bar_set_anim_time(sw, anim_time);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the state of a switch
 * @param sw pointer to a switch object
 * @return false: OFF; true: ON
 */
static inline bool lv_switch_get_state(const lv_obj_t * sw)
{
    return lv_bar_get_value(sw) == 1 ? true : false;
}

/**
 * Get the animation time of the switch
 * @param sw pointer to a  switch object
 * @return style pointer to a style
 */
static inline uint16_t lv_switch_get_anim_time(const lv_obj_t * sw)
{
    return lv_bar_get_anim_time(sw);
}

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SWITCH*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_SWITCH_H*/
