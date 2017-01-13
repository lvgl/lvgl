/**
 * @file lv_pb.h
 * 
 */

#ifndef LV_PB_H
#define LV_PB_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_PB != 0

/*Testing of dependencies*/
#if USE_LV_RECT == 0
#error "lv_pb: lv_rect is required. Enable it in lv_conf.h (USE_LV_RECT  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_pb: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif

#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of progress bar*/
typedef struct
{
	lv_rects_t bg;
	lv_rects_t bar;
	lv_labels_t label;
}lv_pbs_t;

/*Built-in styles of progress bar*/
typedef enum
{
	LV_PBS_DEF,
}lv_pbs_builtin_t;

/*Data of progress bar*/
typedef struct
{
	lv_rect_ext_t rect_ext;	/*Ext. of ancestor*/
	/*New data for this type */
	lv_obj_t * label;
	uint16_t act_value;
	uint16_t min_value;
	uint16_t max_value;
	char * format_str; /*Format string of the label*/
}lv_pb_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a progress bar objects
 * @param par pointer to an object, it will be the parent of the new progress bar
 * @param copy pointer to a progress bar object, if not NULL then the new object will be copied from it
 * @return pointer to the created progress bar
 */
lv_obj_t * lv_pb_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the progress bar
 * @param pb pointer to a progress bar object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_pb_signal(lv_obj_t * pb, lv_signal_t sign, void * param);

/**
 * Set a new value on the progress bar
 * @param pb pointer to a progress bar object
 * @param value new value
 */
void lv_pb_set_value(lv_obj_t * pb, uint16_t value);

/**
 * Set minimum and the maximum values of a progress bar
 * @param pb pointer to he progress bar object
 * @param min minimum value
 * @param max maximum value
 */
void lv_pb_set_min_max_value(lv_obj_t * pb, uint16_t min, uint16_t max);

/**
 * Set format string for the label of the progress bar
 * @param pb pointer to progress bar object
 * @param format a printf-like format string with one number (e.g. "Loading (%d)")
 */
void lv_pb_set_format_str(lv_obj_t * pb, const char * format);

/**
 * Get the value of a progress bar
 * @param pb pointer to a progress bar object
 * @return the value of the progress bar
 */
uint16_t lv_pb_get_value(lv_obj_t * pb);

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_pbs_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_pbs_t style
 */
lv_pbs_t * lv_pbs_get(lv_pbs_builtin_t style, lv_pbs_t * copy);

/**********************
 *      MACROS
 **********************/

#endif

#endif
