/**
 * @file lv_chart.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_chart.h"
#if LV_USE_CHART != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_core/lv_refr.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_core/lv_disp.h"
#include "../lv_misc/lv_math.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_chart"

#define LV_CHART_YMIN_DEF 0
#define LV_CHART_YMAX_DEF 100
#define LV_CHART_HDIV_DEF 3
#define LV_CHART_VDIV_DEF 5
#define LV_CHART_PNUM_DEF 10
#define LV_CHART_LABEL_ITERATOR_FORWARD 1
#define LV_CHART_LABEL_ITERATOR_REVERSE 0
#define TICK_LABEL_GAP LV_DPX(2)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_chart_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_chart_destructor(lv_obj_t * obj);
static lv_draw_res_t lv_chart_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_chart_signal(lv_obj_t * obj, lv_signal_t sign, void * param);

static void draw_div_lines(lv_obj_t * obj , const lv_area_t * mask);
static void draw_series_line(lv_obj_t * obj, const lv_area_t * clip_area);
static void draw_series_column(lv_obj_t * obj, const lv_area_t * clip_area);
static void draw_cursors(lv_obj_t * obj, const lv_area_t * clip_area);
static void draw_axes(lv_obj_t * obj, const lv_area_t * mask);
static uint32_t get_index_from_x(lv_obj_t * obj, lv_coord_t x);
static void invalidate_point(lv_obj_t * obj, uint16_t i);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_chart = {
    .constructor = lv_chart_constructor,
    .destructor = lv_chart_destructor,
    .signal_cb = lv_chart_signal,
    .draw_cb = lv_chart_draw,
    .instance_size = sizeof(lv_chart_t),
    .base_class = &lv_obj
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a chart background objects
 * @param par pointer to an object, it will be the parent of the new chart background
 * @param copy pointer to a chart background object, if not NULL then the new object will be copied
 * from it
 * @return pointer to the created chart background
 */
lv_obj_t * lv_chart_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    return lv_obj_create_from_class(&lv_chart, parent, copy);
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Allocate and add a data series to the chart
 * @param chart pointer to a chart object
 * @param color color of the data series
 * @return pointer to the allocated data series
 */
lv_chart_series_t * lv_chart_add_series(lv_obj_t * obj, lv_color_t color)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_chart_t * chart    = (lv_chart_t *)obj;
    lv_chart_series_t * ser = _lv_ll_ins_head(&chart->series_ll);
    LV_ASSERT_MEM(ser);
    if(ser == NULL) return NULL;

    lv_coord_t def = LV_CHART_POINT_DEF;

    ser->color  = color;
    ser->points = lv_mem_alloc(sizeof(lv_coord_t) * chart->point_cnt);
    LV_ASSERT_MEM(ser->points);
    if(ser->points == NULL) {
        _lv_ll_remove(&chart->series_ll, ser);
        lv_mem_free(ser);
        return NULL;
    }

    ser->start_point = 0;
    ser->ext_buf_assigned = false;
    ser->hidden = 0;
    ser->y_axis = LV_CHART_AXIS_PRIMARY_Y;

    uint16_t i;
    lv_coord_t * p_tmp = ser->points;
    for(i = 0; i < chart->point_cnt; i++) {
        *p_tmp = def;
        p_tmp++;
    }

    return ser;
}

/**
 * Deallocate and remove a data series from a chart
 * @param chart pointer to a chart object
 * @param series pointer to a data series on 'chart'
 */
void lv_chart_remove_series(lv_obj_t * obj, lv_chart_series_t * series)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_NULL(series);
    
    lv_chart_t * chart    = (lv_chart_t *)obj;
    if(!series->ext_buf_assigned && series->points) lv_mem_free(series->points);

    _lv_ll_remove(&chart->series_ll, series);
    lv_mem_free(series);

    return;
}

/**
 * Add a cursor with a given color
 * @param chart pointer to chart object
 * @param color color of the cursor
 * @param dir direction of the cursor. `LV_DIR_RIGHT/LEFT/TOP/DOWN`. OR-ed values are possible
 * @return pointer to the created cursor
 */
lv_chart_cursor_t  * lv_chart_add_cursor(lv_obj_t * obj, lv_color_t color, lv_dir_t axes)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_chart_t * chart    = (lv_chart_t *)obj;
    lv_chart_cursor_t * cursor = _lv_ll_ins_head(&chart->cursors_ll);
    LV_ASSERT_MEM(cursor);
    if(cursor == NULL) return NULL;

    cursor->point.x = 0U;
    cursor->point.y = LV_CHART_POINT_DEF;
    cursor->color = color;
    cursor->axes = axes;

    return cursor;
}

/**
 * Clear the point of a series
 * @param chart pointer to a chart object
 * @param series pointer to the chart's series to clear
 */
void lv_chart_clear_series(lv_obj_t * obj, lv_chart_series_t * series)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_NULL(series);

    lv_chart_t * chart  = (lv_chart_t *)obj;

    uint32_t i;
    for(i = 0; i < chart->point_cnt; i++) {
        series->points[i] = LV_CHART_POINT_DEF;
    }

    series->start_point = 0;
}

/**
 * Hide/Unhide a single series of a chart.
 * @param chart pointer to a chart object.
 * @param series pointer to a series object
 * @param hide true: hide the series
 */
void lv_chart_hide_series(lv_obj_t * chart, lv_chart_series_t * series, bool hide)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);
    LV_ASSERT_NULL(series);

    series->hidden = hide ? 1 : 0;
    lv_chart_refresh(chart);
}


/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of horizontal and vertical division lines
 * @param chart pointer to a graph background object
 * @param hdiv number of horizontal division lines
 * @param vdiv number of vertical division lines
 */
void lv_chart_set_div_line_count(lv_obj_t * obj, uint8_t hdiv, uint8_t vdiv)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->hdiv_cnt == hdiv && chart->vdiv_cnt == vdiv) return;

    chart->hdiv_cnt = hdiv;
    chart->vdiv_cnt = vdiv;

    lv_obj_invalidate(obj);
}

/**
 * Set the minimal and maximal y values on an axis
 * @param chart pointer to a graph background object
 * @param axis `LV_CHART_AXIS_PRIMARY_Y` or `LV_CHART_AXIS_SECONDARY_Y`
 * @param ymin y minimum value
 * @param ymax y maximum value
 */
