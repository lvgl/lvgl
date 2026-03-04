/**
 * @file lv_polarchart.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_polarchart_private.h"
#if LV_USE_POLARCHART != 0

#include "../../misc/lv_area_private.h"
#include "../../draw/lv_draw_private.h"
#include "../../draw/lv_draw_vector_private.h"
#include "../../core/lv_obj_private.h"
#include "../../core/lv_obj_class_private.h"
#include "../../core/lv_obj_draw_private.h"
#include "../../misc/lv_assert.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_polarchart_class)

#define LV_POLARCHART_RADIAL_DIV_DEF 5
#define LV_POLARCHART_ANGLE_DIV_DEF 8
#define LV_POLARCHART_POINT_CNT_DEF 10
#define LV_POLARCHART_LABEL_MAX_TEXT_LENGTH 16

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_polarchart_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_polarchart_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_polarchart_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void draw_div_lines(lv_obj_t * obj, lv_layer_t * layer);
static void draw_series_line(lv_obj_t * obj, lv_layer_t * layer);
static void draw_series_curve(lv_obj_t * obj, lv_layer_t * layer);
static void draw_series_scatter(lv_obj_t * obj, lv_layer_t * layer);
static void draw_cursors(lv_obj_t * obj, lv_layer_t * layer);
static uint32_t get_index_from_x(lv_obj_t * obj, int32_t x);
static void invalidate_point(lv_obj_t * obj, uint32_t i);
static void new_points_alloc(lv_obj_t * obj, lv_polarchart_series_t * ser, uint32_t cnt, int32_t ** a);
static int32_t value_to_y(lv_obj_t * obj, lv_polarchart_series_t * ser, int32_t v, int32_t h);

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_USE_OBJ_PROPERTY
static const lv_property_ops_t lv_polarchart_properties[] = {
    {
        .id = LV_PROPERTY_POLARCHART_TYPE,
        .setter = lv_polarchart_set_type,
        .getter = lv_polarchart_get_type,
    },
    {
        .id = LV_PROPERTY_POLARCHART_POINT_COUNT,
        .setter = lv_polarchart_set_point_count,
        .getter = lv_polarchart_get_point_count,
    },
    {
        .id = LV_PROPERTY_POLARCHART_UPDATE_MODE,
        .setter = lv_polarchart_set_update_mode,
        .getter = lv_polarchart_get_update_mode,
    },
    {
        .id = LV_PROPERTY_POLARCHART_RADIAL_DIV_LINE_COUNT,
        .setter = lv_polarchart_set_radial_div_line_count,
        .getter = lv_polarchart_get_radial_div_line_count,
    },
    {
        .id = LV_PROPERTY_POLARCHART_VER_DIV_LINE_COUNT,
        .setter = lv_polarchart_set_angle_div_line_count,
        .getter = lv_polarchart_get_angle_div_line_count,
    },
};
#endif

const lv_obj_class_t lv_polarchart_class = {
    .constructor_cb = lv_polarchart_constructor,
    .destructor_cb = lv_polarchart_destructor,
    .event_cb = lv_polarchart_event,
    .width_def = LV_PCT(100),
    .height_def = LV_DPI_DEF * 2,
    .instance_size = sizeof(lv_polarchart_t),
    .base_class = &lv_obj_class,
    .name = "lv_polarchart",
    LV_PROPERTY_CLASS_FIELDS(chart, POLARCHART)
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_polarchart_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_polarchart_set_type(lv_obj_t * obj, lv_polarchart_type_t type)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(chart->type == type) return;

    if(chart->type == LV_POLARCHART_TYPE_SCATTER) {
        lv_polarchart_series_t * ser;
        LV_LL_READ_BACK(&chart->series_ll, ser) {
            lv_free(ser->angle_points);
            ser->angle_points = NULL;
        }
    }

    if(type == LV_POLARCHART_TYPE_SCATTER) {
        lv_polarchart_series_t * ser;
        LV_LL_READ_BACK(&chart->series_ll, ser) {
            ser->angle_points = lv_malloc(sizeof(int32_t) * chart->point_cnt);
            LV_ASSERT_MALLOC(ser->angle_points);
            if(ser->angle_points == NULL) return;
        }
    }

    chart->type = type;

    lv_polarchart_refresh(obj);
}

void lv_polarchart_set_point_count(lv_obj_t * obj, uint32_t cnt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(chart->point_cnt == cnt) return;

    lv_polarchart_series_t * ser;

    if(cnt < 1) cnt = 1;

    LV_LL_READ_BACK(&chart->series_ll, ser) {
        if(chart->type == LV_POLARCHART_TYPE_SCATTER) {
            if(!ser->angle_ext_buf_assigned) new_points_alloc(obj, ser, cnt, &ser->angle_points);
        }
        if(!ser->radial_ext_buf_assigned) new_points_alloc(obj, ser, cnt, &ser->radial_points);
        ser->start_point = 0;
    }

    chart->point_cnt = cnt;

    lv_polarchart_refresh(obj);
}

void lv_polarchart_set_axis_min_value(lv_obj_t * obj, lv_polarchart_axis_t axis, int32_t min)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;

    switch(axis) {
        case LV_POLARCHART_AXIS_RADIAL:
            if(chart->radial_min == min) return;
            chart->radial_min = min;
            break;
        case LV_POLARCHART_AXIS_ANGLE:
            return;
        default:
            LV_LOG_WARN("Invalid axis: %d", axis);
            return;
    }

    lv_polarchart_refresh(obj);
}

void lv_polarchart_set_axis_max_value(lv_obj_t * obj, lv_polarchart_axis_t axis, int32_t max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    switch(axis) {
        case LV_POLARCHART_AXIS_RADIAL:
            if(chart->radial_max == max) return;
            chart->radial_max = max;
            break;
        case LV_POLARCHART_AXIS_ANGLE:
            return;
        default:
            LV_LOG_WARN("Invalid axis: %d", axis);
            return;
    }

    lv_polarchart_refresh(obj);
}

void lv_polarchart_set_axis_range(lv_obj_t * obj, lv_polarchart_axis_t axis, int32_t min, int32_t max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_set_axis_min_value(obj, axis, min);
    lv_polarchart_set_axis_max_value(obj, axis, max);
}

void lv_polarchart_set_update_mode(lv_obj_t * obj, lv_polarchart_update_mode_t update_mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(chart->update_mode == update_mode) return;

    chart->update_mode = update_mode;
    lv_obj_invalidate(obj);
}

void lv_polarchart_set_div_line_count(lv_obj_t * obj, uint32_t angle_div, uint32_t radial_div)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(chart->angle_div_cnt == angle_div && chart->radial_div_cnt == radial_div) return;

    chart->angle_div_cnt = angle_div;
    chart->radial_div_cnt = radial_div;

    lv_obj_invalidate(obj);
}

void lv_polarchart_set_radial_div_line_count(lv_obj_t * obj, uint32_t cnt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(chart->radial_div_cnt == cnt) return;
    chart->radial_div_cnt = cnt;
    lv_obj_invalidate(obj);
}

void lv_polarchart_set_angle_div_line_count(lv_obj_t * obj, uint32_t cnt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(chart->angle_div_cnt == cnt) return;
    chart->angle_div_cnt = cnt;
    lv_obj_invalidate(obj);
}

lv_polarchart_type_t lv_polarchart_get_type(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    return chart->type;
}

uint32_t lv_polarchart_get_point_count(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    return chart->point_cnt;
}

lv_polarchart_update_mode_t lv_polarchart_get_update_mode(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * polarchart  = (lv_polarchart_t *)obj;
    return polarchart->update_mode;
}

uint32_t lv_polarchart_get_radial_div_line_count(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * polarchart  = (lv_polarchart_t *)obj;
    return polarchart->radial_div_cnt;
}

uint32_t lv_polarchart_get_angle_div_line_count(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * polarchart  = (lv_polarchart_t *)obj;
    return polarchart->angle_div_cnt;
}

uint32_t lv_polarchart_get_angle_start_point(const lv_obj_t * obj, lv_polarchart_series_t * ser)
{
    LV_ASSERT_NULL(ser);
    LV_UNUSED(obj);

    return ser->start_point;
}

void lv_polarchart_get_point_pos_by_id(lv_obj_t * obj, lv_polarchart_series_t * ser, uint32_t id, lv_point_t * p_out)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(ser);
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(id >= chart->point_cnt) {
        LV_LOG_WARN("Invalid index: %"LV_PRIu32, id);
        p_out->x = 0;
        p_out->y = 0;
        return;
    }

    int32_t w = lv_obj_get_content_width(obj);
    int32_t h = lv_obj_get_content_height(obj);

    if(chart->type == LV_POLARCHART_TYPE_LINE || chart->type == LV_POLARCHART_TYPE_CURVE) {
        if(chart->point_cnt > 1) {
            p_out->x = (w * id) / (chart->point_cnt - 1);
        }
        else {
            p_out->x = 0;
        }
        int32_t temp_y = value_to_y(obj, ser, ser->radial_points[id], h);
        p_out->y = h - temp_y;
    }
    else if(chart->type == LV_POLARCHART_TYPE_SCATTER) {
        p_out->x = lv_map(ser->angle_points[id], chart->angle_min, chart->angle_max, 0, w);
        int32_t temp_y = value_to_y(obj, ser, ser->radial_points[id], h);
        p_out->y = h - temp_y;
    }
    /*LV_POLARCHART_TYPE_NONE*/
    else {
        p_out->x = 0;
        p_out->y = 0;
    }

    int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    p_out->x += lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
    p_out->x -= lv_obj_get_scroll_left(obj);

    uint32_t start_point = chart->update_mode == LV_POLARCHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;
    id = ((int32_t)start_point + id) % chart->point_cnt;

    p_out->y += lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;
    p_out->y -= lv_obj_get_scroll_top(obj);
}

