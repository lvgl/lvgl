/**
 * @file lv_chart.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_chart.h"
#if LV_USE_CHART != 0

#include "../lv_core/lv_debug.h"
#include "../lv_core/lv_refr.h"
#include "../lv_draw/lv_draw.h"
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
#define LV_CHART_AXIS_TO_LABEL_DISTANCE 4
#define LV_CHART_AXIS_MAJOR_TICK_LEN_COE 1 / 15
#define LV_CHART_AXIS_MINOR_TICK_LEN_COE 2 / 3
#define LV_CHART_AXIS_PRIMARY_Y 1
#define LV_CHART_AXIS_SECONDARY_Y 0
#define LV_CHART_LABEL_ITERATOR_FORWARD 1
#define LV_CHART_LABEL_ITERATOR_REVERSE 0

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * list_start;
    const char * current_pos;
    uint8_t items_left;
    uint8_t is_reverse_iter;
} lv_chart_label_iterator_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_chart_design(lv_obj_t * chart, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_chart_signal(lv_obj_t * chart, lv_signal_t sign, void * param);
static void lv_chart_draw_div(lv_obj_t * chart, const lv_area_t * mask);
static void lv_chart_draw_lines(lv_obj_t * chart, const lv_area_t * mask);
static void lv_chart_draw_points(lv_obj_t * chart, const lv_area_t * mask);
static void lv_chart_draw_cols(lv_obj_t * chart, const lv_area_t * mask);
static void lv_chart_draw_vertical_lines(lv_obj_t * chart, const lv_area_t * mask);
static void lv_chart_draw_areas(lv_obj_t * chart, const lv_area_t * mask);
static void lv_chart_draw_axes(lv_obj_t * chart, const lv_area_t * mask);
static void lv_chart_inv_lines(lv_obj_t * chart, uint16_t i);
static void lv_chart_inv_points(lv_obj_t * chart, uint16_t i);
static void lv_chart_inv_cols(lv_obj_t * chart, uint16_t i);
static void lv_chart_get_next_label(lv_chart_label_iterator_t * iterator, char * buf);
static inline bool lv_chart_is_tick_with_label(uint8_t tick_num, lv_chart_axis_cfg_t * axis);
static lv_chart_label_iterator_t lv_chart_create_label_iter(const char * list, uint8_t iterator_dir);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_cb_t ancestor_design_f;
static lv_signal_cb_t ancestor_signal;

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
lv_obj_t * lv_chart_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("chart create started");

    /*Create the ancestor basic object*/
    lv_obj_t * new_chart = lv_obj_create(par, copy);
    LV_ASSERT_MEM(new_chart);
    if(new_chart == NULL) return NULL;

    /*Allocate the object type specific extended data*/
    lv_chart_ext_t * ext = lv_obj_allocate_ext_attr(new_chart, sizeof(lv_chart_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) return NULL;

    lv_ll_init(&ext->series_ll, sizeof(lv_chart_series_t));

    ext->series.num            = 0;
    ext->ymin                  = LV_CHART_YMIN_DEF;
    ext->ymax                  = LV_CHART_YMAX_DEF;
    ext->hdiv_cnt              = LV_CHART_HDIV_DEF;
    ext->vdiv_cnt              = LV_CHART_VDIV_DEF;
    ext->point_cnt             = LV_CHART_PNUM_DEF;
    ext->type                  = LV_CHART_TYPE_LINE;
    ext->update_mode           = LV_CHART_UPDATE_MODE_SHIFT;
    ext->series.opa            = LV_OPA_COVER;
    ext->series.dark           = LV_OPA_50;
    ext->series.width          = 2;
    ext->margin                = 0;
    memset(&ext->x_axis, 0, sizeof(ext->x_axis));
    memset(&ext->y_axis, 0, sizeof(ext->y_axis));
    memset(&ext->secondary_y_axis, 0, sizeof(ext->secondary_y_axis));
    ext->x_axis.major_tick_len = LV_CHART_TICK_LENGTH_AUTO;
    ext->x_axis.minor_tick_len = LV_CHART_TICK_LENGTH_AUTO;
    ext->y_axis.major_tick_len = LV_CHART_TICK_LENGTH_AUTO;
    ext->y_axis.minor_tick_len = LV_CHART_TICK_LENGTH_AUTO;
    ext->secondary_y_axis.major_tick_len = LV_CHART_TICK_LENGTH_AUTO;
    ext->secondary_y_axis.minor_tick_len = LV_CHART_TICK_LENGTH_AUTO;

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_cb(new_chart);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_chart);

    lv_obj_set_signal_cb(new_chart, lv_chart_signal);
    lv_obj_set_design_cb(new_chart, lv_chart_design);

    /*Init the new chart background object*/
    if(copy == NULL) {
        lv_obj_set_size(new_chart, LV_DPI * 3, LV_DPI * 2);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_chart_set_style(new_chart, LV_CHART_STYLE_MAIN, th->style.chart);
        } else {
            lv_chart_set_style(new_chart, LV_CHART_STYLE_MAIN, &lv_style_pretty);
        }

    } else {
        lv_chart_ext_t * ext_copy = lv_obj_get_ext_attr(copy);

        ext->type       = ext_copy->type;
        ext->ymin       = ext_copy->ymin;
        ext->ymax       = ext_copy->ymax;
        ext->hdiv_cnt   = ext_copy->hdiv_cnt;
        ext->vdiv_cnt   = ext_copy->vdiv_cnt;
        ext->point_cnt  = ext_copy->point_cnt;
        ext->series.opa = ext_copy->series.opa;
        ext->margin     = ext_copy->margin;
        memcpy(&ext->x_axis, &ext_copy->x_axis, sizeof(lv_chart_axis_cfg_t));
        memcpy(&ext->y_axis, &ext_copy->y_axis, sizeof(lv_chart_axis_cfg_t));
        memcpy(&ext->secondary_y_axis, &ext_copy->secondary_y_axis, sizeof(lv_chart_axis_cfg_t));

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_chart);
    }

    LV_LOG_INFO("chart created");

    return new_chart;
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
lv_chart_series_t * lv_chart_add_series(lv_obj_t * chart, lv_color_t color)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext    = lv_obj_get_ext_attr(chart);
    lv_chart_series_t * ser = lv_ll_ins_head(&ext->series_ll);
    LV_ASSERT_MEM(ser);
    if(ser == NULL) return NULL;

    lv_coord_t def = LV_CHART_POINT_DEF;

    if(ser == NULL) return NULL;

    ser->color  = color;
    ser->points = lv_mem_alloc(sizeof(lv_coord_t) * ext->point_cnt);
    LV_ASSERT_MEM(ser->points);
    if(ser->points == NULL) {
        lv_ll_rem(&ext->series_ll, ser);
        lv_mem_free(ser);
        return NULL;
    }

    ser->start_point = 0;

    uint16_t i;
    lv_coord_t * p_tmp = ser->points;
    for(i = 0; i < ext->point_cnt; i++) {
        *p_tmp = def;
        p_tmp++;
    }

    ext->series.num++;

    return ser;
}

