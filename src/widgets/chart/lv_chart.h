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
#include "../../lv_conf_internal.h"
#include "../../core/lv_obj.h"

#if LV_USE_CHART != 0

/*********************
 *      DEFINES
 *********************/

/**Default value of points. Can be used to not draw a point*/
#define LV_CHART_POINT_NONE     (INT32_MAX)
LV_EXPORT_CONST_INT(LV_CHART_POINT_NONE);

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Chart types
 */
typedef enum {
    LV_CHART_TYPE_NONE,     /**< Don't draw the series*/
    LV_CHART_TYPE_LINE,     /**< Connect the points with lines*/
    LV_CHART_TYPE_BAR,      /**< Draw columns*/
    LV_CHART_TYPE_SCATTER,  /**< Draw points and lines in 2D (x,y coordinates)*/
} lv_chart_type_t;

/**
 * Chart update mode for `lv_chart_set_next`
 */
typedef enum {
    LV_CHART_UPDATE_MODE_SHIFT,     /**< Shift old data to the left and add the new one the right*/
    LV_CHART_UPDATE_MODE_CIRCULAR,  /**< Add the new data in a circular way*/
} lv_chart_update_mode_t;

/**
 * Enumeration of the axis'
 */
typedef enum {
    LV_CHART_AXIS_PRIMARY_Y     = 0x00,
    LV_CHART_AXIS_SECONDARY_Y   = 0x01,
    LV_CHART_AXIS_PRIMARY_X     = 0x02,
    LV_CHART_AXIS_SECONDARY_X   = 0x04,
    LV_CHART_AXIS_LAST
} lv_chart_axis_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_chart_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a chart object
 * @param parent    pointer to an object, it will be the parent of the new chart
 * @return          pointer to the created chart
 */
lv_obj_t * lv_chart_create(lv_obj_t * parent);

/**
 * Set a new type for a chart
 * @param obj       pointer to a chart object
 * @param type      new type of the chart (from 'lv_chart_type_t' enum)
 */
void lv_chart_set_type(lv_obj_t * obj, lv_chart_type_t type);
/**
 * Set the number of points on a data line on a chart
 * @param obj       pointer to a chart object
 * @param cnt       new number of points on the data lines
 */
void lv_chart_set_point_count(lv_obj_t * obj, uint32_t cnt);

/**
 * Set the minimal and maximal y values on an axis
 * @param obj       pointer to a chart object
 * @param axis      `LV_CHART_AXIS_PRIMARY_Y` or `LV_CHART_AXIS_SECONDARY_Y`
 * @param min       minimum value of the y axis
 * @param max       maximum value of the y axis
 */
void lv_chart_set_axis_range(lv_obj_t * obj, lv_chart_axis_t axis, int32_t min, int32_t max);

/**
 * Set update mode of the chart object. Affects
 * @param obj              pointer to a chart object
 * @param update_mode      the update mode
 */
void lv_chart_set_update_mode(lv_obj_t * obj, lv_chart_update_mode_t update_mode);

/**
 * Set the number of horizontal and vertical division lines
 * @param obj       pointer to a chart object
 * @param hdiv      number of horizontal division lines
 * @param vdiv      number of vertical division lines
 */
void lv_chart_set_div_line_count(lv_obj_t * obj, uint8_t hdiv, uint8_t vdiv);

/**
 * Get the type of a chart
 * @param obj       pointer to chart object
 * @return          type of the chart (from 'lv_chart_t' enum)
 */
lv_chart_type_t lv_chart_get_type(const lv_obj_t * obj);

/**
 * Get the data point number per data line on chart
 * @param obj       pointer to chart object
 * @return          point number on each data line
 */
uint32_t lv_chart_get_point_count(const lv_obj_t * obj);

/**
 * Get the current index of the x-axis start point in the data array
 * @param obj       pointer to a chart object
 * @param ser       pointer to a data series on 'chart'
 * @return          the index of the current x start point in the data array
 */
uint32_t lv_chart_get_x_start_point(const lv_obj_t * obj, lv_chart_series_t * ser);

/**
 * Get the position of a point to the chart.
 * @param obj       pointer to a chart object
 * @param ser       pointer to series
 * @param id        the index.
 * @param p_out     store the result position here
 */
void lv_chart_get_point_pos_by_id(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t id, lv_point_t * p_out);

/**
 * Refresh a chart if its data line has changed
 * @param   obj   pointer to chart object
 */
void lv_chart_refresh(lv_obj_t * obj);

