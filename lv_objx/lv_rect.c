/**
 * @file lv_rect.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/

#include <lvgl/lv_misc/area.h>
#include <misc/mem/dyn_mem.h>
#include <misc/mem/linked_list.h>
#include <misc/others/color.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#if USE_LV_RECT != 0

#include "lv_rect.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_draw/lv_draw_vbasic.h"
#include "misc/math/math_base.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_rect_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);
static void lv_rect_refr_layout(lv_obj_t * obj_dp);
static void lv_rect_layout_col(lv_obj_t * obj_dp);
static void lv_rect_layout_row(lv_obj_t * obj_dp);
static void lv_rect_layout_center(lv_obj_t * obj_dp);
static void lv_rect_layout_grid(lv_obj_t * obj_dp);
static void lv_rect_refr_autofit(lv_obj_t * obj_dp);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_rects_t lv_rects_def =
{ .objs.color = COLOR_MAKE(0x50, 0x70, 0x90), .gcolor = COLOR_MAKE(0x70, 0xA0, 0xC0),
  .bcolor = COLOR_WHITE, .bwidth = 2 * LV_STYLE_MULT, .bopa = 50,
  .round = 4 * LV_STYLE_MULT, .empty = 0,
  .hpad = 10 * LV_STYLE_MULT, .vpad = 10 * LV_STYLE_MULT, .opad = 10 * LV_STYLE_MULT };

static lv_rects_t lv_rects_transp =
{ .objs.transp = 1, .bwidth = 0, .empty = 1,
  .hpad = 10 * LV_STYLE_MULT, .vpad = 10 * LV_STYLE_MULT, .opad = 10 * LV_STYLE_MULT};

static lv_rects_t lv_rects_border =
{ .bcolor = COLOR_BLACK, .bwidth = 2 * LV_STYLE_MULT, .bopa = 100,
  .round = 4 * LV_STYLE_MULT, .empty = 1,
  .hpad = 10 * LV_STYLE_MULT, .vpad = 10 * LV_STYLE_MULT, .opad = 10 * LV_STYLE_MULT};

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
 * Create a label objects
 * @param par_dp pointer to an object, it will be the parent of the new label
 * @param copy_dp pointer to a rectangle object, if not NULL then the new object will be copied from it
 * @return pointer to the created label
 */
lv_obj_t* lv_rect_create(lv_obj_t* par_dp, lv_obj_t * copy_dp)
{
    /*Create a basic object*/
    lv_obj_t* new_obj_dp = lv_obj_create(par_dp, copy_dp);
    dm_assert(new_obj_dp);
    lv_obj_alloc_ext(new_obj_dp, sizeof(lv_rect_ext_t));
    lv_rect_ext_t * rect_ext_dp = lv_obj_get_ext(new_obj_dp);
    lv_obj_set_design_f(new_obj_dp, lv_rect_design);
    lv_obj_set_signal_f(new_obj_dp, lv_rect_signal);

    /*Init the new rectangle*/
    if(copy_dp == NULL) {
		lv_obj_set_style(new_obj_dp, &lv_rects_def);
		rect_ext_dp->hfit_en = 0;
		rect_ext_dp->vfit_en = 0;
    } else {
    	lv_rect_ext_t * ori_rect_ext = lv_obj_get_ext(copy_dp);
    	rect_ext_dp->hfit_en = ori_rect_ext->hfit_en;
    	rect_ext_dp->vfit_en = ori_rect_ext->vfit_en;
    	rect_ext_dp->layout = ori_rect_ext->layout;
    }

    return new_obj_dp;
}
uint32_t sign_cnt = 0;
/**
 * Signal function of the rectangle
 * @param obj_dp pointer to a rectangle object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_rect_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_obj_signal(obj_dp, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	switch(sign) {
    	case LV_SIGNAL_STYLE_CHG: /*Recalculate the padding if the style changed*/
        case LV_SIGNAL_CHILD_CHG:
        	sign_cnt++;
        	lv_rect_refr_layout(obj_dp);
        	lv_rect_refr_autofit(obj_dp);
        	break;
        case LV_SIGNAL_CORD_CHG:
        	sign_cnt++;
        	if(lv_obj_get_width(obj_dp) != area_get_width(param) ||
    		  lv_obj_get_height(obj_dp) != area_get_height(param)) {
            	lv_rect_refr_layout(obj_dp);
            	lv_rect_refr_autofit(obj_dp);
        	}
        	break;


            break;

    		default:
    			break;
    	}
    }
    
    return valid;
}

