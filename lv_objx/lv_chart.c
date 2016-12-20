/**
 * @file lv_chart.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_CHART != 0

#include "lv_chart.h"
#include "../lv_draw/lv_draw.h"

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
static bool lv_chart_design(lv_obj_t * chart, const area_t * mask, lv_design_mode_t mode);
static void lv_charts_init(void);
static void lv_chart_draw_div(lv_obj_t * chart, const area_t * mask);
static void lv_chart_draw_lines(lv_obj_t * chart, const area_t * mask);
static void lv_chart_draw_points(lv_obj_t * chart, const area_t * mask);
static void lv_chart_draw_cols(lv_obj_t * chart, const area_t * mask);


/**********************
 *  STATIC VARIABLES
 **********************/
static lv_charts_t lv_charts_def;
static lv_design_f_t ancestor_design_fp;

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
 * @param par pointer to an object, it will be the parent of the new chart background
 * @param copy pointer to a chart background object, if not NULL then the new object will be copied from it
 * @return pointer to the created chart background
 */
lv_obj_t * lv_chart_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor basic object*/
    lv_obj_t * new_chart = lv_rect_create(par, copy);
    dm_assert(new_chart);

    /*Allocate the object type specific extended data*/
    lv_chart_ext_t * ext = lv_obj_alloc_ext(new_chart, sizeof(lv_chart_ext_t));
    dm_assert(ext);

    if(ancestor_design_fp == NULL) {
    	ancestor_design_fp = lv_obj_get_design_f(new_chart);
    }

    ll_init(&ext->dl_ll, sizeof(cord_t *));
    ext->dl_num = 0;

    lv_obj_set_signal_f(new_chart, lv_chart_signal);
    lv_obj_set_design_f(new_chart, lv_chart_design);

    /*Init the new chart background object*/
    if(copy == NULL) {
        ext->type = LV_CHART_LINE;
    	lv_obj_set_style(new_chart, lv_charts_get(LV_CHARTS_DEF, NULL));
        ext->ymin = LV_CHART_YMIN_DEF;
        ext->ymax = LV_CHART_YMAX_DEF;
        ext->hdiv_num = LV_CHART_HDIV_DEF;
        ext->vdiv_num = LV_CHART_VDIV_DEF;
        ext->pnum = LV_CHART_PNUM_DEF;
    } else {
    	lv_chart_ext_t * ext_copy = lv_obj_get_ext(copy);
        ext->type = ext_copy->type;
		ext->ymin = ext_copy->ymin;
		ext->ymax = ext_copy->ymax;
		ext->hdiv_num = ext_copy->hdiv_num;
		ext->vdiv_num = ext_copy->vdiv_num;
        ext->pnum = ext_copy->pnum;
    }

    return new_chart;
}

/**
 * Signal function of the chart background
 * @param chart pointer to a chart background object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_chart_signal(lv_obj_t * chart, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_rect_signal(chart, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	cord_t ** datal;
    	lv_chart_ext_t * ext = lv_obj_get_ext(chart);
    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    			LL_READ(ext->dl_ll, datal) {
    				dm_free(*datal);
    			}

    			ll_clear(&ext->dl_ll);

    		break;
    		default:
    			break;
    	}
    }

    return valid;
}

/**
 * Allocate and add a data line t the chart
 * @param chart pointer to a chart object
 * @return pointer to the allocated data lie (an array for the data points)
 */
cord_t * lv_chart_add_dataline(lv_obj_t * chart)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);
	cord_t ** dl = ll_ins_head(&ext->dl_ll);
	cord_t def = (ext->ymax - ext->ymin) >> 2;	/*1/4 range as default value*/

	if(dl == NULL) return NULL;

	*dl = dm_alloc(sizeof(cord_t) * ext->pnum);

	uint16_t i;
	cord_t * p_tmp = *dl;
	for(i = 0; i < ext->pnum; i++) {
		*p_tmp = def;
		p_tmp++;
	}

	ext->dl_num++;

	return *dl;
}

/**
 * Refresh a chart if its data line has changed
 * @param chart pointer to chart object
 */
void lv_chart_refr(lv_obj_t * chart)
{
	lv_obj_inv(chart);
}
/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of horizontal and vertical division lines
 * @param chart pointer to a graph background object
 * @param hdiv number of horizontal division lines
 * @param vdiv number of vertical division lines
 */
