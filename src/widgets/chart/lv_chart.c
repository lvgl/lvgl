/**
 * @file lv_chart.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_chart_private.h"
#include "../../misc/lv_area_private.h"
#include "../../draw/lv_draw_private.h"
#include "../../core/lv_obj_private.h"
#include "../../core/lv_obj_class_private.h"
#if LV_USE_CHART != 0

#include "../../misc/lv_assert.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_chart_class)

#define LV_CHART_HDIV_DEF 3
#define LV_CHART_VDIV_DEF 5
#define LV_CHART_POINT_CNT_DEF 10
#define LV_CHART_LABEL_MAX_TEXT_LENGTH 16

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_chart_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_chart_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_chart_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void draw_div_lines(lv_obj_t * obj, lv_layer_t * layer);
static void draw_series_line(lv_obj_t * obj, lv_layer_t * layer);
static void draw_series_bar(lv_obj_t * obj, lv_layer_t * layer);
static void draw_series_scatter(lv_obj_t * obj, lv_layer_t * layer);
static void draw_cursors(lv_obj_t * obj, lv_layer_t * layer);
static uint32_t get_index_from_x(lv_obj_t * obj, int32_t x);
static void invalidate_point(lv_obj_t * obj, uint32_t i);
static void new_points_alloc(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t cnt, int32_t ** a);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_chart_class = {
    .constructor_cb = lv_chart_constructor,
    .destructor_cb = lv_chart_destructor,
    .event_cb = lv_chart_event,
    .width_def = LV_PCT(100),
    .height_def = LV_DPI_DEF * 2,
    .instance_size = sizeof(lv_chart_t),
    .base_class = &lv_obj_class,
    .name = "chart",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_chart_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_chart_set_type(lv_obj_t * obj, lv_chart_type_t type)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->type == type) return;

    if(chart->type == LV_CHART_TYPE_SCATTER) {
        lv_chart_series_t * ser;
        LV_LL_READ_BACK(&chart->series_ll, ser) {
            lv_free(ser->x_points);
            ser->x_points = NULL;
        }
    }

    if(type == LV_CHART_TYPE_SCATTER) {
        lv_chart_series_t * ser;
        LV_LL_READ_BACK(&chart->series_ll, ser) {
            ser->x_points = lv_malloc(sizeof(int32_t) * chart->point_cnt);
            LV_ASSERT_MALLOC(ser->x_points);
            if(ser->x_points == NULL) return;
        }
    }

    chart->type = type;

    lv_chart_refresh(obj);
}

void lv_chart_set_point_count(lv_obj_t * obj, uint32_t cnt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->point_cnt == cnt) return;

    lv_chart_series_t * ser;

    if(cnt < 1) cnt = 1;

    LV_LL_READ_BACK(&chart->series_ll, ser) {
        if(chart->type == LV_CHART_TYPE_SCATTER) {
            if(!ser->x_ext_buf_assigned) new_points_alloc(obj, ser, cnt, &ser->x_points);
        }
        if(!ser->y_ext_buf_assigned) new_points_alloc(obj, ser, cnt, &ser->y_points);
        ser->start_point = 0;
    }

    chart->point_cnt = cnt;

    lv_chart_refresh(obj);
}

void lv_chart_set_range(lv_obj_t * obj, lv_chart_axis_t axis, int32_t min, int32_t max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    max = max == min ? max + 1 : max;

    lv_chart_t * chart  = (lv_chart_t *)obj;
    switch(axis) {
        case LV_CHART_AXIS_PRIMARY_Y:
            chart->ymin[0] = min;
            chart->ymax[0] = max;
            break;
        case LV_CHART_AXIS_SECONDARY_Y:
            chart->ymin[1] = min;
            chart->ymax[1] = max;
            break;
        case LV_CHART_AXIS_PRIMARY_X:
            chart->xmin[0] = min;
            chart->xmax[0] = max;
            break;
        case LV_CHART_AXIS_SECONDARY_X:
            chart->xmin[1] = min;
            chart->xmax[1] = max;
            break;
        default:
            LV_LOG_WARN("Invalid axis: %d", axis);
            return;
    }

    lv_chart_refresh(obj);
}

void lv_chart_set_update_mode(lv_obj_t * obj, lv_chart_update_mode_t update_mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->update_mode == update_mode) return;

    chart->update_mode = update_mode;
    lv_obj_invalidate(obj);
}

void lv_chart_set_div_line_count(lv_obj_t * obj, uint8_t hdiv, uint8_t vdiv)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->hdiv_cnt == hdiv && chart->vdiv_cnt == vdiv) return;

    chart->hdiv_cnt = hdiv;
    chart->vdiv_cnt = vdiv;

    lv_obj_invalidate(obj);
}

lv_chart_type_t lv_chart_get_type(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    return chart->type;
}

uint32_t lv_chart_get_point_count(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    return chart->point_cnt;
}

uint32_t lv_chart_get_x_start_point(const lv_obj_t * obj, lv_chart_series_t * ser)
{
    LV_ASSERT_NULL(ser);
    LV_UNUSED(obj);

    return ser->start_point;
}

void lv_chart_get_point_pos_by_id(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t id, lv_point_t * p_out)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(ser);
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(id >= chart->point_cnt) {
        LV_LOG_WARN("Invalid index: %"LV_PRIu32, id);
        p_out->x = 0;
        p_out->y = 0;
        return;
    }

    int32_t w = lv_obj_get_content_width(obj);
    int32_t h = lv_obj_get_content_height(obj);

    if(chart->type == LV_CHART_TYPE_LINE) {
        if(chart->point_cnt > 1) {
            p_out->x = (w * id) / (chart->point_cnt - 1);
        }
        else {
            p_out->x = 0;
        }
    }
    else if(chart->type == LV_CHART_TYPE_SCATTER) {
        p_out->x = lv_map(ser->x_points[id], chart->xmin[ser->x_axis_sec], chart->xmax[ser->x_axis_sec], 0, w);
    }
    else if(chart->type == LV_CHART_TYPE_BAR) {
        uint32_t ser_cnt = lv_ll_get_len(&chart->series_ll);
        int32_t ser_gap = lv_obj_get_style_pad_column(obj, LV_PART_ITEMS);

        /*Gap between the columns on adjacent X ticks*/
        int32_t block_gap = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);

        int32_t block_w = (w - ((chart->point_cnt - 1) * block_gap)) / chart->point_cnt;

        if(chart->point_cnt > 1) {
            p_out->x = (int32_t)((int32_t)(w - block_w) * id) / (chart->point_cnt - 1);
        }
        else {
            p_out->x = 0;
        }

        lv_chart_series_t * ser_i = NULL;
        uint32_t ser_idx = 0;
        LV_LL_READ(&chart->series_ll, ser_i) {
            if(ser_i == ser) break;
            ser_idx++;
        }

        p_out->x = (int32_t)((int32_t)(w + block_gap) * id) / chart->point_cnt;
        p_out->x += block_w * ser_idx / ser_cnt;

        int32_t col_w = (block_w - (ser_gap * (ser_cnt - 1))) / ser_cnt;
        p_out->x += col_w / 2;
    }
    else {
        p_out->x = 0;
    }

    int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    p_out->x += lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
    p_out->x -= lv_obj_get_scroll_left(obj);

    uint32_t start_point = chart->update_mode == LV_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;
    id = ((int32_t)start_point + id) % chart->point_cnt;
    int32_t temp_y = 0;
    temp_y = (int32_t)((int32_t)ser->y_points[id] - chart->ymin[ser->y_axis_sec]) * h;
    temp_y = temp_y / (chart->ymax[ser->y_axis_sec] - chart->ymin[ser->y_axis_sec]);
    p_out->y = h - temp_y;
    p_out->y += lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;
    p_out->y -= lv_obj_get_scroll_top(obj);
}