void lv_chart_set_y_range(lv_obj_t * obj, lv_chart_axis_t axis, lv_coord_t ymin, lv_coord_t ymax)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(axis != LV_CHART_AXIS_PRIMARY_Y && axis != LV_CHART_AXIS_SECONDARY_Y) {
        LV_LOG_WARN("Invalid axis: %d", axis);
        return;
    }

    lv_chart_t * chart  = (lv_chart_t *)obj;
    chart->ymin[axis] = ymin;
    chart->ymax[axis] = (ymax == ymin ? ymax + 1 : ymax);
    lv_chart_refresh(obj);
}

/**
 * Set a new type for a chart
 * @param chart pointer to a chart object
 * @param type new type of the chart (from 'lv_chart_type_t' enum)
 */
void lv_chart_set_type(lv_obj_t * obj, lv_chart_type_t type)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->type == type) return;

    chart->type = type;

    lv_chart_refresh(obj);
}

/**
 * Set the number of points on a data line on a chart
 * @param chart pointer r to chart object
 * @param point_cnt new number of points on the data lines
 */
void lv_chart_set_point_count(lv_obj_t * obj, uint16_t point_cnt)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->point_cnt == point_cnt) return;

    lv_chart_series_t * ser;
    uint16_t point_cnt_old = chart->point_cnt;
    uint16_t i;
    lv_coord_t def = LV_CHART_POINT_DEF;

    if(point_cnt < 1) point_cnt = 1;

    _LV_LL_READ_BACK(&chart->series_ll, ser) {
        if(!ser->ext_buf_assigned) {
            if(ser->start_point != 0) {
                lv_coord_t * new_points = lv_mem_alloc(sizeof(lv_coord_t) * point_cnt);
                LV_ASSERT_MEM(new_points);
                if(new_points == NULL) return;

                if(point_cnt >= point_cnt_old) {
                    for(i = 0; i < point_cnt_old; i++) {
                        new_points[i] =
                                ser->points[(i + ser->start_point) % point_cnt_old]; /*Copy old contents to new array*/
                    }
                    for(i = point_cnt_old; i < point_cnt; i++) {
                        new_points[i] = def; /*Fill up the rest with default value*/
                    }
                }
                else {
                    for(i = 0; i < point_cnt; i++) {
                        new_points[i] =
                                ser->points[(i + ser->start_point) % point_cnt_old]; /*Copy old contents to new array*/
                    }
                }

                /*Switch over pointer from old to new*/
                lv_mem_free(ser->points);
                ser->points = new_points;
            }
            else {
                ser->points = lv_mem_realloc(ser->points, sizeof(lv_coord_t) * point_cnt);
                LV_ASSERT_MEM(ser->points);
                if(ser->points == NULL) return;
                /*Initialize the new points*/
                if(point_cnt > point_cnt_old) {
                    for(i = point_cnt_old - 1; i < point_cnt; i++) {
                        ser->points[i] = def;
                    }
                }
            }
        }
        ser->start_point = 0;
    }

    chart->point_cnt = point_cnt;

    lv_chart_refresh(obj);
}

/**
 * Initialize all data points with a value
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y the new value  for all points
 */
void lv_chart_init_points(lv_obj_t * obj, lv_chart_series_t * ser, lv_coord_t y)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    uint16_t i;
    for(i = 0; i < chart->point_cnt; i++) {
        ser->points[i] = y;
    }
    ser->start_point = 0;
    lv_chart_refresh(obj);
}

/**
 * Set the value of points from an array
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y_array array of 'lv_coord_t' points (with 'points count' elements )
 */
void lv_chart_set_points(lv_obj_t * obj, lv_chart_series_t * ser, lv_coord_t y_array[])
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    memcpy(ser->points, y_array, chart->point_cnt * (sizeof(lv_coord_t)));
    ser->start_point = 0;
    lv_chart_refresh(obj);
}

/**
 * Shift all data left and set the rightmost data on a data line
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y the new value of the rightmost data
 */
void lv_chart_set_next(lv_obj_t * obj, lv_chart_series_t * ser, lv_coord_t y)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->update_mode == LV_CHART_UPDATE_MODE_SHIFT) {
        ser->points[ser->start_point] =
                y; /*This was the place of the former left most value, after shifting it is the rightmost*/
        ser->start_point = (ser->start_point + 1) % chart->point_cnt;
        lv_chart_refresh(obj);
    }
    else if(chart->update_mode == LV_CHART_UPDATE_MODE_CIRCULAR) {
        ser->points[ser->start_point] = y;

        invalidate_point(obj, ser->start_point);

        ser->start_point = (ser->start_point + 1) % chart->point_cnt; /*update the x for next incoming y*/
    }
}

/**
 * Set update mode of the chart object.
 * @param chart pointer to a chart object
 * @param update mode
 */
void lv_chart_set_update_mode(lv_obj_t * obj, lv_chart_update_mode_t update_mode)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->update_mode == update_mode) return;

    chart->update_mode = update_mode;
    lv_obj_invalidate(obj);
}

/**
 * Set the secondary y-axis tick count and labels of a chart
 * @param chart             pointer to a chart object
 * @param list_of_values    list of string values, terminated with \n, except the last
 * @param num_tick_marks    if list_of_values is NULL: total number of ticks per axis
 *                          else number of ticks between two value labels
 * @param options           extra options
 */
void lv_chart_set_tick_label_cb(lv_obj_t * obj, lv_chart_tick_label_cb_t tick_label_cb, lv_coord_t ext_size)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    chart->tick_label_cb = tick_label_cb;
    chart->ext_size = ext_size;
    _lv_obj_refresh_ext_draw_pad(obj);
    lv_obj_invalidate(obj);
}

void lv_chart_set_sub_tick_conut(lv_obj_t * obj, lv_chart_axis_t axis, uint32_t cnt)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    chart->sub_tick_cnt[axis] = cnt;
    lv_obj_invalidate(obj);
}

/**
 * Set the index of the x-axis start point in the data array
 * @param chart             pointer to a chart object
 * @param ser               pointer to a data series on 'chart'
 * @param id                the index of the x point in the data array
 */