void lv_chart_set_hvdiv(lv_obj_t * chart, uint8_t hdiv, uint8_t vdiv)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);

	ext->hdiv_num = hdiv;
	ext->vdiv_num = vdiv;

	lv_obj_inv(chart);
}

/**
 * Set the minimal and maximal x and y values
 * @param chart pointer to a graph background object
 * @param xmin x minimum value
 * @param xmax x maximum value
 * @param ymin y minimum value
 * @param ymax y maximum value
 */
void lv_chart_set_range(lv_obj_t * chart, cord_t ymin, cord_t ymax)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);

	ext->ymin = ymin;
	ext->ymax = ymax;

	lv_chart_refr(chart);
}

/**
 * Set a new type for a chart
 * @param chart pointer to a chart object
 * @param type new type of the chart (from 'lv_chart_type_t' enum)
 */
void lv_chart_set_type(lv_obj_t * chart, lv_chart_type_t type)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);
	ext->type = type;

	lv_chart_refr(chart);
}

/**
 * Set the number of points on a data line on a chart
 * @param chart pointer r to chart object
 * @param pnum new number of points on the data lines
 */
void lv_chart_set_pnum(lv_obj_t * chart, uint16_t pnum)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);
	cord_t ** y_data;

	if(pnum < 1) pnum = 1;

	LL_READ_BACK(ext->dl_ll, y_data) {
		*y_data = dm_realloc(*y_data, sizeof(cord_t) * pnum);
	}

	ext->pnum = pnum;
	lv_chart_refr(chart);
}

/**
 * Shift all data right and set the most right data on a data line
 * @param chart pointer to chart object
 * @param dl pointer to a data line on 'chart'
 * @param y the new value of the most right data
 */
void lv_chart_set_next(lv_obj_t * chart, cord_t * dl, cord_t y)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);

	uint16_t i;
	for(i = 0; i < ext->pnum - 1; i++) {
		dl[i] = dl[i + 1];
	}

	dl[ext->pnum - 1] = y;
	lv_chart_refr(chart);

}

/*=====================
 * Getter functions
 *====================*/

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_charts_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_charts_t style
 */
lv_charts_t * lv_charts_get(lv_charts_builtin_t style, lv_charts_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_charts_init();
		style_inited = true;
	}

	lv_charts_t * style_p;

	switch(style) {
		case LV_CHARTS_DEF:
			style_p = &lv_charts_def;
			break;
		default:
			style_p = &lv_charts_def;
	}

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_charts_t));
		else memcpy(copy, &lv_charts_def, sizeof(lv_charts_t));
	}

	return style_p;
}

/**
 * Get the type of a chart
 * @param chart pointer to chart object
 * @return type of the chart (from 'lv_chart_t' enum)
 */
lv_chart_type_t lv_chart_get_type(lv_obj_t * chart)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);

	return ext->type;
}

/**
 * Get the data point number per data line on chart
 * @param chart pointer to chart object
 * @return point number on each data line
 */
uint16_t lv_chart_get_pnum(lv_obj_t * chart)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);

	return ext->pnum;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * Handle the drawing related tasks of the chart backgrounds
 * @param chart pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_chart_design(lv_obj_t * chart, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return ancestor_design_fp(chart, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Draw the rectangle ancient*/
		ancestor_design_fp(chart, mask, mode);

		/*Draw the object*/

		lv_chart_ext_t * ext = lv_obj_get_ext(chart);

		lv_chart_draw_div(chart, mask);

		switch(ext->type) {
			case LV_CHART_LINE:
				lv_chart_draw_lines(chart, mask);
				break;
			case LV_CHART_COL:
				lv_chart_draw_cols(chart, mask);
				break;
			case LV_CHART_POINT:
				lv_chart_draw_points(chart, mask);
				break;
			default:
				break;
		}
    }
    return true;
}

