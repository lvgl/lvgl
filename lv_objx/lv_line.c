/**
 * @file lv_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"

#if USE_LV_LINE != 0
#include "lv_line.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_misc/lv_math.h"
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
static bool lv_line_design(lv_obj_t * line, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_line_signal(lv_obj_t * line, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;

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
    lv_mem_assert(new_line);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_line);

    /*Extend the basic object to line object*/
    lv_line_ext_t * ext = lv_obj_allocate_ext_attr(new_line, sizeof(lv_line_ext_t));
    lv_mem_assert(ext);
    ext->point_num = 0;
    ext->point_array = NULL;
    ext->auto_size = 1;
    ext->y_inv = 0;

    lv_obj_set_design_func(new_line, lv_line_design);
    lv_obj_set_signal_func(new_line, lv_line_signal);

    /*Init the new line*/
    if(copy == NULL) {
        lv_obj_set_size(new_line, LV_DPI, LV_DPI);  /*Auto size is enables, but set default size until no points are added*/
	    lv_obj_set_style(new_line, NULL);           /*Inherit parent's style*/
	    lv_obj_set_click(new_line, false);
    }
    /*Copy an existing object*/
    else {
        lv_line_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
    	lv_line_set_auto_size(new_line,lv_line_get_auto_size(copy));
    	lv_line_set_y_invert(new_line,lv_line_get_y_inv(copy));
    	lv_line_set_auto_size(new_line,lv_line_get_auto_size(copy));
    	lv_line_set_points(new_line, copy_ext->point_array, copy_ext->point_num);
        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_line);
    }

    return new_line;
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
void lv_line_set_points(lv_obj_t * line, const lv_point_t * point_a, uint16_t point_num)
{
	lv_line_ext_t * ext = lv_obj_get_ext_attr(line);
	ext->point_array = point_a;
	ext->point_num = point_num;

	if(point_num > 0 && ext->auto_size != 0) {
		uint16_t i;
		lv_coord_t xmax = LV_COORD_MIN;
		lv_coord_t ymax = LV_COORD_MIN;
		for(i = 0; i < point_num; i++) {
			xmax = LV_MATH_MAX(point_a[i].x, xmax);
			ymax = LV_MATH_MAX(point_a[i].y, ymax);
		}

		lv_style_t * style = lv_line_get_style(line);
		lv_obj_set_size(line, xmax + style->line.width, ymax + style->line.width);
	}

	lv_obj_invalidate(line);
}

/**
 * Enable (or disable) the auto-size option. The size of the object will fit to its points.
 * (set width to x max and height to y max)
 * @param line pointer to a line object
 * @param autosize_en true: auto size is enabled, false: auto size is disabled
 */
void lv_line_set_auto_size(lv_obj_t * line, bool autosize_en)
{
	lv_line_ext_t * ext = lv_obj_get_ext_attr(line);

	ext->auto_size = autosize_en == false ? 0 : 1;

	/*Refresh the object*/
	if(autosize_en) lv_line_set_points(line, ext->point_array, ext->point_num);
}

/**
 * Enable (or disable) the y coordinate inversion.
 * If enabled then y will be subtracted from the height of the object,
 * therefore the y=0 coordinate will be on the bottom.
 * @param line pointer to a line object
 * @param yinv_en true: enable the y inversion, false:disable the y inversion
 */
void lv_line_set_y_invert(lv_obj_t * line, bool yinv_en)
{
	lv_line_ext_t * ext = lv_obj_get_ext_attr(line);

	ext->y_inv = yinv_en == false ? 0 : 1;

	lv_obj_invalidate(line);
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
static bool lv_line_design(lv_obj_t * line, const lv_area_t * mask, lv_design_mode_t mode)
{
    /*A line never covers an area*/
    if(mode == LV_DESIGN_COVER_CHK) return false;
    else if(mode == LV_DESIGN_DRAW_MAIN) {
		lv_line_ext_t * ext = lv_obj_get_ext_attr(line);

		if(ext->point_num == 0 || ext->point_array == NULL) return false;

		lv_style_t * style = lv_obj_get_style(line);
		lv_area_t area;
		lv_obj_get_coords(line, &area);
		lv_coord_t x_ofs = area.x1;
		lv_coord_t y_ofs = area.y1;
		lv_point_t p1;
		lv_point_t p2;
		lv_coord_t h = lv_obj_get_height(line);
		uint16_t i;

		/*Read all pints and draw the lines*/
		for (i = 0; i < ext->point_num - 1; i++) {

			p1.x = ext->point_array[i].x + x_ofs;
			p2.x = ext->point_array[i + 1].x + x_ofs;

			if(ext->y_inv == 0) {
				p1.y = ext->point_array[i].y + y_ofs;
				p2.y = ext->point_array[i + 1].y + y_ofs;
			} else {
				p1.y = h - ext->point_array[i].y  + y_ofs;
				p2.y = h - ext->point_array[i + 1].y + y_ofs;
			}
			lv_draw_line(&p1, &p2, mask, style);
		}
    }
    return true;
}

/**
 * Signal function of the line
 * @param line pointer to a line object
 * @param sign a signal type from lv_signal_t enum
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_line_signal(lv_obj_t * line, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(line, sign, param);
    if(res != LV_RES_OK) return res;


    if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_line";
    }

    return res;
}
#endif
