/**
 * @file lv_preload.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_preload.h"
#if USE_LV_PRELOAD != 0

#include "../lv_misc/lv_math.h"
#include "../lv_draw/lv_draw_rect.h"
#include "../lv_draw/lv_draw_arc.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_PRELOAD_DEF_ARC_LENGTH
# define LV_PRELOAD_DEF_ARC_LENGTH  60      /*[deg]*/
#endif

#ifndef LV_PRELOAD_DEF_SPIN_TIME
# define LV_PRELOAD_DEF_SPIN_TIME   1000    /*[ms]*/
#endif
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_preload_design(lv_obj_t * preload, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_preload_signal(lv_obj_t * preload, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;
static lv_design_func_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a pre loader object
 * @param par pointer to an object, it will be the parent of the new pre loader
 * @param copy pointer to a pre loader object, if not NULL then the new object will be copied from it
 * @return pointer to the created pre loader
 */
lv_obj_t * lv_preload_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("preload create started");

    /*Create the ancestor of pre loader*/
    lv_obj_t * new_preload = lv_arc_create(par, copy);
    lv_mem_assert(new_preload);
    if(new_preload == NULL) return NULL;

    /*Allocate the pre loader type specific extended data*/
    lv_preload_ext_t * ext = lv_obj_allocate_ext_attr(new_preload, sizeof(lv_preload_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_preload);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_preload);

    /*Initialize the allocated 'ext' */
    ext->arc_length = LV_PRELOAD_DEF_ARC_LENGTH;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_preload, lv_preload_signal);
    lv_obj_set_design_func(new_preload, lv_preload_design);
#if USE_LV_ANIMATION
    lv_anim_t a;
    a.var = new_preload;
    a.start = 0;
    a.end = 360;
    a.fp = (lv_anim_fp_t)lv_preload_spinner_animation;
    a.path = lv_anim_path_ease_in_out;
    a.end_cb = NULL;
    a.act_time = 0;
    a.time = LV_PRELOAD_DEF_SPIN_TIME;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 1;
    a.repeat_pause = 0;
    lv_anim_create(&a);
#endif

    /*Init the new pre loader pre loader*/
    if(copy == NULL) {
        lv_obj_set_size(new_preload, LV_DPI / 2, LV_DPI / 2);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_preload_set_style(new_preload, LV_PRELOAD_STYLE_MAIN, th->preload);
        } else {
            lv_obj_set_style(new_preload, &lv_style_pretty_color);
        }

    }
    /*Copy an existing pre loader*/
    else {
        lv_preload_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->arc_length = copy_ext->arc_length;
        ext->time = copy_ext->time;
        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_preload);
    }


    LV_LOG_INFO("preload created");

    return new_preload;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Set the length of the spinning  arc in degrees
 * @param preload pointer to a preload object
 * @param deg length of the arc
 */
void lv_preload_set_arc_length(lv_obj_t * preload, uint16_t deg)
{
    lv_preload_ext_t * ext = lv_obj_get_ext_attr(preload);

    ext->arc_length = deg;
}

/**
 * Set the spin time of the arc
 * @param preload pointer to a preload object
 * @param time time of one round in milliseconds
 */
void lv_preload_set_spin_time(lv_obj_t * preload, uint16_t time)
{
    lv_preload_ext_t * ext = lv_obj_get_ext_attr(preload);

    ext->time = time;
#if USE_LV_ANIMATION
    lv_anim_t a;
    a.var = preload;
    a.start = 0;
    a.end = 360;
    a.fp = (lv_anim_fp_t)lv_preload_spinner_animation;
    a.path = lv_anim_path_ease_in_out;
    a.end_cb = NULL;
    a.act_time = 0;
    a.time = time;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 1;
    a.repeat_pause = 0;
    lv_anim_create(&a);
#endif
}
/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a pre loader.
 * @param preload pointer to pre loader object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_preload_set_style(lv_obj_t * preload, lv_preload_style_t type, lv_style_t * style)
{
    switch(type) {
        case LV_PRELOAD_STYLE_MAIN:
            lv_arc_set_style(preload, LV_ARC_STYLE_MAIN, style);
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the arc length [degree] of the a pre loader
 * @param preload pointer to a pre loader object
 */
uint16_t lv_preload_get_arc_length(const lv_obj_t * preload)
{
    lv_preload_ext_t * ext = lv_obj_get_ext_attr(preload);
    return ext->arc_length;

}

/**
 * Get the spin time of the arc
 * @param preload pointer to a pre loader object [milliseconds]
 */
uint16_t lv_preload_get_spin_time(const lv_obj_t * preload)
{
    lv_preload_ext_t * ext = lv_obj_get_ext_attr(preload);
    return ext->time;
}

/**
 * Get style of a pre loader.
 * @param preload pointer to pre loader object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
lv_style_t * lv_preload_get_style(const lv_obj_t * preload, lv_preload_style_t type)
{
    lv_style_t * style = NULL;

    switch(type) {
        case LV_PRELOAD_STYLE_MAIN:
            style = lv_arc_get_style(preload, LV_ARC_STYLE_MAIN);
            break;
        default:
            style = NULL;
            break;
    }

    return style;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Automatically in an animation to rotate the arc of spinner.
 * @param ptr pointer to preloader
 * @param val the current desired value [0..360]
 */
void lv_preload_spinner_animation(void * ptr, int32_t val)
{
    lv_obj_t * preload = ptr;
    lv_preload_ext_t * ext = lv_obj_get_ext_attr(preload);
    int16_t angle_start = val - ext->arc_length / 2 + 180;
    int16_t angle_end = angle_start + ext->arc_length;

    angle_start = angle_start % 360;
    angle_end = angle_end % 360;

    lv_arc_set_angles(preload, angle_start, angle_end);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the pre loaders
 * @param preload pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_preload_design(lv_obj_t * preload, const lv_area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {

        /*Draw a circle as background*/
        lv_style_t * style = lv_arc_get_style(preload, LV_ARC_STYLE_MAIN);
        if(style->body.border.width > 0) {
            lv_coord_t r = (LV_MATH_MIN(lv_obj_get_width(preload), lv_obj_get_height(preload))) / 2;
            r -= LV_MATH_MIN(style->body.padding.hor, style->body.padding.ver);

            lv_coord_t x = preload->coords.x1 + lv_obj_get_width(preload) / 2;
            lv_coord_t y = preload->coords.y1 + lv_obj_get_height(preload) / 2;

            lv_style_t bg_style;
            lv_style_copy(&bg_style, &lv_style_plain);
            bg_style.body.empty = 1;
            bg_style.body.radius = LV_RADIUS_CIRCLE;
            bg_style.body.border.color = style->body.border.color;
            bg_style.body.border.width = style->body.border.width;

            lv_area_t bg_area;
            bg_area.x1 = x - r;
            bg_area.y1 = y - r;
            bg_area.x2 = x + r;
            bg_area.y2 = y + r;

            lv_draw_rect(&bg_area, mask, &bg_style, lv_obj_get_opa_scale(preload));
        }
        /*Draw the arc above the background circle */
        ancestor_design(preload, mask, mode);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}

/**
 * Signal function of the pre loader
 * @param preload pointer to a pre loader object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_preload_signal(lv_obj_t * preload, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(preload, sign, param);
    if(res != LV_RES_OK) return res;


    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_preload";
    }

    return res;
}

#endif
