/**
 * @file lv_linemeter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_linemeter.h"
#if LV_USE_METER != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_core/lv_group.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_linemeter"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_meter_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_meter_destructor(lv_obj_t * obj);
static lv_design_res_t lv_meter_design(lv_obj_t * lmeter, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_meter_signal(lv_obj_t * lmeter, lv_signal_t sign, void * param);
static void draw_arcs(lv_obj_t * obj, const lv_area_t * clip_area, const lv_area_t * scale_area);
static void draw_lines_and_labels(lv_obj_t * obj, const lv_area_t * clip_area, const lv_area_t * scale_area);
static void draw_needles(lv_obj_t * obj, const lv_area_t * clip_area, const lv_area_t * scale_area);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_meter = {
    .constructor = lv_meter_constructor,
    .destructor = lv_meter_destructor,
    .signal_cb = lv_meter_signal,
    .design_cb = lv_meter_design,
    .instance_size = sizeof(lv_meter_t),
    .base_class = &lv_obj
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a line meter objects
 * @param par pointer to an object, it will be the parent of the new line meter
 * @param copy pointer to a line meter object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created line meter
 */
lv_obj_t * lv_meter_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    return lv_obj_create_from_class(&lv_meter, parent, copy);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the line meter
 * @param lmeter pointer to a line meter object
 * @param value new value
 */
lv_meter_indicator_t * lv_meter_add_indicator(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_meter_t * meter = (lv_meter_t *) obj;
    lv_meter_indicator_t * indic = _lv_ll_ins_head(&meter->indic_ll);

    indic->color = LV_COLOR_RED;
    indic->img_src = NULL;
    indic->value = 40;
    indic->type = LV_METER_INDICATOR_TYPE_NEEDLE | LV_METER_INDICATOR_TYPE_ARC |  LV_METER_INDICATOR_TYPE_SCALE;

    return indic;

}

lv_meter_segment_t * lv_meter_add_segment(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_meter_t * meter = (lv_meter_t *) obj;
    lv_meter_segment_t * seg = _lv_ll_ins_head(&meter->segment_ll);

    seg->color = LV_COLOR_BLUE;
    seg->start_value = 20;
    seg->end_value = 80;
    seg->type = LV_METER_SEGMENT_TYPE_SCALE;
    seg->width = 5;
    return seg;

}
void lv_meter_set_value(lv_obj_t * obj, lv_meter_indicator_t * indic, int32_t value)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    indic->value = value;

    lv_obj_invalidate(obj);
}

/**
 * Set minimum and the maximum values of a line meter
 * @param lmeter pointer to he line meter object
 * @param min minimum value
 * @param max maximum value
 */
void lv_linemeter_set_range(lv_obj_t * obj, int32_t min, int32_t max)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_meter_t * meter = (lv_meter_t *) obj;
    if(meter->min_value == min && meter->max_value == max) return;

    meter->max_value = max;
    meter->min_value = min;
    lv_obj_invalidate(obj);
}

/**
 * Set the scale settings of a line meter
 * @param lmeter pointer to a line meter object
 * @param angle angle of the scale (0..360)
 * @param line_cnt number of lines
 */
void lv_linemeter_set_scale(lv_obj_t * obj, uint16_t angle, uint16_t line_cnt)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_meter_t * meter = (lv_meter_t *) obj;

    if(meter->scale_angle == angle && meter->line_cnt == line_cnt) return;

    meter->scale_angle = angle;
    meter->line_cnt    = line_cnt;

    lv_obj_invalidate(obj);
}

/**
 * Set the set an offset for the line meter's angles to rotate it.
 * @param lmeter pointer to a line meter object
 * @param angle angle where the meter will be facing (with its center)
 */
void lv_linemeter_set_angle_offset(lv_obj_t * obj, uint16_t angle)
{
    lv_meter_t * meter = (lv_meter_t *) obj;
    if(meter->angle_ofs == angle) return;

    meter->angle_ofs = angle;

    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the value of the line meter
 */
int32_t lv_meter_get_value(const lv_obj_t * obj, const lv_meter_indicator_t * indic)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return indic->value;
}

/**
 * Get the minimum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the minimum value of the line meter
 */
int32_t lv_meter_get_min_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_meter_t * meter = (lv_meter_t *) obj;
    return meter->min_value;
}

/**
 * Get the maximum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the maximum value of the line meter
 */
int32_t lv_meter_get_max_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_meter_t * meter = (lv_meter_t *) obj;
    return meter->max_value;
}

/**
 * Get the scale number of a line meter
 * @param lmeter pointer to a line meter object
 * @return number of the scale units
 */
uint16_t lv_linemeter_get_line_count(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_meter_t * meter = (lv_meter_t *) obj;
    return meter->line_cnt;
}