void lv_polarchart_refresh(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_invalidate(obj);
}

/*======================
 * Series
 *=====================*/

lv_polarchart_series_t * lv_polarchart_add_series(lv_obj_t * obj, lv_color_t color, lv_polarchart_axis_t axis)
{
    LV_LOG_INFO("begin");

    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_UNUSED(axis);

    lv_polarchart_t * chart    = (lv_polarchart_t *)obj;

    /* Allocate space for a new series and add it to the chart series linked list */
    lv_polarchart_series_t * ser = lv_ll_ins_tail(&chart->series_ll);
    LV_ASSERT_MALLOC(ser);
    if(ser == NULL) return NULL;
    lv_memzero(ser, sizeof(lv_polarchart_series_t));

    /* Allocate memory for point_cnt points, handle failure below */
    ser->radial_points = lv_malloc(sizeof(int32_t) * chart->point_cnt);
    LV_ASSERT_MALLOC(ser->radial_points);

    if(chart->type == LV_POLARCHART_TYPE_SCATTER) {
        ser->angle_points = lv_malloc(sizeof(int32_t) * chart->point_cnt);
        LV_ASSERT_MALLOC(ser->angle_points);
        if(NULL == ser->angle_points) {
            lv_free(ser->radial_points);
            lv_ll_remove(&chart->series_ll, ser);
            lv_free(ser);
            return NULL;
        }
    }
    else {
        ser->angle_points = NULL;
    }

    if(ser->radial_points == NULL) {
        if(ser->angle_points) {
            lv_free(ser->angle_points);
            ser->angle_points = NULL;
        }

        lv_ll_remove(&chart->series_ll, ser);
        lv_free(ser);
        return NULL;
    }

    /* Set series properties on successful allocation */
    ser->color = color;
    ser->start_point = 0;
    ser->radial_ext_buf_assigned = false;
    ser->hidden = 0;

    uint32_t i;
    const int32_t def = LV_POLARCHART_POINT_NONE;
    int32_t * p_tmp = ser->radial_points;
    for(i = 0; i < chart->point_cnt; i++) {
        *p_tmp = def;
        p_tmp++;
    }

    return ser;
}

void lv_polarchart_remove_series(lv_obj_t * obj, lv_polarchart_series_t * series)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(series);

    lv_polarchart_t * chart    = (lv_polarchart_t *)obj;
    if(!series->radial_ext_buf_assigned && series->radial_points) lv_free(series->radial_points);
    if(!series->angle_ext_buf_assigned && series->angle_points) lv_free(series->angle_points);

    lv_ll_remove(&chart->series_ll, series);
    lv_free(series);

    return;
}

void lv_polarchart_hide_series(lv_obj_t * chart, lv_polarchart_series_t * series, bool hide)
{
    LV_ASSERT_OBJ(chart, MY_CLASS);
    LV_ASSERT_NULL(series);

    series->hidden = hide ? 1 : 0;
    lv_polarchart_refresh(chart);
}

void lv_polarchart_set_series_color(lv_obj_t * chart, lv_polarchart_series_t * series, lv_color_t color)
{
    LV_ASSERT_OBJ(chart, MY_CLASS);
    LV_ASSERT_NULL(series);

    series->color = color;
    lv_polarchart_refresh(chart);
}

lv_color_t lv_polarchart_get_series_color(lv_obj_t * chart, const lv_polarchart_series_t * series)
{
    LV_ASSERT_OBJ(chart, MY_CLASS);
    LV_ASSERT_NULL(series);
    LV_UNUSED(chart);

    return series->color;
}

void lv_polarchart_set_angle_start_point(lv_obj_t * obj, lv_polarchart_series_t * ser, uint32_t id)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(id >= chart->point_cnt) return;
    ser->start_point = id;
}

lv_polarchart_series_t * lv_polarchart_get_series_next(const lv_obj_t * obj, const lv_polarchart_series_t * ser)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(ser == NULL) return lv_ll_get_head(&chart->series_ll);
    else return lv_ll_get_next(&chart->series_ll, ser);
}