void lv_chart_set_x_start_point(lv_obj_t * obj, lv_chart_series_t * ser, uint16_t id)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(id >= chart->point_cnt) return;
    ser->start_point = id;
}

/**
 * Set an external array of data points to use for the chart
 * NOTE: It is the users responsibility to make sure the point_cnt matches the external array size.
 * @param chart             pointer to a chart object
 * @param ser               pointer to a data series on 'chart'
 * @param array             external array of points for chart
 * @param point_cnt         number of external points in the array
 */
void lv_chart_set_ext_array(lv_obj_t * obj, lv_chart_series_t * ser, lv_coord_t array[], uint16_t point_cnt)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_NULL(ser);
    lv_chart_t * chart  = (lv_chart_t *)obj;

    if(!ser->ext_buf_assigned && ser->points) lv_mem_free(ser->points);
    ser->ext_buf_assigned = true;
    ser->points = array;
    chart->point_cnt = point_cnt;
}

/**
 * Set an individual point y value in the chart series directly based on index
 * @param chart             pointer to a chart object
 * @param ser               pointer to a data series on 'chart'
 * @param value             value to assign to array point
 * @param id                the index of the x point in the array
 */
void lv_chart_set_point_id(lv_obj_t * obj, lv_chart_series_t * ser, lv_coord_t value, uint16_t id)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_NULL(ser);
    lv_chart_t * chart  = (lv_chart_t *)obj;

    if(id >= chart->point_cnt) return;
    ser->points[id] = value;
}

/**
 * Set the Y axis of a series
 * @param chart pointer to a chart object
 * @param ser pointer to series
 * @param axis `LV_CHART_AXIS_PRIMARY_Y` or `LV_CHART_AXIS_SECONDARY_Y`
 */
void lv_chart_set_series_axis(lv_obj_t * obj, lv_chart_series_t * ser, lv_chart_axis_t axis)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_NULL(ser);

    if(axis >= _LV_CHART_AXIS_LAST) {
        LV_LOG_WARN("Invalid axis: %d", axis);
        return;
    }

    if(ser->y_axis == axis) return;

    ser->y_axis = axis;
    lv_chart_refresh(obj);
}

/**
 * Set the coordinate of the cursor with respect
 * to the origin of series area of the chart.
 * @param chart pointer to a chart object.
 * @param cursor pointer to the cursor.
 * @param point the new coordinate of cursor relative to the series area
 */
void lv_chart_set_cursor_point(lv_obj_t * obj, lv_chart_cursor_t * cursor, lv_point_t * point)
{
    LV_ASSERT_NULL(cursor);
    LV_UNUSED(obj);

    cursor->point.x = point->x;
    cursor->point.y = point->y;
    lv_chart_refresh(obj);
}


/*=====================
 * Getter functions
 *====================*/

/**
 * Get the type of a chart
 * @param chart pointer to chart object
 * @return type of the chart (from 'lv_chart_t' enum)
 */
lv_chart_type_t lv_chart_get_type(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    return chart->type;
}

/**
 * Get the data point number per data line on chart
 * @param chart pointer to chart object
 * @return point number on each data line
 */
uint16_t lv_chart_get_point_count(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    return chart->point_cnt;
}

/**
 * Get the current index of the x-axis start point in the data array
 * @param ser               pointer to a data series on 'chart'
 * @return                  the index of the current x start point in the data array
 */
uint16_t lv_chart_get_x_start_point(lv_chart_series_t * ser)
{
    LV_ASSERT_NULL(ser);

    return(ser->start_point);
}

/**
 * Get an individual point y value in the chart series directly based on index
 * @param chart             pointer to a chart object
 * @param ser               pointer to a data series on 'chart'
 * @param id                the index of the x point in the array
 * @return                  value of array point at index id
 */
lv_coord_t lv_chart_get_point_id(lv_obj_t * obj, lv_chart_series_t * ser, uint16_t id)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(id >= chart->point_cnt) id = 0;
    return(ser->points[id]);

}

/**
 * Get the Y axis of a series
 * @param chart pointer to a chart object
 * @param ser pointer to series
 * @return `LV_CHART_AXIS_PRIMARY_Y` or `LV_CHART_AXIS_SECONDARY_Y`
 */
lv_chart_axis_t lv_chart_get_series_axis(lv_obj_t * obj, lv_chart_series_t * ser)
{
    LV_ASSERT_NULL(ser);
    LV_UNUSED(obj);

    return ser->y_axis;
}

/**
 * Get the coordinate of the cursor with respect
 * to the origin of series area of the chart.
 * @param chart pointer to a chart object
 * @param cursor pointer to cursor
 * @return coordinate of the cursor as lv_point_t
 */
lv_point_t lv_chart_get_cursor_point(lv_obj_t * obj, lv_chart_cursor_t * cursor)
{
    LV_ASSERT_NULL(cursor);
    LV_UNUSED(obj);

    return cursor->point;
}

/**
 * Get the x coordinate of the an index with respect
 * to the origin of series area of the chart.
 * @param chart pointer to a chart object
 * @param ser pointer to series
 * @param id the index.
 * @return x coordinate of index
 */
lv_coord_t lv_chart_get_x_from_index(lv_obj_t * obj, lv_chart_series_t * ser, uint16_t id)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(id >= chart->point_cnt) {
        LV_LOG_WARN("Invalid index: %d", id);
        return 0;
    }

    lv_coord_t w = lv_obj_get_width(obj);

    lv_coord_t x = 0;


    if(chart->type & LV_CHART_TYPE_LINE) {
        x = (w * id) / (chart->point_cnt - 1);
    }
    else if(chart->type & LV_CHART_TYPE_COLUMN) {
        lv_coord_t col_w = w / ((_lv_ll_get_len(&chart->series_ll) + 1) * chart->point_cnt); /* Suppose + 1 series as separator*/
        lv_chart_series_t * itr_ser = NULL;
        lv_coord_t col_space = lv_obj_get_style_pad_left(obj, LV_PART_ITEMS);

        x = (int32_t)((int32_t)w * id) / chart->point_cnt;
        x += col_w / 2; /*Start offset*/

        _LV_LL_READ_BACK(&chart->series_ll, itr_ser) {
            if(itr_ser == ser) break;
            x += col_w;
        }

        x += (col_w - col_space) / 2;
    }

    return x;
}

