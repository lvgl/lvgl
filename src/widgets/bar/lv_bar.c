/**
 * @file lv_bar.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_bar_private.h"
#include "../../misc/lv_area_private.h"
#include "../../draw/lv_draw_mask_private.h"
#include "../../core/lv_obj_private.h"
#include "../../core/lv_obj_class_private.h"
#if LV_USE_BAR != 0

#include "../../draw/lv_draw.h"
#include "../../others/observer/lv_observer_private.h"
#include "../../misc/lv_assert.h"
#include "../../misc/lv_anim_private.h"
#include "../../misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_bar_class)

/** hor. pad and ver. pad cannot make the indicator smaller than this [px]*/
#define LV_BAR_SIZE_MIN  4

#define LV_BAR_IS_ANIMATING(anim_struct) (((anim_struct).anim_state) != LV_BAR_ANIM_STATE_INV)
#define LV_BAR_GET_ANIM_VALUE(orig_value, anim_struct) (LV_BAR_IS_ANIMATING(anim_struct) ? ((anim_struct).anim_end) : (orig_value))

/** Bar animation start value. (Not the real value of the Bar just indicates process animation)*/
#define LV_BAR_ANIM_STATE_START 0

/** Bar animation end value.  (Not the real value of the Bar just indicates process animation)*/
#define LV_BAR_ANIM_STATE_END   256

/** Mark no animation is in progress*/
#define LV_BAR_ANIM_STATE_INV   -1

/** log2(LV_BAR_ANIM_STATE_END) used to normalize data*/
#define LV_BAR_ANIM_STATE_NORM  8

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_bar_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_bar_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_bar_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_indic(lv_event_t * e);
static void lv_bar_set_value_with_anim(lv_obj_t * obj, int32_t new_value, int32_t * value_ptr,
                                       lv_bar_anim_t * anim_info, lv_anim_enable_t en);
static void lv_bar_init_anim(lv_obj_t * bar, lv_bar_anim_t * bar_anim);
static void lv_bar_anim(void * bar, int32_t value);
static void lv_bar_anim_completed(lv_anim_t * a);

#if LV_USE_OBSERVER
    static void bar_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_bar_class = {
    .constructor_cb = lv_bar_constructor,
    .destructor_cb = lv_bar_destructor,
    .event_cb = lv_bar_event,
    .width_def = LV_DPI_DEF * 2,
    .height_def = LV_DPI_DEF / 10,
    .instance_size = sizeof(lv_bar_t),
    .base_class = &lv_obj_class,
    .name = "lv_bar",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_bar_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_bar_set_value(lv_obj_t * obj, int32_t value, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_bar_t * bar = (lv_bar_t *)obj;

    if(bar->cur_value == value) return;

    value = LV_CLAMP(bar->min_value, value, bar->max_value);
    value = value < bar->start_value ? bar->start_value : value; /*Can't be smaller than the left value*/

    if(bar->cur_value == value) return;

    lv_bar_set_value_with_anim(obj, value, &bar->cur_value, &bar->cur_value_anim, anim);
}

void lv_bar_set_start_value(lv_obj_t * obj, int32_t value, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_bar_t * bar = (lv_bar_t *)obj;

    if(bar->mode != LV_BAR_MODE_RANGE) {
        return;
    }

    value = LV_CLAMP(bar->min_value, value, bar->max_value);
    value = value > bar->cur_value ? bar->cur_value : value; /*Can't be greater than the right value*/

    if(bar->start_value == value) return;

    lv_bar_set_value_with_anim(obj, value, &bar->start_value, &bar->start_value_anim, anim);
}

void lv_bar_set_range(lv_obj_t * obj, int32_t min, int32_t max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_bar_t * bar = (lv_bar_t *)obj;

    bar->val_reversed = min > max;

    int32_t real_min = bar->val_reversed ? max : min;
    int32_t real_max = bar->val_reversed ? min : max;
    if(bar->min_value == real_min && bar->max_value == real_max) return;

    bar->max_value = real_max;
    bar->min_value = real_min;

    if(lv_bar_get_mode(obj) != LV_BAR_MODE_RANGE)
        bar->start_value = real_min;

    if(bar->cur_value > real_max) {
        bar->cur_value = real_max;
        lv_bar_set_value(obj, bar->cur_value, LV_ANIM_OFF);
    }
    if(bar->cur_value < real_min) {
        bar->cur_value = real_min;
        lv_bar_set_value(obj, bar->cur_value, LV_ANIM_OFF);
    }

    lv_obj_invalidate(obj);
}

void lv_bar_set_min_value(lv_obj_t * obj, int32_t min)
{
    lv_bar_set_range(obj, min, lv_bar_get_max_value(obj));
}

void lv_bar_set_max_value(lv_obj_t * obj, int32_t max)
{
    lv_bar_set_range(obj, lv_bar_get_min_value(obj), max);
}

void lv_bar_set_mode(lv_obj_t * obj, lv_bar_mode_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_bar_t * bar = (lv_bar_t *)obj;

    bar->mode = mode;
    if(bar->mode != LV_BAR_MODE_RANGE) {
        bar->start_value = bar->min_value;
    }

    lv_obj_invalidate(obj);
}

void lv_bar_set_orientation(lv_obj_t * obj, lv_bar_orientation_t orientation)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_bar_t * bar = (lv_bar_t *)obj;

    bar->orientation = orientation;
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

int32_t lv_bar_get_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_bar_t * bar = (lv_bar_t *)obj;

    return LV_BAR_GET_ANIM_VALUE(bar->cur_value, bar->cur_value_anim);
}