/**
 * Clear the point of a serie
 * @param chart pointer to a chart object
 * @param serie pointer to the chart's serie to clear
 */
void lv_chart_clear_serie(lv_obj_t * chart, lv_chart_series_t * serie)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);
    LV_ASSERT_NULL(serie);

    if(chart == NULL || serie == NULL) return;
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext == NULL) return;

    uint32_t i;
    for(i = 0; i < ext->point_cnt; i++) {
        serie->points[i] = LV_CHART_POINT_DEF;
    }

    serie->start_point = 0;
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
void lv_chart_set_div_line_count(lv_obj_t * chart, uint8_t hdiv, uint8_t vdiv)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->hdiv_cnt == hdiv && ext->vdiv_cnt == vdiv) return;

    ext->hdiv_cnt = hdiv;
    ext->vdiv_cnt = vdiv;

    lv_obj_invalidate(chart);
}

/**
 * Set the minimal and maximal y values
 * @param chart pointer to a graph background object
 * @param ymin y minimum value
 * @param ymax y maximum value
 */
void lv_chart_set_range(lv_obj_t * chart, lv_coord_t ymin, lv_coord_t ymax)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->ymin == ymin && ext->ymax == ymax) return;

    ext->ymin = ymin;
    ext->ymax = ymax;

    lv_chart_refresh(chart);
}

/**
 * Set a new type for a chart
 * @param chart pointer to a chart object
 * @param type new type of the chart (from 'lv_chart_type_t' enum)
 */
void lv_chart_set_type(lv_obj_t * chart, lv_chart_type_t type)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->type == type) return;

    ext->type = type;

    lv_chart_refresh(chart);
}

/**
 * Set the number of points on a data line on a chart
 * @param chart pointer r to chart object
 * @param point_cnt new number of points on the data lines
 */
void lv_chart_set_point_count(lv_obj_t * chart, uint16_t point_cnt)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->point_cnt == point_cnt) return;

    lv_chart_series_t * ser;
    uint16_t point_cnt_old = ext->point_cnt;
    uint16_t i;
    lv_coord_t def = LV_CHART_POINT_DEF;

    if(point_cnt < 1) point_cnt = 1;

    LV_LL_READ_BACK(ext->series_ll, ser)
    {
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
            } else {
                for(i = 0; i < point_cnt; i++) {
                    new_points[i] =
                        ser->points[(i + ser->start_point) % point_cnt_old]; /*Copy old contents to new array*/
                }
            }

            /*Switch over pointer from old to new*/
            lv_mem_free(ser->points);
            ser->points = new_points;
        } else {
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

        ser->start_point = 0;
    }

    ext->point_cnt = point_cnt;

    lv_chart_refresh(chart);
}

/**
 * Set the opacity of the data series
 * @param chart pointer to a chart object
 * @param opa opacity of the data series
 */
void lv_chart_set_series_opa(lv_obj_t * chart, lv_opa_t opa)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->series.opa == opa) return;

    ext->series.opa = opa;
    lv_obj_invalidate(chart);
}

/**
 * Set the line width or point radius of the data series
 * @param chart pointer to a chart object
 * @param width the new width
 */
void lv_chart_set_series_width(lv_obj_t * chart, lv_coord_t width)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->series.width == width) return;

    ext->series.width = width;
    lv_obj_invalidate(chart);
}
/**
 * Set the dark effect on the bottom of the points or columns
 * @param chart pointer to a chart object
 * @param dark_eff dark effect level (LV_OPA_TRANSP to turn off)
 */
void lv_chart_set_series_darking(lv_obj_t * chart, lv_opa_t dark_eff)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->series.dark == dark_eff) return;

    ext->series.dark = dark_eff;
    lv_obj_invalidate(chart);
}

/**
 * Initialize all data points with a value
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y the new value  for all points
 */
void lv_chart_init_points(lv_obj_t * chart, lv_chart_series_t * ser, lv_coord_t y)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);
    LV_ASSERT_NULL(ser);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    uint16_t i;
    for(i = 0; i < ext->point_cnt; i++) {
        ser->points[i] = y;
    }
    ser->start_point = 0;
    lv_chart_refresh(chart);
}

/**
 * Set the value of points from an array
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y_array array of 'lv_coord_t' points (with 'points count' elements )
 */
void lv_chart_set_points(lv_obj_t * chart, lv_chart_series_t * ser, lv_coord_t y_array[])
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);
    LV_ASSERT_NULL(ser);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    memcpy(ser->points, y_array, ext->point_cnt * (sizeof(lv_coord_t)));
    ser->start_point = 0;
    lv_chart_refresh(chart);
}

/**
 * Shift all data left and set the rightmost data on a data line
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y the new value of the rightmost data
 */
void lv_chart_set_next(lv_obj_t * chart, lv_chart_series_t * ser, lv_coord_t y)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);
    LV_ASSERT_NULL(ser);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->update_mode == LV_CHART_UPDATE_MODE_SHIFT) {
        ser->points[ser->start_point] =
            y; /*This was the place of the former left most value, after shifting it is the rightmost*/
        ser->start_point = (ser->start_point + 1) % ext->point_cnt;
        lv_chart_refresh(chart);
    } else if(ext->update_mode == LV_CHART_UPDATE_MODE_CIRCULAR) {
        ser->points[ser->start_point] = y;

        if(ext->type & LV_CHART_TYPE_LINE) lv_chart_inv_lines(chart, ser->start_point);
        if(ext->type & LV_CHART_TYPE_COLUMN) lv_chart_inv_cols(chart, ser->start_point);
        if(ext->type & LV_CHART_TYPE_POINT) lv_chart_inv_points(chart, ser->start_point);
        if(ext->type & LV_CHART_TYPE_VERTICAL_LINE) lv_chart_inv_lines(chart, ser->start_point);
        if(ext->type & LV_CHART_TYPE_AREA) lv_chart_inv_lines(chart, ser->start_point);

        ser->start_point = (ser->start_point + 1) % ext->point_cnt; /*update the x for next incoming y*/
    }
}

