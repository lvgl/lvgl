/**
 * @file lv_chart.h
 *
 */

#ifndef LV_CHART_H
#define LV_CHART_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_CHART != 0

#include "../lv_obj/lv_obj.h"
#include "lv_line.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    cord_t * points;
    color_t color;
}lv_chart_dl_t;

/*Data of chart */
typedef struct
{
    /*No inherited ext*/ /*Ext. of ancestor*/
    /*New data for this type */
    ll_dsc_t dl_ll;       /*Linked list for the data line pointers (stores lv_chart_dl_t)*/
    cord_t ymin;          /*y min value (used to scale the data)*/
    cord_t ymax;          /*y max value (used to scale the data)*/
    uint8_t hdiv_num;     /*Number of horizontal division lines*/
    uint8_t vdiv_num;     /*Number of vertical division lines*/
    uint16_t pnum;        /*Point number in a data line*/
    cord_t dl_width;      /*Line width or point radius*/
    uint8_t dl_num;       /*Number of data lines in dl_ll*/
    opa_t dl_opa;       /*Opacity of data lines*/
    opa_t dl_dark;       /*Dark level of the point/column bottoms*/
    uint8_t type    :3;   /*Line, column or point chart (from 'lv_chart_type_t')*/
}lv_chart_ext_t;

/*Chart types*/
typedef enum
{
    LV_CHART_NONE = 0,
	LV_CHART_LINE = 0x01,
	LV_CHART_COL = 0x02,
	LV_CHART_POINT = 0x04,
}lv_chart_type_t;


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
 * @param color color of the data line
 * @return pointer to the allocated data line (
 */
lv_chart_dl_t * lv_chart_add_data_line(lv_obj_t * chart, color_t color);

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
 * Set the opacity of the data lines
 * @param chart pointer to chart object
 * @param opa opacity of the data lines
 */
void lv_chart_set_dl_opa(lv_obj_t * chart, opa_t opa);

/**
 * Set the line width or point radius of the data lines
 * @param chart pointer to chart object
 * @param width the new width
 */
void lv_chart_set_dl_width(lv_obj_t * chart, cord_t width);

/**
 * Set the dark effect on the bottom of the points or columns
 * @param chart pointer to chart object
 * @param dark_eff dark effect level (OPA_TRANSP to turn off)
 */
void lv_chart_set_dl_dark(lv_obj_t * chart, opa_t dark_eff);

/**
 * Shift all data right and set the most right data on a data line
 * @param chart pointer to chart object
 * @param dl pointer to a data line on 'chart'
 * @param y the new value of the most right data
 */
void lv_chart_set_next(lv_obj_t * chart, lv_chart_dl_t * dl, cord_t y);

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
 * Get the opacity of the data lines
 * @param chart pointer to chart object
 * @return the opacity of the data lines
 */
opa_t lv_chart_get_dl_opa(lv_obj_t * chart);

/**
 * Get the data line width
 * @param chart pointer to chart object
 * @return the width the data lines (lines or points)
 */
cord_t lv_chart_get_dl_width(lv_obj_t * chart);

/**
 * Get the dark effect level on the bottom of the points or columns
 * @param chart pointer to chart object
 * @return dark effect level (OPA_TRANSP to turn off)
 */
opa_t lv_chart_get_dl_dark(lv_obj_t * chart, opa_t dark_eff);

/**********************
 *      MACROS
 **********************/

#endif

#endif