int32_t lv_bar_get_start_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_bar_t * bar = (lv_bar_t *)obj;

    if(bar->mode != LV_BAR_MODE_RANGE) return bar->min_value;

    return LV_BAR_GET_ANIM_VALUE(bar->start_value, bar->start_value_anim);
}

int32_t lv_bar_get_min_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_bar_t * bar = (lv_bar_t *)obj;
    return bar->val_reversed ? bar->max_value : bar->min_value;
}

int32_t lv_bar_get_max_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_bar_t * bar = (lv_bar_t *)obj;

    return bar->val_reversed ? bar->min_value : bar->max_value;
}

lv_bar_mode_t lv_bar_get_mode(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_bar_t * bar = (lv_bar_t *)obj;

    return bar->mode;
}

lv_bar_orientation_t lv_bar_get_orientation(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_bar_t * bar = (lv_bar_t *)obj;

    return bar->orientation;
}

bool lv_bar_is_symmetrical(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_bar_t * bar = (lv_bar_t *)obj;

    return  bar->mode == LV_BAR_MODE_SYMMETRICAL && bar->min_value < 0 && bar->max_value > 0 &&
            bar->start_value == bar->min_value;
}

#if LV_USE_OBSERVER
lv_observer_t * lv_bar_bind_value(lv_obj_t * obj, lv_subject_t * subject)
{
    LV_ASSERT_NULL(subject);
    LV_ASSERT_NULL(obj);

    if(subject->type != LV_SUBJECT_TYPE_INT && subject->type != LV_SUBJECT_TYPE_FLOAT) {
        LV_LOG_WARN("Incompatible subject type: %d", subject->type);
        return NULL;
    }

    lv_observer_t * observer = lv_subject_add_observer_obj(subject, bar_value_observer_cb, obj, NULL);
    return observer;
}
#endif /*LV_USE_OBSERVER*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_bar_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_bar_t * bar = (lv_bar_t *)obj;
    bar->min_value = 0;
    bar->max_value = 100;
    bar->start_value = 0;
    bar->cur_value = 0;
    bar->indic_area.x1 = 0;
    bar->indic_area.x2 = 0;
    bar->indic_area.y1 = 0;
    bar->indic_area.y2 = 0;
    bar->mode = LV_BAR_MODE_NORMAL;
    bar->orientation = LV_BAR_ORIENTATION_AUTO;
    bar->val_reversed = false;

    lv_bar_init_anim(obj, &bar->cur_value_anim);
    lv_bar_init_anim(obj, &bar->start_value_anim);

    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_bar_set_value(obj, 0, LV_ANIM_OFF);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_bar_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_bar_t * bar = (lv_bar_t *)obj;

    lv_anim_delete(&bar->cur_value_anim, NULL);
    lv_anim_delete(&bar->start_value_anim, NULL);
}

