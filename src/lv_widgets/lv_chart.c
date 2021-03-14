/**
 * @file lv_chart.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_chart.h"
#if LV_USE_CHART != 0

#include "../lv_misc/lv_assert.h"
#include "../lv_core/lv_refr.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_core/lv_disp.h"
#include "../lv_core/lv_indev.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_chart_class

#define LV_CHART_YMIN_DEF 0
#define LV_CHART_YMAX_DEF 100
#define LV_CHART_HDIV_DEF 3
#define LV_CHART_VDIV_DEF 5
#define LV_CHART_POINT_CNT_DEF 10
#define LV_CHART_LABEL_ITERATOR_FORWARD 1
#define LV_CHART_LABEL_ITERATOR_REVERSE 0

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_chart_constructor(lv_obj_t * obj, const lv_obj_t * copy);
static void lv_chart_destructor(lv_obj_t * obj);
static lv_draw_res_t lv_chart_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_chart_signal(lv_obj_t * obj, lv_signal_t sign, void * param);

static void draw_div_lines(lv_obj_t * obj , const lv_area_t * mask);
static void draw_series_line(lv_obj_t * obj, const lv_area_t * clip_area);
static void draw_series_bar(lv_obj_t * obj, const lv_area_t * clip_area);
static void draw_axes(lv_obj_t * obj, const lv_area_t * mask);
static uint32_t get_index_from_x(lv_obj_t * obj, lv_coord_t x);
static void invalidate_point(lv_obj_t * obj, uint16_t i);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_chart_class = {
    .constructor_cb = lv_chart_constructor,
    .destructor_cb = lv_chart_destructor,
    .signal_cb = lv_chart_signal,
    .draw_cb = lv_chart_draw,
    .instance_size = sizeof(lv_chart_t),
    .base_class = &lv_obj_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_chart_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_INFO("begin")
    return lv_obj_create_from_class(&lv_chart_class, parent, copy);
}

void lv_chart_set_type(lv_obj_t * obj, lv_chart_type_t type)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->type == type) return;

    chart->type = type;

    lv_chart_refresh(obj);
}

void lv_chart_set_point_count(lv_obj_t * obj, uint16_t cnt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->point_cnt == cnt) return;

    lv_chart_series_t * ser;
    uint16_t point_cnt_old = chart->point_cnt;
    uint16_t i;
    lv_coord_t def = LV_CHART_POINT_DEF;

    if(cnt < 1) cnt = 1;

    _LV_LL_READ_BACK(&chart->series_ll, ser) {
        if(!ser->ext_buf_assigned) {
            if(ser->last_point != 0) {
                lv_coord_t * new_points = lv_mem_alloc(sizeof(lv_coord_t) * cnt);
                LV_ASSERT_MALLOC(new_points);
                if(new_points == NULL) return;

                if(cnt >= point_cnt_old) {
                    for(i = 0; i < point_cnt_old; i++) {
                        new_points[i] =
                                ser->points[(i + ser->last_point) % point_cnt_old]; /*Copy old contents to new array*/
                    }
                    for(i = point_cnt_old; i < cnt; i++) {
                        new_points[i] = def; /*Fill up the rest with default value*/
                    }
                }
                else {
                    for(i = 0; i < cnt; i++) {
                        new_points[i] =
                                ser->points[(i + ser->last_point) % point_cnt_old]; /*Copy old contents to new array*/
                    }
                }

                /*Switch over pointer from old to new*/
                lv_mem_free(ser->points);
                ser->points = new_points;
            }
            else {
                ser->points = lv_mem_realloc(ser->points, sizeof(lv_coord_t) * cnt);
                LV_ASSERT_MALLOC(ser->points);
                if(ser->points == NULL) return;
                /*Initialize the new points*/
                if(cnt > point_cnt_old) {
                    for(i = point_cnt_old - 1; i < cnt; i++) {
                        ser->points[i] = def;
                    }
                }
            }
        }
        ser->last_point = 0;
    }

    chart->point_cnt = cnt;

    lv_chart_refresh(obj);
}

void lv_chart_set_range(lv_obj_t * obj, lv_chart_axis_t axis, lv_coord_t min, lv_coord_t max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(axis != LV_CHART_AXIS_PRIMARY_Y && axis != LV_CHART_AXIS_SECONDARY_Y) {
        LV_LOG_WARN("Invalid axis: %d", axis);
        return;
    }

    lv_chart_t * chart  = (lv_chart_t *)obj;
    chart->ymin[axis] = min;
    chart->ymax[axis] = (max == min ? max + 1 : max);
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


void lv_chart_set_zoom_x(lv_obj_t * obj, uint16_t zoom_x)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->zoom_x == zoom_x) return;

    chart->zoom_x = zoom_x;
    lv_obj_invalidate(obj);
}

