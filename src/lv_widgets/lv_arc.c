/**
 * @file lv_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_arc.h"
#if LV_USE_ARC != 0

#include "../lv_core/lv_group.h"
#include "../lv_core/lv_indev.h"
#include "../lv_misc/lv_debug.h"
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
static void inv_arc_area(lv_obj_t * arc, uint16_t start_angle, uint16_t end_angle, lv_arc_part_t part);
static void get_center(lv_obj_t * arc, lv_point_t * center, lv_coord_t * arc_r);
static void get_knob_area(lv_obj_t * arc, const lv_point_t * center, lv_coord_t r, lv_area_t * knob_area);
static void value_update(lv_obj_t * arc);

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
    ext->rotation_angle = 0;
    ext->bg_angle_start = 135;
    ext->bg_angle_end   = 45;
    ext->arc_angle_start = 135;
    ext->arc_angle_end   = 270;
    ext->type = LV_ARC_TYPE_NORMAL;
    ext->cur_value = -1;
    ext->min_value = 0;
    ext->max_value = 100;
    ext->dragging = false;
    ext->adjustable = false;
    ext->chg_rate = 540;
    ext->last_tick = lv_tick_get();
    ext->last_angle = ext->arc_angle_end;
    lv_style_list_init(&ext->style_arc);
    lv_style_list_init(&ext->style_knob);

    lv_obj_set_size(arc, LV_DPI, LV_DPI);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(arc, lv_arc_signal);
    lv_obj_set_design_cb(arc, lv_arc_design);

    /*Init the new arc arc*/
    if(copy == NULL) {
        lv_obj_set_click(arc, true);
        lv_obj_add_protect(arc, LV_PROTECT_PRESS_LOST);
        lv_obj_set_ext_click_area(arc, LV_DPI / 10, LV_DPI / 10, LV_DPI / 10, LV_DPI / 10);
        lv_arc_set_value(arc, ext->min_value);
        lv_theme_apply(arc, LV_THEME_ARC);
    }
    /*Copy an existing arc*/
    else {
        lv_arc_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->arc_angle_start = copy_ext->arc_angle_start;
        ext->arc_angle_end   = copy_ext->arc_angle_end;
        ext->bg_angle_start  = copy_ext->bg_angle_start;
        ext->bg_angle_end    = copy_ext->bg_angle_end;
        ext->type = copy_ext->type;
        ext->cur_value = copy_ext->cur_value;
        ext->min_value = copy_ext->min_value;
        ext->max_value = copy_ext->max_value;
        ext->dragging = copy_ext->dragging;
        ext->adjustable = copy_ext->adjustable;
        ext->chg_rate = copy_ext->chg_rate;
        ext->last_tick = copy_ext->last_tick;
        ext->last_angle = copy_ext->last_angle;
        lv_style_list_copy(&ext->style_knob, &copy_ext->style_knob);
        lv_style_list_copy(&ext->style_arc, &copy_ext->style_arc);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(arc, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
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
 * Set the start angle of an arc. 0 deg: right, 90 bottom, etc.
 * @param arc pointer to an arc object
 * @param start the start angle [0..360]
 */
void lv_arc_set_start_angle(lv_obj_t * arc, uint16_t start)
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
        inv_arc_area(arc, LV_MATH_MIN(ext->arc_angle_start, start), LV_MATH_MAX(ext->arc_angle_start, start),
                     LV_ARC_PART_INDIC);
    }
    /*Only a smaller incremental move*/
    else  if(ext->arc_angle_start < ext->arc_angle_end && start < ext->arc_angle_end) {
        inv_arc_area(arc, LV_MATH_MIN(ext->arc_angle_start, start), LV_MATH_MAX(ext->arc_angle_start, start),
                     LV_ARC_PART_INDIC);
    }
    /*Crossing the start angle makes the whole arc change*/
    else {
        lv_obj_invalidate(arc);
    }

    ext->arc_angle_start = start;
}

/**
 * Set the start angle of an arc. 0 deg: right, 90 bottom, etc.
 * @param arc pointer to an arc object
 * @param start the start angle [0..360]
 */