/**
 * Set update mode of the chart object.
 * @param chart pointer to a chart object
 * @param update mode
 */
void lv_chart_set_update_mode(lv_obj_t * chart, lv_chart_update_mode_t update_mode)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    if(ext->update_mode == update_mode) return;

    ext->update_mode = update_mode;
    lv_obj_invalidate(chart);
}

/**
 * Set the length of the tick marks on the x axis
 * @param chart pointer to the chart
 * @param major_tick_len the length of the major tick or `LV_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where labels are added)
 * @param minor_tick_len the length of the minor tick, `LV_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where no labels are added)
 */
void lv_chart_set_x_tick_length(lv_obj_t * chart, uint8_t major_tick_len, uint8_t minor_tick_len)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext       = lv_obj_get_ext_attr(chart);
    ext->x_axis.major_tick_len = major_tick_len;
    ext->x_axis.minor_tick_len = minor_tick_len;
}

/**
 * Set the length of the tick marks on the y axis
 * @param chart pointer to the chart
 * @param major_tick_len the length of the major tick or `LV_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where labels are added)
 * @param minor_tick_len the length of the minor tick, `LV_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where no labels are added)
 */
void lv_chart_set_y_tick_length(lv_obj_t * chart, uint8_t major_tick_len, uint8_t minor_tick_len)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext       = lv_obj_get_ext_attr(chart);
    ext->y_axis.major_tick_len = major_tick_len;
    ext->y_axis.minor_tick_len = minor_tick_len;
}

/**
 * Set the length of the tick marks on the secondary y axis
 * @param chart pointer to the chart
 * @param major_tick_len the length of the major tick or `LV_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where labels are added)
 * @param minor_tick_len the length of the minor tick, `LV_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where no labels are added)
 */
void lv_chart_set_secondary_y_tick_length(lv_obj_t * chart, uint8_t major_tick_len, uint8_t minor_tick_len)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext       = lv_obj_get_ext_attr(chart);
    ext->secondary_y_axis.major_tick_len = major_tick_len;
    ext->secondary_y_axis.minor_tick_len = minor_tick_len;
}

/**
 * Set the x-axis tick count and labels of a chart
 * @param chart 			pointer to a chart object
 * @param list_of_values 	list of string values, terminated with \n, except the last
 * @param num_tick_marks 	if list_of_values is NULL: total number of ticks per axis
 * 							else number of ticks between two value labels
 * @param options			extra options
 */
void lv_chart_set_x_tick_texts(lv_obj_t * chart, const char * list_of_values, uint8_t num_tick_marks,
                               lv_chart_axis_options_t options)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);
    LV_ASSERT_NULL(list_of_values);

    lv_chart_ext_t * ext       = lv_obj_get_ext_attr(chart);
    ext->x_axis.num_tick_marks = num_tick_marks;
    ext->x_axis.list_of_values = list_of_values;
    ext->x_axis.options        = options;
}

/**
 * Set the y-axis tick count and labels of a chart
 * @param chart             pointer to a chart object
 * @param list_of_values    list of string values, terminated with \n, except the last
 * @param num_tick_marks    if list_of_values is NULL: total number of ticks per axis
 *                          else number of ticks between two value labels
 * @param options           extra options
 */
void lv_chart_set_y_tick_texts(lv_obj_t * chart, const char * list_of_values, uint8_t num_tick_marks,
                               lv_chart_axis_options_t options)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);
    LV_ASSERT_NULL(list_of_values);

    lv_chart_ext_t * ext       = lv_obj_get_ext_attr(chart);
    ext->y_axis.num_tick_marks = num_tick_marks;
    ext->y_axis.list_of_values = list_of_values;
    ext->y_axis.options        = options;
}

/**
 * Set the secondary y-axis tick count and labels of a chart
 * @param chart             pointer to a chart object
 * @param list_of_values    list of string values, terminated with \n, except the last
 * @param num_tick_marks    if list_of_values is NULL: total number of ticks per axis
 *                          else number of ticks between two value labels
 * @param options           extra options
 */
void lv_chart_set_secondary_y_tick_texts(lv_obj_t * chart, const char * list_of_values, uint8_t num_tick_marks,
                                        lv_chart_axis_options_t options)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);
    LV_ASSERT_NULL(list_of_values);

    lv_chart_ext_t * ext       = lv_obj_get_ext_attr(chart);
    ext->secondary_y_axis.num_tick_marks = num_tick_marks;
    ext->secondary_y_axis.list_of_values = list_of_values;
    ext->secondary_y_axis.options        = options;
}

/**
 * Set the margin around the chart, used for axes value and ticks
 * @param chart     pointer to an chart object
 * @param margin    value of the margin [px]
 */
void lv_chart_set_margin(lv_obj_t * chart, uint16_t margin)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    ext->margin          = margin;
    lv_obj_refresh_ext_draw_pad(chart);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the type of a chart
 * @param chart pointer to chart object
 * @return type of the chart (from 'lv_chart_t' enum)
 */
lv_chart_type_t lv_chart_get_type(const lv_obj_t * chart)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    return ext->type;
}

/**
 * Get the data point number per data line on chart
 * @param chart pointer to chart object
 * @return point number on each data line
 */
uint16_t lv_chart_get_point_cnt(const lv_obj_t * chart)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    return ext->point_cnt;
}

/**
 * Get the opacity of the data series
 * @param chart pointer to chart object
 * @return the opacity of the data series
 */
lv_opa_t lv_chart_get_series_opa(const lv_obj_t * chart)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    return ext->series.opa;
}

/**
 * Get the data series width
 * @param chart pointer to chart object
 * @return the width the data series (lines or points)
 */
lv_coord_t lv_chart_get_series_width(const lv_obj_t * chart)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    return ext->series.width;
}

/**
 * Get the dark effect level on the bottom of the points or columns
 * @param chart pointer to chart object
 * @return dark effect level (LV_OPA_TRANSP to turn off)
 */
lv_opa_t lv_chart_get_series_darking(const lv_obj_t * chart)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    return ext->series.dark;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Refresh a chart if its data line has changed
 * @param chart pointer to chart object
 */
void lv_chart_refresh(lv_obj_t * chart)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_obj_invalidate(chart);
}

/**
 * Get the margin around the chart, used for axes value and labels
 * @param chart pointer to an chart object
 * @param return value of the margin
 */