/*=====================
 * Setter functions
 *====================*/

void lv_rect_set_layout(lv_obj_t * obj_dp, lv_rect_layout_t layout)
{
	lv_rect_ext_t * ext_p = lv_obj_get_ext(obj_dp);
	ext_p->layout = layout;

	/*Send a signal to run the paddig calculations*/
	obj_dp->signal_f(obj_dp, LV_SIGNAL_CHILD_CHG, NULL);
}


/**
 * Enable the horizontal or vertical padding
 * @param obj_dp pointer to a rectangle object
 * @param hor_en true: enable the horizontal padding
 * @param ver_en true: enable the vertical padding
 */
void lv_rect_set_fit(lv_obj_t * obj_dp, bool hor_en, bool ver_en)
{
	lv_obj_inv(obj_dp);
	lv_rect_ext_t * ext_p = lv_obj_get_ext(obj_dp);
	ext_p->hfit_en = hor_en == false ? 0 : 1;
	ext_p->vfit_en = ver_en == false ? 0 : 1;

	/*Send a signal to run the paddig calculations*/
	obj_dp->signal_f(obj_dp, LV_SIGNAL_CORD_CHG, obj_dp);
}

/*=====================
 * Getter functions
 *====================*/

lv_rect_layout_t lv_rect_get_layout(lv_obj_t * obj_dp)
{
	lv_rect_ext_t * ext_p = lv_obj_get_ext(obj_dp);
	return ext_p->layout;
}

/**
 * Get horizontal padding enable attribute of a rectangle
 * @param obj_dp pointer to a rectangle object
 * @return true: horizontal padding is enabled
 */
bool lv_rect_get_hfit(lv_obj_t * obj_dp)
{
	lv_rect_ext_t * ext_p = lv_obj_get_ext(obj_dp);
	return ext_p->hfit_en == 0 ? false : true;
}

/**
 * Get vertical padding enable attribute of a rectangle
 * @param obj_dp pointer to a rectangle object
 * @return true: vertical padding is enabled
 */
bool lv_rect_get_vfit(lv_obj_t * obj_dp)
{
	lv_rect_ext_t * ext_p = lv_obj_get_ext(obj_dp);
	return ext_p->vfit_en == 0 ? false : true;
}


/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_rects_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_rects_t style
 */
lv_rects_t * lv_rects_get(lv_rects_builtin_t style, lv_rects_t * copy_p)
{
	lv_rects_t * style_p;

	switch(style) {
		case LV_RECTS_DEF:
			style_p = &lv_rects_def;
			break;
		case LV_RECTS_BORDER:
			style_p = &lv_rects_border;
			break;
		case LV_RECTS_TRANSP:
			style_p = &lv_rects_transp;
			break;
		default:
			style_p = &lv_rects_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_rects_t));
		else memcpy(copy_p, &lv_rects_def, sizeof(lv_rects_t));
	}

	return style_p;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the rectangles
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_rect_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    /* Because of the radius it is not sure the area is covered*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	if(LV_SA(obj_dp, lv_rects_t)->empty != 0) return false;

    	uint16_t r = LV_SA(obj_dp, lv_rects_t)->round;
    	area_t area_tmp;

    	/*Check horizontally without radius*/
    	lv_obj_get_cords(obj_dp, &area_tmp);
    	area_tmp.x1 += r;
    	area_tmp.x2 -= r;
    	if(area_is_in(mask_p, &area_tmp) == true) return true;

    	/*Check vertically without radius*/
    	lv_obj_get_cords(obj_dp, &area_tmp);
    	area_tmp.y1 += r;
    	area_tmp.y2 -= r;
    	if(area_is_in(mask_p, &area_tmp) == true) return true;

    	return false;
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		opa_t opa = lv_obj_get_opa(obj_dp);
		area_t area;
		lv_obj_get_cords(obj_dp, &area);

		/*Draw the rectangle*/
		lv_draw_rect(&area, mask_p, lv_obj_get_style(obj_dp), opa);
    }
    return true;
}