/**
 * Get the y coordinate of the an index with respect
 * to the origin of series area of the chart.
 * @param chart pointer to a chart object
 * @param ser pointer to series
 * @param id the index.
 * @return y coordinate of index
 */
lv_coord_t lv_chart_get_y_from_index(lv_obj_t * obj, lv_chart_series_t * ser, uint16_t id)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(id >= chart->point_cnt) {
        LV_LOG_WARN("Invalid index: %d", id);
        return 0;
    }

    lv_coord_t h = lv_obj_get_height(obj);

    int32_t y = (int32_t)((int32_t)ser->points[id] - chart->ymin[ser->y_axis]) * h;
    y = y / (chart->ymax[ser->y_axis] - chart->ymin[ser->y_axis]);
    y  = h - y;

    return (lv_coord_t)y;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Refresh a chart if its data line has changed
 * @param chart pointer to chart object
 */
void lv_chart_refresh(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_invalidate(obj);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/



static void lv_chart_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_TRACE("chart create started");

    lv_obj_construct_base(obj, parent, copy);

    lv_chart_t * chart = (lv_chart_t *) obj;

    _lv_ll_init(&chart->series_ll, sizeof(lv_chart_series_t));
    _lv_ll_init(&chart->cursors_ll, sizeof(lv_chart_cursor_t));

    chart->ymin[LV_CHART_AXIS_PRIMARY_Y] = LV_CHART_YMIN_DEF;
    chart->ymax[LV_CHART_AXIS_PRIMARY_Y] = LV_CHART_YMAX_DEF;
    chart->ymin[LV_CHART_AXIS_SECONDARY_Y] = LV_CHART_YMIN_DEF;
    chart->ymax[LV_CHART_AXIS_SECONDARY_Y] = LV_CHART_YMAX_DEF;

    chart->hdiv_cnt    = LV_CHART_HDIV_DEF;
    chart->vdiv_cnt    = LV_CHART_VDIV_DEF;
    chart->point_cnt   = LV_CHART_PNUM_DEF;
    chart->sub_tick_cnt[0]   = 5;
    chart->sub_tick_cnt[1]   = 5;
    chart->sub_tick_cnt[2]   = 5;
    chart->type        = LV_CHART_TYPE_LINE;
    chart->update_mode = LV_CHART_UPDATE_MODE_SHIFT;
    chart->x_zoom      = 512;
    chart->y_zoom      = 256;

    /*Init the new chart background object*/
    if(copy == NULL) {
        lv_obj_set_size(obj, LV_DPI * 3, LV_DPI * 2);
    }
    else {
        lv_chart_t * chart_copy = (lv_chart_t*)copy;

        chart->type       = chart_copy->type;
        chart->hdiv_cnt   = chart_copy->hdiv_cnt;
        chart->vdiv_cnt   = chart_copy->vdiv_cnt;
        chart->point_cnt  = chart_copy->point_cnt;
        lv_memcpy_small(chart->ymin, chart_copy->ymin, sizeof(chart->ymin));
        lv_memcpy_small(chart->ymax, chart_copy->ymax, sizeof(chart->ymax));
    }
    LV_LOG_INFO("chart created");
}

static void lv_chart_destructor(lv_obj_t * obj)
{

//    lv_chart_series_t * ser;
//    while(chart->series_ll.head != NULL) {
//        ser = _lv_ll_get_head(&chart->series_ll);
//
//        if(!ser->ext_buf_assigned) lv_mem_free(ser->points);
//
//        _lv_ll_remove(&chart->series_ll, ser);
//        lv_mem_free(ser);
//    }
//    _lv_ll_clear(&chart->series_ll);
//
//    _lv_obj_reset_style_list_no_refr(chart, LV_PART_ITEMS);
//    _lv_obj_reset_style_list_no_refr(chart, LV_PART_CURSOR);
//    _lv_obj_reset_style_list_no_refr(chart, LV_PART_MAIN);
//}

    //    lv_bar_t * bar = obj;
    //
    //    _lv_obj_reset_style_list_no_refr(obj, LV_BAR_PART_INDIC);
    //    _lv_obj_reset_style_list_no_refr(sw, LV_PART_KNOB);
    //
    //    bar->class_p->base_p->destructor(obj);
}
/**
 * Handle the drawing related tasks of the chart backgrounds
 * @param chart pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DRAW_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DRAW_DRAW: draw the object (always return 'true')
 *             LV_DRAW_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_draw_res_t`
 */
static lv_draw_res_t lv_chart_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    if(mode == LV_DRAW_MODE_COVER_CHECK) {
        return lv_obj.draw_cb(obj, clip_area, mode);
    }
    else if(mode == LV_DRAW_MODE_MAIN_DRAW) {
        lv_obj.draw_cb(obj, clip_area, mode);

        draw_div_lines(obj, clip_area);
        draw_axes(obj, clip_area);

        lv_chart_t * chart  = (lv_chart_t *)obj;
        if(chart->type & LV_CHART_TYPE_LINE) draw_series_line(obj, clip_area);
        else if(chart->type & LV_CHART_TYPE_COLUMN) draw_series_column(obj, clip_area);

        draw_cursors(obj, clip_area);
    }
    else if(mode == LV_DRAW_MODE_POST_DRAW) {
        lv_obj.draw_cb(obj, clip_area, mode);
    }
    return LV_DRAW_RES_OK;
}