void lv_chart_set_zoom_y(lv_obj_t * obj, uint16_t zoom_y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->zoom_y == zoom_y) return;

    chart->zoom_y = zoom_y;
    lv_obj_invalidate(obj);
}

uint16_t lv_chart_get_zoom_x(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    return chart->zoom_x;
}

uint16_t lv_chart_get_zoom_y(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    return chart->zoom_y;
}

void lv_chart_set_axis_tick(lv_obj_t * obj, lv_chart_axis_t axis, lv_coord_t major_len, lv_coord_t minor_len, lv_coord_t major_cnt, lv_coord_t minor_cnt, bool label_en, lv_coord_t draw_size)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    chart->tick[axis].major_len = major_len;
    chart->tick[axis].minor_len = minor_len;
    chart->tick[axis].minor_cnt = minor_cnt;
    chart->tick[axis].major_cnt = major_cnt;
    chart->tick[axis].label_en = label_en;
    chart->tick[axis].draw_size = draw_size;

    lv_obj_refresh_ext_draw_size(obj);
    lv_obj_invalidate(obj);
}

lv_chart_type_t lv_chart_get_type(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    return chart->type;
}

uint16_t lv_chart_get_point_count(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    return chart->point_cnt;
}

uint16_t lv_chart_get_x_start_point(const lv_obj_t * obj, lv_chart_series_t * ser)
{
    LV_UNUSED(obj);
    LV_ASSERT_NULL(ser);

    return ser->last_point;
}

void lv_chart_get_point_pos_by_id(lv_obj_t * obj, lv_chart_series_t * ser, uint16_t id, lv_point_t * p_out)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(id >= chart->point_cnt) {
        LV_LOG_WARN("Invalid index: %d", id);
        p_out->x = 0;
        p_out->y = 0;
        return;
    }

    lv_coord_t w = (lv_obj_get_width_fit(obj) * chart->zoom_x) >> 8;

    if(chart->type & LV_CHART_TYPE_LINE) {
        p_out->x = (w * id) / (chart->point_cnt - 1);
    }
    else if(chart->type & LV_CHART_TYPE_BAR) {
        uint32_t ser_cnt = _lv_ll_get_len(&chart->series_ll);
        int32_t ser_gap = (lv_obj_get_style_pad_column(obj, LV_PART_ITEMS) * chart->zoom_x) >> 8; /*Gap between the column on the ~same X */
        int32_t block_gap = (lv_obj_get_style_pad_column(obj, LV_PART_MAIN) * chart->zoom_x) >> 8;  /*Gap between the column on ~adjacent X*/
        lv_coord_t block_w = (w - ((chart->point_cnt - 1) * block_gap)) / chart->point_cnt;
        lv_coord_t col_w = block_w / ser_cnt;

        p_out->x = (int32_t)((int32_t)w * id) / chart->point_cnt;

        lv_chart_series_t * ser_i = NULL;
        _LV_LL_READ_BACK(&chart->series_ll, ser_i) {
            if(ser_i == ser) break;
            p_out->x += col_w;
        }

        p_out->x += (col_w - ser_gap) / 2;
    }

    p_out->x += lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    p_out->x -= lv_obj_get_scroll_left(obj);

    lv_coord_t h = (lv_obj_get_height_fit(obj) * chart->zoom_y) >> 8;

    p_out->y = (int32_t)((int32_t)ser->points[id] - chart->ymin[ser->y_axis]) * h;
    p_out->y = p_out->y / (chart->ymax[ser->y_axis] - chart->ymin[ser->y_axis]);
    p_out->y = h - p_out->y;
    p_out->y += lv_obj_get_style_pad_top(obj, LV_PART_MAIN);

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
    LV_LOG_INFO("begin")

    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(axis != LV_CHART_AXIS_PRIMARY_Y && axis != LV_CHART_AXIS_SECONDARY_Y) {
        LV_LOG_WARN("Invalid y axis");
        return NULL;
    }

    lv_chart_t * chart    = (lv_chart_t *)obj;
    lv_chart_series_t * ser = _lv_ll_ins_head(&chart->series_ll);
    LV_ASSERT_MALLOC(ser);
    if(ser == NULL) return NULL;

    lv_coord_t def = LV_CHART_POINT_DEF;

    ser->color  = color;
    ser->points = lv_mem_alloc(sizeof(lv_coord_t) * chart->point_cnt);
    LV_ASSERT_MALLOC(ser->points);
    if(ser->points == NULL) {
        _lv_ll_remove(&chart->series_ll, ser);
        lv_mem_free(ser);
        return NULL;
    }

    ser->last_point = 0;
    ser->ext_buf_assigned = false;
    ser->hidden = 0;
    ser->y_axis = axis;

    uint16_t i;
    lv_coord_t * p_tmp = ser->points;
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
    if(!series->ext_buf_assigned && series->points) lv_mem_free(series->points);

    _lv_ll_remove(&chart->series_ll, series);
    lv_mem_free(series);

    return;
}

