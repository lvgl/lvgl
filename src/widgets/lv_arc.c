/**
 * @file lv_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_arc.h"
#if LV_USE_ARC != 0

#include "../core/lv_group.h"
#include "../core/lv_indev.h"
#include "../misc/lv_assert.h"
#include "../misc/lv_math.h"
#include "../draw/lv_draw_arc.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_arc_class

#define VALUE_UNSET INT16_MIN
#define CLICK_OUTSIDE_BG_ANGLES ((uint32_t) 0x00U)
#define CLICK_INSIDE_BG_ANGLES  ((uint32_t) 0x01U)
#define CLICK_CLOSER_TO_MAX_END ((uint32_t) 0x00U)
#define CLICK_CLOSER_TO_MIN_END ((uint32_t) 0x01U)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_arc_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_arc_draw(lv_event_t * e);
static void lv_arc_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void inv_arc_area(lv_obj_t * arc, uint16_t start_angle, uint16_t end_angle, lv_part_t part);
static void inv_knob_area(lv_obj_t * obj);
static void get_center(const lv_obj_t * obj, lv_point_t * center, lv_coord_t * arc_r);
static lv_coord_t get_angle(const lv_obj_t * obj);
static void get_knob_area(lv_obj_t * arc, const lv_point_t * center, lv_coord_t r, lv_area_t * knob_area);
static void value_update(lv_obj_t * arc);
static lv_coord_t knob_get_extra_size(lv_obj_t * obj);
static bool lv_arc_angle_within_bg_bounds(lv_obj_t * obj, const uint32_t angle, const uint32_t tolerance_deg);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_arc_class  = {
    .constructor_cb = lv_arc_constructor,
    .event_cb = lv_arc_event,
    .instance_size = sizeof(lv_arc_t),
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .base_class = &lv_obj_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_arc_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*======================
 * Add/remove functions
 *=====================*/

/*
 * New object specific "add" or "remove" functions come here
 */

/*=====================
 * Setter functions
 *====================*/

void lv_arc_set_start_angle(lv_obj_t * obj, uint16_t start)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    if(start > 360) start -= 360;

    int16_t old_delta = arc->indic_angle_end - arc->indic_angle_start;
    int16_t new_delta = arc->indic_angle_end - start;

    if(old_delta < 0) old_delta = 360 + old_delta;
    if(new_delta < 0) new_delta = 360 + new_delta;

    if(LV_ABS(new_delta - old_delta) > 180)  lv_obj_invalidate(obj);
    else if(new_delta < old_delta) inv_arc_area(obj, arc->indic_angle_start, start, LV_PART_INDICATOR);
    else if(old_delta < new_delta) inv_arc_area(obj, start, arc->indic_angle_start, LV_PART_INDICATOR);

    inv_knob_area(obj);

    arc->indic_angle_start = start;

    inv_knob_area(obj);
}

void lv_arc_set_end_angle(lv_obj_t * obj, uint16_t end)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;
    if(end > 360) end -= 360;

    int16_t old_delta = arc->indic_angle_end - arc->indic_angle_start;
    int16_t new_delta = end - arc->indic_angle_start;

    if(old_delta < 0) old_delta = 360 + old_delta;
    if(new_delta < 0) new_delta = 360 + new_delta;

    if(LV_ABS(new_delta - old_delta) > 180)  lv_obj_invalidate(obj);
    else if(new_delta < old_delta) inv_arc_area(obj, end, arc->indic_angle_end, LV_PART_INDICATOR);
    else if(old_delta < new_delta) inv_arc_area(obj, arc->indic_angle_end, end, LV_PART_INDICATOR);

    inv_knob_area(obj);

    arc->indic_angle_end = end;

    inv_knob_area(obj);
}

void lv_arc_set_angles(lv_obj_t * obj, uint16_t start, uint16_t end)
{
    lv_arc_set_end_angle(obj, end);
    lv_arc_set_start_angle(obj, start);
}

void lv_arc_set_bg_start_angle(lv_obj_t * obj, uint16_t start)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    if(start > 360) start -= 360;

    int16_t old_delta = arc->bg_angle_end - arc->bg_angle_start;
    int16_t new_delta = arc->bg_angle_end - start;

    if(old_delta < 0) old_delta = 360 + old_delta;
    if(new_delta < 0) new_delta = 360 + new_delta;

    if(LV_ABS(new_delta - old_delta) > 180)  lv_obj_invalidate(obj);
    else if(new_delta < old_delta) inv_arc_area(obj, arc->bg_angle_start, start, LV_PART_MAIN);
    else if(old_delta < new_delta) inv_arc_area(obj, start, arc->bg_angle_start, LV_PART_MAIN);

    arc->bg_angle_start = start;

    value_update(obj);
}

