/**
 * @file lv_preload.h
 *
 */

#ifndef LV_PRELOAD_H
#define LV_PRELOAD_H

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

#if USE_LV_PRELOAD != 0

/*Testing of dependencies*/
#if USE_LV_ARC == 0
#error "lv_preload: lv_arc is required. Enable it in lv_conf.h (USE_LV_ARC  1) "
#endif

#if USE_LV_ANIMATION == 0
#error "lv_preload: animations are required. Enable it in lv_conf.h (USE_LV_ANIMATION  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "lv_arc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_PRELOAD_TYPE_SPINNING_ARC,
};
typedef uint8_t lv_preloader_type_t;

/*Data of pre loader*/
typedef struct {
    lv_arc_ext_t arc; /*Ext. of ancestor*/
    /*New data for this type */
    uint16_t arc_length;            /*Length of the spinning indicator in degree*/
    uint16_t time;                  /*Time of one round*/
} lv_preload_ext_t;


/*Styles*/
enum {
    LV_PRELOAD_STYLE_MAIN,
};
typedef uint8_t lv_preload_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a pre loader objects
 * @param par pointer to an object, it will be the parent of the new pre loader
 * @param copy pointer to a pre loader object, if not NULL then the new object will be copied from it
 * @return pointer to the created pre loader
 */
lv_obj_t * lv_preload_create(lv_obj_t * par, const lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Set the length of the spinning  arc in degrees
 * @param preload pointer to a preload object
 * @param deg length of the arc
 */
void lv_preload_set_arc_length(lv_obj_t * preload, uint16_t deg);

/**
 * Set the spin time of the arc
 * @param preload pointer to a preload object
 * @param time time of one round in milliseconds
 */
void lv_preload_set_spin_time(lv_obj_t * preload, uint16_t time);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a pre loader.
 * @param preload pointer to pre loader object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_preload_set_style(lv_obj_t * preload, lv_preload_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the arc length [degree] of the a pre loader
 * @param preload pointer to a pre loader object
 */
uint16_t lv_preload_get_arc_length(const lv_obj_t * preload);

/**
 * Get the spin time of the arc
 * @param preload pointer to a pre loader object [milliseconds]
 */
uint16_t lv_preload_get_spin_time(const lv_obj_t * preload);

/**
 * Get style of a pre loader.
 * @param preload pointer to pre loader object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
lv_style_t * lv_preload_get_style(const lv_obj_t * preload, lv_preload_style_t type);

/*=====================
 * Other functions
 *====================*/

/**
 * Get style of a pre loader.
 * @param preload pointer to pre loader object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
void lv_preload_spinner_animation(void * ptr, int32_t val);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_PRELOAD*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_PRELOAD_H*/
