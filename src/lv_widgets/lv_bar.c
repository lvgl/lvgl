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

#if LV_USE_ANIMATION
    #define LV_BAR_IS_ANIMATING(anim_struct) (((anim_struct).anim_state) != LV_BAR_ANIM_STATE_INV)
    #define LV_BAR_GET_ANIM_VALUE(orig_value, anim_struct) (LV_BAR_IS_ANIMATING(anim_struct) ? ((anim_struct).anim_end) : (orig_value))
#else
    #define LV_BAR_GET_ANIM_VALUE(orig_value, anim_struct) (orig_value)
#endif
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_bar_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_bar_destructor(lv_obj_t * obj);
static lv_design_res_t lv_bar_design(lv_obj_t * bar, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_bar_signal(lv_obj_t * bar, lv_signal_t sign, void * param);
static void draw_indic(lv_obj_t * bar, const lv_area_t * clip_area);

#if LV_USE_ANIMATION
static void lv_bar_set_value_with_anim(lv_obj_t * obj, int16_t new_value, int16_t * value_ptr,
                                       lv_bar_anim_t * anim_info, lv_anim_enable_t en);
static void lv_bar_init_anim(lv_obj_t * bar, lv_bar_anim_t * bar_anim);
static void lv_bar_anim(lv_bar_anim_t * bar, lv_anim_value_t value);
static void lv_bar_anim_ready(lv_anim_t * a);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_bar  = {
    .constructor = lv_bar_constructor,
    .destructor = lv_bar_destructor,
    .signal_cb = lv_bar_signal,
    .design_cb = lv_bar_design,
    .ext_size = sizeof(lv_bar_ext_t),
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
    lv_bar_ext_t * ext = obj->ext_attr;

    if(ext->cur_value == value) return;

    int16_t new_value = value;
    new_value = value > ext->max_value ? ext->max_value : new_value;
    new_value = new_value < ext->min_value ? ext->min_value : new_value;
    new_value = new_value < ext->start_value ? ext->start_value : new_value;


    if(ext->cur_value == new_value) return;
#if LV_USE_ANIMATION == 0
    LV_UNUSED(anim);
    ext->cur_value = new_value;
    lv_obj_invalidate(bar);
#else
    lv_bar_set_value_with_anim(obj, new_value, &ext->cur_value, &ext->cur_value_anim, anim);
#endif
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

    lv_bar_ext_t * ext = obj->ext_attr;

    int16_t new_value = start_value;
    new_value = new_value > ext->max_value ? ext->max_value : new_value;
    new_value = new_value < ext->min_value ? ext->min_value : new_value;
    new_value = new_value > ext->cur_value ? ext->cur_value : new_value;

    if(ext->start_value == new_value) return;
#if LV_USE_ANIMATION == 0
    LV_UNUSED(anim);
    ext->start_value = new_value;
#else
    lv_bar_set_value_with_anim(obj, new_value, &ext->start_value, &ext->start_value_anim, anim);
#endif
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

    lv_bar_ext_t * ext = obj->ext_attr;

    if(ext->min_value == min && ext->max_value == max) return;

    ext->max_value = max;
    ext->min_value = min;

    if(lv_bar_get_type(obj) != LV_BAR_TYPE_CUSTOM)
        ext->start_value = min;

    if(ext->cur_value > max) {
        ext->cur_value = max;
        lv_bar_set_value(obj, ext->cur_value, false);
    }
    if(ext->cur_value < min) {
        ext->cur_value = min;
        lv_bar_set_value(obj, ext->cur_value, false);
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
    lv_bar_ext_t * ext = obj->ext_attr;

    ext->type = type;
    if(ext->type != LV_BAR_TYPE_CUSTOM)
        ext->start_value = ext->min_value;

    lv_obj_invalidate(obj);
}

/**
 * Set the animation time of the bar
 * @param bar pointer to a bar object
 * @param anim_time the animation time in milliseconds.
 */
void lv_bar_set_anim_time(lv_obj_t * obj, uint16_t anim_time)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    lv_bar_ext_t * ext = obj->ext_attr;
    ext->anim_time     = anim_time;
#else
    (void)bar;       /*Unused*/
    (void)anim_time; /*Unused*/
#endif
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
    lv_bar_ext_t * ext = obj->ext_attr;

    return LV_BAR_GET_ANIM_VALUE(ext->cur_value, ext->cur_value_anim);
}

/**
 * Get the start value of a bar
 * @param bar pointer to a bar object
 * @return the start value of the bar
 */
int16_t lv_bar_get_start_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_bar_ext_t * ext = obj->ext_attr;

    if(ext->type != LV_BAR_TYPE_CUSTOM) return ext->min_value;

    return LV_BAR_GET_ANIM_VALUE(ext->start_value, ext->start_value_anim);
}