static void draw_indic(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_bar_t * bar = (lv_bar_t *)obj;

    lv_layer_t * layer = lv_event_get_layer(e);

    lv_area_t bar_coords;
    lv_obj_get_coords(obj, &bar_coords);

    int32_t transf_w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
    int32_t transf_h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
    lv_area_increase(&bar_coords, transf_w, transf_h);
    int32_t barw = lv_area_get_width(&bar_coords);
    int32_t barh = lv_area_get_height(&bar_coords);
    int32_t range = bar->max_value - bar->min_value;

    /*Prevent division by 0*/
    if(range == 0) {
        range = 1;
    }

    bool hor = false;
    switch(bar->orientation) {
        case LV_BAR_ORIENTATION_HORIZONTAL:
            hor = true;
            break;
        case LV_BAR_ORIENTATION_VERTICAL:
            hor = false;
            break;
        case LV_BAR_ORIENTATION_AUTO:
        default:
            hor = (barw >= barh);
            break;
    }

    bool sym = lv_bar_is_symmetrical(obj);

    /*Calculate the indicator area*/
    int32_t bg_left = lv_obj_get_style_pad_left(obj,     LV_PART_MAIN);
    int32_t bg_right = lv_obj_get_style_pad_right(obj,   LV_PART_MAIN);
    int32_t bg_top = lv_obj_get_style_pad_top(obj,       LV_PART_MAIN);
    int32_t bg_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

    /*Respect padding and minimum width/height too*/
    lv_area_copy(&bar->indic_area, &bar_coords);
    bar->indic_area.x1 += bg_left;
    bar->indic_area.x2 -= bg_right;
    bar->indic_area.y1 += bg_top;
    bar->indic_area.y2 -= bg_bottom;

    if(hor && lv_area_get_height(&bar->indic_area) < LV_BAR_SIZE_MIN) {
        bar->indic_area.y1 = obj->coords.y1 + (barh / 2) - (LV_BAR_SIZE_MIN / 2);
        bar->indic_area.y2 = bar->indic_area.y1 + LV_BAR_SIZE_MIN;
    }
    else if(!hor && lv_area_get_width(&bar->indic_area) < LV_BAR_SIZE_MIN) {
        bar->indic_area.x1 = obj->coords.x1 + (barw / 2) - (LV_BAR_SIZE_MIN / 2);
        bar->indic_area.x2 = bar->indic_area.x1 + LV_BAR_SIZE_MIN;
    }
    int32_t indic_max_w = lv_area_get_width(&bar->indic_area);
    int32_t indic_max_h = lv_area_get_height(&bar->indic_area);

    /*Calculate the indicator length*/
    int32_t anim_length = hor ? indic_max_w : indic_max_h;

    int32_t anim_cur_value_x, anim_start_value_x;

    int32_t * axis1, * axis2;
    int32_t (*indic_length_calc)(const lv_area_t * area);

    if(hor) {
        axis1 = &bar->indic_area.x1;
        axis2 = &bar->indic_area.x2;
        indic_length_calc = lv_area_get_width;
    }
    else {
        axis1 = &bar->indic_area.y1;
        axis2 = &bar->indic_area.y2;
        indic_length_calc = lv_area_get_height;
    }

    if(LV_BAR_IS_ANIMATING(bar->start_value_anim)) {
        int32_t anim_start_value_start_x =
            (int32_t)((int32_t)anim_length * (bar->start_value_anim.anim_start - bar->min_value)) / range;
        int32_t anim_start_value_end_x =
            (int32_t)((int32_t)anim_length * (bar->start_value_anim.anim_end - bar->min_value)) / range;

        anim_start_value_x = (((anim_start_value_end_x - anim_start_value_start_x) * bar->start_value_anim.anim_state) /
                              LV_BAR_ANIM_STATE_END);

        anim_start_value_x += anim_start_value_start_x;
    }
    else {
        anim_start_value_x = (int32_t)((int32_t)anim_length * (bar->start_value - bar->min_value)) / range;
    }

    if(LV_BAR_IS_ANIMATING(bar->cur_value_anim)) {
        int32_t anim_cur_value_start_x =
            (int32_t)((int32_t)anim_length * (bar->cur_value_anim.anim_start - bar->min_value)) / range;
        int32_t anim_cur_value_end_x =
            (int32_t)((int32_t)anim_length * (bar->cur_value_anim.anim_end - bar->min_value)) / range;

        anim_cur_value_x = anim_cur_value_start_x + (((anim_cur_value_end_x - anim_cur_value_start_x) *
                                                      bar->cur_value_anim.anim_state) /
                                                     LV_BAR_ANIM_STATE_END);
    }
    else {
        anim_cur_value_x = (int32_t)((int32_t)anim_length * (bar->cur_value - bar->min_value)) / range;
    }

    /**
     * The drawing direction of the bar can be reversed only when one of the two conditions(value inversion
     * or horizontal direction base dir is LV_BASE_DIR_RTL) is met.
    */
    lv_base_dir_t base_dir = lv_obj_get_style_base_dir(obj, LV_PART_MAIN);
    bool hor_need_reversed = hor && base_dir == LV_BASE_DIR_RTL;
    bool reversed = bar->val_reversed ^ hor_need_reversed;

    /* An area with width 0 is {x1 = 0 x2 = -1} so subtracting 1 from `anim_cur_value_x` causes...
     *     anim_start_value_x = 0   anim_cur_value_x = 0   to be {x1 = 0 x2 = -1  } which is width 0
     *     anim_start_value_x = 0   anim_cur_value_x = 300 to be {x1 = 0 x2 =  299} which is width 300
     */
    anim_cur_value_x -= 1;

    if(reversed) {
        /*Swap axes*/
        int32_t * tmp;
        tmp = axis1;
        axis1 = axis2;
        axis2 = tmp;
        anim_cur_value_x = -anim_cur_value_x;
        anim_start_value_x = -anim_start_value_x;
    }

    /*Set the indicator length*/
    if(hor) {
        *axis2 = *axis1 + anim_cur_value_x;
        *axis1 += anim_start_value_x;
    }
    else {
        *axis1 = *axis2 - anim_cur_value_x;
        *axis2 -= anim_start_value_x;
    }

    if(sym) {
        int32_t zero, shift;
        shift = (-bar->min_value * anim_length) / range;

        if(hor) {
            int32_t * left = reversed ? axis2 : axis1;
            int32_t * right = reversed ? axis1 : axis2;
            if(reversed)
                zero = *axis1 - shift + 1;
            else
                zero = *axis1 + shift;

            if(*axis2 > zero) {
                *right = *axis2;
                *left = zero;
            }
            else {
                *left = *axis2;
                *right = zero;
            }
        }
        else {
            int32_t * top = reversed ? axis2 : axis1;
            int32_t * bottom = reversed ? axis1 : axis2;
            if(reversed)
                zero = *axis2 + shift;
            else
                zero = *axis2 - shift + 1;

            if(*axis1 > zero) {
                *bottom = *axis1;
                *top = zero;
            }
            else {
                *top = *axis1;
                *bottom = zero;
            }
        }
    }

    /*Do not draw a zero length indicator but at least call the draw task event*/
    if(!sym && indic_length_calc(&bar->indic_area) <= 1) {
        lv_obj_send_event(obj, LV_EVENT_DRAW_TASK_ADDED, NULL);
        return;
    }

    lv_area_t indic_area;
    lv_area_copy(&indic_area, &bar->indic_area);

    lv_draw_rect_dsc_t draw_rect_dsc;
    lv_draw_rect_dsc_init(&draw_rect_dsc);
    draw_rect_dsc.base.layer = layer;
    lv_obj_init_draw_rect_dsc(obj, LV_PART_INDICATOR, &draw_rect_dsc);

    int32_t bg_radius = lv_obj_get_style_radius(obj, LV_PART_MAIN);
    int32_t short_side = LV_MIN(barw, barh);
    if(bg_radius > short_side >> 1) bg_radius = short_side >> 1;

    int32_t indic_radius = draw_rect_dsc.radius;
    short_side = LV_MIN(lv_area_get_width(&bar->indic_area), lv_area_get_height(&bar->indic_area));
    if(indic_radius > short_side >> 1) indic_radius = short_side >> 1;

    /*Cases:
     * Simple:
     *   - indicator area is the same or smaller then the bg
     *   - indicator has the same or larger radius than the bg
     *   - what to do? just draw the indicator
     * Radius issue:
     *   - indicator area is the same or smaller then bg
     *   - indicator has smaller radius than the bg and the indicator overflows on the corners
     *   - what to do? draw the indicator on a layer and clip to bg radius
     * Larger indicator:
     *   - indicator area is the larger then the bg
     *   - radius doesn't matter
     *   - shadow doesn't matter
     *   - what to do? just draw the indicator
     * Shadow:
     *   - indicator area is the same or smaller then the bg
     *   - indicator has the same or larger radius than the bg (shadow needs to be drawn on strange clipped shape)
     *   - what to do? don't draw the shadow if the indicator is too small has strange shape
     * Gradient:
     *   - the indicator has a gradient
     *   - what to do? draw it on a bg sized layer clip the indicator are from the gradient
     *
     */

    bool mask_needed = false;
    if(hor && draw_rect_dsc.bg_grad.dir == LV_GRAD_DIR_HOR) mask_needed = true;
    else if(!hor && draw_rect_dsc.bg_grad.dir == LV_GRAD_DIR_VER) mask_needed = true;

    if(draw_rect_dsc.bg_image_src) mask_needed = true;

    bool radius_issue = true;
    /*The indicator is fully drawn if it's larger than the bg*/
    if((bg_left < 0 || bg_right < 0 || bg_top < 0 || bg_bottom < 0)) radius_issue = false;
    else if(indic_radius >= bg_radius) radius_issue = false;
    else if(lv_area_is_in(&indic_area, &bar_coords, bg_radius)) radius_issue = false;

    if(radius_issue || mask_needed) {
        if(!radius_issue) {
            /*Draw only the shadow*/
            lv_draw_rect_dsc_t draw_tmp_dsc = draw_rect_dsc;
            draw_tmp_dsc.border_opa = 0;
            draw_tmp_dsc.outline_opa = 0;
            draw_tmp_dsc.bg_opa = 0;
            draw_tmp_dsc.bg_image_opa = 0;
            lv_draw_rect(layer, &draw_tmp_dsc, &indic_area);
        }
        else {
            draw_rect_dsc.border_opa = 0;
            draw_rect_dsc.outline_opa = 0;
        }
        draw_rect_dsc.shadow_opa = 0;

        /*If clipped for any reason cannot the border, outline, and shadow
         *as they would be clipped and looked ugly*/
        lv_draw_rect_dsc_t draw_tmp_dsc = draw_rect_dsc;
        draw_tmp_dsc.border_opa = 0;
        draw_tmp_dsc.outline_opa = 0;
        draw_tmp_dsc.shadow_opa = 0;
        lv_area_t indic_draw_area = indic_area;
        if(mask_needed) {
            if(hor) {
                indic_draw_area.x1 = bar_coords.x1 + bg_left;
                indic_draw_area.x2 = bar_coords.x2 - bg_right;
            }
            else {
                indic_draw_area.y1 = bar_coords.y1 + bg_top;
                indic_draw_area.y2 = bar_coords.y2 - bg_bottom;
            }
            draw_tmp_dsc.radius = 0;
        }

        lv_layer_t * layer_indic = lv_draw_layer_create(layer, LV_COLOR_FORMAT_ARGB8888, &indic_draw_area);

        lv_draw_rect(layer_indic, &draw_tmp_dsc, &indic_draw_area);

        lv_draw_mask_rect_dsc_t mask_dsc;
        lv_draw_mask_rect_dsc_init(&mask_dsc);
        if(radius_issue) {
            mask_dsc.area = bar_coords;
            mask_dsc.radius = bg_radius;
            lv_draw_mask_rect(layer_indic, &mask_dsc);
        }

        if(mask_needed) {
            mask_dsc.area = indic_area;
            mask_dsc.radius = indic_radius;
            lv_draw_mask_rect(layer_indic, &mask_dsc);
        }

        lv_draw_image_dsc_t layer_draw_dsc;
        lv_draw_image_dsc_init(&layer_draw_dsc);
        layer_draw_dsc.src = layer_indic;
        lv_draw_layer(layer, &layer_draw_dsc, &indic_draw_area);

        /*Add the border, outline, and shadow only to the indicator area.
         *They might have disabled if there is a radius_issue*/
        draw_tmp_dsc = draw_rect_dsc;
        draw_tmp_dsc.bg_opa = 0;
        draw_tmp_dsc.bg_image_opa = 0;
        lv_draw_rect(layer, &draw_tmp_dsc, &indic_area);
    }
    else {
        lv_draw_rect(layer, &draw_rect_dsc, &indic_area);
    }
}

