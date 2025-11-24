/**
 * @file lv_polarchart.h
 *
 */

#ifndef LV_POLARCHART_H
#define LV_POLARCHART_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#include "../../core/lv_obj.h"

#if LV_USE_POLARCHART != 0

/*********************
 *      DEFINES
 *********************/

/**Default value of points. Can be used to not draw a point*/
#define LV_POLARCHART_POINT_NONE     (INT32_MAX)
LV_EXPORT_CONST_INT(LV_POLARCHART_POINT_NONE);

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Chart types
 */
typedef enum {
    LV_POLARCHART_TYPE_NONE,     /**< Don't draw the series*/
    LV_POLARCHART_TYPE_LINE,     /**< Connect the points with lines*/
} lv_polarchart_type_t;

/**
 * Chart update mode for `lv_polarchart_set_next`
 */
typedef enum {
    LV_POLARCHART_UPDATE_MODE_SHIFT,     /**< Shift old data to the left and add the new one the right*/
    LV_POLARCHART_UPDATE_MODE_CIRCULAR,  /**< Add the new data in a circular way*/
} lv_polarchart_update_mode_t;

/**
 * Enumeration of the axis'
 */
typedef enum {
    LV_POLARCHART_AXIS_RADIAL    = 0x00,
    LV_POLARCHART_AXIS_ANGLE     = 0x01,
    LV_POLARCHART_AXIS_LAST
} lv_polarchart_axis_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_polarchart_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a polarchart object
 * @param parent    pointer to an object, it will be the parent of the new chart
 * @return          pointer to the created chart
 */
lv_obj_t * lv_polarchart_create(lv_obj_t * parent);

/**
 * Set a new type for a polarchart
 * @param obj       pointer to a polarchart object
 * @param type      new type of the chart (from 'lv_chart_type_t' enum)
 */
void lv_polarchart_set_type(lv_obj_t * obj, lv_polarchart_type_t type);

/**
 * Set the number of points on a data line on a polarchart
 * @param obj       pointer to a polarchart object
 * @param cnt       new number of points on the data lines
 */
void lv_polarchart_set_point_count(lv_obj_t * obj, uint32_t cnt);

/**
 * Set the minimal and maximal y values on an axis
 * @param obj       pointer to a polarchart object
 * @param axis      `LV_POLARCHART_AXIS_RADIAL`
 * @param min       minimum value of the y axis
 * @param max       maximum value of the y axis
 */
void lv_polarchart_set_axis_range(lv_obj_t * obj, lv_polarchart_axis_t axis, int32_t min, int32_t max);

/**
 * Set the minimal values on an axis
 * @param obj       pointer to a polarchart object
 * @param axis      `LV_POLARCHART_AXIS_RADIAL`
 * @param min       minimal value of the y axis
 */
void lv_polarchart_set_axis_min_value(lv_obj_t * obj, lv_polarchart_axis_t axis, int32_t min);

/**
 * Set the maximal y values on an axis
 * @param obj       pointer to a polarchart object
 * @param axis      `LV_POLARCHART_AXIS_RADIAL`
 * @param max       maximum value of the y axis
 */
void lv_polarchart_set_axis_max_value(lv_obj_t * obj, lv_polarchart_axis_t axis, int32_t max);


/**
 * Set update mode of the chart object. Affects
 * @param obj              pointer to a chart object
 * @param update_mode      the update mode
 */
void lv_polarchart_set_update_mode(lv_obj_t * obj, lv_polarchart_update_mode_t update_mode);

/**
 * Set the number of angle and radial division lines
 * @param obj           pointer to a polarchart object
 * @param angle_div     number of angle division lines
 * @param radial_div    number of radial division lines
 */
void lv_polarchart_set_div_line_count(lv_obj_t * obj, uint32_t angle_div, uint32_t radial_div);

/**
 * Set the number of radial division lines
 * @param obj       pointer to a polarchart object
 * @param cnt       number of radial division lines
 */
void lv_polarchart_set_radial_div_line_count(lv_obj_t * obj, uint32_t cnt);

/**
 * Set the number of angle division lines
 * @param obj       pointer to a polarchart object
 * @param cnt       number of angle division lines
 */
void lv_polarchart_set_angle_div_line_count(lv_obj_t * obj, uint32_t cnt);

/**
 * Get the type of a chart
 * @param obj       pointer to polarchart object
 * @return          type of the polarchart (from 'lv_polarchart_t' enum)
 */
lv_polarchart_type_t lv_polarchart_get_type(const lv_obj_t * obj);

/**
 * Get the data point number per data line on polarchart
 * @param obj       pointer to polarchart object
 * @return          point number on each data line
 */
uint32_t lv_polarchart_get_point_count(const lv_obj_t * obj);

