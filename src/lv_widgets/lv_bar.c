/**
 * @file lv_bar.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_bar.h"
#if LV_USE_BAR != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_math.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_bar"

#define LV_BAR_SIZE_MIN  4   /*hor. pad and ver. pad cannot make the indicator smaller then this [px]*/

#define LV_BAR_IS_ANIMATING(anim_struct) (((anim_struct).anim_state) != LV_BAR_ANIM_STATE_INV)
#define LV_BAR_GET_ANIM_VALUE(orig_value, anim_struct) (LV_BAR_IS_ANIMATING(anim_struct) ? ((anim_struct).anim_end) : (orig_value))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_bar_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_bar_destructor(lv_obj_t * obj);
static lv_draw_res_t lv_bar_draw(lv_obj_t * bar, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_bar_signal(lv_obj_t * bar, lv_signal_t sign, void * param);
static void draw_indic(lv_obj_t * bar, const lv_area_t * clip_area);

static void lv_bar_set_value_with_anim(lv_obj_t * obj, int16_t new_value, int16_t * value_ptr,
                                       lv_bar_anim_t * anim_info, lv_anim_enable_t en);
static void lv_bar_init_anim(lv_obj_t * bar, lv_bar_anim_t * bar_anim);
static void lv_bar_anim(lv_bar_anim_t * bar, lv_anim_value_t value);
static void lv_bar_anim_ready(lv_anim_t * a);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_bar = {
    .constructor = lv_bar_constructor,
    .destructor = lv_bar_destructor,
    .signal_cb = lv_bar_signal,
    .draw_cb = lv_bar_draw,
    .instance_size = sizeof(lv_bar_t),
    .base_class = &lv_obj
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * Create a bar objects
 * @param par pointer to an object, it will be the parent of the new bar
 * @param copy DEPRECATED, will be removed in v9.
 *             Pointer to an other bar to copy.
 * @return pointer to the created bar
 */
lv_obj_t * lv_bar_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    return lv_obj_create_from_class(&lv_bar, parent, copy);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the bar
 * @param bar pointer to a bar object
 * @param value new value
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_bar_set_value(lv_obj_t * obj, int16_t value, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_bar_t * bar = (lv_bar_t *)obj;

    if(bar->cur_value == value) return;

    value = LV_CLAMP(bar->min_value, value, bar->max_value);
    value = value < bar->start_value ? bar->start_value : value; /*Can be smaller then the left value*/

    if(bar->cur_value == value) return;
    lv_bar_set_value_with_anim(obj, value, &bar->cur_value, &bar->cur_value_anim, anim);
}

/**
 * Set a new start value on the bar
 * @param bar pointer to a bar object
 * @param value new start value
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_bar_set_start_value(lv_obj_t * obj, int16_t start_value, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_bar_t * bar = (lv_bar_t *)obj;

    int16_t new_value = start_value;
    new_value = new_value > bar->max_value ? bar->max_value : new_value;
    new_value = new_value < bar->min_value ? bar->min_value : new_value;
    new_value = new_value > bar->cur_value ? bar->cur_value : new_value;

    if(bar->start_value == new_value) return;
    lv_bar_set_value_with_anim(obj, new_value, &bar->start_value, &bar->start_value_anim, anim);
}

/**
 * Set minimum and the maximum values of a bar
 * @param bar pointer to the bar object
 * @param min minimum value
 * @param max maximum value
 */
void lv_bar_set_range(lv_obj_t * obj, int16_t min, int16_t max)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_bar_t * bar = (lv_bar_t *)obj;

    if(bar->min_value == min && bar->max_value == max) return;

    bar->max_value = max;
    bar->min_value = min;

    if(lv_bar_get_type(obj) != LV_BAR_TYPE_CUSTOM)
        bar->start_value = min;

    if(bar->cur_value > max) {
        bar->cur_value = max;
        lv_bar_set_value(obj, bar->cur_value, false);
    }
    if(bar->cur_value < min) {
        bar->cur_value = min;
        lv_bar_set_value(obj, bar->cur_value, false);
    }
    lv_obj_invalidate(obj);
}

/**
 * Set the type of bar.
 * @param bar pointer to bar object
 * @param type bar type
 */
void lv_bar_set_type(lv_obj_t * obj, lv_bar_type_t type)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_bar_t * bar = (lv_bar_t *)obj;

    bar->type = type;
    if(bar->type != LV_BAR_TYPE_CUSTOM)
        bar->start_value = bar->min_value;

    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a bar
 * @param bar pointer to a bar object
 * @return the value of the bar
 */
int16_t lv_bar_get_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_bar_t * bar = (lv_bar_t *)obj;

    return LV_BAR_GET_ANIM_VALUE(bar->cur_value, bar->cur_value_anim);
}

