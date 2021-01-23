/**
 * @file lv_meter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl/src/lv_widgets/lv_meter.h>
#if LV_USE_METER != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_core/lv_group.h"
#include "../lv_misc/lv_math.h"
#include "../lv_core/lv_disp.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_meter"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_meter_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_meter_destructor(lv_obj_t * obj);
static lv_draw_res_t lv_meter_draw(lv_obj_t * lmeter, const lv_area_t * clip_area, lv_draw_mode_t mode);
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
    .draw_cb = lv_meter_draw,
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
    lv_memset_00(indic, sizeof(lv_meter_indicator_t));
    indic->start_value = meter->min_value;
    indic->end_value = meter->min_value;
    indic->type = LV_METER_INDICATOR_TYPE_NEEDLE;
    indic->color = LV_COLOR_GRAY;
    indic->grad_color = LV_COLOR_GRAY;
    indic->width = LV_DPX(4);
    indic->opa = LV_OPA_COVER;
    return indic;

}

void lv_meter_set_indicator_value(lv_obj_t * obj, lv_meter_indicator_t * indic, int32_t value)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    indic->end_value = value;
    lv_obj_invalidate(obj);
}

void lv_meter_set_indicator_start_value(lv_obj_t * obj, lv_meter_indicator_t * indic, int32_t value)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    indic->start_value = value;
    lv_obj_invalidate(obj);
}

void lv_meter_set_indicator_end_value(lv_obj_t * obj, lv_meter_indicator_t * indic, int32_t value)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    indic->end_value = value;
    lv_obj_invalidate(obj);
}

void lv_meter_set_indicator_radius_mod(lv_obj_t * obj, lv_meter_indicator_t * indic, lv_coord_t r_mod)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    indic->r_mod = r_mod;
    lv_obj_invalidate(obj);
}


void lv_meter_set_indicator_width(lv_obj_t * obj, lv_meter_indicator_t * indic, lv_coord_t width)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    indic->width = width;
    lv_obj_invalidate(obj);
}


void lv_meter_set_indicator_color(lv_obj_t * obj, lv_meter_indicator_t * indic, lv_color_t color)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    indic->color = color;
    lv_obj_invalidate(obj);
}

void lv_meter_set_indicator_opa(lv_obj_t * obj, lv_meter_indicator_t * indic, lv_opa_t opa)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    indic->opa = opa;
    lv_obj_invalidate(obj);
}

void lv_meter_set_indicator_needle_img(lv_obj_t * obj, lv_meter_indicator_t * indic, const void * img_src, lv_coord_t pivot_x, lv_coord_t pivot_y)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    indic->img_src = img_src;
    indic->img_pivot.x = pivot_x;
    indic->img_pivot.y = pivot_y;
    lv_obj_invalidate(obj);
}



void lv_meter_set_indicator_recolor_opa(lv_obj_t * obj, lv_meter_indicator_t * indic, lv_opa_t recolor_opa)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    indic->recolor_opa = recolor_opa;
    lv_obj_invalidate(obj);
}

void lv_meter_set_indicator_arc_ofs(lv_obj_t * obj, lv_meter_indicator_t * indic, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    indic->arc_ofs.x = x_ofs;
    indic->arc_ofs.y = y_ofs;
    lv_obj_invalidate(obj);
}

void lv_meter_set_indicator_needle_color(lv_obj_t * obj, lv_meter_indicator_t * indic, lv_color_t main_color, lv_color_t grad_color, bool local)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    indic->color = main_color;
    indic->grad_color = grad_color;
    indic->scale_color_local = local;
    lv_obj_invalidate(obj);
}

/**
 * Set minimum and the maximum values of a line meter
 * @param lmeter pointer to he line meter object
 * @param min minimum value
 * @param max maximum value
 */
void lv_meter_set_range(lv_obj_t * obj, int32_t min, int32_t max)
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
void lv_meter_set_scale(lv_obj_t * obj, uint16_t angle, uint16_t line_cnt, uint16_t marker_nth)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_meter_t * meter = (lv_meter_t *) obj;

    if(meter->scale_angle == angle && meter->line_cnt == line_cnt) return;

    meter->scale_angle = angle;
    meter->line_cnt    = line_cnt;
    meter->marker_nth  = marker_nth;

    lv_obj_invalidate(obj);
}

/**
 * Set the set an offset for the line meter's angles to rotate it.
 * @param lmeter pointer to a line meter object
 * @param angle angle where the meter will be facing (with its center)
 */
