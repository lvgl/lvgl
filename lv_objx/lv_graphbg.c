

/**
 * @file lv_graphbg.c
 *
 */

/*Search an replace: graph. background -> object normal name with lower case (e.g. button, label etc.)
 * 					 graphbg -> object short name with lower case(e.g. btn, label etc)
 *                   GRAPHBG -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_GRAPHBG != 0

#include "lv_graphbg.h"
#include "lvgl/lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define LV_GRAPHBG_XMIN_DEF	0
#define LV_GRAPHBG_XMAX_DEF	100
#define LV_GRAPHBG_YMIN_DEF	0
#define LV_GRAPHBG_YMAX_DEF	100
#define LV_GRAPHBG_HDIV_DEF	3
#define LV_GRAPHBG_VDIV_DEF	5

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_graphbg_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool (*ancestor_design_fp)(lv_obj_t*, const area_t *, lv_design_mode_t);
static lv_graphbgs_t lv_graphbgs_def =
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
	.div_line_opa = 80,

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
 * Create a graph. background objects
 * @param par_dp pointer to an object, it will be the parent of the new graph. background
 * @param copy_dp pointer to a graph. background object, if not NULL then the new object will be copied from it
 * @return pointer to the created graph. background
 */
lv_obj_t* lv_graphbg_create(lv_obj_t* par_dp, lv_obj_t * copy_dp)
{
    /*Create the ancestor basic object*/
    lv_obj_t * new_obj_dp = lv_rect_create(par_dp, copy_dp);
    dm_assert(new_obj_dp);

    /*Allocate the object type specific extended data*/
    lv_graphbg_ext_t * ext_dp = lv_obj_alloc_ext(new_obj_dp, sizeof(lv_graphbg_ext_t));
    dm_assert(ext_dp);

    if(ancestor_design_fp == NULL) {
    	ancestor_design_fp = lv_obj_get_design_f(new_obj_dp);
    }

    /*Init the new graph. background object*/
    if(copy_dp == NULL) {
    	lv_obj_set_style(new_obj_dp, &lv_graphbgs_def);
        lv_obj_set_signal_f(new_obj_dp, lv_graphbg_signal);
        lv_obj_set_design_f(new_obj_dp, lv_graphbg_design);
        ext_dp->xmin = LV_GRAPHBG_XMIN_DEF;
        ext_dp->xmax = LV_GRAPHBG_XMAX_DEF;
        ext_dp->ymin = LV_GRAPHBG_YMIN_DEF;
        ext_dp->ymax = LV_GRAPHBG_YMAX_DEF;
        ext_dp->hdiv_num = LV_GRAPHBG_HDIV_DEF;
        ext_dp->vdiv_num = LV_GRAPHBG_VDIV_DEF;
    } else {
    	lv_graphbg_ext_t * ext_copy_dp = lv_obj_get_ext(copy_dp);
    	ext_dp->xmin = ext_copy_dp->xmin;
		ext_dp->xmax = ext_copy_dp->xmax;
		ext_dp->ymin = ext_copy_dp->ymin;
		ext_dp->ymax = ext_copy_dp->ymax;
		ext_dp->hdiv_num = ext_copy_dp->hdiv_num;
		ext_dp->vdiv_num = ext_copy_dp->vdiv_num;
    }

    return new_obj_dp;
}

/**
 * Signal function of the graph. background
 * @param obj_dp pointer to a graph. background object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_graphbg_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_rect_signal(obj_dp, sign, param);

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
 * Set the number of horizontal and vertical division lines
 * @param obj_dp pointer to a graph background object
 * @param hdiv number of horizontal division lines
 * @param vdiv number of vertical division lines
 */
void lv_graphbg_set_hvdiv(lv_obj_t * obj_dp, uint8_t hdiv, uint8_t vdiv)
{
	lv_graphbg_ext_t * ext_dp = lv_obj_get_ext(obj_dp);

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
void lv_graphbg_set_min_max(lv_obj_t * obj_dp, int32_t xmin, int32_t xmax, int32_t ymin, int32_t ymax)
{
	lv_graphbg_ext_t * ext_dp = lv_obj_get_ext(obj_dp);

	ext_dp->xmin = xmin;
	ext_dp->xmax = xmax;
	ext_dp->ymin = ymin;
	ext_dp->ymax = ymax;
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_graphbgs_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_graphbgs_t style
 */
lv_graphbgs_t * lv_graphbgs_get(lv_graphbgs_builtin_t style, lv_graphbgs_t * copy_p)
{
	lv_graphbgs_t * style_p;

	switch(style) {
		case LV_GRAPHBGS_DEF:
			style_p = &lv_graphbgs_def;
			break;
		default:
			style_p = &lv_graphbgs_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_graphbgs_t));
		else memcpy(copy_p, &lv_graphbgs_def, sizeof(lv_graphbgs_t));
	}

	return style_p;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * Handle the drawing related tasks of the graph. backgrounds
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'
 */
static bool lv_graphbg_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return ancestor_design_fp(obj_dp, mask_p, mode);
    }

    /*Draw the rectangle ancient*/
    ancestor_design_fp(obj_dp, mask_p, mode);

    /*Draw the object*/

    lv_graphbg_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
    lv_graphbgs_t * style_p = lv_obj_get_style(obj_dp);

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
    return true;
}


#endif