uint16_t lv_chart_get_margin(lv_obj_t * chart)
{
    LV_ASSERT_OBJ(chart, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    return ext->margin;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the chart backgrounds
 * @param chart pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_chart_design(lv_obj_t * chart, const lv_area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        /*Return false if the object is not covers the mask_p area*/
        return ancestor_design_f(chart, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
        /*Draw the background*/
        lv_draw_rect(&chart->coords, mask, lv_obj_get_style(chart), lv_obj_get_opa_scale(chart));

        lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

        lv_chart_draw_div(chart, mask);

        /* Adjust the mask to remove the margin (clips chart contents to be within background) */

        lv_area_t mask_tmp, adjusted_mask;
        lv_obj_get_coords(chart, &mask_tmp);

        bool union_ok = lv_area_intersect(&adjusted_mask, mask, &mask_tmp);

        if(union_ok) {
                if(ext->type & LV_CHART_TYPE_LINE) lv_chart_draw_lines(chart, &adjusted_mask);
                if(ext->type & LV_CHART_TYPE_COLUMN) lv_chart_draw_cols(chart, &adjusted_mask);
                if(ext->type & LV_CHART_TYPE_POINT) lv_chart_draw_points(chart, &adjusted_mask);
                if(ext->type & LV_CHART_TYPE_VERTICAL_LINE) lv_chart_draw_vertical_lines(chart, &adjusted_mask);
                if(ext->type & LV_CHART_TYPE_AREA) lv_chart_draw_areas(chart, &adjusted_mask);
        }

        lv_chart_draw_axes(chart, mask);
    }
    return true;
}

/**
 * Signal function of the chart background
 * @param chart pointer to a chart background object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
static lv_res_t lv_chart_signal(lv_obj_t * chart, lv_signal_t sign, void * param)
{
    /* Include the ancient signal function */
    lv_res_t res;
    res = ancestor_signal(chart, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

    if(sign == LV_SIGNAL_CLEANUP) {
        lv_coord_t ** datal;
        LV_LL_READ(ext->series_ll, datal)
        {
            lv_mem_free(*datal);
        }
        lv_ll_clear(&ext->series_ll);
    } else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        /*Provide extra px draw area around the chart*/
        chart->ext_draw_pad = ext->margin;
    }

    return res;
}

/**
 * Draw the division lines on chart background
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void lv_chart_draw_div(lv_obj_t * chart, const lv_area_t * mask)
{
    lv_chart_ext_t * ext     = lv_obj_get_ext_attr(chart);
    const lv_style_t * style = lv_obj_get_style(chart);
    lv_opa_t opa_scale       = lv_obj_get_opa_scale(chart);

    uint8_t div_i;
    uint8_t div_i_end;
    uint8_t div_i_start;
    lv_point_t p1;
    lv_point_t p2;
    lv_coord_t w     = lv_obj_get_width(chart);
    lv_coord_t h     = lv_obj_get_height(chart);
    lv_coord_t x_ofs = chart->coords.x1;
    lv_coord_t y_ofs = chart->coords.y1;

    if(ext->hdiv_cnt != 0) {
        /*Draw side lines if no border*/
        if(style->body.border.width != 0) {
            div_i_start = 1;
            div_i_end   = ext->hdiv_cnt;
        } else {
            div_i_start = 0;
            div_i_end   = ext->hdiv_cnt + 1;
        }

        p1.x = 0 + x_ofs;
        p2.x = w - 1 + x_ofs;
        for(div_i = div_i_start; div_i <= div_i_end; div_i++) {
            p1.y = (int32_t)((int32_t)(h - style->line.width) * div_i) / (ext->hdiv_cnt + 1);
            p1.y += y_ofs;
            p2.y = p1.y;
            lv_draw_line(&p1, &p2, mask, style, opa_scale);
        }
    }

    if(ext->vdiv_cnt != 0) {
        /*Draw side lines if no border*/
        if(style->body.border.width != 0) {
            div_i_start = 1;
            div_i_end   = ext->vdiv_cnt;
        } else {
            div_i_start = 0;
            div_i_end   = ext->vdiv_cnt + 1;
        }

        p1.y = 0 + y_ofs;
        p2.y = h + y_ofs - 1;
        for(div_i = div_i_start; div_i <= div_i_end; div_i++) {
            p1.x = (int32_t)((int32_t)(w - style->line.width) * div_i) / (ext->vdiv_cnt + 1);
            p1.x += x_ofs;
            p2.x = p1.x;
            lv_draw_line(&p1, &p2, mask, style, opa_scale);
        }
    }
}

/**
 * Draw the data lines as lines on a chart
 * @param obj pointer to chart object
 */
static void lv_chart_draw_lines(lv_obj_t * chart, const lv_area_t * mask)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

    uint16_t i;
    lv_point_t p1;
    lv_point_t p2;
    lv_coord_t w     = lv_obj_get_width(chart);
    lv_coord_t h     = lv_obj_get_height(chart);
    lv_coord_t x_ofs = chart->coords.x1;
    lv_coord_t y_ofs = chart->coords.y1;
    int32_t y_tmp;
    lv_coord_t p_prev;
    lv_coord_t p_act;
    lv_chart_series_t * ser;
    lv_opa_t opa_scale = lv_obj_get_opa_scale(chart);
    lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.line.opa   = ext->series.opa;
    style.line.width = ext->series.width;

    /*Go through all data lines*/
    LV_LL_READ_BACK(ext->series_ll, ser)
    {
        style.line.color = ser->color;

        lv_coord_t start_point = ext->update_mode == LV_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

        p1.x = 0 + x_ofs;
        p2.x = 0 + x_ofs;

        p_prev = start_point;
        y_tmp  = (int32_t)((int32_t)ser->points[p_prev] - ext->ymin) * h;
        y_tmp  = y_tmp / (ext->ymax - ext->ymin);
        p2.y   = h - y_tmp + y_ofs;

        for(i = 1; i < ext->point_cnt; i++) {
            p1.x = p2.x;
            p1.y = p2.y;

            p2.x = ((w * i) / (ext->point_cnt - 1)) + x_ofs;

            p_act = (start_point + i) % ext->point_cnt;

            y_tmp = (int32_t)((int32_t)ser->points[p_act] - ext->ymin) * h;
            y_tmp = y_tmp / (ext->ymax - ext->ymin);
            p2.y  = h - y_tmp + y_ofs;

            if(ser->points[p_prev] != LV_CHART_POINT_DEF && ser->points[p_act] != LV_CHART_POINT_DEF)
                lv_draw_line(&p1, &p2, mask, &style, opa_scale);

            p_prev = p_act;
        }
    }
}

