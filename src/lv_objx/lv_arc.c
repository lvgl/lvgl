/**
 * @file lv_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_arc.h"
#if LV_USE_ARC != 0

#include "../lv_core/lv_debug.h"
#include "../lv_misc/lv_math.h"
#include "../lv_draw/lv_draw_arc.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_arc"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_arc_design(lv_obj_t * arc, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_arc_signal(lv_obj_t * arc, lv_signal_t sign, void * param);
static lv_style_list_t * lv_arc_get_style(lv_obj_t * arc, uint8_t part);

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
 * Create a arc object
 * @param par pointer to an object, it will be the parent of the new arc
 * @param copy pointer to a arc object, if not NULL then the new object will be copied from it
 * @return pointer to the created arc
 */
lv_obj_t * lv_arc_create(lv_obj_t * par, const lv_obj_t * copy)
{

    LV_LOG_TRACE("arc create started");

    /*Create the ancestor of arc*/
    lv_obj_t * new_arc = lv_obj_create(par, copy);
    LV_ASSERT_MEM(new_arc);
    if(new_arc == NULL) return NULL;

    /*Allocate the arc type specific extended data*/
    lv_arc_ext_t * ext = lv_obj_allocate_ext_attr(new_arc, sizeof(lv_arc_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(new_arc);
        return NULL;
    }

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_arc);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(new_arc);

    /*Initialize the allocated 'ext' */
    ext->angle_start = 45;
    ext->angle_end   = 315;

    lv_obj_set_size(new_arc, LV_DPI, LV_DPI);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_arc, lv_arc_signal);
    lv_obj_set_design_cb(new_arc, lv_arc_design);

    /*Init the new arc arc*/
    if(copy == NULL) {
        lv_style_list_init(&ext->style_arc);
        lv_style_list_reset(&new_arc->style_list);
        _ot(new_arc, LV_ARC_PART_BG, ARC_BG);
        _ot(new_arc, LV_ARC_PART_ARC, ARC);

    }
    /*Copy an existing arc*/
    else {
        lv_arc_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->angle_start        = copy_ext->angle_start;
        ext->angle_end          = copy_ext->angle_end;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_arc);
    }

    LV_LOG_INFO("arc created");

    return new_arc;
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

/**
 * Set the start angle of an arc. 0 deg: right, 90 bottom: right etc.
 * @param arc pointer to an arc object
 * @param start the start angle [0..360]
 */
void lv_arc_set_start_angle(lv_obj_t * arc, int16_t start)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    if(start > 360) start -= 360;
    if(start < 0) start += 360;

    ext->angle_start = start;

    lv_obj_invalidate(arc);
}

/**
 * Set the start angle of an arc. 0 deg: right, 90 bottom: right etc.
 * @param arc pointer to an arc object
 * @param start the start angle [0..360]
 */
void lv_arc_set_end_angle(lv_obj_t * arc, int16_t end)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    if(end > 360) end -= 360;
    if(end < 0) end += 360;

    ext->angle_end= end;

    lv_obj_invalidate(arc);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the start angle of an arc.
 * @param arc pointer to an arc object
 * @return the start angle [0..360]
 */
uint16_t lv_arc_get_angle_start(lv_obj_t * arc)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    return ext->angle_start;
}

/**
 * Get the end angle of an arc.
 * @param arc pointer to an arc object
 * @return the end angle [0..360]
 */
uint16_t lv_arc_get_angle_end(lv_obj_t * arc)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    return ext->angle_end;
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
 * Handle the drawing related tasks of the arcs
 * @param arc pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_arc_design(lv_obj_t * arc, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return LV_DESIGN_RES_NOT_COVER;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_arc_ext_t * ext       = lv_obj_get_ext_attr(arc);

        lv_draw_rect_dsc_t arc_bg_dsc;
        lv_draw_rect_dsc_init(&arc_bg_dsc);
        lv_obj_init_draw_rect_dsc(arc, LV_ARC_PART_BG, &arc_bg_dsc);
        arc_bg_dsc.radius = LV_RADIUS_CIRCLE;
        lv_draw_rect(&arc->coords, clip_area, &arc_bg_dsc);

        lv_draw_line_dsc_t arc_dsc;
        lv_draw_line_dsc_init(&arc_dsc);
        lv_obj_init_draw_line_dsc(arc, LV_ARC_PART_ARC, &arc_dsc);

        lv_coord_t r       = (LV_MATH_MIN(lv_obj_get_width(arc), lv_obj_get_height(arc))) / 2;
        lv_coord_t x       = arc->coords.x1 + lv_obj_get_width(arc) / 2;
        lv_coord_t y       = arc->coords.y1 + lv_obj_get_height(arc) / 2;
        lv_draw_arc(x, y, r, ext->angle_start, ext->angle_end, clip_area, &arc_dsc);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
    }

    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the arc
 * @param arc pointer to a arc object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_arc_signal(lv_obj_t * arc, lv_signal_t sign, void * param)
{
    lv_res_t res;
    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_arc_get_style(arc, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(arc, sign, param);
    }

    /* Include the ancient signal function */
    res = ancestor_signal(arc, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }

    return res;
}

/**
 * Get the style descriptor of a part of the object
 * @param arc pointer the object
 * @param part the part of the object. (LV_ARC_PART_...)
 * @return pointer to the style descriptor of the specified part
 */
static lv_style_list_t * lv_arc_get_style(lv_obj_t * arc, uint8_t part)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    lv_style_list_t * style_dsc_p;

    switch(part) {
    case LV_ARC_PART_BG:
        style_dsc_p = &arc->style_list;
        break;
    case LV_ARC_PART_ARC:
        style_dsc_p = &ext->style_arc;
        break;
    default:
        style_dsc_p = NULL;
    }

    return style_dsc_p;
}
#endif