void lv_arc_set_end_angle(lv_obj_t * arc, uint16_t end)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    if(end > (ext->arc_angle_start + 360)) end = ext->arc_angle_start + 360;

    /*Too large move, the whole arc need to be invalidated anyway*/
    if(LV_MATH_ABS(end - ext->arc_angle_end) >= 180) {
        lv_obj_invalidate(arc);
    }
    /*Only a smaller incremental move*/
    else if(ext->arc_angle_end > ext->arc_angle_start && end > ext->arc_angle_start) {
        inv_arc_area(arc, LV_MATH_MIN(ext->arc_angle_end, end), LV_MATH_MAX(ext->arc_angle_end, end), LV_ARC_PART_INDIC);
    }
    /*Only a smaller incremental move*/
    else  if(ext->arc_angle_end < ext->arc_angle_start && end < ext->arc_angle_start) {
        inv_arc_area(arc, LV_MATH_MIN(ext->arc_angle_end, end), LV_MATH_MAX(ext->arc_angle_end, end), LV_ARC_PART_INDIC);
    }
    /*Crossing the end angle makes the whole arc change*/
    else {
        lv_obj_invalidate(arc);
    }

    ext->arc_angle_end = end;
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

    if(start > 360) start -= 360;
    if(end > (start + 360)) end = start + 360;

    inv_arc_area(arc, ext->arc_angle_start, ext->arc_angle_end, LV_ARC_PART_INDIC);

    ext->arc_angle_start = start;
    ext->arc_angle_end = end;

    inv_arc_area(arc, ext->arc_angle_start, ext->arc_angle_end, LV_ARC_PART_INDIC);
}

/**
 * Set the start angle of an arc background. 0 deg: right, 90 bottom, etc.
 * @param arc pointer to an arc object
 * @param start the start angle
 */
void lv_arc_set_bg_start_angle(lv_obj_t * arc, uint16_t start)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    if(start > 360) start -= 360;

    /*Too large move, the whole arc need to be invalidated anyway*/
    if(LV_MATH_ABS(start - ext->bg_angle_start) >= 180) {
        lv_obj_invalidate(arc);
    }
    /*Only a smaller incremental move*/
    else if(ext->bg_angle_start > ext->bg_angle_end && start > ext->bg_angle_end) {
        inv_arc_area(arc, LV_MATH_MIN(ext->bg_angle_start, start), LV_MATH_MAX(ext->bg_angle_start, start), LV_ARC_PART_BG);
    }
    /*Only a smaller incremental move*/
    else  if(ext->bg_angle_start < ext->bg_angle_end && start < ext->bg_angle_end) {
        inv_arc_area(arc, LV_MATH_MIN(ext->bg_angle_start, start), LV_MATH_MAX(ext->bg_angle_start, start), LV_ARC_PART_BG);
    }
    /*Crossing the start angle makes the whole arc change*/
    else {
        lv_obj_invalidate(arc);
    }

    ext->bg_angle_start = start;
}

/**
 * Set the start angle of an arc background. 0 deg: right, 90 bottom etc.
 * @param arc pointer to an arc object
 * @param end the end angle
 */
void lv_arc_set_bg_end_angle(lv_obj_t * arc, uint16_t end)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    if(end > (ext->bg_angle_start + 360)) end = ext->bg_angle_start + 360;

    /*Too large move, the whole arc need to be invalidated anyway*/
    if(LV_MATH_ABS(end - ext->bg_angle_end) >= 180) {
        lv_obj_invalidate(arc);
    }
    /*Only a smaller incremental move*/
    else if(ext->bg_angle_end > ext->bg_angle_start && end > ext->bg_angle_start) {
        inv_arc_area(arc, LV_MATH_MIN(ext->bg_angle_end, end), LV_MATH_MAX(ext->bg_angle_end, end), LV_ARC_PART_BG);
    }
    /*Only a smaller incremental move*/
    else  if(ext->bg_angle_end < ext->bg_angle_start && end < ext->bg_angle_start) {
        inv_arc_area(arc, LV_MATH_MIN(ext->bg_angle_end, end), LV_MATH_MAX(ext->bg_angle_end, end), LV_ARC_PART_BG);
    }
    /*Crossing the end angle makes the whole arc change*/
    else {
        lv_obj_invalidate(arc);
    }

    ext->bg_angle_end = end;
}

