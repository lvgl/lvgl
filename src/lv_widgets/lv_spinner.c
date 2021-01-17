/**
 * @file lv_spinner.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_spinner.h"
#if LV_USE_SPINNER != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_misc/lv_math.h"
#include "../lv_draw/lv_draw_rect.h"
#include "../lv_draw/lv_draw_arc.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_spinner"

#ifndef LV_SPINNER_DEF_ARC_LENGTH
    #define LV_SPINNER_DEF_ARC_LENGTH 60 /*[deg]*/
#endif

#ifndef LV_SPINNER_DEF_SPIN_TIME
    #define LV_SPINNER_DEF_SPIN_TIME 1000 /*[ms]*/
#endif

#ifndef LV_SPINNER_DEF_ANIM
    #define LV_SPINNER_DEF_ANIM LV_SPINNER_TYPE_SPINNING_ARC /*animation type*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_spinner_signal(lv_obj_t * spinner, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_design_cb_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a spinner object
 * @param par pointer to an object, it will be the parent of the new spinner
 * @param copy pointer to a spinner object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created spinner
 */
lv_obj_t * lv_spinner_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("spinner create started");

    /*Create the ancestor of spinner*/
    lv_obj_t * spinner = lv_arc_create(par, copy);
    LV_ASSERT_MEM(spinner);
    if(spinner == NULL) return NULL;

    /*Allocate the spinner type specific extended data*/
    lv_spinner_ext_t * ext = lv_obj_allocate_ext_attr(spinner, sizeof(lv_spinner_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(spinner);
        return NULL;
    }

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(spinner);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(spinner);

    /*Initialize the allocated 'ext' */
    ext->arc_length = LV_SPINNER_DEF_ARC_LENGTH;
    ext->anim_type  = LV_SPINNER_DEF_ANIM;
    ext->anim_dir   = LV_SPINNER_DIR_FORWARD;
    ext->time = LV_SPINNER_DEF_SPIN_TIME;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(spinner, lv_spinner_signal);

    /*Init the new spinner spinner*/
    if(copy == NULL) {
        ext->arc.bg_angle_start = 0;
        ext->arc.bg_angle_end = 360;
        lv_obj_set_size(spinner, LV_DPI, LV_DPI);
        lv_theme_apply(spinner, LV_THEME_SPINNER);

    }
    /*Copy an existing spinner*/
    else {
        lv_spinner_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->arc_length             = copy_ext->arc_length;
        ext->time                   = copy_ext->time;
        ext->anim_dir               = copy_ext->anim_dir;
        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(spinner, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    lv_spinner_set_type(spinner, ext->anim_type);

    LV_LOG_INFO("spinner created");

    return spinner;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Set the length of the spinning arc in degrees
 * @param spinner pointer to a spinner object
 * @param deg length of the arc
 */
void lv_spinner_set_arc_length(lv_obj_t * spinner, lv_anim_value_t deg)
{
    LV_ASSERT_OBJ(spinner, LV_OBJX_NAME);

    lv_spinner_ext_t * ext = lv_obj_get_ext_attr(spinner);

    ext->arc_length = deg;
}

/**
 * Set the spin time of the arc
 * @param spinner pointer to a spinner object
 * @param time time of one round in milliseconds
 */
void lv_spinner_set_spin_time(lv_obj_t * spinner, uint16_t time)
{
    LV_ASSERT_OBJ(spinner, LV_OBJX_NAME);

    lv_spinner_ext_t * ext = lv_obj_get_ext_attr(spinner);

    ext->time = time;
    lv_spinner_set_type(spinner, ext->anim_type);
}
/*=====================
 * Setter functions
 *====================*/

/**
 * Set the animation type of a spinner.
 * @param spinner pointer to spinner object
 * @param type animation type of the spinner
 *  */
void lv_spinner_set_type(lv_obj_t * spinner, lv_spinner_type_t type)
{
    LV_ASSERT_OBJ(spinner, LV_OBJX_NAME);

    lv_spinner_ext_t * ext = lv_obj_get_ext_attr(spinner);

    /*delete previous animation*/
    lv_anim_del(spinner, NULL);
    switch(type) {
        case LV_SPINNER_TYPE_FILLSPIN_ARC: {
                ext->anim_type = LV_SPINNER_TYPE_FILLSPIN_ARC;
                lv_anim_path_t path;
                lv_anim_path_init(&path);
                lv_anim_path_set_cb(&path, lv_anim_path_ease_in_out);

                lv_anim_t a;
                lv_anim_init(&a);
                lv_anim_set_var(&a, spinner);
                lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_spinner_anim_cb);
                lv_anim_set_path(&a, &path);
                lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
                lv_anim_set_time(&a, ext->time);
                if(ext->anim_dir == LV_SPINNER_DIR_FORWARD) lv_anim_set_values(&a, 0, 360);
                else lv_anim_set_values(&a, 360, 0);
                lv_anim_start(&a);

                lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_spinner_set_arc_length);
                if(ext->anim_dir == LV_SPINNER_DIR_FORWARD) lv_anim_set_values(&a, ext->arc_length, 360 - ext->arc_length);
                else lv_anim_set_values(&a, 360 - ext->arc_length, ext->arc_length);

                lv_anim_set_playback_time(&a, ext->time);
                lv_anim_start(&a);
                break;
            }
        case LV_SPINNER_TYPE_CONSTANT_ARC:
        case LV_SPINNER_TYPE_SPINNING_ARC:
        default: {
                ext->anim_type = type;

                lv_anim_path_t path;
                lv_anim_path_init(&path);
                lv_anim_path_set_cb(&path, (LV_SPINNER_TYPE_CONSTANT_ARC == type ? lv_anim_path_linear : lv_anim_path_ease_in_out));

                lv_anim_t a;
                lv_anim_init(&a);
                lv_anim_set_var(&a, spinner);
                lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_spinner_anim_cb);
                lv_anim_set_time(&a, ext->time);
                lv_anim_set_path(&a, &path);
                lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
                if(ext->anim_dir == LV_SPINNER_DIR_FORWARD) lv_anim_set_values(&a, 0, 360);
                else lv_anim_set_values(&a, 360, 0);
                lv_anim_start(&a);
                break;
            }
    }
}

