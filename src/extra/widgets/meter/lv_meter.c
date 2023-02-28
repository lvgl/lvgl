/**
 * @file lv_meter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_meter.h"
#if LV_USE_METER != 0

#include "../../../misc/lv_assert.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_meter_class

/* Triangle side length */
#define TRIANGLE_NEEDLE_SZ_PX 12

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_meter_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_meter_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_meter_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_arcs(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx, const lv_area_t * scale_area);
static void draw_ticks_and_labels(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx, const lv_area_t * scale_area);
static void draw_needles(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx, const lv_area_t * scale_area);
static void inv_arc(lv_obj_t * obj, lv_meter_indicator_t * indic, int32_t old_value, int32_t new_value);
static void inv_line(lv_obj_t * obj, lv_meter_indicator_t * indic, int32_t value);
static void get_tick_params(lv_meter_t * meter,  lv_meter_scale_t * scale, uint16_t i,
                            int32_t * value_of_line_out, lv_coord_t * line_width_out,
                            lv_color_t * line_color_out);
static void calc_label_coord(lv_meter_indicator_mode_t mode, lv_meter_scale_t * scale,
                             lv_point_t * p_center, lv_coord_t r_in_major, bool is_hor,
                             int32_t angle_upscale, lv_point_t label_size,
                             lv_point_t tick_start, lv_point_t tick_end, lv_area_t * coord_out);
static void calc_start_and_end(bool major, bool is_hor, uint16_t i, lv_meter_scale_t * scale,
                               uint32_t max_tick_len, const lv_area_t * scale_area,
                               lv_point_t * tick_start, lv_point_t * tick_end);
static void swap_values(lv_coord_t * a, lv_coord_t * b);
static void get_min_scale(lv_obj_t * obj, lv_meter_scale_t * scale,
                          int32_t * min_out, int32_t * max_out);
static bool lv_meter_is_hor(lv_obj_t * obj);


/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_meter_class = {
    .constructor_cb = lv_meter_constructor,
    .destructor_cb = lv_meter_destructor,
    .event_cb = lv_meter_event,
    .instance_size = sizeof(lv_meter_t),
    .base_class = &lv_obj_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_meter_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);

    /* By default mode is classical round meter */
    lv_meter_t * meter = (lv_meter_t *)obj;
    meter->mode = LV_METER_INDICATOR_MODE_ROUND;

    return obj;
}

/*=====================
 * Add scale
 *====================*/

lv_meter_scale_t * lv_meter_add_scale(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_meter_t * meter = (lv_meter_t *)obj;

    lv_meter_scale_t * scale = _lv_ll_ins_head(&meter->scale_ll);
    LV_ASSERT_MALLOC(scale);
    lv_memset_00(scale, sizeof(lv_meter_scale_t));

    scale->angle_range = 270;
    scale->rotation = 90 + (360 - scale->angle_range) / 2;
    scale->min = 0;
    scale->max = 100;
    scale->tick_cnt = 6;
    scale->tick_length = 8;
    scale->tick_width = 2;
    scale->label_gap = 2;

    return scale;
}

void lv_meter_set_scale_ticks(lv_obj_t * obj, lv_meter_scale_t * scale, uint16_t cnt, uint16_t width, uint16_t len,
                              lv_color_t color)
{
    scale->tick_cnt = cnt;
    scale->tick_width = width;
    scale->tick_length = len;
    scale->tick_color = color;
    lv_obj_invalidate(obj);
}

void lv_meter_set_scale_major_ticks(lv_obj_t * obj, lv_meter_scale_t * scale, uint16_t nth, uint16_t width,
                                    uint16_t len, lv_color_t color, int16_t label_gap)
{
    scale->tick_major_nth = nth;
    scale->tick_major_width = width;
    scale->tick_major_length = len;
    scale->tick_major_color = color;
    scale->label_gap = label_gap;
    lv_obj_invalidate(obj);
}

void lv_meter_set_scale_range(lv_obj_t * obj, lv_meter_scale_t * scale, int32_t min, int32_t max, uint32_t angle_range,
                              uint32_t rotation)
{
    scale->min = min;
    scale->max = max;
    scale->angle_range = angle_range;
    scale->rotation = rotation;
    lv_obj_invalidate(obj);
}

/*=====================
 * Add indicator
 *====================*/

lv_meter_indicator_t * lv_meter_add_needle_line(lv_obj_t * obj, lv_meter_scale_t * scale, uint16_t width,
                                                lv_color_t color, int16_t r_mod)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_meter_t * meter = (lv_meter_t *)obj;
    lv_meter_indicator_t * indic = _lv_ll_ins_head(&meter->indicator_ll);
    LV_ASSERT_MALLOC(indic);
    lv_memset_00(indic, sizeof(lv_meter_indicator_t));
    indic->scale = scale;
    indic->opa = LV_OPA_COVER;

    if(meter->mode == LV_METER_INDICATOR_MODE_ROUND) {
        indic->type = LV_METER_INDICATOR_TYPE_NEEDLE_LINE;
    }
    else {
        indic->type = LV_METER_INDICATOR_TYPE_NEEDLE_TRIANGLE;
    }
    indic->type_data.needle_line.width = width;
    indic->type_data.needle_line.color = color;
    indic->type_data.needle_line.r_mod = r_mod;
    lv_obj_invalidate(obj);

    return indic;
}

lv_meter_indicator_t * lv_meter_add_needle_img(lv_obj_t * obj, lv_meter_scale_t * scale, const void * src,
                                               lv_coord_t pivot_x, lv_coord_t pivot_y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_meter_t * meter = (lv_meter_t *)obj;
    lv_meter_indicator_t * indic = _lv_ll_ins_head(&meter->indicator_ll);
    LV_ASSERT_MALLOC(indic);
    lv_memset_00(indic, sizeof(lv_meter_indicator_t));
    indic->scale = scale;
    indic->opa = LV_OPA_COVER;

    indic->type = LV_METER_INDICATOR_TYPE_NEEDLE_IMG;
    indic->type_data.needle_img.src = src;
    indic->type_data.needle_img.pivot.x = pivot_x;
    indic->type_data.needle_img.pivot.y = pivot_y;
    lv_obj_invalidate(obj);

    return indic;
}

