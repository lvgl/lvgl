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
#include "../lv_draw/lv_draw_vbasic.h"
#include "../lv_draw/lv_draw_rbasic.h"
#include "../lv_draw/lv_draw.h"
#include <lvgl/lv_misc/area.h>
#include <misc/math/math_base.h>
#include <misc/mem/dyn_mem.h>
#include <misc/others/color.h>
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
static bool lv_line_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);
static void lv_lines_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_lines_t lv_lines_def;
static lv_lines_t lv_lines_decor;
static lv_lines_t lv_lines_chart;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a line objects
 * @param par_dp pointer to an object, it will be the parent of the new line
 * @return pointer to the created line
 */
lv_obj_t* lv_line_create(lv_obj_t* par_dp, lv_obj_t * copy_dp)
{
    /*Create a basic object*/
    lv_obj_t* new_obj_dp = lv_obj_create(par_dp, copy_dp);
    dm_assert(new_obj_dp);

    /*Extend the basic object to rectangle object*/
    lv_line_ext_t *ext_p = lv_obj_alloc_ext(new_obj_dp, sizeof(lv_line_ext_t));
    lv_obj_set_design_f(new_obj_dp, lv_line_design);
    lv_obj_set_signal_f(new_obj_dp, lv_line_signal);

    /*Init the new rectangle*/
    if(copy_dp == NULL) {
		ext_p->point_num = 0;
		ext_p->point_p = NULL;
		ext_p->auto_size = 1;
		ext_p->y_inv = 0;
		ext_p->upscale = 0;
	    lv_obj_set_style(new_obj_dp, lv_lines_get(LV_LINES_DEF, NULL));
    }
    /*Copy 'copy_p' is not NULL*/
    else {
    	lv_line_set_auto_size(new_obj_dp,lv_line_get_auto_size(copy_dp));
    	lv_line_set_y_inv(new_obj_dp,lv_line_get_y_inv(copy_dp));
    	lv_line_set_auto_size(new_obj_dp,lv_line_get_auto_size(copy_dp));
    	lv_line_set_upscale(new_obj_dp,lv_line_get_upscale(copy_dp));
    	lv_line_set_points(new_obj_dp, LV_EA(copy_dp, lv_line_ext_t)->point_p,
    								   LV_EA(copy_dp, lv_line_ext_t)->point_num);
    }


    return new_obj_dp;
}

/**
 * Signal function of the line
 * @param obj_dp pointer to a line object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_line_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_obj_signal(obj_dp, sign, param);

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
 * @param obj_dp pointer to a line object
 * @param point_a an array of points. Only the address is saved,
 * so the array can be a local variable which will be destroyed
 * @param point_num number of points in 'point_a'
 */
void lv_line_set_points(lv_obj_t* obj_dp, const point_t * point_a, uint16_t point_num)
{
	lv_line_ext_t * ext_p = lv_obj_get_ext(obj_dp);
	ext_p->point_p = point_a;
	ext_p->point_num = point_num;

	uint8_t us = 1;
	if(ext_p->upscale != 0) {
		us = LV_DOWNSCALE;
	}

	if(point_num > 0 && ext_p->auto_size != 0) {
		uint16_t i;
		cord_t xmax = LV_CORD_MIN;
		cord_t ymax = LV_CORD_MIN;
		for(i = 0; i < point_num; i++) {
			xmax = max(point_a[i].x * us, xmax);
			ymax = max(point_a[i].y * us, ymax);
		}

		lv_lines_t * lines_p = lv_obj_get_style(obj_dp);
		lv_obj_set_size(obj_dp, xmax + lines_p->width, ymax + lines_p->width);
	}
}

/**
 * Enable (or disable) the auto-size option. The size of the object will fit to its points.
 * (set width to x max and height to y max)
 * @param obj_dp pointer to a line object
 * @param en true: auto size is enabled, false: auto size is disabled
 */
void lv_line_set_auto_size(lv_obj_t * obj_dp, bool en)
{
	lv_line_ext_t * line_p = lv_obj_get_ext(obj_dp);

	line_p->auto_size = en == false ? 0 : 1;

	/*Refresh the object*/
	if(en != false) {
		lv_line_set_points(obj_dp, line_p->point_p, line_p->point_num);
	}
}

/**
 * Enable (or disable) the y coordinate inversion.
 * If enabled then y will be subtracted from the height of the object,
 * therefore the y=0 coordinate will be on the bottom.
 * @param obj_dp pointer to a line object
 * @param en true: enable the y inversion, false:disable the y inversion
 */
void lv_line_set_y_inv(lv_obj_t * obj_dp, bool en)
{
	lv_line_ext_t * line_p = lv_obj_get_ext(obj_dp);

	line_p->y_inv = en == false ? 0 : 1;

	lv_obj_inv(obj_dp);
}

