/**
 * @file lv_rect.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"

#if USE_LV_RECT != 0

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "lv_rect.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_draw/lv_draw_vbasic.h"
#include "../lv_misc/area.h"

#include "misc/mem/dyn_mem.h"
#include "misc/mem/linked_list.h"
#include "misc/others/color.h"
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
static bool lv_rect_design(lv_obj_t * rect, const area_t * mask, lv_design_mode_t mode);
static void lv_rect_draw_light(lv_obj_t * rect, const area_t * mask);
static void lv_rect_refr_layout(lv_obj_t * rect);
static void lv_rect_layout_col(lv_obj_t * rect);
static void lv_rect_layout_row(lv_obj_t * rect);
static void lv_rect_layout_center(lv_obj_t * rect);
static void lv_rect_layout_pretty(lv_obj_t * rect);
static void lv_rect_layout_grid(lv_obj_t * rect);
static void lv_rect_refr_autofit(lv_obj_t * rect);
static void lv_rects_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_rects_t lv_rects_def;
static lv_rects_t lv_rects_transp;
static lv_rects_t lv_rects_border;

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
 * @param par pointer to an object, it will be the parent of the new label
 * @param copy pointer to a rectangle object, if not NULL then the new object will be copied from it
 * @return pointer to the created label
 */
lv_obj_t * lv_rect_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create a basic object*/
    lv_obj_t * new_rect = lv_obj_create(par, copy);
    dm_assert(new_rect);
    lv_obj_alloc_ext(new_rect, sizeof(lv_rect_ext_t));
    lv_rect_ext_t * ext = lv_obj_get_ext(new_rect);
    dm_assert(ext);
    ext->hfit_en = 0;
    ext->vfit_en = 0;
    ext->layout = LV_RECT_LAYOUT_OFF;

    lv_obj_set_design_f(new_rect, lv_rect_design);
    lv_obj_set_signal_f(new_rect, lv_rect_signal);

    /*Init the new rectangle*/
    if(copy == NULL) {
		lv_obj_set_style(new_rect, lv_rects_get(LV_RECTS_DEF, NULL));
    }
    /*Copy an existing object*/
    else {
    	lv_rect_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->hfit_en = copy_ext->hfit_en;
    	ext->vfit_en = copy_ext->vfit_en;
    	ext->layout = copy_ext->layout;

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_rect);

    }

    return new_rect;
}

/**
 * Signal function of the rectangle
 * @param rect pointer to a rectangle object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_rect_signal(lv_obj_t * rect, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_obj_signal(rect, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {

    	lv_rects_t * style = lv_obj_get_style(rect);

    	switch(sign) {
    	case LV_SIGNAL_STYLE_CHG: /*Recalculate the padding if the style changed*/
        	lv_rect_refr_layout(rect);
        	lv_rect_refr_autofit(rect);
        	lv_obj_refr_ext_size(rect);
        	break;
        case LV_SIGNAL_CHILD_CHG:
        	lv_rect_refr_layout(rect);
        	lv_rect_refr_autofit(rect);
        	break;
        case LV_SIGNAL_CORD_CHG:
        	if(lv_obj_get_width(rect) != area_get_width(param) ||
    		  lv_obj_get_height(rect) != area_get_height(param)) {
            	lv_rect_refr_layout(rect);
            	lv_rect_refr_autofit(rect);
        	}
        	break;
        case LV_SIGNAL_REFR_EXT_SIZE:
        	if(style->light > rect->ext_size) rect->ext_size = style->light;
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

/**
 * Set the layout on a rectangle
 * @param rect pointer to a rectangle object
 * @param layout a layout from 'lv_rect_layout_t'
 */
void lv_rect_set_layout(lv_obj_t * rect, lv_rect_layout_t layout)
{
	lv_rect_ext_t * ext = lv_obj_get_ext(rect);
	ext->layout = layout;

	/*Send a signal to refresh the layout*/
	rect->signal_f(rect, LV_SIGNAL_CHILD_CHG, NULL);
}