/*=====================
 * Cursor
 *====================*/

lv_polarchart_cursor_t  * lv_polarchart_add_cursor(lv_obj_t * obj, lv_color_t color, lv_dir_t dir)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    lv_polarchart_cursor_t * cursor = lv_ll_ins_head(&chart->cursor_ll);
    LV_ASSERT_MALLOC(cursor);
    if(cursor == NULL) return NULL;

    lv_point_set(&cursor->pos, LV_POLARCHART_POINT_NONE, LV_POLARCHART_POINT_NONE);
    cursor->point_id = LV_POLARCHART_POINT_NONE;
    cursor->pos_set = 0;
    cursor->color = color;
    cursor->dir = dir;

    return cursor;
}

void lv_polarchart_remove_cursor(lv_obj_t * obj, lv_polarchart_cursor_t * cursor)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(cursor);

    lv_polarchart_t * chart = (lv_polarchart_t *)obj;
    lv_ll_remove(&chart->cursor_ll, cursor);
    lv_free(cursor);
}

void lv_polarchart_set_cursor_pos(lv_obj_t * chart, lv_polarchart_cursor_t * cursor, lv_point_t * pos)
{
    LV_ASSERT_NULL(cursor);
    LV_UNUSED(chart);

    cursor->pos = *pos;
    cursor->pos_set = 1;
    lv_polarchart_refresh(chart);
}

void lv_polarchart_set_cursor_pos_angle(lv_obj_t * chart, lv_polarchart_cursor_t * cursor, int32_t angle)
{
    LV_ASSERT_NULL(cursor);
    LV_UNUSED(chart);

    cursor->pos.x = angle;
    cursor->pos_set = 1;
    lv_polarchart_refresh(chart);
}

void lv_polarchart_set_cursor_pos_radial(lv_obj_t * chart, lv_polarchart_cursor_t * cursor, int32_t radial)
{
    LV_ASSERT_NULL(cursor);
    LV_UNUSED(chart);

    cursor->pos.y = radial;
    cursor->pos_set = 1;
    lv_polarchart_refresh(chart);
}

void lv_polarchart_set_cursor_point(lv_obj_t * chart, lv_polarchart_cursor_t * cursor, lv_polarchart_series_t * ser,
                                    uint32_t point_id)
{
    LV_ASSERT_NULL(cursor);
    LV_UNUSED(chart);

    cursor->point_id = point_id;
    cursor->pos_set = 0;
    if(ser == NULL) ser = lv_polarchart_get_series_next(chart, NULL);
    cursor->ser = ser;
    lv_polarchart_refresh(chart);
}

lv_point_t lv_polarchart_get_cursor_point(lv_obj_t * chart, lv_polarchart_cursor_t * cursor)
{
    LV_ASSERT_NULL(cursor);
    LV_UNUSED(chart);

    return cursor->pos;
}

/*=====================
 * Set/Get value(s)
 *====================*/

void lv_polarchart_set_all_values(lv_obj_t * obj, lv_polarchart_series_t * ser, int32_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    uint32_t i;
    for(i = 0; i < chart->point_cnt; i++) {
        ser->radial_points[i] = value;
    }
    ser->start_point = 0;
    lv_polarchart_refresh(obj);
}

void lv_polarchart_set_next_value(lv_obj_t * obj, lv_polarchart_series_t * ser, int32_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;

    ser->radial_points[ser->start_point] = value;
    invalidate_point(obj, ser->start_point);
    ser->start_point = (ser->start_point + 1) % chart->point_cnt;
}

void lv_polarchart_set_next_value2(lv_obj_t * obj, lv_polarchart_series_t * ser, int32_t angle_value,
                                   int32_t radial_value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;

    if(chart->type != LV_POLARCHART_TYPE_SCATTER) {
        LV_LOG_WARN("Type must be LV_POLARCHART_TYPE_SCATTER");
        return;
    }

    ser->angle_points[ser->start_point] = angle_value;
    ser->radial_points[ser->start_point] = radial_value;
    ser->start_point = (ser->start_point + 1) % chart->point_cnt;
    invalidate_point(obj, ser->start_point);
}

void lv_polarchart_set_series_values(lv_obj_t * obj, lv_polarchart_series_t * ser, const int32_t values[],
                                     size_t values_cnt)
{
    size_t i;
    for(i = 0; i < values_cnt; i++) {
        lv_polarchart_set_next_value(obj, ser, values[i]);
    }
}

void lv_polarchart_set_series_values2(lv_obj_t * obj, lv_polarchart_series_t * ser, const int32_t angle_values[],
                                      const int32_t radial_values[], size_t values_cnt)
{
    size_t i;
    for(i = 0; i < values_cnt; i++) {
        lv_polarchart_set_next_value2(obj, ser, angle_values[i], radial_values[i]);
    }
}

void lv_polarchart_set_series_value_by_id(lv_obj_t * obj, lv_polarchart_series_t * ser, uint32_t id, int32_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;

    if(id >= chart->point_cnt) return;
    ser->radial_points[id] = value;
    invalidate_point(obj, id);
}

void lv_polarchart_set_series_value_by_id2(lv_obj_t * obj, lv_polarchart_series_t * ser, uint32_t id,
                                           int32_t angle_value,
                                           int32_t radial_value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;

    if(chart->type != LV_POLARCHART_TYPE_SCATTER) {
        LV_LOG_WARN("Type must be LV_POLARCHART_TYPE_SCATTER");
        return;
    }

    if(id >= chart->point_cnt) return;
    ser->angle_points[id] = angle_value;
    ser->radial_points[id] = radial_value;
    invalidate_point(obj, id);
}