/**
 * Get the minimum value of a bar
 * @param bar pointer to a bar object
 * @return the minimum value of the bar
 */
int16_t lv_bar_get_min_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_bar_ext_t * ext = obj->ext_attr;
    return ext->min_value;
}

/**
 * Get the maximum value of a bar
 * @param bar pointer to a bar object
 * @return the maximum value of the bar
 */
int16_t lv_bar_get_max_value(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_bar_ext_t * ext = obj->ext_attr;

    return ext->max_value;
}

/**
 * Get the type of bar.
 * @param bar pointer to bar object
 * @return bar type
 */
lv_bar_type_t lv_bar_get_type(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_bar_ext_t * ext = obj->ext_attr;

    return ext->type;
}

/**
 * Get the animation time of the bar
 * @param bar pointer to a bar object
 * @return the animation time in milliseconds.
 */
uint16_t lv_bar_get_anim_time(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_bar_ext_t * ext = obj->ext_attr;
    return ext->anim_time;
#else
    (void)bar;       /*Unused*/
    return 0;
#endif
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_bar_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_TRACE("lv_bar create started");

    LV_CLASS_CONSTRUCTOR_BEGIN(obj, lv_bar)
    lv_obj.constructor(obj, parent, copy);

    lv_bar_ext_t * ext = obj->ext_attr;
    ext->min_value = 0;
    ext->max_value = 100;
    ext->start_value = 0;
    ext->cur_value = 0;
    ext->type         = LV_BAR_TYPE_NORMAL;

#if LV_USE_ANIMATION
    ext->anim_time  = 200;
    lv_bar_init_anim(obj, &ext->cur_value_anim);
    lv_bar_init_anim(obj, &ext->start_value_anim);
#endif

   if(copy == NULL) {
       lv_obj_clear_flag(obj, LV_OBJ_FLAG_CHECKABLE);
       lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
       lv_obj_set_size(obj, LV_DPI * 2, LV_DPI / 10);
       lv_bar_set_value(obj, 0, LV_ANIM_OFF);
   } else {
       lv_bar_ext_t * copy_ext = copy->ext_attr;
       ext->min_value   = copy_ext->min_value;
       ext->start_value = copy_ext->start_value;
       ext->max_value   = copy_ext->max_value;
       ext->cur_value   = copy_ext->cur_value;
       ext->type        = copy_ext->type;

       lv_bar_set_value(obj, ext->cur_value, LV_ANIM_OFF);
   }
   LV_CLASS_CONSTRUCTOR_END(obj, lv_bar)
   LV_LOG_INFO("bar created");
}

static void lv_bar_destructor(lv_obj_t * obj)
{
//    lv_bar_t * bar = obj;
//
//    _lv_obj_reset_style_list_no_refr(obj, LV_PART_INDICATOR);
//#if LV_USE_ANIMATION
//    lv_anim_del(&ext->cur_value_anim, NULL);
//    lv_anim_del(&ext->start_value_anim, NULL);
//#endif

//    ext->class_p->base_p->destructor(obj);
}

/**
 * Handle the drawing related tasks of the bars
 * @param bar pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_bar_design(lv_obj_t * obj, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        /*Return false if the object is not covers the mask area*/
        return lv_obj.design_cb(obj, clip_area, mode);
    }
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        //Draw the background
        lv_obj.design_cb(obj, clip_area, mode);
        draw_indic(obj, clip_area);

        /*Get the value and draw it after the indicator*/
        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);
        draw_dsc.bg_opa = LV_OPA_TRANSP;
        draw_dsc.border_opa = LV_OPA_TRANSP;
        draw_dsc.shadow_opa = LV_OPA_TRANSP;
        draw_dsc.content_opa = LV_OPA_TRANSP;
        draw_dsc.outline_opa = LV_OPA_TRANSP;
        lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &draw_dsc);
        lv_draw_rect(&obj->coords, clip_area, &draw_dsc);
    }
    else if(mode == LV_DESIGN_DRAW_POST) {
        lv_obj.design_cb(obj, clip_area, mode);
    }
    return LV_DESIGN_RES_OK;
}