/*======================
 * Series
 *=====================*/

/**
 * Allocate and add a data series to the chart
 * @param obj       pointer to a chart object
 * @param color     color of the data series
 * @param axis      the y axis to which the series should be attached (::LV_CHART_AXIS_PRIMARY_Y or ::LV_CHART_AXIS_SECONDARY_Y)
 * @return          pointer to the allocated data series or NULL on failure
 */
lv_chart_series_t * lv_chart_add_series(lv_obj_t * obj, lv_color_t color, lv_chart_axis_t axis);

/**
 * Deallocate and remove a data series from a chart
 * @param obj       pointer to a chart object
 * @param series    pointer to a data series on 'chart'
 */
void lv_chart_remove_series(lv_obj_t * obj, lv_chart_series_t * series);

/**
 * Hide/Unhide a single series of a chart.
 * @param chart     pointer to a chart object.
 * @param series    pointer to a series object
 * @param hide      true: hide the series
 */
void lv_chart_hide_series(lv_obj_t * chart, lv_chart_series_t * series, bool hide);

/**
 * Change the color of a series
 * @param chart     pointer to a chart object.
 * @param series    pointer to a series object
 * @param color     the new color of the series
 */
void lv_chart_set_series_color(lv_obj_t * chart, lv_chart_series_t * series, lv_color_t color);

/**
 * Get the color of a series
 * @param chart     pointer to a chart object.
 * @param series    pointer to a series object
 * @return          the color of the series
 */
lv_color_t lv_chart_get_series_color(lv_obj_t * chart, const lv_chart_series_t * series);

/**
 * Set the index of the x-axis start point in the data array.
 * This point will be considers the first (left) point and the other points will be drawn after it.
 * @param obj       pointer to a chart object
 * @param ser       pointer to a data series on 'chart'
 * @param id        the index of the x point in the data array
 */
void lv_chart_set_x_start_point(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t id);

/**
 * Get the next series.
 * @param chart     pointer to a chart
 * @param ser      the previous series or NULL to get the first
 * @return          the next series or NULL if there is no more.
 */
lv_chart_series_t * lv_chart_get_series_next(const lv_obj_t * chart, const lv_chart_series_t * ser);

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
lv_chart_cursor_t  * lv_chart_add_cursor(lv_obj_t * obj, lv_color_t color, lv_dir_t dir);

/**
 * Set the coordinate of the cursor with respect to the paddings
 * @param chart     pointer to a chart object
 * @param cursor    pointer to the cursor
 * @param pos       the new coordinate of cursor relative to the chart
 */
void lv_chart_set_cursor_pos(lv_obj_t * chart, lv_chart_cursor_t * cursor, lv_point_t * pos);

/**
 * Set the X coordinate of the cursor with respect to the paddings
 * @param chart     pointer to a chart object
 * @param cursor    pointer to the cursor
 * @param x         the new X coordinate of cursor relative to the chart
 */
void lv_chart_set_cursor_pos_x(lv_obj_t * chart, lv_chart_cursor_t * cursor, int32_t x);

/**
 * Set the coordinate of the cursor with respect to the paddings
 * @param chart     pointer to a chart object
 * @param cursor    pointer to the cursor
 * @param y         the new Y coordinate of cursor relative to the chart
 */
void lv_chart_set_cursor_pos_y(lv_obj_t * chart, lv_chart_cursor_t * cursor, int32_t y);

/**
 * Stick the cursor to a point
 * @param chart     pointer to a chart object
 * @param cursor    pointer to the cursor
 * @param ser       pointer to a series
 * @param point_id  the point's index or `LV_CHART_POINT_NONE` to not assign to any points.
 */
void lv_chart_set_cursor_point(lv_obj_t * chart, lv_chart_cursor_t * cursor, lv_chart_series_t * ser,
                               uint32_t point_id);

/**
 * Get the coordinate of the cursor with respect to the paddings
 * @param chart     pointer to a chart object
 * @param cursor    pointer to cursor
 * @return          coordinate of the cursor as lv_point_t
 */
lv_point_t lv_chart_get_cursor_point(lv_obj_t * chart, lv_chart_cursor_t * cursor);

/*=====================
 * Set/Get value(s)
 *====================*/

/**
 * Initialize all data points of a series with a value
 * @param obj       pointer to chart object
 * @param ser       pointer to a data series on 'chart'
 * @param value     the new value for all points. `LV_CHART_POINT_NONE` can be used to hide the points.
 */