/**
 * Refresh the layout of a rectangle
 * @param obj_dp pointer to an object which layout should be refreshed
 */
static void lv_rect_refr_layout(lv_obj_t * obj_dp)
{
	lv_rect_layout_t type = lv_rect_get_layout(obj_dp);

	if(type == LV_RECT_LAYOUT_OFF) return;

	if(type == LV_RECT_LAYOUT_CENTER) {
		lv_rect_layout_center(obj_dp);
	} else if(type == LV_RECT_LAYOUT_COL_L || type == LV_RECT_LAYOUT_COL_M || type == LV_RECT_LAYOUT_COL_R) {
		lv_rect_layout_col(obj_dp);
	} else if(type == LV_RECT_LAYOUT_ROW_T || type == LV_RECT_LAYOUT_ROW_M || type == LV_RECT_LAYOUT_ROW_B) {
		lv_rect_layout_row(obj_dp);
	} else if(type == LV_RECT_LAYOUT_GRID) {
		lv_rect_layout_grid(obj_dp);
	}
}


/**
 * Handle column type layouts
 * @param obj_dp pointer to an object which layout should be handled
 */
static void lv_rect_layout_col(lv_obj_t * obj_dp)
{
	lv_rect_layout_t type = lv_rect_get_layout(obj_dp);
	lv_obj_t * child;

	/*Adjust margin and get the alignment type*/
	lv_align_t align;
	lv_rects_t * rects_p = lv_obj_get_style(obj_dp);
	cord_t hpad_corr;
	switch(type) {
		case LV_RECT_LAYOUT_COL_L:
			hpad_corr = rects_p->hpad;
			align = LV_ALIGN_IN_TOP_LEFT;
			break;
		case LV_RECT_LAYOUT_COL_M:
			hpad_corr = 0;
			align = LV_ALIGN_IN_TOP_MID;
			break;
		case LV_RECT_LAYOUT_COL_R:
			hpad_corr = -rects_p->hpad;
			align = LV_ALIGN_IN_TOP_RIGHT;
			break;
		default:
			align = LV_ALIGN_IN_TOP_LEFT;
			break;
	}

	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/
	obj_dp->child_chg_off = 1;
	/* Align the children */
	cord_t last_cord = rects_p->vpad;
	LL_READ_BACK(obj_dp->child_ll, child) {
		if(lv_obj_get_hidden(child) != false) continue;

		lv_obj_align(child, obj_dp, align, hpad_corr , last_cord);
		last_cord += lv_obj_get_height(child) + rects_p->opad;
	}

	obj_dp->child_chg_off = 0;
}

/**
 * Handle row type layouts
 * @param obj_dp pointer to an object which layout should be handled
 */
static void lv_rect_layout_row(lv_obj_t * obj_dp)
{
	lv_rect_layout_t type = lv_rect_get_layout(obj_dp);
	lv_obj_t * child;

	/*Adjust margin and get the alignment type*/
	lv_align_t align;
	lv_rects_t * rects_p = lv_obj_get_style(obj_dp);
	cord_t vpad_corr = rects_p->vpad;
	switch(type) {
		case LV_RECT_LAYOUT_ROW_T:
			vpad_corr = rects_p->vpad;
			align = LV_ALIGN_IN_TOP_LEFT;
			break;
		case LV_RECT_LAYOUT_ROW_M:
			vpad_corr = 0;
			align = LV_ALIGN_IN_LEFT_MID;
			break;
		case LV_RECT_LAYOUT_ROW_B:
			vpad_corr = -rects_p->vpad;
			align = LV_ALIGN_IN_BOTTOM_LEFT;
			break;
		default:
			vpad_corr = 0;
			align = LV_ALIGN_IN_TOP_LEFT;
			break;
	}

	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/
	obj_dp->child_chg_off = 1;


	/* Align the children */
	cord_t last_cord = rects_p->hpad;
	LL_READ_BACK(obj_dp->child_ll, child) {
		if(lv_obj_get_hidden(child) != false) continue;

		lv_obj_align(child, obj_dp, align, last_cord, vpad_corr);
		last_cord += lv_obj_get_width(child) + rects_p->opad;
	}

	obj_dp->child_chg_off = 0;
}