lv_meter_indicator_t * lv_meter_add_arc(lv_obj_t * obj, lv_meter_scale_t * scale, uint16_t width, lv_color_t color,
                                        int16_t r_mod)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_meter_t * meter = (lv_meter_t *)obj;
    lv_meter_indicator_t * indic = _lv_ll_ins_head(&meter->indicator_ll);
    LV_ASSERT_MALLOC(indic);
    lv_memset_00(indic, sizeof(lv_meter_indicator_t));
    indic->scale = scale;
    indic->opa = LV_OPA_COVER;

    indic->type = LV_METER_INDICATOR_TYPE_ARC;
    indic->type_data.arc.width = width;
    indic->type_data.arc.color = color;
    indic->type_data.arc.r_mod = r_mod;

    lv_obj_invalidate(obj);
    return indic;
}

lv_meter_indicator_t * lv_meter_add_scale_lines(lv_obj_t * obj, lv_meter_scale_t * scale, lv_color_t color_start,
                                                lv_color_t color_end, bool local, int16_t width_mod)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_meter_t * meter = (lv_meter_t *)obj;
    lv_meter_indicator_t * indic = _lv_ll_ins_head(&meter->indicator_ll);
    LV_ASSERT_MALLOC(indic);
    lv_memset_00(indic, sizeof(lv_meter_indicator_t));
    indic->scale = scale;
    indic->opa = LV_OPA_COVER;

    indic->type = LV_METER_INDICATOR_TYPE_SCALE_LINES;
    indic->type_data.scale_lines.color_start = color_start;
    indic->type_data.scale_lines.color_end = color_end;
    indic->type_data.scale_lines.local_grad = local;
    indic->type_data.scale_lines.width_mod = width_mod;

    lv_obj_invalidate(obj);
    return indic;
}

/*=====================
 * Set indicator value
 *====================*/

void lv_meter_set_indicator_value(lv_obj_t * obj, lv_meter_indicator_t * indic, int32_t value)
{
    int32_t old_start = indic->start_value;
    int32_t old_end = indic->end_value;
    indic->start_value = value;
    indic->end_value = value;

    if(indic->type == LV_METER_INDICATOR_TYPE_ARC) {
        inv_arc(obj, indic, old_start, value);
        inv_arc(obj, indic, old_end, value);
    }
    else if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_IMG ||
            indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_LINE ||
            indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_TRIANGLE) {
        inv_line(obj, indic, old_start);
        inv_line(obj, indic, old_end);
        inv_line(obj, indic, value);
    }
    else {
        lv_obj_invalidate(obj);
    }
}

void lv_meter_set_indicator_start_value(lv_obj_t * obj, lv_meter_indicator_t * indic, int32_t value)
{
    int32_t old_value = indic->start_value;
    indic->start_value = value;

    if(indic->type == LV_METER_INDICATOR_TYPE_ARC) {
        inv_arc(obj, indic, old_value, value);
    }
    else if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_IMG ||
            indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_LINE) {
        inv_line(obj, indic, old_value);
        inv_line(obj, indic, value);
    }
    else {
        lv_obj_invalidate(obj);
    }
}

void lv_meter_set_indicator_end_value(lv_obj_t * obj, lv_meter_indicator_t * indic, int32_t value)
{
    int32_t old_value = indic->end_value;
    indic->end_value = value;

    if(indic->type == LV_METER_INDICATOR_TYPE_ARC) {
        inv_arc(obj, indic, old_value, value);
    }
    else if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_IMG ||
            indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_LINE) {
        inv_line(obj, indic, old_value);
        inv_line(obj, indic, value);
    }
    else {
        lv_obj_invalidate(obj);
    }
}

/*=====================
 * Set indicator options
 *====================*/

void lv_meter_set_indicator_mode(lv_obj_t * obj, lv_meter_indicator_mode_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_meter_t * meter = (lv_meter_t *)obj;
    meter->mode = mode;

    /* Adjust default style for the Linear mode meter */
    if(mode == LV_METER_INDICATOR_MODE_ROUND) {
        lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
    }
    else {
        lv_obj_set_style_radius(obj, 40, 0);
    }
}