/**
 * Get the current index of the angle-axis start point in the data array
 * @param obj       pointer to a polarchart object
 * @param ser       pointer to a data series on polarchart
 * @return          the index of the current angle start point in the data array
 */
uint32_t lv_polarchart_get_angle_start_point(const lv_obj_t * obj, lv_polarchart_series_t * ser);

/**
 * Get the position of a point on the polarchart.
 * @param obj       pointer to a polarchart object
 * @param ser       pointer to series
 * @param id        the index.
 * @param p_out     store the result position here
 */
void lv_polarchart_get_point_pos_by_id(lv_obj_t * obj, lv_polarchart_series_t * ser, uint32_t id, lv_point_t * p_out);

/**
 * Refresh a chart if its data line has changed
 * @param   obj   pointer to chart object
 */
void lv_polarchart_refresh(lv_obj_t * obj);

/*======================
 * Series
 *=====================*/

/**
 * Allocate and add a data series to the chart
 * @param obj       pointer to a chart object
 * @param color     color of the data series
 * @param axis      the y axis to which the series should be attached (::LV_CHART_AXIS_PRIMARY_RADIAL)
 * @return          pointer to the allocated data series or NULL on failure
 */
lv_polarchart_series_t * lv_polarchart_add_series(lv_obj_t * obj, lv_color_t color, lv_polarchart_axis_t axis);

/**
 * Deallocate and remove a data series from a chart
 * @param obj       pointer to a chart object
 * @param series    pointer to a data series on 'chart'
 */
void lv_polarchart_remove_series(lv_obj_t * obj, lv_polarchart_series_t * series);

/**
 * Hide/Unhide a single series of a chart.
 * @param chart     pointer to a chart object.
 * @param series    pointer to a series object
 * @param hide      true: hide the series
 */
void lv_polarchart_hide_series(lv_obj_t * chart, lv_polarchart_series_t * series, bool hide);

/**
 * Change the color of a series
 * @param chart     pointer to a chart object.
 * @param series    pointer to a series object
 * @param color     the new color of the series
 */
void lv_polarchart_set_series_color(lv_obj_t * chart, lv_polarchart_series_t * series, lv_color_t color);

/**
 * Get the color of a series
 * @param chart     pointer to a chart object.
 * @param series    pointer to a series object
 * @return          the color of the series
 */
lv_color_t lv_polarchart_get_series_color(lv_obj_t * chart, const lv_polarchart_series_t * series);

/**
 * Set the index of the x-axis start point in the data array.
 * This point will be considers the first (left) point and the other points will be drawn after it.
 * @param obj       pointer to a chart object
 * @param ser       pointer to a data series on 'chart'
 * @param id        the index of the x point in the data array
 */
void lv_polarchart_set_x_start_point(lv_obj_t * obj, lv_polarchart_series_t * ser, uint32_t id);

/**
 * Get the next series.
 * @param chart     pointer to a chart
 * @param ser      the previous series or NULL to get the first
 * @return          the next series or NULL if there is no more.
 */
lv_polarchart_series_t * lv_polarchart_get_series_next(const lv_obj_t * chart, const lv_polarchart_series_t * ser);

/*=====================
 * Cursor
 *====================*/

/**
 * Add a cursor with a given color
 * @param obj       pointer to chart object
 * @param color     color of the cursor
 * @param dir       direction of the cursor. `LV_DIR_RIGHT/LEFT/TOP/DOWN/HOR/VER/ALL`. OR-ed values are possible
 * @return          pointer to the created cursor
 */
lv_polarchart_cursor_t  * lv_polarchart_add_cursor(lv_obj_t * obj, lv_color_t color, lv_dir_t dir);

/**
 * Remove a cursor
 * @param obj       pointer to chart object
 * @param cursor    pointer to the cursor
 */
void lv_polarchart_remove_cursor(lv_obj_t * obj, lv_polarchart_cursor_t * cursor);

/**
 * Set the coordinate of the cursor with respect to the paddings
 * @param chart     pointer to a chart object
 * @param cursor    pointer to the cursor
 * @param pos       the new coordinate of cursor relative to the chart
 */
void lv_polarchart_set_cursor_pos(lv_obj_t * chart, lv_polarchart_cursor_t * cursor, lv_point_t * pos);

/**
 * Set the 'angle' coordinate of the cursor with respect to the paddings
 * @param chart     pointer to a polarchart object
 * @param cursor    pointer to the cursor
 * @param angle     the new 'angle' coordinate of cursor relative to the chart
 */
void lv_polarchart_set_cursor_pos_angle(lv_obj_t * chart, lv_polarchart_cursor_t * cursor, int32_t angle);

/**
 * Set the coordinate of the cursor with respect to the paddings
 * @param chart     pointer to a polarchart object
 * @param cursor    pointer to the cursor
 * @param y         the new 'radial' coordinate of cursor relative to the chart
 */
