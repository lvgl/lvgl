/**
 * @file lv_gauge.h
 * 
 */

#ifndef LV_GAUGE_H
#define LV_GAUGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include "misc_conf.h"
#if USE_LV_GAUGE != 0

/*Testing of dependencies*/
#if USE_LV_LMETER == 0
#error "lv_gauge: lv_lmeter is required. Enable it in lv_conf.h (USE_LV_LMETER  1) "
#endif

#if USE_TRIGO == 0
#error "lv_gauge: trigo is required. Enable it in misc_conf.h (USE_TRIGO  1) "
#endif


#include "../lv_obj/lv_obj.h"
#include "lv_lmeter.h"
#include "lv_label.h"
#include "lv_line.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of gauge*/
typedef struct
{
    lv_lmeter_ext_t lmeter;     /*Ext. of ancestor*/
    /*New data for this type */
    int16_t * values;               /*Array of the set values (for needles) */
    color_t * needle_colors;        /*Color of the needles (color_t my_colors[needle_num])*/
    uint8_t needle_count;             /*Number of needles*/
    uint8_t label_count;              /*Number of labels on the scale*/
}lv_gauge_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a gauge objects
 * @param par pointer to an object, it will be the parent of the new gauge
 * @param copy pointer to a gauge object, if not NULL then the new object will be copied from it
 * @return pointer to the created gauge
 */
lv_obj_t * lv_gauge_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the gauge
 * @param gauge pointer to a gauge object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_gauge_signal(lv_obj_t * gauge, lv_signal_t sign, void * param);

/**
 * Set the number of needles
 * @param gauge pointer to gauge object
 * @param num number of needles
 * @param colors an array of colors for needles (with 'num' elements)
 */
void lv_gauge_set_needle_num(lv_obj_t * gauge, uint8_t num, color_t * colors);

/**
 * Set the value of a needle
 * @param gauge pointer to gauge
 * @param needle the id of the needle
 * @param value the new value
 */
void lv_gauge_set_value(lv_obj_t * gauge, uint8_t needle, int16_t value);

/**
 * Set which value is more critical (lower or higher)
 * @param gauge pointer to a gauge object
 * @param low false: higher / true: lower value is more critical
 */
void lv_gauge_set_low_critical(lv_obj_t * gauge, bool low);

/**
 * Set the critical style of the gauge
 * @param gauge pointer to a gauge object
 * @param style pointer to the new critical style
 */
void lv_gauge_set_style_critical(lv_obj_t * gauge, lv_style_t * style);

/**
 * Get the number of needles on a gauge
 * @param gauge pointer to gauge
 * @return number of needles
 */
uint8_t lv_gauge_get_needle_count(lv_obj_t * gauge);

uint8_t lv_gauge_get_label_count(lv_obj_t * gauge);
/**
 * Get the value of a needle
 * @param gauge pointer to gauge object
 * @param needle the id of the needle
 * @return the value of the needle [min,max]
 */
int16_t lv_gauge_get_value(lv_obj_t * gauge,  uint8_t needle);

/**
 * Get which value is more critical (lower or higher)
 * @param gauge pointer to a gauge object
 * @param low false: higher / true: lower value is more critical
 */
bool lv_gauge_get_low_critical(lv_obj_t * gauge);

/**
 * Get the critical style of the gauge
 * @param gauge pointer to a gauge object
 * @return pointer to the critical style
 */
lv_style_t * lv_gauge_get_style_critical(lv_obj_t * gauge);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_GAUGE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_GAUGE_H*/