void lv_chart_refresh(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_invalidate(obj);
}

/*======================
 * Series
 *=====================*/

lv_chart_series_t * lv_chart_add_series(lv_obj_t * obj, lv_color_t color, lv_chart_axis_t axis)
{
    LV_LOG_INFO("begin");

    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart    = (lv_chart_t *)obj;

    /* Allocate space for a new series and add it to the chart series linked list */
    lv_chart_series_t * ser = lv_ll_ins_tail(&chart->series_ll);
    LV_ASSERT_MALLOC(ser);
    if(ser == NULL) return NULL;
    lv_memzero(ser, sizeof(lv_chart_series_t));

    /* Allocate memory for point_cnt points, handle failure below */
    ser->y_points = lv_malloc(sizeof(int32_t) * chart->point_cnt);
    LV_ASSERT_MALLOC(ser->y_points);

    if(chart->type == LV_CHART_TYPE_SCATTER) {
        ser->x_points = lv_malloc(sizeof(int32_t) * chart->point_cnt);
        LV_ASSERT_MALLOC(ser->x_points);
        if(NULL == ser->x_points) {
            lv_free(ser->y_points);
            lv_ll_remove(&chart->series_ll, ser);
            lv_free(ser);
            return NULL;
        }
    }
    else {
        ser->x_points = NULL;
    }

    if(ser->y_points == NULL) {
        if(ser->x_points) {
            lv_free(ser->x_points);
            ser->x_points = NULL;
        }

        lv_ll_remove(&chart->series_ll, ser);
        lv_free(ser);
        return NULL;
    }

    /* Set series properties on successful allocation */
    ser->color = color;
    ser->start_point = 0;
    ser->y_ext_buf_assigned = false;
    ser->hidden = 0;
    ser->x_axis_sec = axis & LV_CHART_AXIS_SECONDARY_X ? 1 : 0;
    ser->y_axis_sec = axis & LV_CHART_AXIS_SECONDARY_Y ? 1 : 0;

    uint32_t i;
    const int32_t def = LV_CHART_POINT_NONE;
    int32_t * p_tmp = ser->y_points;
    for(i = 0; i < chart->point_cnt; i++) {
        *p_tmp = def;
        p_tmp++;
    }

    return ser;
}