/**
 * Get the scale angle of a line meter
 * @param lmeter pointer to a line meter object
 * @return angle_ofs of the scale
 */
uint16_t lv_linemeter_get_scale_angle(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_meter_t * meter = (lv_meter_t *) obj;
    return meter->scale_angle;
}

/**
 * Get the offset for the line meter.
 * @param lmeter pointer to a line meter object
 * @return angle offset (0..360)
 */
uint16_t lv_linemeter_get_angle_offset(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_meter_t * meter = (lv_meter_t *) obj;
    return meter->angle_ofs;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_meter_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_TRACE("line meter create started");
    lv_obj_construct_base(obj, parent, copy);


    lv_meter_t * meter = (lv_meter_t *) obj;

    /*Initialize the allocated 'ext' */
    meter->min_value   = 0;
    meter->max_value   = 100;
    meter->scale_angle = 240;
    meter->angle_ofs = 0;
    meter->line_cnt = 26;
    meter->marker_nth = 5;
    meter->indicator_stacked = 0;
    _lv_ll_init(&meter->indic_ll, sizeof(lv_meter_indicator_t));
    _lv_ll_init(&meter->segment_ll, sizeof(lv_meter_segment_t));

    /*Init the new line meter line meter*/
    if(copy == NULL) {
        lv_obj_set_size(obj, 3 * LV_DPI / 2, 3 * LV_DPI / 2);
    }
    /*Copy an existing line meter*/
    else {
//        lv_linemeter_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
//        meter->scale_angle           = copy_meter->scale_angle;
//        meter->line_cnt              = copy_meter->line_cnt;
//        meter->min_value             = copy_meter->min_value;
//        meter->max_value             = copy_meter->max_value;
//        meter->cur_value             = copy_meter->cur_value;
    }

    LV_LOG_INFO("line meter created");
}

static void lv_meter_destructor(lv_obj_t * obj)
{

}
/**
 * Handle the drawing related tasks of the line meters
 * @param lmeter pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_meter_design(lv_obj_t * obj, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return lv_obj.design_cb(obj, clip_area, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_obj.design_cb(obj, clip_area, mode);

        lv_area_t scale_area;
        lv_obj_get_coords(obj, &scale_area);
        scale_area.x1 += lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
        scale_area.y1 += lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
        scale_area.x2 -= lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
        scale_area.y2 -= lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

        draw_arcs(obj, clip_area, &scale_area);
        draw_lines_and_labels(obj, clip_area, &scale_area);
        draw_needles(obj, clip_area, &scale_area);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        lv_obj.design_cb(obj, clip_area, mode);
    }

    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the line meter
 * @param lmeter pointer to a line meter object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_meter_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = lv_obj.signal_cb(obj, sign, param);
    if(res != LV_RES_OK) return res;
//    if(sign == LV_SIGNAL_GET_TYPE) return _lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);
//
//    if(sign == LV_SIGNAL_CLEANUP) {
//        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
//    }

    return res;
}


static void draw_arcs(lv_obj_t * obj, const lv_area_t * clip_area, const lv_area_t * scale_area)
{
    lv_meter_t * meter    = (lv_meter_t *)obj;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_ITEMS, &line_dsc);
    line_dsc.width = 5;

    int16_t angle_ofs = meter->angle_ofs + 90 + (360 - meter->scale_angle) / 2;

    lv_coord_t r_out = lv_area_get_width(scale_area) / 2 ;
    lv_point_t scale_center;
    scale_center.x = scale_area->x1 + r_out;
    scale_center.y = scale_area->y1 + r_out;

    /*Draw the segments first*/
    lv_meter_segment_t * seg;
    _LV_LL_READ(&meter->segment_ll, seg) {
        if(seg->type != LV_METER_SEGMENT_TYPE_ARC) continue;
        int32_t start_angle = _lv_map(seg->start_value, meter->min_value, meter->max_value, 0, meter->scale_angle) + angle_ofs;
        int32_t end_angle = _lv_map(seg->end_value, meter->min_value, meter->max_value, 0, meter->scale_angle) +  angle_ofs;

        line_dsc.color = seg->color;
        line_dsc.width = seg->width;
        lv_draw_arc(scale_center.x, scale_center.y, r_out, start_angle, end_angle, clip_area, &line_dsc);
    }

    lv_meter_indicator_t * indic;
    int32_t value = 0;
    if(meter->indicator_stacked) {
        _LV_LL_READ_BACK(&meter->indic_ll, indic) {
            if((indic->type & LV_METER_INDICATOR_TYPE_ARC) == false) continue;

            int32_t start_angle = _lv_map(value, meter->min_value, meter->max_value, 0, meter->scale_angle) + angle_ofs;
            value += indic->value;
            int32_t end_angle = _lv_map(value, meter->min_value, meter->max_value, 0, meter->scale_angle) + angle_ofs;
            line_dsc.color = indic->color;
            line_dsc.width = 20;
            lv_draw_arc(scale_center.x, scale_center.y, r_out, start_angle, end_angle, clip_area, &line_dsc);
            r_out-=10;
        }
    } else {
        _LV_LL_READ(&meter->indic_ll, indic) {
            if((indic->type & LV_METER_INDICATOR_TYPE_ARC) == false) continue;

            int32_t end_angle = _lv_map(indic->value, meter->min_value, meter->max_value, 0, meter->scale_angle) + angle_ofs;
            line_dsc.color = indic->color;
            line_dsc.width = 10;
            lv_draw_arc(scale_center.x, scale_center.y, r_out, angle_ofs, end_angle, clip_area, &line_dsc);
            r_out-=10;
        }
    }
}

