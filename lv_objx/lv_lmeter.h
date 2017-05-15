/**
 * @file lv_lmeter.h
 * 
 */

#ifndef LV_LMETER_H
#define LV_LMETER_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include "misc_conf.h"
#if USE_LV_LMETER != 0

/*Testing of dependencies*/
#if USE_LV_BAR == 0
#error "lv_lmeter: lv_bar is required. Enable it in lv_conf.h (USE_LV_BAR  1) "
#endif

#if USE_TRIGO == 0
#error "lv_lmeter: trigo is required. Enable it in misc_conf.h (USE_TRIGO  1) "
#endif


#include "../lv_obj/lv_obj.h"
#include "lv_bar.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of line meter*/
typedef struct
{
    lv_bar_ext_t bar;            /*Ext. of ancestor*/
    /*New data for this type */
    uint16_t scale_angle;        /*Angle of the scale in deg. (0..360)*/
    uint8_t scale_num;           /*Number of scale units */
}lv_lmeter_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a line meter objects
 * @param par pointer to an object, it will be the parent of the new line meter
 * @param copy pointer to a line meter object, if not NULL then the new object will be copied from it
 * @return pointer to the created line meter
 */
lv_obj_t * lv_lmeter_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the line meter
 * @param lmeter pointer to a line meter object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_lmeter_signal(lv_obj_t * lmeter, lv_signal_t sign, void * param);

/**
 * Set the scale settings of a line meter
 * @param lmeter pointer to a line meter object
 * @param angle angle of the scale (0..360)
 * @param num number of scale units
 */
void lv_lmeter_set_scale(lv_obj_t * lmeter, uint16_t angle, uint8_t num);

/**
 * Get the scale number of a line meter
 * @param lmeter pointer to a line meter object
 * @return number of the scale units
 */
uint8_t lv_lmeter_get_scale_num(lv_obj_t * lmeter);

/**
 * Get the scale angle of a line meter
 * @param lmeter pointer to a line meter object
 * @return angle of the scale
 */
uint16_t lv_lmeter_get_scale_angle(lv_obj_t * lmeter);
/**********************
 *      MACROS
 **********************/

#endif

#endif
