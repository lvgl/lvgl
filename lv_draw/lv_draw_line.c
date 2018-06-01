/**
 * @file lv_draw_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <stdbool.h>
#include "lv_draw.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define LINE_WIDTH_CORR_BASE 64
#define LINE_WIDTH_CORR_SHIFT 6

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
	lv_point_t p1;
	lv_point_t p2;
	lv_point_t p_act;
	lv_coord_t dx;
	lv_coord_t sx;		/*-1: x1 < x2; 1: x2 >= x1*/
	lv_coord_t dy;
	lv_coord_t sy;		/*-1: y1 < y2; 1: y2 >= y1*/
	lv_coord_t err;
	lv_coord_t e2;
	bool hor;	/*Rather horizontal or vertical*/
}line_draw_t;

typedef struct {
	lv_coord_t width;
	lv_coord_t width_1;
	lv_coord_t width_half;
}line_width_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void line_draw_hor(line_draw_t * main_line, const lv_area_t * mask, const lv_style_t * style);
static void line_draw_ver(line_draw_t * main_line, const lv_area_t * mask, const lv_style_t * style);
static void line_draw_skew(line_draw_t * main_line, const lv_area_t * mask, const lv_style_t * style);
static void line_init(line_draw_t * line, const lv_point_t * p1, const lv_point_t * p2);
static bool line_next(line_draw_t * line);
static bool line_next_y(line_draw_t * line);
static void line_ver_aa(lv_coord_t x, lv_coord_t y, lv_coord_t length, const lv_area_t * mask, lv_color_t color, lv_opa_t opa);
static void line_hor_aa(lv_coord_t x, lv_coord_t y, lv_coord_t length, const lv_area_t * mask, lv_color_t color, lv_opa_t opa);

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
 * Draw a line
 * @param p1 first point of the line
 * @param p2 second point of the line
 * @param maskthe line will be drawn only on this area
 * @param lines_p pointer to a line style
 */
void lv_draw_line(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * mask,
                  const lv_style_t * style)
{

    if(style->line.width == 0) return;
    if(point1->x == point2->x && point1->y == point2->y) return;

    line_draw_t main_line;
    lv_point_t p1;
    lv_point_t p2;

    /*Be sure always x1 < x2*/
    if(point1->x < point2->x) {
    	p1.x = point1->x;
    	p1.y = point1->y;
    	p2.x = point2->x;
    	p2.y = point2->y;
    } else {
    	p1.x = point2->x;
    	p1.y = point2->y;
    	p2.x = point1->x;
    	p2.y = point1->y;
    }
    line_init(&main_line, &p1, &p2);


    /*Special case draw a horizontal line*/
    if(main_line.p1.y == main_line.p2.y ) {
    	line_draw_hor(&main_line, mask, style);
    }
    /*Special case draw a vertical line*/
    else if(main_line.p1.x == main_line.p2.x ) {
    	line_draw_ver(&main_line, mask, style);
    }
    /*Arbitrary skew line*/
    else {
    	line_draw_skew(&main_line, mask, style);
    }
}


/**********************
 *   STATIC FUNCTIONS
 **********************/


static void line_draw_hor(line_draw_t * line, const lv_area_t * mask, const lv_style_t * style)
{
	lv_coord_t width = style->line.width - 1;
	lv_coord_t width_half = width >> 1;
	lv_coord_t width_1 = width & 0x1;

    lv_area_t act_area;
    act_area.x1 = line->p1.x;
    act_area.x2 = line->p2.x;
    act_area.y1 = line->p1.y - width_half - width_1;
    act_area.y2 = line->p2.y + width_half ;

    lv_area_t draw_area;
    draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
    draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
    draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
    draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
    fill_fp(&draw_area, mask, style->line.color, style->line.opa);
}