/**
 * Draw the division lines on chart background
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void lv_chart_draw_div(lv_obj_t * chart, const area_t * mask)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);
	lv_charts_t * style = lv_obj_get_style(chart);

	uint8_t div_i;
	point_t p1;
	point_t p2;
	cord_t w = lv_obj_get_width(chart);
	cord_t h = lv_obj_get_height(chart);
	opa_t div_opa = (uint16_t)lv_obj_get_opa(chart) * style->div_line_opa / 100;
	cord_t x_ofs = lv_obj_get_x(chart);
	cord_t y_ofs = lv_obj_get_y(chart);
	p1.x = 0 + x_ofs;
	p2.x = w + x_ofs;
	for(div_i = 1; div_i <= ext->hdiv_num; div_i ++) {
		p1.y = (int32_t)((int32_t)h * div_i) / (ext->hdiv_num + 1);
		p1.y +=  y_ofs;
		p2.y = p1.y;
		lv_draw_line(&p1, &p2, mask, &style->div_lines, div_opa);
	}

	p1.y = 0 + y_ofs;
	p2.y = h + y_ofs;
	for(div_i = 1; div_i <= ext->vdiv_num; div_i ++) {
		p1.x = (int32_t)((int32_t)w * div_i) / (ext->vdiv_num + 1);
		p1.x +=  x_ofs;
		p2.x = p1.x;
		lv_draw_line(&p1, &p2, mask, &style->div_lines, div_opa);
	}
}

/**
 * Draw the data lines as lines on a chart
 * @param obj pointer to chart object
 */
static void lv_chart_draw_lines(lv_obj_t * chart, const area_t * mask)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);
	lv_charts_t * style_p = lv_obj_get_style(chart);

	uint8_t i;
	point_t p1;
	point_t p2;
	cord_t w = lv_obj_get_width(chart);
	cord_t h = lv_obj_get_height(chart);
	opa_t opa = (uint16_t)lv_obj_get_opa(chart) * style_p->data_opa / 100;
	cord_t x_ofs = lv_obj_get_x(chart);
	cord_t y_ofs = lv_obj_get_y(chart);
	int32_t y_tmp;
	cord_t ** y_data;
	uint8_t dl_cnt = 0;
	lv_lines_t lines;
	lv_lines_get(LV_LINES_CHART, &lines);

	/*Go through all data lines*/
	LL_READ_BACK(ext->dl_ll, y_data) {
		lines.objs.color = style_p->color[dl_cnt];
		lines.width = style_p->width;

		p1.x = 0 + x_ofs;
		p2.x = 0 + x_ofs;
		y_tmp = (int32_t)((int32_t) (*y_data)[0] - ext->ymin) * h;
		y_tmp = y_tmp / (ext->ymax - ext->ymin);
		p2.y = h - y_tmp + y_ofs;

		for(i = 1; i < ext->pnum; i ++) {
			p1.x = p2.x;
			p1.y = p2.y;

			p2.x = (w / (ext->pnum - 1)) * i + x_ofs;

			y_tmp = (int32_t)((int32_t) (*y_data)[i] - ext->ymin) * h;
			y_tmp = y_tmp / (ext->ymax - ext->ymin);
			p2.y = h - y_tmp + y_ofs;

			lv_draw_line(&p1, &p2, mask, &lines, opa);
		}
		dl_cnt++;
	}
}