/**
 * Enable the horizontal or vertical fit.
 * The rectangle size will be set to involve the children horizontally or vertically.
 * @param rect pointer to a rectangle object
 * @param hor_en true: enable the horizontal padding
 * @param ver_en true: enable the vertical padding
 */
void lv_rect_set_fit(lv_obj_t * rect, bool hor_en, bool ver_en)
{
	lv_obj_inv(rect);
	lv_rect_ext_t * ext = lv_obj_get_ext(rect);
	ext->hfit_en = hor_en == false ? 0 : 1;
	ext->vfit_en = ver_en == false ? 0 : 1;

	/*Send a signal to set a new size*/
	rect->signal_f(rect, LV_SIGNAL_CORD_CHG, rect);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the layout of a rectangle
 * @param rect pointer to rectangle object
 * @return the layout from 'lv_rect_layout_t'
 */
lv_rect_layout_t lv_rect_get_layout(lv_obj_t * rect)
{
	lv_rect_ext_t * ext = lv_obj_get_ext(rect);
	return ext->layout;
}

/**
 * Get horizontal fit enable attribute of a rectangle
 * @param rect pointer to a rectangle object
 * @return true: horizontal padding is enabled
 */
bool lv_rect_get_hfit(lv_obj_t * rect)
{
	lv_rect_ext_t * ext = lv_obj_get_ext(rect);
	return ext->hfit_en == 0 ? false : true;
}

/**
 * Get vertical fit enable attribute of a rectangle
 * @param obj pointer to a rectangle object
 * @return true: vertical padding is enabled
 */
bool lv_rect_get_vfit(lv_obj_t * rect)
{
	lv_rect_ext_t * ext = lv_obj_get_ext(rect);
	return ext->vfit_en == 0 ? false : true;
}


/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_rects_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_rects_t style
 */
lv_rects_t * lv_rects_get(lv_rects_builtin_t style, lv_rects_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_rects_init();
		style_inited = true;
	}

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

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_rects_t));
		else memcpy(copy, &lv_rects_def, sizeof(lv_rects_t));
	}

	return style_p;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the rectangles
 * @param rect pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_rect_design(lv_obj_t * rect, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        /* Because of the radius it is not sure the area is covered
         * Check the areas where there is no radius*/
    	if(LV_SA(rect, lv_rects_t)->empty != 0) return false;

    	uint16_t r = LV_SA(rect, lv_rects_t)->round;

    	if(r == LV_RECT_CIRCLE) return false;

    	area_t area_tmp;

    	/*Check horizontally without radius*/
    	lv_obj_get_cords(rect, &area_tmp);
    	area_tmp.x1 += r;
    	area_tmp.x2 -= r;
    	if(area_is_in(mask, &area_tmp) == true) return true;

    	/*Check vertically without radius*/
    	lv_obj_get_cords(rect, &area_tmp);
    	area_tmp.y1 += r;
    	area_tmp.y2 -= r;
    	if(area_is_in(mask, &area_tmp) == true) return true;

    	return false;
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		opa_t opa = lv_obj_get_opa(rect);
		lv_rects_t * style =  lv_obj_get_style(rect);
		area_t area;
		lv_obj_get_cords(rect, &area);

		/*Draw the rectangle*/
		lv_draw_rect(&area, mask, style, opa);
		lv_rect_draw_light(rect, mask);

    } else if(mode == LV_DESIGN_DRAW_POST) {

    }
    return true;
}

/**
 * Draw a light around the object
 * @param rect pointer to rectangle object
 * @param mask pointer to a mask area (from the design functions)
 */
