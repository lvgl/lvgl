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

    indic->type = LV_METER_INDICATOR_TYPE_NEEDLE_LINE;
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
    else if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_IMG || indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_LINE) {
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
    else if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_IMG || indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_LINE) {
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
    else if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_IMG || indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_LINE) {
        inv_line(obj, indic, old_value);
        inv_line(obj, indic, value);
    }
    else {
        lv_obj_invalidate(obj);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

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

static void lv_meter_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_DRAW_MAIN) {
        lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);
        lv_area_t scale_area;
        lv_obj_get_content_coords(obj, &scale_area);

        draw_arcs(obj, draw_ctx, &scale_area);
        draw_ticks_and_labels(obj, draw_ctx, &scale_area);
        draw_needles(obj, draw_ctx, &scale_area);

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

static void draw_arcs(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx, const lv_area_t * scale_area)
{
    lv_meter_t * meter = (lv_meter_t *)obj;

    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_arc_dsc_init(&arc_dsc);
    arc_dsc.rounded = lv_obj_get_style_arc_rounded(obj, LV_PART_ITEMS);

    lv_coord_t r_out = lv_area_get_width(scale_area) / 2 ;
    lv_point_t scale_center;
    scale_center.x = scale_area->x1 + r_out;
    scale_center.y = scale_area->y1 + r_out;

    lv_opa_t opa_main = lv_obj_get_style_opa_recursive(obj, LV_PART_MAIN);
    lv_meter_indicator_t * indic;

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

        lv_meter_scale_t * scale = indic->scale;

        int32_t start_angle = lv_map(indic->start_value, scale->min, scale->max, scale->rotation,
                                     scale->rotation + scale->angle_range);
        int32_t end_angle = lv_map(indic->end_value, scale->min, scale->max, scale->rotation,
                                   scale->rotation + scale->angle_range);

        part_draw_dsc.radius = r_out + indic->type_data.arc.r_mod;
        part_draw_dsc.sub_part_ptr = indic;
        part_draw_dsc.p1 = &scale_center;

        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
        lv_draw_arc(draw_ctx, &arc_dsc, &scale_center, part_draw_dsc.radius, start_angle, end_angle);
        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
    }
}