void lv_chart_hide_series(lv_obj_t * chart, lv_chart_series_t * series, bool hide)
{
    LV_ASSERT_OBJ(chart, MY_CLASS);
    LV_ASSERT_NULL(series);

    series->hidden = hide ? 1 : 0;
    lv_chart_refresh(chart);
}

void lv_chart_set_x_start_point(lv_obj_t * obj, lv_chart_series_t * ser, uint16_t id)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(id >= chart->point_cnt) return;
    ser->last_point = id;
}

lv_chart_series_t * lv_chart_get_series_next(const lv_obj_t * obj, const lv_chart_series_t * ser)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(ser == NULL) return _lv_ll_get_head(&chart->series_ll);
    else return _lv_ll_get_next(&chart->series_ll, ser);
}

/*=====================
 * Set/Get value(s)
 *====================*/


void lv_chart_set_all_value(lv_obj_t * obj, lv_chart_series_t * ser, lv_coord_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    uint16_t i;
    for(i = 0; i < chart->point_cnt; i++) {
        ser->points[i] = value;
    }
    ser->last_point = 0;
    lv_chart_refresh(obj);
}

void lv_chart_set_next_value(lv_obj_t * obj, lv_chart_series_t * ser, lv_coord_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(chart->update_mode == LV_CHART_UPDATE_MODE_SHIFT) {
        ser->points[ser->last_point] =
                value; /*This was the place of the former left most value, after shifting it is the rightmost*/
        ser->last_point = (ser->last_point + 1) % chart->point_cnt;
        lv_chart_refresh(obj);
    }
    else if(chart->update_mode == LV_CHART_UPDATE_MODE_CIRCULAR) {
        ser->points[ser->last_point] = value;

        invalidate_point(obj, ser->last_point);

        uint32_t next_point = (ser->last_point + 1) % chart->point_cnt; /*update the x for next incoming y*/
        ser->last_point = next_point;
        invalidate_point(obj, next_point);

        lv_obj_invalidate(obj);

    }
}

void lv_chart_set_value_by_id(lv_obj_t * obj, lv_chart_series_t * ser, lv_coord_t value, uint16_t id)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    lv_chart_t * chart  = (lv_chart_t *)obj;

    if(id >= chart->point_cnt) return;
    ser->points[id] = value;
}

void lv_chart_set_ext_array(lv_obj_t * obj, lv_chart_series_t * ser, lv_coord_t array[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);

    if(!ser->ext_buf_assigned && ser->points) lv_mem_free(ser->points);
    ser->ext_buf_assigned = true;
    ser->points = array;
    lv_obj_invalidate(obj);
}

