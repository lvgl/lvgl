/**
 * @file lv_rotary.h
 *
 */

#ifndef LV_ROTARY_H
#define LV_ROTARY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_ROTARY != 0

/*Testing of dependencies*/
#if LV_USE_ARC == 0
#error "lv_rotary: lv_arc is required. Enable it in lv_conf.h (LV_USE_ARC 1) "
#endif

#if LV_USE_BTN == 0
#error "lv_rotary: lv_btn is required. Enable it in lv_conf.h (LV_USE_BTN  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "lv_arc.h"
#include "lv_btn.h"

/*********************
 *      DEFINES
 *********************/

#define LV_ROTARY_KNOB_SIZE_MIN  4   /* cannot make the knob smaller then this [px] */

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_ROTARY_TYPE_NORMAL
};
typedef uint8_t lv_rotary_type_t;

/** Possible states of a rotary.*/
enum {
    LV_ROTARY_STATE_RELEASED = LV_BTN_STATE_RELEASED,
    LV_ROTARY_STATE_PRESSED = LV_BTN_STATE_PRESSED,
    LV_ROTARY_STATE_DISABLED = LV_BTN_STATE_DISABLED,
    LV_ROTARY_STATE_CHECKED_RELEASED = LV_BTN_STATE_CHECKED_RELEASED,
    LV_ROTARY_STATE_CHECKED_PRESSED = LV_BTN_STATE_CHECKED_PRESSED,
    LV_ROTARY_STATE_CHECKED_DISABLED = LV_BTN_STATE_CHECKED_DISABLED,
    _LV_ROTARY_STATE_LAST = _LV_BTN_STATE_LAST, /* Number of states*/
};
typedef uint8_t lv_btn_state_t;

/*Data of rotary*/
typedef struct {
    /*Ext. of ancestor*/
    lv_arc_ext_t arc;
    /*New data for this type*/
    lv_style_list_t style_knob; /* Style of the knob */
    int16_t cur_value; /*Current value of the rotary*/
    int16_t min_value; /*Minimum value of the rotary*/
    int16_t max_value; /*Maximum value of the rotary*/
    int16_t * value_to_set; /*Start value of the rotary*/
    uint16_t dragging   :1;
    uint16_t sym        :1;
} lv_rotary_ext_t;