static void lv_bar_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);

    if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        int32_t indic_size;
        indic_size = lv_obj_calculate_ext_draw_size(obj, LV_PART_INDICATOR);

        /*Bg size is handled by lv_obj*/
        int32_t * s = lv_event_get_param(e);
        *s = LV_MAX(*s, indic_size);

        /*Calculate the indicator area*/
        int32_t bg_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
        int32_t bg_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
        int32_t bg_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
        int32_t bg_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

        int32_t pad = LV_MIN4(bg_left, bg_right, bg_top, bg_bottom);
        if(pad < 0) {
            *s = *s - pad;
        }
    }
    else if(code == LV_EVENT_PRESSED || code == LV_EVENT_RELEASED) {
        lv_bar_t * bar = (lv_bar_t *)obj;
        lv_obj_invalidate_area(obj, &bar->indic_area);
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        draw_indic(e);
    }
}

static void lv_bar_anim(void * var, int32_t value)
{
    lv_bar_anim_t * bar_anim = var;
    bar_anim->anim_state    = value;
    lv_obj_invalidate(bar_anim->bar);
}

static void lv_bar_anim_completed(lv_anim_t * a)
{
    lv_bar_anim_t * var = a->var;
    lv_obj_t * obj = (lv_obj_t *)var->bar;
    lv_bar_t * bar = (lv_bar_t *)obj;

    var->anim_state = LV_BAR_ANIM_STATE_INV;
    if(var == &bar->cur_value_anim)
        bar->cur_value = var->anim_end;
    else if(var == &bar->start_value_anim)
        bar->start_value = var->anim_end;
    lv_obj_invalidate(var->bar);
}