void lv_arc_set_bg_end_angle(lv_obj_t * obj, uint16_t end)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    if(end > 360) end -= 360;

    int16_t old_delta = arc->bg_angle_end - arc->bg_angle_start;
    int16_t new_delta = end - arc->bg_angle_start;

    if(old_delta < 0) old_delta = 360 + old_delta;
    if(new_delta < 0) new_delta = 360 + new_delta;

    if(LV_ABS(new_delta - old_delta) > 180)  lv_obj_invalidate(obj);
    else if(new_delta < old_delta) inv_arc_area(obj, end, arc->bg_angle_end, LV_PART_MAIN);
    else if(old_delta < new_delta) inv_arc_area(obj, arc->bg_angle_end, end, LV_PART_MAIN);

    arc->bg_angle_end = end;

    value_update(obj);
}

void lv_arc_set_bg_angles(lv_obj_t * obj, uint16_t start, uint16_t end)
{
    lv_arc_set_bg_end_angle(obj, end);
    lv_arc_set_bg_start_angle(obj, start);
}

void lv_arc_set_rotation(lv_obj_t * obj, uint16_t rotation)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    arc->rotation = rotation;

    lv_obj_invalidate(obj);
}

void lv_arc_set_mode(lv_obj_t * obj, lv_arc_mode_t type)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    int16_t val = arc->value;

    arc->type = type;
    arc->value = -1; /** Force set_value handling*/

    int16_t bg_midpoint, bg_end = arc->bg_angle_end;
    if(arc->bg_angle_end < arc->bg_angle_start) bg_end = arc->bg_angle_end + 360;

    switch(arc->type) {
        case LV_ARC_MODE_SYMMETRICAL:
            bg_midpoint = (arc->bg_angle_start + bg_end) / 2;
            lv_arc_set_start_angle(obj, bg_midpoint);
            lv_arc_set_end_angle(obj, bg_midpoint);
            break;
        case LV_ARC_MODE_REVERSE:
            lv_arc_set_end_angle(obj, arc->bg_angle_end);
            break;
        default: /** LV_ARC_TYPE_NORMAL*/
            lv_arc_set_start_angle(obj, arc->bg_angle_start);
    }

    lv_arc_set_value(obj, val);
}

void lv_arc_set_value(lv_obj_t * obj, int16_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    if(arc->value == value) return;

    int16_t new_value;
    new_value = value > arc->max_value ? arc->max_value : value;
    new_value = new_value < arc->min_value ? arc->min_value : new_value;

    if(arc->value == new_value) return;
    arc->value = new_value;

    value_update(obj);
}

void lv_arc_set_range(lv_obj_t * obj, int16_t min, int16_t max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    if(arc->min_value == min && arc->max_value == max) return;

    arc->min_value = min;
    arc->max_value = max;

    if(arc->value < min) {
        arc->value = min;
    }
    if(arc->value > max) {
        arc->value = max;
    }

    value_update(obj); /*value has changed relative to the new range*/
}

void lv_arc_set_change_rate(lv_obj_t * obj, uint16_t rate)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    arc->chg_rate = rate;
}

/*=====================
 * Getter functions
 *====================*/

uint16_t lv_arc_get_angle_start(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t *) obj)->indic_angle_start;
}

uint16_t lv_arc_get_angle_end(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t *) obj)->indic_angle_end;
}

uint16_t lv_arc_get_bg_angle_start(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t *) obj)->bg_angle_start;
}

uint16_t lv_arc_get_bg_angle_end(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t *) obj)->bg_angle_end;
}

int16_t lv_arc_get_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t *) obj)->value;
}

int16_t lv_arc_get_min_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t *) obj)->min_value;
}

int16_t lv_arc_get_max_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t *) obj)->max_value;
}

lv_arc_mode_t lv_arc_get_mode(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t *) obj)->type;
}

/*=====================
 * Other functions
 *====================*/