void lv_chart_set_all_values(lv_obj_t * obj, lv_chart_series_t * ser, int32_t value);

/**
 * Set the next point's Y value according to the update mode policy.
 * @param obj       pointer to chart object
 * @param ser       pointer to a data series on 'chart'
 * @param value     the new value of the next data
 */
void lv_chart_set_next_value(lv_obj_t * obj, lv_chart_series_t * ser, int32_t value);

/**
 * Set the next point's X and Y value according to the update mode policy.
 * @param obj       pointer to chart object
 * @param ser       pointer to a data series on 'chart'
 * @param x_value   the new X value of the next data
 * @param y_value   the new Y value of the next data
 */
void lv_chart_set_next_value2(lv_obj_t * obj, lv_chart_series_t * ser, int32_t x_value, int32_t y_value);

/**
 * Same as `lv_chart_set_next_value` but set the values from an array
 * @param obj           pointer to chart object
 * @param ser           pointer to a data series on 'chart'
 * @param values        the new values to set
 * @param values_cnt    number of items in `values`
 */
void lv_chart_set_series_values(lv_obj_t * obj, lv_chart_series_t * ser, const int32_t values[], size_t values_cnt);

/**
 * Same as `lv_chart_set_next_value2` but set the values from an array
 * @param obj           pointer to chart object
 * @param ser           pointer to a data series on 'chart'
 * @param x_values      the new values to set on the X axis
 * @param y_values      the new values to set o nthe Y axis
 * @param values_cnt    number of items in `x_values` and `y_values`
 */
void lv_chart_set_series_values2(lv_obj_t * obj, lv_chart_series_t * ser, const int32_t x_values[],
                                 const int32_t y_values[], size_t values_cnt);

/**
 * Set an individual point's y value of a chart's series directly based on its index
 * @param obj     pointer to a chart object
 * @param ser     pointer to a data series on 'chart'
 * @param id      the index of the x point in the array
 * @param value   value to assign to array point
 */
void lv_chart_set_series_value_by_id(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t id, int32_t value);

/**
 * Set an individual point's x and y value of a chart's series directly based on its index
 * Can be used only with `LV_CHART_TYPE_SCATTER`.
 * @param obj       pointer to chart object
 * @param ser       pointer to a data series on 'chart'
 * @param id        the index of the x point in the array
 * @param x_value   the new X value of the next data
 * @param y_value   the new Y value of the next data
 */
void lv_chart_set_series_value_by_id2(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t id, int32_t x_value,
                                      int32_t y_value);

/**
 * Set an external array for the y data points to use for the chart
 * NOTE: It is the users responsibility to make sure the `point_cnt` matches the external array size.
 * @param obj       pointer to a chart object
 * @param ser       pointer to a data series on 'chart'
 * @param array     external array of points for chart
 */
void lv_chart_set_series_ext_y_array(lv_obj_t * obj, lv_chart_series_t * ser, int32_t array[]);

/**
 * Set an external array for the x data points to use for the chart
 * NOTE: It is the users responsibility to make sure the `point_cnt` matches the external array size.
 * @param obj       pointer to a chart object
 * @param ser       pointer to a data series on 'chart'
 * @param array     external array of points for chart
 */
void lv_chart_set_series_ext_x_array(lv_obj_t * obj, lv_chart_series_t * ser, int32_t array[]);

/**
 * Get the array of y values of a series
 * @param obj   pointer to a chart object
 * @param ser   pointer to a data series on 'chart'
 * @return      the array of values with 'point_count' elements
 */
int32_t * lv_chart_get_series_y_array(const lv_obj_t * obj, lv_chart_series_t * ser);

/**
 * Get the array of x values of a series
 * @param obj   pointer to a chart object
 * @param ser   pointer to a data series on 'chart'
 * @return      the array of values with 'point_count' elements
 */
int32_t * lv_chart_get_series_x_array(const lv_obj_t * obj, lv_chart_series_t * ser);

/**
 * Get the index of the currently pressed point. It's the same for every series.
 * @param obj       pointer to a chart object
 * @return          the index of the point [0 .. point count] or LV_CHART_POINT_ID_NONE if no point is being pressed
 */
uint32_t lv_chart_get_pressed_point(const lv_obj_t * obj);

/**
 * Get the overall offset from the chart's side to the center of the first point.
 * In case of a bar chart it will be the center of the first column group
 * @param obj       pointer to a chart object
 * @return          the offset of the center
 */
int32_t lv_chart_get_first_point_center_offset(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CHART*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CHART_H*/
