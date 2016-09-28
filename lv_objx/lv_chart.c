/**
 * @file lv_chart.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_CHART != 0

#include <lvgl/lv_objx/lv_chart.h>
#include "lvgl/lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define LV_CHART_YMIN_DEF	0
#define LV_CHART_YMAX_DEF	100
#define LV_CHART_HDIV_DEF	3
#define LV_CHART_VDIV_DEF	5
#define LV_CHART_PNUM_DEF	10

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_chart_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);
static void lv_chart_draw_div(lv_obj_t* obj_dp, const area_t * mask_p);
static void lv_chart_draw_lines(lv_obj_t* obj_dp, const area_t * mask_p);
static void lv_chart_draw_points(lv_obj_t* obj_dp, const area_t * mask_p);
static void lv_chart_draw_cols(lv_obj_t* obj_dp, const area_t * mask_p);


/**********************
 *  STATIC VARIABLES
 **********************/
static bool (*ancestor_design_fp)(lv_obj_t*, const area_t *, lv_design_mode_t);
static lv_charts_t lv_charts_def =
{
	/* Background */
	.bg_rects.objs.color = COLOR_MAKE(0x60, 0x80, 0xA0),
	.bg_rects.gcolor = COLOR_WHITE,
	.bg_rects.bcolor = COLOR_BLACK,
	.bg_rects.bwidth = 2 * LV_STYLE_MULT,
	.bg_rects.bopa = 50,
	.bg_rects.empty = 0,
	.bg_rects.round = 4 * LV_STYLE_MULT,
	.bg_rects.hpad = 10 * LV_STYLE_MULT,
	.bg_rects.vpad = 15 * LV_STYLE_MULT,
	.bg_rects.opad = 3 * LV_STYLE_MULT,

	/* Div. line */
	.div_lines.width = 1 * LV_STYLE_MULT,
	.div_lines.objs.color = COLOR_BLACK,
	.div_line_opa = 100,

	/*Data lines*/
	.width = 3 * LV_STYLE_MULT,
	.data_opa = 100,
	.dark_eff = 150,
	.color[0] = COLOR_RED,
	.color[1] = COLOR_LIME,
	.color[2] = COLOR_BLUE,
	.color[3] = COLOR_MAGENTA,
	.color[4] = COLOR_CYAN,
	.color[5] = COLOR_YELLOW,
	.color[6] = COLOR_WHITE,
	.color[7] = COLOR_GRAY,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*-----------------
 * Create function
 *-----------------*/

/**
 * Create a chart background objects
 * @param par_dp pointer to an object, it will be the parent of the new chart background
 * @param copy_dp pointer to a chart background object, if not NULL then the new object will be copied from it
 * @return pointer to the created chart background
 */
lv_obj_t* lv_chart_create(lv_obj_t* par_dp, lv_obj_t * copy_dp)
{
    /*Create the ancestor basic object*/
    lv_obj_t * new_obj_dp = lv_rect_create(par_dp, copy_dp);
    dm_assert(new_obj_dp);

    /*Allocate the object type specific extended data*/
    lv_chart_ext_t * ext_dp = lv_obj_alloc_ext(new_obj_dp, sizeof(lv_chart_ext_t));
    dm_assert(ext_dp);

    if(ancestor_design_fp == NULL) {
    	ancestor_design_fp = lv_obj_get_design_f(new_obj_dp);
    }

    ll_init(&ext_dp->dl_ll, sizeof(cord_t *));
    ext_dp->dl_num = 0;

    lv_obj_set_signal_f(new_obj_dp, lv_chart_signal);
    lv_obj_set_design_f(new_obj_dp, lv_chart_design);

    /*Init the new chart background object*/
    if(copy_dp == NULL) {
        ext_dp->type = LV_CHART_LINE;
    	lv_obj_set_style(new_obj_dp, &lv_charts_def);
        ext_dp->ymin = LV_CHART_YMIN_DEF;
        ext_dp->ymax = LV_CHART_YMAX_DEF;
        ext_dp->hdiv_num = LV_CHART_HDIV_DEF;
        ext_dp->vdiv_num = LV_CHART_VDIV_DEF;
        ext_dp->pnum = LV_CHART_PNUM_DEF;
    } else {
    	lv_chart_ext_t * ext_copy_dp = lv_obj_get_ext(copy_dp);
        ext_dp->type = ext_copy_dp->type;
		ext_dp->ymin = ext_copy_dp->ymin;
		ext_dp->ymax = ext_copy_dp->ymax;
		ext_dp->hdiv_num = ext_copy_dp->hdiv_num;
		ext_dp->vdiv_num = ext_copy_dp->vdiv_num;
        ext_dp->pnum = ext_copy_dp->pnum;
    }

    return new_obj_dp;
}

/**
 * Signal function of the chart background
 * @param obj_dp pointer to a chart background object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_chart_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_rect_signal(obj_dp, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	cord_t ** datal;
    	lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    			LL_READ(ext_dp->dl_ll, datal) {
    				dm_free(*datal);
    			}

    			ll_clear(&ext_dp->dl_ll);

    		break;
    		default:
    			break;
    	}
    }

    return valid;
}

cord_t * lv_chart_add_dataline(lv_obj_t* obj_dp)
{
	lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
	cord_t ** dl_dp = ll_ins_head(&ext_dp->dl_ll);
	cord_t def = (ext_dp->ymax - ext_dp->ymin) >> 2;	/*1/4 range as default value*/

	if(dl_dp == NULL) return NULL;

	*dl_dp = dm_alloc(sizeof(cord_t) * ext_dp->pnum);

	uint16_t i;
	cord_t * p_tmp = *dl_dp;
	for(i = 0; i < ext_dp->pnum; i++) {
		*p_tmp = def;
		p_tmp++;
	}

	ext_dp->dl_num++;

	return *dl_dp;
}

