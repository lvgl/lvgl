/**
 * @file lv_chart.h
 *
 */

#ifndef LV_CHART_H
#define LV_CHART_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#if LV_USE_CHART != 0

#include "../lv_core/lv_obj.h"
#include "lv_line.h"

/*********************
 *      DEFINES
 *********************/

/**Default value of points. Can be used to not draw a point*/
#define LV_CHART_POINT_DEF (LV_COORD_MIN)

/**Automatically calculate the tick length*/
#define LV_CHART_TICK_LENGTH_AUTO 255

/**********************
 *      TYPEDEFS
 **********************/

/*Chart types*/
enum {
    LV_CHART_TYPE_NONE          = 0x00, /*Don't draw the series*/
    LV_CHART_TYPE_LINE          = 0x01, /*Connect the points with lines*/
    LV_CHART_TYPE_COLUMN        = 0x02, /*Draw columns*/
    LV_CHART_TYPE_POINT         = 0x04, /*Draw circles on the points*/
    LV_CHART_TYPE_VERTICAL_LINE = 0x08, /*Draw vertical lines on points (useful when chart width == point count)*/
    LV_CHART_TYPE_AREA          = 0x10, /*Draw area chart*/
};
typedef uint8_t lv_chart_type_t;

/*Chart update mode for `lv_chart_set_next`*/
enum {
    LV_CHART_UPDATE_MODE_SHIFT,    /*Shift old data to the left and add the new one o the right*/
    LV_CHART_UPDATE_MODE_CIRCULAR, /*Add the new data in a circular way*/
};
typedef uint8_t lv_chart_update_mode_t;

typedef struct
{
    lv_coord_t * points;
    lv_color_t color;
    uint16_t start_point;
} lv_chart_series_t;

/*Data of axis */
enum {
    LV_CHART_AXIS_SKIP_LAST_TICK = 0x00, /* don't draw the last tick */
    LV_CHART_AXIS_DRAW_LAST_TICK = 0x01  /* draw the last tick */
};
typedef uint8_t lv_chart_axis_options_t;

typedef struct
{
    const char * list_of_values;
    lv_chart_axis_options_t options;
    uint8_t num_tick_marks;
    uint8_t major_tick_len;
    uint8_t minor_tick_len;
} lv_chart_axis_cfg_t;

/*Data of chart */
typedef struct
{
    /*No inherited ext*/ /*Ext. of ancestor*/
    /*New data for this type */
    lv_ll_t series_ll;    /*Linked list for the data line pointers (stores lv_chart_dl_t)*/
    lv_coord_t ymin;      /*y min value (used to scale the data)*/
    lv_coord_t ymax;      /*y max value (used to scale the data)*/
    uint8_t hdiv_cnt;     /*Number of horizontal division lines*/
    uint8_t vdiv_cnt;     /*Number of vertical division lines*/
    uint16_t point_cnt;   /*Point number in a data line*/
    lv_chart_type_t type; /*Line, column or point chart (from 'lv_chart_type_t')*/
    lv_chart_axis_cfg_t y_axis;
    lv_chart_axis_cfg_t x_axis;
    uint16_t margin;
    uint8_t update_mode : 1;
    struct
    {
        lv_coord_t width; /*Line width or point radius*/
        uint8_t num;      /*Number of data lines in dl_ll*/
        lv_opa_t opa;     /*Opacity of data lines*/
        lv_opa_t dark;    /*Dark level of the point/column bottoms*/
    } series;
} lv_chart_ext_t;

enum {
    LV_CHART_STYLE_MAIN,
};
typedef uint8_t lv_chart_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a chart background objects
 * @param par pointer to an object, it will be the parent of the new chart background
 * @param copy pointer to a chart background object, if not NULL then the new object will be copied
 * from it
 * @return pointer to the created chart background
 */