void lv_chart_remove_series(lv_obj_t * obj, lv_chart_series_t * series)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(series);

    lv_chart_t * chart    = (lv_chart_t *)obj;
    if(!series->y_ext_buf_assigned && series->y_points) lv_free(series->y_points);
    if(!series->x_ext_buf_assigned && series->x_points) lv_free(series->x_points);

    lv_ll_remove(&chart->series_ll, series);
    lv_free(series);

    return;
}

void lv_chart_hide_series(lv_obj_t * chart, lv_chart_series_t * series, bool hide)
{
    LV_ASSERT_OBJ(chart, MY_CLASS);
    LV_ASSERT_NULL(series);

    series->hidden = hide ? 1 : 0;
    lv_chart_refresh(chart);
}

void lv_chart_set_series_color(lv_obj_t * chart, lv_chart_series_t * series, lv_color_t color)
{
    LV_ASSERT_OBJ(chart, MY_CLASS);
    LV_ASSERT_NULL(series);

    series->color = color;
    lv_chart_refresh(chart);
}

lv_color_t lv_chart_get_series_color(lv_obj_t * chart, const lv_chart_series_t * series)
{
    LV_ASSERT_OBJ(chart, MY_CLASS);
    LV_ASSERT_NULL(series);
    LV_UNUSED(chart);

    return series->color;
}

void lv_chart_set_x_start_point(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t id)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(id >= chart->point_cnt) return;
    ser->start_point = id;
}

lv_chart_series_t * lv_chart_get_series_next(const lv_obj_t * obj, const lv_chart_series_t * ser)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(ser == NULL) return lv_ll_get_head(&chart->series_ll);
    else return lv_ll_get_next(&chart->series_ll, ser);
}

/*=====================
 * Cursor
 *====================*/

lv_chart_cursor_t  * lv_chart_add_cursor(lv_obj_t * obj, lv_color_t color, lv_dir_t dir)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    lv_chart_cursor_t * cursor = lv_ll_ins_head(&chart->cursor_ll);
    LV_ASSERT_MALLOC(cursor);
    if(cursor == NULL) return NULL;

    lv_point_set(&cursor->pos, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE);
    cursor->point_id = LV_CHART_POINT_NONE;
    cursor->pos_set = 0;
    cursor->color = color;
    cursor->dir = dir;

    return cursor;
}

void lv_chart_set_cursor_pos(lv_obj_t * chart, lv_chart_cursor_t * cursor, lv_point_t * pos)
{
    LV_ASSERT_NULL(cursor);
    LV_UNUSED(chart);

    cursor->pos = *pos;
    cursor->pos_set = 1;
    lv_chart_refresh(chart);
}

void lv_chart_set_cursor_point(lv_obj_t * chart, lv_chart_cursor_t * cursor, lv_chart_series_t * ser, uint32_t point_id)
{
    LV_ASSERT_NULL(cursor);
    LV_UNUSED(chart);

    cursor->point_id = point_id;
    cursor->pos_set = 0;
    if(ser == NULL) ser = lv_chart_get_series_next(chart, NULL);
    cursor->ser = ser;
    lv_chart_refresh(chart);
}

lv_point_t lv_chart_get_cursor_point(lv_obj_t * chart, lv_chart_cursor_t * cursor)
{
    LV_ASSERT_NULL(cursor);
    LV_UNUSED(chart);

    return cursor->pos;
}

/*=====================
 * Set/Get value(s)
 *====================*/

void lv_chart_set_all_values(lv_obj_t * obj, lv_chart_series_t * ser, int32_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    uint32_t i;
    for(i = 0; i < chart->point_cnt; i++) {
        ser->y_points[i] = value;
    }
    ser->start_point = 0;
    lv_chart_refresh(obj);
}

void lv_chart_set_next_value(lv_obj_t * obj, lv_chart_series_t * ser, int32_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    ser->y_points[ser->start_point] = value;
    invalidate_point(obj, ser->start_point);
    ser->start_point = (ser->start_point + 1) % chart->point_cnt;
    invalidate_point(obj, ser->start_point);
}

void lv_chart_set_next_value2(lv_obj_t * obj, lv_chart_series_t * ser, int32_t x_value, int32_t y_value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;

    if(chart->type != LV_CHART_TYPE_SCATTER) {
        LV_LOG_WARN("Type must be LV_CHART_TYPE_SCATTER");
        return;
    }

    ser->x_points[ser->start_point] = x_value;
    ser->y_points[ser->start_point] = y_value;
    ser->start_point = (ser->start_point + 1) % chart->point_cnt;
    invalidate_point(obj, ser->start_point);
}