/**
 * Get the start value of a bar
 * @param bar pointer to a bar object
 * @return the start value of the bar
 */
int16_t lv_bar_get_start_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_bar_t * bar = (lv_bar_t *)obj;

    if(bar->type != LV_BAR_TYPE_CUSTOM) return bar->min_value;

    return LV_BAR_GET_ANIM_VALUE(bar->start_value, bar->start_value_anim);
}

/**
 * Get the minimum value of a bar
 * @param bar pointer to a bar object
 * @return the minimum value of the bar
 */
int16_t lv_bar_get_min_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_bar_t * bar = (lv_bar_t *)obj;
    return bar->min_value;
}

/**
 * Get the maximum value of a bar
 * @param bar pointer to a bar object
 * @return the maximum value of the bar
 */
int16_t lv_bar_get_max_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_bar_t * bar = (lv_bar_t *)obj;

    return bar->max_value;
}

/**
 * Get the type of bar.
 * @param bar pointer to bar object
 * @return bar type
 */
lv_bar_type_t lv_bar_get_type(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_bar_t * bar = (lv_bar_t *)obj;

    return bar->type;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_bar_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_TRACE("lv_bar create started");

    lv_obj_construct_base(obj, parent, copy);

    lv_bar_t * bar = (lv_bar_t *)obj;
    bar->min_value = 0;
    bar->max_value = 100;
    bar->start_value = 0;
    bar->cur_value = 0;
    bar->type         = LV_BAR_TYPE_NORMAL;

    lv_bar_init_anim(obj, &bar->cur_value_anim);
    lv_bar_init_anim(obj, &bar->start_value_anim);

   if(copy == NULL) {
       lv_obj_clear_flag(obj, LV_OBJ_FLAG_CHECKABLE);
       lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
       lv_obj_set_size(obj, LV_DPI * 2, LV_DPI / 10);
       lv_bar_set_value(obj, 0, LV_ANIM_OFF);
   } else {
       lv_bar_t * copy_bar = (lv_bar_t *)copy;
       bar->min_value   = copy_bar->min_value;
       bar->start_value = copy_bar->start_value;
       bar->max_value   = copy_bar->max_value;
       bar->cur_value   = copy_bar->cur_value;
       bar->type        = copy_bar->type;

       lv_bar_set_value(obj, bar->cur_value, LV_ANIM_OFF);
   }
   LV_LOG_INFO("bar created");
}

static void lv_bar_destructor(lv_obj_t * obj)
{
//    lv_bar_t * bar = obj;
//
//    _lv_obj_reset_style_list_no_refr(obj, LV_PART_INDICATOR);
//#if LV_USE_ANIMATION
//    lv_anim_del(&bar->cur_value_anim, NULL);
//    lv_anim_del(&bar->start_value_anim, NULL);
//#endif

//    bar->class_p->base_p->destructor(obj);
}

/**
 * Handle the drawing related tasks of the bars
 * @param bar pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DRAW_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DRAW_DRAW: draw the object (always return 'true')
 *             LV_DRAW_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_draw_res_t`
 */