/**
 * Set the start and end angles of the arc background
 * @param arc pointer to an arc object
 * @param start the start angle
 * @param end the end angle
 */
void lv_arc_set_bg_angles(lv_obj_t * arc, uint16_t start, uint16_t end)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    if(start > 360) start -= 360;
    if(end > (start + 360)) end = start + 360;

    inv_arc_area(arc, ext->bg_angle_start, ext->bg_angle_end, LV_ARC_PART_BG);

    ext->bg_angle_start = start;
    ext->bg_angle_end = end;

    inv_arc_area(arc, ext->bg_angle_start, ext->bg_angle_end, LV_ARC_PART_BG);
}

/**
 * Set the rotation for the whole arc
 * @param arc pointer to an arc object
 * @param rotation_angle rotation angle
 */
void lv_arc_set_rotation(lv_obj_t * arc, uint16_t rotation_angle)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    ext->rotation_angle = rotation_angle;

    lv_obj_invalidate(arc);
}


/**
 * Set the type of arc.
 * @param arc pointer to arc object
 * @param type arc type
 */
void lv_arc_set_type(lv_obj_t * arc, lv_arc_type_t type)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = (lv_arc_ext_t *)lv_obj_get_ext_attr(arc);
    int16_t val = ext->cur_value;

    ext->type = type;
    ext->cur_value = -1; /** Force set_value handling*/

    int16_t bg_midpoint, bg_end = ext->bg_angle_end;
    if(ext->bg_angle_end < ext->bg_angle_start) bg_end = ext->bg_angle_end + 360;

    switch(ext->type) {
        case LV_ARC_TYPE_SYMMETRIC:
            bg_midpoint = (ext->bg_angle_start + bg_end) / 2;
            lv_arc_set_start_angle(arc, bg_midpoint);
            lv_arc_set_end_angle(arc, bg_midpoint);
            break;
        case LV_ARC_TYPE_REVERSE:
            lv_arc_set_end_angle(arc, ext->bg_angle_end);
            break;
        default: /** LV_ARC_TYPE_NORMAL*/
            lv_arc_set_start_angle(arc, ext->bg_angle_start);
    }

    lv_arc_set_value(arc, val);
}

/**
 * Set a new value on the arc
 * @param arc pointer to a arc object
 * @param value new value
 */
void lv_arc_set_value(lv_obj_t * arc, int16_t value)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = (lv_arc_ext_t *)lv_obj_get_ext_attr(arc);
    if(ext->cur_value == value) return;

    int16_t new_value;
    new_value = value > ext->max_value ? ext->max_value : value;
    new_value = new_value < ext->min_value ? ext->min_value : new_value;

    if(ext->cur_value == new_value) return;
    ext->cur_value = new_value;

    value_update(arc);
}

/**
 * Set minimum and the maximum values of a arc
 * @param arc pointer to the arc object
 * @param min minimum value
 * @param max maximum value
 */
void lv_arc_set_range(lv_obj_t * arc, int16_t min, int16_t max)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = (lv_arc_ext_t *)lv_obj_get_ext_attr(arc);
    if(ext->min_value == min && ext->max_value == max) return;

    ext->min_value = min;
    ext->max_value = max;

    if(ext->cur_value < min) {
        ext->cur_value = min;
    }
    if(ext->cur_value > max) {
        ext->cur_value = max;
    }

    value_update(arc); /* value has changed relative to the new range */
}

/**
 * Set the threshold of arc knob increments
 * position.
 * @param arc pointer to a arc object
 * @param threshold increment threshold
 */
void lv_arc_set_chg_rate(lv_obj_t * arc, uint16_t rate)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = (lv_arc_ext_t *)lv_obj_get_ext_attr(arc);
    ext->chg_rate = rate;
}

/**
 * Set whether the arc is adjustable.
 * @param arc pointer to a arc object
 * @param adjustable whether the arc has a knob that can be dragged
 */