void lv_meter_set_needle_side(lv_meter_scale_t * scale, bool toggle_side)
{
    if(scale) {
        scale->toggle_needle_side = toggle_side;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool lv_meter_is_hor(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return lv_obj_get_width(obj) >= lv_obj_get_height(obj) ? true : false;
}

static void lv_meter_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_meter_t * meter = (lv_meter_t *)obj;

    _lv_ll_init(&meter->scale_ll, sizeof(lv_meter_scale_t));
    _lv_ll_init(&meter->indicator_ll, sizeof(lv_meter_indicator_t));

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_meter_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_meter_t * meter = (lv_meter_t *)obj;
    _lv_ll_clear(&meter->indicator_ll);
    _lv_ll_clear(&meter->scale_ll);

}

static void swap_values(lv_coord_t * a, lv_coord_t * b)
{
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

static void get_min_scale(lv_obj_t * obj, lv_meter_scale_t * scale,
                          int32_t * min_out, int32_t * max_out)
{
    lv_base_dir_t base_dir = lv_obj_get_style_base_dir(obj, LV_PART_MAIN);

    if(base_dir == LV_BASE_DIR_RTL) {
        *min_out = scale->max;
        *max_out = scale->min;
    }
    else if(base_dir == LV_BASE_DIR_LTR) {
        *min_out = scale->min;
        *max_out = scale->max;
    }
}

static void lv_meter_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_meter_t * meter = (lv_meter_t *)obj;

    if(code == LV_EVENT_DRAW_MAIN) {
        lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);
        lv_area_t scale_area;
        lv_obj_get_content_coords(obj, &scale_area);

        /* For linear meter arcs is like head and tail of scale */
        draw_arcs(obj, draw_ctx, &scale_area);
        draw_ticks_and_labels(obj, draw_ctx, &scale_area);
        draw_needles(obj, draw_ctx, &scale_area);

        /* By default draw the center point */
        if(meter->mode == LV_METER_INDICATOR_MODE_ROUND) {
            lv_coord_t r_edge = lv_area_get_width(&scale_area) / 2;
            lv_point_t scale_center;
            scale_center.x = scale_area.x1 + r_edge;
            scale_center.y = scale_area.y1 + r_edge;

            lv_draw_rect_dsc_t mid_dsc;
            lv_draw_rect_dsc_init(&mid_dsc);
            lv_obj_init_draw_rect_dsc(obj, LV_PART_INDICATOR, &mid_dsc);
            lv_coord_t w = lv_obj_get_style_width(obj, LV_PART_INDICATOR) / 2;
            lv_coord_t h = lv_obj_get_style_height(obj, LV_PART_INDICATOR) / 2;
            lv_area_t nm_cord;
            nm_cord.x1 = scale_center.x - w;
            nm_cord.y1 = scale_center.y - h;
            nm_cord.x2 = scale_center.x + w;
            nm_cord.y2 = scale_center.y + h;
            lv_draw_rect(draw_ctx, &mid_dsc, &nm_cord);
        }
    }
}

static void draw_arcs(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx, const lv_area_t * scale_area)
{
    lv_meter_t * meter = (lv_meter_t *)obj;
    lv_meter_indicator_t * indic;
    lv_meter_scale_t * scale;
    lv_opa_t opa_main = lv_obj_get_style_opa(obj, LV_PART_MAIN);

    /* Classical round gauge */
    if(meter->mode == LV_METER_INDICATOR_MODE_ROUND) {
        lv_draw_arc_dsc_t arc_dsc;
        lv_draw_arc_dsc_init(&arc_dsc);
        arc_dsc.rounded = lv_obj_get_style_arc_rounded(obj, LV_PART_ITEMS);

        lv_coord_t r_out = lv_area_get_width(scale_area) / 2 ;
        lv_point_t scale_center;
        scale_center.x = scale_area->x1 + r_out;
        scale_center.y = scale_area->y1 + r_out;

        lv_obj_draw_part_dsc_t part_draw_dsc;
        lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
        part_draw_dsc.arc_dsc = &arc_dsc;
        part_draw_dsc.part = LV_PART_INDICATOR;
        part_draw_dsc.class_p = MY_CLASS;
        part_draw_dsc.type = LV_METER_DRAW_PART_ARC;

        _LV_LL_READ_BACK(&meter->indicator_ll, indic) {
            if(indic->type != LV_METER_INDICATOR_TYPE_ARC) continue;

            arc_dsc.color = indic->type_data.arc.color;
            arc_dsc.width = indic->type_data.arc.width;
            arc_dsc.opa = indic->opa > LV_OPA_MAX ? opa_main : (opa_main * indic->opa) >> 8;

            scale = indic->scale;

            int32_t start_angle = lv_map(indic->start_value, scale->min, scale->max,
                                         scale->rotation,
                                         scale->rotation + scale->angle_range);
            int32_t end_angle = lv_map(indic->end_value, scale->min, scale->max,
                                       scale->rotation,
                                       scale->rotation + scale->angle_range);

            part_draw_dsc.radius = r_out + indic->type_data.arc.r_mod;
            part_draw_dsc.sub_part_ptr = indic;
            part_draw_dsc.p1 = &scale_center;

            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
            lv_draw_arc(draw_ctx, &arc_dsc, &scale_center, part_draw_dsc.radius,
                        start_angle, end_angle);
            lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
        }
        /* Linear gauge */
    }
    else {
        bool is_hor = lv_meter_is_hor(obj);
        lv_point_t line_start;
        lv_point_t line_end;
        int32_t scale_min;
        int32_t scale_max;
        uint32_t scale_len = 0;
        uint32_t max_tick_len;

        lv_draw_line_dsc_t line_dsc;
        lv_draw_line_dsc_init(&line_dsc);
        lv_obj_init_draw_line_dsc(obj, LV_PART_TICKS, &line_dsc);
        line_dsc.raw_end = 1;


        _LV_LL_READ_BACK(&meter->indicator_ll, indic) {
            if(indic->type != LV_METER_INDICATOR_TYPE_ARC) continue;

            scale = indic->scale;
            line_dsc.color = indic->type_data.arc.color;
            line_dsc.width = indic->type_data.arc.width;
            line_dsc.opa = indic->opa > LV_OPA_MAX ? opa_main : (opa_main * indic->opa) >> 8;

            max_tick_len = scale->tick_major_nth ? scale->tick_major_length :
                           scale->tick_length;

            get_min_scale(obj, scale, &scale_min, &scale_max);

            if(is_hor) {
                scale_len = lv_area_get_width(scale_area);
                line_start.y = scale_area->y1 + (lv_area_get_height(scale_area) - max_tick_len) / 2 -
                               indic->type_data.arc.r_mod + line_dsc.width / 2;
                line_end.y = line_start.y;
                line_start.x = scale_area->x1 + lv_map(indic->start_value, scale_min,
                                                       scale_max, 0, scale_len);
                line_end.x = scale_area->x1 +  lv_map(indic->end_value, scale_min,
                                                      scale_max, 0, scale_len);
                if(scale->tick_major_width) {
                    line_start.x -= scale->tick_major_width / 2;
                    line_end.x += scale->tick_major_width / 2;
                }
            }
            else {
                scale_len = lv_area_get_height(scale_area);
                line_start.x = scale_area->x1 + (lv_area_get_width(scale_area) - max_tick_len) / 2 -
                               indic->type_data.arc.r_mod + line_dsc.width / 2;
                line_end.x = line_start.x;
                line_start.y = scale_area->y1 + lv_map(indic->start_value, scale_min,
                                                       scale_max, 0, scale_len);
                line_end.y = scale_area->y1 + lv_map(indic->end_value, scale_min, scale_max, 0,
                                                     scale_len);

                if(scale->tick_major_width) {
                    line_start.y -= scale->tick_major_width / 2;
                    line_end.y += scale->tick_major_width / 2;
                }
            }
            lv_draw_line(draw_ctx, &line_dsc, &line_start, &line_end);
        }
    }
}