/**
 * Refresh a chart if its data line has changed
 * @param obj_dp pionter to chart object
 */
void lv_chart_refr(lv_obj_t * obj_dp)
{
	lv_obj_inv(obj_dp);
}
/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of horizontal and vertical division lines
 * @param obj_dp pointer to a graph background object
 * @param hdiv number of horizontal division lines
 * @param vdiv number of vertical division lines
 */
void lv_chart_set_hvdiv(lv_obj_t * obj_dp, uint8_t hdiv, uint8_t vdiv)
{
	lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);

	ext_dp->hdiv_num = hdiv;
	ext_dp->vdiv_num = vdiv;

	lv_obj_inv(obj_dp);
}

/**
 * Set the minimal and maximal x and y values
 * @param obj_dp pointer to a graph background object
 * @param xmin x minimum value
 * @param xmax x maximum value
 * @param ymin y minimum value
 * @param ymax y maximum value
 */
void lv_chart_set_range(lv_obj_t * obj_dp, cord_t ymin, cord_t ymax)
{
	lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);

	ext_dp->ymin = ymin;
	ext_dp->ymax = ymax;

	lv_chart_refr(obj_dp);
}

/**
 * Set a new type for a chart
 * @param obj_dp pointer to a chart object
 * @param type ew type of the chart (from 'lv_chart_type_t' enum)
 */
void lv_chart_set_type(lv_obj_t * obj_dp, lv_chart_type_t type)
{
	lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
	ext_dp->type = type;

	lv_chart_refr(obj_dp);
}

/**
 * Set the number of points on a data line on a chart
 * @param obj_dp pointer r to chart object
 * @param pnum new number of points on the data lines
 */
void lv_chart_set_pnum(lv_obj_t * obj_dp, uint16_t pnum)
{
	lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
	cord_t ** y_data;

	if(pnum < 1) pnum = 1;

	LL_READ_BACK(ext_dp->dl_ll, y_data) {
		*y_data = dm_realloc(*y_data, sizeof(cord_t) * pnum);
	}

	ext_dp->pnum = pnum;
	lv_chart_refr(obj_dp);
}

/**
 * Shift all data right and set the most right data on a data line
 * @param obj_dp pointer to chart object
 * @param dl_p pointer to a data line on 'obj_dp'
 * @param y the new value of the most right data
 */
void lv_chart_set_next(lv_obj_t * obj_dp, cord_t * dl_p, cord_t y)
{
	lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);

	uint16_t i;
	for(i = 0; i < ext_dp->pnum - 1; i++) {
		dl_p[i] = dl_p[i + 1];
	}

	dl_p[ext_dp->pnum - 1] = y;
	lv_chart_refr(obj_dp);

}

/*=====================
 * Getter functions
 *====================*/

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_charts_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_charts_t style
 */
lv_charts_t * lv_charts_get(lv_charts_builtin_t style, lv_charts_t * copy_p)
{
	lv_charts_t * style_p;

	switch(style) {
		case LV_CHARTS_DEF:
			style_p = &lv_charts_def;
			break;
		default:
			style_p = &lv_charts_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_charts_t));
		else memcpy(copy_p, &lv_charts_def, sizeof(lv_charts_t));
	}

	return style_p;
}

/**
 * Get the type of a chart
 * @param obj_dp pointer to chart object
 * @return type of the chart (from 'lv_chart_t' enum)
 */
lv_chart_type_t lv_chart_get_type(lv_obj_t * obj_dp)
{
	lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);

	return ext_dp->type;
}

/**
 * Get the data point number per data line on chart
 * @param obj_dp pointer to chart object
 * @return point number on each data line
 */