static void draw_lines_and_labels(lv_obj_t * obj, const lv_area_t * clip_area, const lv_area_t * scale_area)
{
    lv_meter_t * meter    = (lv_meter_t *)obj;

    lv_coord_t sub_tick_len = 10;
    lv_coord_t major_tick_len = 20;

    lv_coord_t r_out = lv_area_get_width(scale_area) / 2 ;
    lv_coord_t r_in_sub = r_out - sub_tick_len;
    lv_coord_t r_in_marker = r_out - major_tick_len;
    if(r_in_marker < 1) r_in_marker = 1;
    if(r_in_sub < 1) r_in_sub = 1;

    lv_point_t scale_center;
    scale_center.x = scale_area->x1 + r_out;
    scale_center.y = scale_area->y1 + r_out;

    int16_t angle_ofs = meter->angle_ofs + 90 + (360 - meter->scale_angle) / 2;
    uint8_t i;

    lv_draw_line_dsc_t line_sub_dsc;
    lv_draw_line_dsc_init(&line_sub_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_sub_dsc);
    line_sub_dsc.width = 2;

    lv_draw_line_dsc_t line_marker_dsc;
    lv_draw_line_dsc_init(&line_marker_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MARKER, &line_marker_dsc);
    line_marker_dsc.width = 5;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_MARKER, &label_dsc);

    uint32_t sub_cnt = meter->marker_nth - 1;
    bool marker;
    for(i = 0; i < meter->line_cnt; i++) {
        if(sub_cnt == meter->marker_nth - 1) {
            sub_cnt = 0;
            marker = true;
        } else {
            sub_cnt++;
            marker = false;
        }

        /* `* 256` for extra precision*/
        int32_t angle_upscale = ((i * meter->scale_angle) << 8) / (meter->line_cnt - 1);

        int32_t angle_low = (angle_upscale >> 8);
        int32_t angle_high = angle_low + 1;
        int32_t angle_rem = angle_upscale & 0xFF;

        /*Interpolate sine and cos*/
        int32_t sin_low = _lv_trigo_sin(angle_low + angle_ofs);
        int32_t sin_high = _lv_trigo_sin(angle_high + angle_ofs);
        int32_t sin_mid = (sin_low * (256 - angle_rem) + sin_high * angle_rem) >> 8;

        int32_t cos_low = _lv_trigo_cos(angle_low + angle_ofs);
        int32_t cos_high = _lv_trigo_cos(angle_high + angle_ofs);
        int32_t cos_mid = (cos_low * (256 - angle_rem) + cos_high * angle_rem) >> 8;

        lv_point_t p_inner;
        lv_point_t p_outer;
        /*Use the interpolated values to get the outer x and y coordinates*/
        p_outer.x = (int32_t)(((int32_t)cos_mid * r_out + 127) >> (LV_TRIGO_SHIFT)) + scale_center.y;
        p_outer.y = (int32_t)(((int32_t)sin_mid * r_out + 127) >> (LV_TRIGO_SHIFT)) + scale_center.x;

        /*Use the interpolated values to get the inner x and y coordinates*/
        uint32_t r_in = marker ? r_in_marker : r_in_sub;
        p_inner.x = (int32_t)(((int32_t)cos_mid * r_in + 127) >> (LV_TRIGO_SHIFT)) + scale_center.y;
        p_inner.y = (int32_t)(((int32_t)sin_mid * r_in + 127) >> (LV_TRIGO_SHIFT)) + scale_center.x;

        lv_draw_line_dsc_t * line_dsc_act;
        line_dsc_act = marker ? &line_marker_dsc : &line_sub_dsc;

        int32_t value_of_line = _lv_map(i, 0, meter->line_cnt - 1, meter->min_value, meter->max_value);
        lv_color_t line_color = line_dsc_act->color;

        lv_meter_segment_t * seg;
        _LV_LL_READ(&meter->segment_ll, seg) {
            if((seg->type & LV_METER_SEGMENT_TYPE_SCALE) == false) continue;
            if(value_of_line >= seg->start_value && value_of_line <= seg->end_value) line_color = seg->color;
        }
        lv_meter_indicator_t * indic;
        if(!meter->indicator_stacked) {
            _LV_LL_READ(&meter->indic_ll, indic) {
                if((indic->type & LV_METER_INDICATOR_TYPE_SCALE) == false) continue;
                if(value_of_line <= indic->value) line_color = indic->color;
            }
        } else {
            int32_t last_value = meter->min_value;
            _LV_LL_READ_BACK(&meter->indic_ll, indic) {
                if((indic->type & LV_METER_INDICATOR_TYPE_SCALE) == false) continue;
                if(value_of_line >= last_value &&  value_of_line <= last_value + indic->value) {
                    line_color = indic->color;
                    break;
                }
                last_value += indic->value;
            }
        }

        lv_color_t line_color_ori = line_dsc_act->color;
        line_dsc_act->color = line_color;
        lv_draw_line(&p_inner, &p_outer, clip_area, line_dsc_act);
        line_dsc_act->color = line_color_ori;

        /*Draw the text*/
        if(marker) {
            uint32_t r_text = r_in_marker - 10;
            lv_point_t p;
            p.x = (int32_t)(((int32_t)cos_mid * r_text + 127) >> (LV_TRIGO_SHIFT)) + scale_center.y;
            p.y = (int32_t)(((int32_t)sin_mid * r_text + 127) >> (LV_TRIGO_SHIFT)) + scale_center.x;

            char buf[32];
            lv_snprintf(buf, sizeof(buf), "%d", value_of_line);

            lv_point_t label_size;
            _lv_txt_get_size(&label_size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space,
                    LV_COORD_MAX, LV_TEXT_FLAG_NONE);

            lv_area_t label_cord;
            label_cord.x1 = p.x - label_size.x / 2;
            label_cord.y1 = p.y - label_size.y / 2;
            label_cord.x2 = label_cord.x1 + label_size.x;
            label_cord.y2 = label_cord.y1 + label_size.y;

            lv_draw_label(&label_cord, clip_area, &label_dsc, buf, NULL);
        }
    }
}