static void calc_start_and_end(bool major, bool is_hor, uint16_t i,
                               lv_meter_scale_t * scale, uint32_t max_tick_len,
                               const lv_area_t * scale_area,
                               lv_point_t * tick_start, lv_point_t * tick_end)
{
    if(is_hor) {
        tick_start->x = scale_area->x1 +
                        lv_area_get_width(scale_area) * i / (scale->tick_cnt - 1);
        tick_end->x =  tick_start->x;
        tick_start->y = scale_area->y1 +
                        (lv_area_get_height(scale_area) - max_tick_len) / 2;

        if(major) {
            tick_end->y = tick_start->y + max_tick_len;
        }
        else {
            tick_end->y = tick_start->y + scale->tick_length;
        }
    }
    else {
        tick_start->x = scale_area->x1 +
                        (lv_area_get_width(scale_area) - max_tick_len) / 2;
        tick_start->y = scale_area->y1 +
                        lv_area_get_height(scale_area) * i / (scale->tick_cnt - 1);
        tick_end->y = tick_start->y;

        if(major) {
            tick_end->x = tick_start->x + max_tick_len;
        }
        else {
            tick_end->x = tick_start->x + scale->tick_length;
        }
    }
}

static void calc_label_coord(lv_meter_indicator_mode_t mode, lv_meter_scale_t * scale,
                             lv_point_t * p_center, lv_coord_t r_in_major, bool is_hor,
                             int32_t angle_upscale, lv_point_t label_size,
                             lv_point_t tick_start, lv_point_t tick_end,
                             lv_area_t * coord_out)
{
    lv_point_t p;

    if(mode == LV_METER_INDICATOR_MODE_ROUND) {
        uint32_t r_text = r_in_major - scale->label_gap;
        p.x = p_center->x + r_text;
        p.y = p_center->y;
        lv_point_transform(&p, angle_upscale, 256, p_center);
    }
    else {
        if(is_hor) {
            p.x = tick_start.x;
            /*Toggle labels side if need*/
            if(scale->toggle_needle_side) {
                p.y = tick_end.y + scale->label_gap;
            }
            else {
                p.y = tick_start.y - scale->label_gap;
            }
        }
        else {
            p.y = tick_start.y;
            /*Toggle labels side if need*/
            if(scale->toggle_needle_side) {
                p.x = tick_end.x + scale->label_gap;
            }
            else {
                p.x = tick_start.x - scale->label_gap;
            }
        }
    }

    coord_out->x1 = p.x - label_size.x / 2;
    coord_out->y1 = p.y - label_size.y / 2;
    coord_out->x2 = coord_out->x1 + label_size.x;
    coord_out->y2 = coord_out->y1 + label_size.y;
}

static void get_tick_params(lv_meter_t * meter,  lv_meter_scale_t * scale, uint16_t i,
                            int32_t * value_of_line_out, lv_coord_t * line_width_out,
                            lv_color_t * line_color_out)
{
    lv_meter_indicator_t * indic;
    int32_t scale_min;
    int32_t scale_max;

    get_min_scale((lv_obj_t *)meter, scale, &scale_min, &scale_max);

    int32_t value_of_line = lv_map(i, 0, scale->tick_cnt - 1, scale_min, scale_max);

    _LV_LL_READ_BACK(&meter->indicator_ll, indic) {
        if(indic->type != LV_METER_INDICATOR_TYPE_SCALE_LINES) continue;
        if(value_of_line >= indic->start_value && value_of_line <= indic->end_value) {
            *line_width_out += indic->type_data.scale_lines.width_mod;

            if(indic->type_data.scale_lines.color_start.full ==
               indic->type_data.scale_lines.color_end.full) {
                *line_color_out = indic->type_data.scale_lines.color_start;
            }
            else {
                lv_opa_t ratio;
                if(indic->type_data.scale_lines.local_grad) {
                    ratio = lv_map(value_of_line, indic->start_value,
                                   indic->end_value, LV_OPA_TRANSP, LV_OPA_COVER);
                }
                else {
                    ratio = lv_map(value_of_line, scale->min, scale->max,
                                   LV_OPA_TRANSP, LV_OPA_COVER);
                }
                *line_color_out = lv_color_mix(indic->type_data.scale_lines.color_end,
                                               indic->type_data.scale_lines.color_start, ratio);
            }
            /*We found the place, don't need to continue*/
            break;
        }
    }
    *value_of_line_out = value_of_line;
}

