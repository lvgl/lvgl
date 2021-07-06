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
static void get_center(lv_obj_t * obj, lv_point_t * center, lv_coord_t * arc_r);
static void get_knob_area(lv_obj_t * arc, const lv_point_t * center, lv_coord_t r, lv_area_t * knob_area);
static void value_update(lv_obj_t * arc);

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

/**
 * Create a arc object
 * @param par pointer to an object, it will be the parent of the new arc
 * @return pointer to the created arc
 */
lv_obj_t * lv_arc_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin")
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

/**
 * Set the start angle of an arc. 0 deg: right, 90 bottom, etc.
 * @param arc pointer to an arc object
 * @param start the start angle [0..360]
 */
void lv_arc_set_start_angle(lv_obj_t * obj, uint16_t start)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    if(start > 360) start -= 360;

    /*Too large move, the whole arc need to be invalidated anyway*/
    if(LV_ABS(start -arc->indic_angle_start) >= 180) {
        lv_obj_invalidate(obj);
    }
    /*Only a smaller incremental move*/
    else if(arc->indic_angle_start >arc->indic_angle_end && start >arc->indic_angle_end) {
        inv_arc_area(obj, LV_MIN(arc->indic_angle_start, start), LV_MAX(arc->indic_angle_start, start),
                     LV_PART_INDICATOR);
    }
    /*Only a smaller incremental move*/
    else  if(arc->indic_angle_start <arc->indic_angle_end && start <arc->indic_angle_end) {
        inv_arc_area(obj, LV_MIN(arc->indic_angle_start, start), LV_MAX(arc->indic_angle_start, start),
                     LV_PART_INDICATOR);
    }
    /*Crossing the start angle makes the whole arc change*/
    else {
        lv_obj_invalidate(obj);
    }

   arc->indic_angle_start = start;
}

/**
 * Set the start angle of an arc. 0 deg: right, 90 bottom, etc.
 * @param arc pointer to an arc object
 * @param start the start angle [0..360]
 */
void lv_arc_set_end_angle(lv_obj_t * obj, uint16_t end)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    if(end > (arc->indic_angle_start + 360)) end =arc->indic_angle_start + 360;

    /*Too large move, the whole arc need to be invalidated anyway*/
    if(LV_ABS(end -arc->indic_angle_end) >= 180) {
        lv_obj_invalidate(obj);
    }
    /*Only a smaller incremental move*/
    else if(arc->indic_angle_end >arc->indic_angle_start && end >arc->indic_angle_start) {
        inv_arc_area(obj, LV_MIN(arc->indic_angle_end, end), LV_MAX(arc->indic_angle_end, end), LV_PART_INDICATOR);
    }
    /*Only a smaller incremental move*/
    else  if(arc->indic_angle_end <arc->indic_angle_start && end <arc->indic_angle_start) {
        inv_arc_area(obj, LV_MIN(arc->indic_angle_end, end), LV_MAX(arc->indic_angle_end, end), LV_PART_INDICATOR);
    }
    /*Crossing the end angle makes the whole arc change*/
    else {
        lv_obj_invalidate(obj);
    }

   arc->indic_angle_end = end;
}

/**
 * Set the start and end angles
 * @param arc pointer to an arc object
 * @param start the start angle
 * @param end the end angle
 */
void lv_arc_set_angles(lv_obj_t * obj, uint16_t start, uint16_t end)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    if(start > 360) start -= 360;
    if(end > (start + 360)) end = start + 360;

    inv_arc_area(obj,arc->indic_angle_start,arc->indic_angle_end, LV_PART_INDICATOR);

    arc->indic_angle_start = start;
    arc->indic_angle_end = end;

    inv_arc_area(obj,arc->indic_angle_start,arc->indic_angle_end, LV_PART_INDICATOR);
}

/**
 * Set the start angle of an arc background. 0 deg: right, 90 bottom, etc.
 * @param arc pointer to an arc object
 * @param start the start angle
 */
