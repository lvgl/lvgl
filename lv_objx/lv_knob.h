/**
 * @file lv_knob.h
 * 
 */

#ifndef LV_KNOB_H
#define LV_KNOB_H

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
#if USE_LV_KNOB != 0

/*Testing of dependencies*/
#if USE_LV_LMETER == 0
#error "lv_knob: lv_lmeter is required. Enable it in lv_conf.h (USE_LV_LMETER  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "lv_lmeter.h"
#include "lv_label.h"
#include "lv_line.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of knob*/
typedef struct
{
    lv_lmeter_ext_t lmeter;     /*Ext. of ancestor*/
    /*New data for this type */
    int16_t * values;               /*Array of the set values (for needles) */
    const lv_color_t * needle_colors;        /*Color of the needles (lv_color_t my_colors[needle_num])*/
    uint8_t needle_count;             /*Number of needles*/
    uint8_t label_count;              /*Number of labels on the scale*/
}lv_knob_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a knob objects
 * @param par pointer to an object, it will be the parent of the new knob
 * @param copy pointer to a knob object, if not NULL then the new object will be copied from it
 * @return pointer to the created knob
 */
lv_obj_t * lv_knob_create(lv_obj_t * par, lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of needles
 * @param knob pointer to knob object
 * @param needle_cnt new count of needles
 * @param colors an array of colors for needles (with 'num' elements)
 */
void lv_knob_set_needle_count(lv_obj_t * knob, uint8_t needle_cnt, const lv_color_t * colors);

/**
 * Set the value of a needle
 * @param knob pointer to a knob
 * @param needle_id the id of the needle
 * @param value the new value
 */
void lv_knob_set_value(lv_obj_t * knob, int16_t value);

/**
 * Set minimum and the maximum values of a knob
 * @param knob pointer to he knob object
 * @param min minimum value
 * @param max maximum value
 */
static inline void lv_knob_set_range(lv_obj_t *knob, int16_t min, int16_t max)
{
    lv_lmeter_set_range(knob, min, max);
}

/**
 * Set a critical value on the scale. After this value 'line.color' scale lines will be drawn
 * @param knob pointer to a knob object
 * @param value the critical value
 */
static inline void lv_knob_set_critical_value(lv_obj_t * knob, int16_t value)
{
    lv_lmeter_set_value(knob, value);
}

/**
 * Set the scale settings of a knob
 * @param knob pointer to a knob object
 * @param angle angle of the scale (0..360)
 * @param line_cnt count of scale lines
 * @param label_cnt count of scale labels
 */
void lv_knob_set_scale(lv_obj_t * knob, uint16_t angle, uint8_t line_cnt, uint8_t label_cnt);

/**
 * Set the styles of a knob
 * @param knob pointer to a knob object
 * @param bg set the style of the knob
 *  */
static inline void lv_knob_set_style(lv_obj_t *knob, lv_style_t *bg)
{
    lv_obj_set_style(knob, bg);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a needle
 * @param knob pointer to knob object
 * @param needle the id of the needle
 * @return the value of the needle [min,max]
 */
int16_t lv_knob_get_value(lv_obj_t * knob);

/**
 * Get the count of needles on a knob
 * @param knob pointer to knob
 * @return count of needles
 */
uint8_t lv_knob_get_needle_count(lv_obj_t * knob);

/**
 * Get the minimum value of a knob
 * @param knob pointer to a knob object
 * @return the minimum value of the knob
 */
static inline int16_t lv_knob_get_min_value(lv_obj_t * lmeter)
{
    return lv_lmeter_get_min_value(lmeter);
}

/**
 * Get the maximum value of a knob
 * @param knob pointer to a knob object
 * @return the maximum value of the knob
 */
static inline int16_t lv_knob_get_max_value(lv_obj_t * lmeter)
{
    return lv_lmeter_get_max_value(lmeter);
}

/**
 * Get a critical value on the scale.
 * @param knob pointer to a knob object
 * @return the critical value
 */
static inline int16_t lv_knob_get_critical_value(lv_obj_t * knob)
{
    return lv_lmeter_get_value(knob);
}

/**
 * Set the number of labels (and the thicker lines too)
 * @param knob pointer to a knob object
 * @return count of labels
 */
uint8_t lv_knob_get_label_count(lv_obj_t * knob);

/**
 * Get the scale number of a knob
 * @param knob pointer to a knob object
 * @return number of the scale units
 */
static inline uint8_t lv_knob_get_line_count(lv_obj_t * knob)
{
    return lv_lmeter_get_line_count(knob);
}

/**
 * Get the scale angle of a knob
 * @param knob pointer to a knob object
 * @return angle of the scale
 */
static inline uint16_t lv_knob_get_scale_angle(lv_obj_t * knob)
{
    return lv_lmeter_get_scale_angle(knob);
}

/**
 * Get the style of a knob
 * @param knob pointer to a knob object
 * @return pointer to the knob's style
 */
static inline lv_style_t * lv_knob_get_style(lv_obj_t *knob)
{
    return lv_obj_get_style(knob);
}

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_KNOB*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_KNOB_H*/