static void draw_ticks_and_labels(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx, const lv_area_t * scale_area)
{
    lv_meter_t * meter = (lv_meter_t *)obj;
    lv_coord_t r_edge = LV_MIN(lv_area_get_width(scale_area) / 2,
                               lv_area_get_height(scale_area) / 2);
    lv_point_t p_center;
    bool is_hor = lv_meter_is_hor(obj);

    p_center.x = scale_area->x1 + r_edge;
    p_center.y = scale_area->y1 + r_edge;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_TICKS, &line_dsc);
    line_dsc.raw_end = 1;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_TICKS, &label_dsc);

    lv_draw_mask_radius_param_t inner_minor_mask;
    lv_draw_mask_radius_param_t inner_major_mask;
    lv_draw_mask_radius_param_t outer_mask;

    lv_obj_draw_part_dsc_t part_draw_dsc;
    lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
    part_draw_dsc.class_p = MY_CLASS;
    part_draw_dsc.part = LV_PART_TICKS;
    part_draw_dsc.type = LV_METER_DRAW_PART_TICK;
    part_draw_dsc.line_dsc = &line_dsc;

    lv_meter_scale_t * scale;

    _LV_LL_READ_BACK(&meter->scale_ll, scale) {

        part_draw_dsc.sub_part_ptr = scale;
        lv_coord_t r_out = r_edge;
        lv_coord_t r_in_minor = 0;
        int16_t outer_mask_id = 0;
        lv_coord_t r_in_major = 0;
        lv_area_t area_inner_minor;
        lv_area_t area_inner_major;
        lv_area_t area_outer;

        if(meter->mode == LV_METER_INDICATOR_MODE_ROUND) {
            r_in_minor = r_out - scale->tick_length;
            r_in_major = r_out - scale->tick_major_length;

            area_inner_minor.x1 = p_center.x - r_in_minor;
            area_inner_minor.y1 = p_center.y - r_in_minor;
            area_inner_minor.x2 = p_center.x + r_in_minor;
            area_inner_minor.y2 = p_center.y + r_in_minor;
            lv_draw_mask_radius_init(&inner_minor_mask, &area_inner_minor,
                                     LV_RADIUS_CIRCLE, true);

            area_inner_major.x1 = p_center.x - r_in_major;
            area_inner_major.y1 = p_center.y - r_in_major;
            area_inner_major.x2 = p_center.x + r_in_major - 1;
            area_inner_major.y2 = p_center.y + r_in_major - 1;
            lv_draw_mask_radius_init(&inner_major_mask, &area_inner_major,
                                     LV_RADIUS_CIRCLE, true);

            area_outer.x1 = p_center.x - r_out;
            area_outer.y1 = p_center.y - r_out;
            area_outer.x2 = p_center.x + r_out - 1;
            area_outer.y2 = p_center.y + r_out - 1;
            lv_draw_mask_radius_init(&outer_mask, &area_outer, LV_RADIUS_CIRCLE, false);

            outer_mask_id = lv_draw_mask_add(&outer_mask, NULL);
        }

        int16_t inner_act_mask_id = LV_MASK_ID_INV; /*Will be added later*/
        uint32_t minor_cnt;
        uint32_t max_tick_len;

        if(scale->tick_major_nth) {
            minor_cnt = scale->tick_major_nth - 1;
            max_tick_len = scale->tick_major_length;
        }
        else {
            minor_cnt = 0xFFFF;
            max_tick_len = scale->tick_length;
        }

        lv_point_t tick_start;
        lv_point_t tick_end;
        uint16_t i;
        for(i = 0; i < scale->tick_cnt; i++) {
            minor_cnt++;
            bool major = false;

            if(minor_cnt == scale->tick_major_nth) {
                minor_cnt = 0;
                major = true;
            }

            if(meter->mode == LV_METER_INDICATOR_MODE_LINEAR) {
                calc_start_and_end(major, is_hor, i, scale, max_tick_len,
                                   scale_area, &tick_start, &tick_end);
            }

            line_dsc.color = major ? scale->tick_major_color : scale->tick_color;
            line_dsc.width = major ? scale->tick_major_width : scale->tick_width;

            int32_t angle_upscale = 0;
            lv_point_t p_outer = {0};

            get_tick_params(meter, scale, i, &part_draw_dsc.value, &line_dsc.width,
                            &line_dsc.color);

            if(meter->mode == LV_METER_INDICATOR_MODE_ROUND) {
                angle_upscale = ((i * scale->angle_range) * 10) / (scale->tick_cnt - 1) +
                                scale->rotation * 10;

                /*Draw a little bit longer lines to be sure the mask will clip them correctly
                 *and to get a better precision*/
                p_outer.x = p_center.x + r_out + LV_MAX(LV_DPI_DEF, r_out);
                p_outer.y = p_center.y;
                lv_point_transform(&p_outer, angle_upscale, 256, &p_center);

                part_draw_dsc.p1 = &p_center;
                part_draw_dsc.p2 = &p_outer;
                part_draw_dsc.id = i;
                part_draw_dsc.label_dsc = &label_dsc;
            }

            /*Draw the text*/
            if(major) {
                lv_draw_label_dsc_t label_dsc_tmp;
                lv_memcpy(&label_dsc_tmp, &label_dsc, sizeof(label_dsc_tmp));

                part_draw_dsc.label_dsc = &label_dsc_tmp;
                char buf[16];

                lv_snprintf(buf, sizeof(buf), "%" LV_PRId32, part_draw_dsc.value);
                part_draw_dsc.text = buf;

                lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);

                lv_point_t label_size;
                lv_txt_get_size(&label_size, part_draw_dsc.text, label_dsc.font,
                                label_dsc.letter_space, label_dsc.line_space,
                                LV_COORD_MAX, LV_TEXT_FLAG_NONE);

                lv_area_t label_cord;
                calc_label_coord(meter->mode, scale, &p_center, r_in_major, is_hor,
                                 angle_upscale, label_size, tick_start, tick_end,
                                 &label_cord);

                if(meter->mode == LV_METER_INDICATOR_MODE_ROUND) {
                    lv_draw_mask_remove_id(outer_mask_id);
                    lv_draw_label(draw_ctx, part_draw_dsc.label_dsc, &label_cord,
                                  part_draw_dsc.text, NULL);
                    outer_mask_id = lv_draw_mask_add(&outer_mask, NULL);
                }
                else {
                    lv_draw_label(draw_ctx, part_draw_dsc.label_dsc, &label_cord,
                                  part_draw_dsc.text, NULL);
                }
            }
            else {
                part_draw_dsc.label_dsc = NULL;
                part_draw_dsc.text = NULL;
            }

            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);

            if(meter->mode == LV_METER_INDICATOR_MODE_ROUND) {
                inner_act_mask_id = lv_draw_mask_add(major ? &inner_major_mask :
                                                     &inner_minor_mask, NULL);
                lv_draw_line(draw_ctx, &line_dsc, &p_outer, &p_center);
                lv_draw_mask_remove_id(inner_act_mask_id);
            }
            else {
                lv_draw_line(draw_ctx, &line_dsc, &tick_start, &tick_end);
            }

            lv_event_send(obj, LV_EVENT_DRAW_MAIN_END, &part_draw_dsc);
        }
        if(meter->mode == LV_METER_INDICATOR_MODE_ROUND) {
            lv_draw_mask_free_param(&inner_minor_mask);
            lv_draw_mask_free_param(&inner_major_mask);
            lv_draw_mask_free_param(&outer_mask);
            lv_draw_mask_remove_id(outer_mask_id);
        }
    }
}

