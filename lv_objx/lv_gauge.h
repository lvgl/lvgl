/**
 * @file lv_gauge.h
 * 
 */

#ifndef LV_GAUGE_H
#define LV_GAUGE_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include "misc_conf.h"
#if USE_LV_GAUGE != 0

/*Testing of dependencies*/
#if USE_LV_RECT == 0
#error "lv_gauge: lv_rect is required. Enable it in lv_conf.h (USE_LV_RECT  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_gauge: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif

#if USE_LV_RECT == 0
#error "lv_gauge: lv_line is required. Enable it in lv_conf.h (USE_LV_LINE  1) "
#endif

#if USE_TRIGO == 0
#error "lv_gauge: trigo is required. Enable it in misc_conf.h (USE_TRIGO  1) "
#endif


#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"
#include "lv_label.h"
#include "lv_line.h"

/*********************
 *      DEFINES
 *********************/
#define LV_GAUGE_MAX_NEEDLE     4   /*Max number of needles. Used in the style.*/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of gauge*/
typedef struct
{
    /*No inherited ext*/ /*Ext. of ancestor*/
    /*New data for this type */
    int16_t min;                /*Minimum value of the scale*/
    int16_t max;                /*Maximum value of the scale*/
    int16_t * values;           /*Array of the set values (for needles) */
    lv_style_t * style_critical;/*Fade to this style nearer to the critical value*/
    color_t * needle_color;     /*A color of the needles (color_t my_colors[needle_num])*/
    uint16_t scale_angle;       /*Angle of the scale in deg. (e.g. 220)*/
    uint8_t scale_label_num;    /*Number of scale labels (~6)*/
    uint8_t needle_num;         /*Number of needles*/
    uint8_t low_critical:1;     /*0: the higher value is more critical, 1: the lower value is more critical*/
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
 * Set the number of needles (should be  <= LV_GAUGE_MAX_NEEDLE)
 * @param gauge pointer to gauge object
 * @param num number of needles
 */
void lv_gauge_set_needle_num(lv_obj_t * gauge, uint8_t num, color_t * colors);

/**
 * Set the range of a gauge
 * @param gauge pointer to gauge object
 * @param min min value
 * @param max max value
 */
void lv_gauge_set_range(lv_obj_t * gauge, int16_t min, int16_t max);

/**
 * Set the value of a needle
 * @param gauge pointer to gauge
 * @param needle the id of the needle
 * @param value the new value
 */
void lv_gauge_set_value(lv_obj_t * gauge, uint8_t needle, int16_t value);

/**
 * Set text on a gauge
 * @param gauge pinter to a gauge object
 * @param txt a printf like format string
 *            with 1 place for a number (e.g. "Value: %d");
 */
void lv_gauge_set_text(lv_obj_t * gauge, const char * txt);

/**
 * Set which value is more critical (lower or higher)
 * @param gauge pointer to a gauge object
 * @param low false: higher / true: lower value is more critical
 */
void lv_gauge_set_low_critical(lv_obj_t * gauge, bool low);

void lv_gauge_vet_style_critical(lv_obj_t * gauge, lv_style_t * style);
/**
 * Get the number of needles on a gauge
 * @param gauge pointer to gauge
 * @return number of needles
 */
uint8_t lv_gauge_get_needle_num(lv_obj_t * gauge);

/**
 * Get the value of a needle
 * @param gauge pointer to gauge object
 * @param needle the id of the needle
 * @return the value of the needle [min,max]
 */
int16_t lv_gauge_get_value(lv_obj_t * gauge,  uint8_t needle);

/**
 * Get the text of a gauge
 * @param gauge pointer to gauge
 * @return the set text. (not with the current value)
 */
const char * lv_gauge_get_text(lv_obj_t * gauge);

/**
 * Get which value is more critical (lower or higher)
 * @param gauge pointer to a gauge object
 * @param low false: higher / true: lower value is more critical
 */
bool lv_gauge_get_low_critical(lv_obj_t * gauge);

lv_style_t * lv_gauge_get_style_crit(lv_obj_t * gauge);

/**********************
 *      MACROS
 **********************/

#endif

#endif