lv_coord_t * lv_chart_get_array(const lv_obj_t * obj, lv_chart_series_t * ser)
{
    LV_UNUSED(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(ser);
    return ser->points;
}

uint32_t lv_chart_get_pressed_point(const lv_obj_t * obj)
{
    lv_chart_t * chart = (lv_chart_t *)obj;
    return chart->pressed_point_id;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_chart_constructor(lv_obj_t * obj, const lv_obj_t * copy)
{
    LV_TRACE_OBJ_CREATE("begin");

    lv_chart_t * chart = (lv_chart_t *)obj;

    _lv_ll_init(&chart->series_ll, sizeof(lv_chart_series_t));

    chart->ymin[LV_CHART_AXIS_PRIMARY_Y] = LV_CHART_YMIN_DEF;
    chart->ymax[LV_CHART_AXIS_PRIMARY_Y] = LV_CHART_YMAX_DEF;
    chart->ymin[LV_CHART_AXIS_SECONDARY_Y] = LV_CHART_YMIN_DEF;
    chart->ymax[LV_CHART_AXIS_SECONDARY_Y] = LV_CHART_YMAX_DEF;

    chart->hdiv_cnt    = LV_CHART_HDIV_DEF;
    chart->vdiv_cnt    = LV_CHART_VDIV_DEF;
    chart->point_cnt   = LV_CHART_POINT_CNT_DEF;
    chart->pressed_point_id  = LV_CHART_POINT_ID_NONE;
    chart->type        = LV_CHART_TYPE_LINE;
    chart->update_mode = LV_CHART_UPDATE_MODE_SHIFT;
    chart->zoom_x      = LV_IMG_ZOOM_NONE;
    chart->zoom_y      = LV_IMG_ZOOM_NONE;

    /*Init the new chart background object*/
    if(copy == NULL) {
        lv_obj_set_size(obj, LV_DPI_DEF * 3, LV_DPI_DEF * 2);
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
    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_chart_destructor(lv_obj_t * obj)
{
    lv_chart_t * chart = (lv_chart_t *)obj;
    lv_chart_series_t * ser;
    while(chart->series_ll.head) {
        ser = _lv_ll_get_head(&chart->series_ll);

        if(!ser->ext_buf_assigned) lv_mem_free(ser->points);

        _lv_ll_remove(&chart->series_ll, ser);
        lv_mem_free(ser);
    }
    _lv_ll_clear(&chart->series_ll);
}

static lv_draw_res_t lv_chart_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    if(mode == LV_DRAW_MODE_COVER_CHECK) {
        return lv_obj_draw_base(MY_CLASS, obj, clip_area, mode);
    }
    else if(mode == LV_DRAW_MODE_MAIN_DRAW) {
        lv_obj_draw_base(MY_CLASS, obj, clip_area, mode);

        draw_div_lines(obj, clip_area);
        draw_axes(obj, clip_area);

        lv_chart_t * chart  = (lv_chart_t *)obj;
        if(chart->type & LV_CHART_TYPE_LINE) draw_series_line(obj, clip_area);
        else if(chart->type & LV_CHART_TYPE_BAR) draw_series_bar(obj, clip_area);
    }
    else if(mode == LV_DRAW_MODE_POST_DRAW) {
        lv_obj_draw_base(MY_CLASS, obj, clip_area, mode);
    }
    return LV_DRAW_RES_OK;
}

static lv_res_t lv_chart_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    /* Include the ancient signal function */
    lv_res_t res;

    res = lv_obj_signal_base(MY_CLASS, obj, sign, param);

    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(sign == LV_SIGNAL_GET_SELF_SIZE) {
        lv_point_t * p = param;
        p->x = (lv_obj_get_width_fit(obj) * chart->zoom_x) >> 8;
        p->y = (lv_obj_get_height_fit(obj) * chart->zoom_y) >> 8;
    } else if(sign == LV_SIGNAL_PRESSED) {
        lv_indev_t * indev = lv_indev_get_act();
        lv_point_t p;
        lv_indev_get_point(indev, &p);

        p.x -= obj->coords.x1;
        uint32_t id = get_index_from_x(obj, p.x + lv_obj_get_scroll_left(obj));
        if(id != chart->pressed_point_id) {
            invalidate_point(obj, id);
            invalidate_point(obj, chart->pressed_point_id);
            chart->pressed_point_id = id;
            lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
        }
    } else if(sign == LV_SIGNAL_RELEASED) {
        chart->pressed_point_id = LV_CHART_POINT_ID_NONE;
    } else if(sign == LV_SIGNAL_REFR_EXT_DRAW_SIZE) {
        lv_coord_t * s = param;
        *s = LV_MAX4(*s, chart->tick[LV_CHART_AXIS_X].draw_size,
                     chart->tick[LV_CHART_AXIS_PRIMARY_Y].draw_size, chart->tick[LV_CHART_AXIS_SECONDARY_Y].draw_size);
    }

    return res;
}

static void draw_div_lines(lv_obj_t * obj, const lv_area_t * clip_area)
{
    lv_chart_t * chart  = (lv_chart_t *)obj;

    lv_area_t series_mask;
    bool mask_ret = _lv_area_intersect(&series_mask, &obj->coords, clip_area);
    if(mask_ret == false) return;

    uint16_t i;
    lv_point_t p1;
    lv_point_t p2;
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t w     = (lv_obj_get_width_fit(obj) * chart->zoom_x) >> 8;
    lv_coord_t h     = (lv_obj_get_height_fit(obj) * chart->zoom_y) >> 8;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);

    lv_coord_t scroll_left = lv_obj_get_scroll_left(obj);
    lv_coord_t scroll_top = lv_obj_get_scroll_top(obj);
    if(chart->hdiv_cnt != 0) {
        lv_coord_t y_ofs = obj->coords.y1 + pad_top - scroll_top;
        p1.x = obj->coords.x1;
        p2.x = obj->coords.x2;
        for(i = 0; i < chart->hdiv_cnt; i++) {
            p1.y = (int32_t)((int32_t)(h - line_dsc.width) * i) / (chart->hdiv_cnt - 1);
            p1.y += y_ofs;
            p2.y = p1.y;
            lv_draw_line(&p1, &p2, &series_mask, &line_dsc);
        }
    }

    if(chart->vdiv_cnt != 0) {
        lv_coord_t x_ofs = obj->coords.x1 + pad_left - scroll_left;
        p1.y = obj->coords.y1;
        p2.y = obj->coords.y2;
        for(i = 0; i < chart->vdiv_cnt; i++) {
            p1.x = (int32_t)((int32_t)(w - line_dsc.width) * i) / (chart->vdiv_cnt - 1);
            p1.x += x_ofs;
            p2.x = p1.x;
            lv_draw_line(&p1, &p2, &series_mask, &line_dsc);
        }
    }
}

static void draw_series_line(lv_obj_t * obj, const lv_area_t * clip_area)
{
    lv_area_t com_area;
    if(_lv_area_intersect(&com_area, &obj->coords, clip_area) == false) return;

    lv_chart_t * chart  = (lv_chart_t *)obj;

    uint16_t i;
    lv_point_t p1;
    lv_point_t p2;
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t w     = (lv_obj_get_width_fit(obj) * chart->zoom_x) >> 8;
    lv_coord_t h     = (lv_obj_get_height_fit(obj) * chart->zoom_y) >> 8;
    lv_coord_t x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
    lv_coord_t y_ofs = obj->coords.y1 + pad_top - lv_obj_get_scroll_top(obj);
    lv_chart_series_t * ser;

    lv_area_t series_mask;
    bool mask_ret = _lv_area_intersect(&series_mask, &obj->coords, clip_area);
    if(mask_ret == false) return;

    lv_state_t state_ori = obj->state;
    obj->state = LV_STATE_DEFAULT;
    obj->style_list.skip_trans = 1;
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
    obj->style_list.skip_trans = 0;

    lv_coord_t point_size_act;

    /*Do not bother with line ending is the point will over it*/
    if(point_size > line_dsc_default.width / 2) line_dsc_default.raw_end = 1;
    if(line_dsc_default.width == 1) line_dsc_default.raw_end = 1;

    /*If there are mire points than pixels draw only vertical lines*/
    bool crowded_mode = chart->point_cnt >= w ? true : false;

    /*Go through all data lines*/
    _LV_LL_READ_BACK(&chart->series_ll, ser) {
        if (ser->hidden) continue;
        line_dsc_default.color = ser->color;
        point_dsc_default.bg_color = ser->color;

        lv_coord_t start_point = chart->update_mode == LV_CHART_UPDATE_MODE_SHIFT ? ser->last_point : 0;

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
        hook_dsc.sub_part_ptr = ser;

        lv_coord_t y_min = p2.y;
        lv_coord_t y_max = p2.y;

        for(i = 0; i < chart->point_cnt; i++) {
            p1.x = p2.x;
            p1.y = p2.y;

            point_size_act = p_act == chart->pressed_point_id ? point_size_pr : point_size;

            if(p1.x > clip_area->x2 + point_size_act + 1) break;
            p2.x = ((w * i) / (chart->point_cnt - 1)) + x_ofs;

            p_act = (start_point + i) % chart->point_cnt;

            y_tmp = (int32_t)((int32_t)ser->points[p_act] - chart->ymin[ser->y_axis]) * h;
            y_tmp = y_tmp / (chart->ymax[ser->y_axis] - chart->ymin[ser->y_axis]);
            p2.y  = h - y_tmp + y_ofs;

            if(p2.x < clip_area->x1 - point_size_act - 1) {
                p_prev = p_act;
                continue;
            }

            /*Don't draw the first point. A second point is also required to draw the line*/
            if(i != 0) {
                if(crowded_mode) {
                    if(ser->points[p_prev] != LV_CHART_POINT_DEF && ser->points[p_act] != LV_CHART_POINT_DEF) {
                        /*Draw only one vertical line between the min an max y values on the same x value*/
                        y_max = LV_MAX(y_max, p2.y);
                        y_min = LV_MIN(y_min, p2.y);
                        if(p1.x != p2.x) {
                            lv_coord_t y_cur = p2.y;
                            p2.x--;         /*It's already on the next x value*/
                            p1.x = p2.x;
                            p1.y = y_min;
                            p2.y = y_max;
                            lv_draw_line(&p1, &p2, &series_mask, &line_dsc_default);
                            p2.x++;         /*Compensate the previous x--*/
                            y_min = y_cur;  /*Start the line of the next x from the current last y*/
                            y_max = y_cur;
                        }
                    }
                } else {
                    lv_area_t point_area;
                    point_area.x1 = p1.x - point_size_act;
                    point_area.x2 = p1.x + point_size_act;
                    point_area.y1 = p1.y - point_size_act;
                    point_area.y2 = p1.y + point_size_act;

                    hook_dsc.id = i - 1;
                    hook_dsc.p1 = ser->points[p_prev] != LV_CHART_POINT_DEF ? &p1 : NULL;
                    hook_dsc.p2 = ser->points[p_act] != LV_CHART_POINT_DEF ? &p2 : NULL;
                    hook_dsc.draw_area = &point_area;
                    hook_dsc.value = ser->points[p_prev];

                    lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &hook_dsc);

                    if(ser->points[p_prev] != LV_CHART_POINT_DEF && ser->points[p_act] != LV_CHART_POINT_DEF) {
                        lv_draw_line(&p1, &p2, &series_mask, &line_dsc_default);
                    }

                    if(point_size_act && ser->points[p_act] != LV_CHART_POINT_DEF) {
                        lv_draw_rect(&point_area, &series_mask, &point_dsc_default);
                    }

                    lv_event_send(obj, LV_EVENT_DRAW_PART_END, &hook_dsc);
                }

            }
            p_prev = p_act;
        }

        /*Draw the last point*/
        point_size_act = p_act == chart->pressed_point_id ? point_size_pr : point_size;
        if(point_size_act && i == chart->point_cnt) {

            if(ser->points[p_act] != LV_CHART_POINT_DEF) {
                lv_area_t point_area;
                point_area.x1 = p2.x - point_size_act;
                point_area.x2 = p2.x + point_size_act;
                point_area.y1 = p2.y - point_size_act;
                point_area.y2 = p2.y + point_size_act;

                hook_dsc.id = i - 1;
                hook_dsc.p1 = NULL;
                hook_dsc.p2 = NULL;
                hook_dsc.draw_area = &point_area;
                hook_dsc.value = ser->points[p_act];
                lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &hook_dsc);
                lv_draw_rect(&point_area, &series_mask, &point_dsc_default);
                lv_event_send(obj, LV_EVENT_DRAW_PART_END, &hook_dsc);
            }
        }
    }
}