void lv_polarchart_set_series_ext_radial_array(lv_obj_t * obj, lv_polarchart_series_t * ser, int32_t array[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    if(!ser->radial_ext_buf_assigned && ser->radial_points) lv_free(ser->radial_points);
    ser->radial_ext_buf_assigned = true;
    ser->radial_points = array;
    lv_obj_invalidate(obj);
}

void lv_polarchart_set_series_ext_angle_array(lv_obj_t * obj, lv_polarchart_series_t * ser, int32_t array[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    if(!ser->angle_ext_buf_assigned && ser->angle_points) lv_free(ser->angle_points);
    ser->angle_ext_buf_assigned = true;
    ser->angle_points = array;
    lv_obj_invalidate(obj);
}

int32_t * lv_polarchart_get_series_radial_array(const lv_obj_t * obj, lv_polarchart_series_t * ser)
{
    LV_UNUSED(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    return ser->radial_points;
}

int32_t * lv_polarchart_get_series_angle_array(const lv_obj_t * obj, lv_polarchart_series_t * ser)
{
    LV_UNUSED(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    return ser->angle_points;
}

uint32_t lv_polarchart_get_pressed_point(const lv_obj_t * obj)
{
    lv_polarchart_t * chart = (lv_polarchart_t *)obj;
    return chart->pressed_point_id;
}

int32_t lv_polarchart_get_first_point_center_offset(lv_obj_t * obj)
{
    int32_t x_ofs = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);

    return x_ofs;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_polarchart_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_polarchart_t * chart = (lv_polarchart_t *)obj;

    lv_ll_init(&chart->series_ll, sizeof(lv_polarchart_series_t));
    lv_ll_init(&chart->cursor_ll, sizeof(lv_polarchart_cursor_t));

    chart->radial_min = 0;
    chart->angle_min = 0;
    chart->radial_max = 100;
    chart->angle_max = 360;

    chart->radial_div_cnt    = LV_POLARCHART_RADIAL_DIV_DEF;
    chart->angle_div_cnt    = LV_POLARCHART_ANGLE_DIV_DEF;
    chart->point_cnt   = LV_POLARCHART_POINT_CNT_DEF;
    chart->pressed_point_id  = LV_POLARCHART_POINT_NONE;
    chart->type        = LV_POLARCHART_TYPE_LINE;
    chart->update_mode = LV_POLARCHART_UPDATE_MODE_SHIFT;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_polarchart_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_polarchart_t * chart = (lv_polarchart_t *)obj;
    lv_polarchart_series_t * ser;
    while(chart->series_ll.head) {
        ser = lv_ll_get_head(&chart->series_ll);
        if(!ser) continue;

        if(!ser->radial_ext_buf_assigned) lv_free(ser->radial_points);
        if(!ser->angle_ext_buf_assigned) lv_free(ser->angle_points);

        lv_ll_remove(&chart->series_ll, ser);
        lv_free(ser);
    }
    lv_ll_clear(&chart->series_ll);

    lv_polarchart_cursor_t * cur;
    while(chart->cursor_ll.head) {
        cur = lv_ll_get_head(&chart->cursor_ll);
        lv_ll_remove(&chart->cursor_ll, cur);
        lv_free(cur);
    }
    lv_ll_clear(&chart->cursor_ll);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_polarchart_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    /*Call the ancestor's event handler*/
    lv_result_t res;

    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(code == LV_EVENT_PRESSED) {
        lv_indev_t * indev = lv_indev_active();
        lv_point_t p;
        lv_indev_get_point(indev, &p);

        p.x -= obj->coords.x1;
        uint32_t id = get_index_from_x(obj, p.x + lv_obj_get_scroll_left(obj));
        if(id != (uint32_t)chart->pressed_point_id) {
            invalidate_point(obj, id);
            invalidate_point(obj, chart->pressed_point_id);
            chart->pressed_point_id = id;
            lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, NULL);
        }
    }
    else if(code == LV_EVENT_RELEASED) {
        invalidate_point(obj, chart->pressed_point_id);
        chart->pressed_point_id = LV_POLARCHART_POINT_NONE;
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        lv_layer_t * layer = lv_event_get_layer(e);

        lv_area_t ext_coords;
        lv_obj_get_coords(obj, &ext_coords);
        int32_t ext_draw_size = lv_obj_get_ext_draw_size(obj);
        lv_area_increase(&ext_coords, ext_draw_size, ext_draw_size);

        lv_area_t clip_area;
        if(lv_area_intersect(&clip_area, &ext_coords, &layer->_clip_area)) {
            const lv_area_t clip_area_ori = layer->_clip_area;
            layer->_clip_area = clip_area;

            draw_div_lines(obj, layer);

            if(lv_ll_is_empty(&chart->series_ll) == false) {
                if(chart->type == LV_POLARCHART_TYPE_LINE) draw_series_line(obj, layer);
                else if(chart->type == LV_POLARCHART_TYPE_CURVE) draw_series_curve(obj, layer);
                else if(chart->type == LV_POLARCHART_TYPE_SCATTER) draw_series_scatter(obj, layer);
            }

            draw_cursors(obj, layer);

            layer->_clip_area = clip_area_ori;
        }
    }
}

static void draw_div_lines(lv_obj_t * obj, lv_layer_t * layer)
{
    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;

    int16_t i;
    int16_t i_start;
    int16_t i_end;
    int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    int32_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
    int32_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;
    int32_t w = lv_obj_get_content_width(obj);
    int32_t h = lv_obj_get_content_height(obj);

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.base.layer = layer;
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);

    lv_opa_t border_opa = lv_obj_get_style_border_opa(obj, LV_PART_MAIN);
    int32_t border_w = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_border_side_t border_side = lv_obj_get_style_border_side(obj, LV_PART_MAIN);

    int32_t scroll_left = lv_obj_get_scroll_left(obj);
    int32_t scroll_top = lv_obj_get_scroll_top(obj);
    if(chart->radial_div_cnt > 1) {
        int32_t y_ofs = obj->coords.y1 + pad_top - scroll_top;
        line_dsc.p1.x = obj->coords.x1;
        line_dsc.p2.x = obj->coords.x2;

        i_start = 0;
        i_end = chart->radial_div_cnt;
        if(border_opa > LV_OPA_MIN && border_w > 0) {
            if((border_side & LV_BORDER_SIDE_TOP) && (lv_obj_get_style_pad_top(obj, LV_PART_MAIN) == 0)) i_start++;
            if((border_side & LV_BORDER_SIDE_BOTTOM) && (lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN) == 0)) i_end--;
        }

        for(i = i_start; i < i_end; i++) {
            line_dsc.p1.y = (int32_t)((int32_t)h * i) / (chart->radial_div_cnt - 1);
            line_dsc.p1.y += y_ofs;
            line_dsc.p2.y = line_dsc.p1.y;
            line_dsc.base.id1 = i;

            lv_draw_line(layer, &line_dsc);
        }
    }

    if(chart->angle_div_cnt > 1) {
        int32_t x_ofs = obj->coords.x1 + pad_left - scroll_left;
        line_dsc.p1.y = obj->coords.y1;
        line_dsc.p2.y = obj->coords.y2;
        i_start = 0;
        i_end = chart->angle_div_cnt;
        if(border_opa > LV_OPA_MIN && border_w > 0) {
            if((border_side & LV_BORDER_SIDE_LEFT) && (lv_obj_get_style_pad_left(obj, LV_PART_MAIN) == 0)) i_start++;
            if((border_side & LV_BORDER_SIDE_RIGHT) && (lv_obj_get_style_pad_right(obj, LV_PART_MAIN) == 0)) i_end--;
        }

        for(i = i_start; i < i_end; i++) {
            line_dsc.p1.x = (int32_t)((int32_t)w * i) / (chart->angle_div_cnt - 1);
            line_dsc.p1.x += x_ofs;
            line_dsc.p2.x = line_dsc.p1.x;
            line_dsc.base.id1 = i;

            lv_draw_line(layer, &line_dsc);
        }
    }
}

