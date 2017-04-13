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
static void lv_chart_draw_div(lv_obj_t * chart, const area_t * mask);
static void lv_chart_draw_lines(lv_obj_t * chart, const area_t * mask);
static void lv_chart_draw_points(lv_obj_t * chart, const area_t * mask);
static void lv_chart_draw_cols(lv_obj_t * chart, const area_t * mask);


/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_f_t ancestor_design_f;

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
    lv_obj_t * new_chart = lv_obj_create(par, copy);
    dm_assert(new_chart);

    /*Allocate the object type specific extended data*/
    lv_chart_ext_t * ext = lv_obj_alloc_ext(new_chart, sizeof(lv_chart_ext_t));
    dm_assert(ext);
    ll_init(&ext->dl_ll, sizeof(lv_chart_dl_t));
    ext->dl_num = 0;
    ext->ymin = LV_CHART_YMIN_DEF;
    ext->ymax = LV_CHART_YMAX_DEF;
    ext->hdiv_num = LV_CHART_HDIV_DEF;
    ext->vdiv_num = LV_CHART_VDIV_DEF;
    ext->pnum = LV_CHART_PNUM_DEF;
    ext->type = LV_CHART_LINE;
    ext->data_opa = OPA_COVER;
    ext->dark_eff = OPA_50;

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_chart);

    lv_obj_set_signal_f(new_chart, lv_chart_signal);
    lv_obj_set_design_f(new_chart, lv_chart_design);

    /*Init the new chart background object*/
    if(copy == NULL) {
    	lv_obj_set_style(new_chart, lv_style_get(LV_STYLE_PRETTY, NULL));
    } else {
    	lv_chart_ext_t * ext_copy = lv_obj_get_ext(copy);
        ext->type = ext_copy->type;
		ext->ymin = ext_copy->ymin;
		ext->ymax = ext_copy->ymax;
		ext->hdiv_num = ext_copy->hdiv_num;
		ext->vdiv_num = ext_copy->vdiv_num;
        ext->pnum = ext_copy->pnum;
        ext->data_opa =  ext_copy->data_opa;

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_chart);
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
    valid = lv_obj_signal(chart, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	cord_t ** datal;
    	lv_chart_ext_t * ext = lv_obj_get_ext(chart);
    	if(sign == LV_SIGNAL_CLEANUP) {
            LL_READ(ext->dl_ll, datal) {
                dm_free(*datal);
            }
            ll_clear(&ext->dl_ll);
    	}
    }

    return valid;
}

/**
 * Allocate and add a data line to the chart
 * @param chart pointer to a chart object
 * @param color color of the data line
 * @param width line width/point radius/column width
 * @return pointer to the allocated data line (
 */
lv_chart_dl_t * lv_chart_add_dataline(lv_obj_t * chart, color_t color, cord_t width)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);
	lv_chart_dl_t * dl = ll_ins_head(&ext->dl_ll);
	cord_t def = (ext->ymax - ext->ymin) >> 2;	/*1/4 range as default value*/

	if(dl == NULL) return NULL;

	dl->width = width;
    dl->color = color;

	dl->points = dm_alloc(sizeof(cord_t) * ext->pnum);

	uint16_t i;
	cord_t * p_tmp = dl->points;
	for(i = 0; i < ext->pnum; i++) {
		*p_tmp = def;
		p_tmp++;
	}

	ext->dl_num++;

	return dl;
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
	lv_chart_dl_t * dl;

	if(pnum < 1) pnum = 1;

	LL_READ_BACK(ext->dl_ll, dl) {
		dl->points = dm_realloc(dl->points, sizeof(cord_t) * pnum);
	}

	ext->pnum = pnum;
	lv_chart_refr(chart);
}

/**
 * Set the opacity of the data lines
 * @param chart pointer to chart object
 * @param opa opacity of the data lines
 */
void lv_chart_set_data_opa(lv_obj_t * chart, opa_t opa)
{
    lv_chart_ext_t * ext = lv_obj_get_ext(chart);
    ext->data_opa = opa;
}

/**
 * Set the dark effect on the bottom of the points or columns
 * @param chart pointer to chart object
 * @param dark_eff dark effect level (OPA_TRANSP to turn off)
 */