void lv_meter_set_angle_offset(lv_obj_t * obj, uint16_t angle)
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

    return indic->end_value;
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
uint16_t lv_meter_get_line_count(const lv_obj_t * obj)
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
uint16_t lv_meter_get_scale_angle(const lv_obj_t * obj)
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
uint16_t lv_meter_get_angle_offset(lv_obj_t * obj)
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
    _lv_ll_init(&meter->indic_ll, sizeof(lv_meter_indicator_t));

    /*Init the new line meter line meter*/
    if(copy == NULL) {
        lv_obj_set_size(obj, 3 * LV_DPI / 2, 3 * LV_DPI / 2);
    }
    /*Copy an existing line meter*/
    else {
//        lv_meter_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
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
 * @param mode LV_DRAW_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DRAW_DRAW: draw the object (always return 'true')
 *             LV_DRAW_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_draw_res_t`
 */
static lv_draw_res_t lv_meter_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DRAW_MODE_COVER_CHECK) {
        return lv_obj.draw_cb(obj, clip_area, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DRAW_MODE_MAIN_DRAW) {
        lv_obj.draw_cb(obj, clip_area, mode);

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
    else if(mode == LV_DRAW_MODE_POST_DRAW) {
        lv_obj.draw_cb(obj, clip_area, mode);
    }

    return LV_DRAW_RES_OK;
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

    int16_t angle_ofs = meter->angle_ofs + 90 + (360 - meter->scale_angle) / 2;

    lv_coord_t r_out = lv_area_get_width(scale_area) / 2 ;
    lv_point_t scale_center;
    scale_center.x = scale_area->x1 + r_out;
    scale_center.y = scale_area->y1 + r_out;

    lv_opa_t opa_main = lv_obj_get_style_opa(obj, LV_PART_MAIN);
    lv_meter_indicator_t * indic;
    _LV_LL_READ(&meter->indic_ll, indic) {
        if((indic->type & LV_METER_INDICATOR_TYPE_ARC) == false) continue;

        line_dsc.color = indic->color;
        line_dsc.width = indic->width;
        line_dsc.opa = indic->opa > LV_OPA_MAX ? opa_main : (opa_main * indic->opa) >> 8;

        int32_t start_angle = lv_map(indic->start_value, meter->min_value, meter->max_value, 0, meter->scale_angle) + angle_ofs;
        int32_t end_angle = lv_map(indic->end_value, meter->min_value, meter->max_value, 0, meter->scale_angle) + angle_ofs;
        lv_draw_arc(scale_center.x + indic->arc_ofs.x, scale_center.y + indic->arc_ofs.y, r_out + indic->r_mod, start_angle, end_angle, clip_area, &line_dsc);
    }
}

static void draw_lines_and_labels(lv_obj_t * obj, const lv_area_t * clip_area, const lv_area_t * scale_area)
{
    lv_meter_t * meter    = (lv_meter_t *)obj;

    lv_coord_t sub_tick_len = lv_obj_get_style_size(obj, LV_PART_MAIN);
    lv_coord_t major_tick_len = lv_obj_get_style_size(obj, LV_PART_MARKER);
    lv_coord_t label_gap = lv_obj_get_style_pad_left(obj, LV_PART_MARKER);
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

    lv_draw_line_dsc_t line_marker_dsc;
    lv_draw_line_dsc_init(&line_marker_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MARKER, &line_marker_dsc);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    lv_obj_init_draw_label_dsc(obj, LV_PART_MARKER, &label_dsc);

    lv_obj_draw_hook_dsc_t hook_dsc;
    lv_obj_draw_hook_dsc_init(&hook_dsc, clip_area);
    hook_dsc.part = LV_PART_MARKER;

    uint32_t sub_cnt = meter->marker_nth - 1;
    lv_opa_t opa_main = lv_obj_get_style_opa(obj, LV_PART_MAIN);
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
        int32_t sin_low = lv_trigo_sin(angle_low + angle_ofs);
        int32_t sin_high = lv_trigo_sin(angle_high + angle_ofs);
        int32_t sin_mid = (sin_low * (256 - angle_rem) + sin_high * angle_rem) >> 8;

        int32_t cos_low = lv_trigo_cos(angle_low + angle_ofs);
        int32_t cos_high = lv_trigo_cos(angle_high + angle_ofs);
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

        int32_t value_of_line = lv_map(i, 0, meter->line_cnt - 1, meter->min_value, meter->max_value);

        lv_color_t line_color = line_dsc_act->color;
        lv_color_t line_color_ori = line_dsc_act->color;

        lv_coord_t line_width_ori = line_dsc_act->width;
        lv_coord_t line_width = line_dsc_act->width;

        lv_opa_t line_opa = line_dsc_act->opa;
        lv_opa_t line_opa_ori = line_dsc_act->opa;

        lv_meter_indicator_t * indic;
        _LV_LL_READ(&meter->indic_ll, indic) {
            if((indic->type & LV_METER_INDICATOR_TYPE_SCALE) == false) continue;
            if(value_of_line >= indic->start_value && value_of_line <= indic->end_value) {
                line_width += indic->width;
                line_opa = indic->opa > LV_OPA_MAX ? opa_main : (opa_main * indic->opa) >> 8;
                if(indic->color.full == indic->grad_color.full) {
                    line_color = indic->color;
                } else {
                    lv_opa_t ratio;
                    if(indic->scale_color_local) {
                        ratio = lv_map(value_of_line, indic->start_value, indic->end_value, LV_OPA_TRANSP, LV_OPA_COVER);
                    } else {
                        ratio = lv_map(value_of_line, meter->min_value, meter->max_value, LV_OPA_TRANSP, LV_OPA_COVER);
                    }
                    line_color = lv_color_mix(indic->grad_color, indic->color, ratio);
                }
            }
        }

        line_dsc_act->color = line_color;
        line_dsc_act->width = line_width;
        line_dsc_act->opa = line_opa;
        lv_draw_line(&p_inner, &p_outer, clip_area, line_dsc_act);
        line_dsc_act->color = line_color_ori;
        line_dsc_act->width = line_width_ori;
        line_dsc_act->opa = line_opa_ori;

        /*Draw the text*/
        if(marker) {
            uint32_t r_text = r_in_marker - label_gap;
            lv_point_t p;
            p.x = (int32_t)(((int32_t)cos_mid * r_text + 127) >> (LV_TRIGO_SHIFT)) + scale_center.y;
            p.y = (int32_t)(((int32_t)sin_mid * r_text + 127) >> (LV_TRIGO_SHIFT)) + scale_center.x;

            lv_snprintf(hook_dsc.text, sizeof(hook_dsc.text), "%d", value_of_line);
            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &hook_dsc);

            lv_point_t label_size;
            _lv_txt_get_size(&label_size, hook_dsc.text, label_dsc.font, label_dsc.letter_space, label_dsc.line_space,
                    LV_COORD_MAX, LV_TEXT_FLAG_NONE);

            lv_area_t label_cord;
            label_cord.x1 = p.x - label_size.x / 2;
            label_cord.y1 = p.y - label_size.y / 2;
            label_cord.x2 = label_cord.x1 + label_size.x;
            label_cord.y2 = label_cord.y1 + label_size.y;

            lv_draw_label(&label_cord, clip_area, &label_dsc, hook_dsc.text, NULL);
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

    lv_draw_img_dsc_t img_dsc;
    lv_draw_img_dsc_init(&img_dsc);
    lv_obj_init_draw_img_dsc(obj, LV_PART_INDICATOR, &img_dsc);

    lv_opa_t opa_main = lv_obj_get_style_opa(obj, LV_PART_MAIN);
    lv_meter_indicator_t * indic;
    _LV_LL_READ_BACK(&meter->indic_ll, indic) {
        if((indic->type & LV_METER_INDICATOR_TYPE_NEEDLE) == false) continue;

        int32_t angle = lv_map(indic->end_value, meter->min_value, meter->max_value, 0, meter->scale_angle) + angle_ofs;

        /*Draw a line*/
        if(indic->img_src == NULL) {
            lv_point_t p_end;
            p_end.y = (lv_trigo_sin(angle) * (r_out + indic->r_mod)) / LV_TRIGO_SIN_MAX + scale_center.x;
            p_end.x = (lv_trigo_cos(angle) * (r_out + indic->r_mod)) / LV_TRIGO_SIN_MAX + scale_center.y;
            line_dsc.color = indic->color;
            line_dsc.width = indic->width;
            line_dsc.opa = indic->opa > LV_OPA_MAX ? opa_main : (opa_main * indic->opa) >> 8;
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

            img_dsc.recolor_opa = indic->recolor_opa;
            img_dsc.recolor = indic->color;
            img_dsc.opa = indic->opa > LV_OPA_MAX ? opa_main : (opa_main * indic->opa) >> 8;

            angle = angle * 10;
            if(angle > 3600) angle -= 3600;
            img_dsc.angle = angle;
            lv_draw_img(&a, clip_area, indic->img_src, &img_dsc);
        }
    }

    lv_draw_rect_dsc_t mid_dsc;
    lv_draw_rect_dsc_init(&mid_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_INDICATOR, &mid_dsc);
    lv_coord_t size = lv_obj_get_style_size(obj, LV_PART_INDICATOR) / 2;
    lv_area_t nm_cord;
    nm_cord.x1 = scale_center.x - size;
    nm_cord.y1 = scale_center.y - size;
    nm_cord.x2 = scale_center.x + size;
    nm_cord.y2 = scale_center.y + size;
    lv_draw_rect(&nm_cord, clip_area, &mid_dsc);
}

#endif