void lv_arc_align_obj_to_angle(const lv_obj_t * obj, lv_obj_t * obj_to_align, lv_coord_t r_offset)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(obj_to_align);

    lv_obj_update_layout(obj);

    lv_point_t center;
    lv_coord_t arc_r;
    get_center(obj, &center, &arc_r);
    lv_coord_t indic_width = lv_obj_get_style_arc_width(obj, LV_PART_INDICATOR);
    lv_coord_t indic_width_half = indic_width / 2;
    arc_r -= indic_width_half;
    arc_r += r_offset;

    uint16_t angle = get_angle(obj);
    lv_coord_t knob_x = (arc_r * lv_trigo_sin(angle + 90)) >> LV_TRIGO_SHIFT;
    lv_coord_t knob_y = (arc_r * lv_trigo_sin(angle)) >> LV_TRIGO_SHIFT;
    lv_obj_align_to(obj_to_align, obj, LV_ALIGN_CENTER, knob_x, knob_y);
}

void lv_arc_rotate_obj_to_angle(const lv_obj_t * obj, lv_obj_t * obj_to_rotate, lv_coord_t r_offset)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(obj_to_rotate);

    lv_obj_update_layout(obj);

    lv_point_t center;
    lv_coord_t arc_r;
    get_center(obj, &center, &arc_r);
    lv_coord_t indic_width = lv_obj_get_style_arc_width(obj, LV_PART_INDICATOR);
    lv_coord_t indic_width_half = indic_width / 2;
    arc_r -= indic_width_half;

    arc_r += r_offset;
    lv_obj_align_to(obj_to_rotate, obj, LV_ALIGN_CENTER, 0, -arc_r);

    lv_obj_update_layout(obj);

    uint16_t angle = get_angle(obj);
    lv_coord_t pivot_x = obj_to_rotate->coords.x1 - center.x;
    lv_coord_t pivot_y = obj_to_rotate->coords.y1 - center.y;
    lv_obj_set_style_transform_pivot_x(obj_to_rotate, -pivot_x, 0);
    lv_obj_set_style_transform_pivot_y(obj_to_rotate, -pivot_y, 0);
    lv_obj_set_style_transform_angle(obj_to_rotate, angle * 10 + 900, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_arc_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_arc_t * arc = (lv_arc_t *)obj;

    /*Initialize the allocated 'ext'*/
    arc->rotation = 0;
    arc->bg_angle_start = 135;
    arc->bg_angle_end   = 45;
    arc->indic_angle_start = 135;
    arc->indic_angle_end   = 270;
    arc->type = LV_ARC_MODE_NORMAL;
    arc->value = VALUE_UNSET;
    arc->min_close = 1;
    arc->min_value = 0;
    arc->max_value = 100;
    arc->dragging = false;
    arc->chg_rate = 720;
    arc->last_tick = lv_tick_get();
    arc->last_angle = arc->indic_angle_end;
    arc->in_out = CLICK_OUTSIDE_BG_ANGLES;

    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_ext_click_area(obj, LV_DPI_DEF / 10);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_arc_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_arc_t * arc = (lv_arc_t *)lv_event_get_target(e);
    if(code == LV_EVENT_PRESSING) {
        lv_indev_t * indev = lv_indev_get_act();
        if(indev == NULL) return;

        /*Handle only pointers here*/
        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if(indev_type != LV_INDEV_TYPE_POINTER) return;

        lv_point_t p;
        lv_indev_get_point(indev, &p);

        /*Make point relative to the arc's center*/
        lv_point_t center;
        lv_coord_t r;
        get_center(obj, &center, &r);

        p.x -= center.x;
        p.y -= center.y;

        /*Enter dragging mode if pressed out of the knob*/
        if(arc->dragging == false) {
            lv_coord_t indic_width = lv_obj_get_style_arc_width(obj, LV_PART_INDICATOR);
            r -= indic_width;
            /*Add some more sensitive area if there is no advanced git testing.
             * (Advanced hit testing is more precise)*/
            if(lv_obj_has_flag(obj, LV_OBJ_FLAG_ADV_HITTEST)) {
                r -= indic_width;

            }
            else {
                r -= LV_MAX(r / 4, indic_width);
            }
            if(r < 1) r = 1;

            if(p.x * p.x + p.y * p.y > r * r) {
                arc->dragging = true;
                arc->last_tick = lv_tick_get(); /*Capture timestamp at dragging start*/
            }
        }

        /*It must be in "dragging" mode to turn the arc*/
        if(arc->dragging == false) return;

        /*No angle can be determined if exactly the middle of the arc is being pressed*/
        if(p.x == 0 && p.y == 0) return;

        /*Calculate the angle of the pressed point*/
        int16_t angle;
        int16_t bg_end = arc->bg_angle_end;
        if(arc->bg_angle_end < arc->bg_angle_start) {
            bg_end = arc->bg_angle_end + 360;
        }

        angle = lv_atan2(p.y, p.x);
        angle -= arc->rotation;
        angle -= arc->bg_angle_start;  /*Make the angle relative to the start angle*/

        /* If we click near the bg_angle_start the angle will be close to 360° instead of an small angle */
        if(angle < 0) angle += 360;

        const uint32_t circumference = (uint32_t)((2U * r * 314U) / 100U);  /* Equivalent to: 2r * 3.14, avoiding floats */
        const uint32_t tolerance_deg = (360U * LV_DPX(50U)) / circumference;
        const uint32_t min_close_prev = (uint32_t) arc->min_close;

        const bool is_angle_within_bg_bounds = lv_arc_angle_within_bg_bounds(obj, (uint32_t) angle, tolerance_deg);
        if(!is_angle_within_bg_bounds) {
            return;
        }

        int16_t deg_range = bg_end - arc->bg_angle_start;
        int16_t last_angle_rel = arc->last_angle - arc->bg_angle_start;
        int16_t delta_angle = angle - last_angle_rel;

        /*Do not allow big jumps (jumps bigger than 280°).
         *It's mainly to avoid jumping to the opposite end if the "dead" range between min. and max. is crossed.
         *Check which end was closer on the last valid press (arc->min_close) and prefer that end*/
        if(LV_ABS(delta_angle) > 280) {
            if(arc->min_close) angle = 0;
            else angle = deg_range;
        }
        /* Check if click was outside the background arc start and end angles */
        else if(CLICK_OUTSIDE_BG_ANGLES == arc->in_out) {
            if(arc->min_close) angle = -deg_range;
            else angle = deg_range;
        }
        else { /* Keep the angle value */ }

        /* Prevent big jumps when the click goes from start to end angle in the invisible
         * part of the background arc without being released */
        if(((min_close_prev == CLICK_CLOSER_TO_MIN_END) && (arc->min_close == CLICK_CLOSER_TO_MAX_END))
           && ((CLICK_OUTSIDE_BG_ANGLES == arc->in_out) && (LV_ABS(delta_angle) > 280))) {
            angle = 0;
        }
        else if(((min_close_prev == CLICK_CLOSER_TO_MAX_END) && (arc->min_close == CLICK_CLOSER_TO_MIN_END))
                && (CLICK_OUTSIDE_BG_ANGLES == arc->in_out)) {
            angle = deg_range;
        }
        else { /* Keep the angle value */ }

        /*Calculate the slew rate limited angle based on change rate (degrees/sec)*/
        delta_angle = angle - last_angle_rel;

        uint32_t delta_tick = lv_tick_elaps(arc->last_tick);
        /* delta_angle_max can never be signed. delta_tick is always signed, same for ch_rate */
        const uint16_t delta_angle_max = (arc->chg_rate * delta_tick) / 1000;

        if(delta_angle > delta_angle_max) {
            delta_angle = delta_angle_max;
        }
        else if(delta_angle < -delta_angle_max) {
            delta_angle = -delta_angle_max;
        }
        else { /* Nothing to do */ }

        angle = last_angle_rel + delta_angle; /*Apply the limited angle change*/

        /*Rounding for symmetry*/
        int32_t round = ((bg_end - arc->bg_angle_start) * 8) / (arc->max_value - arc->min_value);
        round = (round + 4) >> 4;
        angle += round;

        angle += arc->bg_angle_start;  /*Make the angle absolute again*/

        /*Set the new value*/
        int16_t old_value = arc->value;
        int16_t new_value = lv_map(angle, arc->bg_angle_start, bg_end, arc->min_value, arc->max_value);
        if(new_value != lv_arc_get_value(obj)) {
            arc->last_tick = lv_tick_get(); /*Cache timestamp for the next iteration*/
            lv_arc_set_value(obj, new_value); /*set_value caches the last_angle for the next iteration*/
            if(new_value != old_value) {
                res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
                if(res != LV_RES_OK) return;
            }
        }

        /*Don't let the elapsed time become too big while sitting on an end point*/
        if(new_value == arc->min_value || new_value == arc->max_value) {
            arc->last_tick = lv_tick_get(); /*Cache timestamp for the next iteration*/
        }
    }
    else if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        arc->dragging = false;

        /*Leave edit mode if released. (No need to wait for LONG_PRESS)*/
        lv_group_t * g             = lv_obj_get_group(obj);
        bool editing               = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            if(editing) lv_group_set_editing(g, false);
        }

    }
    else if(code == LV_EVENT_KEY) {
        char c = *((char *)lv_event_get_param(e));

        int16_t old_value = arc->value;
        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            lv_arc_set_value(obj, lv_arc_get_value(obj) + 1);
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            lv_arc_set_value(obj, lv_arc_get_value(obj) - 1);
        }

        if(old_value != arc->value) {
            res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return;
        }
    }
    else if(code == LV_EVENT_HIT_TEST) {
        lv_hit_test_info_t * info = lv_event_get_param(e);

        lv_point_t p;
        lv_coord_t r;
        get_center(obj, &p, &r);

        lv_coord_t ext_click_area = 0;
        if(obj->spec_attr) ext_click_area = obj->spec_attr->ext_click_pad;

        lv_coord_t w = lv_obj_get_style_arc_width(obj, LV_PART_MAIN);
        r -= w + ext_click_area;

        lv_area_t a;
        /*Invalid if clicked inside*/
        lv_area_set(&a, p.x - r, p.y - r, p.x + r, p.y + r);
        if(_lv_area_is_point_on(&a, info->point, LV_RADIUS_CIRCLE)) {
            info->res = false;
            return;
        }

        /*Valid if no clicked outside*/
        lv_area_increase(&a, w + ext_click_area * 2, w + ext_click_area * 2);
        info->res = _lv_area_is_point_on(&a, info->point, LV_RADIUS_CIRCLE);
    }
    else if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        lv_coord_t bg_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
        lv_coord_t bg_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
        lv_coord_t bg_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
        lv_coord_t bg_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
        lv_coord_t bg_pad = LV_MAX4(bg_left, bg_right, bg_top, bg_bottom);

        lv_coord_t knob_left = lv_obj_get_style_pad_left(obj, LV_PART_KNOB);
        lv_coord_t knob_right = lv_obj_get_style_pad_right(obj, LV_PART_KNOB);
        lv_coord_t knob_top = lv_obj_get_style_pad_top(obj, LV_PART_KNOB);
        lv_coord_t knob_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);
        lv_coord_t knob_pad = LV_MAX4(knob_left, knob_right, knob_top, knob_bottom) + 2;

        lv_coord_t knob_extra_size = knob_pad - bg_pad;
        knob_extra_size += knob_get_extra_size(obj);

        lv_coord_t * s = lv_event_get_param(e);
        *s = LV_MAX(*s, knob_extra_size);
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        lv_arc_draw(e);
    }
}