/**
 * Draw the data lines as points on a chart
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void lv_chart_draw_points(lv_obj_t * chart, const lv_area_t * mask)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

    uint16_t i;
    lv_area_t cir_a;
    lv_coord_t w     = lv_obj_get_width(chart);
    lv_coord_t h     = lv_obj_get_height(chart);
    lv_coord_t x_ofs = chart->coords.x1;
    lv_coord_t y_ofs = chart->coords.y1;
    int32_t y_tmp;
    lv_coord_t p_act;
    lv_chart_series_t * ser;
    uint8_t series_cnt = 0;
    lv_style_t style_point;
    lv_style_copy(&style_point, &lv_style_plain);

    style_point.body.border.width = 0;
    style_point.body.radius       = LV_RADIUS_CIRCLE;
    style_point.body.opa          = ext->series.opa;
    style_point.body.radius       = ext->series.width;

    /*Go through all data lines*/

    LV_LL_READ_BACK(ext->series_ll, ser)
    {
        lv_coord_t start_point = ext->update_mode == LV_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

        style_point.body.main_color = ser->color;
        style_point.body.grad_color = lv_color_mix(LV_COLOR_BLACK, ser->color, ext->series.dark);

        for(i = 0; i < ext->point_cnt; i++) {
            cir_a.x1 = ((w * i) / (ext->point_cnt - 1)) + x_ofs;
            cir_a.x2 = cir_a.x1 + style_point.body.radius;
            cir_a.x1 -= style_point.body.radius;

            p_act = (start_point + i) % ext->point_cnt;
            y_tmp = (int32_t)((int32_t)ser->points[p_act] - ext->ymin) * h;
            y_tmp = y_tmp / (ext->ymax - ext->ymin);

            cir_a.y1 = h - y_tmp + y_ofs - 1;
            cir_a.y2 = cir_a.y1 + style_point.body.radius;
            cir_a.y1 -= style_point.body.radius;

            if(ser->points[p_act] != LV_CHART_POINT_DEF)
                lv_draw_rect(&cir_a, mask, &style_point, lv_obj_get_opa_scale(chart));
        }
        series_cnt++;
    }
}

/**
 * Draw the data lines as columns on a chart
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void lv_chart_draw_cols(lv_obj_t * chart, const lv_area_t * mask)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

    uint16_t i;
    lv_area_t col_a;
    lv_area_t col_mask;
    bool mask_ret;
    lv_coord_t w = lv_obj_get_width(chart);
    lv_coord_t h = lv_obj_get_height(chart);
    int32_t y_tmp;
    lv_chart_series_t * ser;
    lv_style_t rects;
    lv_coord_t col_w = w / ((ext->series.num + 1) * ext->point_cnt); /* Suppose + 1 series as separator*/
    lv_coord_t x_ofs = col_w / 2;                                    /*Shift with a half col.*/

    lv_style_copy(&rects, &lv_style_plain);
    rects.body.border.width = 0;
    rects.body.radius       = 0;
    rects.body.opa          = ext->series.opa;

    col_a.y2 = chart->coords.y2;

    lv_coord_t x_act;

    /*Go through all points*/
    for(i = 0; i < ext->point_cnt; i++) {
        x_act = (int32_t)((int32_t)w * i) / ext->point_cnt;
        x_act += chart->coords.x1 + x_ofs;

        /*Draw the current point of all data line*/
        LV_LL_READ_BACK(ext->series_ll, ser)
        {
            lv_coord_t start_point = ext->update_mode == LV_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

            col_a.x1 = x_act;
            col_a.x2 = col_a.x1 + col_w;
            x_act += col_w;

            if(col_a.x2 < mask->x1) continue;
            if(col_a.x1 > mask->x2) break;

            rects.body.main_color = ser->color;
            rects.body.grad_color = lv_color_mix(LV_COLOR_BLACK, ser->color, ext->series.dark);

            lv_coord_t p_act = (start_point + i) % ext->point_cnt;
            y_tmp            = (int32_t)((int32_t)ser->points[p_act] - ext->ymin) * h;
            y_tmp            = y_tmp / (ext->ymax - ext->ymin);
            col_a.y1         = h - y_tmp + chart->coords.y1;

            mask_ret = lv_area_intersect(&col_mask, mask, &col_a);
            if(mask_ret != false && ser->points[p_act] != LV_CHART_POINT_DEF) {
                lv_draw_rect(&chart->coords, &col_mask, &rects, lv_obj_get_opa_scale(chart));
            }
        }
    }
}

/**
 * Draw the data lines as vertical lines on a chart if there is only 1px between point
 * @param obj pointer to chart object
 */
static void lv_chart_draw_vertical_lines(lv_obj_t * chart, const lv_area_t * mask)
{

    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    lv_coord_t w         = lv_obj_get_width(chart);
    /*Vertical lines works only if the width == point count. Else use the normal line type*/
    if(ext->point_cnt != w) {
        lv_chart_draw_lines(chart, mask);
        return;
    }

    uint16_t i;
    lv_point_t p1;
    lv_point_t p2;
    lv_coord_t p_act;
    lv_coord_t h     = lv_obj_get_height(chart);
    lv_coord_t x_ofs = chart->coords.x1;
    lv_coord_t y_ofs = chart->coords.y1;
    int32_t y_tmp;
    lv_chart_series_t * ser;
    lv_opa_t opa_scale = lv_obj_get_opa_scale(chart);
    lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.line.opa   = ext->series.opa;
    style.line.width = ext->series.width;

    /*Go through all data lines*/
    LV_LL_READ_BACK(ext->series_ll, ser)
    {
        lv_coord_t start_point = ext->update_mode == LV_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;
        style.line.color       = ser->color;

        p1.x  = 0 + x_ofs;
        p2.x  = 0 + x_ofs;
        y_tmp = (int32_t)((int32_t)ser->points[0] - ext->ymin) * h;
        y_tmp = y_tmp / (ext->ymax - ext->ymin);
        p2.y  = h - y_tmp + y_ofs;
        p1.y  = p2.y;

        for(i = 0; i < ext->point_cnt; i++) {
            p_act = (start_point + i) % ext->point_cnt;

            y_tmp = (int32_t)((int32_t)ser->points[p_act] - ext->ymin) * h;
            y_tmp = y_tmp / (ext->ymax - ext->ymin);
            p2.y  = h - y_tmp + y_ofs;

            if(p1.y == p2.y) {
                p2.x++;
            }

            if(ser->points[p_act] != LV_CHART_POINT_DEF) {
                lv_draw_line(&p1, &p2, mask, &style, opa_scale);
            }

            p2.x = ((w * p_act) / (ext->point_cnt - 1)) + x_ofs;
            p1.x = p2.x;
            p1.y = p2.y;
        }
    }
}

