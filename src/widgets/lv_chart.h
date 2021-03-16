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
#include "../lv_conf_internal.h"

#if LV_USE_CHART != 0

#include "../core/lv_obj.h"
#include "lv_line.h"

/*********************
 *      DEFINES
 *********************/

/**Default value of points. Can be used to not draw a point*/
#define LV_CHART_POINT_NONE (INT16_MAX)
LV_EXPORT_CONST_INT(LV_CHART_POINT_NONE);

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Chart types
 */
enum {
    LV_CHART_TYPE_NONE     = 0x00, /**< Don't draw the series*/
    LV_CHART_TYPE_LINE     = 0x01, /**< Connect the points with lines*/
    LV_CHART_TYPE_BAR   = 0x02, /**< Draw columns*/
};
typedef uint8_t lv_chart_type_t;

/**
 * Chart update mode for `lv_chart_set_next`
 */
enum {
    LV_CHART_UPDATE_MODE_SHIFT,     /**< Shift old data to the left and add the new one the right*/
    LV_CHART_UPDATE_MODE_CIRCULAR,  /**< Add the new data in a circular way*/
};
typedef uint8_t lv_chart_update_mode_t;

/**
 * Enumeration of the axis'
 */
enum {
    LV_CHART_AXIS_PRIMARY_Y,    /*Y axis should be the first to allow indexing arrays with the values*/
    LV_CHART_AXIS_SECONDARY_Y,
    LV_CHART_AXIS_X,
    _LV_CHART_AXIS_LAST
};
typedef uint8_t lv_chart_axis_t;

/**
 * Descriptor a chart series
 */
typedef struct {
    lv_coord_t * points;
    lv_color_t color;
    uint16_t last_point;
    uint8_t hidden : 1;
    uint8_t ext_buf_assigned : 1;
    lv_chart_axis_t y_axis  : 2;
} lv_chart_series_t;

typedef struct {
    lv_point_t point;
    lv_color_t color;
    lv_dir_t dir;
} lv_chart_cursor_t;

typedef struct {
    lv_coord_t major_len;
    lv_coord_t minor_len;
    lv_coord_t draw_size;
    uint32_t minor_cnt :15;
    uint32_t major_cnt :15;
    uint32_t label_en  :1;
}lv_chart_tick_dsc_t;

typedef struct {
    lv_obj_t obj;
    lv_ll_t series_ll;     /**< Linked list for the series (stores lv_chart_series_t)*/
    lv_ll_t cursor_ll;     /**< Linked list for the cursors (stores lv_chart_cursor_t)*/
    lv_chart_tick_dsc_t tick[_LV_CHART_AXIS_LAST];
    lv_coord_t ymin[2];
    lv_coord_t ymax[2];
    uint16_t pressed_point_id;
    uint16_t hdiv_cnt;      /**< Number of horizontal division lines*/
    uint16_t vdiv_cnt;      /**< Number of vertical division lines*/
    uint16_t point_cnt;    /**< Point number in a data line*/
    uint16_t zoom_x;
    uint16_t zoom_y;
    lv_chart_type_t type :2; /**< Line or column chart */
    lv_chart_update_mode_t update_mode : 1;
}lv_chart_t;

extern const lv_obj_class_t lv_chart_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a chart objects
 * @param parent    pointer to an object, it will be the parent of the new button
 * @param copy      DEPRECATED, will be removed in v9.
 *                  Pointer to an other chart to copy.
 * @return          pointer to the created chart
 */
lv_obj_t * lv_chart_create(lv_obj_t * parent, const lv_obj_t * copy);

/**
 * Set a new type for a chart
 * @param obj       pointer to a chart object
 * @param type      new type of the chart (from 'lv_chart_type_t' enum)
 */
void lv_chart_set_type(lv_obj_t * obj, lv_chart_type_t type);
/**
 * Set the number of points on a data line on a chart
 * @param obj       pointer r to chart object
 * @param cnt       new number of points on the data lines
 */
void lv_chart_set_point_count(lv_obj_t * obj, uint16_t cnt);

/**
 * Set the minimal and maximal y values on an axis
 * @param obj       pointer to a chart object
 * @param axis      `LV_CHART_AXIS_PRIMARY_Y` or `LV_CHART_AXIS_SECONDARY_Y`
 * @param min       minimum value of the y axis
 * @param max       maximum value of the y axis
 */
void lv_chart_set_range(lv_obj_t * obj, lv_chart_axis_t axis, lv_coord_t min, lv_coord_t max);

/**
 * Set update mode of the chart object. Affects
 * @param obj       pointer to a chart object
 * @param mode      the update mode
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
 * Zoom into the chart in X direction
 * @param obj       pointer to a chart object
 * @param zoom_x    zoom in x direction. LV_ZOOM_NONE or 256 for no zoom, 512 double zoom
 */
void lv_chart_set_zoom_x(lv_obj_t * obj, uint16_t zoom_x);

/**
 * Zoom into the chart in Y direction
 * @param obj       pointer to a chart object
 * @param zoom_y    zoom in y direction. LV_ZOOM_NONE or 256 for no zoom, 512 double zoom
 */
void lv_chart_set_zoom_y(lv_obj_t * obj, uint16_t zoom_y);

/**
 * Get X zoom of a chart
 * @param obj       pointer to a chart object
 * @return          the X zoom value
 */
uint16_t lv_chart_get_zoom_x(const lv_obj_t * obj);

/**
 * Get Y zoom of a chart
 * @param obj       pointer to a chart object
 * @return          the Y zoom value
 */
uint16_t lv_chart_get_zoom_y(const lv_obj_t * obj);