static void draw_needles(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx, const lv_area_t * scale_area)
{
    lv_meter_t * meter = (lv_meter_t *)obj;
    bool is_hor = lv_meter_is_hor(obj);

    lv_coord_t r_edge = lv_area_get_width(scale_area) / 2;
    lv_point_t scale_center;
    scale_center.x = scale_area->x1 + r_edge;
    scale_center.y = scale_area->y1 + r_edge;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &line_dsc);

    lv_draw_img_dsc_t img_dsc;
    lv_draw_img_dsc_init(&img_dsc);
    lv_obj_init_draw_img_dsc(obj, LV_PART_ITEMS, &img_dsc);
    lv_opa_t opa_main = lv_obj_get_style_opa(obj, LV_PART_MAIN);

    lv_obj_draw_part_dsc_t part_draw_dsc;
    lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
    part_draw_dsc.class_p = MY_CLASS;
    part_draw_dsc.p1 = &scale_center;

    lv_meter_indicator_t * indic;

    _LV_LL_READ_BACK(&meter->indicator_ll, indic) {
        lv_meter_scale_t * scale = indic->scale;
        part_draw_dsc.sub_part_ptr = indic;

        if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_LINE ||
           indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_TRIANGLE) {

            line_dsc.color = indic->type_data.needle_line.color;
            line_dsc.width = indic->type_data.needle_line.width;
            line_dsc.opa = indic->opa > LV_OPA_MAX ? opa_main : (opa_main * indic->opa) >> 8;
            part_draw_dsc.id = LV_METER_DRAW_PART_NEEDLE_LINE;
            part_draw_dsc.line_dsc = &line_dsc;

            if(meter->mode == LV_METER_INDICATOR_MODE_ROUND) {
                lv_point_t p_end;
                int32_t angle;
                lv_coord_t r_out;
                angle = lv_map(indic->end_value, scale->min, scale->max, scale->rotation,
                               scale->rotation + scale->angle_range);
                r_out = r_edge + scale->r_mod + indic->type_data.needle_line.r_mod;
                p_end.y = (lv_trigo_sin(angle) * (r_out)) / LV_TRIGO_SIN_MAX + scale_center.y;
                p_end.x = (lv_trigo_cos(angle) * (r_out)) / LV_TRIGO_SIN_MAX + scale_center.x;
                part_draw_dsc.p2 = &p_end;

                lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
                lv_draw_line(draw_ctx, &line_dsc, &scale_center, &p_end);
                lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);

            }
            else {
                uint32_t max_tick_len;
                lv_point_t tick_center;
                lv_point_t tick_end;
                int32_t scale_min;
                int32_t scale_max;

                get_min_scale(obj, scale, &scale_min, &scale_max);

                part_draw_dsc.p1 = &tick_center;
                part_draw_dsc.p2 = &tick_end;
                max_tick_len = scale->tick_major_length > scale->tick_length ?
                               scale->tick_major_length : scale->tick_length;

                lv_point_t points[3] = {0};

                if(is_hor) {
                    tick_center.y = scale_area->y1 + lv_area_get_height(scale_area) / 2 -
                                    indic->type_data.needle_line.r_mod;
                    tick_end.y = tick_center.y + max_tick_len / 2;
                    tick_center.x = scale_area->x1 + lv_map(indic->end_value,
                                                            scale_min, scale_max, 0,
                                                            lv_area_get_width(scale_area));
                    tick_end.x = tick_center.x;

                    if(scale->toggle_needle_side) {
                        points[0].x = tick_center.x;
                        points[0].y = tick_end.y - max_tick_len;
                        points[1].x = points[0].x - TRIANGLE_NEEDLE_SZ_PX / 2;
                        points[1].y = points[0].y - TRIANGLE_NEEDLE_SZ_PX;
                        points[2].x = points[1].x + TRIANGLE_NEEDLE_SZ_PX;
                        points[2].y = points[1].y;
                    }
                    else {
                        points[0].x = tick_end.x;
                        points[0].y = tick_end.y;
                        points[1].x = points[0].x  - TRIANGLE_NEEDLE_SZ_PX / 2;
                        points[1].y = points[0].y + TRIANGLE_NEEDLE_SZ_PX;
                        points[2].x = points[1].x + TRIANGLE_NEEDLE_SZ_PX;
                        points[2].y = points[1].y;
                    }
                }
                else {
                    tick_center.x = scale_area->x1 + lv_area_get_width(scale_area) / 2 -
                                    indic->type_data.needle_line.r_mod;
                    tick_end.x = tick_center.x + max_tick_len / 2;
                    tick_center.y = scale_area->y1 + lv_map(indic->end_value, scale_min,
                                                            scale_max, 0,
                                                            lv_area_get_height(scale_area));
                    tick_end.y = tick_center.y;

                    /*Triangle on the left side*/
                    if(scale->toggle_needle_side) {
                        points[0].x = tick_end.x - max_tick_len;
                        points[0].y = tick_end.y;
                        points[1].x = points[0].x - TRIANGLE_NEEDLE_SZ_PX;
                        points[1].y = points[0].y - TRIANGLE_NEEDLE_SZ_PX / 2;
                        points[2].x = points[1].x;
                        points[2].y = points[1].y + TRIANGLE_NEEDLE_SZ_PX;
                        /*Triangle on the right side*/
                    }
                    else {
                        points[0].x = tick_end.x;
                        points[0].y = tick_end.y;
                        points[1].x = tick_end.x + TRIANGLE_NEEDLE_SZ_PX;
                        points[1].y = tick_end.y + TRIANGLE_NEEDLE_SZ_PX / 2;
                        points[2].x =  points[1].x;
                        points[2].y = points[1].y - TRIANGLE_NEEDLE_SZ_PX;
                    }
                }

                lv_draw_rect_dsc_t triag_dsc;
                lv_draw_rect_dsc_init(&triag_dsc);
                triag_dsc.bg_color = indic->type_data.needle_line.color;
                triag_dsc.bg_img_opa = LV_OPA_COVER;

                lv_draw_triangle(draw_ctx, &triag_dsc, points);
            }
        }
        else if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_IMG) {
            if(indic->type_data.needle_img.src == NULL) continue;

            int32_t angle = lv_map(indic->end_value, scale->min, scale->max,
                                   scale->rotation, scale->rotation + scale->angle_range);
            lv_img_header_t info;
            lv_img_decoder_get_info(indic->type_data.needle_img.src, &info);
            lv_area_t a;
            a.x1 = scale_center.x - indic->type_data.needle_img.pivot.x;
            a.y1 = scale_center.y - indic->type_data.needle_img.pivot.y;
            a.x2 = a.x1 + info.w - 1;
            a.y2 = a.y1 + info.h - 1;

            img_dsc.opa = indic->opa > LV_OPA_MAX ? opa_main : (opa_main * indic->opa) >> 8;
            img_dsc.pivot.x = indic->type_data.needle_img.pivot.x;
            img_dsc.pivot.y = indic->type_data.needle_img.pivot.y;
            angle = angle * 10;
            if(angle > 3600) angle -= 3600;
            img_dsc.angle = angle;

            part_draw_dsc.id = LV_METER_DRAW_PART_NEEDLE_IMG;
            part_draw_dsc.img_dsc = &img_dsc;

            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
            lv_draw_img(draw_ctx, &img_dsc, &a, indic->type_data.needle_img.src);
            lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
        }
    }
}