static void line_draw_ver(line_draw_t * line, const lv_area_t * mask, const lv_style_t * style)
{
	lv_coord_t width = style->line.width - 1;
	lv_coord_t width_half = width >> 1;
	lv_coord_t width_1 = width & 0x1;
    lv_area_t act_area;

    act_area.x1 = line->p1.x - width_half;
    act_area.x2 = line->p2.x + width_half + width_1;
    act_area.y1 = line->p1.y;
    act_area.y2 = line->p2.y;

    lv_area_t draw_area;
    draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
    draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
    draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
    draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
    fill_fp(&draw_area, mask, style->line.color, style->line.opa);
}

static void line_draw_skew(line_draw_t * main_line, const lv_area_t * mask, const lv_style_t * style)
{

	lv_coord_t width;
	width = style->line.width - 1;


#if LV_ANTIALIAS != 0
	width--;    						   /*Because of anti aliasing*/
#endif

	static const uint8_t width_corr_array[] = {
			64, 64, 64, 64, 64, 64, 64, 64, 64, 65, 65, 65, 65, 65, 66, 66, 66, 66, 66,
			67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74,
			74, 75, 75, 76, 77, 77, 78, 78, 79, 79, 80, 81, 81, 82, 82, 83, 84, 84, 85,
			86, 86, 87, 88, 88, 89, 90, 91,
	};

	lv_coord_t width_half;
	lv_coord_t width_1;
	uint16_t wcor;
	if(main_line->hor == false) {
		wcor = (main_line->dx * LINE_WIDTH_CORR_BASE) / main_line->dy;
	} else  {
		wcor = (main_line->dy * LINE_WIDTH_CORR_BASE) / main_line->dx;
	}

	/*Make the correction on line width*/
	if(width > 0) {
		width = (width * width_corr_array[wcor]);
		width = width >> LINE_WIDTH_CORR_SHIFT;
		width_half = width >> 1;
		width_1 = width & 0x1;
	} else {
		width_1 = 0;
		width_half = 0;
	}

	volatile lv_point_t vect_main, vect_norm;
	vect_main.x = main_line->p2.x - main_line->p1.x;
	vect_main.y = main_line->p2.y - main_line->p1.y;
	if(main_line->p1.y < main_line->p2.y) {
		vect_norm.x = - vect_main.y;
		vect_norm.y = vect_main.x;
	} else {
		vect_norm.x = vect_main.y;
		vect_norm.y = -vect_main.x;
		return;

	}

	if(main_line->hor) return;

	lv_coord_t shift_y = 0;
	lv_coord_t shift_x = 0;
	/*The main line need to be moved to the result line start/end on the desired coordinates*/
	if((width + 1) * vect_main.x > vect_main.y) {		/*The calculation contains two divisions. Do it only if it will result non zero*/
		lv_coord_t inters_y = (width * vect_main.x) / vect_main.y;	/*Get the y coordinate (height) of the intersection of "end" and "neg" line"*/
		shift_y = inters_y / 2;
		shift_x = (shift_y * vect_norm.y) / vect_norm.x;
		printf("sx:%d, sy:%d\n", shift_x, shift_y);
	}

	line_draw_t line_neg;
	line_draw_t line_pos;
	line_draw_t line_end;
	lv_point_t p1_neg, p2_neg, p1_pos, p2_pos, p1_end, p2_end;
	p1_neg.x = main_line->p1.x - width_half + shift_x;
	p1_neg.y = main_line->p1.y - shift_y;
	p2_neg.x = main_line->p2.x - width_half - shift_x;
	p2_neg.y = main_line->p2.y + shift_y;
	p1_pos.x = main_line->p1.x + width_half + width_1 + shift_x;
	p1_pos.y = main_line->p1.y - shift_y;
	p2_pos.x = main_line->p2.x + width_half + width_1 - shift_x;
	p2_pos.y = main_line->p2.y + shift_y;
	p1_end.x = p1_pos.x;
	p1_end.y = p1_pos.y;
	p2_end.x = p1_end.x + vect_norm.x;
	p2_end.y = p1_end.y + vect_norm.y;

	line_init(&line_pos, &p1_pos, &p2_pos);
	line_init(&line_end, &p1_end, &p2_end);

	lv_area_t draw_area;

	/*Top perpendicular end*/
#if LV_ANTIALIAS
	volatile lv_point_t aa_last_step_pos;			/*The point where the last step in x occurred*/
	volatile lv_point_t aa_last_step_end;
	aa_last_step_pos.x = line_pos.p_act.x;
	aa_last_step_pos.y = line_pos.p_act.y;
	aa_last_step_end.x = line_end.p_act.x;
	aa_last_step_end.y = line_end.p_act.y;
//
//	/*On pixel at the top and bottom*/
//	px_fp(aa_last_step_pos.x, aa_last_step_pos.y - 1, mask, LV_COLOR_AQUA, LV_OPA_80 / 2);
#endif
	do {
		draw_area.x1 = LV_MATH_MIN(line_end.p_act.x, line_pos.p_act.x);
		draw_area.y1 = LV_MATH_MIN(line_end.p_act.y, line_pos.p_act.y);
		draw_area.x2 = LV_MATH_MAX(line_end.p_act.x, line_pos.p_act.x);
		draw_area.y2 = LV_MATH_MAX(line_pos.p_act.y, line_pos.p_act.y);

#if LV_ANTIALIAS
		if(aa_last_step_pos.x != line_pos.p_act.x) {
			line_ver_aa(aa_last_step_pos.x + 1,  aa_last_step_pos.y, line_pos.p_act.y - aa_last_step_pos.y, mask, LV_COLOR_RED, style->line.opa);
			aa_last_step_pos.x = line_pos.p_act.x;
			aa_last_step_pos.y = line_pos.p_act.y;
		}

		if(aa_last_step_end.y != line_end.p_act.y) {
			lv_coord_t seg_w = aa_last_step_end.x - line_end.p_act.x;				/*Segment width*/
			if(line_pos.p_act.x - (aa_last_step_end.x - seg_w) >= width) {
				seg_w = width - (line_pos.p_act.x - aa_last_step_end.x);
			}
			line_hor_aa(aa_last_step_end.x - seg_w,  aa_last_step_end.y, seg_w , mask, LV_COLOR_RED, LV_OPA_50);
			aa_last_step_end.x = line_end.p_act.x;
			aa_last_step_end.y = line_end.p_act.y;
		}
#endif
		/*Be sure the area is not wide ("x" can be more then where the intersection should be)*/
		if(lv_area_get_width(&draw_area) > width) {
			if(width < 0) draw_area.x1 = draw_area.x2;
			else draw_area.x1 = draw_area.x2 - width;
		}
		fill_fp(&draw_area, mask, LV_COLOR_MAGENTA, style->line.opa);

		line_next_y(&line_end);
		line_next_y(&line_pos);

	}while(lv_area_get_width(&draw_area) < width);

#if LV_ANTIALIAS
	/*Anti-aliasing of the last perpendicular segment (only one pixel)*/
	px_fp(draw_area.x1 - 1, draw_area.y1, mask, LV_COLOR_RED, style->line.opa / 2);
#endif

	/*Middle part*/
	if(line_end.p_act.x == line_end.p2.x) line_end.p_act.y ++;	/*If the the perpendicular line is finished (very steep line) then the last y step is missing */
	lv_coord_t perp_height = line_end.p_act.y - line_pos.p1.y;	/*Height of the perpendicular area*/
	printf("perp_height: %d\n", perp_height);

	p1_neg.x = draw_area.x1 = draw_area.x2 - width;
	p1_neg.y = line_end.p_act.y;
	line_init(&line_neg, &p1_neg, &p2_neg);

#if LV_ANTIALIAS
	lv_point_t aa_last_step_neg;
	aa_last_step_neg.x = line_neg.p_act.x;
	aa_last_step_neg.y = line_neg.p_act.y;
#endif

	do {
		draw_area.x1 = LV_MATH_MIN(line_neg.p_act.x, line_pos.p_act.x);
		draw_area.y1 = LV_MATH_MIN(line_neg.p_act.y, line_pos.p_act.y);
		draw_area.x2 = LV_MATH_MAX(line_neg.p_act.x, line_pos.p_act.x);
		draw_area.y2 = LV_MATH_MAX(line_neg.p_act.y, line_pos.p_act.y);
		fill_fp(&draw_area, mask, LV_COLOR_GREEN,  style->line.opa);

#if LV_ANTIALIAS
		if(aa_last_step_neg.x != line_neg.p_act.x) {
			line_ver_aa(aa_last_step_neg.x - 1,  aa_last_step_neg.y, aa_last_step_neg.y - line_neg.p_act.y, mask, LV_COLOR_RED, style->line.opa);
			aa_last_step_neg.x = line_neg.p_act.x;
			aa_last_step_neg.y = line_neg.p_act.y;
		}

		if(aa_last_step_pos.x != line_pos.p_act.x) {
			line_ver_aa(aa_last_step_pos.x + 1,  aa_last_step_pos.y, line_pos.p_act.y - aa_last_step_pos.y, mask, LV_COLOR_RED, style->line.opa);
			aa_last_step_pos.x = line_pos.p_act.x;
			aa_last_step_pos.y = line_pos.p_act.y;
		}
#endif
		if(!line_next_y(&line_neg)) break;
		if(!line_next_y(&line_pos)) break;
	}while(LV_MATH_ABS(line_pos.p_act.y - line_pos.p2.y) >=  LV_MATH_ABS(perp_height));

#if LV_ANTIALIAS
	/*Anti-aliasing of the last segment*/
	line_ver_aa(aa_last_step_pos.x + 1,  aa_last_step_pos.y, line_pos.p2.y - aa_last_step_pos.y - perp_height + 1, mask, LV_COLOR_RED, style->line.opa);

#endif
	/*Bottom perpendicular end*/
	p1_end.x = line_pos.p_act.x;
	p1_end.y = line_pos.p_act.y;
	line_init(&line_end, &p1_end, &line_neg.p2);
#if LV_ANTIALIAS
	aa_last_step_end.x = line_end.p_act.x;
	aa_last_step_end.y = line_end.p_act.y;
#endif

	do {
		draw_area.x1 = LV_MATH_MIN(line_neg.p_act.x, line_end.p_act.x);
		draw_area.y1 = LV_MATH_MIN(line_neg.p_act.y, line_end.p_act.y);
		draw_area.x2 = LV_MATH_MAX(line_neg.p_act.x, line_end.p_act.x);
		draw_area.y2 = LV_MATH_MAX(line_neg.p_act.y, line_end.p_act.y);
		fill_fp(&draw_area, mask, LV_COLOR_MAGENTA,  style->line.opa);

#if LV_ANTIALIAS
		if(aa_last_step_neg.x != line_neg.p_act.x) {
			line_ver_aa(aa_last_step_neg.x - 1,  aa_last_step_neg.y, aa_last_step_neg.y - line_neg.p_act.y, mask, LV_COLOR_RED, style->line.opa);
			aa_last_step_neg.x = line_neg.p_act.x;
			aa_last_step_neg.y = line_neg.p_act.y;
		}

		if(aa_last_step_end.y != line_end.p_act.y) {
			lv_coord_t seg_w = aa_last_step_end.x - line_end.p_act.x;				/*Segment width*/
			if(line_neg.p_act.x - (aa_last_step_end.x - seg_w) >= width) {
				seg_w = width - (line_neg.p_act.x - aa_last_step_end.x);
			}
			line_hor_aa(aa_last_step_end.x - seg_w + 1,  aa_last_step_end.y + 1, -seg_w , mask, LV_COLOR_RED, LV_OPA_50);
			aa_last_step_end.x = line_end.p_act.x;
			aa_last_step_end.y = line_end.p_act.y;
		}

#endif

		if(!line_next_y(&line_neg)) break;
		if(!line_next_y(&line_end)) break;

	}while(1);


#if LV_ANTIALIAS
	line_ver_aa(aa_last_step_neg.x - 1,  aa_last_step_neg.y, aa_last_step_neg.y - line_neg.p2.y - 1, mask, LV_COLOR_RED, style->line.opa);
	lv_coord_t seg_w = aa_last_step_end.x - line_neg.p2.x + 1;				/*Segment width*/
	if(line_neg.p2.x - (aa_last_step_end.x - seg_w) >= width) {
		seg_w = width - (line_neg.p2.x - aa_last_step_end.x);
	}
	line_hor_aa(line_neg.p2.x, line_neg.p2.y + 1, -seg_w , mask, LV_COLOR_RED, style->line.opa);
#endif
}