/**
 * Enable (or disable) the point coordinate upscaling (compensate LV_DOWNSCALE).
 * @param obj_dp pointer to a line object
 * @param en true: enable the point coordinate upscaling
 */
void lv_line_set_upscale(lv_obj_t * obj_dp, bool en)
{
	lv_line_ext_t * ext_p = lv_obj_get_ext(obj_dp);

	ext_p->upscale = en == false ? 0 : 1;

	/*Refresh to point to handle auto size*/
	lv_line_set_points(obj_dp, ext_p->point_p, ext_p->point_num);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the auto size attribute
 * @param obj_dp pointer to a line object
 * @return true: auto size is enabled, false: disabled
 */
bool lv_line_get_auto_size(lv_obj_t * obj_dp)
{
	lv_line_ext_t * line_p = lv_obj_get_ext(obj_dp);

	return line_p->auto_size == 0 ? false : true;
}

/**
 * Get the y inversion attribute
 * @param obj_dp pointer to a line object
 * @return true: y inversion is enabled, false: disabled
 */
bool lv_line_get_y_inv(lv_obj_t * obj_dp)
{
	lv_line_ext_t * line_p = lv_obj_get_ext(obj_dp);

	return line_p->y_inv == 0 ? false : true;
}

/**
 * Get the point upscale enable attribute
 * @param obj_dp pointer to a line object
 * @return true: point coordinate upscale is enabled, false: disabled
 */
bool lv_line_get_upscale(lv_obj_t * obj_dp)
{
	lv_line_ext_t * ext_p = lv_obj_get_ext(obj_dp);

	return ext_p->upscale == 0 ? false : true;
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_lines_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_lines_t style
 */
lv_lines_t * lv_lines_get(lv_lines_builtin_t style, lv_lines_t * copy_p)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_lines_init();
		style_inited = true;
	}

	lv_lines_t  *style_p;

	switch(style) {
		case LV_LINES_DEF:
			style_p = &lv_lines_def;
			break;
		case LV_LINES_DECOR:
			style_p = &lv_lines_decor;
			break;
		case LV_LINES_CHART:
			style_p = &lv_lines_chart;
			break;
		default:
			style_p = &lv_lines_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_lines_t));
		else memcpy(copy_p, &lv_lines_def, sizeof(lv_lines_t));
	}

	return style_p;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the lines
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_line_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    /*A line never covers an area*/
    if(mode == LV_DESIGN_COVER_CHK) return false;
    else if(mode == LV_DESIGN_DRAW_MAIN) {
		lv_line_ext_t * ext_p = lv_obj_get_ext(obj_dp);

		if(ext_p->point_num == 0 || ext_p->point_p == NULL) return false;

		lv_lines_t * lines_p = lv_obj_get_style(obj_dp);

		opa_t opa = lv_obj_get_opa(obj_dp);
		area_t area;
		lv_obj_get_cords(obj_dp, &area);
		cord_t x_ofs = area.x1;
		cord_t y_ofs = area.y1;
		point_t p1;
		point_t p2;
		cord_t h = lv_obj_get_height(obj_dp);
		uint16_t i;
		uint8_t us = 1;
		if(ext_p->upscale != 0) {
			us = LV_DOWNSCALE;
		}

		/*Read all pints and draw the lines*/
		for (i = 0; i < ext_p->point_num - 1; i++) {

			p1.x = ext_p->point_p[i].x * us + x_ofs;
			p2.x = ext_p->point_p[i + 1].x * us + x_ofs;

			if(ext_p->y_inv == 0) {
				p1.y = ext_p->point_p[i].y * us + y_ofs;
				p2.y = ext_p->point_p[i + 1].y * us + y_ofs;
			} else {
				p1.y = h - ext_p->point_p[i].y * us + y_ofs;
				p2.y = h - ext_p->point_p[i + 1].y * us + y_ofs;
			}
			lv_draw_line(&p1, &p2, mask_p, lines_p, opa);
		}
    }
    return true;
}

/**
 * Initialize the line styles
 */
static void lv_lines_init(void)
{
	/*Default style*/
	lv_lines_def.width = 2 * LV_STYLE_MULT;
	lv_lines_def.objs.color = COLOR_RED;
	lv_lines_def.objs.transp = 0;

	/*Decoration line style*/
	memcpy(&lv_lines_decor, &lv_lines_def, sizeof(lv_lines_t));
	lv_lines_decor.width = 1 * LV_STYLE_MULT;
	lv_lines_decor.objs.color = COLOR_GRAY;

	/*Chart line style*/
	memcpy(&lv_lines_chart, &lv_lines_def, sizeof(lv_lines_t));
	lv_lines_chart.width = 3 * LV_STYLE_MULT;
	lv_lines_chart.objs.color = COLOR_RED;
}
#endif