static void draw_ticks_and_labels(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx, const lv_area_t * scale_area)
{
    lv_meter_t * meter    = (lv_meter_t *)obj;

    lv_point_t p_center;
    lv_coord_t r_edge = LV_MIN(lv_area_get_width(scale_area) / 2, lv_area_get_height(scale_area) / 2);
    p_center.x = scale_area->x1 + r_edge;
    p_center.y = scale_area->y1 + r_edge;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_TICKS, &line_dsc);
    line_dsc.raw_end = 1;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_TICKS, &label_dsc);

    lv_meter_scale_t * scale;

    lv_draw_mask_radius_param_t inner_minor_mask;
    lv_draw_mask_radius_param_t inner_major_mask;
    lv_draw_mask_radius_param_t outer_mask;

    lv_obj_draw_part_dsc_t part_draw_dsc;
    lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
    part_draw_dsc.class_p = MY_CLASS;
    part_draw_dsc.part = LV_PART_TICKS;
    part_draw_dsc.type = LV_METER_DRAW_PART_TICK;
    part_draw_dsc.line_dsc = &line_dsc;

    _LV_LL_READ_BACK(&meter->scale_ll, scale) {
        part_draw_dsc.sub_part_ptr = scale;

        lv_coord_t r_out = r_edge;
        lv_coord_t r_in_minor = r_out - scale->tick_length;
        lv_coord_t r_in_major = r_out - scale->tick_major_length;

        lv_area_t area_inner_minor;
        area_inner_minor.x1 = p_center.x - r_in_minor;
        area_inner_minor.y1 = p_center.y - r_in_minor;
        area_inner_minor.x2 = p_center.x + r_in_minor;
        area_inner_minor.y2 = p_center.y + r_in_minor;
        lv_draw_mask_radius_init(&inner_minor_mask, &area_inner_minor, LV_RADIUS_CIRCLE, true);

        lv_area_t area_inner_major;
        area_inner_major.x1 = p_center.x - r_in_major;
        area_inner_major.y1 = p_center.y - r_in_major;
        area_inner_major.x2 = p_center.x + r_in_major - 1;
        area_inner_major.y2 = p_center.y + r_in_major - 1;
        lv_draw_mask_radius_init(&inner_major_mask, &area_inner_major, LV_RADIUS_CIRCLE, true);

        lv_area_t area_outer;
        area_outer.x1 = p_center.x - r_out;
        area_outer.y1 = p_center.y - r_out;
        area_outer.x2 = p_center.x + r_out - 1;
        area_outer.y2 = p_center.y + r_out - 1;
        lv_draw_mask_radius_init(&outer_mask, &area_outer, LV_RADIUS_CIRCLE, false);
        int16_t outer_mask_id = lv_draw_mask_add(&outer_mask, NULL);

        int16_t inner_act_mask_id = LV_MASK_ID_INV; /*Will be added later*/

        uint32_t minor_cnt = scale->tick_major_nth ? scale->tick_major_nth - 1 : 0xFFFF;
        uint16_t i;
        for(i = 0; i < scale->tick_cnt; i++) {
            minor_cnt++;
            bool major = false;
            if(minor_cnt == scale->tick_major_nth) {
                minor_cnt = 0;
                major = true;
            }

            int32_t value_of_line = lv_map(i, 0, scale->tick_cnt - 1, scale->min, scale->max);
            part_draw_dsc.value = value_of_line;

            lv_color_t line_color = major ? scale->tick_major_color : scale->tick_color;
            lv_color_t line_color_ori = line_color;

            lv_coord_t line_width_ori = major ? scale->tick_major_width : scale->tick_width;
            lv_coord_t line_width = line_width_ori;

            lv_meter_indicator_t * indic;
            _LV_LL_READ_BACK(&meter->indicator_ll, indic) {
                if(indic->type != LV_METER_INDICATOR_TYPE_SCALE_LINES) continue;
                if(value_of_line >= indic->start_value && value_of_line <= indic->end_value) {
                    line_width += indic->type_data.scale_lines.width_mod;

                    if(indic->type_data.scale_lines.color_start.full == indic->type_data.scale_lines.color_end.full) {
                        line_color = indic->type_data.scale_lines.color_start;
                    }
                    else {
                        lv_opa_t ratio;
                        if(indic->type_data.scale_lines.local_grad) {
                            ratio = lv_map(value_of_line, indic->start_value, indic->end_value, LV_OPA_TRANSP, LV_OPA_COVER);
                        }
                        else {
                            ratio = lv_map(value_of_line, scale->min, scale->max, LV_OPA_TRANSP, LV_OPA_COVER);
                        }
                        line_color = lv_color_mix(indic->type_data.scale_lines.color_end, indic->type_data.scale_lines.color_start, ratio);
                    }
                }
            }

            int32_t angle_upscale = ((i * scale->angle_range) * 10) / (scale->tick_cnt - 1) +  + scale->rotation * 10;

            line_dsc.color = line_color;
            line_dsc.width = line_width;

            /*Draw a little bit longer lines to be sure the mask will clip them correctly
             *and to get a better precision*/
            lv_point_t p_outer;
            p_outer.x = p_center.x + r_out + LV_MAX(LV_DPI_DEF, r_out);
            p_outer.y = p_center.y;
            lv_point_transform(&p_outer, angle_upscale, 256, &p_center);

            part_draw_dsc.p1 = &p_center;
            part_draw_dsc.p2 = &p_outer;
            part_draw_dsc.id = i;
            part_draw_dsc.label_dsc = &label_dsc;

            /*Draw the text*/
            if(major) {
                lv_draw_mask_remove_id(outer_mask_id);
                uint32_t r_text = r_in_major - scale->label_gap;
                lv_point_t p;
                p.x = p_center.x + r_text;
                p.y = p_center.y;
                lv_point_transform(&p, angle_upscale, 256, &p_center);

                lv_draw_label_dsc_t label_dsc_tmp;
                lv_memcpy(&label_dsc_tmp, &label_dsc, sizeof(label_dsc_tmp));

                part_draw_dsc.label_dsc = &label_dsc_tmp;
                char buf[16];

                lv_snprintf(buf, sizeof(buf), "%" LV_PRId32, value_of_line);
                part_draw_dsc.text = buf;

                lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);

                lv_point_t label_size;
                lv_txt_get_size(&label_size, part_draw_dsc.text, label_dsc.font, label_dsc.letter_space, label_dsc.line_space,
                                LV_COORD_MAX, LV_TEXT_FLAG_NONE);

                lv_area_t label_cord;
                label_cord.x1 = p.x - label_size.x / 2;
                label_cord.y1 = p.y - label_size.y / 2;
                label_cord.x2 = label_cord.x1 + label_size.x;
                label_cord.y2 = label_cord.y1 + label_size.y;

                lv_draw_label(draw_ctx, part_draw_dsc.label_dsc, &label_cord, part_draw_dsc.text, NULL);

                outer_mask_id = lv_draw_mask_add(&outer_mask, NULL);
            }
            else {
                part_draw_dsc.label_dsc = NULL;
                part_draw_dsc.text = NULL;
                lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
            }

            inner_act_mask_id = lv_draw_mask_add(major ? &inner_major_mask : &inner_minor_mask, NULL);
            lv_draw_line(draw_ctx, &line_dsc, &p_outer, &p_center);
            lv_draw_mask_remove_id(inner_act_mask_id);
            lv_event_send(obj, LV_EVENT_DRAW_MAIN_END, &part_draw_dsc);

            line_dsc.color = line_color_ori;
            line_dsc.width = line_width_ori;

        }
        lv_draw_mask_free_param(&inner_minor_mask);
        lv_draw_mask_free_param(&inner_major_mask);
        lv_draw_mask_free_param(&outer_mask);
        lv_draw_mask_remove_id(outer_mask_id);
    }
}