/**
 * Signal function of the chart background
 * @param chart pointer to a chart background object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
static lv_res_t lv_chart_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    /* Include the ancient signal function */
    lv_res_t res;

    res = lv_obj.signal_cb(obj, sign, param);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(sign == LV_SIGNAL_GET_SELF_SIZE) {
        lv_point_t * p = param;
        p->x = (lv_obj_get_width(obj) * chart->x_zoom) >> 8;
        p->y = (lv_obj_get_height(obj) * chart->y_zoom) >> 8;
    } else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        lv_coord_t * s = param;
        *s = LV_MAX(*s, chart->ext_size);
    } else if(sign == LV_SIGNAL_PRESSING) {
        lv_indev_t * indev = lv_indev_get_act();
        lv_point_t p;
        lv_indev_get_point(indev, &p);

        p.x -= obj->coords.x1;
        uint32_t id = get_index_from_x(obj, p.x);
        if(id != chart->pressed_point_id) {
            invalidate_point(obj, id);
            invalidate_point(obj, chart->pressed_point_id);
            chart->pressed_point_id = id;
        }
    } else if(sign == LV_SIGNAL_PRESSING) {
        invalidate_point(obj, chart->pressed_point_id);
        chart->pressed_point_id = -1;
    }

    return res;
}

/**
 * Draw the division lines on chart background
 * @param chart pointer to chart object
 * @param clip_area mask, inherited from the draw function
 */
static void draw_div_lines(lv_obj_t * obj, const lv_area_t * clip_area)
{
    lv_chart_t * chart  = (lv_chart_t *)obj;

    lv_area_t series_mask;
    bool mask_ret = _lv_area_intersect(&series_mask, &obj->coords, clip_area);
    if(mask_ret == false) return;

    uint32_t i;
    lv_point_t p1;
    lv_point_t p2;
    lv_coord_t h     = (lv_obj_get_height(obj) * chart->y_zoom) >> 8;
    lv_coord_t w     = (lv_obj_get_width(obj) * chart->x_zoom) >> 8;
    lv_coord_t x_ofs = obj->coords.x1 - lv_obj_get_scroll_left(obj);
    lv_coord_t y_ofs = obj->coords.y1 - lv_obj_get_scroll_top(obj);

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);

    if(chart->hdiv_cnt != 0) {
        p1.x = obj->coords.x1;
        p2.x = obj->coords.x2;
        for(i = 0; i <= chart->hdiv_cnt + 1; i++) {
            p1.y = (int32_t)((int32_t)(h - line_dsc.width) * i) / (chart->hdiv_cnt + 1);
            p1.y += y_ofs;
            p2.y = p1.y;
            lv_draw_line(&p1, &p2, &series_mask, &line_dsc);
        }
    }

    if(chart->vdiv_cnt != 0) {
        p1.y = obj->coords.y1;
        p2.y = obj->coords.y2;
        for(i = 0; i <= chart->vdiv_cnt + 1; i++) {
            p1.x = (int32_t)((int32_t)(w - line_dsc.width) * i) / (chart->vdiv_cnt + 1);
            p1.x += x_ofs;
            p2.x = p1.x;
            lv_draw_line(&p1, &p2, &series_mask, &line_dsc);
        }
    }
}

/**
 * Draw the data lines as lines on a chart
 * @param obj pointer to chart object
 */
static void draw_series_line(lv_obj_t * obj, const lv_area_t * clip_area)
{
    lv_area_t com_area;
    if(_lv_area_intersect(&com_area, &obj->coords, clip_area) == false) return;

    lv_chart_t * chart  = (lv_chart_t *)obj;

    uint16_t i;
    lv_point_t p1;
    lv_point_t p2;
    lv_coord_t w     = (lv_obj_get_width(obj) * chart->x_zoom) >> 8;
    lv_coord_t h     = lv_obj_get_height(obj);
    lv_coord_t x_ofs = obj->coords.x1 - lv_obj_get_scroll_left(obj);
    lv_coord_t y_ofs = obj->coords.y1 - lv_obj_get_scroll_top(obj);
    lv_chart_series_t * ser;

    lv_area_t series_mask;
    bool mask_ret = _lv_area_intersect(&series_mask, &obj->coords, clip_area);
    if(mask_ret == false) return;

    lv_state_t state_ori = obj->state;
    obj->state = LV_STATE_DEFAULT;
    lv_draw_line_dsc_t line_dsc_default;
    lv_draw_line_dsc_init(&line_dsc_default);
    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &line_dsc_default);

    lv_draw_rect_dsc_t point_dsc_default;
    lv_draw_rect_dsc_init(&point_dsc_default);
    point_dsc_default.radius = LV_RADIUS_CIRCLE;

    lv_coord_t point_size = lv_obj_get_style_size(obj, LV_PART_ITEMS);

    obj->state = LV_STATE_PRESSED;
    lv_coord_t point_size_pr = lv_obj_get_style_size(obj, LV_PART_ITEMS);

    obj->state = state_ori;

    lv_coord_t point_size_act;

    /*Do not bother with line ending is the point will over it*/
    if(point_size > line_dsc_default.width / 2) line_dsc_default.raw_end = 1;

    /*Go through all data lines*/
    _LV_LL_READ_BACK(&chart->series_ll, ser) {
    	if (ser->hidden) continue;
        line_dsc_default.color = ser->color;
        point_dsc_default.bg_color = ser->color;

        lv_coord_t start_point = chart->update_mode == LV_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

        p1.x = x_ofs;
        p2.x = x_ofs;

        lv_coord_t p_act = start_point;
        lv_coord_t p_prev = start_point;
        int32_t y_tmp = (int32_t)((int32_t)ser->points[p_prev] - chart->ymin[ser->y_axis]) * h;
        y_tmp  = y_tmp / (chart->ymax[ser->y_axis] - chart->ymin[ser->y_axis]);
        p2.y   = h - y_tmp + y_ofs;

        lv_obj_draw_hook_dsc_t hook_dsc;
        lv_obj_draw_hook_dsc_init(&hook_dsc, clip_area);
        hook_dsc.part = LV_PART_ITEMS;
        hook_dsc.line_dsc = &line_dsc_default;
        hook_dsc.rect_dsc = &point_dsc_default;

        for(i = 0; i < chart->point_cnt; i++) {
            p1.x = p2.x;
            p1.y = p2.y;

            point_size_act = p_act == chart->pressed_point_id ? point_size_pr : point_size;

            if(p1.x > clip_area->x2 + point_size_act) break;
            p2.x = ((w * i) / (chart->point_cnt - 1)) + x_ofs;

            p_act = (start_point + i) % chart->point_cnt;

            y_tmp = (int32_t)((int32_t)ser->points[p_act] - chart->ymin[ser->y_axis]) * h;
            y_tmp = y_tmp / (chart->ymax[ser->y_axis] - chart->ymin[ser->y_axis]);
            p2.y  = h - y_tmp + y_ofs;

            if(p2.x < clip_area->x1 - point_size_act) continue;

            hook_dsc.id = p_act;
            hook_dsc.p1 = &p1;
            hook_dsc.p2 = &p2;

            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &hook_dsc);

            /*Don't draw the first point. A second point is also required to draw the line*/
            if(i != 0 && ser->points[p_prev] != LV_CHART_POINT_DEF && ser->points[p_act] != LV_CHART_POINT_DEF) {
                lv_draw_line(&p1, &p2, &series_mask, &line_dsc_default);
            }

            if(point_size_act) {
                lv_area_t point_area;

                point_area.x1 = p1.x;
                point_area.x2 = point_area.x1 + point_size_act;
                point_area.x1 -= point_size_act;

                point_area.y1 = p1.y;
                point_area.y2 = point_area.y1 + point_size_act;
                point_area.y1 -= point_size_act;

                if(ser->points[p_act] != LV_CHART_POINT_DEF) {
                    lv_draw_rect(&point_area, &series_mask, &point_dsc_default);
                }
            }

            lv_event_send(obj, LV_EVENT_DRAW_PART_END, &hook_dsc);
            p_prev = p_act;
        }

        /*Draw the last point*/
        if(point_size) {
            lv_area_t point_area;

            point_area.x1 = p2.x;
            point_area.x2 = point_area.x1 + point_size;
            point_area.x1 -= point_size;

            point_area.y1 = p2.y;
            point_area.y2 = point_area.y1 + point_size;
            point_area.y1 -= point_size;

            if(ser->points[p_act] != LV_CHART_POINT_DEF) {
                lv_draw_rect(&point_area, &series_mask, &point_dsc_default);
            }
        }
    }
}