void lv_chart_set_drak_effect(lv_obj_t * chart, opa_t dark_eff)
{
    lv_chart_ext_t * ext = lv_obj_get_ext(chart);
    ext->dark_eff = dark_eff;
}
/**
 * Shift all data right and set the most right data on a data line
 * @param chart pointer to chart object
 * @param dl pointer to a data line on 'chart'
 * @param y the new value of the most right data
 */
void lv_chart_set_next(lv_obj_t * chart, lv_chart_dl_t * dl, cord_t y)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);

	uint16_t i;
	for(i = 0; i < ext->pnum - 1; i++) {
		dl->points[i] = dl->points[i + 1];
	}

	dl->points[ext->pnum - 1] = y;
	lv_chart_refr(chart);

}


/*=====================
 * Getter functions
 *====================*/

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

/**
 * Get the opacity of the data lines
 * @param chart pointer to chart object
 * @return the opacity of the data lines
 */
opa_t lv_chart_get_data_opa(lv_obj_t * chart)
{
    lv_chart_ext_t * ext = lv_obj_get_ext(chart);
    return ext->data_opa;
}

/**
 * Get the dark effect level on the bottom of the points or columns
 * @param chart pointer to chart object
 * @return dark effect level (OPA_TRANSP to turn off)
 */
opa_t lv_chart_get_dark_effect(lv_obj_t * chart, opa_t dark_eff)
{
    lv_chart_ext_t * ext = lv_obj_get_ext(chart);
    return ext->dark_eff;
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
    	return ancestor_design_f(chart, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Draw the background*/
        lv_draw_rect(&chart->cords, mask, lv_obj_get_style(chart));

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
	lv_style_t * style = lv_obj_get_style(chart);

	uint8_t div_i;
	point_t p1;
	point_t p2;
	cord_t w = lv_obj_get_width(chart);
	cord_t h = lv_obj_get_height(chart);
	cord_t x_ofs = chart->cords.x1;
	cord_t y_ofs = chart->cords.y1;
	p1.x = 0 + x_ofs;
	p2.x = w + x_ofs;
	for(div_i = 1; div_i <= ext->hdiv_num; div_i ++) {
		p1.y = (int32_t)((int32_t)h * div_i) / (ext->hdiv_num + 1);
		p1.y +=  y_ofs;
		p2.y = p1.y;
		lv_draw_line(&p1, &p2, mask, style);
	}

	p1.y = 0 + y_ofs;
	p2.y = h + y_ofs;
	for(div_i = 1; div_i <= ext->vdiv_num; div_i ++) {
		p1.x = (int32_t)((int32_t)w * div_i) / (ext->vdiv_num + 1);
		p1.x +=  x_ofs;
		p2.x = p1.x;
		lv_draw_line(&p1, &p2, mask, style);
	}
}

/**
 * Draw the data lines as lines on a chart
 * @param obj pointer to chart object
 */
static void lv_chart_draw_lines(lv_obj_t * chart, const area_t * mask)
{
	lv_chart_ext_t * ext = lv_obj_get_ext(chart);
	lv_style_t * style = lv_obj_get_style(chart);

	uint8_t i;
	point_t p1;
	point_t p2;
	cord_t w = lv_obj_get_width(chart);
	cord_t h = lv_obj_get_height(chart);
    cord_t x_ofs = chart->cords.x1;
    cord_t y_ofs = chart->cords.y1;
	int32_t y_tmp;
	lv_chart_dl_t * dl;
	uint8_t dl_cnt = 0;
	lv_style_t lines;
	lv_style_get(LV_STYLE_PLAIN, &lines);
	lines.opa = (uint16_t)((uint16_t)style->opa * ext->data_opa) >> 8;

	/*Go through all data lines*/
	LL_READ_BACK(ext->dl_ll, dl) {
	    lines.line_width = dl->width;
		lines.ccolor = dl->color;

		p1.x = 0 + x_ofs;
		p2.x = 0 + x_ofs;
		y_tmp = (int32_t)((int32_t) dl->points[0] - ext->ymin) * h;
		y_tmp = y_tmp / (ext->ymax - ext->ymin);
		p2.y = h - y_tmp + y_ofs;

		for(i = 1; i < ext->pnum; i ++) {
			p1.x = p2.x;
			p1.y = p2.y;

			p2.x = ((w * i) / (ext->pnum - 1)) + x_ofs;

			y_tmp = (int32_t)((int32_t) dl->points[i] - ext->ymin) * h;
			y_tmp = y_tmp / (ext->ymax - ext->ymin);
			p2.y = h - y_tmp + y_ofs;

			lv_draw_line(&p1, &p2, mask, &lines);
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
	lv_style_t * style = lv_obj_get_style(chart);

	uint8_t i;
	area_t cir_a;
	cord_t w = lv_obj_get_width(chart);
	cord_t h = lv_obj_get_height(chart);
    cord_t x_ofs = chart->cords.x1;
    cord_t y_ofs = chart->cords.y1;
	int32_t y_tmp;
    lv_chart_dl_t * dl;
    uint8_t dl_cnt = 0;
    lv_style_t rects;
    lv_style_get(LV_STYLE_PLAIN, &rects);

	rects.bwidth = 0;
	rects.empty = 0;
	rects.radius = LV_RECT_CIRCLE;
    rects.opa = (uint16_t)((uint16_t)style->opa * ext->data_opa) >> 8;

	/*Go through all data lines*/
	LL_READ_BACK(ext->dl_ll, dl) {
	    rects.radius = dl->width;
		rects.mcolor = dl->color;
		rects.gcolor = color_mix(COLOR_BLACK, dl->color, ext->dark_eff);

		for(i = 0; i < ext->pnum; i ++) {
			cir_a.x1 = ((w * i) / (ext->pnum - 1)) + x_ofs;
			cir_a.x2 = cir_a.x1 + rects.radius;
			cir_a.x1 -= rects.radius;

			y_tmp = (int32_t)((int32_t) dl->points[i] - ext->ymin) * h;
			y_tmp = y_tmp / (ext->ymax - ext->ymin);
			cir_a.y1 = h - y_tmp + y_ofs;
			cir_a.y2 = cir_a.y1 + rects.radius;
			cir_a.y1 -= rects.radius;

			lv_draw_rect(&cir_a, mask, &rects);
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
	lv_style_t * style = lv_obj_get_style(chart);

	uint8_t i;
	area_t col_a;
	area_t col_mask;
	bool mask_ret;
	cord_t w = lv_obj_get_width(chart);
	cord_t h = lv_obj_get_height(chart);
	int32_t y_tmp;
    lv_chart_dl_t * dl;
    uint8_t dl_cnt = 0;
    lv_style_t rects;
	cord_t col_w = w / (2 * ext->dl_num * ext->pnum); /* Suppose (2 * dl_num) * pnum columns*/
	cord_t x_ofs = col_w / 2; /*Shift with a half col.*/

	lv_style_get(LV_STYLE_PLAIN, &rects);
	rects.bwidth = 0;
	rects.empty = 0;
	rects.radius = 0;
	rects.opa = (uint16_t)((uint16_t)style->opa * ext->data_opa) >> 8;

	col_a.y2 = chart->cords.y2;

	/*Go through all data lines*/
	LL_READ_BACK(ext->dl_ll, dl) {
		rects.mcolor = dl->color;
		rects.gcolor = color_mix(COLOR_BLACK, dl->color, ext->dark_eff);

		for(i = 0; i < ext->pnum; i ++) {
			/* Calculate the x coordinates. Suppose (2 * dl_num) * pnum columns and draw to every second
			 * the other columns will be spaces.
			 * col_w =  w / (2 * ext->dl_num * ext->pnum)
			 * act_col_x = col_w * i * ext->dl_num * 2 + 2 * dl_cnt
			 * Reorder the operation to multiply first*/

			col_a.x1 = (int32_t)((int32_t) w * ((i * ext->dl_num * 2) + (2 * dl_cnt))) /
					   (ext->pnum * 2 * ext->dl_num);
            col_a.x1 += chart->cords.x1;
			col_a.x2 = col_a.x1 + col_w;
			col_a.x1 += x_ofs;
			col_a.x2 += x_ofs;

			y_tmp = (int32_t)((int32_t) dl->points[i] - ext->ymin) * h;
			y_tmp = y_tmp / (ext->ymax - ext->ymin);
			col_a.y1 = h - y_tmp + chart->cords.y1;

			mask_ret = area_union(&col_mask, mask, &col_a);
			if(mask_ret != false) {
				lv_draw_rect(&chart->cords, &col_mask, &rects);
			}
		}
		dl_cnt++;
	}
}
#endif
