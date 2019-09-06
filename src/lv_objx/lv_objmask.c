/**
 * @file lv_objmask.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_objmask.h"

#if defined(LV_USE_OBJMASK) && LV_USE_OBJMASK != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_objmask_design(lv_obj_t * objmask, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_objmask_signal(lv_obj_t * objmask, lv_signal_t sign, void * param);

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
 * Create a object mask object
 * @param par pointer to an object, it will be the parent of the new object mask
 * @param copy pointer to a object mask object, if not NULL then the new object will be copied from it
 * @return pointer to the created object mask
 */
lv_obj_t * lv_objmask_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("object mask create started");

    /*Create the ancestor of object mask*/
    lv_obj_t * new_objmask = lv_cont_create(par, copy);
    lv_mem_assert(new_objmask);
    if(new_objmask == NULL) return NULL;

    /*Allocate the object mask type specific extended data*/
    lv_objmask_ext_t * ext = lv_obj_allocate_ext_attr(new_objmask, sizeof(lv_objmask_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_objmask);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(new_objmask);

    /*Initialize the allocated 'ext' */
    lv_ll_init(&ext->mask_ll, sizeof(lv_objmask_mask_t));

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_objmask, lv_objmask_signal);
    lv_obj_set_design_cb(new_objmask, lv_objmask_design);

    /*Init the new object mask object mask*/
    if(copy == NULL) {
        lv_objmask_set_style(new_objmask, LV_OBJMASK_STYLE_BG, &lv_style_plain);

    }
    /*Copy an existing object mask*/
    else {
        lv_objmask_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_objmask);
    }

    LV_LOG_INFO("object mask created");

    return new_objmask;
}

/*======================
 * Add/remove functions
 *=====================*/

void lv_objmask_add_mask(lv_obj_t * objmask, lv_draw_mask_param_t * param, lv_draw_mask_cb_t mask_cb, uint8_t id)
{
    lv_objmask_ext_t * ext = lv_obj_get_ext_attr(objmask);

    lv_objmask_mask_t * m = lv_ll_ins_head(&ext->mask_ll);

    memcpy(&m->param, param, sizeof(lv_draw_mask_param_t));
    m->mask_cb = mask_cb;
    m->id = id;
}

/*=====================
 * Setter functions
 *====================*/


/*=====================
 * Getter functions
 *====================*/


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
 * Handle the drawing related tasks of the object masks
 * @param objmask pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_objmask_design(lv_obj_t * objmask, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        lv_objmask_ext_t * ext = lv_obj_get_ext_attr(objmask);
        if(lv_ll_get_len(&ext->mask_ll) > 0) return LV_DESIGN_RES_MASKED;
        else return ancestor_design(objmask, clip_area, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_design(objmask, clip_area, mode);

        lv_objmask_ext_t * ext = lv_obj_get_ext_attr(objmask);
        lv_objmask_mask_t * m;

        LV_LL_READ(ext->mask_ll, m) {
            lv_draw_mask_add(m->mask_cb, &m->param, objmask);
        }


    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        lv_draw_mask_remove_custom(objmask);
    }

    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the object mask
 * @param objmask pointer to a object mask object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_objmask_signal(lv_obj_t * objmask, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(objmask, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_objmask";
    }

    return res;
}

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