void lv_chart_set_value_by_id(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t id, int32_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    lv_chart_t * chart  = (lv_chart_t *)obj;

    if(id >= chart->point_cnt) return;
    ser->y_points[id] = value;
    invalidate_point(obj, id);
}

void lv_chart_set_value_by_id2(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t id, int32_t x_value,
                               int32_t y_value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    lv_chart_t * chart  = (lv_chart_t *)obj;

    if(chart->type != LV_CHART_TYPE_SCATTER) {
        LV_LOG_WARN("Type must be LV_CHART_TYPE_SCATTER");
        return;
    }

    if(id >= chart->point_cnt) return;
    ser->x_points[id] = x_value;
    ser->y_points[id] = y_value;
    invalidate_point(obj, id);
}

void lv_chart_set_ext_y_array(lv_obj_t * obj, lv_chart_series_t * ser, int32_t array[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    if(!ser->y_ext_buf_assigned && ser->y_points) lv_free(ser->y_points);
    ser->y_ext_buf_assigned = true;
    ser->y_points = array;
    lv_obj_invalidate(obj);
}

void lv_chart_set_ext_x_array(lv_obj_t * obj, lv_chart_series_t * ser, int32_t array[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    if(!ser->x_ext_buf_assigned && ser->x_points) lv_free(ser->x_points);
    ser->x_ext_buf_assigned = true;
    ser->x_points = array;
    lv_obj_invalidate(obj);
}

int32_t * lv_chart_get_y_array(const lv_obj_t * obj, lv_chart_series_t * ser)
{
    LV_UNUSED(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    return ser->y_points;
}

int32_t * lv_chart_get_x_array(const lv_obj_t * obj, lv_chart_series_t * ser)
{
    LV_UNUSED(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    return ser->x_points;
}

uint32_t lv_chart_get_pressed_point(const lv_obj_t * obj)
{
    lv_chart_t * chart = (lv_chart_t *)obj;
    return chart->pressed_point_id;
}

int32_t lv_chart_get_first_point_center_offset(lv_obj_t * obj)
{
    lv_chart_t * chart = (lv_chart_t *)obj;

    int32_t x_ofs = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    if(chart->type == LV_CHART_TYPE_BAR) {
        lv_obj_update_layout(obj);
        /*Gap between the columns on ~adjacent X*/
        int32_t block_gap = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);
        int32_t w = lv_obj_get_content_width(obj);
        int32_t block_w = (w + block_gap) / (chart->point_cnt);

        x_ofs += (block_w - block_gap) / 2;
    }

    return x_ofs;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_chart_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_chart_t * chart = (lv_chart_t *)obj;

    lv_ll_init(&chart->series_ll, sizeof(lv_chart_series_t));
    lv_ll_init(&chart->cursor_ll, sizeof(lv_chart_cursor_t));

    chart->ymin[0] = 0;
    chart->xmin[0] = 0;
    chart->ymin[1] = 0;
    chart->xmin[1] = 0;
    chart->ymax[0] = 100;
    chart->xmax[0] = 100;
    chart->ymax[1] = 100;
    chart->xmax[1] = 100;

    chart->hdiv_cnt    = LV_CHART_HDIV_DEF;
    chart->vdiv_cnt    = LV_CHART_VDIV_DEF;
    chart->point_cnt   = LV_CHART_POINT_CNT_DEF;
    chart->pressed_point_id  = LV_CHART_POINT_NONE;
    chart->type        = LV_CHART_TYPE_LINE;
    chart->update_mode = LV_CHART_UPDATE_MODE_SHIFT;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_chart_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_chart_t * chart = (lv_chart_t *)obj;
    lv_chart_series_t * ser;
    while(chart->series_ll.head) {
        ser = lv_ll_get_head(&chart->series_ll);
        if(!ser) continue;

        if(!ser->y_ext_buf_assigned) lv_free(ser->y_points);
        if(!ser->x_ext_buf_assigned) lv_free(ser->x_points);

        lv_ll_remove(&chart->series_ll, ser);
        lv_free(ser);
    }
    lv_ll_clear(&chart->series_ll);

    lv_chart_cursor_t * cur;
    while(chart->cursor_ll.head) {
        cur = lv_ll_get_head(&chart->cursor_ll);
        lv_ll_remove(&chart->cursor_ll, cur);
        lv_free(cur);
    }
    lv_ll_clear(&chart->cursor_ll);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_chart_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    /*Call the ancestor's event handler*/
    lv_result_t res;

    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);

    lv_chart_t * chart  = (lv_chart_t *)obj;
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
        chart->pressed_point_id = LV_CHART_POINT_NONE;
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        lv_layer_t * layer = lv_event_get_layer(e);
        draw_div_lines(obj, layer);

        if(lv_ll_is_empty(&chart->series_ll) == false) {
            if(chart->type == LV_CHART_TYPE_LINE) draw_series_line(obj, layer);
            else if(chart->type == LV_CHART_TYPE_BAR) draw_series_bar(obj, layer);
            else if(chart->type == LV_CHART_TYPE_SCATTER) draw_series_scatter(obj, layer);
        }

        draw_cursors(obj, layer);
    }
}

static void draw_div_lines(lv_obj_t * obj, lv_layer_t * layer)
{
    lv_chart_t * chart  = (lv_chart_t *)obj;

    lv_area_t series_clip_area;
    bool mask_ret = lv_area_intersect(&series_clip_area, &obj->coords, &layer->_clip_area);
    if(mask_ret == false) return;

    const lv_area_t clip_area_ori = layer->_clip_area;
    layer->_clip_area = series_clip_area;

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
    if(chart->hdiv_cnt != 0) {
        int32_t y_ofs = obj->coords.y1 + pad_top - scroll_top;
        line_dsc.p1.x = obj->coords.x1;
        line_dsc.p2.x = obj->coords.x2;

        i_start = 0;
        i_end = chart->hdiv_cnt;
        if(border_opa > LV_OPA_MIN && border_w > 0) {
            if((border_side & LV_BORDER_SIDE_TOP) && (lv_obj_get_style_pad_top(obj, LV_PART_MAIN) == 0)) i_start++;
            if((border_side & LV_BORDER_SIDE_BOTTOM) && (lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN) == 0)) i_end--;
        }

        for(i = i_start; i < i_end; i++) {
            line_dsc.p1.y = (int32_t)((int32_t)h * i) / (chart->hdiv_cnt - 1);
            line_dsc.p1.y += y_ofs;
            line_dsc.p2.y = line_dsc.p1.y;
            line_dsc.base.id1 = i;

            lv_draw_line(layer, &line_dsc);
        }
    }

    if(chart->vdiv_cnt != 0) {
        int32_t x_ofs = obj->coords.x1 + pad_left - scroll_left;
        line_dsc.p1.y = obj->coords.y1;
        line_dsc.p2.y = obj->coords.y2;
        i_start = 0;
        i_end = chart->vdiv_cnt;
        if(border_opa > LV_OPA_MIN && border_w > 0) {
            if((border_side & LV_BORDER_SIDE_LEFT) && (lv_obj_get_style_pad_left(obj, LV_PART_MAIN) == 0)) i_start++;
            if((border_side & LV_BORDER_SIDE_RIGHT) && (lv_obj_get_style_pad_right(obj, LV_PART_MAIN) == 0)) i_end--;
        }

        for(i = i_start; i < i_end; i++) {
            line_dsc.p1.x = (int32_t)((int32_t)w * i) / (chart->vdiv_cnt - 1);
            line_dsc.p1.x += x_ofs;
            line_dsc.p2.x = line_dsc.p1.x;
            line_dsc.base.id1 = i;

            lv_draw_line(layer, &line_dsc);
        }
    }

    layer->_clip_area = clip_area_ori;
}

