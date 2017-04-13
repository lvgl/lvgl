/**
 * @file lv_bar.h
 * 
 */

#ifndef LV_BAR_H
#define LV_BAR_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_BAR != 0

/*Testing of dependencies*/
#if USE_LV_RECT == 0
#error "lv_bar: lv_rect is required. Enable it in lv_conf.h (USE_LV_RECT  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_bar: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif

#include "../lv_obj/lv_obj.h"
#include <lvgl/lv_objx/lv_cont.h>
#include "lv_btn.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of bar*/
typedef struct
{
    /*No inherited ext*/        /*Ext. of ancestor*/
    /*New data for this type */
    int16_t act_value;          /*Current value of the bar*/
    int16_t min_value;          /*Minimum value of the bar*/
    int16_t max_value;          /*Maximum value of the bar*/
    lv_style_t * style_indic;   /*Style of the indicator*/
}lv_bar_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a progress bar objects
 * @param par pointer to an object, it will be the parent of the new progress bar
 * @param copy pointer to a progress bar object, if not NULL then the new object will be copied from it
 * @return pointer to the created progress bar
 */
lv_obj_t * lv_bar_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the progress bar
 * @param bar pointer to a progress bar object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_bar_signal(lv_obj_t * bar, lv_signal_t sign, void * param);

/**
 * Set a new value on the progress bar
 * @param bar pointer to a progress bar object
 * @param value new value
 */
void lv_bar_set_value(lv_obj_t * bar, int16_t value);

/**
 * Set minimum and the maximum values of a progress bar
 * @param bar pointer to he progress bar object
 * @param min minimum value
 * @param max maximum value
 */
void lv_bar_set_range(lv_obj_t * bar, int16_t min, int16_t max);

/**
 * Set format string for the label of the progress bar
 * @param bar pointer to progress bar object
 * @param format a printf-like format string with one number (e.g. "Loading (%d)")
 */
void lv_bar_set_format_str(lv_obj_t * bar, const char * format);

/**
 * Get the value of a progress bar
 * @param bar pointer to a progress bar object
 * @return the value of the progress bar
 */
int16_t lv_bar_get_value(lv_obj_t * bar);

lv_style_t * lv_bar_get_style_indic(lv_obj_t * bar);
void lv_bar_set_style_indic(lv_obj_t * bar, lv_style_t * style);
/**********************
 *      MACROS
 **********************/

#endif

#endif