/**
 * Draw the data lines as areas on a chart
 * @param obj pointer to chart object
 */
static void lv_chart_draw_areas(lv_obj_t * chart, const lv_area_t * mask)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

    uint16_t i;
    lv_point_t p1;
    lv_point_t p2;
    lv_coord_t w     = lv_obj_get_width(chart);
    lv_coord_t h     = lv_obj_get_height(chart);
    lv_coord_t x_ofs = chart->coords.x1;
    lv_coord_t y_ofs = chart->coords.y1;
    int32_t y_tmp;
    lv_coord_t p_prev;
    lv_coord_t p_act;
    lv_chart_series_t * ser;
    lv_opa_t opa_scale = lv_obj_get_opa_scale(chart);
    lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);

    /*Go through all data lines*/
    LV_LL_READ_BACK(ext->series_ll, ser)
    {
        lv_coord_t start_point = ext->update_mode == LV_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;
        style.body.main_color  = ser->color;
        style.body.opa         = ext->series.opa;

        p2.x = 0 + x_ofs;

        p_prev = start_point;
        y_tmp  = (int32_t)((int32_t)ser->points[p_prev] - ext->ymin) * h;
        y_tmp  = y_tmp / (ext->ymax - ext->ymin);
        p2.y   = h - y_tmp + y_ofs;

        for(i = 1; i < ext->point_cnt; i++) {
            p1.x = p2.x;
            p1.y = p2.y;

            p_act = (start_point + i) % ext->point_cnt;
            p2.x  = ((w * i) / (ext->point_cnt - 1)) + x_ofs;

            y_tmp = (int32_t)((int32_t)ser->points[p_act] - ext->ymin) * h;
            y_tmp = y_tmp / (ext->ymax - ext->ymin);
            p2.y  = h - y_tmp + y_ofs;

            if(ser->points[p_prev] != LV_CHART_POINT_DEF && ser->points[p_act] != LV_CHART_POINT_DEF) {
                lv_point_t triangle_points[3];
                triangle_points[0]   = p1;
                triangle_points[1]   = p2;
                triangle_points[2].x = p1.x;
                triangle_points[2].y = chart->coords.y2;
                lv_draw_triangle(triangle_points, mask, &style, opa_scale);
                triangle_points[2].x = p2.x;
                triangle_points[0].y = chart->coords.y2;
                lv_draw_triangle(triangle_points, mask, &style, opa_scale);
            }
            p_prev = p_act;
        }
    }
}

/**
 * Create iterator for newline-separated list
 * @param list pointer to newline-separated labels list
 * @param iterator_dir LV_CHART_ITERATOR_FORWARD or LV_CHART_LABEL_ITERATOR_REVERSE
 * @return lv_chart_label_iterator_t
 */
static lv_chart_label_iterator_t lv_chart_create_label_iter(const char * list, uint8_t iterator_dir)
{
    lv_chart_label_iterator_t iterator = {0};
    uint8_t j;

    iterator.list_start = list;

    /* count number of list items */
    for(j = 0; list[j] != '\0'; j++) {
            if(list[j] == '\n')
               iterator.items_left++;
    }

    if(iterator_dir == LV_CHART_LABEL_ITERATOR_FORWARD) {
        iterator.is_reverse_iter = 0;
        iterator.current_pos = list;
    } else {
        iterator.is_reverse_iter = 1;
        // -1 to skip '\0' at the end of the string
        iterator.current_pos = list + j - 1;
    }
    iterator.items_left++;
    return iterator;
}

/**
 * Get next label from iterator created by lv_chart_create_label_iter()
 * @param iterator iterator to get label from
 * @param[out] buf buffer to point next label to
 */
static void lv_chart_get_next_label(lv_chart_label_iterator_t * iterator, char * buf)
{
    uint8_t label_len = 0;
    if (iterator->is_reverse_iter) {
        const char * label_start;
        /* count the length of the current label*/
        while ((*iterator->current_pos != '\n') &&
                (iterator->current_pos != iterator->list_start)) {
            iterator->current_pos--;
            label_len++;
        }

        label_start = iterator->current_pos;

        if (*iterator->current_pos == '\n') {
            /* do not copy \n symbol, +1 to skip it*/
            label_start++;
            /* skip newline*/
            iterator->current_pos--;
        } else {
            /* it is last label in list (first one from the beginning )*/
            label_len++;
        }

        /* do not allow output buffer overflow */
        if (label_len > LV_CHART_AXIS_TICK_LABEL_MAX_LEN) {
            label_len = LV_CHART_AXIS_TICK_LABEL_MAX_LEN;
        }

        strncpy(buf, label_start, label_len);
    } else {
        /* search for tick string */
        while(iterator->current_pos[label_len] != '\n' &&
            iterator->current_pos[label_len] != '\0') {
            /* do not overflow the buffer, but move to the end of the current label */
            if(label_len < LV_CHART_AXIS_TICK_LABEL_MAX_LEN) {
                buf[label_len] = iterator->current_pos[label_len];
                label_len++;
            } else {
                label_len++;
            }
        }

        iterator->current_pos += label_len;

        /* do not allow output buffer overflow */
        if (label_len > LV_CHART_AXIS_TICK_LABEL_MAX_LEN) {
            label_len = LV_CHART_AXIS_TICK_LABEL_MAX_LEN;
        }

        if(*iterator->current_pos == '\n') iterator->current_pos++;
    }

    /* terminate the string */
    buf[label_len] = '\0';
}

/**
 * Check whether there should be a label next to tick with given
 * number
 * @param tick_num number of the tick to check
 * @param axis pointer to struct containing info on the axis
 * @return true if label should be located next to current tick
 */
static inline bool lv_chart_is_tick_with_label(uint8_t tick_num, lv_chart_axis_cfg_t * axis)
{
    return ((tick_num == 0) || ((tick_num % axis->num_tick_marks) == 0));
}

