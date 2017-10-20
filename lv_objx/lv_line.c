/**
 * @file lv_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"

#if USE_LV_LINE != 0
#include "lv_line.h"
#include "../lv_draw/lv_draw.h"
#include "misc/math/math_base.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_line_design(lv_obj_t * line, const area_t * mask, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a line objects
 * @param par pointer to an object, it will be the parent of the new line
 * @return pointer to the created line
 */
lv_obj_t * lv_line_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create a basic object*/
    lv_obj_t * new_line = lv_obj_create(par, copy);
    dm_assert(new_line);

    /*Extend the basic object to line object*/
    lv_line_ext_t * ext = lv_obj_allocate_ext_attr(new_line, sizeof(lv_line_ext_t));
    dm_assert(ext);
    ext->point_num = 0;
    ext->point_array = NULL;
    ext->auto_size = 1;
    ext->y_inv = 0;
    ext->upscale = 0;

    lv_obj_set_design_func(new_line, lv_line_design);
    lv_obj_set_signal_func(new_line, lv_line_signal);

    /*Init the new line*/
    if(copy == NULL) {
	    lv_obj_set_style(new_line, lv_style_get(LV_STYLE_PLAIN));
    }
    /*Copy an existing object*/
    else {
        lv_line_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
    	lv_line_set_auto_size(new_line,lv_line_get_auto_size(copy));
    	lv_line_set_y_inv(new_line,lv_line_get_y_inv(copy));
    	lv_line_set_auto_size(new_line,lv_line_get_auto_size(copy));
    	lv_line_set_upscale(new_line,lv_line_get_upscale(copy));
    	lv_line_set_points(new_line, copy_ext->point_array, copy_ext->point_num);
        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_line);
    }

    return new_line;
}

/**
 * Signal function of the line
 * @param line pointer to a line object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_line_signal(lv_obj_t * line, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_obj_signal(line, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	switch(sign) {
    		default:
    			break;
    	}
    }

    return valid;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set an array of points. The line object will connect these points.
 * @param line pointer to a line object
 * @param point_a an array of points. Only the address is saved,
 * so the array can NOT be a local variable which will be destroyed
 * @param point_num number of points in 'point_a'
 */
void lv_line_set_points(lv_obj_t * line, const point_t * point_a, uint16_t point_num)
{
	lv_line_ext_t * ext = lv_obj_get_ext_attr(line);
	ext->point_array = point_a;
	ext->point_num = point_num;

	uint8_t us = 1;
	if(ext->upscale != 0) {
		us = 1 << LV_ANTIALIAS;
	}

	if(point_num > 0 && ext->auto_size != 0) {
		uint16_t i;
		cord_t xmax = CORD_MIN;
		cord_t ymax = CORD_MIN;
		for(i = 0; i < point_num; i++) {
			xmax = MATH_MAX(point_a[i].x * us, xmax);
			ymax = MATH_MAX(point_a[i].y * us, ymax);
		}

		lv_style_t * lines = lv_obj_get_style(line);
		lv_obj_set_size(line, xmax + lines->line.width, ymax + lines->line.width);
	}
}

/**
 * Enable (or disable) the auto-size option. The size of the object will fit to its points.
 * (set width to x max and height to y max)
 * @param line pointer to a line object
 * @param autosize true: auto size is enabled, false: auto size is disabled
 */
void lv_line_set_auto_size(lv_obj_t * line, bool autosize)
{
	lv_line_ext_t * ext = lv_obj_get_ext_attr(line);

	ext->auto_size = autosize == false ? 0 : 1;

	/*Refresh the object*/
	if(autosize != false) {
		lv_line_set_points(line, ext->point_array, ext->point_num);
	}
}

/**
 * Enable (or disable) the y coordinate inversion.
 * If enabled then y will be subtracted from the height of the object,
 * therefore the y=0 coordinate will be on the bottom.
 * @param line pointer to a line object
 * @param yinv true: enable the y inversion, false:disable the y inversion
 */
void lv_line_set_y_inv(lv_obj_t * line, bool yinv)
{
	lv_line_ext_t * ext = lv_obj_get_ext_attr(line);

	ext->y_inv = yinv == false ? 0 : 1;

	lv_obj_invalidate(line);
}

/**
 * Enable (or disable) the points' coordinate upscaling (if LV_ANTIALIAS is enabled).
 * @param line pointer to a line object
 * @param unscale true: enable the point coordinate upscaling
 */
void lv_line_set_upscale(lv_obj_t * line, bool unscale)
{
	lv_line_ext_t * ext = lv_obj_get_ext_attr(line);

	ext->upscale = unscale == false ? 0 : 1;

	/*Refresh to point to handle auto size*/
	lv_line_set_points(line, ext->point_array, ext->point_num);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the auto size attribute
 * @param line pointer to a line object
 * @return true: auto size is enabled, false: disabled
 */
bool lv_line_get_auto_size(lv_obj_t * line)
{
	lv_line_ext_t * ext = lv_obj_get_ext_attr(line);

	return ext->auto_size == 0 ? false : true;
}

/**
 * Get the y inversion attribute
 * @param line pointer to a line object
 * @return true: y inversion is enabled, false: disabled
 */
bool lv_line_get_y_inv(lv_obj_t * line)
{
	lv_line_ext_t * ext = lv_obj_get_ext_attr(line);

	return ext->y_inv == 0 ? false : true;
}

/**
 * Get the point upscale enable attribute
 * @param obj pointer to a line object
 * @return true: point coordinate upscale is enabled, false: disabled
 */
bool lv_line_get_upscale(lv_obj_t * line)
{
	lv_line_ext_t * ext = lv_obj_get_ext_attr(line);

	return ext->upscale == 0 ? false : true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the lines
 * @param line pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_line_design(lv_obj_t * line, const area_t * mask, lv_design_mode_t mode)
{
    /*A line never covers an area*/
    if(mode == LV_DESIGN_COVER_CHK) return false;
    else if(mode == LV_DESIGN_DRAW_MAIN) {
		lv_line_ext_t * ext = lv_obj_get_ext_attr(line);

		if(ext->point_num == 0 || ext->point_array == NULL) return false;

		lv_style_t * style = lv_obj_get_style(line);
		area_t area;
		lv_obj_get_coords(line, &area);
		cord_t x_ofs = area.x1;
		cord_t y_ofs = area.y1;
		point_t p1;
		point_t p2;
		cord_t h = lv_obj_get_height(line);
		uint16_t i;
		uint8_t us = 1;
		if(ext->upscale != 0) {
			us = 1 << LV_ANTIALIAS;
		}

		/*Read all pints and draw the lines*/
		for (i = 0; i < ext->point_num - 1; i++) {

			p1.x = ext->point_array[i].x * us + x_ofs;
			p2.x = ext->point_array[i + 1].x * us + x_ofs;

			if(ext->y_inv == 0) {
				p1.y = ext->point_array[i].y * us + y_ofs;
				p2.y = ext->point_array[i + 1].y * us + y_ofs;
			} else {
				p1.y = h - ext->point_array[i].y * us + y_ofs;
				p2.y = h - ext->point_array[i + 1].y * us + y_ofs;
			}
			lv_draw_line(&p1, &p2, mask, style);
		}
    }
    return true;
}

#endif
