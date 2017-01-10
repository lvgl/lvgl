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
#define LV_GAUGE_MAX_NEEDLE     4   /*Max number of needles. Used in the style.*/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of gauge*/
typedef struct
{
	lv_rects_t rects;   /*Style of ancestor*/
	/*New style element for this type */
	color_t mcolor_critical;    /*Top color at critical.*/
    color_t gcolor_critical;    /*Bottom color at critical*/
    /*Scale settings*/
    uint16_t scale_angle;       /*Angle of the scale in deg. (~220)*/
	lv_labels_t scale_labels;   /*Style of the scale labels*/
    cord_t scale_pad;           /*Padding of scale labels from the edge*/
    uint8_t scale_label_num;    /*Number of scale labels (~6)*/
    /*Needle settings*/
    lv_lines_t needle_lines;    /*Style of neddles*/
    color_t needle_color[LV_GAUGE_MAX_NEEDLE];  /*Color of needles*/
    color_t needle_mid_color;   /*Color of middle  where the needles start*/
    cord_t needle_mid_r;        /*Radius of the needle middle area*/
    opa_t needle_opa;           /*Opacity of the needles*/
    /*Value text settings*/
    lv_labels_t value_labels;   /*Style of the value label*/
    uint8_t value_show  :1;     /*1: draw a label woth the most critical value*/
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
    int16_t * values;
    uint8_t needle_num;
    uint8_t low_critical    :1; /*0: the higher value is more critical, 1: the lower value is more critical*/
}lv_gauge_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t * lv_gauge_create(lv_obj_t * par, lv_obj_t * copy);
bool lv_gauge_signal(lv_obj_t * gauge, lv_signal_t sign, void * param);
lv_gauges_t * lv_gauges_get(lv_gauges_builtin_t style, lv_gauges_t * copy);

void lv_gauge_set_value(lv_obj_t * gauge, uint8_t needle, int16_t value)
void lv_gauge_set_needle_num(lv_obj_t * gauge, uint8_t num);
void lv_gauge_set_range(lv_obj_t * gauge, int16_t min, int16_t max);
void lv_gauge_set_low_critical(lv_obj_t * gauge, bool low);

uint8_t lv_gauge_get_needle_num(lv_obj_t * gauge);
int16_t lv_gauge_get_value(lv_obj_t * gauge,  uint8_t needle);
bool lv_gauge_get_low_critical(lv_obj_t * gauge);

/**********************
 *      MACROS
 **********************/

#endif

#endif