void lv_polarchart_set_cursor_pos_radial(lv_obj_t * chart, lv_polarchart_cursor_t * cursor, int32_t radial);

/**
 * Stick the cursor to a point
 * @param chart     pointer to a chart object
 * @param cursor    pointer to the cursor
 * @param ser       pointer to a series
 * @param point_id  the point's index or `LV_CHART_POINT_NONE` to not assign to any points.
 */
void lv_polarchart_set_cursor_point(lv_obj_t * chart, lv_polarchart_cursor_t * cursor, lv_polarchart_series_t * ser,
                                    uint32_t point_id);

/**
 * Get the coordinate of the cursor with respect to the paddings
 * @param chart     pointer to a chart object
 * @param cursor    pointer to cursor
 * @return          coordinate of the cursor as lv_point_t
 */
lv_point_t lv_polarchart_get_cursor_point(lv_obj_t * chart, lv_polarchart_cursor_t * cursor);

/*=====================
 * Set/Get value(s)
 *====================*/

/**
 * Initialize all data points of a series with a value
 * @param obj       pointer to chart object
 * @param ser       pointer to a data series on 'chart'
 * @param value     the new value for all points. `LV_CHART_POINT_NONE` can be used to hide the points.
 */
void lv_polarchart_set_all_values(lv_obj_t * obj, lv_polarchart_series_t * ser, int32_t value);

/**
 * Set the next point's Y value according to the update mode policy.
 * @param obj       pointer to chart object
 * @param ser       pointer to a data series on 'chart'
 * @param value     the new value of the next data
 */
void lv_polarchart_set_next_value(lv_obj_t * obj, lv_polarchart_series_t * ser, int32_t value);

/**
 * Same as `lv_chart_set_next_value` but set the values from an array
 * @param obj           pointer to chart object
 * @param ser           pointer to a data series on 'chart'
 * @param values        the new values to set
 * @param values_cnt    number of items in `values`
 */
void lv_polarchart_set_series_values(lv_obj_t * obj, lv_polarchart_series_t * ser, const int32_t values[],
                                     size_t values_cnt);

/**
 * Set an individual point's y value of a chart's series directly based on its index
 * @param obj     pointer to a chart object
 * @param ser     pointer to a data series on 'chart'
 * @param id      the index of the x point in the array
 * @param value   value to assign to array point
 */
void lv_polarchart_set_series_value_by_id(lv_obj_t * obj, lv_polarchart_series_t * ser, uint32_t id, int32_t value);

/**
 * Set an external array for the 'radial' data points to use for the polarchart
 * NOTE: It is the users responsibility to make sure the `point_cnt` matches the external array size.
 * @param obj       pointer to a polarchart object
 * @param ser       pointer to a data series on polarchart
 * @param array     external array of points for polarchart
 */
void lv_polarchart_set_series_ext_radial_array(lv_obj_t * obj, lv_polarchart_series_t * ser, int32_t array[]);

/**
 * Set an external array for the 'angle' data points to use for the polarchart
 * NOTE: It is the users responsibility to make sure the `point_cnt` matches the external array size.
 * @param obj       pointer to a polarchart object
 * @param ser       pointer to a data series on polarchart
 * @param array     external array of points for polarchart
 */
void lv_polarchart_set_series_ext_angle_array(lv_obj_t * obj, lv_polarchart_series_t * ser, int32_t array[]);

/**
 * Get the array of 'radial' values of a series
 * @param obj   pointer to a polarchart object
 * @param ser   pointer to a data series on polarchart
 * @return      the array of values with 'point_count' elements
 */
int32_t * lv_polarchart_get_series_radial_array(const lv_obj_t * obj, lv_polarchart_series_t * ser);

/**
 * Get the array of 'angle' values of a series
 * @param obj   pointer to a polarchart object
 * @param ser   pointer to a data series on polarchart
 * @return      the array of values with 'point_count' elements
 */
int32_t * lv_polarchart_get_series_angle_array(const lv_obj_t * obj, lv_polarchart_series_t * ser);

/**
 * Get the index of the currently pressed point. It's the same for every series.
 * @param obj       pointer to a chart object
 * @return          the index of the point [0 .. point count] or LV_CHART_POINT_ID_NONE if no point is being pressed
 */
uint32_t lv_polarchart_get_pressed_point(const lv_obj_t * obj);

/**
 * Get the overall offset from the chart's side to the center of the first point.
 * In case of a bar chart it will be the center of the first column group
 * @param obj       pointer to a chart object
 * @return          the offset of the center
 */
int32_t lv_polarchart_get_first_point_center_offset(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_POLARCHART*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_POLARCHART_H*/