static void line_init(line_draw_t * line, const lv_point_t * p1, const lv_point_t * p2)
{
	line->p1.x = p1->x;
	line->p1.y = p1->y;
	line->p2.x = p2->x;
	line->p2.y = p2->y;

	line->dx = LV_MATH_ABS(line->p2.x - line->p1.x);
	line->sx = line->p1.x < line->p2.x ? 1 : -1;
	line->dy = LV_MATH_ABS(line->p2.y - line->p1.y);
	line->sy = line->p1.y < line->p2.y ? 1 : -1;
	line->err = (line->dx > line->dy ? line->dx : -line->dy) / 2;
	line->e2 = 0;
	line->hor = line->dx > line->dy ? true : false;	/*Rather horizontal or vertical*/

    line->p_act.x = line->p1.x;
    line->p_act.y = line->p1.y;
}

static bool line_next(line_draw_t * line)
{
	if (line->p_act.x == line->p2.x && line->p_act.y == line->p2.y) return false;
	line->e2 = line->err;
	if (line->e2 >-line->dx) {
		line->err -= line->dy;
		line->p_act.x += line->sx;
	}
	if (line->e2 < line->dy) {
		line->err += line->dx;
		line->p_act.y += line->sy;
	}
	return true;
}

/**
 * Iterate until step one in y direction.
 * @param line
 * @return
 */