void lv_arc_set_bg_start_angle(lv_obj_t * obj, uint16_t start)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    if(start > 360) start -= 360;

    /*Too large move, the whole arc need to be invalidated anyway*/
    if(LV_ABS(start -arc->bg_angle_start) >= 180) {
        lv_obj_invalidate(obj);
    }
    /*Only a smaller incremental move*/
    else if(arc->bg_angle_start >arc->bg_angle_end && start >arc->bg_angle_end) {
        inv_arc_area(obj, LV_MIN(arc->bg_angle_start, start), LV_MAX(arc->bg_angle_start, start), LV_PART_MAIN);
    }
    /*Only a smaller incremental move*/
    else  if(arc->bg_angle_start <arc->bg_angle_end && start <arc->bg_angle_end) {
        inv_arc_area(obj, LV_MIN(arc->bg_angle_start, start), LV_MAX(arc->bg_angle_start, start), LV_PART_MAIN);
    }
    /*Crossing the start angle makes the whole arc change*/
    else {
        lv_obj_invalidate(obj);
    }

   arc->bg_angle_start = start;

    value_update(obj);
}

/**
 * Set the start angle of an arc background. 0 deg: right, 90 bottom etc.
 * @param arc pointer to an arc object
 * @param end the end angle
 */
void lv_arc_set_bg_end_angle(lv_obj_t * obj, uint16_t end)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;


    if(end > (arc->bg_angle_start + 360)) end =arc->bg_angle_start + 360;

    /*Too large move, the whole arc need to be invalidated anyway*/
    if(LV_ABS(end -arc->bg_angle_end) >= 180) {
        lv_obj_invalidate(obj);
    }
    /*Only a smaller incremental move*/
    else if(arc->bg_angle_end >arc->bg_angle_start && end >arc->bg_angle_start) {
        inv_arc_area(obj, LV_MIN(arc->bg_angle_end, end), LV_MAX(arc->bg_angle_end, end), LV_PART_MAIN);
    }
    /*Only a smaller incremental move*/
    else  if(arc->bg_angle_end <arc->bg_angle_start && end <arc->bg_angle_start) {
        inv_arc_area(obj, LV_MIN(arc->bg_angle_end, end), LV_MAX(arc->bg_angle_end, end), LV_PART_MAIN);
    }
    /*Crossing the end angle makes the whole arc change*/
    else {
        lv_obj_invalidate(obj);
    }

    value_update(obj);

   arc->bg_angle_end = end;
}

/**
 * Set the start and end angles of the arc background
 * @param arc pointer to an arc object
 * @param start the start angle
 * @param end the end angle
 */
void lv_arc_set_bg_angles(lv_obj_t * obj, uint16_t start, uint16_t end)
{

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    if(start > 360) start -= 360;
    if(end > (start + 360)) end = start + 360;

    inv_arc_area(obj,arc->bg_angle_start,arc->bg_angle_end, LV_PART_MAIN);

   arc->bg_angle_start = start;
   arc->bg_angle_end = end;

    inv_arc_area(obj,arc->bg_angle_start,arc->bg_angle_end, LV_PART_MAIN);

    value_update(obj);
}

/**
 * Set the rotation for the whole arc
 * @param arc pointer to an arc object
 * @param rotation rotation angle
 */
void lv_arc_set_rotation(lv_obj_t * obj, uint16_t rotation)
{

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

   arc->rotation = rotation;

    lv_obj_invalidate(obj);
}

/**
 * Set the type of arc.
 * @param arc pointer to arc object
 * @param type arc type
 */
void lv_arc_set_mode(lv_obj_t * obj, lv_arc_mode_t type)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    int16_t val =arc->value;

   arc->type = type;
   arc->value = -1; /** Force set_value handling*/

    int16_t bg_midpoint, bg_end =arc->bg_angle_end;
    if(arc->bg_angle_end <arc->bg_angle_start) bg_end =arc->bg_angle_end + 360;

    switch(arc->type) {
        case LV_ARC_MODE_SYMMETRICAL:
            bg_midpoint = (arc->bg_angle_start + bg_end) / 2;
            lv_arc_set_start_angle(obj, bg_midpoint);
            lv_arc_set_end_angle(obj, bg_midpoint);
            break;
        case LV_ARC_MODE_REVERSE:
            lv_arc_set_end_angle(obj,arc->bg_angle_end);
            break;
        default: /** LV_ARC_TYPE_NORMAL*/
            lv_arc_set_start_angle(obj,arc->bg_angle_start);
    }

    lv_arc_set_value(obj, val);
}

/**
 * Set a new value on the arc
 * @param arc pointer to a arc object
 * @param value new value
 */
void lv_arc_set_value(lv_obj_t * obj, int16_t value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    if(arc->value == value) return;

    int16_t new_value;
    new_value = value >arc->max_value ?arc->max_value : value;
    new_value = new_value <arc->min_value ?arc->min_value : new_value;

    if(arc->value == new_value) return;
   arc->value = new_value;

    value_update(obj);
}