void lv_spinner_set_dir(lv_obj_t * spinner, lv_spinner_dir_t dir)
{
    LV_ASSERT_OBJ(spinner, LV_OBJX_NAME);

    lv_spinner_ext_t * ext = lv_obj_get_ext_attr(spinner);

    ext->anim_dir = dir;
    lv_spinner_set_type(spinner, ext->anim_type);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the arc length [degree] of the a spinner
 * @param spinner pointer to a spinner object
 */
lv_anim_value_t lv_spinner_get_arc_length(const lv_obj_t * spinner)
{
    LV_ASSERT_OBJ(spinner, LV_OBJX_NAME);

    lv_spinner_ext_t * ext = lv_obj_get_ext_attr(spinner);
    return ext->arc_length;
}

/**
 * Get the spin time of the arc
 * @param spinner pointer to a spinner object [milliseconds]
 */
uint16_t lv_spinner_get_spin_time(const lv_obj_t * spinner)
{
    LV_ASSERT_OBJ(spinner, LV_OBJX_NAME);

    lv_spinner_ext_t * ext = lv_obj_get_ext_attr(spinner);
    return ext->time;
}

/**
 * Get the animation type of a spinner.
 * @param spinner pointer to spinner object
 * @return animation type
 *  */
lv_spinner_type_t lv_spinner_get_type(lv_obj_t * spinner)
{
    LV_ASSERT_OBJ(spinner, LV_OBJX_NAME);

    lv_spinner_ext_t * ext = lv_obj_get_ext_attr(spinner);
    return ext->anim_type;
}

lv_spinner_dir_t lv_spinner_get_dir(lv_obj_t * spinner)
{
    lv_spinner_ext_t * ext = lv_obj_get_ext_attr(spinner);
    return ext->anim_dir;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Animator function  (exec_cb) to rotate the arc of spinner.
 * @param ptr pointer to spinner
 * @param val the current desired value [0..360]
 */
void lv_spinner_anim_cb(void * ptr, lv_anim_value_t val)
{
    lv_obj_t * spinner     = ptr;
    lv_spinner_ext_t * ext = lv_obj_get_ext_attr(spinner);

    int16_t angle_start = val - ext->arc_length / 2 - 90;
    if(angle_start < 0) angle_start += 360;
    int16_t angle_end   = angle_start + ext->arc_length;

    angle_start = angle_start % 360;
    angle_end   = angle_end % 360;

    lv_arc_set_angles(spinner, angle_start, angle_end);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the spinner
 * @param spinner pointer to a spinner object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_spinner_signal(lv_obj_t * spinner, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(spinner, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }

    return res;
}

#endif
