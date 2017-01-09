/**
 * @file lv_gauge.h
 * 
 */


/*Search an replace: gauge -> object normal name with lower case (e.g. button, label etc.)
 * 					 gauge -> object short name with lower case(e.g. btn, label etc)
 *                   GAUGE -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

#ifndef LV_GAUGE_H
#define LV_GAUGE_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_GAUGE != 0

#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"
#include "lv_label.h"
#include "lv_line.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of gauge*/
typedef struct
{
	lv_rects_t rects;   /*Style of ancestor*/
	/*New style element for this type */
	lv_labels_t scale_labels;
    lv_lines_t needle_lines;
	cord_t label_pad;
	uint16_t angle;
    uint8_t label_num;
}lv_gauges_t;

/*Built-in styles of gauge*/
typedef enum
{
	LV_GAUGES_DEF,
}lv_gauges_builtin_t;

/*Data of gauge*/
typedef struct
{
	lv_rect_ext_t rect; /*Ext. of ancestor*/
	/*New data for this type */
    int16_t min;
    int16_t max;
    int16_t value;
}lv_gauge_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t * lv_gauge_create(lv_obj_t * par, lv_obj_t * copy);
bool lv_gauge_signal(lv_obj_t * gauge, lv_signal_t sign, void * param);
lv_gauges_t * lv_gauges_get(lv_gauges_builtin_t style, lv_gauges_t * copy);

void lv_gauge_set_value(lv_obj_t * gauge, int16_t value);

/**********************
 *      MACROS
 **********************/

#endif

#endif