/**
 * Set minimum and the maximum values of a arc
 * @param arc pointer to the arc object
 * @param min minimum value
 * @param max maximum value
 */
void lv_arc_set_range(lv_obj_t * obj, int16_t min, int16_t max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    if(arc->min_value == min &&arc->max_value == max) return;

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

/**
 * Set the threshold of arc knob increments
 * position.
 * @param arc pointer to a arc object
 * @param threshold increment threshold
 */
void lv_arc_set_change_rate(lv_obj_t * obj, uint16_t rate)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

   arc->chg_rate = rate;
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the start angle of an arc.
 * @param arc pointer to an arc object
 * @return the start angle [0..360]
 */
uint16_t lv_arc_get_angle_start(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t*) obj)->indic_angle_start;
}

/**
 * Get the end angle of an arc.
 * @param arc pointer to an arc object
 * @return the end angle [0..360]
 */
 uint16_t lv_arc_get_angle_end(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t*) obj)->indic_angle_end;
}

/**
 * Get the start angle of an arc background.
 * @param arc pointer to an arc object
 * @return the start angle [0..360]
 */
uint16_t lv_arc_get_bg_angle_start(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t*) obj)->bg_angle_start;
}

/**
 * Get the end angle of an arc background.
 * @param arc pointer to an arc object
 * @return the end angle [0..360]
 */
uint16_t lv_arc_get_bg_angle_end(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t*) obj)->bg_angle_end;
}


/**
 * Get the value of a arc
 * @param arc pointer to a arc object
 * @return the value of the arc
 */
int16_t lv_arc_get_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t*) obj)->value;
}

/**
 * Get the minimum value of a arc
 * @param arc pointer to a arc object
 * @return the minimum value of the arc
 */
int16_t lv_arc_get_min_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t*) obj)->min_value;
}

/**
 * Get the maximum value of a arc
 * @param arc pointer to a arc object
 * @return the maximum value of the arc
 */
int16_t lv_arc_get_max_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t*) obj)->max_value;
}

/**
 * Get whether the arc is type or not.
 * @param arc pointer to a arc object
 * @return arc type
 */
lv_arc_mode_t lv_arc_get_mode(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    return ((lv_arc_t*) obj)->type;
}

/*=====================
 * Other functions
 *====================*/

/*
 * New object specific "other" functions come here
 */

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
   arc->last_angle =arc->indic_angle_end;

   lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
   lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
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

        p.x -=  center.x;
        p.y -=  center.y;

        /*Enter dragging mode if pressed out of the knob*/
        if(arc->dragging == false) {
            lv_coord_t indic_width = lv_obj_get_style_arc_width(obj, LV_PART_INDICATOR);
            r -=  indic_width;
            r -= r / 2; /*Add some more sensitive area*/
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
        int16_t bg_end =arc->bg_angle_end;
        if(arc->bg_angle_end <arc->bg_angle_start) {
            bg_end =arc->bg_angle_end + 360;
        }

        angle = lv_atan2(p.y, p.x);
        angle -=arc->rotation;
        angle -=arc->bg_angle_start;   /*Make the angle relative to the start angle*/

        if(angle < 0) angle += 360;

        int16_t deg_range = bg_end -arc->bg_angle_start;

        int16_t last_angle_rel =arc->last_angle -arc->bg_angle_start;
        int16_t delta_angle = angle - last_angle_rel;

        /*Do not allow big jumps.
         *It's mainly to avoid jumping to the opposite end if the "dead" range between min. an max. is crossed.
         *Check which was closer on the last valid press (arc->min_close) and prefer that end*/
        if(LV_ABS(delta_angle) > 280) {
            if(arc->min_close) angle = 0;
            else angle = deg_range;
        }
        else {
            if(angle < deg_range / 2)arc->min_close = 1;
            else arc->min_close = 0;
        }

        /*Calculate the slew rate limited angle based on change rate (degrees/sec)*/
        delta_angle = angle - last_angle_rel;
        uint32_t delta_tick = lv_tick_elaps(arc->last_tick);
        int16_t delta_angle_max = (arc->chg_rate * delta_tick) / 1000;

        if(delta_angle > delta_angle_max) {
            delta_angle = delta_angle_max;
        }
        else if(delta_angle < -delta_angle_max) {
            delta_angle = -delta_angle_max;
        }

        angle = last_angle_rel + delta_angle; /*Apply the limited angle change*/

        /*Rounding for symmetry*/
        int32_t round = ((bg_end -arc->bg_angle_start) * 8) / (arc->max_value -arc->min_value);
        round = (round + 4) >> 4;
        angle += round;

        angle +=arc->bg_angle_start;   /*Make the angle absolute again*/

        /*Set the new value*/
        int16_t old_value =arc->value;
        int16_t new_value = lv_map(angle,arc->bg_angle_start, bg_end,arc->min_value,arc->max_value);
        if(new_value != lv_arc_get_value(obj)) {
           arc->last_tick = lv_tick_get(); /*Cache timestamp for the next iteration*/
            lv_arc_set_value(obj, new_value); /*set_value caches the last_angle for the next iteration*/
            if(new_value != old_value) {
                res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
                if(res != LV_RES_OK) return;
            }
        }

        /*Don1't let the elapsed time to big while sitting on an end point*/
        if(new_value ==arc->min_value || new_value ==arc->max_value) {
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

        int16_t old_value =arc->value;
        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            lv_arc_set_value(obj, lv_arc_get_value(obj) + 1);
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            lv_arc_set_value(obj, lv_arc_get_value(obj) - 1);
        }

        if(old_value !=arc->value) {
            res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return;
        }
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

        lv_coord_t * s = lv_event_get_param(e);
        *s = LV_MAX(*s, knob_pad - bg_pad);
    } else if(code == LV_EVENT_DRAW_MAIN) {
        lv_arc_draw(e);
    }
}

