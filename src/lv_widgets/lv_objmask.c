/**
 * @file lv_objmask.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_objmask.h"
#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"

#if defined(LV_USE_OBJMASK) && LV_USE_OBJMASK != 0

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_objmask"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_objmask_design(lv_obj_t * objmask, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_objmask_signal(lv_obj_t * objmask, lv_signal_t sign, void * param);
static uint16_t get_param_size(lv_draw_mask_type_t type);

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
    lv_obj_t * objmask = lv_cont_create(par, copy);
    LV_ASSERT_MEM(objmask);
    if(objmask == NULL) return NULL;

    /*Allocate the object mask type specific extended data*/
    lv_objmask_ext_t * ext = lv_obj_allocate_ext_attr(objmask, sizeof(lv_objmask_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(objmask);
        return NULL;
    }

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(objmask);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(objmask);

    /*Initialize the allocated 'ext' */
    _lv_ll_init(&ext->mask_ll, sizeof(lv_objmask_mask_t));

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(objmask, lv_objmask_signal);
    lv_obj_set_design_cb(objmask, lv_objmask_design);

    /*Init the new object mask object mask*/
    if(copy == NULL) {
        lv_theme_apply(objmask, LV_THEME_OBJMASK);

    }
    /*TODO: Copy an existing object mask*/
    else {
        /* lv_objmask_ext_t * copy_ext = lv_obj_get_ext_attr(copy); */

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(objmask, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    LV_LOG_INFO("object mask created");

    return objmask;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add a mask
 * @param objmask pointer to an Object mask object
 * @param param an initialized mask parameter
 * @return pointer to the added mask
 */
lv_objmask_mask_t * lv_objmask_add_mask(lv_obj_t * objmask, void * param)
{
    LV_ASSERT_OBJ(objmask, LV_OBJX_NAME);
    LV_ASSERT_NULL(param);

    lv_objmask_ext_t * ext = lv_obj_get_ext_attr(objmask);

    lv_draw_mask_common_dsc_t * dsc = param;
    uint16_t param_size = get_param_size(dsc->type);

    lv_objmask_mask_t * m = _lv_ll_ins_head(&ext->mask_ll);
    LV_ASSERT_MEM(m);
    if(m == NULL) return NULL;
    m->param = lv_mem_alloc(param_size);
    LV_ASSERT_MEM(m->param);
    if(m->param == NULL) return NULL;

    _lv_memcpy(m->param, param, param_size);

    lv_obj_invalidate(objmask);

    return m;
}

/**
 * Update an already created mask
 * @param objmask pointer to an Object mask object
 * @param mask pointer to created mask (returned by `lv_objmask_add_mask`)
 * @param param an initialized mask parameter (initialized by `lv_draw_mask_line/angle/.../_init`)
 */
void lv_objmask_update_mask(lv_obj_t * objmask, lv_objmask_mask_t * mask, void * param)
{
    LV_ASSERT_OBJ(objmask, LV_OBJX_NAME);
    LV_ASSERT_NULL(mask);
    LV_ASSERT_NULL(param);
    lv_draw_mask_common_dsc_t * dsc = param;

    memcpy(mask->param, param, get_param_size(dsc->type));

    lv_obj_invalidate(objmask);
}

/**
 * Remove a mask
 * @param objmask pointer to an Object mask object
 * @param mask pointer to created mask (returned by `lv_objmask_add_mask`)
 * If `NULL` passed all masks will be deleted.
 */
void lv_objmask_remove_mask(lv_obj_t * objmask, lv_objmask_mask_t * mask)
{
    LV_ASSERT_OBJ(objmask, LV_OBJX_NAME);

    lv_objmask_ext_t * ext = lv_obj_get_ext_attr(objmask);

    /*Remove all masks*/
    if(mask == NULL) {
        lv_objmask_mask_t * m;
        _LV_LL_READ(ext->mask_ll, m) {
            lv_mem_free(m->param);
        }

        _lv_ll_clear(&ext->mask_ll);
    }
    /*Remove only the specified mask*/
    else {
        lv_mem_free(mask->param);
        _lv_ll_remove(&ext->mask_ll, mask);
        lv_mem_free(mask);
    }

    lv_obj_invalidate(objmask);
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
        if(_lv_ll_get_len(&ext->mask_ll) > 0) return LV_DESIGN_RES_MASKED;
        else return ancestor_design(objmask, clip_area, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_design(objmask, clip_area, mode);

        lv_objmask_ext_t * ext = lv_obj_get_ext_attr(objmask);

        lv_coord_t xofs = objmask->coords.x1;
        lv_coord_t yofs = objmask->coords.y1;

        lv_objmask_mask_t * m;

        _LV_LL_READ(ext->mask_ll, m) {
            lv_draw_mask_common_dsc_t * dsc = m->param;

            if(dsc->type == LV_DRAW_MASK_TYPE_LINE) {
                lv_draw_mask_line_param_t * p_ori = m->param;
                lv_draw_mask_line_param_t * p_new = _lv_mem_buf_get(sizeof(lv_draw_mask_line_param_t));

                lv_draw_mask_line_points_init(p_new, p_ori->cfg.p1.x + xofs, p_ori->cfg.p1.y + yofs,
                                              p_ori->cfg.p2.x + xofs, p_ori->cfg.p2.y + yofs,
                                              p_ori->cfg.side);
                lv_draw_mask_add(p_new, m->param);
            }
            else if(dsc->type == LV_DRAW_MASK_TYPE_ANGLE) {
                lv_draw_mask_angle_param_t * p_ori = m->param;
                lv_draw_mask_angle_param_t * p_new = _lv_mem_buf_get(sizeof(lv_draw_mask_angle_param_t));

                lv_draw_mask_angle_init(p_new, p_ori->cfg.vertex_p.x + xofs, p_ori->cfg.vertex_p.y + yofs,
                                        p_ori->cfg.start_angle, p_ori->cfg.end_angle);
                lv_draw_mask_add(p_new, m->param);
            }
            else if(dsc->type == LV_DRAW_MASK_TYPE_RADIUS) {
                lv_draw_mask_radius_param_t * p_ori = m->param;
                lv_draw_mask_radius_param_t * p_new = _lv_mem_buf_get(sizeof(lv_draw_mask_radius_param_t));

                lv_area_t rect;
                rect.x1 = p_ori->cfg.rect.x1 + xofs;
                rect.y1 = p_ori->cfg.rect.y1 + yofs;
                rect.x2 = p_ori->cfg.rect.x2 + xofs;
                rect.y2 = p_ori->cfg.rect.y2 + yofs;

                lv_draw_mask_radius_init(p_new, &rect, p_ori->cfg.radius, p_ori->cfg.outer);
                lv_draw_mask_add(p_new, m->param);
            }
            else if(dsc->type == LV_DRAW_MASK_TYPE_FADE) {
                lv_draw_mask_fade_param_t * p_ori = m->param;
                lv_draw_mask_fade_param_t * p_new = _lv_mem_buf_get(sizeof(lv_draw_mask_fade_param_t));

                lv_area_t rect;
                rect.x1 = p_ori->cfg.coords.x1 + xofs;
                rect.y1 = p_ori->cfg.coords.y1 + yofs;
                rect.x2 = p_ori->cfg.coords.x2 + xofs;
                rect.y2 = p_ori->cfg.coords.y2 + yofs;

                lv_draw_mask_fade_init(p_new, &rect, p_ori->cfg.opa_top, p_ori->cfg.y_top + yofs,
                                       p_ori->cfg.opa_bottom, p_ori->cfg.y_bottom + yofs);
                lv_draw_mask_add(p_new, m->param);
            }
            else if(dsc->type == LV_DRAW_MASK_TYPE_MAP) {
                lv_draw_mask_map_param_t * p_ori = m->param;
                lv_draw_mask_map_param_t * p_new = _lv_mem_buf_get(sizeof(lv_draw_mask_map_param_t));

                lv_area_t rect;
                rect.x1 = p_ori->cfg.coords.x1 + xofs;
                rect.y1 = p_ori->cfg.coords.y1 + yofs;
                rect.x2 = p_ori->cfg.coords.x2 + xofs;
                rect.y2 = p_ori->cfg.coords.y2 + yofs;

                lv_draw_mask_map_init(p_new, &rect, p_ori->cfg.map);
                lv_draw_mask_add(p_new, m->param);
            }


        }
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        lv_objmask_ext_t * ext = lv_obj_get_ext_attr(objmask);


        lv_objmask_mask_t * m;

        _LV_LL_READ(ext->mask_ll, m) {
            void * param;
            param = lv_draw_mask_remove_custom(m->param);
            _lv_mem_buf_release(param);
        }

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
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_CLEANUP) {
        lv_objmask_ext_t * ext = lv_obj_get_ext_attr(objmask);
        lv_objmask_mask_t * i;
        _LV_LL_READ(ext->mask_ll, i) {
            if(i->param) {
                lv_mem_free(i->param);
                i->param = NULL;
            }
        }

        _lv_ll_clear(&ext->mask_ll);
    }

    return res;
}

static uint16_t get_param_size(lv_draw_mask_type_t type)
{
    uint16_t param_size;
    switch(type) {
        case LV_DRAW_MASK_TYPE_LINE:
            param_size = sizeof(lv_draw_mask_line_param_t);
            break;
        case LV_DRAW_MASK_TYPE_ANGLE:
            param_size = sizeof(lv_draw_mask_angle_param_t);
            break;
        case LV_DRAW_MASK_TYPE_RADIUS:
            param_size = sizeof(lv_draw_mask_radius_param_t);
            break;
        case LV_DRAW_MASK_TYPE_FADE:
            param_size = sizeof(lv_draw_mask_fade_param_t);
            break;
        case LV_DRAW_MASK_TYPE_MAP:
            param_size = sizeof(lv_draw_mask_map_param_t);
            break;
        default:
            param_size = 0;
    }

    return param_size;
}

#else /* Enable this file at the top */

#endif