static void lv_chart_draw_y_ticks(lv_obj_t * chart, const lv_area_t * mask, uint8_t which_axis)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);
    lv_chart_axis_cfg_t * y_axis = (which_axis == LV_CHART_AXIS_PRIMARY_Y) ?
                                            &ext->y_axis : &ext->secondary_y_axis;

    if(y_axis->list_of_values != NULL || y_axis->num_tick_marks != 0) {

        const lv_style_t * style = lv_obj_get_style(chart);
        lv_opa_t opa_scale       = lv_obj_get_opa_scale(chart);

        uint8_t i;
        uint8_t num_of_labels;
        uint8_t num_scale_ticks;
        int8_t major_tick_len, minor_tick_len;
        uint8_t iter_dir;

        lv_point_t p1;
        lv_point_t p2;
        lv_coord_t x_ofs;
        lv_chart_label_iterator_t iter;
        lv_coord_t y_ofs = chart->coords.y1;
        lv_coord_t h     = lv_obj_get_height(chart);
        lv_coord_t w     = lv_obj_get_width(chart);
        char buf[LV_CHART_AXIS_TICK_LABEL_MAX_LEN + 1]; /* up to N symbols per label + null terminator */

        /* chose correct side of the chart */
        if(which_axis == LV_CHART_AXIS_PRIMARY_Y)
            x_ofs = chart->coords.x1;
        else
            x_ofs = chart->coords.x2;

        /* calculate the size of tick marks */
        if(y_axis->major_tick_len == LV_CHART_TICK_LENGTH_AUTO)
            major_tick_len = (int32_t)w * LV_CHART_AXIS_MAJOR_TICK_LEN_COE;
        else
            major_tick_len = y_axis->major_tick_len;

        if(y_axis->minor_tick_len == LV_CHART_TICK_LENGTH_AUTO)
            minor_tick_len = major_tick_len * LV_CHART_AXIS_MINOR_TICK_LEN_COE;
        else
            minor_tick_len = y_axis->minor_tick_len;

        /* tick lines on secondary y axis are drawn in other direction*/
        if(which_axis == LV_CHART_AXIS_SECONDARY_Y) {
            major_tick_len *= -1;
            minor_tick_len *= -1;
        }

        iter_dir = (y_axis->options & LV_CHART_AXIS_INVERSE_LABELS_ORDER) ? LV_CHART_LABEL_ITERATOR_REVERSE : LV_CHART_LABEL_ITERATOR_FORWARD;
        iter = lv_chart_create_label_iter(y_axis->list_of_values, iter_dir);

        /*determine the number of options */
        num_of_labels = iter.items_left;

        /* we can't have string labels without ticks step, set to 1 if not specified */
        if(y_axis->num_tick_marks == 0) y_axis->num_tick_marks = 1;

        /* calculate total number of ticks */
        if(num_of_labels < 2)
            num_scale_ticks = y_axis->num_tick_marks;
        else
            num_scale_ticks = (y_axis->num_tick_marks * (num_of_labels - 1));

        for(i = 0; i < (num_scale_ticks + 1); i++) { /* one extra loop - it may not exist in the list, empty label */
                                                     /* first point of the tick */
            p1.x = x_ofs;

            /* move extra pixel out of chart boundary */
            if (which_axis == LV_CHART_AXIS_PRIMARY_Y)
                p1.x--;
            else
                p1.x++;

            /* second point of the tick */
            if((num_of_labels != 0) && (i == 0 || i % y_axis->num_tick_marks == 0))
                p2.x = p1.x - major_tick_len; /* major tick */
            else
                p2.x = p1.x - minor_tick_len; /* minor tick */

            /* draw a line at moving y position */
            p2.y = p1.y =
                y_ofs + (int32_t)((int32_t)(h - style->line.width) * i) / num_scale_ticks;

            if(y_axis->options & LV_CHART_AXIS_INVERSE_LABELS_ORDER) {
                /*if label order is inversed last tick have number 0*/
                if(i != 0)
                    lv_draw_line(&p1, &p2, mask, style, opa_scale);
                else if((y_axis->options & LV_CHART_AXIS_DRAW_LAST_TICK) != 0)
                    lv_draw_line(&p1, &p2, mask, style, opa_scale);
            } else {
                if(i != num_scale_ticks)
                    lv_draw_line(&p1, &p2, mask, style, opa_scale);
                else if((y_axis->options & LV_CHART_AXIS_DRAW_LAST_TICK) != 0)
                    lv_draw_line(&p1, &p2, mask, style, opa_scale);
            }

            /* draw values if available */
            if(num_of_labels != 0) {
                /* add text only to major tick */
                if(lv_chart_is_tick_with_label(i, y_axis)) {

                    lv_chart_get_next_label(&iter, buf);

                    /* reserve appropriate area */
                    lv_point_t size;
                    lv_txt_get_size(&size, buf, style->text.font, style->text.letter_space, style->text.line_space,
                                    LV_COORD_MAX, LV_TXT_FLAG_CENTER);

                    /* set the area at some distance of the major tick len left of the tick */
                    lv_area_t a = {.y1 = p2.y - size.y / 2, .y2 = p2.y + size.y / 2};

                    if(which_axis == LV_CHART_AXIS_PRIMARY_Y) {
                        a.x1 = p2.x - size.x - LV_CHART_AXIS_TO_LABEL_DISTANCE;
                        a.x2 = p2.x - LV_CHART_AXIS_TO_LABEL_DISTANCE;
                    } else {
                        a.x1 = p2.x + LV_CHART_AXIS_TO_LABEL_DISTANCE;
                        a.x2 = p2.x + size.x + LV_CHART_AXIS_TO_LABEL_DISTANCE;
                    }

                    lv_draw_label(&a, mask, style, opa_scale, buf, LV_TXT_FLAG_CENTER, NULL, NULL, NULL, lv_obj_get_base_dir(chart));
                }
            }

        }
    }
}