void lv_arc_set_adjustable(lv_obj_t * arc, bool adjustable)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = (lv_arc_ext_t *)lv_obj_get_ext_attr(arc);
    if(ext->adjustable == adjustable)
        return;

    ext->adjustable = adjustable;
    if(!adjustable)
        ext->dragging = false;
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

/**
 * Get the start angle of an arc background.
 * @param arc pointer to an arc object
 * @return the start angle [0..360]
 */
uint16_t lv_arc_get_bg_angle_start(lv_obj_t * arc)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    return ext->bg_angle_start;
}

/**
 * Get the end angle of an arc background.
 * @param arc pointer to an arc object
 * @return the end angle [0..360]
 */
uint16_t lv_arc_get_bg_angle_end(lv_obj_t * arc)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    return ext->bg_angle_end;
}


/**
 * Get the value of a arc
 * @param arc pointer to a arc object
 * @return the value of the arc
 */
int16_t lv_arc_get_value(const lv_obj_t * arc)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);
    return ext->cur_value;
}

/**
 * Get the minimum value of a arc
 * @param arc pointer to a arc object
 * @return the minimum value of the arc
 */
int16_t lv_arc_get_min_value(const lv_obj_t * arc)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = (lv_arc_ext_t *)lv_obj_get_ext_attr(arc);
    return ext->min_value;
}

/**
 * Get the maximum value of a arc
 * @param arc pointer to a arc object
 * @return the maximum value of the arc
 */
int16_t lv_arc_get_max_value(const lv_obj_t * arc)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = (lv_arc_ext_t *)lv_obj_get_ext_attr(arc);
    return ext->max_value;
}

/**
 * Give the arc is being dragged or not
 * @param arc pointer to a arc object
 * @return true: drag in progress false: not dragged
 */
bool lv_arc_is_dragged(const lv_obj_t * arc)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);
    return ext->dragging;
}

/**
 * Get whether the arc is type or not.
 * @param arc pointer to a arc object
 * @return arc type
 */
lv_arc_type_t lv_arc_get_type(const lv_obj_t * arc)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = (lv_arc_ext_t *)lv_obj_get_ext_attr(arc);
    return ext->type;
}

/**
 * Get whether the arc is adjustable.
 * @param arc pointer to a arc object
 * @return whether the arc has a knob that can be dragged
 */