static void lv_arc_draw(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_arc_t * arc = (lv_arc_t *)obj;

    const lv_area_t * clip_area = lv_event_get_param(e);

    lv_point_t center;
    lv_coord_t arc_r;
    get_center(obj, &center, &arc_r);

    lv_obj_draw_part_dsc_t obj_draw_dsc;
    lv_obj_draw_dsc_init(&obj_draw_dsc, clip_area);

    /*Draw the background arc*/
    lv_draw_arc_dsc_t arc_dsc;
    if(arc_r > 0) {
        lv_draw_arc_dsc_init(&arc_dsc);
        lv_obj_init_draw_arc_dsc(obj, LV_PART_MAIN, &arc_dsc);

        obj_draw_dsc.part = LV_PART_MAIN;
        obj_draw_dsc.p1 = &center;
        obj_draw_dsc.radius = arc_r;
        obj_draw_dsc.arc_dsc = &arc_dsc;
        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &obj_draw_dsc);

        lv_draw_arc(center.x, center.y, arc_r, arc->bg_angle_start + arc->rotation,
                   arc->bg_angle_end + arc->rotation, clip_area,
                    &arc_dsc);

        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &obj_draw_dsc);
    }

    /*make the indicator arc smaller or larger according to its greatest padding value*/
    lv_coord_t left_indic = lv_obj_get_style_pad_left(obj, LV_PART_INDICATOR);
    lv_coord_t right_indic = lv_obj_get_style_pad_right(obj, LV_PART_INDICATOR);
    lv_coord_t top_indic = lv_obj_get_style_pad_top(obj, LV_PART_INDICATOR);
    lv_coord_t bottom_indic = lv_obj_get_style_pad_bottom(obj, LV_PART_INDICATOR);
    lv_coord_t indic_r = arc_r - LV_MAX4(left_indic, right_indic, top_indic, bottom_indic);

    if(indic_r > 0) {
        lv_draw_arc_dsc_init(&arc_dsc);
        lv_obj_init_draw_arc_dsc(obj, LV_PART_INDICATOR, &arc_dsc);

        obj_draw_dsc.part = LV_PART_INDICATOR;
        obj_draw_dsc.p1 = &center;
        obj_draw_dsc.radius = indic_r;
        obj_draw_dsc.arc_dsc = &arc_dsc;
        lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &obj_draw_dsc);

        lv_draw_arc(center.x, center.y, indic_r, arc->indic_angle_start +arc->rotation,
                   arc->indic_angle_end + arc->rotation, clip_area,
                    &arc_dsc);

        lv_event_send(obj, LV_EVENT_DRAW_PART_END, &obj_draw_dsc);
    }

    lv_area_t knob_area;
    get_knob_area(obj, &center, arc_r, &knob_area);

    lv_draw_rect_dsc_t knob_rect_dsc;
    lv_draw_rect_dsc_init(&knob_rect_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_KNOB, &knob_rect_dsc);

    obj_draw_dsc.part = LV_PART_KNOB;
    obj_draw_dsc.draw_area = &knob_area;
    obj_draw_dsc.rect_dsc = &knob_rect_dsc;
    lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &obj_draw_dsc);

    lv_draw_rect(&knob_area, clip_area, &knob_rect_dsc);

    lv_event_send(obj, LV_EVENT_DRAW_PART_END, &obj_draw_dsc);
}