static void draw_needles(lv_obj_t * obj, const lv_area_t * clip_area, const lv_area_t * scale_area)
{
    lv_meter_t * meter    = (lv_meter_t *)obj;

    lv_coord_t r_out = lv_area_get_width(scale_area) / 2 ;

    lv_point_t scale_center;
    scale_center.x = scale_area->x1 + r_out;
    scale_center.y = scale_area->y1 + r_out;

    int16_t angle_ofs = meter->angle_ofs + 90 + (360 - meter->scale_angle) / 2;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_INDICATOR, &line_dsc);
    line_dsc.width = 2;

    lv_draw_img_dsc_t img_dsc;
    lv_draw_img_dsc_init(&img_dsc);
    lv_obj_init_draw_img_dsc(obj, LV_PART_INDICATOR, &img_dsc);

    lv_meter_indicator_t * indic;
    int32_t value = 0;
    _LV_LL_READ_BACK(&meter->indic_ll, indic) {
        if((indic->type & LV_METER_INDICATOR_TYPE_NEEDLE) == false) continue;

        if(meter->indicator_stacked) value += indic->value;
        else value = indic->value;

        int32_t angle = _lv_map(value, meter->min_value, meter->max_value, 0, meter->scale_angle) + angle_ofs;

        /*Draw a line*/
        if(indic->img_src == NULL) {
            lv_point_t p_end;
            p_end.y = (_lv_trigo_sin(angle) * r_out) / LV_TRIGO_SIN_MAX + scale_center.x;
            p_end.x = (_lv_trigo_sin(angle + 90) * r_out) / LV_TRIGO_SIN_MAX + scale_center.y;
            lv_draw_line(&scale_center, &p_end, clip_area, &line_dsc);
        }
        /*Draw an image*/
        else {
            lv_img_header_t info;
            lv_img_decoder_get_info(indic->img_src, &info);
            lv_area_t a;
            a.x1 = scale_center.x - indic->img_pivot.x;
            a.y1 = scale_center.y - indic->img_pivot.y;
            a.x2 = a.x1 + info.w - 1;
            a.y2 = a.y1 + info.h - 1;

            img_dsc.recolor_opa = indic->img_recolor_opa;
            img_dsc.recolor = indic->color;

            angle = angle * 10;
            if(angle > 3600) angle -= 3600;
            img_dsc.angle = angle;
            lv_draw_img(&a, clip_area, indic->img_src, &img_dsc);
        }
    }
}

#endif