static void inv_arc(lv_obj_t * obj, lv_meter_indicator_t * indic, int32_t old_value, int32_t new_value)
{
    lv_area_t a;
    lv_area_t scale_area;
    lv_meter_t * meter = (lv_meter_t *)obj;

    lv_obj_get_content_coords(obj, &scale_area);

    /* Classical round gauge */
    if(meter->mode == LV_METER_INDICATOR_MODE_ROUND) {
        bool rounded = lv_obj_get_style_arc_rounded(obj, LV_PART_ITEMS);
        lv_coord_t r_out = lv_area_get_width(&scale_area) / 2;
        lv_point_t scale_center;

        scale_center.x = scale_area.x1 + r_out;
        scale_center.y = scale_area.y1 + r_out;

        r_out += indic->type_data.arc.r_mod;

        lv_meter_scale_t * scale = indic->scale;

        int32_t start_angle = lv_map(old_value, scale->min, scale->max,
                                     scale->rotation, scale->angle_range + scale->rotation);
        int32_t end_angle = lv_map(new_value, scale->min, scale->max,
                                   scale->rotation, scale->angle_range + scale->rotation);

        lv_draw_arc_get_area(scale_center.x, scale_center.y, r_out,
                             LV_MIN(start_angle, end_angle), LV_MAX(start_angle,
                                                                    end_angle), indic->type_data.arc.width, rounded, &a);
    }
    else {
        int32_t scale_min;
        int32_t scale_max;
        uint32_t scale_len = 0;
        uint32_t max_tick_len;

        max_tick_len = indic->scale->tick_major_nth ? indic->scale->tick_major_length :
                       indic->scale->tick_length;

        get_min_scale(obj, indic->scale, &scale_min, &scale_max);

        if(lv_meter_is_hor(obj)) {
            scale_len = lv_area_get_width(&scale_area);
            a.y1 = scale_area.y1 + (lv_area_get_height(&scale_area) - max_tick_len) / 2 -
                   indic->type_data.arc.r_mod + indic->type_data.arc.width / 2;
            a.y2 = a.y1;
            a.x1 = scale_area.x1 + lv_map(old_value, scale_min, scale_max, 0,
                                          scale_len);
            a.x2 = scale_area.x1 +  lv_map(new_value, scale_min, scale_max, 0,
                                           scale_len);

            a.y1 -= indic->type_data.arc.width / 2;
            a.y2 += indic->type_data.arc.width / 2;

            /*Swap coord for reverse direction*/
            if(a.x1 > a.x2) {
                swap_values(&a.x1, &a.x2);
            }
        }
        else {
            scale_len = lv_area_get_height(&scale_area);
            a.x1 = scale_area.x1 + (lv_area_get_width(&scale_area) - max_tick_len) / 2 -
                   indic->type_data.arc.r_mod + indic->type_data.arc.width / 2;
            a.x2 = a.x1;
            a.y1 = scale_area.y1 + lv_map(old_value, scale_min, scale_max, 0,
                                          scale_len);
            a.y2 = scale_area.y1 + lv_map(new_value, scale_min, scale_max, 0,
                                          scale_len);

            a.x1 -= indic->type_data.arc.width / 2;
            a.x2 += indic->type_data.arc.width / 2;

            /*Swap coord for reverse direction*/
            if(a.y1 > a.y2) {
                swap_values(&a.y1, &a.y2);
            }
        }
    }
    lv_obj_invalidate_area(obj, &a);
}


