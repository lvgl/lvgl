/**
 * @file lv_templ.c
 *
 */

/**
 * TODO Remove these instructions
 * Search an replace: template -> object normal name with lower case (e.g. button, label etc.)
 *                    templ -> object short name with lower case(e.g. btn, label etc)
 *                    TEMPL -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 * You can remove the defined() clause from the #if statement below. This exists because
 * LV_USE_TEMPL is not in lv_conf.h or lv_conf_template.h by default.
 */

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_assert.h"
//#include "lv_templ.h" /*TODO uncomment this*/

#if defined(LV_USE_TEMPL) && LV_USE_TEMPL != 0

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_templ_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_draw_res_t lv_templ_draw(lv_obj_t * templ, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_templ_signal(lv_obj_t * templ, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_draw_cb_t ancestor_draw;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a template object
 * @param par pointer to an object, it will be the parent of the new template
 * @param copy pointer to a template object, if not NULL then the new object will be copied from it
 * @return pointer to the created template
 */
lv_obj_t * lv_templ_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("template create started");

    /*Create the ancestor of template*/
    /*TODO modify it to the ancestor create function*/
    lv_obj_t * new_templ = lv_ANCESTOR_create(par, copy);
    LV_ASSERT_MALLOC(new_templ);
    if(new_templ == NULL) return NULL;

    /*Allocate the template type specific extended data*/
    lv_templ_ext_t * ext = lv_obj_allocate_ext_attr(new_templ, sizeof(lv_templ_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) {
        lv_obj_del(new_templ);
        return NULL;
    }

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_templ);
    if(ancestor_draw == NULL) ancestor_draw = lv_obj_get_draw_cb(new_templ);

    /*Initialize the allocated 'ext'*/
    ext->xyz = 0;

    /*The signal and draw functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_templ, lv_templ_signal);
    lv_obj_set_draw_cb(new_templ, lv_templ_draw);

    /*Init the new template template*/
    if(copy == NULL) {

    }
    /*Copy an existing template*/
    else {
        lv_templ_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_templ);
    }

    LV_LOG_INFO("template created");

    return new_templ;
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

/*
 * New object specific "set" functions come here
 */

/**
 * Set a style of a template.
 * @param templ pointer to template object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_templ_set_style(lv_obj_t * templ, lv_templ_style_t type, const lv_style_t * style)
{
    LV_ASSERT_OBJ(templ, MY_CLASS);

    lv_templ_ext_t * ext = lv_obj_get_ext_attr(templ);

    switch(type) {
        case LV_TEMPL_STYLE_X:
            break;
        case LV_TEMPL_STYLE_Y:
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/*
 * New object specific "get" functions come here
 */

/**
 * Get style of a template.
 * @param templ pointer to template object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_templ_get_style(const lv_obj_t * templ, lv_templ_style_t type)
{
    LV_ASSERT_OBJ(templ, MY_CLASS);

    lv_templ_ext_t * ext = lv_obj_get_ext_attr(templ);
    lv_style_t * style   = NULL;

    switch(type) {
        case LV_TEMPL_STYLE_X:
            style = NULL; /*Replace NULL with a pointer to the style*/
        case LV_TEMPL_STYLE_Y:
            style = NULL; /*Replace NULL with a pointer to the style*/
        default:
            style = NULL;
    }

    return style;
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

/**
 * Handle the drawing related tasks of the templates
 * @param templ pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DRAW_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DRAW_DRAW: draw the object (always return 'true')
 *             LV_DRAW_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_draw_res_t`
 */
static lv_draw_res_t lv_templ_draw(lv_obj_t * templ, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DRAW_COVER_CHK) {
        return LV_COVER_RES_NOT_COVER;
    }
    /*Draw the object*/
    else if(mode == LV_DRAW_DRAW_MAIN) {

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DRAW_DRAW_POST) {
    }

    return LV_COVER_RES_OK;
}

/**
 * Signal function of the template
 * @param templ pointer to a template object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_templ_signal(lv_obj_t * templ, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /*Call the ancestor's event handler*/
    res = ancestor_signal(templ, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, MY_CLASS);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }

    return res;
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