static void inv_arc_area(lv_obj_t * obj, uint16_t start_angle, uint16_t end_angle, lv_part_t part)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    /*Skip this complicated invalidation if the arc is not visible*/
    if(lv_obj_is_visible(obj) == false) return;

    start_angle +=arc->rotation;
    end_angle +=arc->rotation;

    if(start_angle >= 360) start_angle -= 360;
    if(end_angle >= 360) end_angle -= 360;

    uint8_t start_quarter = start_angle / 90;
    uint8_t end_quarter = end_angle / 90;

    lv_coord_t left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    lv_coord_t top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
    lv_coord_t rout       = (LV_MIN(lv_obj_get_width(obj) - left - right, lv_obj_get_height(obj) - top - bottom)) / 2;
    lv_coord_t x       = obj->coords.x1 + rout + left;
    lv_coord_t y       = obj->coords.y1 + rout + top;
    lv_coord_t w = lv_obj_get_style_arc_width(obj, part);
    lv_coord_t rounded = lv_obj_get_style_arc_rounded(obj, part);
    lv_coord_t rin       = rout - w;
    lv_coord_t extra_area = 0;

    extra_area = rounded ? w / 2 + 2 : 0;

    if(part == LV_PART_INDICATOR) {
        lv_coord_t knob_extra_size = lv_obj_calculate_ext_draw_size(obj, LV_PART_KNOB);

        lv_coord_t knob_left = lv_obj_get_style_pad_left(obj, LV_PART_KNOB);
        lv_coord_t knob_right = lv_obj_get_style_pad_right(obj, LV_PART_KNOB);
        lv_coord_t knob_top = lv_obj_get_style_pad_top(obj, LV_PART_KNOB);
        lv_coord_t knob_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);

        knob_extra_size += LV_MAX4(knob_left, knob_right, knob_top, knob_bottom);

        extra_area = LV_MAX(extra_area, w / 2 + 2 + knob_extra_size);
    }

    lv_area_t inv_area;

    if(start_quarter == end_quarter && start_angle <= end_angle) {
        if(start_quarter == 0) {
            inv_area.y1 = y + ((lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.x2 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;

            inv_area.y2 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            inv_area.x1 = x + ((lv_trigo_sin(end_angle + 90) * rin) >> LV_TRIGO_SHIFT) - extra_area;

            lv_obj_invalidate_area(obj, &inv_area);
        }
        else if(start_quarter == 1) {
            inv_area.y2 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            inv_area.x2 = x + ((lv_trigo_sin(start_angle + 90) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            inv_area.y1 = y + ((lv_trigo_sin(end_angle) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.x1 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;

            lv_obj_invalidate_area(obj, &inv_area);
        }
        else if(start_quarter == 2) {
            inv_area.x1 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.y2 = y + ((lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            inv_area.y1 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.x2 = x + ((lv_trigo_sin(end_angle + 90) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            lv_obj_invalidate_area(obj, &inv_area);
        }
        else if(start_quarter == 3) {
            /*Small arc here*/
            inv_area.x1 = x + ((lv_trigo_sin(start_angle + 90) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.y1 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;

            inv_area.x2 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            inv_area.y2 = y + ((lv_trigo_sin(end_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            lv_obj_invalidate_area(obj, &inv_area);
        }
    }
    else if(start_quarter == 0 && end_quarter == 1) {
        inv_area.x1 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.y1 = y + ((LV_MIN(lv_trigo_sin(end_angle),
                                        lv_trigo_sin(start_angle))  * rin) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.x2 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        inv_area.y2 = y + rout + extra_area;
        lv_obj_invalidate_area(obj, &inv_area);
    }
    else if(start_quarter == 1 && end_quarter == 2) {
        inv_area.x1 = x - rout - extra_area;
        inv_area.y1 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.x2 = x + ((LV_MAX(lv_trigo_sin(start_angle + 90),
                                        lv_trigo_sin(end_angle + 90)) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        inv_area.y2 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        lv_obj_invalidate_area(obj, &inv_area);
    }
    else if(start_quarter == 2 && end_quarter == 3) {
        inv_area.x1 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.y1 = y - rout - extra_area;
        inv_area.x2 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        inv_area.y2 = y + (LV_MAX(lv_trigo_sin(end_angle) * rin,
                                       lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        lv_obj_invalidate_area(obj, &inv_area);
    }
    else if(start_quarter == 3 && end_quarter == 0) {
        inv_area.x1 = x + ((LV_MIN(lv_trigo_sin(end_angle + 90),
                                        lv_trigo_sin(start_angle + 90)) * rin) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.y1 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.x2 = x + rout + extra_area;
        inv_area.y2 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;

        lv_obj_invalidate_area(obj, &inv_area);
    }
    else {
        lv_obj_invalidate(obj);
    }
}

static void get_center(lv_obj_t * obj, lv_point_t * center, lv_coord_t * arc_r)
{
    lv_coord_t left_bg = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t right_bg = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    lv_coord_t top_bg = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t bottom_bg = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

    lv_coord_t r = (LV_MIN(lv_obj_get_width(obj) - left_bg - right_bg,
                                lv_obj_get_height(obj) - top_bg - bottom_bg)) / 2;

    *arc_r = r;
    center->x = obj->coords.x1 + r + left_bg;
    center->y = obj->coords.y1 + r + top_bg;

    lv_coord_t indic_width = lv_obj_get_style_arc_width(obj, LV_PART_INDICATOR);
    r -= indic_width;
}

static void get_knob_area(lv_obj_t * obj, const lv_point_t * center, lv_coord_t r, lv_area_t * knob_area)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    lv_coord_t indic_width = lv_obj_get_style_arc_width(obj, LV_PART_INDICATOR);
    lv_coord_t indic_width_half = indic_width / 2;
    r -= indic_width_half;

    uint16_t angle =arc->rotation;
    if(arc->type == LV_ARC_MODE_NORMAL) {
        angle +=arc->indic_angle_end;
    }
    else if(arc->type == LV_ARC_MODE_REVERSE) {
        angle +=arc->indic_angle_start;
    }
    else if(arc->type == LV_ARC_MODE_SYMMETRICAL) {
        int32_t range_midpoint = (int32_t)(arc->min_value +arc->max_value) / 2;
        if(arc->value < range_midpoint) angle +=arc->indic_angle_start;
        else angle +=arc->indic_angle_end;
    }
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
 * @param arc pointer to a arc object
 */
static void value_update(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_arc_t * arc = (lv_arc_t *)obj;

    /*If the value is still not set to any value do not update*/
    if(arc->value == VALUE_UNSET) return;

    int16_t bg_midpoint, range_midpoint, bg_end =arc->bg_angle_end;
    if(arc->bg_angle_end <arc->bg_angle_start) bg_end =arc->bg_angle_end + 360;

    int16_t angle;
    switch(arc->type) {
        case LV_ARC_MODE_SYMMETRICAL:
            bg_midpoint = (arc->bg_angle_start + bg_end) / 2;
            range_midpoint = (int32_t)(arc->min_value +arc->max_value) / 2;

            if(arc->value < range_midpoint) {
                angle = lv_map(arc->value,arc->min_value, range_midpoint,arc->bg_angle_start, bg_midpoint);
                lv_arc_set_start_angle(obj, angle);
                lv_arc_set_end_angle(obj, bg_midpoint);
            }
            else {
                angle = lv_map(arc->value, range_midpoint,arc->max_value, bg_midpoint, bg_end);
                lv_arc_set_start_angle(obj, bg_midpoint);
                lv_arc_set_end_angle(obj, angle);
            }
            break;
        case LV_ARC_MODE_REVERSE:
            angle = lv_map(arc->value,arc->min_value,arc->max_value,arc->bg_angle_start, bg_end);
            lv_arc_set_start_angle(obj, angle);
            break;
        default: /** LV_ARC_TYPE_NORMAL*/
            angle = lv_map(arc->value,arc->min_value,arc->max_value,arc->bg_angle_start, bg_end);
            lv_arc_set_end_angle(obj, angle);
            lv_arc_set_start_angle(obj,arc->bg_angle_start);
    }
   arc->last_angle = angle; /*Cache angle for slew rate limiting*/
}

#endif
