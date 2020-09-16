/**
 * @file lv_linemeter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_linemeter.h"
#if LV_USE_LINEMETER != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_core/lv_group.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_linemeter"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_linemeter_design(lv_obj_t * lmeter, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_linemeter_signal(lv_obj_t * lmeter, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a line meter objects
 * @param par pointer to an object, it will be the parent of the new line meter
 * @param copy pointer to a line meter object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created line meter
 */
lv_obj_t * lv_linemeter_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("line meter create started");

    /*Create the ancestor of line meter*/
    lv_obj_t * linemeter = lv_obj_create(par, copy);
    LV_ASSERT_MEM(linemeter);
    if(linemeter == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(linemeter);

    /*Allocate the line meter type specific extended data*/
    lv_linemeter_ext_t * ext = lv_obj_allocate_ext_attr(linemeter, sizeof(lv_linemeter_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(linemeter);
        return NULL;
    }

    /*Initialize the allocated 'ext' */
    ext->min_value   = 0;
    ext->max_value   = 100;
    ext->cur_value   = 0;
    ext->line_cnt    = 18;
    ext->scale_angle = 240;
    ext->angle_ofs = 0;
    ext->mirrored = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(linemeter, lv_linemeter_signal);
    lv_obj_set_design_cb(linemeter, lv_linemeter_design);

    /*Init the new line meter line meter*/
    if(copy == NULL) {
        lv_obj_set_size(linemeter, 3 * LV_DPI / 2, 3 * LV_DPI / 2);
        lv_theme_apply(linemeter, LV_THEME_LINEMETER);
    }
    /*Copy an existing line meter*/
    else {
        lv_linemeter_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->scale_angle           = copy_ext->scale_angle;
        ext->line_cnt              = copy_ext->line_cnt;
        ext->min_value             = copy_ext->min_value;
        ext->max_value             = copy_ext->max_value;
        ext->cur_value             = copy_ext->cur_value;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(linemeter, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    LV_LOG_INFO("line meter created");

    return linemeter;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the line meter
 * @param lmeter pointer to a line meter object
 * @param value new value
 */
void lv_linemeter_set_value(lv_obj_t * lmeter, int32_t value)
{
    LV_ASSERT_OBJ(lmeter, LV_OBJX_NAME);

    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    if(ext->cur_value == value) return;

    int32_t old_value = ext->cur_value;

    ext->cur_value = value > ext->max_value ? ext->max_value : value;
    ext->cur_value = ext->cur_value < ext->min_value ? ext->min_value : ext->cur_value;

    int16_t level_old =
        (int32_t)((int32_t)(old_value - ext->min_value) * (ext->line_cnt - 1)) / (ext->max_value - ext->min_value);
    int16_t level_new =
        (int32_t)((int32_t)(ext->cur_value - ext->min_value) * (ext->line_cnt - 1)) / (ext->max_value - ext->min_value);

    if(level_new == level_old) {
        return;
    }

    lv_style_int_t left = lv_obj_get_style_pad_left(lmeter, LV_LINEMETER_PART_MAIN);
    lv_style_int_t right = lv_obj_get_style_pad_right(lmeter, LV_LINEMETER_PART_MAIN);
    lv_style_int_t top = lv_obj_get_style_pad_top(lmeter, LV_LINEMETER_PART_MAIN);
    lv_style_int_t bottom = lv_obj_get_style_pad_bottom(lmeter, LV_LINEMETER_PART_MAIN);

    lv_coord_t r_out = (lv_obj_get_width(lmeter) - left - right) / 2 ;
    lv_coord_t r_in  = r_out - lv_obj_get_style_scale_width(lmeter, LV_LINEMETER_PART_MAIN);
    if(r_in < 1) r_in = 1;

    lv_coord_t x_ofs  = lmeter->coords.x1 + r_out + left;
    lv_coord_t y_ofs  = lmeter->coords.y1 + r_out + top;
    int16_t angle_ofs = ext->angle_ofs + 90 + (360 - ext->scale_angle) / 2;

    lv_style_int_t line_width = lv_obj_get_style_scale_end_line_width(lmeter, LV_LINEMETER_PART_MAIN);
    lv_style_int_t end_line_width = lv_obj_get_style_scale_end_line_width(lmeter, LV_LINEMETER_PART_MAIN);
    line_width = LV_MATH_MAX(line_width, end_line_width);

    int32_t angle_old = (level_old * ext->scale_angle) / (ext->line_cnt - 1);

    /*Use smaller clip area only around the visible line*/
    int32_t y_in_old  = (int32_t)((int32_t)_lv_trigo_sin(angle_old + angle_ofs) * r_in) >> LV_TRIGO_SHIFT;
    int32_t x_in_old  = (int32_t)((int32_t)_lv_trigo_sin(angle_old + 90 + angle_ofs) * r_in) >> LV_TRIGO_SHIFT;


    int32_t y_out_old  = (int32_t)((int32_t)_lv_trigo_sin(angle_old + angle_ofs) * r_out) >> LV_TRIGO_SHIFT;
    int32_t x_out_old  = (int32_t)((int32_t)_lv_trigo_sin(angle_old + 90 + angle_ofs) * r_out) >> LV_TRIGO_SHIFT;



    int32_t angle_new = (level_new * ext->scale_angle) / (ext->line_cnt - 1);

    /*Use smaller clip area only around the visible line*/
    int32_t y_in_new  = (int32_t)((int32_t)_lv_trigo_sin(angle_new + angle_ofs) * r_in) >> LV_TRIGO_SHIFT;
    int32_t x_in_new  = (int32_t)((int32_t)_lv_trigo_sin(angle_new + 90 + angle_ofs) * r_in) >> LV_TRIGO_SHIFT;


    int32_t y_out_new  = (int32_t)((int32_t)_lv_trigo_sin(angle_new + angle_ofs) * r_out) >> LV_TRIGO_SHIFT;
    int32_t x_out_new  = (int32_t)((int32_t)_lv_trigo_sin(angle_new + 90 + angle_ofs) * r_out) >> LV_TRIGO_SHIFT;

    lv_area_t a;
    if(x_out_old < 0 && x_out_new < 0) {
        a.x1 = lmeter->coords.x1 + left - line_width;
        a.y1 = LV_MATH_MIN4(y_out_old, y_out_new, y_in_old, y_in_new) + y_ofs - line_width;
        a.x2 = LV_MATH_MAX(x_in_old, x_in_new) + x_ofs + line_width;
        a.y2 = LV_MATH_MAX4(y_out_old, y_out_new, y_in_old, y_in_new) + y_ofs + line_width;
    }
    else if(x_out_old > 0 && x_out_new > 0) {
        a.x1 = LV_MATH_MIN(x_in_old, x_in_new) + x_ofs - line_width;
        a.y1 = LV_MATH_MIN4(y_out_old, y_out_new, y_in_old, y_in_new) + y_ofs - line_width;
        a.x2 = lmeter->coords.x2 - right + line_width;
        a.y2 = LV_MATH_MAX4(y_out_old, y_out_new, y_in_old, y_in_new) + y_ofs + line_width;
    }
    else if(y_out_old < 0 && y_out_new < 0) {
        a.x1 = LV_MATH_MIN4(x_out_old, x_out_new, x_in_old, x_in_new) + x_ofs - line_width;
        a.y1 = lmeter->coords.y1 + top - line_width;
        a.x2 = LV_MATH_MAX4(x_out_old, x_out_new, x_in_old, x_in_new) + x_ofs + line_width;
        a.y2 = LV_MATH_MAX(y_in_old, y_in_new) + y_ofs + line_width;
    }
    else if(y_out_old > 0 && y_out_new > 0) {
        a.x1 = LV_MATH_MIN4(x_out_old, x_out_new, x_in_old, x_in_new) + x_ofs - line_width;
        a.y1 = LV_MATH_MIN(y_in_old, y_in_new) + y_ofs - line_width;
        a.x2 = LV_MATH_MAX4(x_out_old, x_out_new, x_in_old, x_in_new) + x_ofs + line_width;
        a.y2 = lmeter->coords.y2 - bottom + line_width;
    }
    else {
        a.x1 = lmeter->coords.x1 + left - line_width;
        a.y1 = lmeter->coords.y1 + top - line_width;
        a.x2 = lmeter->coords.x2 - right + line_width;
        a.y2 = lmeter->coords.y2 - bottom + line_width;
    }

    lv_obj_invalidate_area(lmeter, &a);

}

/**
 * Set minimum and the maximum values of a line meter
 * @param lmeter pointer to he line meter object
 * @param min minimum value
 * @param max maximum value
 */
void lv_linemeter_set_range(lv_obj_t * lmeter, int32_t min, int32_t max)
{
    LV_ASSERT_OBJ(lmeter, LV_OBJX_NAME);

    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    if(ext->min_value == min && ext->max_value == max) return;

    ext->max_value = max;
    ext->min_value = min;
    if(ext->cur_value > max) {
        ext->cur_value = max;
        lv_linemeter_set_value(lmeter, ext->cur_value);
    }
    if(ext->cur_value < min) {
        ext->cur_value = min;
        lv_linemeter_set_value(lmeter, ext->cur_value);
    }
    lv_obj_invalidate(lmeter);
}

/**
 * Set the scale settings of a line meter
 * @param lmeter pointer to a line meter object
 * @param angle angle of the scale (0..360)
 * @param line_cnt number of lines
 */
void lv_linemeter_set_scale(lv_obj_t * lmeter, uint16_t angle, uint16_t line_cnt)
{
    LV_ASSERT_OBJ(lmeter, LV_OBJX_NAME);

    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    if(ext->scale_angle == angle && ext->line_cnt == line_cnt) return;

    ext->scale_angle = angle;
    ext->line_cnt    = line_cnt;

    lv_obj_invalidate(lmeter);
}

/**
 * Set the set an offset for the line meter's angles to rotate it.
 * @param lmeter pointer to a line meter object
 * @param angle angle where the meter will be facing (with its center)
 */
void lv_linemeter_set_angle_offset(lv_obj_t * lmeter, uint16_t angle)
{
    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    if(ext->angle_ofs == angle) return;

    ext->angle_ofs = angle;

    lv_obj_invalidate(lmeter);
}

/**
 * Set the orientation of the meter growth, clockwise or counterclockwise (mirrored)
 * @param lmeter pointer to a line meter object
 * @param mirror mirror setting
 */
void lv_linemeter_set_mirror(lv_obj_t * lmeter, bool mirror)
{
    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    if(ext->mirrored == mirror) return;

    ext->mirrored = mirror;

    lv_obj_invalidate(lmeter);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the value of the line meter
 */
int32_t lv_linemeter_get_value(const lv_obj_t * lmeter)
{
    LV_ASSERT_OBJ(lmeter, LV_OBJX_NAME);

    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    return ext->cur_value;
}

/**
 * Get the minimum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the minimum value of the line meter
 */
int32_t lv_linemeter_get_min_value(const lv_obj_t * lmeter)
{
    LV_ASSERT_OBJ(lmeter, LV_OBJX_NAME);

    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    return ext->min_value;
}

/**
 * Get the maximum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the maximum value of the line meter
 */
int32_t lv_linemeter_get_max_value(const lv_obj_t * lmeter)
{
    LV_ASSERT_OBJ(lmeter, LV_OBJX_NAME);

    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    return ext->max_value;
}

/**
 * Get the scale number of a line meter
 * @param lmeter pointer to a line meter object
 * @return number of the scale units
 */
uint16_t lv_linemeter_get_line_count(const lv_obj_t * lmeter)
{
    LV_ASSERT_OBJ(lmeter, LV_OBJX_NAME);

    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    return ext->line_cnt;
}

/**
 * Get the scale angle of a line meter
 * @param lmeter pointer to a line meter object
 * @return angle_ofs of the scale
 */
uint16_t lv_linemeter_get_scale_angle(const lv_obj_t * lmeter)
{
    LV_ASSERT_OBJ(lmeter, LV_OBJX_NAME);

    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    return ext->scale_angle;
}

/**
 * Get the offset for the line meter.
 * @param lmeter pointer to a line meter object
 * @return angle offset (0..360)
 */
uint16_t lv_linemeter_get_angle_offset(lv_obj_t * lmeter)
{
    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);

    return ext->angle_ofs;
}

/**
 * get the mirror setting for the line meter
 * @param lmeter pointer to a line meter object
 * @return mirror (true or false)
 */
bool lv_linemeter_get_mirror(lv_obj_t * lmeter)
{
    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);

    return ext->mirrored;
}

void lv_linemeter_draw_scale(lv_obj_t * lmeter, const lv_area_t * clip_area, uint8_t part)
{
    lv_linemeter_ext_t * ext    = lv_obj_get_ext_attr(lmeter);

    lv_style_int_t left = lv_obj_get_style_pad_left(lmeter, LV_LINEMETER_PART_MAIN);
    lv_style_int_t right = lv_obj_get_style_pad_right(lmeter, LV_LINEMETER_PART_MAIN);
    lv_style_int_t top = lv_obj_get_style_pad_top(lmeter, LV_LINEMETER_PART_MAIN);

    lv_coord_t r_out = (lv_obj_get_width(lmeter) - left - right) / 2 ;
    lv_coord_t r_in  = r_out - lv_obj_get_style_scale_width(lmeter, part);
    if(r_in < 1) r_in = 1;

    lv_coord_t x_ofs  = lmeter->coords.x1 + r_out + left;
    lv_coord_t y_ofs  = lmeter->coords.y1 + r_out + top;
    int16_t angle_ofs = ext->angle_ofs + 90 + (360 - ext->scale_angle) / 2;
    int16_t level = ext->mirrored ?
                    (int32_t)((int32_t)(ext->max_value - ext->cur_value) * (ext->line_cnt - 1)) / (ext->max_value - ext->min_value) :
                    (int32_t)((int32_t)(ext->cur_value - ext->min_value) * (ext->line_cnt - 1)) / (ext->max_value - ext->min_value);
    uint8_t i;

    lv_color_t main_color = lv_obj_get_style_line_color(lmeter, part);
    lv_color_t grad_color = lv_obj_get_style_scale_grad_color(lmeter, part);
    lv_color_t end_color = lv_obj_get_style_scale_end_color(lmeter, part);

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(lmeter, part, &line_dsc);
#if LV_LINEMETER_PRECISE == 2
    line_dsc.raw_end = 1;
#endif

    lv_style_int_t end_line_width = lv_obj_get_style_scale_end_line_width(lmeter, part);

#if LV_LINEMETER_PRECISE > 0
    lv_area_t mask_area;
    mask_area.x1 = x_ofs - r_in;
    mask_area.x2 = x_ofs + r_in - 1;
    mask_area.y1 = y_ofs - r_in;
    mask_area.y2 = y_ofs + r_in - 1;

    lv_draw_mask_radius_param_t mask_in_param;
    lv_draw_mask_radius_init(&mask_in_param, &mask_area, LV_RADIUS_CIRCLE, true);
    int16_t mask_in_id = lv_draw_mask_add(&mask_in_param, 0);
#endif


#if LV_LINEMETER_PRECISE > 1
    mask_area.x1 = x_ofs - r_out;
    mask_area.x2 = x_ofs + r_out - 1;
    mask_area.y1 = y_ofs - r_out;
    mask_area.y2 = y_ofs + r_out - 1;
    lv_draw_mask_radius_param_t mask_out_param;
    lv_draw_mask_radius_init(&mask_out_param, &mask_area, LV_RADIUS_CIRCLE, false);
    int16_t mask_out_id = lv_draw_mask_add(&mask_out_param, 0);
    /*In calculation use a larger radius to avoid rounding errors */
    lv_coord_t r_out_extra = r_out + LV_DPI;
#else
    lv_coord_t r_out_extra = r_out;
#endif

    for(i = 0; i < ext->line_cnt; i++) {
        /* `* 256` for extra precision*/
        int32_t angle_upscale = (i * ext->scale_angle * 256) / (ext->line_cnt - 1);
        int32_t angle_normal = angle_upscale >> 8;

        int32_t angle_low = (angle_upscale >> 8);
        int32_t angle_high = angle_low + 1;
        int32_t angle_rem = angle_upscale & 0xFF;

        /*Interpolate sine and cos*/
        int32_t sin_low = _lv_trigo_sin(angle_low + angle_ofs);
        int32_t sin_high = _lv_trigo_sin(angle_high + angle_ofs);
        int32_t sin_mid = (sin_low * (256 - angle_rem) + sin_high * angle_rem) >> 8;

        int32_t cos_low = _lv_trigo_sin(angle_low + 90 + angle_ofs);
        int32_t cos_high = _lv_trigo_sin(angle_high + 90 + angle_ofs);
        int32_t cos_mid = (cos_low * (256 - angle_rem) + cos_high * angle_rem) >> 8;

        /*Use the interpolated values to get x and y coordinates*/
        int32_t y_out_extra = (int32_t)((int32_t)sin_mid * r_out_extra) >> (LV_TRIGO_SHIFT - 8);
        int32_t x_out_extra = (int32_t)((int32_t)cos_mid * r_out_extra) >> (LV_TRIGO_SHIFT - 8);

        /*Rounding*/
        if(x_out_extra > 0) x_out_extra = (x_out_extra + 127) >> 8;
        else x_out_extra = (x_out_extra - 127) >> 8;

        if(y_out_extra > 0) y_out_extra = (y_out_extra + 127) >> 8;
        else y_out_extra = (y_out_extra - 127) >> 8;

        x_out_extra += x_ofs;
        y_out_extra += y_ofs;

        /*With no extra precision use the coordinates on the inner radius*/
#if LV_LINEMETER_PRECISE == 0
        /*Use the interpolated values to get x and y coordinates*/
        int32_t y_in_extra = (int32_t)((int32_t)sin_mid * r_in) >> (LV_TRIGO_SHIFT - 8);
        int32_t x_in_extra = (int32_t)((int32_t)cos_mid * r_in) >> (LV_TRIGO_SHIFT - 8);

        /*Rounding*/
        if(x_in_extra > 0) x_in_extra = (x_in_extra + 127) >> 8;
        else x_in_extra = (x_in_extra - 127) >> 8;

        if(y_in_extra > 0) y_in_extra = (y_in_extra + 127) >> 8;
        else y_in_extra = (y_in_extra - 127) >> 8;

        x_in_extra += x_ofs;
        y_in_extra += y_ofs;
#else
        int32_t x_in_extra = x_ofs;
        int32_t y_in_extra = y_ofs;
#endif

        /*Use smaller clip area only around the visible line*/
        int32_t y_in  = (int32_t)((int32_t)_lv_trigo_sin(angle_normal + angle_ofs) * r_in) >> LV_TRIGO_SHIFT;
        int32_t x_in  = (int32_t)((int32_t)_lv_trigo_sin(angle_normal + 90 + angle_ofs) * r_in) >> LV_TRIGO_SHIFT;

        x_in += x_ofs;
        y_in += y_ofs;

        int32_t y_out  = (int32_t)((int32_t)_lv_trigo_sin(angle_normal + angle_ofs) * r_out) >> LV_TRIGO_SHIFT;
        int32_t x_out  = (int32_t)((int32_t)_lv_trigo_sin(angle_normal + 90 + angle_ofs) * r_out) >> LV_TRIGO_SHIFT;

        x_out += x_ofs;
        y_out += y_ofs;

        lv_area_t clip_sub;
        clip_sub.x1 = LV_MATH_MIN(x_in, x_out) - line_dsc.width;
        clip_sub.x2 = LV_MATH_MAX(x_in, x_out) + line_dsc.width;
        clip_sub.y1 = LV_MATH_MIN(y_in, y_out) - line_dsc.width;
        clip_sub.y2 = LV_MATH_MAX(y_in, y_out) + line_dsc.width;

        if(_lv_area_intersect(&clip_sub, &clip_sub, clip_area) == false) continue;

        lv_point_t p1;
        lv_point_t p2;

        p2.x = x_in_extra;
        p2.y = y_in_extra;

        p1.x = x_out_extra;
        p1.y = y_out_extra;

        /* Set the color of the lines */
        if((!ext->mirrored && i >= level) || (ext->mirrored && i <= level)) {
            line_dsc.color = end_color;
            line_dsc.width = end_line_width;
        }
        else {
            line_dsc.color = lv_color_mix(grad_color, main_color, (255 * i) / ext->line_cnt);
        }

        lv_draw_line(&p1, &p2, &clip_sub, &line_dsc);
    }

#if LV_LINEMETER_PRECISE > 0
    lv_draw_mask_remove_id(mask_in_id);
#endif

#if LV_LINEMETER_PRECISE > 1
    lv_draw_mask_remove_id(mask_out_id);
#endif

    if(part == LV_LINEMETER_PART_MAIN && level < ext->line_cnt - 1) {
        lv_style_int_t border_width = lv_obj_get_style_scale_border_width(lmeter, part);
        lv_style_int_t end_border_width = lv_obj_get_style_scale_end_border_width(lmeter, part);

        if(border_width || end_border_width) {
            int16_t end_angle = ((level) * ext->scale_angle) / (ext->line_cnt - 1) + angle_ofs;
            lv_draw_line_dsc_t arc_dsc;
            lv_draw_line_dsc_init(&arc_dsc);
            lv_obj_init_draw_line_dsc(lmeter, part, &arc_dsc);

            if(border_width) {
                arc_dsc.width = border_width;
                arc_dsc.color = main_color;
                lv_draw_arc(x_ofs, y_ofs, r_out, angle_ofs, end_angle, clip_area, &arc_dsc);
            }

            if(end_border_width) {
                arc_dsc.width = end_border_width;
                arc_dsc.color = end_color;
                lv_draw_arc(x_ofs, y_ofs, r_out, end_angle, (angle_ofs + ext->scale_angle) % 360, clip_area, &arc_dsc);
            }
        }
    }


}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the line meters
 * @param lmeter pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_linemeter_design(lv_obj_t * lmeter, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return LV_DESIGN_RES_NOT_COVER;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_draw_rect_dsc_t bg_dsc;
        lv_draw_rect_dsc_init(&bg_dsc);
        lv_obj_init_draw_rect_dsc(lmeter, LV_LINEMETER_PART_MAIN, &bg_dsc);
        lv_draw_rect(&lmeter->coords, clip_area, &bg_dsc);
        lv_linemeter_draw_scale(lmeter, clip_area, LV_LINEMETER_PART_MAIN);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
    }

    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the line meter
 * @param lmeter pointer to a line meter object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_linemeter_signal(lv_obj_t * lmeter, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(lmeter, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        lv_obj_refresh_ext_draw_pad(lmeter);
        lv_obj_invalidate(lmeter);
    }

    return res;
}
#endif