static void lv_rect_draw_light(lv_obj_t * rect, const area_t * mask)
{
	lv_rects_t * style =  lv_obj_get_style(rect);
	cord_t light_size = style->light;
	if(light_size == 0) return;
	if(light_size < LV_DOWNSCALE) light_size = LV_DOWNSCALE;

	area_t light_area;
	lv_rects_t light_style;
	lv_obj_get_cords(rect, &light_area);

	memcpy(&light_style, style, sizeof(lv_rects_t));

	light_style.empty = 1;
	light_style.bwidth = light_size;
	light_style.round =  style->round + light_size + 1;
	light_style.bcolor = style->lcolor;
	light_style.bopa = 100;

	light_area.x1 -= light_size;
	light_area.y1 -= light_size;
	light_area.x2 += light_size;
	light_area.y2 += light_size;

	cord_t i;
	uint8_t res = LV_DOWNSCALE ;
	opa_t opa_sub =  lv_obj_get_opa(rect) / (light_size / LV_DOWNSCALE);

	for(i = 1; i < light_size; i += res) {
		lv_draw_rect(&light_area, mask, &light_style, (uint16_t) opa_sub);
		light_style.round -= res;
		light_style.bwidth -= res;
		light_area.x1 += res;
		light_area.y1 += res;
		light_area.x2 -= res;
		light_area.y2 -= res;
	}
}

/**
 * Refresh the layout of a rectangle
 * @param rect pointer to an object which layout should be refreshed
 */
static void lv_rect_refr_layout(lv_obj_t * rect)
{
	lv_rect_layout_t type = lv_rect_get_layout(rect);

	/*'rect' has to be at least 1 child*/
	if(lv_obj_get_child(rect, NULL) == NULL) return;

	if(type == LV_RECT_LAYOUT_OFF) return;

	if(type == LV_RECT_LAYOUT_CENTER) {
		lv_rect_layout_center(rect);
	} else if(type == LV_RECT_LAYOUT_COL_L || type == LV_RECT_LAYOUT_COL_M || type == LV_RECT_LAYOUT_COL_R) {
		lv_rect_layout_col(rect);
	} else if(type == LV_RECT_LAYOUT_ROW_T || type == LV_RECT_LAYOUT_ROW_M || type == LV_RECT_LAYOUT_ROW_B) {
		lv_rect_layout_row(rect);
	} else if(type == LV_RECT_LAYOUT_PRETTY) {
		lv_rect_layout_pretty(rect);
	}  else if(type == LV_RECT_LAYOUT_GRID) {
		lv_rect_layout_grid(rect);
	}
}

/**
 * Handle column type layouts
 * @param rect pointer to an object which layout should be handled
 */