static bool line_next_y(line_draw_t * line)
{
	lv_coord_t last_y = line->p_act.y;

	do {
		if(!line_next(line)) return false;
	} while(last_y == line->p_act.y);

	return true;

}

/**
 * Add a vertical  anti-aliasing segment (pixels with decreasing opacity)
 * @param x start point x coordinate
 * @param y start point y coordinate
 * @param length length of segment (negative value to start from 0 opacity)
 * @param mask draw only in this area
 * @param color color of pixels
 * @param opa maximum opacity
 */
static void line_ver_aa(lv_coord_t x, lv_coord_t y, lv_coord_t length, const lv_area_t * mask, lv_color_t color, lv_opa_t opa)
{
	bool aa_inv = false;
	if(length < 0) {
		aa_inv = true;
		length = -length;
	}

	lv_coord_t i;
	for(i = 0; i < length; i++) {
		lv_opa_t px_opa = antialias_get_opa(length, i, opa);
		if(aa_inv) px_opa = opa - px_opa;
		px_fp(x, y + i, mask, color, px_opa);
	}
}

/**
 * Add a horizontal anti-aliasing segment (pixels with decreasing opacity)
 * @param x start point x coordinate
 * @param y start point y coordinate
 * @param length length of segment (negative value to start from 0 opacity)
 * @param mask draw only in this area
 * @param color color of pixels
 * @param opa maximum opacity
 */
static void line_hor_aa(lv_coord_t x, lv_coord_t y, lv_coord_t length, const lv_area_t * mask, lv_color_t color, lv_opa_t opa)
{
	bool aa_inv = false;
	if(length < 0) {
		aa_inv = true;
		length = -length;
	}

	lv_coord_t i;
	for(i = 0; i < length; i++) {
		lv_opa_t px_opa = antialias_get_opa(length, i, opa);
		if(aa_inv) px_opa = opa - px_opa;
		px_fp(x + i, y, mask, color, px_opa);
	}
}