static void draw_series_line(lv_obj_t * obj, lv_layer_t * layer)
{
    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(chart->point_cnt < 2) return;

    uint32_t ser_cnt = lv_ll_get_len(&chart->series_ll);
    if(ser_cnt == 0) return;

    int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    int32_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
    int32_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;
    int32_t w     = lv_obj_get_content_width(obj);
    int32_t h     = lv_obj_get_content_height(obj);
    int32_t x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
    int32_t y_ofs = obj->coords.y1 + pad_top - lv_obj_get_scroll_top(obj);
    lv_polarchart_series_t * ser;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.base.layer = layer;
    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &line_dsc);
    line_dsc.base.id1 = ser_cnt - 1;

    /*If there are at least as many points as pixels then draw only vertical lines*/
    bool crowded_mode = (int32_t)chart->point_cnt >= w;

    int32_t bullet_w = lv_obj_get_style_width(obj, LV_PART_INDICATOR) / 2;
    int32_t bullet_h = lv_obj_get_style_height(obj, LV_PART_INDICATOR) / 2;
    int32_t extra_space_x;
    if(chart->point_cnt <= 1) extra_space_x = 0;
    else extra_space_x = w  / (chart->point_cnt - 1) + bullet_w + line_dsc.width;

    lv_draw_rect_dsc_t point_draw_dsc;
    if(crowded_mode == false) {
        lv_draw_rect_dsc_init(&point_draw_dsc);
        lv_obj_init_draw_rect_dsc(obj, LV_PART_INDICATOR, &point_draw_dsc);
        point_draw_dsc.base.id1 = line_dsc.base.id1;
    }

    lv_point_precise_t * points = NULL;
    if(crowded_mode) {
        points = lv_malloc((w + 2 * extra_space_x) * 3 * sizeof(lv_point_precise_t));
    }
    else {
        points = lv_malloc(chart->point_cnt * sizeof(lv_point_precise_t));
    }

    if(points == NULL) {
        LV_LOG_WARN("Couldn't allocate the points array");
        return;
    }

    line_dsc.points = points;

    /*Go through all data lines*/
    LV_LL_READ_BACK(&chart->series_ll, ser) {
        if(ser->hidden) {
            if(line_dsc.base.id1 > 0) {
                line_dsc.base.id1--;
            }
            continue;
        }
        line_dsc.color = ser->color;
        line_dsc.base.drop_shadow_color = ser->color;

        int32_t start_point = chart->update_mode == LV_POLARCHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;
        int32_t p_act = start_point;
        int32_t p_prev = start_point;

        lv_value_precise_t y_min = obj->coords.y2;
        lv_value_precise_t y_max = obj->coords.y1;
        lv_value_precise_t x_prev = -10000;
        line_dsc.p1.x = x_ofs;
        line_dsc.p2.x = x_ofs;
        line_dsc.point_cnt = 0;

        uint32_t i;
        for(i = 0; i < chart->point_cnt; i++) {
            lv_value_precise_t p_x = (int32_t)((w * i) / (chart->point_cnt - 1)) + x_ofs;
            if(p_x > layer->_clip_area.x2 + extra_space_x + 1) break;
            if(p_x < layer->_clip_area.x1 - extra_space_x - 1) {
                p_prev = p_act;
                continue;
            }
            p_act = (start_point + i) % chart->point_cnt;

            lv_value_precise_t p_y;
            if(ser->radial_points[p_act] == LV_POLARCHART_POINT_NONE) {
                p_y = LV_DRAW_LINE_POINT_NONE;
            }
            else {
                int32_t v = ser->radial_points[p_act];
                int32_t min_v = chart->radial_min;
                int32_t max_v = chart->radial_max;
                p_y = (int32_t)lv_map(v, min_v, max_v, y_ofs + h, y_ofs);
            }

            /*In normal mode just collect the points here*/
            if(crowded_mode == false) {
                points[line_dsc.point_cnt].x = p_x;
                points[line_dsc.point_cnt].y = p_y;
                line_dsc.point_cnt++;
            }
            /*In crowded mode draw vertical lines from the min/max on the same X coordinate*/
            else {
                if(ser->radial_points[p_prev] != LV_POLARCHART_POINT_NONE && ser->radial_points[p_act] != LV_POLARCHART_POINT_NONE) {
                    /*Draw only one vertical line between the min and max y-values on the same x-value*/
                    y_max = LV_MAX(y_max, p_y);
                    y_min = LV_MIN(y_min, p_y);
                    if(x_prev != p_x) {
                        line_dsc.points[line_dsc.point_cnt].y = y_min;
                        line_dsc.points[line_dsc.point_cnt].x = p_x;
                        line_dsc.points[line_dsc.point_cnt + 1].y = y_max;
                        line_dsc.points[line_dsc.point_cnt + 1].x = p_x;
                        line_dsc.points[line_dsc.point_cnt + 2].y = LV_DRAW_LINE_POINT_NONE;
                        line_dsc.points[line_dsc.point_cnt + 2].x = p_x;

                        /*If they are the same no line would be drawn*/
                        if(line_dsc.points[line_dsc.point_cnt].y == line_dsc.points[line_dsc.point_cnt + 1].y) {
                            line_dsc.points[line_dsc.point_cnt + 1].y++;
                        }
                        y_min = p_y;  /*Start the line of the next x from the current last y*/
                        y_max = p_y;
                        x_prev = p_x;
                        line_dsc.point_cnt += 3;
                    }
                }
            }

            p_prev = p_act;
        }

        /*Draw the line from the accumulated points*/
        lv_draw_line(layer, &line_dsc);
        if(!crowded_mode) {
            point_draw_dsc.bg_color = ser->color;
            point_draw_dsc.base.id1 = line_dsc.base.id1;
            /*Add the bullets too*/
            if(bullet_w > 0 && bullet_h > 0) {
                point_draw_dsc.base.id2 = i - 1; /*Start from the last rendered point*/
                int32_t j;
                for(j = line_dsc.point_cnt - 1; j >= 0; j--) {
                    if(points[j].y == LV_DRAW_LINE_POINT_NONE) continue;

                    lv_area_t point_area;
                    point_area.x1 = (int32_t)points[j].x - bullet_w;
                    point_area.x2 = (int32_t)points[j].x + bullet_w;
                    point_area.y1 = (int32_t)points[j].y - bullet_h;
                    point_area.y2 = (int32_t)points[j].y + bullet_h;

                    lv_draw_rect(layer, &point_draw_dsc, &point_area);
                    point_draw_dsc.base.id2--;
                }
            }
        }
        line_dsc.base.id1--;
    }

    if(points) lv_free(points);
}

