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
static void inv_arc_area(lv_obj_t * arc, uint16_t start_angle, uint16_t end_angle);

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
    lv_obj_t * arc = lv_obj_create(par, copy);
    LV_ASSERT_MEM(arc);
    if(arc == NULL) return NULL;

    /*Allocate the arc type specific extended data*/
    lv_arc_ext_t * ext = lv_obj_allocate_ext_attr(arc, sizeof(lv_arc_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(arc);
        return NULL;
    }

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(arc);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(arc);

    /*Initialize the allocated 'ext' */
    ext->bg_angle_start = 135;
    ext->bg_angle_end   = 45;
    ext->arc_angle_start = 135;
    ext->arc_angle_end   = 270;
    lv_style_list_init(&ext->style_arc);


    lv_obj_set_size(arc, LV_DPI, LV_DPI);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(arc, lv_arc_signal);
    lv_obj_set_design_cb(arc, lv_arc_design);

    /*Init the new arc arc*/
    if(copy == NULL) {
        lv_theme_apply(arc, LV_THEME_ARC);
    }
    /*Copy an existing arc*/
    else {
        lv_arc_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->arc_angle_start        = copy_ext->arc_angle_start;
        ext->arc_angle_end          = copy_ext->arc_angle_end;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(arc);
    }

    LV_LOG_INFO("arc created");

    return arc;
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

    /*Too large move, the whole arc need to be invalidated anyway*/
    if(LV_MATH_ABS(start - ext->arc_angle_start) >= 180) {
        lv_obj_invalidate(arc);
    }
    /*Only a smaller incremental move*/
    else if(ext->arc_angle_start > ext->arc_angle_end && start > ext->arc_angle_end) {
        inv_arc_area(arc, LV_MATH_MIN(ext->arc_angle_start, start), LV_MATH_MAX(ext->arc_angle_start, start));
    }
    /*Only a smaller incremental move*/
    else  if(ext->arc_angle_start < ext->arc_angle_end && start < ext->arc_angle_end) {
        inv_arc_area(arc, LV_MATH_MIN(ext->arc_angle_start, start), LV_MATH_MAX(ext->arc_angle_start, start));
    }
    /*Crossing the start angle makes the whole arc change*/
    else {
        lv_obj_invalidate(arc);
    }

    ext->arc_angle_start = start;
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

    /*Too large move, the whole arc need to be invalidated anyway*/
    if(LV_MATH_ABS(end - ext->arc_angle_end) >= 180) {
        lv_obj_invalidate(arc);
    }
    /*Only a smaller incremental move*/
    else if(ext->arc_angle_end > ext->arc_angle_start && end > ext->arc_angle_start ) {
        inv_arc_area(arc, LV_MATH_MIN(ext->arc_angle_end, end), LV_MATH_MAX(ext->arc_angle_end, end));
    }
    /*Only a smaller incremental move*/
    else  if(ext->arc_angle_end < ext->arc_angle_start && end < ext->arc_angle_start ) {
        inv_arc_area(arc, LV_MATH_MIN(ext->arc_angle_end, end), LV_MATH_MAX(ext->arc_angle_end, end));
    }
    /*Crossing the end angle makes the whole arc change*/
    else {
        lv_obj_invalidate(arc);
    }

    ext->arc_angle_end= end;
}


/**
 * Set the start and end angles
 * @param arc pointer to an arc object
 * @param start the start angle
 * @param end the end angle
 */
void lv_arc_set_angles(lv_obj_t * arc, uint16_t start, uint16_t end)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    if(end > 360) end -= 360;
    if(start > 360) start -= 360;

    inv_arc_area(arc, ext->arc_angle_start, ext->arc_angle_end);

    ext->arc_angle_start = start;
    ext->arc_angle_end = end;

    inv_arc_area(arc, ext->arc_angle_start, ext->arc_angle_end);
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

    return ext->arc_angle_start;
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

    return ext->arc_angle_end;
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

        lv_coord_t r       = (LV_MATH_MIN(lv_obj_get_width(arc), lv_obj_get_height(arc))) / 2;
        lv_coord_t x       = arc->coords.x1 + lv_obj_get_width(arc) / 2;
        lv_coord_t y       = arc->coords.y1 + lv_obj_get_height(arc) / 2;

        lv_draw_line_dsc_t arc_dsc;
        lv_draw_line_dsc_init(&arc_dsc);
        lv_obj_init_draw_line_dsc(arc, LV_ARC_PART_BG, &arc_dsc);

        lv_draw_arc(x, y, r, ext->bg_angle_start, ext->bg_angle_end, clip_area, &arc_dsc);

        lv_draw_line_dsc_init(&arc_dsc);
        lv_obj_init_draw_line_dsc(arc, LV_ARC_PART_ARC, &arc_dsc);

        lv_draw_arc(x, y, r, ext->arc_angle_start, ext->arc_angle_end, clip_area, &arc_dsc);
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