uint16_t lv_chart_get_pnum(lv_obj_t * obj_dp)
{
	lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);

	return ext_dp->pnum;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * Handle the drawing related tasks of the chart backgrounds
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'
 */
static bool lv_chart_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return ancestor_design_fp(obj_dp, mask_p, mode);
    }

    /*Draw the rectangle ancient*/
    ancestor_design_fp(obj_dp, mask_p, mode);

    /*Draw the object*/

    lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);

    lv_chart_draw_div(obj_dp, mask_p);

    switch(ext_dp->type) {
    	case LV_CHART_LINE:
    		lv_chart_draw_lines(obj_dp, mask_p);
    		break;
    	case LV_CHART_COL:
    		lv_chart_draw_cols(obj_dp, mask_p);
    		break;
    	case LV_CHART_POINT:
    		lv_chart_draw_points(obj_dp, mask_p);
    		break;
    	default:
    		break;
    }

    return true;
}

/**
 * Draw the division lines on chart background
 * @param obj_dp pointer to chart object
 * @param mask_p mask, inherited from the design function
 */
static void lv_chart_draw_div(lv_obj_t* obj_dp, const area_t * mask_p)
{
	lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
	lv_charts_t * style_p = lv_obj_get_style(obj_dp);

	uint8_t div_i;
	point_t p1;
	point_t p2;
	cord_t w = lv_obj_get_width(obj_dp);
	cord_t h = lv_obj_get_height(obj_dp);
	opa_t div_opa = (uint16_t)lv_obj_get_opa(obj_dp) * style_p->div_line_opa / 100;
	cord_t x_ofs = lv_obj_get_x(obj_dp);
	cord_t y_ofs = lv_obj_get_y(obj_dp);
	p1.x = 0 + x_ofs;
	p2.x = w + x_ofs;
	for(div_i = 1; div_i <= ext_dp->hdiv_num; div_i ++) {
		p1.y = (int32_t)((int32_t)h * div_i) / (ext_dp->hdiv_num + 1);
		p1.y +=  y_ofs;
		p2.y = p1.y;
		lv_draw_line(&p1, &p2, mask_p, &style_p->div_lines, div_opa);
	}

	p1.y = 0 + y_ofs;
	p2.y = h + y_ofs;
	for(div_i = 1; div_i <= ext_dp->vdiv_num; div_i ++) {
		p1.x = (int32_t)((int32_t)w * div_i) / (ext_dp->vdiv_num + 1);
		p1.x +=  x_ofs;
		p2.x = p1.x;
		lv_draw_line(&p1, &p2, mask_p, &style_p->div_lines, div_opa);
	}
}

/**
 * Draw the data lines as lines on a chart
 * @param obj_dp pointer to chart object
 * @param mask_p mask, inherited from the design function
 */
static void lv_chart_draw_lines(lv_obj_t* obj_dp, const area_t * mask_p)
{
	lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
	lv_charts_t * style_p = lv_obj_get_style(obj_dp);

	uint8_t i;
	point_t p1;
	point_t p2;
	cord_t w = lv_obj_get_width(obj_dp);
	cord_t h = lv_obj_get_height(obj_dp);
	opa_t opa = (uint16_t)lv_obj_get_opa(obj_dp) * style_p->data_opa / 100;
	cord_t x_ofs = lv_obj_get_x(obj_dp);
	cord_t y_ofs = lv_obj_get_y(obj_dp);
	int32_t y_tmp;
	cord_t ** y_data;
	uint8_t dl_cnt = 0;
	lv_lines_t lines;
	lv_lines_get(LV_LINES_CHART, &lines);

	/*Go through all data lines*/
	LL_READ_BACK(ext_dp->dl_ll, y_data) {

		lines.objs.color = style_p->color[dl_cnt];
		lines.width = style_p->width;

		p1.x = 0 + x_ofs;
		p2.x = 0 + x_ofs;
		y_tmp = (int32_t)((int32_t) (*y_data)[0] - ext_dp->ymin) * h;
		y_tmp = y_tmp / (ext_dp->ymax - ext_dp->ymin);
		p2.y = h - y_tmp + y_ofs;

		for(i = 1; i < ext_dp->pnum; i ++) {
			p1.x = p2.x;
			p1.y = p2.y;

			p2.x = (w / (ext_dp->pnum - 1)) * i + x_ofs;

			y_tmp = (int32_t)((int32_t) (*y_data)[i] - ext_dp->ymin) * h;
			y_tmp = y_tmp / (ext_dp->ymax - ext_dp->ymin);
			p2.y = h - y_tmp + y_ofs;

			lv_draw_line(&p1, &p2, mask_p, &lines, opa);
		}
		dl_cnt++;
	}
}