static void draw_series_bar(lv_obj_t * obj, const lv_area_t * clip_area)
{
    lv_area_t com_area;
    if(_lv_area_intersect(&com_area, &obj->coords, clip_area) == false) return;

    lv_chart_t * chart  = (lv_chart_t *)obj;

    uint16_t i;
    lv_area_t col_a;
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t w     = (lv_obj_get_width_fit(obj) * chart->zoom_x) >> 8;
    lv_coord_t h     = (lv_obj_get_height_fit(obj) * chart->zoom_y) >> 8;
    int32_t y_tmp;
    lv_chart_series_t * ser;
    uint32_t ser_cnt = _lv_ll_get_len(&chart->series_ll);
    int32_t block_gap = (lv_obj_get_style_pad_column(obj, LV_PART_MAIN) * chart->zoom_x) >> 8;  /*Gap between the column on ~adjacent X*/
    lv_coord_t block_w = (w - ((chart->point_cnt - 1) * block_gap)) / chart->point_cnt;
    lv_coord_t col_w = block_w / ser_cnt;
    int32_t ser_gap = (lv_obj_get_style_pad_column(obj, LV_PART_ITEMS) * chart->zoom_x) >> 8; /*Gap between the column on the ~same X */
    lv_coord_t x_ofs = pad_left - lv_obj_get_scroll_left(obj);
    lv_coord_t y_ofs = pad_top - lv_obj_get_scroll_top(obj);

    lv_draw_rect_dsc_t col_dsc;
    lv_draw_rect_dsc_init(&col_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_ITEMS, &col_dsc);
    col_dsc.bg_grad_dir = LV_GRAD_DIR_NONE;
    col_dsc.bg_opa = LV_OPA_COVER;

    /*Make the cols longer with `radius` to clip the rounding from the bottom*/
    col_a.y2 = obj->coords.y2 + col_dsc.radius;

    lv_area_t series_mask;
    bool mask_ret = _lv_area_intersect(&series_mask, &obj->coords, clip_area);
    if(mask_ret == false) return;

    lv_obj_draw_hook_dsc_t hook_dsc;
    lv_obj_draw_hook_dsc_init(&hook_dsc, &series_mask);
	hook_dsc.part = LV_PART_ITEMS;


    /*Go through all points*/
    for(i = 0; i < chart->point_cnt; i++) {
        lv_coord_t x_act = (int32_t)((int32_t)(w + block_gap) * i) / (chart->point_cnt) + obj->coords.x1 + x_ofs;

		hook_dsc.id = i;

        /*Draw the current point of all data line*/
        _LV_LL_READ_BACK(&chart->series_ll, ser) {
        	if (ser->hidden) continue;
            lv_coord_t start_point = chart->update_mode == LV_CHART_UPDATE_MODE_SHIFT ? ser->last_point : 0;

            col_a.x1 = x_act;
            col_a.x2 = col_a.x1 + col_w - ser_gap - 1;
            x_act += col_w;

            if(col_a.x2 < series_mask.x1) continue;
            if(col_a.x1 > series_mask.x2) break;

            col_dsc.bg_color = ser->color;

            lv_coord_t p_act = (start_point + i) % chart->point_cnt;
            y_tmp            = (int32_t)((int32_t)ser->points[p_act] - chart->ymin[ser->y_axis]) * h;
            y_tmp            = y_tmp / (chart->ymax[ser->y_axis] - chart->ymin[ser->y_axis]);
            col_a.y1         = h - y_tmp + obj->coords.y1 + y_ofs;

            if(ser->points[p_act] != LV_CHART_POINT_DEF) {
            	hook_dsc.draw_area = &col_a;
            	hook_dsc.rect_dsc = &col_dsc;
            	hook_dsc.sub_part_ptr = ser;
                hook_dsc.value = ser->points[p_act];
            	lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &hook_dsc);
                lv_draw_rect(&col_a, &series_mask, &col_dsc);
            	lv_event_send(obj, LV_EVENT_DRAW_PART_END, &hook_dsc);
            }
        }
    }
}