/**
 * Draw the data lines as columns on a chart
 * @param chart pointer to chart object
 * @param mask mask, inherited from the draw function
 */
static void draw_series_column(lv_obj_t * obj, const lv_area_t * clip_area)
{
    lv_area_t com_area;
    if(_lv_area_intersect(&com_area, &obj->coords, clip_area) == false) return;

    lv_chart_t * chart  = (lv_chart_t *)obj;

    uint16_t i;
    lv_area_t col_a;
    lv_coord_t w     = lv_obj_get_width(obj);
    lv_coord_t h     = lv_obj_get_height(obj);
    int32_t y_tmp;
    lv_chart_series_t * ser;
    lv_coord_t col_w = w / ((_lv_ll_get_len(&chart->series_ll) + 1) * chart->point_cnt); /* Suppose + 1 series as separator*/
    lv_coord_t x_ofs = col_w / 2 - lv_obj_get_scroll_left(obj);;                                    /*Shift with a half col.*/
    lv_coord_t col_space = lv_obj_get_style_pad_left(obj, LV_PART_ITEMS);

    lv_coord_t y_ofs =  -lv_obj_get_scroll_top(obj);

    lv_draw_rect_dsc_t col_dsc;
    lv_draw_rect_dsc_init(&col_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_ITEMS, &col_dsc);
    col_dsc.bg_grad_dir = LV_GRAD_DIR_NONE;
    col_dsc.bg_opa = LV_OPA_COVER;

    /*Make the cols longer with `radius` to clip the rounding from the bottom*/
    col_a.y2 = obj->coords.y2 + col_dsc.radius + y_ofs;

    lv_area_t series_mask;
    bool mask_ret = _lv_area_intersect(&series_mask, &obj->coords, clip_area);
    if(mask_ret == false) return;

    /*Go through all points*/
    for(i = 0; i < chart->point_cnt; i++) {
        lv_coord_t x_act = (int32_t)((int32_t)w * i) / chart->point_cnt;
        x_act += obj->coords.x1 + x_ofs;

        /*Draw the current point of all data line*/
        _LV_LL_READ_BACK(&chart->series_ll, ser) {
        	if (ser->hidden) continue;
            lv_coord_t start_point = chart->update_mode == LV_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

            col_a.x1 = x_act;
            col_a.x2 = col_a.x1 + col_w - col_space;
            x_act += col_w;

            if(col_a.x2 < series_mask.x1) continue;
            if(col_a.x1 > series_mask.x2) break;

            col_dsc.bg_color = ser->color;

            lv_coord_t p_act = (start_point + i) % chart->point_cnt;
            y_tmp            = (int32_t)((int32_t)ser->points[p_act] - chart->ymin[ser->y_axis]) * h;
            y_tmp            = y_tmp / (chart->ymax[ser->y_axis] - chart->ymin[ser->y_axis]);
            col_a.y1         = h - y_tmp + obj->coords.y1 + y_ofs;

            if(ser->points[p_act] != LV_CHART_POINT_DEF) {
                lv_draw_rect(&col_a, &series_mask, &col_dsc);
            }
        }
    }
}

/**
 * Draw the cursors as lines on a chart
 * @param chart pointer to chart object
 * @param clip_area the object will be drawn only in this area
 */
