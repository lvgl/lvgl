/**
 * @file lv_linemeter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_linemeter.h"
#if LV_USE_LMETER != 0

#include "../lv_core/lv_debug.h"
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
    lv_obj_t * new_lmeter = lv_obj_create(par, copy);
    LV_ASSERT_MEM(new_lmeter);
    if(new_lmeter == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_lmeter);

    /*Allocate the line meter type specific extended data*/
    lv_linemeter_ext_t * ext = lv_obj_allocate_ext_attr(new_lmeter, sizeof(lv_linemeter_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(new_lmeter);
        return NULL;
    }

    /*Initialize the allocated 'ext' */
    ext->min_value   = 0;
    ext->max_value   = 100;
    ext->cur_value   = 0;
    ext->line_cnt    = 21;  /*Odd scale number looks better*/
    ext->scale_angle = 240; /*(scale_num - 1) * N looks better */
    ext->angle_ofs = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_lmeter, lv_linemeter_signal);
    lv_obj_set_design_cb(new_lmeter, lv_linemeter_design);

    /*Init the new line meter line meter*/
    if(copy == NULL) {
        lv_obj_set_size(new_lmeter, LV_DPI, LV_DPI);
        lv_theme_apply(new_lmeter, LV_THEME_LMETER);
    }
    /*Copy an existing line meter*/
    else {
        lv_linemeter_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->scale_angle           = copy_ext->scale_angle;
        ext->line_cnt              = copy_ext->line_cnt;
        ext->min_value             = copy_ext->min_value;
        ext->max_value             = copy_ext->max_value;
        ext->cur_value             = copy_ext->cur_value;

//        /*Refresh the style with new signal function*/
//        lv_obj_refresh_style(new_lmeter);
    }

    LV_LOG_INFO("line meter created");

    return new_lmeter;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the line meter
 * @param lmeter pointer to a line meter object
 * @param value new value
 */
void lv_linemeter_set_value(lv_obj_t * lmeter, int16_t value)
{
    LV_ASSERT_OBJ(lmeter, LV_OBJX_NAME);

    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);
    if(ext->cur_value == value) return;

    ext->cur_value = value > ext->max_value ? ext->max_value : value;
    ext->cur_value = ext->cur_value < ext->min_value ? ext->min_value : ext->cur_value;
    lv_obj_invalidate(lmeter);
}

/**
 * Set minimum and the maximum values of a line meter
 * @param lmeter pointer to he line meter object
 * @param min minimum value
 * @param max maximum value
 */
void lv_linemeter_set_range(lv_obj_t * lmeter, int16_t min, int16_t max)
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

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the value of the line meter
 */
int16_t lv_linemeter_get_value(const lv_obj_t * lmeter)
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
int16_t lv_linemeter_get_min_value(const lv_obj_t * lmeter)
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
int16_t lv_linemeter_get_max_value(const lv_obj_t * lmeter)
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
 * get the set an offset for the line meter.
 * @param lmeter pointer to a line meter object
 * @return angle offset (0..360)
 */
uint16_t lv_linemeter_get_angle_offset(lv_obj_t * lmeter)
{
    lv_linemeter_ext_t * ext = lv_obj_get_ext_attr(lmeter);

    return ext->angle_ofs;
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
    int16_t level =
        (int32_t)((int32_t)(ext->cur_value - ext->min_value) * ext->line_cnt) / (ext->max_value - ext->min_value);
    uint8_t i;

    lv_color_t main_color = lv_obj_get_style_scale_color(lmeter, part);
    lv_color_t grad_color = lv_obj_get_style_scale_grad_color(lmeter, part);
    lv_color_t end_color = lv_obj_get_style_scale_end_color(lmeter, part);


    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(lmeter, part, &line_dsc);

    lv_style_int_t end_line_width = lv_obj_get_style_scale_end_line_width(lmeter, part);

    for(i = 0; i < ext->line_cnt; i++) {
        /*Calculate the position a scale label*/
        int16_t angle = (i * ext->scale_angle) / (ext->line_cnt - 1) + angle_ofs;

        lv_coord_t y_out = (int32_t)((int32_t)lv_trigo_sin(angle) * r_out) >> (LV_TRIGO_SHIFT - 8);
        lv_coord_t x_out = (int32_t)((int32_t)lv_trigo_sin(angle + 90) * r_out) >> (LV_TRIGO_SHIFT - 8);
        lv_coord_t y_in  = (int32_t)((int32_t)lv_trigo_sin(angle) * r_in) >> (LV_TRIGO_SHIFT - 8);
        lv_coord_t x_in  = (int32_t)((int32_t)lv_trigo_sin(angle + 90) * r_in) >> (LV_TRIGO_SHIFT - 8);

        /*Rounding*/
        if(x_out <= 0) x_out = (x_out + 127) >> 8;
        else x_out = (x_out - 127) >> 8;

        if(x_in <= 0) x_in = (x_in + 127) >> 8;
        else x_in = (x_in - 127) >> 8;

        if(y_out <= 0) y_out = (y_out + 127) >> 8;
        else y_out = (y_out - 127) >> 8;

        if(y_in <= 0) y_in = (y_in + 127) >> 8;
        else y_in = (y_in - 127) >> 8;

        lv_point_t p1;
        lv_point_t p2;

        p2.x = x_in + x_ofs;
        p2.y = y_in + y_ofs;

        p1.x = x_out + x_ofs;
        p1.y = y_out + y_ofs;

        if(i >= level) {
            line_dsc.color = end_color;
            line_dsc.width = end_line_width;
        }
        else line_dsc.color = lv_color_mix(grad_color, main_color, (255 * i) / ext->line_cnt);

        lv_draw_line(&p1, &p2, clip_area, &line_dsc);
    }

    if(part == LV_LINEMETER_PART_MAIN) {
        lv_style_int_t border_width = lv_obj_get_style_scale_border_width(lmeter, part);
        lv_style_int_t end_border_width = lv_obj_get_style_scale_end_border_width(lmeter, part);

        if(border_width || end_border_width)
        {
            int16_t end_angle = (level * ext->scale_angle) / (ext->line_cnt - 1) + angle_ofs - 1;
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
#include <stdlib.h>

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
    } else if(sign == LV_SIGNAL_STYLE_CHG) {
        lv_obj_refresh_ext_draw_pad(lmeter);
        lv_obj_invalidate(lmeter);
    }

    return res;
}
#endif