static void lv_arc_draw(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_arc_t * arc = (lv_arc_t *)obj;

    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);

    lv_point_t center;
    lv_coord_t arc_r;
    get_center(obj, &center, &arc_r);

    lv_obj_draw_part_dsc_t part_draw_dsc;
    lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);

    /*Draw the background arc*/
    lv_draw_arc_dsc_t arc_dsc;
    if(arc_r > 0) {
        lv_draw_arc_dsc_init(&arc_dsc);
        lv_obj_init_draw_arc_dsc(obj, LV_PART_MAIN, &arc_dsc);

        part_draw_dsc.part = LV_PART_MAIN;
        part_draw_dsc.class_p = MY_CLASS;
        part_draw_dsc.type = LV_ARC_DRAW_PART_BACKGROUND;
        part_draw_dsc.p1 = &center;
        part_draw_dsc.radius = arc_r;
        part_draw_dsc.arc_dsc = &arc_dsc;
        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);

        lv_draw_arc(draw_ctx, &arc_dsc, &center, part_draw_dsc.radius, arc->bg_angle_start + arc->rotation,
                    arc->bg_angle_end + arc->rotation);

        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
    }

    /*Make the indicator arc smaller or larger according to its greatest padding value*/
    lv_coord_t left_indic = lv_obj_get_style_pad_left(obj, LV_PART_INDICATOR);
    lv_coord_t right_indic = lv_obj_get_style_pad_right(obj, LV_PART_INDICATOR);
    lv_coord_t top_indic = lv_obj_get_style_pad_top(obj, LV_PART_INDICATOR);
    lv_coord_t bottom_indic = lv_obj_get_style_pad_bottom(obj, LV_PART_INDICATOR);
    lv_coord_t indic_r = arc_r - LV_MAX4(left_indic, right_indic, top_indic, bottom_indic);

    if(indic_r > 0) {
        lv_draw_arc_dsc_init(&arc_dsc);
        lv_obj_init_draw_arc_dsc(obj, LV_PART_INDICATOR, &arc_dsc);

        part_draw_dsc.part = LV_PART_INDICATOR;
        part_draw_dsc.class_p = MY_CLASS;
        part_draw_dsc.type = LV_ARC_DRAW_PART_FOREGROUND;
        part_draw_dsc.p1 = &center;
        part_draw_dsc.radius = indic_r;
        part_draw_dsc.arc_dsc = &arc_dsc;
        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);

        if(arc_dsc.width > part_draw_dsc.radius) arc_dsc.width = part_draw_dsc.radius;
        lv_draw_arc(draw_ctx, &arc_dsc, &center, part_draw_dsc.radius, arc->indic_angle_start + arc->rotation,
                    arc->indic_angle_end + arc->rotation);

        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
    }

    lv_area_t knob_area;
    get_knob_area(obj, &center, arc_r, &knob_area);

    lv_draw_rect_dsc_t knob_rect_dsc;
    lv_draw_rect_dsc_init(&knob_rect_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_KNOB, &knob_rect_dsc);

    part_draw_dsc.part = LV_PART_KNOB;
    part_draw_dsc.class_p = MY_CLASS;
    part_draw_dsc.type = LV_ARC_DRAW_PART_KNOB;
    part_draw_dsc.draw_area = &knob_area;
    part_draw_dsc.rect_dsc = &knob_rect_dsc;
    lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);

    lv_draw_rect(draw_ctx, &knob_rect_dsc, &knob_area);

    lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);
}