static void draw_series_line(lv_obj_t * obj, lv_layer_t * layer)
{
    lv_area_t clip_area;
    if(lv_area_intersect(&clip_area, &obj->coords, &layer->_clip_area) == false) return;

    const lv_area_t clip_area_ori = layer->_clip_area;
    layer->_clip_area = clip_area;

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->point_cnt < 2) return;

    uint32_t i;
    int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    int32_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
    int32_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;
    int32_t w     = lv_obj_get_content_width(obj);
    int32_t h     = lv_obj_get_content_height(obj);
    int32_t x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
    int32_t y_ofs = obj->coords.y1 + pad_top - lv_obj_get_scroll_top(obj);
    lv_chart_series_t * ser;

    lv_area_t series_clip_area;
    bool mask_ret = lv_area_intersect(&series_clip_area, &obj->coords, &layer->_clip_area);
    if(mask_ret == false) return;

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

    /*If there are at least as many points as pixels then draw only vertical lines*/
    bool crowded_mode = (int32_t)chart->point_cnt >= w;

    line_dsc.base.id1 = lv_ll_get_len(&chart->series_ll) - 1;
    point_dsc_default.base.id1 = line_dsc.base.id1;
    /*Go through all data lines*/
    LV_LL_READ_BACK(&chart->series_ll, ser) {
        if(ser->hidden) {
            line_dsc.base.id1--;
            point_dsc_default.base.id1--;
            continue;
        }
        line_dsc.color = ser->color;
        point_dsc_default.bg_color = ser->color;
        line_dsc.base.id2 = 0;
        point_dsc_default.base.id2 = 0;

        int32_t start_point = chart->update_mode == LV_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

        line_dsc.p1.x = x_ofs;
        line_dsc.p2.x = x_ofs;

        int32_t p_act = start_point;
        int32_t p_prev = start_point;
        int32_t y_tmp = (int32_t)((int32_t)ser->y_points[p_prev] - chart->ymin[ser->y_axis_sec]) * h;
        y_tmp  = y_tmp / (chart->ymax[ser->y_axis_sec] - chart->ymin[ser->y_axis_sec]);
        line_dsc.p2.y   = h - y_tmp + y_ofs;

        lv_value_precise_t y_min = line_dsc.p2.y;
        lv_value_precise_t y_max = line_dsc.p2.y;

        for(i = 0; i < chart->point_cnt; i++) {
            line_dsc.p1.x = line_dsc.p2.x;
            line_dsc.p1.y = line_dsc.p2.y;

            if(line_dsc.p1.x > clip_area_ori.x2 + point_w + 1) break;
            line_dsc.p2.x = (lv_value_precise_t)((w * i) / (chart->point_cnt - 1)) + x_ofs;

            p_act = (start_point + i) % chart->point_cnt;

            y_tmp = (int32_t)((int32_t)ser->y_points[p_act] - chart->ymin[ser->y_axis_sec]) * h;
            y_tmp = y_tmp / (chart->ymax[ser->y_axis_sec] - chart->ymin[ser->y_axis_sec]);
            line_dsc.p2.y  = h - y_tmp + y_ofs;

            if(line_dsc.p2.x < clip_area_ori.x1 - point_w - 1) {
                p_prev = p_act;
                continue;
            }

            /*Don't draw the first point. A second point is also required to draw the line*/
            if(i != 0) {
                if(crowded_mode) {
                    if(ser->y_points[p_prev] != LV_CHART_POINT_NONE && ser->y_points[p_act] != LV_CHART_POINT_NONE) {
                        /*Draw only one vertical line between the min and max y-values on the same x-value*/
                        y_max = LV_MAX(y_max, line_dsc.p2.y);
                        y_min = LV_MIN(y_min, line_dsc.p2.y);
                        if(line_dsc.p1.x != line_dsc.p2.x) {
                            lv_value_precise_t y_cur = line_dsc.p2.y;
                            line_dsc.p2.x--;         /*It's already on the next x value*/
                            line_dsc.p1.x = line_dsc.p2.x;
                            line_dsc.p1.y = y_min;
                            line_dsc.p2.y = y_max;
                            if(line_dsc.p1.y == line_dsc.p2.y) line_dsc.p2.y++;    /*If they are the same no line will be drawn*/
                            lv_draw_line(layer, &line_dsc);
                            line_dsc.p2.x++;         /*Compensate the previous x--*/
                            y_min = y_cur;  /*Start the line of the next x from the current last y*/
                            y_max = y_cur;
                        }
                    }
                }
                else {
                    lv_area_t point_area;
                    point_area.x1 = (int32_t)line_dsc.p1.x - point_w;
                    point_area.x2 = (int32_t)line_dsc.p1.x + point_w;
                    point_area.y1 = (int32_t)line_dsc.p1.y - point_h;
                    point_area.y2 = (int32_t)line_dsc.p1.y + point_h;

                    if(ser->y_points[p_prev] != LV_CHART_POINT_NONE && ser->y_points[p_act] != LV_CHART_POINT_NONE) {
                        line_dsc.base.id2 = i;
                        lv_draw_line(layer, &line_dsc);
                    }

                    if(point_w && point_h && ser->y_points[p_prev] != LV_CHART_POINT_NONE) {
                        point_dsc_default.base.id2 = i - 1;
                        lv_draw_rect(layer, &point_dsc_default, &point_area);
                    }
                }

            }
            p_prev = p_act;
        }

        /*Draw the last point*/
        if(!crowded_mode && i == chart->point_cnt) {

            if(ser->y_points[p_act] != LV_CHART_POINT_NONE) {
                lv_area_t point_area;
                point_area.x1 = (int32_t)line_dsc.p2.x - point_w;
                point_area.x2 = (int32_t)line_dsc.p2.x + point_w;
                point_area.y1 = (int32_t)line_dsc.p2.y - point_h;
                point_area.y2 = (int32_t)line_dsc.p2.y + point_h;
                point_dsc_default.base.id2 = i - 1;
                lv_draw_rect(layer, &point_dsc_default, &point_area);
            }
        }

        point_dsc_default.base.id1--;
        line_dsc.base.id1--;
    }

    layer->_clip_area = clip_area_ori;
}

