/**
 * @file lv_spinner.h
 *
 */

#ifndef LV_SPINNER_H
#define LV_SPINNER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_SPINNER != 0

/*Testing of dependencies*/
#if LV_USE_ARC == 0
#error "lv_spinner: lv_arc is required. Enable it in lv_conf.h (LV_USE_ARC 1)"
#endif

#if LV_USE_ANIMATION == 0
#error "lv_spinner: animations are required. Enable it in lv_conf.h (LV_USE_ANIMATION 1)"
#endif

#include "../lv_core/lv_obj.h"
#include "../lv_misc/lv_anim.h"
#include "lv_arc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Type of spinner.
 */
enum {
    LV_SPINNER_TYPE_SPINNING_ARC,
    LV_SPINNER_TYPE_FILLSPIN_ARC,
    LV_SPINNER_TYPE_CONSTANT_ARC,
};
typedef uint8_t lv_spinner_type_t;

/**
 * Direction the spinner should spin.
 */
enum {
    LV_SPINNER_DIR_FORWARD,
    LV_SPINNER_DIR_BACKWARD,
};
typedef uint8_t lv_spinner_dir_t;

/*Data of spinner*/
typedef struct {
    lv_arc_ext_t arc; /*Ext. of ancestor*/
    /*New data for this type */
    lv_anim_value_t arc_length;      /*Length of the spinning indicator in degree*/
    uint16_t time;                   /*Time of one round*/
    lv_spinner_type_t anim_type : 2; /*Type of the arc animation*/
    lv_spinner_dir_t anim_dir : 1;   /*Animation Direction*/
} lv_spinner_ext_t;

/*Parts of the spinner*/
enum {
    LV_SPINNER_PART_BG = LV_ARC_PART_BG,
    LV_SPINNER_PART_INDIC = LV_ARC_PART_INDIC,
    _LV_SPINNER_PART_VIRTUAL_LAST,

    _LV_SPINNER_PART_REAL_LAST = _LV_ARC_PART_REAL_LAST,
};
typedef uint8_t lv_spinner_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a spinner object
 * @param par pointer to an object, it will be the parent of the new spinner
 * @param copy pointer to a spinner object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created spinner
 */
lv_obj_t * lv_spinner_create(lv_obj_t * par, const lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Set the length of the spinning arc in degrees
 * @param spinner pointer to a spinner object
 * @param deg length of the arc
 */
void lv_spinner_set_arc_length(lv_obj_t * spinner, lv_anim_value_t deg);

/**
 * Set the spin time of the arc
 * @param spinner pointer to a spinner object
 * @param time time of one round in milliseconds
 */
void lv_spinner_set_spin_time(lv_obj_t * spinner, uint16_t time);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the animation type of a spinner.
 * @param spinner pointer to spinner object
 * @param type animation type of the spinner
 *  */
void lv_spinner_set_type(lv_obj_t * spinner, lv_spinner_type_t type);

/**
 * Set the animation direction of a spinner
 * @param spinner pointer to spinner object
 * @param direction animation direction of the spinner
 */
void lv_spinner_set_dir(lv_obj_t * spinner, lv_spinner_dir_t dir);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the arc length [degree] of the a spinner
 * @param spinner pointer to a spinner object
 */
lv_anim_value_t lv_spinner_get_arc_length(const lv_obj_t * spinner);

/**
 * Get the spin time of the arc
 * @param spinner pointer to a spinner object [milliseconds]
 */
uint16_t lv_spinner_get_spin_time(const lv_obj_t * spinner);

/**
 * Get the animation type of a spinner.
 * @param spinner pointer to spinner object
 * @return animation type
 *  */
lv_spinner_type_t lv_spinner_get_type(lv_obj_t * spinner);

/**
 * Get the animation direction of a spinner
 * @param spinner pointer to spinner object
 * @return animation direction
 */
lv_spinner_dir_t lv_spinner_get_dir(lv_obj_t * spinner);

/*=====================
 * Other functions
 *====================*/

/**
 * Animator function  (exec_cb) to rotate the arc of spinner.
 * @param ptr pointer to spinner
 * @param val the current desired value [0..360]
 */
void lv_spinner_anim_cb(void * ptr, lv_anim_value_t val);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SPINNER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_SPINNER_H*/