static void inv_arc_area(lv_obj_t * obj, uint16_t start_angle, uint16_t end_angle, lv_part_t part)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    /*Skip this complicated invalidation if the arc is not visible*/
    if(lv_obj_is_visible(obj) == false) return;

    lv_arc_t * arc = (lv_arc_t *)obj;

    if(start_angle == end_angle) return;

    if(start_angle > 360) start_angle -= 360;
    if(end_angle > 360) end_angle -= 360;

    start_angle += arc->rotation;
    end_angle += arc->rotation;

    if(start_angle > 360) start_angle -= 360;
    if(end_angle > 360) end_angle -= 360;

    lv_coord_t r;
    lv_point_t c;
    get_center(obj, &c, &r);

    lv_coord_t w = lv_obj_get_style_arc_width(obj, part);
    lv_coord_t rounded = lv_obj_get_style_arc_rounded(obj, part);

    lv_area_t inv_area;
    lv_draw_arc_get_area(c.x, c.y, r, start_angle, end_angle, w, rounded, &inv_area);

    lv_obj_invalidate_area(obj, &inv_area);
}

static void inv_knob_area(lv_obj_t * obj)
{
    lv_point_t c;
    lv_coord_t r;
    get_center(obj, &c, &r);

    lv_area_t a;
    get_knob_area(obj, &c, r, &a);

    lv_coord_t knob_extra_size = knob_get_extra_size(obj);

    if(knob_extra_size > 0) {
        lv_area_increase(&a, knob_extra_size, knob_extra_size);
    }

    lv_obj_invalidate_area(obj, &a);
}

