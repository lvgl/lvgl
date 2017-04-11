/**
 * @file lv_chart.h
 *
 */

#ifndef LV_CHARTBG_H
#define LV_CHARTBG_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_CHART != 0

/*Testing of dependencies*/
#if USE_LV_RECT == 0
#error "lv_chart: lv_rect is required. Enable it in lv_conf.h (USE_LV_RECT  1) "
#endif

#if USE_LV_LINE == 0
#error "lv_chart: lv_line is required. Enable it in lv_conf.h (USE_LV_LINE  1) "
#endif

#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"
#include "lv_line.h"

/*********************
 *      DEFINES
 *********************/
#define LV_CHART_DL_NUM		8	/*Max data line number. Used in the style.*/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of chart */
typedef struct
{
    lv_rect_ext_t bg_rect; /*Ext. of ancestor*/
    /*New data for this type */
    cord_t ymin;          /*y min value (used to scale the data)*/
    cord_t ymax;          /*y max value (used to scale the data)*/
    uint8_t hdiv_num;     /*Number of horizontal division lines*/
    uint8_t vdiv_num;     /*Number of vertical division lines*/
    ll_dsc_t dl_ll;       /*Linked list for the data line pointers (stores cord_t * )*/
    uint16_t pnum;        /*Point number in a data line*/
    uint8_t type    :2;   /*Line, column or point chart (from 'lv_chart_type_t')*/
    uint8_t dl_num;       /*Data line number in dl_ll*/
}lv_chart_ext_t;

/*Chart types*/
typedef enum
{
	LV_CHART_LINE,
	LV_CHART_COL,
	LV_CHART_POINT,
}lv_chart_type_t;

/*Style of chart*/
typedef struct
{
	lv_rects_t bg_rect;              /*Style of the ancestor*/
	/*New style element for this type */
	lv_lines_t div_line;
	color_t color[LV_CHART_DL_NUM];	/*Line/Point/Col colors */
	cord_t width;                   /*Data line width or point radius*/
	opa_t data_opa;				    /*Line/Point/Col opacity*/
	opa_t dark_eff;			        /*Dark effect on the bottom of points and columns*/
}lv_charts_t;

/*Built-in styles of chart*/
typedef enum
{
	LV_CHARTS_DEF,
}lv_charts_builtin_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a chart background objects
 * @param par pointer to an object, it will be the parent of the new chart background
 * @param copy pointer to a chart background object, if not NULL then the new object will be copied from it
 * @return pointer to the created chart background
 */
lv_obj_t * lv_chart_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the chart background
 * @param chart pointer to a chart background object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_chart_signal(lv_obj_t * chart, lv_signal_t sign, void * param);

/**
 * Allocate and add a data line to the chart
 * @param chart pointer to a chart object
 * @return pointer to the allocated data lie (an array for the data points)
 */
cord_t * lv_chart_add_dataline(lv_obj_t * chart);

/**
 * Refresh a chart if its data line has changed
 * @param chart pointer to chart object
 */
void lv_chart_refr(lv_obj_t * chart);

/**
 * Set the number of horizontal and vertical division lines
 * @param chart pointer to a graph background object
 * @param hdiv number of horizontal division lines
 * @param vdiv number of vertical division lines
 */
void lv_chart_set_hvdiv(lv_obj_t * chart, uint8_t hdiv, uint8_t vdiv);

/**
 * Set the minimal and maximal x and y values
 * @param chart pointer to a graph background object
 * @param xmin x minimum value
 * @param xmax x maximum value
 * @param ymin y minimum value
 * @param ymax y maximum value
 */
void lv_chart_set_range(lv_obj_t * chart, cord_t ymin, cord_t ymax);

/**
 * Set a new type for a chart
 * @param chart pointer to a chart object
 * @param type new type of the chart (from 'lv_chart_type_t' enum)
 */
void lv_chart_set_type(lv_obj_t * chart, lv_chart_type_t type);

/**
 * Set the number of points on a data line on a chart
 * @param chart pointer r to chart object
 * @param pnum new number of points on the data lines
 */
void lv_chart_set_pnum(lv_obj_t * chart, uint16_t pnum);

/**
 * Shift all data right and set the most right data on a data line
 * @param chart pointer to chart object
 * @param dl pointer to a data line on 'chart'
 * @param y the new value of the most right data
 */
void lv_chart_set_next(lv_obj_t * chart, cord_t * dl, cord_t y);

/**
 * Get the type of a chart
 * @param chart pointer to chart object
 * @return type of the chart (from 'lv_chart_t' enum)
 */
lv_chart_type_t lv_chart_get_type(lv_obj_t * chart);

/**
 * Get the data point number per data line on chart
 * @param chart pointer to chart object
 * @return point number on each data line
 */
uint16_t lv_chart_get_pnum(lv_obj_t * chart);

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_charts_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_charts_t style
 */
lv_charts_t * lv_charts_get(lv_charts_builtin_t style, lv_charts_t * copy);

/**********************
 *      MACROS
 **********************/

#endif

#endif