static void inv_arc_area(lv_obj_t * arc, uint16_t start_angle, uint16_t end_angle)
{
    uint8_t start_quarter = start_angle / 90;
    uint8_t end_quarter = end_angle / 90;
    lv_coord_t x       = arc->coords.x1 + lv_obj_get_width(arc) / 2;
    lv_coord_t y       = arc->coords.y1 + lv_obj_get_height(arc) / 2;
    lv_coord_t rout       = (LV_MATH_MIN(lv_obj_get_width(arc), lv_obj_get_height(arc))) / 2;
    lv_style_int_t w = lv_obj_get_style_line_width(arc, LV_ARC_PART_ARC);
    lv_style_int_t rounded = lv_obj_get_style_line_rounded(arc, LV_ARC_PART_ARC);
    lv_coord_t rin       = rout - w;
    lv_coord_t extra_area = rounded ? w / 2 + 2 : 0;

    lv_area_t inv_area;

    if(start_quarter == end_quarter && start_angle <= end_angle) {
        if(start_quarter == 0) {
            inv_area.y1 = y + ((lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.x2 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;

            inv_area.y2 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            inv_area.x1 = x + ((lv_trigo_sin(end_angle + 90) * rin) >> LV_TRIGO_SHIFT) - extra_area;

            lv_obj_invalidate_area(arc, &inv_area);
        }
        else if(start_quarter == 1) {
            inv_area.y2 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            inv_area.x2 = x + ((lv_trigo_sin(start_angle + 90) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            inv_area.y1 = y + ((lv_trigo_sin(end_angle) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.x1 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;

            lv_obj_invalidate_area(arc, &inv_area);
        }
        else if(start_quarter == 2) {
            inv_area.x1 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.y2 = y + ((lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            inv_area.y1 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.x2 = x + ((lv_trigo_sin(end_angle + 90) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            lv_obj_invalidate_area(arc, &inv_area);
        }
        else if(start_quarter == 3) {
            /*Small arc here*/
            inv_area.x1 = x + ((lv_trigo_sin(start_angle + 90) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.y1 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;

            inv_area.x2 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            inv_area.y2 = y + ((lv_trigo_sin(end_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            lv_obj_invalidate_area(arc, &inv_area);
        }

    } else if(start_quarter == 0 && end_quarter == 1) {
        inv_area.x1 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.y1 = y + ((LV_MATH_MIN(lv_trigo_sin(end_angle), lv_trigo_sin(start_angle))  * rin) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.x2 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        inv_area.y2 = y + rout + extra_area;
        lv_obj_invalidate_area(arc, &inv_area);
    } else if(start_quarter == 1 && end_quarter == 2) {
        inv_area.x1 = x - rout - extra_area;
        inv_area.y1 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.x2 = x + ((LV_MATH_MAX(lv_trigo_sin(start_angle + 90) , lv_trigo_sin(end_angle + 90)) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        inv_area.y2 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        lv_obj_invalidate_area(arc, &inv_area);
    }  else if(start_quarter == 2 && end_quarter == 3) {
        inv_area.x1 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.y1 = y - rout - extra_area;
        inv_area.x2 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        inv_area.y2 = y + (LV_MATH_MAX(lv_trigo_sin(end_angle) * rin, lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        lv_obj_invalidate_area(arc, &inv_area);
    } else if(start_quarter == 3 && end_quarter == 0) {
        inv_area.x1 = x + ((LV_MATH_MIN(lv_trigo_sin(end_angle + 90), lv_trigo_sin(start_angle + 90)) * rin) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.y1 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.x2 = x + rout + extra_area;
        inv_area.y2 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;

        lv_obj_invalidate_area(arc, &inv_area);
    } else {
        lv_obj_invalidate(arc);
    }
}
#endif