/**
 * Handle the center layout
 * @param obj_dp pointer to an object which layout should be handled
 */
static void lv_rect_layout_center(lv_obj_t * obj_dp)
{
	lv_obj_t * child;
	lv_rects_t * rects_p = lv_obj_get_style(obj_dp);
	uint32_t obj_num = 0;
	cord_t h_tot = 0;

	LL_READ(obj_dp->child_ll, child) {
		h_tot += lv_obj_get_height(child) + rects_p->opad;
		obj_num ++;
	}

	if(obj_num == 0) return;

	h_tot -= rects_p->opad;

	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/
	obj_dp->child_chg_off = 1;

	/* Align the children */
	cord_t last_cord = - (h_tot / 2);
	LL_READ_BACK(obj_dp->child_ll, child) {
		if(lv_obj_get_hidden(child) != false) continue;

		lv_obj_align(child, obj_dp, LV_ALIGN_CENTER, 0, last_cord + lv_obj_get_height(child) / 2);
		last_cord += lv_obj_get_height(child) + rects_p->opad;
	}

	obj_dp->child_chg_off = 0;
}


/**
 * Handle the grid layout. Put as many object as possible in row
 * then begin a new row
 * @param obj_dp pointer to an object which layout should be handled
 */
static void lv_rect_layout_grid(lv_obj_t * obj_dp)
{
	lv_obj_t * child_rs;    /* Row starter child */
	lv_obj_t * child_rc;    /* Row closer child */
	lv_obj_t * child_tmp;   /* Temporary child */
	lv_rects_t * rects_p = lv_obj_get_style(obj_dp);
	cord_t w_obj = lv_obj_get_width(obj_dp);
	cord_t act_y = rects_p->vpad;
	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/

	child_rs = ll_get_tail(&obj_dp->child_ll); /*Set the row starter child*/
	if(child_rs == NULL) return;	/*Return if no child*/

	obj_dp->child_chg_off = 1;

	child_rc = child_rs; /*Initially the the row starter and closer is the same*/
	while(child_rs != NULL) {
		cord_t h_row = 0;
		cord_t w_row = rects_p->hpad * 2; /*The width is minimum the left-right hpad*/
		uint32_t obj_num = 0;

		/*Find the row closer object and collect some data*/		do {
			if(lv_obj_get_hidden(child_rc) == false) {
				if(w_row + lv_obj_get_width(child_rc) > w_obj) break; /*If the next object is already not fit then break*/
				w_row += lv_obj_get_width(child_rc) + rects_p->opad; /*Add the object width + opad*/
				h_row = max(h_row, lv_obj_get_height(child_rc)); /*Search the highest object*/
				obj_num ++;
			}
			child_rc = ll_get_prev(&obj_dp->child_ll, child_rc); /*Load the next object*/
			if(obj_num == 0) child_rs = child_rc; /*If the first object was hidden (or too long) then set the next as first */
		}while(child_rc != NULL);

		/*Step back one child because the last is already not fit*/
		if(child_rc != NULL  && obj_num != 0) child_rc = ll_get_next(&obj_dp->child_ll, child_rc);

		/*If the object is too long  then align it to the middle*/
		if(obj_num == 0) {
			if(child_rc != NULL) {
				h_row = lv_obj_get_height(child_rc);
				lv_obj_align(child_rc, obj_dp, LV_ALIGN_IN_TOP_MID, 0, act_y);
			}
		}
		/*If here is only one object in the row then align it to the left*/
		else if (obj_num == 1) {
			lv_obj_align(child_rs, obj_dp, LV_ALIGN_IN_TOP_LEFT, rects_p->hpad, act_y);
		}
		/* Align the children (from child_rs to child_rc)*/
		else {
			w_row -= rects_p->opad * obj_num;
			cord_t new_opad = (w_obj -  w_row) / (obj_num  - 1);
			cord_t act_x = rects_p->hpad; /*x init*/
			child_tmp = child_rs;
			do{
				if(lv_obj_get_hidden(child_tmp) == false) {
					lv_obj_align(child_tmp, obj_dp, LV_ALIGN_IN_TOP_LEFT, act_x, act_y);
					act_x += lv_obj_get_width(child_tmp) + new_opad;
				}
				child_tmp = ll_get_prev(&obj_dp->child_ll, child_tmp);
			}while(child_tmp != child_rc);

		}

		if(child_rc == NULL) break;
		act_y += rects_p->opad + h_row; /*y increment*/
		child_rs = ll_get_prev(&obj_dp->child_ll, child_rc); /*Go to the next object*/
		child_rc = child_rs;
	}
	obj_dp->child_chg_off = 0;
}