static void draw_series_curve(lv_obj_t * obj, lv_layer_t * layer)
{
#if LV_USE_VECTOR_GRAPHIC
    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(chart->point_cnt < 2) return;

    int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    int32_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
    int32_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;
    int32_t w     = lv_obj_get_content_width(obj);
    int32_t h     = lv_obj_get_content_height(obj);
    int32_t x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
    int32_t y_ofs = obj->coords.y1 + pad_top - lv_obj_get_scroll_top(obj);
    lv_polarchart_series_t * ser;

    lv_draw_rect_dsc_t point_dsc_default;
    lv_draw_rect_dsc_init(&point_dsc_default);
    point_dsc_default.base.layer = layer;
    lv_obj_init_draw_rect_dsc(obj, LV_PART_INDICATOR, &point_dsc_default);

    int32_t point_w = lv_obj_get_style_width(obj, LV_PART_INDICATOR) / 2;
    int32_t point_h = lv_obj_get_style_height(obj, LV_PART_INDICATOR) / 2;

    uint32_t ser_cnt = lv_ll_get_len(&chart->series_ll);
    if(ser_cnt == 0) {
        return;
    }

    float dashes[2];
    dashes[0] = lv_obj_get_style_line_dash_width(obj, LV_PART_ITEMS);
    dashes[1] = lv_obj_get_style_line_dash_gap(obj, LV_PART_ITEMS);

    lv_draw_vector_dsc_t * dsc = lv_draw_vector_dsc_create(layer);
    if(dsc == NULL) {
        LV_LOG_WARN("Couldn't allocate vector dsc");
        return;
    }
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    dsc->base.id1 = ser_cnt - 1;
    point_dsc_default.base.id1 = dsc->base.id1;
    /*Go through all data lines (series)*/
    LV_LL_READ_BACK(&chart->series_ll, ser) {
        if(ser->hidden) {
            if(dsc->base.id1 > 0) {
                dsc->base.id1--;
                point_dsc_default.base.id1--;
            }
            continue;
        }

        lv_vector_path_clear(path);

        lv_draw_vector_dsc_set_fill_opa(dsc, 0);
        lv_draw_vector_dsc_set_stroke_color(dsc, ser->color);
        lv_draw_vector_dsc_set_stroke_opa(dsc, LV_OPA_COVER);
        lv_draw_vector_dsc_set_stroke_width(dsc, 2.0f);
        if(dashes[0]) lv_draw_vector_dsc_set_stroke_dash(dsc, dashes, 2);

        point_dsc_default.bg_color = ser->color;
        dsc->base.id2 = 0;
        point_dsc_default.base.id2 = 0;

        int32_t start_point = chart->update_mode == LV_POLARCHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

        /*The X distance between points.
         *Just a rough calculation to know the extra area of interest around the chart*/
        int32_t max_dx = w / (chart->point_cnt - 1) + 1;

        lv_fpoint_t scaled_points[3];
        int32_t raw_points[3];
        int32_t s_prev = 0; /*Previous steepness around N-1 (y_diff of N-2 and N) */
        int32_t s_act = 0; /*Steepness around N (y_diff of N-1 and N+1)*/
        int32_t min_v = chart->radial_min;
        int32_t max_v = chart->radial_max;

        int32_t i;
        int32_t valid_point_cnt = 0;
        int32_t point_cnt = chart->point_cnt;
        for(i = 0; i <= point_cnt; i++) {
            int32_t p_x = ((w * i) / (point_cnt - 1)) + x_ofs;

            if(p_x > layer->_clip_area.x2 + 2 * max_dx + point_w + 1) break;
            if(p_x < layer->_clip_area.x1 - 2 * max_dx - point_w - 1) {
                continue;
            }

            /*We need 3 points to draw the curves (N-1, N, N+1)*/
            scaled_points[0] = scaled_points[1];
            scaled_points[1] = scaled_points[2];

            raw_points[0] = raw_points[1];
            raw_points[1] = raw_points[2];

            int32_t p_next = (start_point + i) % point_cnt;
            raw_points[2] = ser->radial_points[p_next];
            if(i > point_cnt - 1) {
                s_act = 0;
            }
            else {
                if(raw_points[2] == LV_POLARCHART_POINT_NONE) {
                    s_act = s_prev;
                }
                else {
                    scaled_points[2].x = p_x;
                    scaled_points[2].y = (int32_t)lv_map(ser->radial_points[p_next], min_v, max_v, y_ofs + h, y_ofs);
                    if(i == 0) {
                        scaled_points[0] = scaled_points[2];
                        scaled_points[1] = scaled_points[2];
                    }

                    if((scaled_points[2].y >= scaled_points[1].y && scaled_points[1].y >= scaled_points[0].y) ||
                       (scaled_points[2].y <= scaled_points[1].y && scaled_points[1].y <= scaled_points[0].y)) {
                        s_act = (int32_t)(scaled_points[2].y - scaled_points[0].y) / 2;
                    }
                    else {
                        s_act = 0;
                    }
                }
            }

            if(valid_point_cnt >= 2) {
                if(raw_points[0] != LV_POLARCHART_POINT_NONE && raw_points[1] != LV_POLARCHART_POINT_NONE) {
                    lv_vector_path_move_to(path, &scaled_points[0]);
                    dsc->base.id2 = i;

                    /*Average slope*/
                    int32_t dx = (int32_t)(scaled_points[1].x - scaled_points[0].x);

                    lv_fpoint_t c1 = {scaled_points[0].x + dx / 3, scaled_points[0].y + s_prev / 3};
                    lv_fpoint_t c2 = {scaled_points[1].x - dx / 3, scaled_points[1].y - s_act / 3};
                    lv_vector_path_cubic_to(path, &c1, &c2, &scaled_points[1]);
                }
                else {
                    s_act = 0;
                }
            }
            s_prev = s_act;

            if(point_w && point_h && ser->radial_points[p_next] != LV_POLARCHART_POINT_NONE) {
                lv_area_t point_area;
                point_area.x1 = (int32_t)scaled_points[2].x - point_w;
                point_area.x2 = (int32_t)scaled_points[2].x + point_w;
                point_area.y1 = (int32_t)scaled_points[2].y - point_h;
                point_area.y2 = (int32_t)scaled_points[2].y + point_h;
                point_dsc_default.base.id2 = i - 1;
                lv_draw_rect(layer, &point_dsc_default, &point_area);
            }
            valid_point_cnt++;
        }

        lv_draw_vector_dsc_add_path(dsc, path); // draw a path

        if(dsc->base.id1 > 0) {
            point_dsc_default.base.id1--;
            dsc->base.id1--;
        }
    }

    lv_draw_vector(dsc);
    lv_vector_path_delete(path);
    lv_draw_vector_dsc_delete(dsc);
#else
    LV_LOG_WARN("LV_USE_VECTOR_GRAPHIC is not enabled for LV_POLARCHART_TYPE_CURVE. Falling back to LV_POLARCHART_TYPE_LINE");
    draw_series_line(obj, layer);
#endif /*LV_USE_VECTOR_GRAPHIC*/

}