static void inv_line(lv_obj_t * obj, lv_meter_indicator_t * indic, int32_t value)
{
    lv_area_t scale_area;
    lv_obj_get_content_coords(obj, &scale_area);

    lv_coord_t r_out = lv_area_get_width(&scale_area) / 2;
    lv_point_t scale_center;
    scale_center.x = scale_area.x1 + r_out;
    scale_center.y = scale_area.y1 + r_out;

    lv_meter_scale_t * scale = indic->scale;

    if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_LINE) {
        int32_t angle = lv_map(value, scale->min, scale->max, scale->rotation,
                               scale->rotation + scale->angle_range);
        r_out += scale->r_mod + indic->type_data.needle_line.r_mod;
        lv_point_t p_end;
        p_end.y = (lv_trigo_sin(angle) * (r_out)) / LV_TRIGO_SIN_MAX + scale_center.y;
        p_end.x = (lv_trigo_cos(angle) * (r_out)) / LV_TRIGO_SIN_MAX + scale_center.x;

        lv_area_t a;
        a.x1 = LV_MIN(scale_center.x, p_end.x) - indic->type_data.needle_line.width - 2;
        a.y1 = LV_MIN(scale_center.y, p_end.y) - indic->type_data.needle_line.width - 2;
        a.x2 = LV_MAX(scale_center.x, p_end.x) + indic->type_data.needle_line.width + 2;
        a.y2 = LV_MAX(scale_center.y, p_end.y) + indic->type_data.needle_line.width + 2;

        lv_obj_invalidate_area(obj, &a);
    }
    else if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_TRIANGLE) {
        lv_area_t a;
        uint32_t max_tick_len;
        lv_point_t tick_center;
        lv_point_t tick_end;
        int32_t scale_min;
        int32_t scale_max;

        get_min_scale(obj, scale, &scale_min, &scale_max);

        max_tick_len = scale->tick_major_length > scale->tick_length ?
                       scale->tick_major_length : scale->tick_length;

        if(lv_meter_is_hor(obj)) {
            tick_center.y = scale_area.y1 + lv_area_get_height(&scale_area) / 2 -
                            indic->type_data.needle_line.r_mod;
            tick_end.y = tick_center.y + max_tick_len / 2;
            tick_center.x = scale_area.x1 + lv_map(value, scale_min, scale_max, 0,
                                                   lv_area_get_width(&scale_area));
            tick_end.x = tick_center.x;

            if(scale->toggle_needle_side) {
                a.x1 = tick_center.x - TRIANGLE_NEEDLE_SZ_PX;
                a.x2 = tick_center.x + TRIANGLE_NEEDLE_SZ_PX;
                a.y1 = tick_end.y - max_tick_len - TRIANGLE_NEEDLE_SZ_PX;
                a.y2 = a.y1 + TRIANGLE_NEEDLE_SZ_PX;
            }
            else {
                a.x1 = tick_end.x - TRIANGLE_NEEDLE_SZ_PX;
                a.x2 = tick_end.x + TRIANGLE_NEEDLE_SZ_PX;
                a.y1 = tick_end.y;
                a.y2 = tick_end.y + TRIANGLE_NEEDLE_SZ_PX;
            }
        }
        else {
            tick_center.x = scale_area.x1 + lv_area_get_width(&scale_area) / 2 -
                            indic->type_data.needle_line.r_mod;
            tick_end.x = tick_center.x + max_tick_len / 2;
            tick_center.y = scale_area.y1 + lv_map(value, scale_min, scale_max, 0,
                                                   lv_area_get_height(&scale_area));
            tick_end.y = tick_center.y;

            if(scale->toggle_needle_side) {
                a.x1 = tick_end.x - max_tick_len - TRIANGLE_NEEDLE_SZ_PX;
                a.x2 = tick_center.x;
                a.y1 = tick_center.y - TRIANGLE_NEEDLE_SZ_PX;
                a.y2 = tick_center.y + TRIANGLE_NEEDLE_SZ_PX;
            }
            else {
                a.x1 = tick_end.x;
                a.x2 = tick_end.x + TRIANGLE_NEEDLE_SZ_PX;
                a.y1 = tick_end.y - TRIANGLE_NEEDLE_SZ_PX;
                a.y2 = tick_end.y + TRIANGLE_NEEDLE_SZ_PX;
            }
        }

        lv_obj_invalidate_area(obj, &a);
    }
    else if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_IMG) {
        int32_t angle = lv_map(value, scale->min, scale->max, scale->rotation,
                               scale->rotation + scale->angle_range);
        lv_img_header_t info;
        lv_img_decoder_get_info(indic->type_data.needle_img.src, &info);

        angle = angle * 10;
        if(angle > 3600) angle -= 3600;

        scale_center.x -= indic->type_data.needle_img.pivot.x;
        scale_center.y -= indic->type_data.needle_img.pivot.y;
        lv_area_t a;
        _lv_img_buf_get_transformed_area(&a, info.w, info.h, angle, LV_IMG_ZOOM_NONE,
                                         &indic->type_data.needle_img.pivot);
        a.x1 += scale_center.x - 2;
        a.y1 += scale_center.y - 2;
        a.x2 += scale_center.x + 2;
        a.y2 += scale_center.y + 2;

        lv_obj_invalidate_area(obj, &a);
    }
}
#endif