static lv_draw_res_t lv_bar_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    if(mode == LV_DRAW_MODE_COVER_CHECK) {
        /*Return false if the object is not covers the mask area*/
        return lv_obj.draw_cb(obj, clip_area, mode);
    }
    else if(mode == LV_DRAW_MODE_MAIN_DRAW) {
        /*Draw the background*/
        lv_obj.draw_cb(obj, clip_area, mode);
        draw_indic(obj, clip_area);
    }
    else if(mode == LV_DRAW_MODE_POST_DRAW) {
        lv_obj.draw_cb(obj, clip_area, mode);
    }
    return LV_DRAW_RES_OK;
}

static void draw_indic(lv_obj_t * obj, const lv_area_t * clip_area)
{
    lv_bar_t * bar = (lv_bar_t *)obj;

    lv_bidi_dir_t base_dir = lv_obj_get_base_dir(obj);

    lv_area_t bar_coords;
    lv_obj_get_coords(obj, &bar_coords);

    lv_coord_t transf_w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
    lv_coord_t transf_h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
    bar_coords.x1 -= transf_w;
    bar_coords.x2 += transf_w;
    bar_coords.y1 -= transf_h;
    bar_coords.y2 += transf_h;
    lv_coord_t barw = lv_area_get_width(&bar_coords);
    lv_coord_t barh = lv_area_get_height(&bar_coords);
    int32_t range = bar->max_value - bar->min_value;
    bool hor = barw >= barh ? true : false;
    bool sym = false;
    if(bar->type == LV_BAR_TYPE_SYMMETRICAL && bar->min_value < 0 && bar->max_value > 0 &&
       bar->start_value == bar->min_value) sym = true;

    /*Calculate the indicator area*/
    lv_coord_t bg_left = lv_obj_get_style_pad_left(obj,     LV_PART_MAIN);
    lv_coord_t bg_right = lv_obj_get_style_pad_right(obj,   LV_PART_MAIN);
    lv_coord_t bg_top = lv_obj_get_style_pad_top(obj,       LV_PART_MAIN);
    lv_coord_t bg_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
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

    lv_coord_t indicw = lv_area_get_width(&bar->indic_area);
    lv_coord_t indich = lv_area_get_height(&bar->indic_area);

    /*Calculate the indicator length*/
    lv_coord_t anim_length = hor ? indicw : indich;

    lv_coord_t anim_cur_value_x, anim_start_value_x;

    lv_coord_t * axis1, * axis2;
    lv_coord_t (*indic_length_calc)(const lv_area_t * area);

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
        lv_coord_t anim_start_value_start_x =
            (int32_t)((int32_t)anim_length * (bar->start_value_anim.anim_start - bar->min_value)) / range;
        lv_coord_t anim_start_value_end_x =
            (int32_t)((int32_t)anim_length * (bar->start_value_anim.anim_end - bar->min_value)) / range;

        anim_start_value_x = (((anim_start_value_end_x - anim_start_value_start_x) * bar->start_value_anim.anim_state) /
                              LV_BAR_ANIM_STATE_END);

        anim_start_value_x += anim_start_value_start_x;
    }
    else
    {
        anim_start_value_x = (int32_t)((int32_t)anim_length * (bar->start_value - bar->min_value)) / range;
    }

    if(LV_BAR_IS_ANIMATING(bar->cur_value_anim)) {
        lv_coord_t anim_cur_value_start_x =
            (int32_t)((int32_t)anim_length * (bar->cur_value_anim.anim_start - bar->min_value)) / range;
        lv_coord_t anim_cur_value_end_x =
            (int32_t)((int32_t)anim_length * (bar->cur_value_anim.anim_end - bar->min_value)) / range;

        anim_cur_value_x = anim_cur_value_start_x + (((anim_cur_value_end_x - anim_cur_value_start_x) *
                                                      bar->cur_value_anim.anim_state) /
                                                     LV_BAR_ANIM_STATE_END);
    }
    else
    {
        anim_cur_value_x = (int32_t)((int32_t)anim_length * (bar->cur_value - bar->min_value)) / range;
    }

    if(hor && base_dir == LV_BIDI_DIR_RTL) {
        /* Swap axes */
        lv_coord_t * tmp;
        tmp = axis1;
        axis1 = axis2;
        axis2 = tmp;
        anim_cur_value_x = -anim_cur_value_x;
        anim_start_value_x = -anim_start_value_x;
    }

    /* Set the indicator length */
    if(hor) {
        *axis2 = *axis1 + anim_cur_value_x;
        *axis1 += anim_start_value_x;
    }
    else {
        *axis1 = *axis2 - anim_cur_value_x;
        *axis2 -= anim_start_value_x;
    }
    if(sym) {
        lv_coord_t zero;
        zero = *axis1 + (-bar->min_value * anim_length) / range;
        if(*axis2 > zero)
            *axis1 = zero;
        else {
            *axis1 = *axis2;
            *axis2 = zero;
        }
    }

    /*Draw the indicator*/

    /*Do not draw a zero length indicator*/
    if(!sym && indic_length_calc(&bar->indic_area) <= 1) return;

    uint16_t bg_radius = lv_obj_get_style_radius(obj, LV_PART_MAIN);
    lv_coord_t short_side = LV_MIN(barw, barh);
    if(bg_radius > short_side >> 1) bg_radius = short_side >> 1;

    lv_draw_rect_dsc_t draw_indic_dsc;
    lv_draw_rect_dsc_init(&draw_indic_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_INDICATOR, &draw_indic_dsc);

    lv_area_t indic_area;
    lv_area_copy(&indic_area, &bar->indic_area);

    /* Draw only the shadow if the indicator is long enough.
     * The radius of the bg and the indicator can make a strange shape where
     * it'd be very difficult to draw shadow. */
    if((hor && lv_area_get_width(&bar->indic_area) > bg_radius * 2) ||
       (!hor && lv_area_get_height(&bar->indic_area) > bg_radius * 2)) {
        lv_opa_t bg_opa = draw_indic_dsc.bg_opa;
        lv_opa_t border_opa = draw_indic_dsc.border_opa;
        lv_opa_t content_opa = draw_indic_dsc.content_opa;
        draw_indic_dsc.bg_opa = LV_OPA_TRANSP;
        draw_indic_dsc.border_opa = LV_OPA_TRANSP;
        draw_indic_dsc.content_opa = LV_OPA_TRANSP;
        lv_draw_rect(&bar->indic_area, clip_area, &draw_indic_dsc);
        draw_indic_dsc.bg_opa = bg_opa;
        draw_indic_dsc.border_opa = border_opa;
        draw_indic_dsc.content_opa = content_opa;
    }

    lv_draw_mask_radius_param_t mask_bg_param;
    lv_draw_mask_radius_init(&mask_bg_param, &bar_coords, bg_radius, false);
    int16_t mask_bg_id = lv_draw_mask_add(&mask_bg_param, NULL);

    /*Draw_only the background and the pattern*/
    lv_opa_t shadow_opa = draw_indic_dsc.shadow_opa;
    lv_opa_t border_opa = draw_indic_dsc.border_opa;
    lv_opa_t content_opa = draw_indic_dsc.content_opa;
    draw_indic_dsc.border_opa = LV_OPA_TRANSP;
    draw_indic_dsc.shadow_opa = LV_OPA_TRANSP;
    draw_indic_dsc.content_opa = LV_OPA_TRANSP;

    /*Get the max possible indicator area. The gradient should be applied on this*/
    lv_area_t mask_indic_max_area;
    lv_area_copy(&mask_indic_max_area, &bar_coords);
    mask_indic_max_area.x1 += bg_left;
    mask_indic_max_area.y1 += bg_top;
    mask_indic_max_area.x2 -= bg_right;
    mask_indic_max_area.y2 -= bg_bottom;
    if(hor && lv_area_get_height(&mask_indic_max_area) < LV_BAR_SIZE_MIN) {
        mask_indic_max_area.y1 = obj->coords.y1 + (barh / 2) - (LV_BAR_SIZE_MIN / 2);
        mask_indic_max_area.y2 = mask_indic_max_area.y1 + LV_BAR_SIZE_MIN;
    }
    else if(!hor && lv_area_get_width(&mask_indic_max_area) < LV_BAR_SIZE_MIN) {
        mask_indic_max_area.x1 = obj->coords.x1 + (barw / 2) - (LV_BAR_SIZE_MIN / 2);
        mask_indic_max_area.x2 = mask_indic_max_area.x1 + LV_BAR_SIZE_MIN;
    }

    /*Create a mask to the current indicator area to see only this part from the whole gradient.*/
    lv_draw_mask_radius_param_t mask_indic_param;
    lv_draw_mask_radius_init(&mask_indic_param, &bar->indic_area, draw_indic_dsc.radius, false);
    int16_t mask_indic_id = lv_draw_mask_add(&mask_indic_param, NULL);

    lv_draw_rect(&mask_indic_max_area, clip_area, &draw_indic_dsc);
    draw_indic_dsc.border_opa = border_opa;
    draw_indic_dsc.shadow_opa = shadow_opa;
    draw_indic_dsc.content_opa = content_opa;

    /*Draw the border*/
    draw_indic_dsc.bg_opa = LV_OPA_TRANSP;
    draw_indic_dsc.shadow_opa = LV_OPA_TRANSP;
    draw_indic_dsc.content_opa = LV_OPA_TRANSP;
    lv_draw_rect(&bar->indic_area, clip_area, &draw_indic_dsc);

    lv_draw_mask_remove_id(mask_indic_id);
    lv_draw_mask_remove_id(mask_bg_id);

    /*When not masks draw the value*/
    draw_indic_dsc.content_opa = content_opa;
    draw_indic_dsc.border_opa = LV_OPA_TRANSP;
    lv_draw_rect(&bar->indic_area, clip_area, &draw_indic_dsc);
}