static void get_center(const lv_obj_t * obj, lv_point_t * center, lv_coord_t * arc_r)
{
    lv_coord_t left_bg = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t right_bg = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    lv_coord_t top_bg = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t bottom_bg = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

    lv_coord_t r = (LV_MIN(lv_obj_get_width(obj) - left_bg - right_bg,
                           lv_obj_get_height(obj) - top_bg - bottom_bg)) / 2;

    center->x = obj->coords.x1 + r + left_bg;
    center->y = obj->coords.y1 + r + top_bg;

    if(arc_r) *arc_r = r;
}

static lv_coord_t get_angle(const lv_obj_t * obj)
{
    lv_arc_t * arc = (lv_arc_t *)obj;
    uint16_t angle = arc->rotation;
    if(arc->type == LV_ARC_MODE_NORMAL) {
        angle += arc->indic_angle_end;
    }
    else if(arc->type == LV_ARC_MODE_REVERSE) {
        angle += arc->indic_angle_start;
    }
    else if(arc->type == LV_ARC_MODE_SYMMETRICAL) {
        int16_t bg_end = arc->bg_angle_end;
        if(arc->bg_angle_end < arc->bg_angle_start) bg_end = arc->bg_angle_end + 360;
        int16_t indic_end = arc->indic_angle_end;
        if(arc->indic_angle_end < arc->indic_angle_start) indic_end = arc->indic_angle_end + 360;

        int32_t angle_midpoint = (int32_t)(arc->bg_angle_start + bg_end) / 2;
        if(arc->indic_angle_start < angle_midpoint) angle += arc->indic_angle_start;
        else if(indic_end > angle_midpoint) angle += arc->indic_angle_end;
        else angle += angle_midpoint;
    }

    return angle;
}