/**
 * Set the number of tick lines on an axis
 * @param obj           pointer to a chart object
 * @param axis          an axis which ticks count should be set
 * @param major_len     length of major ticks
 * @param minor_len     length of minor ticks
 * @param major_cnt     number of major ticks on the axis
 * @param minor_cnt     number of minor ticks between two major ticks
 * @param label_en      true: enable label drawing on major ticks
 * @param draw_size     extra size required to draw the tick and labels
 *                      (start with 20 px and increase if the ticks/labels are clipped)
 */
void lv_chart_set_axis_tick(lv_obj_t * obj, lv_chart_axis_t axis, lv_coord_t major_len, lv_coord_t minor_len, lv_coord_t major_cnt, lv_coord_t minor_cnt, bool label_en, lv_coord_t draw_size);

/**
 * Get the type of a chart
 * @param obj       pointer to chart object
 * @return          type of the chart (from 'lv_chart_t' enum)
 */
lv_chart_type_t lv_chart_get_type(const lv_obj_t * obj);

/**
 * Get the data point number per data line on chart
 * @param chart     pointer to chart object
 * @return          point number on each data line
 */
uint16_t lv_chart_get_point_count(const lv_obj_t * obj);

/**
 * Get the current index of the x-axis start point in the data array
 * @param chart     pointer to a chart object
 * @param ser       pointer to a data series on 'chart'
 * @return          the index of the current x start point in the data array
 */
uint16_t lv_chart_get_x_start_point(const lv_obj_t * obj, lv_chart_series_t * ser);

/**
 * Get the position of point of the an index relative to the chart.
 * @param chart     pointer to a chart object
 * @param ser       pointer to series
 * @param id        the index.
 * @param p_out     store the result position here
 */
void lv_chart_get_point_pos_by_id(lv_obj_t * obj, lv_chart_series_t * ser, uint16_t id, lv_point_t * p_out);

/**
 * Refresh a chart if its data line has changed
 * @param   chart pointer to chart object
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
 * @return          pointer to the allocated data series
 */
lv_chart_series_t * lv_chart_add_series(lv_obj_t * obj, lv_color_t color, lv_chart_axis_t axis);

/**
 * Deallocate and remove a data series from a chart
 * @param chart     pointer to a chart object
 * @param series    pointer to a data series on 'chart'
 */
void lv_chart_remove_series(lv_obj_t * obj, lv_chart_series_t * series);

/**
 * Hide/Unhide a single series of a chart.
 * @param obj       pointer to a chart object.
 * @param series    pointer to a series object
 * @param hide      true: hide the series
 */
void lv_chart_hide_series(lv_obj_t * chart, lv_chart_series_t * series, bool hide);

/**
 * Change the color of a series
 * @param obj       pointer to a chart object.
 * @param series    pointer to a series object
 * @param color     the new color of the series
 */
void lv_chart_set_series_color(lv_obj_t * chart, lv_chart_series_t * series, lv_color_t color);

/**
 * Set the index of the x-axis start point in the data array.
 * This point will be considers the first (left) point and the other points will be drawn after it.
 * @param obj       pointer to a chart object
 * @param ser       pointer to a data series on 'chart'
 * @param id        the index of the x point in the data array
 */
void lv_chart_set_x_start_point(lv_obj_t * obj, lv_chart_series_t * ser, uint16_t id);

/**
 * Get the next series.
 * @param chart     pointer to a chart
 * @param ser      the previous series or NULL to get the first
 * @return          the next series or NULL if thre is no more.
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
 * @param obj       pointer to a chart object
 * @param cursor    pointer to the cursor
 * @param point     the new coordinate of cursor relative to paddings of the background
 */
void lv_chart_set_cursor_point(lv_obj_t * chart, lv_chart_cursor_t * cursor, lv_point_t * point);


/**
 * Get the coordinate of the cursor with respect to the paddings
 * @param obj       pointer to a chart object
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
 * @param value     the new value for all points. `LV_CHART_POINT_DEF` can be used to hide the points.
 */
void lv_chart_set_all_value(lv_obj_t * obj, lv_chart_series_t * ser, lv_coord_t value);

/**
 * Set the next point according to the update mode policy.
 * @param obj       pointer to chart object
 * @param ser       pointer to a data series on 'chart'
 * @param value     the new value of the next data
 */
void lv_chart_set_next_value(lv_obj_t * obj, lv_chart_series_t * ser, lv_coord_t value);

/**
 * Set an individual point's y value of a chart's series directly based on its index
 * @param obj     pointer to a chart object
 * @param ser     pointer to a data series on 'chart'
 * @param value   value to assign to array point
 * @param id      the index of the x point in the array
 */
void lv_chart_set_value_by_id(lv_obj_t * obj, lv_chart_series_t * ser, lv_coord_t value, uint16_t id);

/**
 * Set an external array of data points to use for the chart
 * NOTE: It is the users responsibility to make sure the `point_cnt` matches the external array size.
 * @param obj       pointer to a chart object
 * @param ser       pointer to a data series on 'chart'
 * @param array     external array of points for chart
 */
void lv_chart_set_ext_array(lv_obj_t * obj, lv_chart_series_t * ser, lv_coord_t array[]);

/**
 * Get the array of values of a series
 * @param obj   pointer to a chart object
 * @param ser   pointer to a data series on 'chart'
 * @return      the array of values with 'point_count' elements
 */
lv_coord_t * lv_chart_get_array(const lv_obj_t * obj, lv_chart_series_t * ser);

/**
 * Get the index of the currently pressed point. It's the same for every series.
 * @param obj       pointer to a chart object
 * @return          the index of the point [0 .. point count] or LV_CHART_POINT_ID_NONE if no point is being pressed
 */
uint32_t lv_chart_get_pressed_point(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_CHART*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_CHART_H*/