static void draw_indic(lv_obj_t * obj, const lv_area_t * clip_area)
{
    lv_bar_ext_t * ext = obj->ext_attr;

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
    int32_t range = ext->max_value - ext->min_value;
    bool hor = barw >= barh ? true : false;
    bool sym = false;
    if(ext->type == LV_BAR_TYPE_SYMMETRICAL && ext->min_value < 0 && ext->max_value > 0 &&
       ext->start_value == ext->min_value) sym = true;

    /*Calculate the indicator area*/
    lv_coord_t bg_left = lv_obj_get_style_pad_left(obj,     LV_PART_MAIN);
    lv_coord_t bg_right = lv_obj_get_style_pad_right(obj,   LV_PART_MAIN);
    lv_coord_t bg_top = lv_obj_get_style_pad_top(obj,       LV_PART_MAIN);
    lv_coord_t bg_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
    /*Respect padding and minimum width/height too*/
    lv_area_copy(&ext->indic_area, &bar_coords);
    ext->indic_area.x1 += bg_left;
    ext->indic_area.x2 -= bg_right;
    ext->indic_area.y1 += bg_top;
    ext->indic_area.y2 -= bg_bottom;

    if(hor && lv_area_get_height(&ext->indic_area) < LV_BAR_SIZE_MIN) {
        ext->indic_area.y1 = obj->coords.y1 + (barh / 2) - (LV_BAR_SIZE_MIN / 2);
        ext->indic_area.y2 = ext->indic_area.y1 + LV_BAR_SIZE_MIN;
    }
    else if(!hor && lv_area_get_width(&ext->indic_area) < LV_BAR_SIZE_MIN) {
        ext->indic_area.x1 = obj->coords.x1 + (barw / 2) - (LV_BAR_SIZE_MIN / 2);
        ext->indic_area.x2 = ext->indic_area.x1 + LV_BAR_SIZE_MIN;
    }

    lv_coord_t indicw = lv_area_get_width(&ext->indic_area);
    lv_coord_t indich = lv_area_get_height(&ext->indic_area);

    /*Calculate the indicator length*/
    lv_coord_t anim_length = hor ? indicw : indich;

    lv_coord_t anim_cur_value_x, anim_start_value_x;

    lv_coord_t * axis1, * axis2;
    lv_coord_t (*indic_length_calc)(const lv_area_t * area);

    if(hor) {
        axis1 = &ext->indic_area.x1;
        axis2 = &ext->indic_area.x2;
        indic_length_calc = lv_area_get_width;
    }
    else {
        axis1 = &ext->indic_area.y1;
        axis2 = &ext->indic_area.y2;
        indic_length_calc = lv_area_get_height;
    }

#if LV_USE_ANIMATION
    if(LV_BAR_IS_ANIMATING(ext->start_value_anim)) {
        lv_coord_t anim_start_value_start_x =
            (int32_t)((int32_t)anim_length * (ext->start_value_anim.anim_start - ext->min_value)) / range;
        lv_coord_t anim_start_value_end_x =
            (int32_t)((int32_t)anim_length * (ext->start_value_anim.anim_end - ext->min_value)) / range;

        anim_start_value_x = (((anim_start_value_end_x - anim_start_value_start_x) * ext->start_value_anim.anim_state) /
                              LV_BAR_ANIM_STATE_END);

        anim_start_value_x += anim_start_value_start_x;
    }
    else
#endif
    {
        anim_start_value_x = (int32_t)((int32_t)anim_length * (ext->start_value - ext->min_value)) / range;
    }

#if LV_USE_ANIMATION
    if(LV_BAR_IS_ANIMATING(ext->cur_value_anim)) {
        lv_coord_t anim_cur_value_start_x =
            (int32_t)((int32_t)anim_length * (ext->cur_value_anim.anim_start - ext->min_value)) / range;
        lv_coord_t anim_cur_value_end_x =
            (int32_t)((int32_t)anim_length * (ext->cur_value_anim.anim_end - ext->min_value)) / range;

        anim_cur_value_x = anim_cur_value_start_x + (((anim_cur_value_end_x - anim_cur_value_start_x) *
                                                      ext->cur_value_anim.anim_state) /
                                                     LV_BAR_ANIM_STATE_END);
    }
    else
#endif
    {
        anim_cur_value_x = (int32_t)((int32_t)anim_length * (ext->cur_value - ext->min_value)) / range;
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
        zero = *axis1 + (-ext->min_value * anim_length) / range;
        if(*axis2 > zero)
            *axis1 = zero;
        else {
            *axis1 = *axis2;
            *axis2 = zero;
        }
    }

    /*Draw the indicator*/

    /*Do not draw a zero length indicator*/
    if(!sym && indic_length_calc(&ext->indic_area) <= 1) return;

    uint16_t bg_radius = lv_obj_get_style_radius(obj, LV_PART_MAIN);
    lv_coord_t short_side = LV_MATH_MIN(barw, barh);
    if(bg_radius > short_side >> 1) bg_radius = short_side >> 1;

    lv_draw_rect_dsc_t draw_indic_dsc;
    lv_draw_rect_dsc_init(&draw_indic_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_INDICATOR, &draw_indic_dsc);

    /* Draw only the shadow if the indicator is long enough.
     * The radius of the bg and the indicator can make a strange shape where
     * it'd be very difficult to draw shadow. */
    if((hor && lv_area_get_width(&ext->indic_area) > bg_radius * 2) ||
       (!hor && lv_area_get_height(&ext->indic_area) > bg_radius * 2)) {
        lv_opa_t bg_opa = draw_indic_dsc.bg_opa;
        lv_opa_t border_opa = draw_indic_dsc.border_opa;
        lv_opa_t content_opa = draw_indic_dsc.content_opa;
        draw_indic_dsc.bg_opa = LV_OPA_TRANSP;
        draw_indic_dsc.border_opa = LV_OPA_TRANSP;
        draw_indic_dsc.content_opa = LV_OPA_TRANSP;
        lv_draw_rect(&ext->indic_area, clip_area, &draw_indic_dsc);
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
    lv_draw_mask_radius_init(&mask_indic_param, &ext->indic_area, draw_indic_dsc.radius, false);
    int16_t mask_indic_id = lv_draw_mask_add(&mask_indic_param, NULL);

    lv_draw_rect(&mask_indic_max_area, clip_area, &draw_indic_dsc);
    draw_indic_dsc.border_opa = border_opa;
    draw_indic_dsc.shadow_opa = shadow_opa;
    draw_indic_dsc.content_opa = content_opa;

    /*Draw the border*/
    draw_indic_dsc.bg_opa = LV_OPA_TRANSP;
    draw_indic_dsc.shadow_opa = LV_OPA_TRANSP;
    draw_indic_dsc.content_opa = LV_OPA_TRANSP;
    lv_draw_rect(&ext->indic_area, clip_area, &draw_indic_dsc);

    lv_draw_mask_remove_id(mask_indic_id);
    lv_draw_mask_remove_id(mask_bg_id);

    /*When not masks draw the value*/
    draw_indic_dsc.content_opa = content_opa;
    draw_indic_dsc.border_opa = LV_OPA_TRANSP;
    lv_draw_rect(&ext->indic_area, clip_area, &draw_indic_dsc);

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

    if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        lv_coord_t indic_size;
        indic_size = _lv_obj_get_draw_rect_ext_pad_size(obj, LV_PART_INDICATOR);

        /*Bg size is handled by lv_obj*/
        lv_coord_t * s = param;
        *s = LV_MATH_MAX(*s, indic_size);
    }

    return res;
}

#if LV_USE_ANIMATION
static void lv_bar_anim(lv_bar_anim_t * var, lv_anim_value_t value)
{
    var->anim_state    = value;
    lv_obj_invalidate(var->bar);
}

static void lv_bar_anim_ready(lv_anim_t * a)
{
    lv_bar_anim_t * var = a->var;
    lv_obj_t * obj = (lv_obj_t *)var->bar;
    lv_bar_ext_t * ext = obj->ext_attr;

    var->anim_state = LV_BAR_ANIM_STATE_INV;
    if(var == &ext->cur_value_anim)
        ext->cur_value = var->anim_end;
    else if(var == &ext->start_value_anim)
        ext->start_value = var->anim_end;
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
        lv_bar_ext_t * ext = obj->ext_attr;

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
        lv_anim_set_time(&a, ext->anim_time);
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

#endif