bool lv_arc_get_adjustable(lv_obj_t * arc)
{
    LV_ASSERT_OBJ(arc, LV_OBJX_NAME);

    lv_arc_ext_t * ext = (lv_arc_ext_t *)lv_obj_get_ext_attr(arc);
    return ext->adjustable;
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

        lv_draw_rect_dsc_t bg_dsc;
        lv_draw_rect_dsc_init(&bg_dsc);
        lv_obj_init_draw_rect_dsc(arc, LV_ARC_PART_BG, &bg_dsc);

        lv_draw_rect(&arc->coords, clip_area, &bg_dsc);

        lv_point_t center;
        lv_coord_t arc_r;
        get_center(arc, &center, &arc_r);

        /*Draw the background arc*/
        lv_draw_line_dsc_t arc_dsc;
        if(arc_r > 0) {
            lv_draw_line_dsc_init(&arc_dsc);
            lv_obj_init_draw_line_dsc(arc, LV_ARC_PART_BG, &arc_dsc);

            lv_draw_arc(center.x, center.y, arc_r, ext->bg_angle_start + ext->rotation_angle,
                        ext->bg_angle_end + ext->rotation_angle, clip_area,
                        &arc_dsc);
        }


        /*make the indicator arc smaller or larger according to its greatest padding value*/
        lv_coord_t left_indic = lv_obj_get_style_pad_left(arc, LV_ARC_PART_INDIC);
        lv_coord_t right_indic = lv_obj_get_style_pad_right(arc, LV_ARC_PART_INDIC);
        lv_coord_t top_indic = lv_obj_get_style_pad_top(arc, LV_ARC_PART_INDIC);
        lv_coord_t bottom_indic = lv_obj_get_style_pad_bottom(arc, LV_ARC_PART_INDIC);
        lv_coord_t indic_r = arc_r - LV_MATH_MAX4(left_indic, right_indic, top_indic, bottom_indic);

        if(indic_r > 0) {
            lv_draw_line_dsc_init(&arc_dsc);
            lv_obj_init_draw_line_dsc(arc, LV_ARC_PART_INDIC, &arc_dsc);

            lv_draw_arc(center.x, center.y, indic_r, ext->arc_angle_start + ext->rotation_angle,
                        ext->arc_angle_end + ext->rotation_angle, clip_area,
                        &arc_dsc);
        }

        if(ext->adjustable) {
            lv_area_t knob_area;
            get_knob_area(arc, &center, arc_r, &knob_area);

            lv_draw_rect_dsc_t knob_rect_dsc;
            lv_draw_rect_dsc_init(&knob_rect_dsc);
            lv_obj_init_draw_rect_dsc(arc, LV_ARC_PART_KNOB, &knob_rect_dsc);

            lv_draw_rect(&knob_area, clip_area, &knob_rect_dsc);
        }

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

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    if(sign == LV_SIGNAL_PRESSING) {
        /* Only adjustable arcs can be dragged */
        if(!ext->adjustable) return res;

        lv_indev_t * indev = lv_indev_get_act();
        if(indev == NULL) return res;

        /*Handle only pointers here*/
        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if(indev_type != LV_INDEV_TYPE_POINTER) return res;

        lv_point_t p;
        lv_indev_get_point(indev, &p);

        /*Make point relative to the arc's center*/
        lv_point_t center;
        lv_coord_t r;
        get_center(arc, &center, &r);

        p.x -=  center.x;
        p.y -=  center.y;

        /*Enter dragging mode if pressed out of the knob*/
        if(ext->dragging == false) {
            lv_coord_t indic_width = lv_obj_get_style_line_width(arc, LV_ARC_PART_INDIC);
            r -=  indic_width;
            r -= r / 2; /*Add some more sensitive area*/
            if(p.x * p.x + p.y * p.y > r * r) {
                ext->dragging = true;
                ext->last_tick = lv_tick_get(); /*Capture timestamp at dragging start*/
            }
        }

        /*It must be in "dragging" mode to turn the arc*/
        if(ext->dragging == false) return res;

        /*Calculate the angle of the pressed point*/
        int16_t angle;
        int16_t bg_end = ext->bg_angle_end;
        if(ext->bg_angle_end < ext->bg_angle_start) {
            bg_end = ext->bg_angle_end + 360;
        }

        angle = 360 - _lv_atan2(p.x, p.y) + 90; /*Some transformation is required*/
        angle -= ext->rotation_angle;
        if(angle < ext->bg_angle_start) angle = ext->bg_angle_start;
        if(angle > bg_end) angle = bg_end;

        /*Calculate the slew rate limited angle based on change rate (degrees/sec)*/
        int16_t delta_angle = angle - ext->last_angle;
        uint32_t delta_tick = lv_tick_elaps(ext->last_tick);
        int16_t delta_angle_max = (ext->chg_rate * delta_tick) / 1000;

        if(delta_angle > delta_angle_max) {
            delta_angle = delta_angle_max;
        }
        else if(delta_angle < -delta_angle_max) {
            delta_angle = -delta_angle_max;
        }

        angle = ext->last_angle + delta_angle; /*Apply the limited angle change*/

        /*Rounding for symmetry*/
        int32_t round = ((bg_end - ext->bg_angle_start) * 8) / (ext->max_value - ext->min_value);
        round = (round + 4) >> 4;
        angle += round;

        /*Set the new value*/
        int16_t old_value = ext->cur_value;
        int16_t new_value = _lv_map(angle, ext->bg_angle_start, bg_end, ext->min_value, ext->max_value);
        if(new_value != lv_arc_get_value(arc)) {
            ext->last_tick = lv_tick_get(); /*Cache timestamp for the next iteration*/
            lv_arc_set_value(arc, new_value); /*set_value caches the last_angle for the next iteration*/
            if(new_value != old_value) {
                res = lv_event_send(arc, LV_EVENT_VALUE_CHANGED, NULL);
                if(res != LV_RES_OK) return res;
            }
        }

        /*Don1't let the elapsed time to big while sitting on an end point*/
        if(new_value == ext->min_value || new_value == ext->max_value) {
            ext->last_tick = lv_tick_get(); /*Cache timestamp for the next iteration*/
        }
    }
    else if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
        ext->dragging = false;

#if LV_USE_GROUP
        /*Leave edit mode if released. (No need to wait for LONG_PRESS) */
        lv_group_t * g             = lv_obj_get_group(arc);
        bool editing               = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            if(editing) lv_group_set_editing(g, false);
        }