static void draw_y_ticks(lv_obj_t * obj, const lv_area_t * clip_area, lv_chart_axis_t axis)
{
    lv_chart_t * chart  = (lv_chart_t *)obj;

    lv_chart_tick_dsc_t * t = &chart->tick[axis];
    if(t->major_cnt <= 1) return;
    if(!t->label_en && !t->major_len && !t->minor_len) return;

    uint32_t i;

    lv_point_t p1;
    lv_point_t p2;

    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t h     = (lv_obj_get_height_fit(obj) * chart->zoom_y) >> 8;
    lv_coord_t y_ofs = obj->coords.y1 + pad_top - lv_obj_get_scroll_top(obj);

    lv_coord_t label_gap;
    lv_coord_t x_ofs;
    if(axis == LV_CHART_AXIS_PRIMARY_Y) {
        label_gap = lv_obj_get_style_pad_left(obj, LV_PART_MARKER);
        x_ofs = obj->coords.x1;
    } else {
        label_gap = lv_obj_get_style_pad_right(obj, LV_PART_MARKER);
        x_ofs = obj->coords.x2;
    }

    lv_coord_t major_len = t->major_len;
    lv_coord_t minor_len = t->minor_len;
    /* tick lines on secondary y axis are drawn in other direction*/
    if(axis == LV_CHART_AXIS_SECONDARY_Y) {
        major_len *= -1;
        minor_len *= -1;
    }

    lv_obj_draw_hook_dsc_t hook_dsc;
    lv_obj_draw_hook_dsc_init(&hook_dsc, clip_area);
    hook_dsc.id = axis;
    hook_dsc.part = LV_PART_MARKER;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MARKER, &line_dsc);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_MARKER, &label_dsc);

    uint32_t total_tick_num = (t->major_cnt - 1) * (t->minor_cnt);
    for(i = 0; i <= total_tick_num; i++) {
        /* draw a line at moving y position */
        p2.y = p1.y = y_ofs + (int32_t)((int32_t)(h - line_dsc.width) * i) / total_tick_num;

        /* first point of the tick */
        p1.x = x_ofs;

        /* move extra pixel out of chart boundary */
        if(axis == LV_CHART_AXIS_PRIMARY_Y) p1.x--;
        else p1.x++;

        /* second point of the tick */
        bool major = false;
        if(i % t->minor_cnt == 0) major = true;

        if(major) p2.x = p1.x - major_len; /* major tick */
        else p2.x = p1.x - minor_len; /* minor tick */

        if(p1.y + line_dsc.width / 2  >= obj->coords.y1 &&
           p2.y - line_dsc.width / 2  <= obj->coords.y2)
        {
            lv_draw_line(&p1, &p2, clip_area, &line_dsc);
        }

        /* add text only to major tick */
        if(major  && t->label_en)  {
            int32_t tick_value = chart->ymax[axis] - lv_map(i, 0, total_tick_num, chart->ymin[axis], chart->ymax[axis]);
            lv_snprintf(hook_dsc.text, sizeof(hook_dsc.text), "%d", tick_value);
            hook_dsc.value = tick_value;
            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &hook_dsc);

            /* reserve appropriate area */
            lv_point_t size;
            lv_txt_get_size(&size, hook_dsc.text, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

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

            if(a.y2 >= obj->coords.y1 &&
               a.y1  <= obj->coords.y2)
            {
                lv_draw_label(&a, clip_area, &label_dsc, hook_dsc.text, NULL);
            }
        }
    }
}