static void lv_chart_draw_x_ticks(lv_obj_t * chart, const lv_area_t * mask)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

    if(ext->x_axis.list_of_values != NULL || ext->x_axis.num_tick_marks != 0) {

        const lv_style_t * style = lv_obj_get_style(chart);
        lv_opa_t opa_scale       = lv_obj_get_opa_scale(chart);

        uint8_t i;
        uint8_t num_of_labels;
        uint8_t num_scale_ticks;
        uint8_t major_tick_len, minor_tick_len;
        lv_chart_label_iterator_t iter;
        lv_point_t p1;
        lv_point_t p2;
        lv_coord_t x_ofs = chart->coords.x1;
        lv_coord_t y_ofs = chart->coords.y1;
        lv_coord_t h     = lv_obj_get_height(chart);
        lv_coord_t w     = lv_obj_get_width(chart);
        char buf[LV_CHART_AXIS_TICK_LABEL_MAX_LEN + 1]; /* up to N symbols per label + null terminator */

        /* calculate the size of tick marks */
        if(ext->x_axis.major_tick_len == LV_CHART_TICK_LENGTH_AUTO)
            major_tick_len = (int32_t)w * LV_CHART_AXIS_MAJOR_TICK_LEN_COE;
        else
            major_tick_len = ext->x_axis.major_tick_len;

        if(ext->x_axis.minor_tick_len == LV_CHART_TICK_LENGTH_AUTO)
            minor_tick_len = major_tick_len * LV_CHART_AXIS_MINOR_TICK_LEN_COE;
        else
            minor_tick_len = ext->x_axis.minor_tick_len;

        /*determine the number of options */
        iter = lv_chart_create_label_iter(ext->x_axis.list_of_values, LV_CHART_LABEL_ITERATOR_FORWARD);
        num_of_labels = iter.items_left;

        /* we can't have string labels without ticks step, set to 1 if not specified */
        if(ext->x_axis.num_tick_marks == 0) ext->x_axis.num_tick_marks = 1;

        /* calculate total number of marks */
        if(num_of_labels < 2)
            num_scale_ticks = ext->x_axis.num_tick_marks;
        else
            num_scale_ticks = (ext->x_axis.num_tick_marks * (num_of_labels - 1));

        for(i = 0; i < (num_scale_ticks + 1); i++) { /* one extra loop - it may not exist in the list, empty label */
                                                     /* first point of the tick */
            p1.y = h + y_ofs;

            /* second point of the tick */
            if((num_of_labels != 0) && (i == 0 || i % ext->x_axis.num_tick_marks == 0))
                p2.y = p1.y + major_tick_len; /* major tick */
            else
                p2.y = p1.y + minor_tick_len; /* minor tick */

            /* draw a line at moving x position */
            p2.x = p1.x = x_ofs + (int32_t)((int32_t)(w - style->line.width) * i) / num_scale_ticks;

            if(i != num_scale_ticks)
                lv_draw_line(&p1, &p2, mask, style, opa_scale);
            else if((ext->x_axis.options & LV_CHART_AXIS_DRAW_LAST_TICK) != 0)
                lv_draw_line(&p1, &p2, mask, style, opa_scale);

            /* draw values if available */
            if(num_of_labels != 0) {
                /* add text only to major tick */
                if(lv_chart_is_tick_with_label(i, &(ext->x_axis))) {
                    lv_chart_get_next_label(&iter, buf);

                    /* reserve appropriate area */
                    lv_point_t size;
                    lv_txt_get_size(&size, buf, style->text.font, style->text.letter_space, style->text.line_space,
                                    LV_COORD_MAX, LV_TXT_FLAG_CENTER);

                    /* set the area at some distance of the major tick len under of the tick */
                    lv_area_t a = {(p2.x - size.x / 2), (p2.y + LV_CHART_AXIS_TO_LABEL_DISTANCE), (p2.x + size.x / 2),
                                   (p2.y + size.y + LV_CHART_AXIS_TO_LABEL_DISTANCE)};
                    lv_draw_label(&a, mask, style, opa_scale, buf, LV_TXT_FLAG_CENTER, NULL, NULL, NULL, lv_obj_get_base_dir(chart));
                }
            }
        }
    }
}

static void lv_chart_draw_axes(lv_obj_t * chart, const lv_area_t * mask)
{
    lv_chart_draw_y_ticks(chart, mask, LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_draw_y_ticks(chart, mask, LV_CHART_AXIS_SECONDARY_Y);
    lv_chart_draw_x_ticks(chart, mask);
}

/**
 * invalid area of the new line data lines on a chart
 * @param obj pointer to chart object
 */
static void lv_chart_inv_lines(lv_obj_t * chart, uint16_t i)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

    lv_coord_t w     = lv_obj_get_width(chart);
    lv_coord_t x_ofs = chart->coords.x1;

    if(i < ext->point_cnt) {
        lv_area_t coords;
        lv_obj_get_coords(chart, &coords);
        if(i < ext->point_cnt - 1) {
            coords.x1 = ((w * i) / (ext->point_cnt - 1)) + x_ofs - ext->series.width;
            coords.x2 = ((w * (i + 1)) / (ext->point_cnt - 1)) + x_ofs + ext->series.width;
            lv_obj_invalidate_area(chart, &coords);
        }

        if(i > 0) {
            coords.x1 = ((w * (i - 1)) / (ext->point_cnt - 1)) + x_ofs - ext->series.width;
            coords.x2 = ((w * i) / (ext->point_cnt - 1)) + x_ofs + ext->series.width;
            lv_obj_invalidate_area(chart, &coords);
        }
    }
}

/**
 * invalid area of the new point data lines on a chart
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void lv_chart_inv_points(lv_obj_t * chart, uint16_t i)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

    lv_area_t cir_a;
    lv_coord_t w     = lv_obj_get_width(chart);
    lv_coord_t x_ofs = chart->coords.x1;

    lv_obj_get_coords(chart, &cir_a);
    cir_a.x1 = ((w * i) / (ext->point_cnt - 1)) + x_ofs;
    cir_a.x2 = cir_a.x1 + ext->series.width;
    cir_a.x1 -= ext->series.width;

    lv_inv_area(lv_obj_get_disp(chart), &cir_a);
}

/**
 * invalid area of the new column data lines on a chart
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void lv_chart_inv_cols(lv_obj_t * chart, uint16_t i)
{
    lv_chart_ext_t * ext = lv_obj_get_ext_attr(chart);

    lv_area_t col_a;
    lv_coord_t w     = lv_obj_get_width(chart);
    lv_coord_t col_w = w / ((ext->series.num + 1) * ext->point_cnt); /* Suppose + 1 series as separator*/
    lv_coord_t x_ofs = col_w / 2;                                    /*Shift with a half col.*/

    lv_coord_t x_act;

    x_act = (int32_t)((int32_t)w * i) / ext->point_cnt;
    x_act += chart->coords.x1 + x_ofs;

    lv_obj_get_coords(chart, &col_a);
    col_a.x1 = x_act;
    col_a.x2 = col_a.x1 + col_w;

    lv_inv_area(lv_obj_get_disp(chart), &col_a);
}

#endif