static void get_knob_area(lv_obj_t * obj, const lv_point_t * center, lv_coord_t r, lv_area_t * knob_area)
{
    lv_coord_t indic_width = lv_obj_get_style_arc_width(obj, LV_PART_INDICATOR);
    lv_coord_t indic_width_half = indic_width / 2;
    r -= indic_width_half;

    lv_coord_t angle = get_angle(obj);
    lv_coord_t knob_x = (r * lv_trigo_sin(angle + 90)) >> LV_TRIGO_SHIFT;
    lv_coord_t knob_y = (r * lv_trigo_sin(angle)) >> LV_TRIGO_SHIFT;

    lv_coord_t left_knob = lv_obj_get_style_pad_left(obj, LV_PART_KNOB);
    lv_coord_t right_knob = lv_obj_get_style_pad_right(obj, LV_PART_KNOB);
    lv_coord_t top_knob = lv_obj_get_style_pad_top(obj, LV_PART_KNOB);
    lv_coord_t bottom_knob = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);

    knob_area->x1 = center->x + knob_x - left_knob - indic_width_half;
    knob_area->x2 = center->x + knob_x + right_knob + indic_width_half;
    knob_area->y1 = center->y + knob_y - top_knob - indic_width_half;
    knob_area->y2 = center->y + knob_y + bottom_knob + indic_width_half;
}

/**
 * Used internally to update arc angles after a value change
 * @param arc pointer to an arc object
 */
static void value_update(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    /*If the value is still not set to any value do not update*/
    if(arc->value == VALUE_UNSET) return;

    int16_t bg_midpoint, range_midpoint, bg_end = arc->bg_angle_end;
    if(arc->bg_angle_end < arc->bg_angle_start) bg_end = arc->bg_angle_end + 360;

    int16_t angle;
    switch(arc->type) {
        case LV_ARC_MODE_SYMMETRICAL:
            bg_midpoint = (arc->bg_angle_start + bg_end) / 2;
            range_midpoint = (int32_t)(arc->min_value + arc->max_value) / 2;

            if(arc->value < range_midpoint) {
                angle = lv_map(arc->value, arc->min_value, range_midpoint, arc->bg_angle_start, bg_midpoint);
                lv_arc_set_start_angle(obj, angle);
                lv_arc_set_end_angle(obj, bg_midpoint);
            }
            else {
                angle = lv_map(arc->value, range_midpoint, arc->max_value, bg_midpoint, bg_end);
                lv_arc_set_start_angle(obj, bg_midpoint);
                lv_arc_set_end_angle(obj, angle);
            }
            break;
        case LV_ARC_MODE_REVERSE:
            angle = lv_map(arc->value, arc->min_value, arc->max_value, bg_end, arc->bg_angle_start);
            lv_arc_set_angles(obj, angle, arc->bg_angle_end);
            break;
        case LV_ARC_MODE_NORMAL:
            angle = lv_map(arc->value, arc->min_value, arc->max_value, arc->bg_angle_start, bg_end);
            lv_arc_set_angles(obj, arc->bg_angle_start, angle);

            break;
        default:
            LV_LOG_WARN("Invalid mode: %d", arc->type);
            return;
    }
    arc->last_angle = angle; /*Cache angle for slew rate limiting*/
}

static lv_coord_t knob_get_extra_size(lv_obj_t * obj)
{
    lv_coord_t knob_shadow_size = 0;
    knob_shadow_size += lv_obj_get_style_shadow_width(obj, LV_PART_KNOB);
    knob_shadow_size += lv_obj_get_style_shadow_spread(obj, LV_PART_KNOB);
    knob_shadow_size += LV_ABS(lv_obj_get_style_shadow_ofs_x(obj, LV_PART_KNOB));
    knob_shadow_size += LV_ABS(lv_obj_get_style_shadow_ofs_y(obj, LV_PART_KNOB));

    lv_coord_t knob_outline_size = 0;
    knob_outline_size += lv_obj_get_style_outline_width(obj, LV_PART_KNOB);
    knob_outline_size += lv_obj_get_style_outline_pad(obj, LV_PART_KNOB);

    return LV_MAX(knob_shadow_size, knob_outline_size);
}