static void draw_cursors(lv_obj_t * obj, const lv_area_t * clip_area)
{
    lv_area_t series_mask;
    bool mask_ret = _lv_area_intersect(&series_mask, &obj->coords, clip_area);
    if(mask_ret == false) return;

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(_lv_ll_is_empty(&chart->cursors_ll)) return;

    lv_point_t p1;
    lv_point_t p2;
    lv_chart_cursor_t * cursor;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_CURSOR, &line_dsc);

    lv_draw_rect_dsc_t point_dsc;
    lv_draw_rect_dsc_init(&point_dsc);
    point_dsc.bg_opa = line_dsc.opa;
    point_dsc.radius = LV_RADIUS_CIRCLE;

    lv_coord_t point_radius = lv_obj_get_style_size(chart, LV_PART_CURSOR);

    /*Do not bother with line ending is the point will over it*/
    if(point_radius > line_dsc.width / 2) line_dsc.raw_end = 1;

    /*Go through all cursor lines*/
    _LV_LL_READ_BACK(&chart->cursors_ll, cursor) {
        line_dsc.color = cursor->color;
        point_dsc.bg_color = cursor->color;

        if(cursor->axes & LV_DIR_RIGHT) {
            p1.x = obj->coords.x1 + cursor->point.x;
            p1.y = obj->coords.y1 + cursor->point.y;
            p2.x = obj->coords.x2;
            p2.y = p1.y;
            lv_draw_line(&p1, &p2, &series_mask, &line_dsc);
        }

        if(cursor->axes & LV_DIR_TOP) {

            p1.x = obj->coords.x1 + cursor->point.x;
            p1.y = obj->coords.y1;
            p2.x = p1.x;
            p2.y = obj->coords.y1 + cursor->point.y;
            lv_draw_line(&p1, &p2, &series_mask, &line_dsc);
        }

        if(cursor->axes & LV_DIR_LEFT) {
            p1.x = obj->coords.x1;
            p1.y = obj->coords.y1 + cursor->point.y;
            p2.x = p1.x + cursor->point.x;
            p2.y = p1.y;
            lv_draw_line(&p1, &p2, &series_mask, &line_dsc);
        }

        if(cursor->axes & LV_DIR_BOTTOM) {

            p1.x = obj->coords.x1 + cursor->point.x;
            p1.y = obj->coords.y1 + cursor->point.y;
            p2.x = p1.x;
            p2.y = obj->coords.y2;
            lv_draw_line(&p1, &p2, &series_mask, &line_dsc);
        }

        if(point_radius) {
            lv_area_t point_area;

            point_area.x1 = obj->coords.x1 + cursor->point.x - point_radius;
            point_area.x2 = obj->coords.x1 + cursor->point.x + point_radius;

            point_area.y1 = obj->coords.y1 + cursor->point.y - point_radius;
            point_area.y2 = obj->coords.y1 + cursor->point.y + point_radius;

            /*Don't limit to `series_mask` to get full circles on the ends*/
            lv_draw_rect(&point_area, clip_area, &point_dsc);
        }

    }

}

static void draw_y_ticks(lv_obj_t * obj, const lv_area_t * mask, lv_chart_axis_t axis)
{
    lv_chart_t * chart  = (lv_chart_t *)obj;

    uint32_t i;
    uint32_t major_tick_cnt = chart->hdiv_cnt + 1;
    uint32_t sub_tick_cnt = chart->sub_tick_cnt[LV_CHART_AXIS_X] + 1;

    lv_point_t p1;
    lv_point_t p2;
    lv_coord_t x_ofs;
    lv_coord_t y_ofs = obj->coords.y1;
    lv_coord_t h     = (lv_obj_get_height(obj) * chart->y_zoom) >> 8;
    char buf[LV_CHART_AXIS_TICK_LABEL_MAX_LEN + 1]; /* up to N symbols per label + null terminator */

    /* chose correct side of the chart */
    lv_coord_t major_tick_len;
    if(axis == LV_CHART_AXIS_PRIMARY_Y) {
        x_ofs = obj->coords.x1;
        major_tick_len = lv_obj_get_style_pad_left(obj, LV_PART_MARKER);
    } else {
        x_ofs = obj->coords.x2;
        major_tick_len = lv_obj_get_style_pad_right(obj, LV_PART_MARKER);
    }

    lv_coord_t minor_tick_len = major_tick_len / 2;
    lv_coord_t label_gap = TICK_LABEL_GAP;

    /* tick lines on secondary y axis are drawn in other direction*/
    if(axis == LV_CHART_AXIS_SECONDARY_Y) {
        major_tick_len *= -1;
        minor_tick_len *= -1;
    }

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MARKER, &line_dsc);
    line_dsc.dash_gap = 0;
    line_dsc.dash_width = 0;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_MARKER, &label_dsc);

    uint32_t total_tick_num = major_tick_cnt * sub_tick_cnt;
    for(i = 0; i <= total_tick_num; i++) {
        /* draw a line at moving y position */
        p2.y = p1.y = y_ofs + (int32_t)((int32_t)(h - line_dsc.width) * i) / total_tick_num;

        if(p2.y - label_dsc.font->line_height > mask->y2) return;
        if(p2.y + label_dsc.font->line_height < mask->y1) continue;

        /* first point of the tick */
        p1.x = x_ofs;

        /* move extra pixel out of chart boundary */
        if(axis == LV_CHART_AXIS_PRIMARY_Y) p1.x--;
        else p1.x++;

        /* second point of the tick */
        bool major = false;
        if(i % sub_tick_cnt == 0) major = true;

        if(major) p2.x = p1.x - major_tick_len; /* major tick */
        else p2.x = p1.x - minor_tick_len; /* minor tick */

        lv_draw_line(&p1, &p2, mask, &line_dsc);

        /* add text only to major tick */
        if(!major) continue;
        chart->tick_label_cb(obj, axis, i / sub_tick_cnt, buf, sizeof(buf));

        /* reserve appropriate area */
        lv_point_t size;
        _lv_txt_get_size(&size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space,
                          LV_COORD_MAX, LV_TEXT_FLAG_CENTER);

        /* set the area at some distance of the major tick len left of the tick */
        lv_area_t a;
        a.y1 = p2.y - size.y / 2;
        a.y2 = p2.y + size.y / 2;

        if(axis == LV_CHART_AXIS_PRIMARY_Y) {
            a.x1 = p2.x - size.x - label_gap;
            a.x2 = p2.x - label_gap;
        }
        else {
            a.x1 = p2.x + label_gap;
            a.x2 = p2.x + size.x + label_gap;
        }

        lv_draw_label(&a, mask, &label_dsc, buf, NULL);
    }
}

