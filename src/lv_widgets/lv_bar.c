

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
static lv_design_res_t lv_bar_design(lv_obj_t * bar, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_bar_signal(lv_obj_t * bar, lv_signal_t sign, void * param);
static lv_style_list_t * lv_bar_get_style(lv_obj_t * bar, uint8_t part);

static void draw_bg(lv_obj_t * bar, const lv_area_t * clip_area);
static void draw_indic(lv_obj_t * bar, const lv_area_t * clip_area);

#if LV_USE_ANIMATION
static void lv_bar_set_value_with_anim(lv_obj_t * bar, int16_t new_value, int16_t * value_ptr,
                                       lv_bar_anim_t * anim_info, lv_anim_enable_t en);
static void lv_bar_init_anim(lv_obj_t * bar, lv_bar_anim_t * bar_anim);
static void lv_bar_anim(lv_bar_anim_t * bar, lv_anim_value_t value);
static void lv_bar_anim_ready(lv_anim_t * a);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_cb_t ancestor_design_f;
static lv_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a bar objects
 * @param par pointer to an object, it will be the parent of the new bar
 * @param copy pointer to a bar object, if not NULL then the new object will be copied from it
 * @return pointer to the created bar
 */
lv_obj_t * lv_bar_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("lv_bar create started");

    /*Create the ancestor basic object*/
    lv_obj_t * bar = lv_obj_create(par, copy);
    LV_ASSERT_MEM(bar);
    if(bar == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(bar);
    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_cb(bar);

    /*Allocate the object type specific extended data*/
    lv_bar_ext_t * ext = lv_obj_allocate_ext_attr(bar, sizeof(lv_bar_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(bar);
        return NULL;
    }

    ext->min_value = 0;
    ext->start_value = 0;
    ext->max_value = 100;
    ext->cur_value = 0;
#if LV_USE_ANIMATION
    ext->anim_time  = 200;
    lv_bar_init_anim(bar, &ext->cur_value_anim);
    lv_bar_init_anim(bar, &ext->start_value_anim);
#endif
    ext->type         = LV_BAR_TYPE_NORMAL;

    lv_style_list_init(&ext->style_indic);

    lv_obj_set_signal_cb(bar, lv_bar_signal);
    lv_obj_set_design_cb(bar, lv_bar_design);


    /*Init the new  bar object*/
    if(copy == NULL) {

        lv_obj_set_click(bar, false);
        lv_obj_set_size(bar, LV_DPI * 2, LV_DPI / 10);
        lv_bar_set_value(bar, ext->cur_value, false);

        lv_theme_apply(bar, LV_THEME_BAR);
    }
    else {
        lv_bar_ext_t * ext_copy = lv_obj_get_ext_attr(copy);
        ext->min_value          = ext_copy->min_value;
        ext->start_value        = ext_copy->start_value;
        ext->max_value          = ext_copy->max_value;
        ext->cur_value          = ext_copy->cur_value;
        ext->type                = ext_copy->type;

        lv_style_list_copy(&ext->style_indic, &ext_copy->style_indic);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(bar, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);

        lv_bar_set_value(bar, ext->cur_value, LV_ANIM_OFF);
    }

    LV_LOG_INFO("bar created");

    return bar;
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
void lv_bar_set_value(lv_obj_t * bar, int16_t value, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(bar, LV_OBJX_NAME);

    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
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
    lv_bar_set_value_with_anim(bar, new_value, &ext->cur_value, &ext->cur_value_anim, anim);
#endif
}

/**
 * Set a new start value on the bar
 * @param bar pointer to a bar object
 * @param value new start value
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_bar_set_start_value(lv_obj_t * bar, int16_t start_value, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(bar, LV_OBJX_NAME);

    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
    if(ext->start_value == start_value) return;

    int16_t new_value = start_value;
    new_value = new_value > ext->max_value ? ext->max_value : new_value;
    new_value = new_value < ext->min_value ? ext->min_value : new_value;
    new_value = new_value > ext->cur_value ? ext->cur_value : new_value;

    if(ext->start_value == new_value) return;
#if LV_USE_ANIMATION == 0
    LV_UNUSED(anim);
    ext->start_value = new_value;
#else
    lv_bar_set_value_with_anim(bar, new_value, &ext->start_value, &ext->start_value_anim, anim);
#endif
}

/**
 * Set minimum and the maximum values of a bar
 * @param bar pointer to the bar object
 * @param min minimum value
 * @param max maximum value
 */
void lv_bar_set_range(lv_obj_t * bar, int16_t min, int16_t max)
{
    LV_ASSERT_OBJ(bar, LV_OBJX_NAME);

    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
    if(ext->min_value == min && ext->max_value == max) return;

    ext->max_value = max;
    ext->min_value = min;

    if(lv_bar_get_type(bar) != LV_BAR_TYPE_CUSTOM)
        ext->start_value = min;

    if(ext->cur_value > max) {
        ext->cur_value = max;
        lv_bar_set_value(bar, ext->cur_value, false);
    }
    if(ext->cur_value < min) {
        ext->cur_value = min;
        lv_bar_set_value(bar, ext->cur_value, false);
    }
    lv_obj_invalidate(bar);
}

/**
 * Set the type of bar.
 * @param bar pointer to bar object
 * @param type bar type
 */
void lv_bar_set_type(lv_obj_t * bar, lv_bar_type_t type)
{
    LV_ASSERT_OBJ(bar, LV_OBJX_NAME);

    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
    ext->type = type;
    if(ext->type != LV_BAR_TYPE_CUSTOM)
        ext->start_value = ext->min_value;

    lv_obj_invalidate(bar);
}

/**
 * Set the animation time of the bar
 * @param bar pointer to a bar object
 * @param anim_time the animation time in milliseconds.
 */
void lv_bar_set_anim_time(lv_obj_t * bar, uint16_t anim_time)
{
    LV_ASSERT_OBJ(bar, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
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
int16_t lv_bar_get_value(const lv_obj_t * bar)
{
    LV_ASSERT_OBJ(bar, LV_OBJX_NAME);

    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
    return LV_BAR_GET_ANIM_VALUE(ext->cur_value, ext->cur_value_anim);
}

/**
 * Get the start value of a bar
 * @param bar pointer to a bar object
 * @return the start value of the bar
 */
int16_t lv_bar_get_start_value(const lv_obj_t * bar)
{
    LV_ASSERT_OBJ(bar, LV_OBJX_NAME);

    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);

    if(ext->type != LV_BAR_TYPE_CUSTOM) return ext->min_value;

    return LV_BAR_GET_ANIM_VALUE(ext->start_value, ext->start_value_anim);
}

/**
 * Get the minimum value of a bar
 * @param bar pointer to a bar object
 * @return the minimum value of the bar
 */
int16_t lv_bar_get_min_value(const lv_obj_t * bar)
{
    LV_ASSERT_OBJ(bar, LV_OBJX_NAME);

    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
    return ext->min_value;
}

/**
 * Get the maximum value of a bar
 * @param bar pointer to a bar object
 * @return the maximum value of the bar
 */
int16_t lv_bar_get_max_value(const lv_obj_t * bar)
{
    LV_ASSERT_OBJ(bar, LV_OBJX_NAME);

    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
    return ext->max_value;
}

/**
 * Get the type of bar.
 * @param bar pointer to bar object
 * @return bar type
 */
lv_bar_type_t lv_bar_get_type(lv_obj_t * bar)
{
    LV_ASSERT_OBJ(bar, LV_OBJX_NAME);

    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
    return ext->type;
}

/**
 * Get the animation time of the bar
 * @param bar pointer to a bar object
 * @return the animation time in milliseconds.
 */
uint16_t lv_bar_get_anim_time(const lv_obj_t * bar)
{
    LV_ASSERT_OBJ(bar, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
    return ext->anim_time;
#else
    (void)bar;       /*Unused*/
    return 0;
#endif
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

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
static lv_design_res_t lv_bar_design(lv_obj_t * bar, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        /*Return false if the object is not covers the mask area*/
        return ancestor_design_f(bar, clip_area, mode);
    }
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        draw_bg(bar, clip_area);
        draw_indic(bar, clip_area);

        /*Get the value and draw it after the indicator*/
        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);
        draw_dsc.bg_opa = LV_OPA_TRANSP;
        draw_dsc.border_opa = LV_OPA_TRANSP;
        draw_dsc.shadow_opa = LV_OPA_TRANSP;
        draw_dsc.pattern_opa = LV_OPA_TRANSP;
        draw_dsc.outline_opa = LV_OPA_TRANSP;
        lv_obj_init_draw_rect_dsc(bar, LV_BAR_PART_BG, &draw_dsc);
        lv_draw_rect(&bar->coords, clip_area, &draw_dsc);
    }
    else if(mode == LV_DESIGN_DRAW_POST) {
        /*If the border is drawn later disable loading other properties*/
        if(lv_obj_get_style_border_post(bar, LV_OBJ_PART_MAIN)) {
            lv_draw_rect_dsc_t draw_dsc;
            lv_draw_rect_dsc_init(&draw_dsc);
            draw_dsc.bg_opa = LV_OPA_TRANSP;
            draw_dsc.pattern_opa = LV_OPA_TRANSP;
            draw_dsc.outline_opa = LV_OPA_TRANSP;
            draw_dsc.shadow_opa = LV_OPA_TRANSP;
            draw_dsc.value_opa = LV_OPA_TRANSP;
            lv_obj_init_draw_rect_dsc(bar, LV_OBJ_PART_MAIN, &draw_dsc);

            lv_draw_rect(&bar->coords, clip_area, &draw_dsc);
        }
    }
    return LV_DESIGN_RES_OK;
}

static void draw_bg(lv_obj_t * bar, const lv_area_t * clip_area)
{
    /*Simply draw the background*/
    lv_draw_rect_dsc_t draw_dsc;
    lv_draw_rect_dsc_init(&draw_dsc);
    /*If the border is drawn later disable loading its properties*/
    if(lv_obj_get_style_border_post(bar, LV_BAR_PART_BG)) {
        draw_dsc.border_opa = LV_OPA_TRANSP;
    }

    /*value will be drawn later*/
    draw_dsc.value_opa = LV_OPA_TRANSP;
    lv_obj_init_draw_rect_dsc(bar, LV_BAR_PART_BG, &draw_dsc);
    lv_draw_rect(&bar->coords, clip_area, &draw_dsc);

}

static void draw_indic(lv_obj_t * bar, const lv_area_t * clip_area)
{
    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
    lv_bidi_dir_t base_dir = lv_obj_get_base_dir(bar);

    lv_coord_t objw = lv_obj_get_width(bar);
    lv_coord_t objh = lv_obj_get_height(bar);
    int32_t range = ext->max_value - ext->min_value;
    bool hor = objw >= objh ? true : false;
    bool sym = false;
    if(ext->type == LV_BAR_TYPE_SYMMETRICAL && ext->min_value < 0 && ext->max_value > 0 &&
       ext->start_value == ext->min_value) sym = true;

    /*Calculate the indicator area*/
    lv_style_int_t bg_left = lv_obj_get_style_pad_left(bar,     LV_BAR_PART_BG);
    lv_style_int_t bg_right = lv_obj_get_style_pad_right(bar,   LV_BAR_PART_BG);
    lv_style_int_t bg_top = lv_obj_get_style_pad_top(bar,       LV_BAR_PART_BG);
    lv_style_int_t bg_bottom = lv_obj_get_style_pad_bottom(bar, LV_BAR_PART_BG);

    /*Respect padding and minimum width/height too*/
    lv_area_copy(&ext->indic_area, &bar->coords);
    ext->indic_area.x1 += bg_left;
    ext->indic_area.x2 -= bg_right;
    ext->indic_area.y1 += bg_top;
    ext->indic_area.y2 -= bg_bottom;

    if(hor && lv_area_get_height(&ext->indic_area) < LV_BAR_SIZE_MIN) {
        ext->indic_area.y1 = bar->coords.y1 + (objh / 2) - (LV_BAR_SIZE_MIN / 2);
        ext->indic_area.y2 = ext->indic_area.y1 + LV_BAR_SIZE_MIN;
    }
    else if(!hor && lv_area_get_width(&ext->indic_area) < LV_BAR_SIZE_MIN) {
        ext->indic_area.x1 = bar->coords.x1 + (objw / 2) - (LV_BAR_SIZE_MIN / 2);
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

    uint16_t bg_radius = lv_obj_get_style_radius(bar, LV_BAR_PART_BG);
    lv_coord_t short_side = LV_MATH_MIN(objw, objh);
    if(bg_radius > short_side >> 1) bg_radius = short_side >> 1;

    lv_draw_rect_dsc_t draw_indic_dsc;
    lv_draw_rect_dsc_init(&draw_indic_dsc);
    lv_obj_init_draw_rect_dsc(bar, LV_BAR_PART_INDIC, &draw_indic_dsc);

    /* Draw only the shadow if the indicator is long enough.
     * The radius of the bg and the indicator can make a strange shape where
     * it'd be very difficult to draw shadow. */
    if((hor && lv_area_get_width(&ext->indic_area) > bg_radius * 2) ||
       (!hor && lv_area_get_height(&ext->indic_area) > bg_radius * 2)) {
        lv_opa_t bg_opa = draw_indic_dsc.bg_opa;
        lv_opa_t border_opa = draw_indic_dsc.border_opa;
        lv_opa_t value_opa = draw_indic_dsc.value_opa;
        const void * pattern_src = draw_indic_dsc.pattern_image;
        draw_indic_dsc.bg_opa = LV_OPA_TRANSP;
        draw_indic_dsc.border_opa = LV_OPA_TRANSP;
        draw_indic_dsc.value_opa = LV_OPA_TRANSP;
        draw_indic_dsc.pattern_image = NULL;
        lv_draw_rect(&ext->indic_area, clip_area, &draw_indic_dsc);
        draw_indic_dsc.bg_opa = bg_opa;
        draw_indic_dsc.border_opa = border_opa;
        draw_indic_dsc.value_opa = value_opa;
        draw_indic_dsc.pattern_image = pattern_src;

    }

    lv_draw_mask_radius_param_t mask_bg_param;
    lv_draw_mask_radius_init(&mask_bg_param, &bar->coords, bg_radius, false);
    int16_t mask_bg_id = lv_draw_mask_add(&mask_bg_param, NULL);

    /*Draw_only the background and the pattern*/
    lv_opa_t shadow_opa = draw_indic_dsc.shadow_opa;
    lv_opa_t border_opa = draw_indic_dsc.border_opa;
    lv_opa_t value_opa = draw_indic_dsc.value_opa;
    draw_indic_dsc.border_opa = LV_OPA_TRANSP;
    draw_indic_dsc.shadow_opa = LV_OPA_TRANSP;
    draw_indic_dsc.value_opa = LV_OPA_TRANSP;

    /*Get the max possible indicator area. The gradient should be applied on this*/
    lv_area_t mask_indic_max_area;
    lv_area_copy(&mask_indic_max_area, &bar->coords);
    mask_indic_max_area.x1 += bg_left;
    mask_indic_max_area.y1 += bg_top;
    mask_indic_max_area.x2 -= bg_right;
    mask_indic_max_area.y2 -= bg_bottom;
    if(hor && lv_area_get_height(&mask_indic_max_area) < LV_BAR_SIZE_MIN) {
        mask_indic_max_area.y1 = bar->coords.y1 + (objh / 2) - (LV_BAR_SIZE_MIN / 2);
        mask_indic_max_area.y2 = mask_indic_max_area.y1 + LV_BAR_SIZE_MIN;
    }
    else if(!hor && lv_area_get_width(&mask_indic_max_area) < LV_BAR_SIZE_MIN) {
        mask_indic_max_area.x1 = bar->coords.x1 + (objw / 2) - (LV_BAR_SIZE_MIN / 2);
        mask_indic_max_area.x2 = mask_indic_max_area.x1 + LV_BAR_SIZE_MIN;
    }

    /*Create a mask to the current indicator area to see only this part from the whole gradient.*/
    lv_draw_mask_radius_param_t mask_indic_param;
    lv_draw_mask_radius_init(&mask_indic_param, &ext->indic_area, draw_indic_dsc.radius, false);
    int16_t mask_indic_id = lv_draw_mask_add(&mask_indic_param, NULL);

    lv_draw_rect(&mask_indic_max_area, clip_area, &draw_indic_dsc);
    draw_indic_dsc.border_opa = border_opa;
    draw_indic_dsc.shadow_opa = shadow_opa;
    draw_indic_dsc.value_opa = value_opa;

    /*Draw the border*/
    draw_indic_dsc.bg_opa = LV_OPA_TRANSP;
    draw_indic_dsc.shadow_opa = LV_OPA_TRANSP;
    draw_indic_dsc.value_opa = LV_OPA_TRANSP;
    draw_indic_dsc.pattern_image = NULL;
    lv_draw_rect(&ext->indic_area, clip_area, &draw_indic_dsc);

    lv_draw_mask_remove_id(mask_indic_id);
    lv_draw_mask_remove_id(mask_bg_id);

    /*When not masks draw the value*/
    draw_indic_dsc.value_opa = value_opa;
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
static lv_res_t lv_bar_signal(lv_obj_t * bar, lv_signal_t sign, void * param)
{
    lv_res_t res;

    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_bar_get_style(bar, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(bar, sign, param);
    }

    /* Include the ancient signal function */
    res = ancestor_signal(bar, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {

        lv_coord_t indic_size;
        indic_size = lv_obj_get_draw_rect_ext_pad_size(bar, LV_BAR_PART_INDIC);

        /*Bg size is handled by lv_obj*/
        bar->ext_draw_pad = LV_MATH_MAX(bar->ext_draw_pad, indic_size);

    }
    if(sign == LV_SIGNAL_CLEANUP) {
        lv_obj_clean_style_list(bar, LV_BAR_PART_INDIC);
#if LV_USE_ANIMATION
        lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
        lv_anim_del(&ext->cur_value_anim, NULL);
        lv_anim_del(&ext->start_value_anim, NULL);
#endif
    }

    return res;
}

static lv_style_list_t * lv_bar_get_style(lv_obj_t * bar, uint8_t part)
{
    LV_ASSERT_OBJ(bar, LV_OBJX_NAME);

    lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
    lv_style_list_t * style_dsc_p;

    switch(part) {
        case LV_BAR_PART_BG:
            style_dsc_p = &bar->style_list;
            break;
        case LV_BAR_PART_INDIC:
            style_dsc_p = &ext->style_indic;
            break;
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
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
    lv_bar_ext_t * ext = lv_obj_get_ext_attr(var->bar);
    var->anim_state = LV_BAR_ANIM_STATE_INV;
    if(var == &ext->cur_value_anim)
        ext->cur_value = var->anim_end;
    else if(var == &ext->start_value_anim)
        ext->start_value = var->anim_end;
    lv_obj_invalidate(var->bar);
}

static void lv_bar_set_value_with_anim(lv_obj_t * bar, int16_t new_value, int16_t * value_ptr,
                                       lv_bar_anim_t * anim_info, lv_anim_enable_t en)
{
    if(en == LV_ANIM_OFF) {
        *value_ptr = new_value;
        lv_obj_invalidate(bar);
    }
    else {
        lv_bar_ext_t * ext = lv_obj_get_ext_attr(bar);
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

static void lv_bar_init_anim(lv_obj_t * bar, lv_bar_anim_t * bar_anim)
{
    bar_anim->bar = bar;
    bar_anim->anim_start = 0;
    bar_anim->anim_end = 0;
    bar_anim->anim_state = LV_BAR_ANIM_STATE_INV;
}
#endif

#endif