/**
 * Draw the data lines as points on a chart
 * @param obj_dp pointer to chart object
 * @param mask_p mask, inherited from the design function
 */
static void lv_chart_draw_points(lv_obj_t* obj_dp, const area_t * mask_p)
{
	lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
	lv_charts_t * style_p = lv_obj_get_style(obj_dp);

	uint8_t i;
	area_t cir_a;
	cord_t w = lv_obj_get_width(obj_dp);
	cord_t h = lv_obj_get_height(obj_dp);
	opa_t opa = (uint16_t)lv_obj_get_opa(obj_dp) * style_p->data_opa / 100;
	cord_t x_ofs = lv_obj_get_x(obj_dp);
	cord_t y_ofs = lv_obj_get_y(obj_dp);
	int32_t y_tmp;
	cord_t ** y_data;
	uint8_t dl_cnt = 0;
	lv_rects_t rects;
	cord_t rad = style_p->width;

	lv_rects_get(LV_RECTS_DEF, &rects);
	rects.bwidth = 0;
	rects.empty = 0;
	rects.round = LV_RECT_CIRCLE;

	/*Go through all data lines*/
	LL_READ_BACK(ext_dp->dl_ll, y_data) {
		rects.objs.color = style_p->color[dl_cnt];
		rects.gcolor = color_mix(COLOR_BLACK, style_p->color[dl_cnt], style_p->dark_eff);

		for(i = 0; i < ext_dp->pnum; i ++) {
			cir_a.x1 = (w / (ext_dp->pnum - 1)) * i + x_ofs;
			cir_a.x2 = cir_a.x1 + rad;
			cir_a.x1 -= rad;

			y_tmp = (int32_t)((int32_t) (*y_data)[i] - ext_dp->ymin) * h;
			y_tmp = y_tmp / (ext_dp->ymax - ext_dp->ymin);
			cir_a.y1 = h - y_tmp + y_ofs;
			cir_a.y2 = cir_a.y1 + rad;
			cir_a.y1 -= rad;

			lv_draw_rect(&cir_a, mask_p, &rects, opa);
		}
		dl_cnt++;
	}
}

/**
 * Draw the data lines as columns on a chart
 * @param obj_dp pointer to chart object
 * @param mask_p mask, inherited from the design function
 */
static void lv_chart_draw_cols(lv_obj_t* obj_dp, const area_t * mask_p)
{
	lv_chart_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
	lv_charts_t * style_p = lv_obj_get_style(obj_dp);

	uint8_t i;
	area_t col_a;
	area_t col_mask;
	bool mask_ret;
	cord_t w = lv_obj_get_width(obj_dp);
	cord_t h = lv_obj_get_height(obj_dp);
	opa_t opa = (uint16_t)lv_obj_get_opa(obj_dp) * style_p->data_opa / 100;
	int32_t y_tmp;
	cord_t ** y_data;
	uint8_t dl_cnt = 0;
	lv_rects_t rects;
	cord_t x_ofs = w / ((ext_dp->dl_num + 1) * ext_dp->pnum) / 2; /*Shift with a half col.*/

	lv_rects_get(LV_RECTS_DEF, &rects);
	rects.bwidth = 0;
	rects.empty = 0;
	rects.round = 0;

	col_a.y2 = obj_dp->cords.y2;

	/*Go through all data lines*/
	LL_READ_BACK(ext_dp->dl_ll, y_data) {
		rects.objs.color = style_p->color[dl_cnt];
		rects.gcolor = color_mix(COLOR_BLACK, style_p->color[dl_cnt], style_p->dark_eff);

		for(i = 0; i < ext_dp->pnum; i ++) {
			/*Calculate the x coordinates. Suppose (dl_num + 1) * pnum columns */
			col_a.x1 = (int32_t)((int32_t)w * (i * (ext_dp->dl_num + 1) + dl_cnt)) /
					   (ext_dp->pnum * (ext_dp->dl_num + 1)) + obj_dp->cords.x1;
			col_a.x2 = (int32_t)((int32_t)w * (i * (ext_dp->dl_num + 1) + dl_cnt + 1 )) /
					   (ext_dp->pnum * (ext_dp->dl_num + 1)) + obj_dp->cords.x1 - 1;

			col_a.x1 += x_ofs;
			col_a.x2 += x_ofs;


			y_tmp = (int32_t)((int32_t) (*y_data)[i] - ext_dp->ymin) * h;
			y_tmp = y_tmp / (ext_dp->ymax - ext_dp->ymin);
			col_a.y1 = h - y_tmp + obj_dp->cords.y1;

			mask_ret = area_union(&col_mask, mask_p, &col_a);
			if(mask_ret != false) {
				lv_draw_rect(&obj_dp->cords, &col_mask, &rects, opa);
			}
		}
		dl_cnt++;
	}
}

#endif