static void draw_series_scatter(lv_obj_t * obj, lv_layer_t * layer)
{

    lv_area_t clip_area;
    if(lv_area_intersect(&clip_area, &obj->coords, &layer->_clip_area) == false) return;

    const lv_area_t clip_area_ori = layer->_clip_area;
    layer->_clip_area = clip_area;

    lv_chart_t * chart  = (lv_chart_t *)obj;

    uint32_t i;
    int32_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    int32_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    int32_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    int32_t w     = lv_obj_get_content_width(obj);
    int32_t h     = lv_obj_get_content_height(obj);
    int32_t x_ofs = obj->coords.x1 + pad_left + border_width - lv_obj_get_scroll_left(obj);
    int32_t y_ofs = obj->coords.y1 + pad_top + border_width - lv_obj_get_scroll_top(obj);
    lv_chart_series_t * ser;

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

        int32_t start_point = chart->update_mode == LV_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

        line_dsc.p1.x = x_ofs;
        line_dsc.p2.x = x_ofs;

        int32_t p_act = start_point;
        int32_t p_prev = start_point;
        if(ser->y_points[p_act] != LV_CHART_POINT_CNT_DEF) {
            line_dsc.p2.x = lv_map(ser->x_points[p_act], chart->xmin[ser->x_axis_sec], chart->xmax[ser->x_axis_sec], 0, w);
            line_dsc.p2.x += x_ofs;

            line_dsc.p2.y = lv_map(ser->y_points[p_act], chart->ymin[ser->y_axis_sec], chart->ymax[ser->y_axis_sec], 0, h);
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
            if(ser->y_points[p_act] != LV_CHART_POINT_NONE) {
                line_dsc.p2.y = lv_map(ser->y_points[p_act], chart->ymin[ser->y_axis_sec], chart->ymax[ser->y_axis_sec], 0, h);
                line_dsc.p2.y = h - line_dsc.p2.y;
                line_dsc.p2.y += y_ofs;

                line_dsc.p2.x = lv_map(ser->x_points[p_act], chart->xmin[ser->x_axis_sec], chart->xmax[ser->x_axis_sec], 0, w);
                line_dsc.p2.x += x_ofs;
            }
            else {
                p_prev = p_act;
                continue;
            }

            /*Don't draw the first point. A second point is also required to draw the line*/
            if(i != 0) {
                lv_area_t point_area;
                point_area.x1 = (int32_t)line_dsc.p1.x - point_w;
                point_area.x2 = (int32_t)line_dsc.p1.x + point_w;
                point_area.y1 = (int32_t)line_dsc.p1.y - point_h;
                point_area.y2 = (int32_t)line_dsc.p1.y + point_h;

                if(ser->y_points[p_prev] != LV_CHART_POINT_NONE && ser->y_points[p_act] != LV_CHART_POINT_NONE) {
                    line_dsc.base.id2 = i;
                    lv_draw_line(layer, &line_dsc);
                    if(point_w && point_h) {
                        point_dsc_default.base.id2 = i;
                        lv_draw_rect(layer, &point_dsc_default, &point_area);
                    }
                }

                p_prev = p_act;
            }

            /*Draw the last point*/
            if(i == chart->point_cnt) {

                if(ser->y_points[p_act] != LV_CHART_POINT_NONE) {
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
        layer->_clip_area = clip_area_ori;
    }
}

static void draw_series_bar(lv_obj_t * obj, lv_layer_t * layer)
{
    lv_area_t clip_area;
    if(lv_area_intersect(&clip_area, &obj->coords, &layer->_clip_area) == false) return;

    const lv_area_t clip_area_ori = layer->_clip_area;
    layer->_clip_area = clip_area;

    lv_chart_t * chart  = (lv_chart_t *)obj;

    uint32_t i;
    lv_area_t col_a;
    int32_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    int32_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    int32_t w = lv_obj_get_content_width(obj);
    int32_t h  = lv_obj_get_content_height(obj);
    int32_t y_tmp;
    lv_chart_series_t * ser;
    uint32_t ser_cnt = lv_ll_get_len(&chart->series_ll);
    if(ser_cnt == 0) {
        return;
    }
    int32_t block_gap = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);  /*Gap between the column on ~adjacent X*/
    int32_t block_w = (w - ((chart->point_cnt - 1) * block_gap)) / chart->point_cnt;
    int32_t ser_gap = lv_obj_get_style_pad_column(obj, LV_PART_ITEMS); /*Gap between the columns on the ~same X*/
    int32_t col_w = (block_w - (ser_cnt - 1) * ser_gap) / ser_cnt;
    if(col_w < 1) col_w  = 1;

    int32_t border_w = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    int32_t x_ofs = pad_left - lv_obj_get_scroll_left(obj) + border_w;
    int32_t y_ofs = pad_top - lv_obj_get_scroll_top(obj) + border_w;

    lv_draw_rect_dsc_t col_dsc;
    lv_draw_rect_dsc_init(&col_dsc);
    col_dsc.base.layer = layer;
    lv_obj_init_draw_rect_dsc(obj, LV_PART_ITEMS, &col_dsc);
    col_dsc.bg_grad.dir = LV_GRAD_DIR_NONE;
    col_dsc.bg_opa = LV_OPA_COVER;

    /*Make the cols longer with `radius` to clip the rounding from the bottom*/
    col_a.y2 = obj->coords.y2 + col_dsc.radius;

    /*Go through all points*/
    for(i = 0; i < chart->point_cnt; i++) {
        int32_t x_act;
        if(chart->point_cnt <= 1) {
            x_act = obj->coords.x1 + x_ofs;
        }
        else {
            x_act = (int32_t)((int32_t)(w - block_w) * i) / (chart->point_cnt - 1) + obj->coords.x1 + x_ofs;
        }

        col_dsc.base.id2 = i;
        col_dsc.base.id1 = 0;

        /*Draw the current point of all data line*/
        LV_LL_READ(&chart->series_ll, ser) {
            if(ser->hidden) continue;

            int32_t start_point = chart->update_mode == LV_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

            col_a.x1 = x_act;
            col_a.x2 = col_a.x1 + col_w - 1;
            x_act += col_w + ser_gap;

            if(col_a.x2 < clip_area.x1) {
                col_dsc.base.id1++;
                continue;
            }
            if(col_a.x1 > clip_area.x2) break;

            col_dsc.bg_color = ser->color;

            int32_t p_act = (start_point + i) % chart->point_cnt;
            y_tmp            = (int32_t)((int32_t)ser->y_points[p_act] - chart->ymin[ser->y_axis_sec]) * h;
            y_tmp            = y_tmp / (chart->ymax[ser->y_axis_sec] - chart->ymin[ser->y_axis_sec]);
            col_a.y1         = h - y_tmp + obj->coords.y1 + y_ofs;

            if(ser->y_points[p_act] != LV_CHART_POINT_NONE) {
                lv_draw_rect(layer, &col_dsc, &col_a);
            }
            col_dsc.base.id1++;
        }
    }
    layer->_clip_area = clip_area_ori;
}

static void draw_cursors(lv_obj_t * obj, lv_layer_t * layer)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(lv_ll_is_empty(&chart->cursor_ll)) return;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, &layer->_clip_area, &obj->coords)) return;

    const lv_area_t clip_area_ori = layer->_clip_area;
    layer->_clip_area = clip_area;

    lv_chart_cursor_t * cursor;

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
            if(cursor->point_id == LV_CHART_POINT_NONE) continue;
            lv_point_t p;
            lv_chart_get_point_pos_by_id(obj, cursor->ser, cursor->point_id, &p);
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

    layer->_clip_area = clip_area_ori;
}