/**
 * Signal function of the bar
 * @param bar pointer to a bar object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_bar_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_res_t res;

    /* Include the ancient signal function */
    res = lv_obj.signal_cb(obj, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_REFR_EXT_DRAW_SIZE) {
        lv_coord_t indic_size;
        indic_size = lv_obj_calculate_ext_draw_size(obj, LV_PART_INDICATOR);

        /*Bg size is handled by lv_obj*/
        lv_coord_t * s = param;
        *s = LV_MAX(*s, indic_size);
    }

    return res;
}

static void lv_bar_anim(lv_bar_anim_t * var, lv_anim_value_t value)
{
    var->anim_state    = value;
    lv_obj_invalidate(var->bar);
}

static void lv_bar_anim_ready(lv_anim_t * a)
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

static void lv_bar_set_value_with_anim(lv_obj_t * obj, int16_t new_value, int16_t * value_ptr,
                                       lv_bar_anim_t * anim_info, lv_anim_enable_t en)
{
    if(en == LV_ANIM_OFF) {
        *value_ptr = new_value;
        lv_obj_invalidate((lv_obj_t*)obj);
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
        /* Stop the previous animation if it exists */
        lv_anim_del(anim_info, NULL);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, anim_info);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_bar_anim);
        lv_anim_set_values(&a, LV_BAR_ANIM_STATE_START, LV_BAR_ANIM_STATE_END);
        lv_anim_set_ready_cb(&a, lv_bar_anim_ready);
        lv_anim_set_time(&a, lv_obj_get_style_anim_time(obj, LV_PART_MAIN));
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

#endif