#endif

    }
    else if(sign == LV_SIGNAL_CONTROL) {
        if(!ext->adjustable) return res;

        char c = *((char *)param);

        int16_t old_value = ext->cur_value;
        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            lv_arc_set_value(arc, lv_arc_get_value(arc) + 1);
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            lv_arc_set_value(arc, lv_arc_get_value(arc) - 1);
        }

        if(old_value != ext->cur_value) {
            res = lv_event_send(arc, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
    }
    else if(sign == LV_SIGNAL_CLEANUP) {
        lv_obj_clean_style_list(arc, LV_ARC_PART_KNOB);
        lv_obj_clean_style_list(arc, LV_ARC_PART_INDIC);
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
        case LV_ARC_PART_INDIC:
            style_dsc_p = &ext->style_arc;
            break;
        case LV_ARC_PART_KNOB:
            style_dsc_p = &ext->style_knob;
            break;
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
}

static void inv_arc_area(lv_obj_t * arc, uint16_t start_angle, uint16_t end_angle, lv_arc_part_t part)
{
    /*Skip this complicated invalidation if the arc is not visible*/
    if(lv_obj_is_visible(arc) == false) return;

    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    start_angle += ext->rotation_angle;
    end_angle += ext->rotation_angle;

    if(start_angle >= 360) start_angle -= 360;
    if(end_angle >= 360) end_angle -= 360;

    uint8_t start_quarter = start_angle / 90;
    uint8_t end_quarter = end_angle / 90;

    lv_coord_t left = lv_obj_get_style_pad_left(arc, LV_ARC_PART_BG);
    lv_coord_t right = lv_obj_get_style_pad_right(arc, LV_ARC_PART_BG);
    lv_coord_t top = lv_obj_get_style_pad_top(arc, LV_ARC_PART_BG);
    lv_coord_t bottom = lv_obj_get_style_pad_bottom(arc, LV_ARC_PART_BG);
    lv_coord_t rout       = (LV_MATH_MIN(lv_obj_get_width(arc) - left - right, lv_obj_get_height(arc) - top - bottom)) / 2;
    lv_coord_t x       = arc->coords.x1 + rout + left;
    lv_coord_t y       = arc->coords.y1 + rout + top;
    lv_style_int_t w = lv_obj_get_style_line_width(arc, part);
    lv_style_int_t rounded = lv_obj_get_style_line_rounded(arc, part);
    lv_coord_t rin       = rout - w;
    lv_coord_t extra_area = 0;

    extra_area = rounded ? w / 2 + 2 : 0;

    if(part == LV_ARC_PART_INDIC && lv_style_list_get_style(&ext->style_knob, 0) != NULL) {
        lv_coord_t knob_extra_size = lv_obj_get_draw_rect_ext_pad_size(arc, LV_ARC_PART_KNOB);

        lv_coord_t knob_left = lv_obj_get_style_pad_left(arc, LV_ARC_PART_KNOB);
        lv_coord_t knob_right = lv_obj_get_style_pad_right(arc, LV_ARC_PART_KNOB);
        lv_coord_t knob_top = lv_obj_get_style_pad_top(arc, LV_ARC_PART_KNOB);
        lv_coord_t knob_bottom = lv_obj_get_style_pad_bottom(arc, LV_ARC_PART_KNOB);

        knob_extra_size += LV_MATH_MAX4(knob_left, knob_right, knob_top, knob_bottom);

        extra_area = LV_MATH_MAX(extra_area, w / 2 + 2 + knob_extra_size);

    }

    lv_area_t inv_area;

    if(start_quarter == end_quarter && start_angle <= end_angle) {
        if(start_quarter == 0) {
            inv_area.y1 = y + ((_lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.x2 = x + ((_lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;

            inv_area.y2 = y + ((_lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            inv_area.x1 = x + ((_lv_trigo_sin(end_angle + 90) * rin) >> LV_TRIGO_SHIFT) - extra_area;

            lv_obj_invalidate_area(arc, &inv_area);
        }
        else if(start_quarter == 1) {
            inv_area.y2 = y + ((_lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            inv_area.x2 = x + ((_lv_trigo_sin(start_angle + 90) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            inv_area.y1 = y + ((_lv_trigo_sin(end_angle) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.x1 = x + ((_lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;

            lv_obj_invalidate_area(arc, &inv_area);
        }
        else if(start_quarter == 2) {
            inv_area.x1 = x + ((_lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.y2 = y + ((_lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            inv_area.y1 = y + ((_lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.x2 = x + ((_lv_trigo_sin(end_angle + 90) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            lv_obj_invalidate_area(arc, &inv_area);
        }
        else if(start_quarter == 3) {
            /*Small arc here*/
            inv_area.x1 = x + ((_lv_trigo_sin(start_angle + 90) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            inv_area.y1 = y + ((_lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;

            inv_area.x2 = x + ((_lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            inv_area.y2 = y + ((_lv_trigo_sin(end_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            lv_obj_invalidate_area(arc, &inv_area);
        }

    }
    else if(start_quarter == 0 && end_quarter == 1) {
        inv_area.x1 = x + ((_lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.y1 = y + ((LV_MATH_MIN(_lv_trigo_sin(end_angle),
                                        _lv_trigo_sin(start_angle))  * rin) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.x2 = x + ((_lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        inv_area.y2 = y + rout + extra_area;
        lv_obj_invalidate_area(arc, &inv_area);
    }
    else if(start_quarter == 1 && end_quarter == 2) {
        inv_area.x1 = x - rout - extra_area;
        inv_area.y1 = y + ((_lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.x2 = x + ((LV_MATH_MAX(_lv_trigo_sin(start_angle + 90),
                                        _lv_trigo_sin(end_angle + 90)) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        inv_area.y2 = y + ((_lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        lv_obj_invalidate_area(arc, &inv_area);
    }
    else if(start_quarter == 2 && end_quarter == 3) {
        inv_area.x1 = x + ((_lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.y1 = y - rout - extra_area;
        inv_area.x2 = x + ((_lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        inv_area.y2 = y + (LV_MATH_MAX(_lv_trigo_sin(end_angle) * rin,
                                       _lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        lv_obj_invalidate_area(arc, &inv_area);
    }
    else if(start_quarter == 3 && end_quarter == 0) {
        inv_area.x1 = x + ((LV_MATH_MIN(_lv_trigo_sin(end_angle + 90),
                                        _lv_trigo_sin(start_angle + 90)) * rin) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.y1 = y + ((_lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        inv_area.x2 = x + rout + extra_area;
        inv_area.y2 = y + ((_lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;

        lv_obj_invalidate_area(arc, &inv_area);
    }
    else {
        lv_obj_invalidate(arc);
    }
}

static void get_center(lv_obj_t * arc, lv_point_t * center, lv_coord_t * arc_r)
{
    lv_coord_t left_bg = lv_obj_get_style_pad_left(arc, LV_ARC_PART_BG);
    lv_coord_t right_bg = lv_obj_get_style_pad_right(arc, LV_ARC_PART_BG);
    lv_coord_t top_bg = lv_obj_get_style_pad_top(arc, LV_ARC_PART_BG);
    lv_coord_t bottom_bg = lv_obj_get_style_pad_bottom(arc, LV_ARC_PART_BG);

    lv_coord_t r = (LV_MATH_MIN(lv_obj_get_width(arc) - left_bg - right_bg,
                                lv_obj_get_height(arc) - top_bg - bottom_bg)) / 2;

    *arc_r = r;
    center->x = arc->coords.x1 + r + left_bg;
    center->y = arc->coords.y1 + r + top_bg;


    lv_coord_t indic_width = lv_obj_get_style_line_width(arc, LV_ARC_PART_INDIC);
    r -= indic_width;
}

static void get_knob_area(lv_obj_t * arc, const lv_point_t * center, lv_coord_t r, lv_area_t * knob_area)
{
    lv_arc_ext_t * ext = lv_obj_get_ext_attr(arc);

    lv_coord_t indic_width = lv_obj_get_style_line_width(arc, LV_ARC_PART_INDIC);
    lv_coord_t indic_width_half = indic_width / 2;
    r -= indic_width_half;

    uint16_t angle = ext->rotation_angle;
    if(ext->type == LV_ARC_TYPE_NORMAL) {
        angle += ext->arc_angle_end;
    }
    else if(ext->type == LV_ARC_TYPE_REVERSE) {
        angle += ext->arc_angle_start;
    }
    else if(ext->type == LV_ARC_TYPE_SYMMETRIC) {
        int32_t range_midpoint = (int32_t)(ext->min_value + ext->max_value) / 2;
        if(ext->cur_value < range_midpoint) angle += ext->arc_angle_start;
        else angle += ext->arc_angle_end;
    }
    lv_coord_t knob_x = (r * _lv_trigo_sin(angle + 90)) >> LV_TRIGO_SHIFT;
    lv_coord_t knob_y = (r * _lv_trigo_sin(angle)) >> LV_TRIGO_SHIFT;

    lv_coord_t left_knob = lv_obj_get_style_pad_left(arc, LV_ARC_PART_KNOB);
    lv_coord_t right_knob = lv_obj_get_style_pad_right(arc, LV_ARC_PART_KNOB);
    lv_coord_t top_knob = lv_obj_get_style_pad_top(arc, LV_ARC_PART_KNOB);
    lv_coord_t bottom_knob = lv_obj_get_style_pad_bottom(arc, LV_ARC_PART_KNOB);

    knob_area->x1 = center->x + knob_x - left_knob - indic_width_half;
    knob_area->x2 = center->x + knob_x + right_knob + indic_width_half;
    knob_area->y1 = center->y + knob_y - top_knob - indic_width_half;
    knob_area->y2 = center->y + knob_y + bottom_knob + indic_width_half;
}

/**
 * Used internally to update arc angles after a value change
 * @param arc pointer to a arc object
 */
static void value_update(lv_obj_t * arc)
{
    lv_arc_ext_t * ext = (lv_arc_ext_t *)lv_obj_get_ext_attr(arc);

    int16_t bg_midpoint, range_midpoint, bg_end = ext->bg_angle_end;
    if(ext->bg_angle_end < ext->bg_angle_start) bg_end = ext->bg_angle_end + 360;

    int16_t angle;
    switch(ext->type) {
        case LV_ARC_TYPE_SYMMETRIC:
            bg_midpoint = (ext->bg_angle_start + bg_end) / 2;
            range_midpoint = (int32_t)(ext->min_value + ext->max_value) / 2;

            if(ext->cur_value < range_midpoint) {
                angle = _lv_map(ext->cur_value, ext->min_value, range_midpoint, ext->bg_angle_start, bg_midpoint);
                lv_arc_set_start_angle(arc, angle);
                lv_arc_set_end_angle(arc, bg_midpoint);
            }
            else {
                angle = _lv_map(ext->cur_value, range_midpoint, ext->max_value, bg_midpoint, bg_end);
                lv_arc_set_start_angle(arc, bg_midpoint);
                lv_arc_set_end_angle(arc, angle);
            }
            break;
        case LV_ARC_TYPE_REVERSE:
            angle = _lv_map(ext->cur_value, ext->min_value, ext->max_value, ext->bg_angle_start, bg_end);
            lv_arc_set_start_angle(arc, angle);
            break;
        default: /** LV_ARC_TYPE_NORMAL*/
            angle = _lv_map(ext->cur_value, ext->min_value, ext->max_value, ext->bg_angle_start, bg_end);
            lv_arc_set_end_angle(arc, angle);
            lv_arc_set_start_angle(arc, ext->bg_angle_start);
    }
    ext->last_angle = angle; /*Cache angle for slew rate limiting*/
}

#endif