/**
 * Check if angle is within arc background bounds
 *
 * In order to avoid unexpected value update of the arc value when the user clicks
 * outside of the arc background we need to check if the angle (of the clicked point)
 * is within the bounds of the background.
 *
 * A tolerance (extra room) also should be taken into consideration.
 *
 * E.g. Arc with start angle of 0° and end angle of 90°, the background is only visible in
 * that range, from 90° to 360° the background is invisible. Click in 150° should not update
 * the arc value, click within the arc angle range should.
 *
 * IMPORTANT NOTE: angle is always relative to bg_angle_start, e.g. if bg_angle_start is 30
 * and we click a bit to the left, angle is 10, not the expected 40.
 *
 * @param obj   Pointer to lv_arc
 * @param angle Angle to be checked
 * @param tolerance_deg Tolerance
 *
 * @return true if angle is within arc background bounds, false otherwise
 */
static bool lv_arc_angle_within_bg_bounds(lv_obj_t * obj, const uint32_t angle, const uint32_t tolerance_deg)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    uint32_t smaller_angle = 0;
    uint32_t bigger_angle = 0;

    /* Determine which background angle is smaller and bigger */
    if(arc->bg_angle_start < arc->bg_angle_end) {
        bigger_angle = arc->bg_angle_end;
        smaller_angle = arc->bg_angle_start;
    }
    else {
        bigger_angle = (360 - arc->bg_angle_start) + arc->bg_angle_end;
        smaller_angle = 0;
    }

    /* Angle is between both background angles */
    if((smaller_angle <= angle) && (angle <= bigger_angle)) {

        if(((bigger_angle - smaller_angle) / 2U) >= angle) {
            arc->min_close = 1;
        }
        else {
            arc->min_close = 0;
        }

        arc->in_out = CLICK_INSIDE_BG_ANGLES;

        return true;
    }
    /* Distance between background start and end angles is less than tolerance,
     * consider the click inside the arc */
    else if(((smaller_angle - tolerance_deg) <= 0U) &&
            (360U - (bigger_angle + (smaller_angle - tolerance_deg)))) {

        arc->min_close = 1;
        arc->in_out = CLICK_INSIDE_BG_ANGLES;
        return true;
    }
    else { /* Case handled below */ }

    /* Legends:
     * 0° = angle 0
     * 360° = angle 360
     * T: Tolerance
     * A: Angle
     * S: Arc background start angle
     * E: Arc background end angle
     *
     * Start angle is bigger or equal to tolerance */
    if((smaller_angle >= tolerance_deg)
       /* (360° - T) --- A --- 360° */
       && ((angle >= (360U - tolerance_deg)) && (angle <= 360U))) {

        arc->min_close = 1;
        arc->in_out = CLICK_OUTSIDE_BG_ANGLES;
        return true;
    }
    /* Tolerance is bigger than bg start angle */
    else if((smaller_angle < tolerance_deg)
            /* (360° - (T - S)) --- A --- 360° */
            && (((360U - (tolerance_deg - smaller_angle)) <= angle)) && (angle <= 360U)) {

        arc->min_close = 1;
        arc->in_out = CLICK_OUTSIDE_BG_ANGLES;
        return true;
    }
    /* 360° is bigger than background end angle + tolerance */
    else if((360U >= (bigger_angle + tolerance_deg))
            /* E --- A --- (E + T) */
            && ((bigger_angle <= (angle + smaller_angle)) &&
                ((angle + smaller_angle) <= (bigger_angle + tolerance_deg)))) {

        arc->min_close = 0;
        arc->in_out = CLICK_OUTSIDE_BG_ANGLES;
        return true;
    }
    /* Background end angle + tolerance is bigger than 360° and bg_start_angle + tolerance is not near 0° + ((bg_end_angle + tolerance) - 360°)
     * Here we can assume background is not near 0° because of the first two initial checks */
    else if((360U < (bigger_angle + tolerance_deg))
            && (angle <= 0U + ((bigger_angle + tolerance_deg) - 360U)) && (angle > bigger_angle)) {

        arc->min_close = 0;
        arc->in_out = CLICK_OUTSIDE_BG_ANGLES;
        return true;
    }
    else {
        /* Nothing to do */
    }

    return false;
}

#endif