lv_obj_t * lv_chart_create(lv_obj_t * par, const lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Allocate and add a data series to the chart
 * @param chart pointer to a chart object
 * @param color color of the data series
 * @return pointer to the allocated data series
 */
lv_chart_series_t * lv_chart_add_series(lv_obj_t * chart, lv_color_t color);

/**
 * Clear the point of a serie
 * @param chart pointer to a chart object
 * @param serie pointer to the chart's serie to clear
 */
void lv_chart_clear_serie(lv_obj_t * chart, lv_chart_series_t * serie);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of horizontal and vertical division lines
 * @param chart pointer to a graph background object
 * @param hdiv number of horizontal division lines
 * @param vdiv number of vertical division lines
 */
void lv_chart_set_div_line_count(lv_obj_t * chart, uint8_t hdiv, uint8_t vdiv);

/**
 * Set the minimal and maximal y values
 * @param chart pointer to a graph background object
 * @param ymin y minimum value
 * @param ymax y maximum value
 */
void lv_chart_set_range(lv_obj_t * chart, lv_coord_t ymin, lv_coord_t ymax);

/**
 * Set a new type for a chart
 * @param chart pointer to a chart object
 * @param type new type of the chart (from 'lv_chart_type_t' enum)
 */
void lv_chart_set_type(lv_obj_t * chart, lv_chart_type_t type);

/**
 * Set the number of points on a data line on a chart
 * @param chart pointer r to chart object
 * @param point_cnt new number of points on the data lines
 */
void lv_chart_set_point_count(lv_obj_t * chart, uint16_t point_cnt);

/**
 * Set the opacity of the data series
 * @param chart pointer to a chart object
 * @param opa opacity of the data series
 */
void lv_chart_set_series_opa(lv_obj_t * chart, lv_opa_t opa);

/**
 * Set the line width or point radius of the data series
 * @param chart pointer to a chart object
 * @param width the new width
 */
void lv_chart_set_series_width(lv_obj_t * chart, lv_coord_t width);

/**
 * Set the dark effect on the bottom of the points or columns
 * @param chart pointer to a chart object
 * @param dark_eff dark effect level (LV_OPA_TRANSP to turn off)
 */
void lv_chart_set_series_darking(lv_obj_t * chart, lv_opa_t dark_eff);

/**
 * Initialize all data points with a value
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y the new value  for all points
 */
void lv_chart_init_points(lv_obj_t * chart, lv_chart_series_t * ser, lv_coord_t y);

/**
 * Set the value of points from an array
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y_array array of 'lv_coord_t' points (with 'points count' elements )
 */
void lv_chart_set_points(lv_obj_t * chart, lv_chart_series_t * ser, lv_coord_t y_array[]);

/**
 * Shift all data right and set the most right data on a data line
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y the new value of the most right data
 */
void lv_chart_set_next(lv_obj_t * chart, lv_chart_series_t * ser, lv_coord_t y);

/**
 * Set update mode of the chart object.
 * @param chart pointer to a chart object
 * @param update mode
 */
void lv_chart_set_update_mode(lv_obj_t * chart, lv_chart_update_mode_t update_mode);

/**
 * Set the style of a chart
 * @param chart pointer to a chart object
 * @param type which style should be set (can be only `LV_CHART_STYLE_MAIN`)
 * @param style pointer to a style
 */
static inline void lv_chart_set_style(lv_obj_t * chart, lv_chart_style_t type, const lv_style_t * style)
{
    (void)type; /*Unused*/
    lv_obj_set_style(chart, style);
}

/**
 * Set the length of the tick marks on the x axis
 * @param chart pointer to the chart
 * @param major_tick_len the length of the major tick or `LV_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where labels are added)
 * @param minor_tick_len the length of the minor tick, `LV_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where no labels are added)
 */
void lv_chart_set_x_tick_length(lv_obj_t * chart, uint8_t major_tick_len, uint8_t minor_tick_len);

/**
 * Set the length of the tick marks on the y axis
 * @param chart pointer to the chart
 * @param major_tick_len the length of the major tick or `LV_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where labels are added)
 * @param minor_tick_len the length of the minor tick, `LV_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where no labels are added)
 */
void lv_chart_set_y_tick_length(lv_obj_t * chart, uint8_t major_tick_len, uint8_t minor_tick_len);

/**
 * Set the x-axis tick count and labels of a chart
 * @param chart             pointer to a chart object
 * @param list_of_values    list of string values, terminated with \n, except the last
 * @param num_tick_marks    if list_of_values is NULL: total number of ticks per axis
 *                          else number of ticks between two value labels
 * @param options           extra options
 */
void lv_chart_set_x_tick_texts(lv_obj_t * chart, const char * list_of_values, uint8_t num_tick_marks,
                               lv_chart_axis_options_t options);

/**
 * Set the y-axis tick count and labels of a chart
 * @param chart             pointer to a chart object
 * @param list_of_values    list of string values, terminated with \n, except the last
 * @param num_tick_marks    if list_of_values is NULL: total number of ticks per axis
 *                          else number of ticks between two value labels
 * @param options           extra options
 */
void lv_chart_set_y_tick_texts(lv_obj_t * chart, const char * list_of_values, uint8_t num_tick_marks,
                               lv_chart_axis_options_t options);

/**
 * Set the margin around the chart, used for axes value and ticks
 * @param chart     pointer to an chart object
 * @param margin    value of the margin [px]
 */
void lv_chart_set_margin(lv_obj_t * chart, uint16_t margin);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the type of a chart
 * @param chart pointer to chart object
 * @return type of the chart (from 'lv_chart_t' enum)
 */
lv_chart_type_t lv_chart_get_type(const lv_obj_t * chart);

/**
 * Get the data point number per data line on chart
 * @param chart pointer to chart object
 * @return point number on each data line
 */
uint16_t lv_chart_get_point_cnt(const lv_obj_t * chart);

/**
 * Get the opacity of the data series
 * @param chart pointer to chart object
 * @return the opacity of the data series
 */
lv_opa_t lv_chart_get_series_opa(const lv_obj_t * chart);

/**
 * Get the data series width
 * @param chart pointer to chart object
 * @return the width the data series (lines or points)
 */
lv_coord_t lv_chart_get_series_width(const lv_obj_t * chart);

/**
 * Get the dark effect level on the bottom of the points or columns
 * @param chart pointer to chart object
 * @return dark effect level (LV_OPA_TRANSP to turn off)
 */
lv_opa_t lv_chart_get_series_darking(const lv_obj_t * chart);

/**
 * Get the style of an chart object
 * @param chart pointer to an chart object
 * @param type which style should be get (can be only `LV_CHART_STYLE_MAIN`)
 * @return pointer to the chart's style
 */
static inline const lv_style_t * lv_chart_get_style(const lv_obj_t * chart, lv_chart_style_t type)
{
    (void)type; /*Unused*/
    return lv_obj_get_style(chart);
}

/**
 * Get the margin around the chart, used for axes value and labels
 * @param chart pointer to an chart object
 * @param return value of the margin
 */
uint16_t lv_chart_get_margin(lv_obj_t * chart);

/*=====================
 * Other functions
 *====================*/

/**
 * Refresh a chart if its data line has changed
 * @param chart pointer to chart object
 */
void lv_chart_refresh(lv_obj_t * chart);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CHART*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_CHART_H*/