static void draw_x_ticks(lv_obj_t * obj, const lv_area_t * clip_area)
{
    lv_chart_t * chart  = (lv_chart_t *)obj;


    lv_chart_tick_dsc_t * t = &chart->tick[LV_CHART_AXIS_X];
    if(t->major_cnt <= 1) return;
    if(!t->label_en && !t->major_len && !t->minor_len) return;

    uint32_t i;
    lv_point_t p1;
    lv_point_t p2;

    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t w     = (lv_obj_get_width_fit(obj) * chart->zoom_x) >> 8;

    lv_coord_t x_ofs = obj->coords.x1 + pad_left - lv_obj_get_scroll_left(obj);
    lv_coord_t y_ofs = obj->coords.y2;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_MARKER, &label_dsc);

    lv_coord_t label_gap = t->label_en ? lv_obj_get_style_pad_bottom(obj, LV_PART_MARKER) : 0;

    if(y_ofs > clip_area->y2) return;
    if(y_ofs + label_gap  + label_dsc.font->line_height + t->major_len < clip_area->y1) return;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MARKER, &line_dsc);
    line_dsc.dash_gap = 0;
    line_dsc.dash_width = 0;

    lv_obj_draw_hook_dsc_t hook_dsc;
    lv_obj_draw_hook_dsc_init(&hook_dsc, clip_area);
    hook_dsc.id = LV_CHART_AXIS_X;
    hook_dsc.part = LV_PART_MARKER;

    /* The columns ticks should be aligned to the center of blocks */
    if(chart->type == LV_CHART_TYPE_BAR) {
        int32_t block_gap = (lv_obj_get_style_pad_column(obj, LV_PART_MAIN) * chart->zoom_x) >> 8;  /*Gap between the columns on ~adjacent X*/
        lv_coord_t block_w = (w + block_gap) / (chart->point_cnt);
        x_ofs += (block_w - block_gap) / 2;
        w -= block_w - block_gap;
    }

    p1.y = y_ofs;
    uint32_t total_tick_num = (t->major_cnt - 1) * t->minor_cnt;
    for(i = 0; i <= total_tick_num; i++) { /* one extra loop - it may not exist in the list, empty label */
        bool major = false;
        if(i % t->minor_cnt == 0) major = true;

        /* draw a line at moving x position */
        p2.x = p1.x = x_ofs + (int32_t)((int32_t)(w - line_dsc.width) * i) / total_tick_num;

        p2.y = p1.y + (major ? t->major_len : t->minor_len);

        if(p1.x + line_dsc.width / 2  >= obj->coords.x1 &&
           p2.x - line_dsc.width / 2  <= obj->coords.x2)
        {
            lv_draw_line(&p1, &p2, clip_area, &line_dsc);
        }

        /* add text only to major tick */
        if(!major || !t->label_en) continue;

        int32_t tick_value = i / t->minor_cnt;
        lv_snprintf(hook_dsc.text, sizeof(hook_dsc.text), "%d", i / t->minor_cnt);
        hook_dsc.value = tick_value;
        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &hook_dsc);

        /* reserve appropriate area */
        lv_point_t size;
        lv_txt_get_size(&size, hook_dsc.text, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);

        /* set the area at some distance of the major tick len under of the tick */
        lv_area_t a;
        a.x1 = (p2.x - size.x / 2);
        a.x2 = (p2.x + size.x / 2),
        a.y1 = p2.y + label_gap;
        a.y2 = (a.y1 + size.y);

        if(a.x2 >= obj->coords.x1 &&
           a.x1 <= obj->coords.x2)
        {
            lv_draw_label(&a, clip_area, &label_dsc, hook_dsc.text, NULL);
        }
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
    lv_chart_t * chart  = (lv_chart_t *)obj;
    lv_coord_t w = (lv_obj_get_width_fit(obj) * chart->zoom_x) >> 8;
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    x-= pad_left;

    if(x < 0) return 0;
    if(x > w) return chart->point_cnt - 1;
    if(chart->type == LV_CHART_TYPE_LINE) return (x * (chart->point_cnt - 1) + w / 2) / w;
    if(chart->type == LV_CHART_TYPE_BAR) return (x * chart->point_cnt) / w;

    return 0;
}

static void invalidate_point(lv_obj_t * obj, uint16_t i)
{
    /*FIXME*/
    lv_chart_t * chart  = (lv_chart_t *)obj;
    if(i >= chart->point_cnt) return;

    lv_coord_t w  = lv_obj_get_width(obj);
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
    else if(chart->type == LV_CHART_TYPE_BAR) {
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
        lv_obj_invalidate(obj);
    }
}

#endif