static void draw_needles(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx, const lv_area_t * scale_area)
{
    lv_meter_t * meter = (lv_meter_t *)obj;

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
    lv_opa_t opa_main = lv_obj_get_style_opa_recursive(obj, LV_PART_MAIN);

    lv_obj_draw_part_dsc_t part_draw_dsc;
    lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
    part_draw_dsc.class_p = MY_CLASS;
    part_draw_dsc.p1 = &scale_center;

    lv_meter_indicator_t * indic;
    _LV_LL_READ_BACK(&meter->indicator_ll, indic) {
        lv_meter_scale_t * scale = indic->scale;
        part_draw_dsc.sub_part_ptr = indic;

        if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_LINE) {
            int32_t angle = lv_map(indic->end_value, scale->min, scale->max, scale->rotation, scale->rotation + scale->angle_range);
            lv_coord_t r_out = r_edge + scale->r_mod + indic->type_data.needle_line.r_mod;
            lv_point_t p_end;
            p_end.y = (lv_trigo_sin(angle) * (r_out)) / LV_TRIGO_SIN_MAX + scale_center.y;
            p_end.x = (lv_trigo_cos(angle) * (r_out)) / LV_TRIGO_SIN_MAX + scale_center.x;
            line_dsc.color = indic->type_data.needle_line.color;
            line_dsc.width = indic->type_data.needle_line.width;
            line_dsc.opa = indic->opa > LV_OPA_MAX ? opa_main : (opa_main * indic->opa) >> 8;

            part_draw_dsc.id = LV_METER_DRAW_PART_NEEDLE_LINE;
            part_draw_dsc.line_dsc = &line_dsc;
            part_draw_dsc.p2 = &p_end;

            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);
            lv_draw_line(draw_ctx, &line_dsc, &scale_center, &p_end);
            lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
        }
        else if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_IMG) {
            if(indic->type_data.needle_img.src == NULL) continue;

            int32_t angle = lv_map(indic->end_value, scale->min, scale->max, scale->rotation, scale->rotation + scale->angle_range);
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
    bool rounded = lv_obj_get_style_arc_rounded(obj, LV_PART_ITEMS);

    lv_area_t scale_area;
    lv_obj_get_content_coords(obj, &scale_area);

    lv_coord_t r_out = lv_area_get_width(&scale_area) / 2;
    lv_point_t scale_center;
    scale_center.x = scale_area.x1 + r_out;
    scale_center.y = scale_area.y1 + r_out;

    r_out += indic->type_data.arc.r_mod;

    lv_meter_scale_t * scale = indic->scale;

    int32_t start_angle = lv_map(old_value, scale->min, scale->max, scale->rotation, scale->angle_range + scale->rotation);
    int32_t end_angle = lv_map(new_value, scale->min, scale->max, scale->rotation, scale->angle_range + scale->rotation);

    lv_area_t a;
    lv_draw_arc_get_area(scale_center.x, scale_center.y, r_out, LV_MIN(start_angle, end_angle), LV_MAX(start_angle,
                                                                                                       end_angle), indic->type_data.arc.width, rounded, &a);
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
        int32_t angle = lv_map(value, scale->min, scale->max, scale->rotation, scale->rotation + scale->angle_range);
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
    else if(indic->type == LV_METER_INDICATOR_TYPE_NEEDLE_IMG) {
        int32_t angle = lv_map(value, scale->min, scale->max, scale->rotation, scale->rotation + scale->angle_range);
        lv_img_header_t info;
        lv_img_decoder_get_info(indic->type_data.needle_img.src, &info);

        angle = angle * 10;
        if(angle > 3600) angle -= 3600;

        scale_center.x -= indic->type_data.needle_img.pivot.x;
        scale_center.y -= indic->type_data.needle_img.pivot.y;
        lv_area_t a;
        _lv_img_buf_get_transformed_area(&a, info.w, info.h, angle, LV_IMG_ZOOM_NONE, &indic->type_data.needle_img.pivot);
        a.x1 += scale_center.x - 2;
        a.y1 += scale_center.y - 2;
        a.x2 += scale_center.x + 2;
        a.y2 += scale_center.y + 2;

        lv_obj_invalidate_area(obj, &a);
    }
}
#endif