/**
 * Handle auto fit. Set the size of the object to involve all children.
 * @param obj_dp pointer to an object which size will be modified
 */
void lv_rect_refr_autofit(lv_obj_t * obj_dp)
{
	lv_rect_ext_t * ext_p = lv_obj_get_ext(obj_dp);

	if(ext_p->hfit_en == 0 &&
	   ext_p->vfit_en == 0) {
		return;
	}

	area_t rect_cords;
	area_t ori;
	lv_rects_t * rects_p = lv_obj_get_style(obj_dp);
	lv_obj_t * i;
	cord_t hpad = rects_p->hpad;
	cord_t vpad = rects_p->vpad;

	/*Search the side coordinates of the children*/
	lv_obj_get_cords(obj_dp, &ori);
	lv_obj_get_cords(obj_dp, &rect_cords);

	rect_cords.x1 = LV_CORD_MAX;
	rect_cords.y1 = LV_CORD_MAX;
	rect_cords.x2 = LV_CORD_MIN;
	rect_cords.y2 = LV_CORD_MIN;

    LL_READ(obj_dp->child_ll, i) {
		if(lv_obj_get_hidden(i) != false) continue;
    	rect_cords.x1 = min(rect_cords.x1, i->cords.x1);
    	rect_cords.y1 = min(rect_cords.y1, i->cords.y1);
        rect_cords.x2 = max(rect_cords.x2, i->cords.x2);
        rect_cords.y2 = max(rect_cords.y2, i->cords.y2);
    }

    /*If the value is not the init value then the page has >=1 child.*/
    if(rect_cords.x1 != LV_CORD_MAX) {
    	if(ext_p->hfit_en != 0) {
			rect_cords.x1 -= hpad;
			rect_cords.x2 += hpad;
    	} else {
    		rect_cords.x1 = obj_dp->cords.x1;
    		rect_cords.x2 = obj_dp->cords.x2;
    	}
    	if(ext_p->vfit_en != 0) {
			rect_cords.y1 -= vpad;
			rect_cords.y2 += vpad;
    	} else {
    		rect_cords.y1 = obj_dp->cords.y1;
    		rect_cords.y2 = obj_dp->cords.y2;
    	}

    	lv_obj_inv(obj_dp);
        area_cpy(&obj_dp->cords, &rect_cords);
    	lv_obj_inv(obj_dp);

        /*Notify the object about its new coordinates*/
    	obj_dp->signal_f(obj_dp, LV_SIGNAL_CORD_CHG, &ori);

        /*Inform the parent about the new coordinates*/
    	lv_obj_t * par_dp = lv_obj_get_parent(obj_dp);
    	par_dp->signal_f(par_dp, LV_SIGNAL_CHILD_CHG, obj_dp);

    }
}

#endif