static void lv_rect_layout_col(lv_obj_t * rect)
{
	lv_rect_layout_t type = lv_rect_get_layout(rect);
	lv_obj_t * child;

	/*Adjust margin and get the alignment type*/
	lv_align_t align;
	lv_rects_t * style = lv_obj_get_style(rect);
	cord_t hpad_corr;

	switch(type) {
		case LV_RECT_LAYOUT_COL_L:
            hpad_corr = style->hpad;
			align = LV_ALIGN_IN_TOP_LEFT;
			break;
		case LV_RECT_LAYOUT_COL_M:
			hpad_corr = 0;
			align = LV_ALIGN_IN_TOP_MID;
			break;
		case LV_RECT_LAYOUT_COL_R:
			hpad_corr = -style->hpad;
			align = LV_ALIGN_IN_TOP_RIGHT;
			break;
		default:
			hpad_corr = 0;
			align = LV_ALIGN_IN_TOP_LEFT;
			break;
	}

	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/
	lv_obj_set_protect(rect, LV_PROTECT_CHILD_CHG);
	/* Align the children */
	cord_t last_cord = style->vpad;
	LL_READ_BACK(rect->child_ll, child) {
        if(lv_obj_get_hidden(child) != false ||
           lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

		lv_obj_align(child, rect, align, hpad_corr , last_cord);
		last_cord += lv_obj_get_height(child) + style->opad;
	}

    lv_obj_clr_protect(rect, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle row type layouts
 * @param rect pointer to an object which layout should be handled
 */
static void lv_rect_layout_row(lv_obj_t * rect)
{
	lv_rect_layout_t type = lv_rect_get_layout(rect);
	lv_obj_t * child;

	/*Adjust margin and get the alignment type*/
	lv_align_t align;
	lv_rects_t * style = lv_obj_get_style(rect);
	cord_t vpad_corr = style->vpad;

	switch(type) {
		case LV_RECT_LAYOUT_ROW_T:
			vpad_corr = style->vpad;
			align = LV_ALIGN_IN_TOP_LEFT;
			break;
		case LV_RECT_LAYOUT_ROW_M:
			vpad_corr = 0;
			align = LV_ALIGN_IN_LEFT_MID;
			break;
		case LV_RECT_LAYOUT_ROW_B:
			vpad_corr = -style->vpad;
			align = LV_ALIGN_IN_BOTTOM_LEFT;
			break;
		default:
			vpad_corr = 0;
			align = LV_ALIGN_IN_TOP_LEFT;
			break;
	}

	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/
    lv_obj_set_protect(rect, LV_PROTECT_CHILD_CHG);

	/* Align the children */
	cord_t last_cord = style->hpad;
	LL_READ_BACK(rect->child_ll, child) {
		if(lv_obj_get_hidden(child) != false ||
           lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

		lv_obj_align(child, rect, align, last_cord, vpad_corr);
		last_cord += lv_obj_get_width(child) + style->opad;
	}

    lv_obj_clr_protect(rect, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle the center layout
 * @param rect pointer to an object which layout should be handled
 */
static void lv_rect_layout_center(lv_obj_t * rect)
{
	lv_obj_t * child;
	lv_rects_t * style = lv_obj_get_style(rect);
	uint32_t obj_num = 0;
	cord_t h_tot = 0;

	LL_READ(rect->child_ll, child) {
		h_tot += lv_obj_get_height(child) + style->opad;
		obj_num ++;
	}

	if(obj_num == 0) return;

	h_tot -= style->opad;

	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/
    lv_obj_set_protect(rect, LV_PROTECT_CHILD_CHG);

	/* Align the children */
	cord_t last_cord = - (h_tot / 2);
	LL_READ_BACK(rect->child_ll, child) {
        if(lv_obj_get_hidden(child) != false ||
           lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

		lv_obj_align(child, rect, LV_ALIGN_CENTER, 0, last_cord + lv_obj_get_height(child) / 2);
		last_cord += lv_obj_get_height(child) + style->opad;
	}

    lv_obj_clr_protect(rect, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle the pretty layout. Put as many object as possible in row
 * then begin a new row
 * @param rect pointer to an object which layout should be handled
 */
static void lv_rect_layout_pretty(lv_obj_t * rect)
{
	lv_obj_t * child_rs;    /* Row starter child */
	lv_obj_t * child_rc;    /* Row closer child */
	lv_obj_t * child_tmp;   /* Temporary child */
	lv_rects_t * style = lv_obj_get_style(rect);
	cord_t w_obj = lv_obj_get_width(rect);
	cord_t act_y = style->vpad;
	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/

	child_rs = ll_get_tail(&rect->child_ll); /*Set the row starter child*/
	if(child_rs == NULL) return;	/*Return if no child*/

    lv_obj_set_protect(rect, LV_PROTECT_CHILD_CHG);

	child_rc = child_rs; /*Initially the the row starter and closer is the same*/
	while(child_rs != NULL) {
		cord_t h_row = 0;
		cord_t w_row = style->hpad * 2; /*The width is minimum the left-right hpad*/
		uint32_t obj_num = 0;

		/*Find the row closer object and collect some data*/
		do {
			if(lv_obj_get_hidden(child_rc) == false &&
			   lv_obj_is_protected(child_rc, LV_PROTECT_POS) == false) {
				if(w_row + lv_obj_get_width(child_rc) > w_obj) break; /*If the next object is already not fit then break*/
				w_row += lv_obj_get_width(child_rc) + style->opad; /*Add the object width + opad*/
				h_row = max(h_row, lv_obj_get_height(child_rc)); /*Search the highest object*/
				obj_num ++;
			}
			child_rc = ll_get_prev(&rect->child_ll, child_rc); /*Load the next object*/
			if(obj_num == 0) child_rs = child_rc; /*If the first object was hidden (or too long) then set the next as first */
		}while(child_rc != NULL);

		/*Step back one child because the last is already not fit*/
		if(child_rc != NULL  && obj_num != 0) child_rc = ll_get_next(&rect->child_ll, child_rc);

		/*If the object is too long  then align it to the middle*/
		if(obj_num == 0) {
			if(child_rc != NULL) {
				h_row = lv_obj_get_height(child_rc);
				lv_obj_align(child_rc, rect, LV_ALIGN_IN_TOP_MID, 0, act_y);
			}
		}
		/*If here is only one object in the row then align it to the left*/
		else if (obj_num == 1) {
			lv_obj_align(child_rs, rect, LV_ALIGN_IN_TOP_LEFT, style->hpad, act_y);
		}
		/* Align the children (from child_rs to child_rc)*/
		else {
			w_row -= style->opad * obj_num;
			cord_t new_opad = (w_obj -  w_row) / (obj_num  - 1);
			cord_t act_x = style->hpad; /*x init*/
			child_tmp = child_rs;
			do{
				if(lv_obj_get_hidden(child_tmp) == false &&
				   lv_obj_is_protected(child_tmp, LV_PROTECT_POS) == false) {
					lv_obj_align(child_tmp, rect, LV_ALIGN_IN_TOP_LEFT, act_x, act_y);
					act_x += lv_obj_get_width(child_tmp) + new_opad;
				}
				child_tmp = ll_get_prev(&rect->child_ll, child_tmp);
			}while(child_tmp != child_rc);

		}

		if(child_rc == NULL) break;
		act_y += style->opad + h_row; /*y increment*/
		child_rs = ll_get_prev(&rect->child_ll, child_rc); /*Go to the next object*/
		child_rc = child_rs;
	}
    lv_obj_clr_protect(rect, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle the grid layout. Align same-sized objects in a grid
 * @param rect pointer to an object which layout should be handled
 */
static void lv_rect_layout_grid(lv_obj_t * rect)
{
	lv_obj_t * child;
	lv_rects_t * style = lv_obj_get_style(rect);
	cord_t w_tot = lv_obj_get_width(rect);
	cord_t w_obj = lv_obj_get_width(lv_obj_get_child(rect, NULL));
	cord_t h_obj = lv_obj_get_height(lv_obj_get_child(rect, NULL));
	uint16_t obj_row = (w_tot - (2 * style->hpad)) / (w_obj + style->opad); /*Obj. num. in a row*/
	cord_t x_ofs;
	if(obj_row > 1) {
		x_ofs = w_obj + (w_tot - (2 * style->hpad) - (obj_row * w_obj)) / (obj_row - 1);
	} else {
		x_ofs = w_tot / 2 - w_obj / 2;
	}
	cord_t y_ofs = h_obj + style->opad;

	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/
    lv_obj_set_protect(rect, LV_PROTECT_CHILD_CHG);

	/* Align the children */
	cord_t act_x = style->hpad;
	cord_t act_y = style->vpad;
	uint16_t obj_cnt = 0;
	LL_READ_BACK(rect->child_ll, child) {
        if(lv_obj_get_hidden(child) != false ||
           lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

		if(obj_row > 1) {
			lv_obj_set_pos(child, act_x, act_y);
			act_x += x_ofs;
		} else {
			lv_obj_set_pos(child, x_ofs, act_y);
		}
		obj_cnt ++;

		if(obj_cnt >= obj_row) {
			obj_cnt = 0;
			act_x = style->hpad;
			act_y += y_ofs;
		}
	}

    lv_obj_clr_protect(rect, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle auto fit. Set the size of the object to involve all children.
 * @param rect pointer to an object which size will be modified
 */
void lv_rect_refr_autofit(lv_obj_t * rect)
{
	lv_rect_ext_t * ext = lv_obj_get_ext(rect);

	if(ext->hfit_en == 0 &&
	   ext->vfit_en == 0) {
		return;
	}

	area_t new_cords;
	area_t ori;
	lv_rects_t * style = lv_obj_get_style(rect);
	lv_obj_t * i;
	cord_t hpad = style->hpad;
	cord_t vpad = style->vpad;

	/*Search the side coordinates of the children*/
	lv_obj_get_cords(rect, &ori);
	lv_obj_get_cords(rect, &new_cords);

	new_cords.x1 = LV_CORD_MAX;
	new_cords.y1 = LV_CORD_MAX;
	new_cords.x2 = LV_CORD_MIN;
	new_cords.y2 = LV_CORD_MIN;

    LL_READ(rect->child_ll, i) {
		if(lv_obj_get_hidden(i) != false) continue;
    	new_cords.x1 = min(new_cords.x1, i->cords.x1);
    	new_cords.y1 = min(new_cords.y1, i->cords.y1);
        new_cords.x2 = max(new_cords.x2, i->cords.x2);
        new_cords.y2 = max(new_cords.y2, i->cords.y2);
    }

    /*If the value is not the init value then the page has >=1 child.*/
    if(new_cords.x1 != LV_CORD_MAX) {
    	if(ext->hfit_en != 0) {
			new_cords.x1 -= hpad;
			new_cords.x2 += hpad;
    	} else {
    		new_cords.x1 = rect->cords.x1;
    		new_cords.x2 = rect->cords.x2;
    	}
    	if(ext->vfit_en != 0) {
			new_cords.y1 -= vpad;
			new_cords.y2 += vpad;
    	} else {
    		new_cords.y1 = rect->cords.y1;
    		new_cords.y2 = rect->cords.y2;
    	}

    	/*Do nothing if the coordinates are not changed*/
    	if(rect->cords.x1 != new_cords.x1 ||
    	   rect->cords.y1 != new_cords.y1 ||
           rect->cords.x2 != new_cords.x2 ||
           rect->cords.y2 != new_cords.y2) {

            lv_obj_inv(rect);
            area_cpy(&rect->cords, &new_cords);
            lv_obj_inv(rect);

            /*Notify the object about its new coordinates*/
            rect->signal_f(rect, LV_SIGNAL_CORD_CHG, &ori);

            /*Inform the parent about the new coordinates*/
            lv_obj_t * par = lv_obj_get_parent(rect);
            par->signal_f(par, LV_SIGNAL_CHILD_CHG, rect);
    	}
    }
}

/**
 * Initialize the rectangle styles
 */
static void lv_rects_init(void)
{
	/*Default style*/
	lv_rects_def.objs.color = COLOR_MAKE(0x50, 0x70, 0x90);
	lv_rects_def.gcolor = COLOR_MAKE(0x70, 0xA0, 0xC0);
	lv_rects_def.bcolor = COLOR_WHITE;
	lv_rects_def.bwidth = 2 * LV_DOWNSCALE;
	lv_rects_def.bopa = 50;
	lv_rects_def.round = 4 * LV_DOWNSCALE;
	lv_rects_def.empty = 0;
	lv_rects_def.hpad = 10 * LV_DOWNSCALE;
	lv_rects_def.vpad = 10 * LV_DOWNSCALE;
	lv_rects_def.opad = 10 * LV_DOWNSCALE;
	lv_rects_def.light = 0;
	lv_rects_def.lcolor = COLOR_MAKE(0x60, 0x60, 0x60);

	/*Transparent style*/
	memcpy(&lv_rects_transp, &lv_rects_def, sizeof(lv_rects_t));
	lv_rects_transp.objs.transp = 1;
	lv_rects_transp.bwidth = 0;
	lv_rects_transp.empty = 1;

	/*Border style*/
	memcpy(&lv_rects_border, &lv_rects_def, sizeof(lv_rects_t));
	lv_rects_border.bcolor = COLOR_BLACK;
	lv_rects_border.bwidth = 2 * LV_DOWNSCALE;
	lv_rects_border.bopa = 100;
	lv_rects_border.round = 4 * LV_DOWNSCALE;
	lv_rects_border.empty = 1;
}
#endif