/** Built-in styles of rotary*/
enum {
    LV_ROTARY_PART_BG = LV_ARC_PART_BG, /** Rotary background style. */
    LV_ROTARY_PART_INDIC = LV_ARC_PART_INDIC, /** Rotary indicator (filled area) style. */
    LV_ROTARY_PART_KNOB = _LV_ARC_PART_VIRTUAL_LAST, /** Rotary knob style. */
    _LV_ROTARY_PART_VIRTUAL_LAST
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a rotary objects
 * @param par pointer to an object, it will be the parent of the new rotary
 * @param copy pointer to a rotary object, if not NULL then the new object will be copied from it
 * @return pointer to the created rotary
 */
lv_obj_t * lv_rotary_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the rotary
 * @param rotary pointer to a rotary object
 * @param value new value
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_rotary_set_value(lv_obj_t * rotary, int16_t value, lv_anim_enable_t anim);

/**
 * Set minimum and the maximum values of a rotary
 * @param rotary pointer to the rotary object
 * @param min minimum value
 * @param max maximum value
 */
void lv_rotary_set_range(lv_obj_t * rotary, int16_t min, int16_t max);

/**
 * Make the rotary symmetric to zero. The indicator will grow from zero instead of the minimum
 * position.
 * @param rotary pointer to a rotary object
 * @param en true: enable disable symmetric behavior; false: disable
 */
void lv_rotary_set_symmetric(lv_obj_t * rotary, bool en);

/**
 * Set the start angle of rotary indicator. 0 deg: right, 90 bottom, etc.
 * @param arc pointer to an arc object
 * @param start the start angle
 */
static inline void lv_rotary_set_start_angle(lv_obj_t * rotary, uint16_t start) {
    lv_arc_set_start_angle(rotary, start);
}

/**
 * Set the start angle of rotary indicator. 0 deg: right, 90 bottom, etc.
 * @param rotary pointer to an rotary object
 * @param end the end angle
 */
static inline void lv_rotary_set_end_angle(lv_obj_t * rotary, uint16_t end) {
    lv_arc_set_end_angle(rotary, end);
}

/**
 * Set the start and end angles
 * @param rotary pointer to an rotary object
 * @param start the start angle
 * @param end the end angle
 */
static inline void lv_rotary_set_angles(lv_obj_t * rotary, uint16_t start, uint16_t end) {
    lv_arc_set_angles(rotary, start, end);
}

/**
 * Set the start angle of ar otary indicator background. 0 deg: right, 90 bottom, etc.
 * @param rotary pointer to an rotary object
 * @param start the start angle
 */
static inline void lv_rotary_set_bg_start_angle(lv_obj_t * rotary, uint16_t start) {
    lv_arc_set_bg_start_angle(rotary, start); 
}

/**
 * Set the start angle of rotary indicator background. 0 deg: right, 90 bottom etc.
 * @param rotary pointer to an rotary object
 * @param end the end angle
 */
static inline void lv_rotary_set_bg_end_angle(lv_obj_t * rotary, uint16_t end) {
    lv_arc_set_bg_end_angle(rotary, end);
}

/**
 * Set the start and end angles of the rotary indicator background
 * @param rotary pointer to an rotary object
 * @param start the start angle
 * @param end the end angle
 */
static inline void lv_rotary_set_bg_angles(lv_obj_t * rotary, uint16_t start, uint16_t end) {
    lv_arc_set_bg_angles(rotary, start, end);
}

/**
 * Set the rotation for the whole rotary indicator
 * @param rotary pointer to an rotary object
 * @param rotation_angle rotation angle
 */
static inline void lv_rotary_set_rotation(lv_obj_t * rotary, uint16_t rotation_angle) {
    lv_arc_set_rotation(rotary, rotation_angle);
}

/**
 * Set the state of the rotary
 * @param rotary pointer to a rotary object
 * @param state the new state of the rotary (from lv_rotary_state_t enum)
 */
void lv_rotary_set_state(lv_obj_t * rotary, lv_rotary_state_t state)
{
    lv_btn_set_state(rotary, state);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the start angle of a rotary indicator.
 * @param rotary pointer to an rotary object
 * @return the start angle [0..360]
 */
static inline uint16_t lv_rotary_get_angle_start(lv_obj_t * rotary) {
    return lv_rotary_get_angle_start(rotary);
}

/**
 * Get the end angle of a rotary indicator.
 * @param rotary pointer to an rotary object
 * @return the end angle [0..360]
 */
static inline uint16_t lv_rotary_get_angle_end(lv_obj_t * rotary) {
    return lv_arc_get_angle_end(rotary);
}

/**
 * Get the start angle of a rotary indicator background.
 * @param rotary pointer to an rotary object
 * @return the start angle [0..360]
 */
static inline uint16_t lv_rotary_get_bg_angle_start(lv_obj_t * rotary) {
    return lv_arc_get_bg_angle_start(rotary);
}

/**
 * Get the end angle of a rotary indicator background.
 * @param rotary pointer to an rotary object
 * @return the end angle [0..360]
 */
static inline uint16_t lv_rotary_get_bg_angle_end(lv_obj_t * rotary) {
    return lv_arc_get_bg_angle_end(rotary);
}

/**
 * Get the value of the of a rotary
 * @param rotary pointer to a rotary object
 * @return the value of the of the rotary
 */
int16_t lv_rotary_get_value(const lv_obj_t * rotary);

/**
 * Get the minimum value of a rotary
 * @param rotary pointer to a rotary object
 * @return the minimum value of the rotary
 */
int16_t lv_rotary_get_min_value(const lv_obj_t * rotary);

/**
 * Get the maximum value of a rotary
 * @param rotary pointer to a rotary object
 * @return the maximum value of the rotary
 */
int16_t lv_rotary_get_max_value(const lv_obj_t * rotary);

/**
 * Give the rotary is being dragged or not
 * @param rotary pointer to a rotary object
 * @return true: drag in progress false: not dragged
 */
bool lv_rotary_is_dragged(const lv_obj_t * rotary);

/**
 * Get whether the rotary is symmetric or not.
 * @param rotary pointer to a rotary object
 * @return true: symmetric is enabled; false: disable
 */
static inline bool lv_rotary_get_symmetric(lv_obj_t * rotary)
{
    lv_rotary_ext_t * ext = (lv_rotary_ext_t *)lv_obj_get_ext_attr(rotary);
    return ext->sym;
}

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_rotary*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_ROTARY_H*/