/**
 * Draw the data lines as points on a chart
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void lv_chart_draw_points(lv_obj_t * chart, const area_t * mask)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);
	lv_charts_t * style_p = lv_obj_get_style(chart);

	uint8_t i;
	area_t cir_a;
	cord_t w = lv_obj_get_width(chart);
	cord_t h = lv_obj_get_height(chart);
	opa_t opa = (uint16_t)lv_obj_get_opa(chart) * style_p->data_opa / 100;
	cord_t x_ofs = lv_obj_get_x(chart);
	cord_t y_ofs = lv_obj_get_y(chart);
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
	LL_READ_BACK(ext->dl_ll, y_data) {
		rects.objs.color = style_p->color[dl_cnt];
		rects.gcolor = color_mix(COLOR_BLACK, style_p->color[dl_cnt], style_p->dark_eff);

		for(i = 0; i < ext->pnum; i ++) {
			cir_a.x1 = (w / (ext->pnum - 1)) * i + x_ofs;
			cir_a.x2 = cir_a.x1 + rad;
			cir_a.x1 -= rad;

			y_tmp = (int32_t)((int32_t) (*y_data)[i] - ext->ymin) * h;
			y_tmp = y_tmp / (ext->ymax - ext->ymin);
			cir_a.y1 = h - y_tmp + y_ofs;
			cir_a.y2 = cir_a.y1 + rad;
			cir_a.y1 -= rad;

			lv_draw_rect(&cir_a, mask, &rects, opa);
		}
		dl_cnt++;
	}
}

/**
 * Draw the data lines as columns on a chart
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void lv_chart_draw_cols(lv_obj_t * chart, const area_t * mask)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);
	lv_charts_t * style_p = lv_obj_get_style(chart);

	uint8_t i;
	area_t col_a;
	area_t col_mask;
	bool mask_ret;
	cord_t w = lv_obj_get_width(chart);
	cord_t h = lv_obj_get_height(chart);
	opa_t opa = (uint16_t)lv_obj_get_opa(chart) * style_p->data_opa / 100;
	int32_t y_tmp;
	cord_t ** y_data;
	uint8_t dl_cnt = 0;
	lv_rects_t rects;
	cord_t x_ofs = w / ((ext->dl_num + 1) * ext->pnum) / 2; /*Shift with a half col.*/

	lv_rects_get(LV_RECTS_DEF, &rects);
	rects.bwidth = 0;
	rects.empty = 0;
	rects.round = 0;

	col_a.y2 = chart->cords.y2;

	/*Go through all data lines*/
	LL_READ_BACK(ext->dl_ll, y_data) {
		rects.objs.color = style_p->color[dl_cnt];
		rects.gcolor = color_mix(COLOR_BLACK, style_p->color[dl_cnt], style_p->dark_eff);

		for(i = 0; i < ext->pnum; i ++) {
			/*Calculate the x coordinates. Suppose (dl_num + 1) * pnum columns */
			col_a.x1 = (int32_t)((int32_t)w * (i * (ext->dl_num + 1) + dl_cnt)) /
					   (ext->pnum * (ext->dl_num + 1)) + chart->cords.x1;
			col_a.x2 = (int32_t)((int32_t)w * (i * (ext->dl_num + 1) + dl_cnt + 1 )) /
					   (ext->pnum * (ext->dl_num + 1)) + chart->cords.x1 - 1;

			col_a.x1 += x_ofs;
			col_a.x2 += x_ofs;


			y_tmp = (int32_t)((int32_t) (*y_data)[i] - ext->ymin) * h;
			y_tmp = y_tmp / (ext->ymax - ext->ymin);
			col_a.y1 = h - y_tmp + chart->cords.y1;

			mask_ret = area_union(&col_mask, mask, &col_a);
			if(mask_ret != false) {
				lv_draw_rect(&chart->cords, &col_mask, &rects, opa);
			}
		}
		dl_cnt++;
	}
}

/**
 * Initialize the chart styles
 */
static void lv_charts_init(void)
{
	/*Default style*/
	/* Background */
	lv_rects_get(LV_RECTS_DEF, &lv_charts_def.bg_rects);
	lv_charts_def.bg_rects.objs.color = COLOR_MAKE(0x60, 0x80, 0xA0);
	lv_charts_def.bg_rects.gcolor = COLOR_WHITE;
	lv_charts_def.bg_rects.bcolor = COLOR_BLACK;

	/* Div. line */
	lv_lines_get(LV_LINES_DECOR, &lv_charts_def.div_lines);
	lv_charts_def.div_lines.width = 1 * LV_DOWNSCALE;
	lv_charts_def.div_lines.objs.color = COLOR_BLACK;
	lv_charts_def.div_line_opa = OPA_COVER;

	/*Data lines*/
	lv_charts_def.width = 3 * LV_DOWNSCALE;
	lv_charts_def.data_opa = 100;
	lv_charts_def.dark_eff = 150;
	lv_charts_def.color[0] = COLOR_RED;
	lv_charts_def.color[1] = COLOR_LIME;
	lv_charts_def.color[2] = COLOR_BLUE;
	lv_charts_def.color[3] = COLOR_MAGENTA;
	lv_charts_def.color[4] = COLOR_CYAN;
	lv_charts_def.color[5] = COLOR_YELLOW;
	lv_charts_def.color[6] = COLOR_WHITE;
	lv_charts_def.color[7] = COLOR_GRAY;
}

#endif
