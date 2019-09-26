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
static bool lv_arc_design(lv_obj_t * arc, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_arc_signal(lv_obj_t * arc, lv_signal_t sign, void * param);

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
    if(ext == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_arc);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(new_arc);

    /*Initialize the allocated 'ext' */
    ext->angle_start = 45;
    ext->angle_end   = 315;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_arc, lv_arc_signal);
    lv_obj_set_design_cb(new_arc, lv_arc_design);

    /*Init the new arc arc*/
    if(copy == NULL) {
        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_arc_set_style(new_arc, LV_ARC_STYLE_MAIN, th->style.arc);
        } else {
            lv_arc_set_style(new_arc, LV_ARC_STYLE_MAIN, &lv_style_plain_color);
        }

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
 * Set the start and end angles of an arc. 0 deg: bottom, 90 deg: right etc.
 * @param arc pointer to an arc object
 * @param start the start angle [0..360]
 * @param end the end angle [0..360]
 */
void lv_arc_set_angles(lv_obj_t * arc, uint16_t start, uint16_t end)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    if(start > 360) start = 360;
    if(end > 360) end = 360;

    ext->angle_start = start;
    ext->angle_end   = end;

    lv_obj_invalidate(arc);
}

/**
 * Set a style of a arc.
 * @param arc pointer to arc object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_arc_set_style(lv_obj_t * arc, lv_arc_style_t type, const lv_style_t * style)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    switch(type) {
        case LV_ARC_STYLE_MAIN: lv_obj_set_style(arc, style); break;
    }
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

/**
 * Get style of a arc.
 * @param arc pointer to arc object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
const lv_style_t * lv_arc_get_style(const lv_obj_t * arc, lv_arc_style_t type)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    const lv_style_t * style = NULL;

    switch(type) {
        case LV_ARC_STYLE_MAIN: style = lv_obj_get_style(arc); break;
        default: style = NULL; break;
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
 * Handle the drawing related tasks of the arcs
 * @param arc pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_arc_design(lv_obj_t * arc, const lv_area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_arc_ext_t * ext       = lv_obj_get_ext_attr(arc);
        const lv_style_t * style = lv_arc_get_style(arc, LV_ARC_STYLE_MAIN);

        lv_coord_t r       = (LV_MATH_MIN(lv_obj_get_width(arc), lv_obj_get_height(arc))) / 2;
        lv_coord_t x       = arc->coords.x1 + lv_obj_get_width(arc) / 2;
        lv_coord_t y       = arc->coords.y1 + lv_obj_get_height(arc) / 2;
        lv_opa_t opa_scale = lv_obj_get_opa_scale(arc);
        lv_draw_arc(x, y, r, mask, ext->angle_start, ext->angle_end, style, opa_scale);

        /*Draw circle on the ends if enabled */
        if(style->line.rounded) {
            lv_coord_t thick_half = style->line.width / 2;
            lv_coord_t cir_x      = ((r - thick_half) * lv_trigo_sin(ext->angle_start) >> LV_TRIGO_SHIFT);
            lv_coord_t cir_y      = ((r - thick_half) * lv_trigo_sin(ext->angle_start + 90) >> LV_TRIGO_SHIFT);

            lv_style_t cir_style;
            lv_style_copy(&cir_style, &lv_style_plain);
            cir_style.body.grad_color = style->line.color;
            cir_style.body.main_color = cir_style.body.grad_color;
            cir_style.body.radius     = LV_RADIUS_CIRCLE;
            lv_area_t cir_area;
            cir_area.x1 = cir_x + x - thick_half;
            cir_area.y1 = cir_y + y - thick_half;
            cir_area.x2 = cir_x + x + thick_half;
            cir_area.y2 = cir_y + y + thick_half;

            lv_draw_rect(&cir_area, mask, &cir_style, opa_scale);

            cir_x = ((r - thick_half) * lv_trigo_sin(ext->angle_end) >> LV_TRIGO_SHIFT);
            cir_y = ((r - thick_half) * lv_trigo_sin(ext->angle_end + 90) >> LV_TRIGO_SHIFT);

            cir_area.x1 = cir_x + x - thick_half;
            cir_area.y1 = cir_y + y - thick_half;
            cir_area.x2 = cir_x + x + thick_half;
            cir_area.y2 = cir_y + y + thick_half;

            lv_draw_rect(&cir_area, mask, &cir_style, opa_scale);
        }

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
    }

    return true;
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

    /* Include the ancient signal function */
    res = ancestor_signal(arc, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }

    return res;
}

#endif