static void lv_bar_set_value_with_anim(lv_obj_t * obj, int32_t new_value, int32_t * value_ptr,
                                       lv_bar_anim_t * anim_info, lv_anim_enable_t en)
{
    if(en == LV_ANIM_OFF) {
        lv_anim_delete(anim_info, NULL);
        anim_info->anim_state = LV_BAR_ANIM_STATE_INV;
        *value_ptr = new_value;
        lv_obj_invalidate((lv_obj_t *)obj);

        /*Stop the previous animation if it exists*/
        lv_anim_delete(anim_info, NULL);
        /*Reset animation state*/
        lv_bar_init_anim(obj, anim_info);
    }
    else {
        /*No animation in progress -> simply set the values*/
        if(anim_info->anim_state == LV_BAR_ANIM_STATE_INV) {
            anim_info->anim_start = *value_ptr;
            anim_info->anim_end   = new_value;
        }
        /*Animation in progress. Start from the animation end value*/
        else {
            anim_info->anim_start = anim_info->anim_end;
            anim_info->anim_end   = new_value;
        }
        *value_ptr = new_value;
        /*Stop the previous animation if it exists*/
        lv_anim_delete(anim_info, NULL);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, anim_info);
        lv_anim_set_exec_cb(&a, lv_bar_anim);
        lv_anim_set_values(&a, LV_BAR_ANIM_STATE_START, LV_BAR_ANIM_STATE_END);
        lv_anim_set_completed_cb(&a, lv_bar_anim_completed);
        lv_anim_set_duration(&a, lv_obj_get_style_anim_duration(obj, LV_PART_MAIN));
        lv_anim_start(&a);
    }
}

static void lv_bar_init_anim(lv_obj_t * obj, lv_bar_anim_t * bar_anim)
{
    bar_anim->bar = obj;
    bar_anim->anim_start = 0;
    bar_anim->anim_end = 0;
    bar_anim->anim_state = LV_BAR_ANIM_STATE_INV;
}

#if LV_USE_OBSERVER

static void bar_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    if(subject->type == LV_SUBJECT_TYPE_INT) {
        lv_bar_set_value(observer->target, subject->value.num, LV_ANIM_OFF);
    }
#if LV_USE_FLOAT
    else {
        lv_bar_set_value(observer->target, (int32_t)subject->value.float_v, LV_ANIM_OFF);
    }
#endif
}

#endif /*LV_USE_OBSERVER*/

#endif
