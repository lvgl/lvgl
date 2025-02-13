/**
 * @file lv_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_switch_private.h"
#include "../../core/lv_obj_private.h"
#include "../../core/lv_obj_class_private.h"

#if LV_USE_SWITCH != 0

#include "../../misc/lv_assert.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_anim_private.h"
#include "../../indev/lv_indev.h"
#include "../../display/lv_display.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_switch_class)

#define LV_SWITCH_IS_ANIMATING(sw) (((sw)->anim_state) != LV_SWITCH_ANIM_STATE_INV)

/** Switch animation start value. (Not the real value of the switch just indicates process animation)*/
#define LV_SWITCH_ANIM_STATE_START 0

/** Switch animation end value.  (Not the real value of the switch just indicates process animation)*/
#define LV_SWITCH_ANIM_STATE_END   256

/** Mark no animation is in progress*/
#define LV_SWITCH_ANIM_STATE_INV   -1

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_switch_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_switch_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_switch_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_main(lv_event_t * e);

static void lv_switch_anim_exec_cb(void * sw, int32_t value);
static void lv_switch_trigger_anim(lv_obj_t * obj);
static void lv_switch_anim_completed(lv_anim_t * a);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_switch_class = {
    .constructor_cb = lv_switch_constructor,
    .destructor_cb = lv_switch_destructor,
    .event_cb = lv_switch_event,
    .width_def = (4 * LV_DPI_DEF) / 10,
    .height_def = (4 * LV_DPI_DEF) / 17,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .instance_size = sizeof(lv_switch_t),
    .base_class = &lv_obj_class,
    .name = "lv_switch",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_switch_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_switch_set_orientation(lv_obj_t * obj, lv_switch_orientation_t orientation)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_switch_t * sw = (lv_switch_t *)obj;

    sw->orientation = orientation;
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

lv_switch_orientation_t lv_switch_get_orientation(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_switch_t * sw = (lv_switch_t *)obj;

    return sw->orientation;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_switch_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_switch_t * sw = (lv_switch_t *)obj;

    sw->anim_state = LV_SWITCH_ANIM_STATE_INV;
    sw->orientation = LV_SWITCH_ORIENTATION_AUTO;

    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_switch_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_switch_t * sw = (lv_switch_t *)obj;

    lv_anim_delete(sw, NULL);
}

static void lv_switch_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);

    if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        int32_t knob_left = lv_obj_get_style_pad_left(obj,   LV_PART_KNOB);
        int32_t knob_right = lv_obj_get_style_pad_right(obj,  LV_PART_KNOB);
        int32_t knob_top = lv_obj_get_style_pad_top(obj,    LV_PART_KNOB);
        int32_t knob_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);

        /*The smaller size is the knob diameter*/
        int32_t knob_size = LV_MAX4(knob_left, knob_right, knob_bottom, knob_top);
        knob_size += LV_SWITCH_KNOB_EXT_AREA_CORRECTION;
        knob_size += lv_obj_calculate_ext_draw_size(obj, LV_PART_KNOB);

        int32_t * s = lv_event_get_param(e);
        *s = LV_MAX(*s, knob_size);
        *s = LV_MAX(*s, lv_obj_calculate_ext_draw_size(obj, LV_PART_INDICATOR));
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        lv_switch_trigger_anim(obj);
        lv_obj_invalidate(obj);
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        draw_main(e);
    }
}