/**
 * Get the nearest index to an X coordinate
 * @param chart pointer to a chart object
 * @param coord the coordination of the point relative to the series area.
 * @return the found index
 */
static uint32_t get_index_from_x(lv_obj_t * obj, int32_t x)
{
    lv_chart_t * chart  = (lv_chart_t *)obj;
    int32_t w = lv_obj_get_content_width(obj);
    int32_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    x -= pad_left;

    if(x < 0) return 0;
    if(x > w) return chart->point_cnt - 1;
    if(chart->type == LV_CHART_TYPE_LINE) return (x * (chart->point_cnt - 1) + w / 2) / w;
    if(chart->type == LV_CHART_TYPE_BAR) return (x * chart->point_cnt) / w;

    return 0;
}

static void invalidate_point(lv_obj_t * obj, uint32_t i)
{
    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(i >= chart->point_cnt) return;

    int32_t w  = lv_obj_get_content_width(obj);
    int32_t scroll_left = lv_obj_get_scroll_left(obj);

    /*In shift mode the whole chart changes so the whole object*/
    if(chart->update_mode == LV_CHART_UPDATE_MODE_SHIFT) {
        lv_obj_invalidate(obj);
        return;
    }

    if(chart->type == LV_CHART_TYPE_LINE) {
        int32_t bwidth = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
        int32_t pleft = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
        int32_t x_ofs = obj->coords.x1 + pleft + bwidth - scroll_left;
        int32_t line_width = lv_obj_get_style_line_width(obj, LV_PART_ITEMS);
        int32_t point_w = lv_obj_get_style_width(obj, LV_PART_INDICATOR);

        lv_area_t coords;
        lv_area_copy(&coords, &obj->coords);
        coords.y1 -= line_width + point_w;
        coords.y2 += line_width + point_w;

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
    else if(chart->type == LV_CHART_TYPE_BAR) {
        lv_area_t col_a;
        /*Gap between the column on ~adjacent X*/
        int32_t block_gap = lv_obj_get_style_pad_column(obj, LV_PART_MAIN);

        int32_t block_w = (w + block_gap) / chart->point_cnt;

        int32_t bwidth = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
        int32_t x_act;
        x_act = (int32_t)((int32_t)(block_w) * i) ;
        x_act += obj->coords.x1 + bwidth + lv_obj_get_style_pad_left(obj, LV_PART_MAIN);

        lv_obj_get_coords(obj, &col_a);
        col_a.x1 = x_act - scroll_left;
        col_a.x2 = col_a.x1 + block_w;
        col_a.x1 -= block_gap;

        lv_obj_invalidate_area(obj, &col_a);
    }
    else {
        lv_obj_invalidate(obj);
    }
}

static void new_points_alloc(lv_obj_t * obj, lv_chart_series_t * ser, uint32_t cnt, int32_t ** a)
{
    if((*a) == NULL) return;

    lv_chart_t * chart = (lv_chart_t *) obj;
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
                new_points[i] = LV_CHART_POINT_NONE; /*Fill up the rest with default value*/
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
                (*a)[i] = LV_CHART_POINT_NONE;
            }
        }
    }
}

#endif