static void draw_series_scatter(lv_obj_t * obj, lv_layer_t * layer)
{
    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;

    uint32_t i;
    int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    int32_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    int32_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    int32_t w     = lv_obj_get_content_width(obj);
    int32_t h     = lv_obj_get_content_height(obj);
    int32_t x_ofs = obj->coords.x1 + pad_left + border_width - lv_obj_get_scroll_left(obj);
    int32_t y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
    lv_polarchart_series_t * ser;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.base.layer = layer;
    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &line_dsc);

    lv_draw_rect_dsc_t point_dsc_default;
    lv_draw_rect_dsc_init(&point_dsc_default);
    point_dsc_default.base.layer = layer;
    lv_obj_init_draw_rect_dsc(obj, LV_PART_INDICATOR, &point_dsc_default);

    int32_t point_w = lv_obj_get_style_width(obj, LV_PART_INDICATOR) / 2;
    int32_t point_h = lv_obj_get_style_height(obj, LV_PART_INDICATOR) / 2;

    /*Do not bother with line ending is the point will over it*/
    if(LV_MIN(point_w, point_h) > line_dsc.width / 2) line_dsc.raw_end = 1;
    if(line_dsc.width == 1) line_dsc.raw_end = 1;

    /*Go through all data lines*/
    LV_LL_READ_BACK(&chart->series_ll, ser) {
        if(ser->hidden) continue;
        line_dsc.color = ser->color;
        point_dsc_default.bg_color = ser->color;

        int32_t start_point = chart->update_mode == LV_POLARCHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

        line_dsc.p1.x = x_ofs;
        line_dsc.p2.x = x_ofs;

        int32_t p_act = start_point;
        int32_t p_prev = start_point;
        if(ser->radial_points[p_act] != LV_POLARCHART_POINT_CNT_DEF) {
            line_dsc.p2.x = lv_map(ser->angle_points[p_act], chart->angle_min, chart->angle_max, 0, w);
            line_dsc.p2.x += x_ofs;

            line_dsc.p2.y = lv_map(ser->radial_points[p_act], chart->radial_min, chart->radial_max, 0, h);
            line_dsc.p2.y = h - line_dsc.p2.y;
            line_dsc.p2.y += y_ofs;
        }
        else {
            line_dsc.p2.x = (lv_value_precise_t)LV_COORD_MIN;
            line_dsc.p2.y = (lv_value_precise_t)LV_COORD_MIN;
        }

        for(i = 0; i < chart->point_cnt; i++) {
            line_dsc.p1.x = line_dsc.p2.x;
            line_dsc.p1.y = line_dsc.p2.y;

            p_act = (start_point + i) % chart->point_cnt;
            if(ser->radial_points[p_act] != LV_POLARCHART_POINT_NONE) {
                line_dsc.p2.y =  lv_map(ser->radial_points[p_act], chart->radial_min, chart->radial_max, 0, h);
                line_dsc.p2.y = h - line_dsc.p2.y;
                line_dsc.p2.y += y_ofs;

                line_dsc.p2.x = lv_map(ser->angle_points[p_act], chart->angle_min, chart->angle_max, 0, w);
                line_dsc.p2.x += x_ofs;
            }
            else {
                p_prev = p_act;
                continue;
            }

            if(i != 0) { /*Don't draw line *to* the first point.*/
                lv_area_t point_area;
                point_area.x1 = (int32_t)line_dsc.p1.x - point_w;
                point_area.x2 = (int32_t)line_dsc.p1.x + point_w;
                point_area.y1 = (int32_t)line_dsc.p1.y - point_h;
                point_area.y2 = (int32_t)line_dsc.p1.y + point_h;

                if(ser->radial_points[p_prev] != LV_POLARCHART_POINT_NONE && ser->radial_points[p_act] != LV_POLARCHART_POINT_NONE) {
                    line_dsc.base.id2 = i - 1;
                    lv_draw_line(layer, &line_dsc);
                    if(point_w && point_h) {
                        point_dsc_default.base.id2 = i - 1;
                        lv_draw_rect(layer, &point_dsc_default, &point_area);
                    }
                }

                p_prev = p_act;
            }

            /*Draw the last point*/
            if(i == chart->point_cnt - 1) {

                if(ser->radial_points[p_act] != LV_POLARCHART_POINT_NONE) {
                    lv_area_t point_area;
                    point_area.x1 = (int32_t)line_dsc.p2.x - point_w;
                    point_area.x2 = (int32_t)line_dsc.p2.x + point_w;
                    point_area.y1 = (int32_t)line_dsc.p2.y - point_h;
                    point_area.y2 = (int32_t)line_dsc.p2.y + point_h;

                    point_dsc_default.base.id2 = i;
                    lv_draw_rect(layer, &point_dsc_default, &point_area);
                }
            }
        }
        line_dsc.base.id1++;
        point_dsc_default.base.id1++;
    }
}

static void draw_cursors(lv_obj_t * obj, lv_layer_t * layer)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(lv_ll_is_empty(&chart->cursor_ll)) return;

    lv_polarchart_cursor_t * cursor;

    lv_draw_line_dsc_t line_dsc_ori;
    lv_draw_line_dsc_init(&line_dsc_ori);
    line_dsc_ori.base.layer = layer;
    lv_obj_init_draw_line_dsc(obj, LV_PART_CURSOR, &line_dsc_ori);

    lv_draw_rect_dsc_t point_dsc_ori;
    lv_draw_rect_dsc_init(&point_dsc_ori);
    point_dsc_ori.base.layer = layer;
    lv_obj_init_draw_rect_dsc(obj, LV_PART_CURSOR, &point_dsc_ori);

    lv_draw_line_dsc_t line_dsc;
    lv_draw_rect_dsc_t point_dsc_tmp;

    int32_t point_w = lv_obj_get_style_width(obj, LV_PART_CURSOR) / 2;
    int32_t point_h = lv_obj_get_style_width(obj, LV_PART_CURSOR) / 2;

    /*Go through all cursor lines*/
    LV_LL_READ_BACK(&chart->cursor_ll, cursor) {
        lv_memcpy(&line_dsc, &line_dsc_ori, sizeof(lv_draw_line_dsc_t));
        lv_memcpy(&point_dsc_tmp, &point_dsc_ori, sizeof(lv_draw_rect_dsc_t));
        line_dsc.color = cursor->color;
        point_dsc_tmp.bg_color = cursor->color;

        int32_t cx;
        int32_t cy;
        if(cursor->pos_set) {
            cx = cursor->pos.x;
            cy = cursor->pos.y;
        }
        else {
            if(cursor->point_id == LV_POLARCHART_POINT_NONE) continue;
            lv_point_t p;
            lv_polarchart_get_point_pos_by_id(obj, cursor->ser, cursor->point_id, &p);
            cx = p.x;
            cy = p.y;
        }

        cx += obj->coords.x1;
        cy += obj->coords.y1;

        lv_area_t point_area;
        bool draw_point = point_w && point_h;
        point_area.x1 = cx - point_w;
        point_area.x2 = cx + point_w;
        point_area.y1 = cy - point_h;
        point_area.y2 = cy + point_h;

        if(cursor->dir & LV_DIR_HOR) {
            line_dsc.p1.x = cursor->dir & LV_DIR_LEFT ? obj->coords.x1 : cx;
            line_dsc.p1.y = cy;
            line_dsc.p2.x = cursor->dir & LV_DIR_RIGHT ? obj->coords.x2 : cx;
            line_dsc.p2.y = line_dsc.p1.y;

            line_dsc.base.id2 = 0;
            point_dsc_tmp.base.id2 = 0;

            lv_draw_line(layer, &line_dsc);

            if(draw_point) {
                lv_draw_rect(layer, &point_dsc_tmp, &point_area);
            }
        }

        if(cursor->dir & LV_DIR_VER) {
            line_dsc.p1.x = cx;
            line_dsc.p1.y = cursor->dir & LV_DIR_TOP ? obj->coords.y1 : cy;
            line_dsc.p2.x = line_dsc.p1.x;
            line_dsc.p2.y = cursor->dir & LV_DIR_BOTTOM ? obj->coords.y2 : cy;

            line_dsc.base.id2 = 1;
            point_dsc_tmp.base.id2 = 1;

            lv_draw_line(layer, &line_dsc);

            if(draw_point) {
                lv_draw_rect(layer, &point_dsc_tmp, &point_area);
            }
        }
        line_dsc_ori.base.id1++;
        point_dsc_ori.base.id1++;
    }
}