static void draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_switch_t * sw = (lv_switch_t *)obj;

    lv_layer_t * layer = lv_event_get_layer(e);

    /*Draw the indicator*/
    lv_area_t indic_area;
    /*Exclude background's padding*/
    lv_obj_get_content_coords(obj, &indic_area);

    lv_draw_rect_dsc_t draw_indic_dsc;
    lv_draw_rect_dsc_init(&draw_indic_dsc);
    draw_indic_dsc.base.layer = layer;
    lv_obj_init_draw_rect_dsc(obj, LV_PART_INDICATOR, &draw_indic_dsc);
    lv_draw_rect(layer, &draw_indic_dsc, &indic_area);

    /*Draw the knob*/
    lv_area_t knob_area;
    lv_area_copy(&knob_area, &obj->coords);

    int32_t switch_w = lv_area_get_width(&obj->coords);
    int32_t switch_h = lv_area_get_height(&obj->coords);
    bool hor = false;

    switch(sw->orientation) {
        case LV_SWITCH_ORIENTATION_HORIZONTAL:
            hor = true;
            break;
        case LV_SWITCH_ORIENTATION_VERTICAL:
            hor = false;
            break;
        case LV_SWITCH_ORIENTATION_AUTO:
        default:
            hor = (switch_w >= switch_h);
            break;
    }

    if(hor) {
        int32_t anim_value_x = 0;
        int32_t knob_size = lv_obj_get_height(obj);
        int32_t anim_length = lv_area_get_width(&obj->coords) - knob_size;
        if(LV_SWITCH_IS_ANIMATING(sw)) {
            /* Use the animation's coordinate */
            anim_value_x = (anim_length * sw->anim_state) / LV_SWITCH_ANIM_STATE_END;
        }
        else {
            /* Use LV_STATE_CHECKED to decide the coordinate */
            bool chk = lv_obj_get_state(obj) & LV_STATE_CHECKED;
            anim_value_x = chk ? anim_length : 0;
        }

        if(LV_BASE_DIR_RTL == lv_obj_get_style_base_dir(obj, LV_PART_MAIN)) {
            anim_value_x = anim_length - anim_value_x;
        }
        knob_area.x1 += anim_value_x;
        knob_area.x2 = knob_area.x1 + (knob_size > 0 ? knob_size - 1 : 0);
    }
    else {
        int32_t anim_value_y = 0;
        int32_t knob_size = lv_obj_get_width(obj);
        int32_t anim_length = lv_area_get_height(&obj->coords) - knob_size;
        if(LV_SWITCH_IS_ANIMATING(sw)) {
            /* Use the animation's coordinate */
            anim_value_y = (anim_length * sw->anim_state) / LV_SWITCH_ANIM_STATE_END;
        }
        else {
            /* Use LV_STATE_CHECKED to decide the coordinate */
            bool chk = lv_obj_get_state(obj) & LV_STATE_CHECKED;
            anim_value_y = chk ? anim_length : 0;
        }

        if(LV_BASE_DIR_RTL == lv_obj_get_style_base_dir(obj, LV_PART_MAIN)) {
            anim_value_y = anim_length - anim_value_y;
        }

        knob_area.y2 -= anim_value_y;
        knob_area.y1 = knob_area.y2 - (knob_size > 0 ? knob_size - 1 : 0);
    }

    int32_t knob_left = lv_obj_get_style_pad_left(obj, LV_PART_KNOB);
    int32_t knob_right = lv_obj_get_style_pad_right(obj, LV_PART_KNOB);
    int32_t knob_top = lv_obj_get_style_pad_top(obj, LV_PART_KNOB);
    int32_t knob_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);

    /*Apply the paddings on the knob area*/
    knob_area.x1 -= knob_left;
    knob_area.x2 += knob_right;
    knob_area.y1 -= knob_top;
    knob_area.y2 += knob_bottom;

    lv_draw_rect_dsc_t knob_rect_dsc;
    lv_draw_rect_dsc_init(&knob_rect_dsc);
    knob_rect_dsc.base.layer = layer;
    lv_obj_init_draw_rect_dsc(obj, LV_PART_KNOB, &knob_rect_dsc);

    lv_draw_rect(layer, &knob_rect_dsc, &knob_area);
}

static void lv_switch_anim_exec_cb(void * var, int32_t value)
{
    lv_switch_t * sw = var;
    sw->anim_state = value;
    lv_obj_invalidate((lv_obj_t *)sw);
}

/**
 * Resets the switch's animation state to "no animation in progress".
 */
static void lv_switch_anim_completed(lv_anim_t * a)
{
    lv_switch_t * sw = a->var;
    sw->anim_state = LV_SWITCH_ANIM_STATE_INV;
    lv_obj_invalidate((lv_obj_t *)sw);
}

/**
 * Starts an animation for the switch knob. if the anim_time style property is greater than 0
 * @param obj the switch to animate
 */
static void lv_switch_trigger_anim(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_switch_t * sw = (lv_switch_t *)obj;

    uint32_t anim_dur_full = lv_obj_get_style_anim_duration(obj, LV_PART_MAIN);

    if(anim_dur_full > 0) {
        bool chk = lv_obj_get_state(obj) & LV_STATE_CHECKED;
        int32_t anim_start;
        int32_t anim_end;
        /*No animation in progress -> simply set the values*/
        if(sw->anim_state == LV_SWITCH_ANIM_STATE_INV) {
            anim_start = chk ? LV_SWITCH_ANIM_STATE_START : LV_SWITCH_ANIM_STATE_END;
            anim_end   = chk ? LV_SWITCH_ANIM_STATE_END : LV_SWITCH_ANIM_STATE_START;
        }
        /*Animation in progress. Start from the animation end value*/
        else {
            anim_start = sw->anim_state;
            anim_end   = chk ? LV_SWITCH_ANIM_STATE_END : LV_SWITCH_ANIM_STATE_START;
        }
        /*Calculate actual animation duration*/
        uint32_t anim_dur = (anim_dur_full * LV_ABS(anim_start - anim_end)) / LV_SWITCH_ANIM_STATE_END;

        /*Stop the previous animation if it exists*/
        lv_anim_delete(sw, NULL);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, sw);
        lv_anim_set_exec_cb(&a, lv_switch_anim_exec_cb);
        lv_anim_set_values(&a, anim_start, anim_end);
        lv_anim_set_completed_cb(&a, lv_switch_anim_completed);
        lv_anim_set_duration(&a, anim_dur);
        lv_anim_start(&a);
    }
}

#endif