static void draw_x_ticks(lv_obj_t * obj, const lv_area_t * mask)
{
    lv_chart_t * chart  = (lv_chart_t *)obj;

    lv_area_t series_mask;
    bool mask_ret = _lv_area_intersect(&series_mask, &obj->coords, mask);
    if(mask_ret == false) return;

    uint32_t i;
    uint32_t major_tick_cnt = chart->vdiv_cnt + 1;
    uint32_t sub_tick_cnt = chart->sub_tick_cnt[LV_CHART_AXIS_X] + 1;
    lv_point_t p1;
    lv_point_t p2;
    lv_coord_t x_ofs = obj->coords.x1 - lv_obj_get_scroll_left(obj);
    lv_coord_t y_ofs = obj->coords.y1;
    lv_coord_t h = lv_obj_get_height(obj);
    lv_coord_t w = (lv_obj_get_width(obj) * chart->x_zoom) >> 8;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_MARKER, &label_dsc);

    lv_coord_t major_tick_len = lv_obj_get_style_pad_bottom(obj, LV_PART_MARKER);
    lv_coord_t minor_tick_len = major_tick_len / 2;
    lv_coord_t label_gap = TICK_LABEL_GAP;

    if(h + y_ofs > mask->y2) return;
    if(h + y_ofs + label_gap  + label_dsc.font->line_height + major_tick_len < mask->y1) return;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MARKER, &line_dsc);
    line_dsc.dash_gap = 0;
    line_dsc.dash_width = 0;

    /* The columns don't start at the most right position
     * so change the width and offset accordingly. */
    if(chart->type == LV_CHART_TYPE_COLUMN) {
        uint32_t ser_num = _lv_ll_get_len(&chart->series_ll);
        lv_coord_t col_w = w / ((ser_num + 1) * chart->point_cnt); /* Suppose + 1 series as separator*/
        x_ofs += col_w / 2 + (col_w * (ser_num) / 2);
        w -= col_w * ser_num + col_w;
    }

    p1.y = h + y_ofs;
    char buf[LV_CHART_AXIS_TICK_LABEL_MAX_LEN + 1]; /* up to N symbols per label + null terminator */
    uint32_t total_tick_num = major_tick_cnt * sub_tick_cnt;
    for(i = 0; i <= total_tick_num; i++) { /* one extra loop - it may not exist in the list, empty label */
        bool major = false;
        if(i % sub_tick_cnt == 0) major = true;

        /* draw a line at moving x position */
        p2.x = p1.x = x_ofs + (int32_t)((int32_t)(w - line_dsc.width) * i) / total_tick_num;
        if(p1.x > series_mask.x2) return;

        p2.y = p1.y + (major ? major_tick_len : minor_tick_len);
        lv_draw_line(&p1, &p2, mask, &line_dsc);

            /* add text only to major tick */
        if(!major) continue;
        chart->tick_label_cb(obj, LV_CHART_AXIS_X, i / sub_tick_cnt, buf, sizeof(buf));

        /* reserve appropriate area */
        lv_point_t size;
        _lv_txt_get_size(&size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space,
                LV_COORD_MAX, LV_TEXT_FLAG_CENTER);

        /* set the area at some distance of the major tick len under of the tick */
        lv_area_t a;
        a.x1 = (p2.x - size.x / 2);
        a.x2 = (p2.x + size.x / 2),
        a.y1 = p2.y + label_gap;
        a.y2 = (a.y1 + size.y);
        lv_draw_label(&a, mask, &label_dsc, buf, NULL);
    }
}

static void draw_axes(lv_obj_t * obj, const lv_area_t * mask)
{
    draw_y_ticks(obj, mask, LV_CHART_AXIS_PRIMARY_Y);
    draw_y_ticks(obj, mask, LV_CHART_AXIS_SECONDARY_Y);
    draw_x_ticks(obj, mask);
}
/**
 * Get the nearest index to an X coordinate
 * @param chart pointer to a chart object
 * @param coord the coordination of the point relative to the series area.
 * @return the found index
 */
static uint32_t get_index_from_x(lv_obj_t * obj, lv_coord_t x)
{
    lv_coord_t w = lv_obj_get_width(obj);
    lv_chart_t * chart  = (lv_chart_t *)obj;

    if(x < 0) return 0;
    if(x > w) return chart->point_cnt - 1;
    if(chart->type == LV_CHART_TYPE_LINE) return (x * (chart->point_cnt - 1) + w / 2) / w;
    if(chart->type == LV_CHART_TYPE_COLUMN) return (x * chart->point_cnt) / w;

    return 0;
}

/**
 * invalid area of the new line data lines on a chart
 * @param obj pointer to chart object
 */
static void invalidate_point(lv_obj_t * obj, uint16_t i)
{
    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(i >= chart->point_cnt) return;


    lv_coord_t w     = lv_obj_get_width(obj);
    if(chart->type == LV_CHART_TYPE_LINE) {
        lv_coord_t x_ofs = obj->coords.x1;

        lv_coord_t line_width = lv_obj_get_style_line_width(obj, LV_PART_ITEMS);
        lv_coord_t point_radius = lv_obj_get_style_size(obj, LV_PART_ITEMS);

        lv_area_t coords;
        lv_area_copy(&coords, &obj->coords);
        coords.y1 -= line_width + point_radius;
        coords.y2 += line_width + point_radius;

        if(i < chart->point_cnt - 1) {
            coords.x1 = ((w * i) / (chart->point_cnt - 1)) + x_ofs - line_width - point_radius;
            coords.x2 = ((w * (i + 1)) / (chart->point_cnt - 1)) + x_ofs + line_width + point_radius;
            lv_obj_invalidate_area(obj, &coords);
        }

        if(i > 0) {
            coords.x1 = ((w * (i - 1)) / (chart->point_cnt - 1)) + x_ofs - line_width - point_radius;
            coords.x2 = ((w * i) / (chart->point_cnt - 1)) + x_ofs + line_width + point_radius;
            lv_obj_invalidate_area(obj, &coords);
        }
    }
    else if(chart->type == LV_CHART_TYPE_COLUMN) {
        lv_area_t col_a;
        lv_coord_t col_w = w / ((_lv_ll_get_len(&chart->series_ll) + 1) * chart->point_cnt); /* Suppose + 1 series as separator*/
        lv_coord_t x_ofs = col_w / 2;                                    /*Shift with a half col.*/

        lv_coord_t x_act;
        x_act = (int32_t)((int32_t)w * i) / chart->point_cnt;
        x_act += obj->coords.x1 + x_ofs;

        lv_obj_get_coords(obj, &col_a);
        col_a.x1 = x_act;
        col_a.x2 = col_a.x1 + col_w;

        _lv_inv_area(lv_obj_get_disp(obj), &col_a);
    } else {
        lv_obj_invalidate(chart);
    }
}

#endif