/**
 * Get the nearest index to an X coordinate
 * @param chart pointer to a polarchart object
 * @param coord the coordinate of the point relative to the series area.
 * @return the found index
 */
static uint32_t get_index_from_x(lv_obj_t * obj, int32_t x)
{
    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    int32_t w = lv_obj_get_content_width(obj);
    int32_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    x -= pad_left;

    if(x < 0) return 0;
    if(x > w) return chart->point_cnt - 1;
    if(chart->type == LV_POLARCHART_TYPE_LINE ||
       chart->type == LV_POLARCHART_TYPE_CURVE) return (x * (chart->point_cnt - 1) + w / 2) / w;
    if(chart->type == LV_POLARCHART_TYPE_SCATTER) {
        /*For scatter polarcharts, the nearest id could be different depending on the series. Just check the first series.*/
        lv_polarchart_series_t * ser = lv_polarchart_get_series_next(obj, NULL);
        if(ser) {
            int32_t best_dist = INT32_MAX;
            uint32_t best_index = 0;
            for(uint32_t i = 0; i < chart->point_cnt; i++) {
                int32_t dist = LV_ABS(x - lv_map(ser->angle_points[i], chart->angle_min, chart->angle_max, 0, w));
                if(dist < best_dist) {
                    best_dist = dist;
                    best_index = i;
                }
            }
            return best_index;
        }
    }

    return 0;
}

static void invalidate_point(lv_obj_t * obj, uint32_t i)
{
    lv_polarchart_t * chart  = (lv_polarchart_t *)obj;
    if(i >= chart->point_cnt) return;


    /*In shift mode the whole chart changes so the whole object*/
    if(chart->update_mode == LV_POLARCHART_UPDATE_MODE_SHIFT) {
        lv_obj_invalidate(obj);
        return;
    }
    int32_t w  = lv_obj_get_content_width(obj);
    int32_t scroll_left = lv_obj_get_scroll_left(obj);
    int32_t bwidth = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    int32_t pleft = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    int32_t x_ofs = obj->coords.x1 + pleft + bwidth - scroll_left;

    if(chart->type == LV_POLARCHART_TYPE_LINE || chart->type == LV_POLARCHART_TYPE_CURVE) {
        int32_t line_width = lv_obj_get_style_line_width(obj, LV_PART_ITEMS);
        int32_t point_w = lv_obj_get_style_width(obj, LV_PART_INDICATOR);

        lv_area_t coords;
        lv_area_copy(&coords, &obj->coords);
        coords.y1 -= line_width + point_w;
        coords.y2 += line_width + point_w;

        /*Invalidate the area between the previous and the next points*/
        if(i < chart->point_cnt - 1) {
            coords.x1 = ((w * i) / (chart->point_cnt - 1)) + x_ofs - line_width - point_w;
            coords.x2 = ((w * (i + 1)) / (chart->point_cnt - 1)) + x_ofs + line_width + point_w;
            lv_obj_invalidate_area(obj, &coords);
        }

        if(i > 0) {
            coords.x1 = ((w * (i - 1)) / (chart->point_cnt - 1)) + x_ofs - line_width - point_w;
            coords.x2 = ((w * i) / (chart->point_cnt - 1)) + x_ofs + line_width + point_w;
            lv_obj_invalidate_area(obj, &coords);
        }
    }
    else {
        lv_obj_invalidate(obj);
    }
}

static void new_points_alloc(lv_obj_t * obj, lv_polarchart_series_t * ser, uint32_t cnt, int32_t ** a)
{
    if((*a) == NULL) return;

    lv_polarchart_t * chart = (lv_polarchart_t *) obj;
    uint32_t point_cnt_old = chart->point_cnt;
    uint32_t i;

    if(ser->start_point != 0) {
        int32_t * new_points = lv_malloc(sizeof(int32_t) * cnt);
        LV_ASSERT_MALLOC(new_points);
        if(new_points == NULL) return;

        if(cnt >= point_cnt_old) {
            for(i = 0; i < point_cnt_old; i++) {
                new_points[i] =
                    (*a)[(i + ser->start_point) % point_cnt_old]; /*Copy old contents to new array*/
            }
            for(i = point_cnt_old; i < cnt; i++) {
                new_points[i] = LV_POLARCHART_POINT_NONE; /*Fill up the rest with default value*/
            }
        }
        else {
            for(i = 0; i < cnt; i++) {
                new_points[i] =
                    (*a)[(i + ser->start_point) % point_cnt_old]; /*Copy old contents to new array*/
            }
        }

        /*Switch over pointer from old to new*/
        lv_free((*a));
        (*a) = new_points;
    }
    else {
        (*a) = lv_realloc((*a), sizeof(int32_t) * cnt);
        LV_ASSERT_MALLOC((*a));
        if((*a) == NULL) return;
        /*Initialize the new points*/
        if(cnt > point_cnt_old) {
            for(i = point_cnt_old - 1; i < cnt; i++) {
                (*a)[i] = LV_POLARCHART_POINT_NONE;
            }
        }
    }
}

/**
 * Map a value to a height
 * @param obj   pointer to a chart
 * @param ser   pointer to the series
 * @param v     the value to map
 * @param h     the height to which the value needs to be mapped
 * @return      the mapped y-coordinate value corresponding to the input value
 */
static int32_t value_to_y(lv_obj_t * obj, lv_polarchart_series_t * ser, int32_t v, int32_t h)
{
    LV_UNUSED(ser);

    lv_polarchart_t * chart = (lv_polarchart_t *) obj;

    return lv_map(v, chart->radial_min, chart->radial_max, 0, h);
}

#endif
