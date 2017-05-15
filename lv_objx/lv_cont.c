/**
 * @file lv_rect.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"

#if USE_LV_CONT != 0

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "lv_cont.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_draw/lv_draw_vbasic.h"
#include "misc/gfx/area.h"

#include "misc/gfx/color.h"
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
#if 0
static bool lv_cont_design(lv_obj_t * cont, const area_t * mask, lv_design_mode_t mode);
#endif

static void lv_cont_refr_layout(lv_obj_t * cont);
static void lv_cont_layout_col(lv_obj_t * cont);
static void lv_cont_layout_row(lv_obj_t * cont);
static void lv_cont_layout_center(lv_obj_t * cont);
static void lv_cont_layout_pretty(lv_obj_t * cont);
static void lv_cont_layout_grid(lv_obj_t * cont);
static void lv_cont_refr_autofit(lv_obj_t * cont);

/**********************
 *  STATIC VARIABLES
 **********************/

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
 * Create a container objects
 * @param par pointer to an object, it will be the parent of the new container
 * @param copy pointer to a container object, if not NULL then the new object will be copied from it
 * @return pointer to the created container
 */
lv_obj_t * lv_cont_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create a basic object*/
    lv_obj_t * new_rect = lv_obj_create(par, copy);
    dm_assert(new_rect);
    lv_obj_alloc_ext(new_rect, sizeof(lv_cont_ext_t));
    lv_cont_ext_t * ext = lv_obj_get_ext(new_rect);
    dm_assert(ext);
    ext->hfit_en = 0;
    ext->vfit_en = 0;
    ext->layout = LV_CONT_LAYOUT_OFF;

    lv_obj_set_signal_f(new_rect, lv_cont_signal);

    /*Init the new container*/
    if(copy == NULL) {
		lv_obj_set_style(new_rect, lv_style_get(LV_STYLE_PLAIN, NULL));
    }
    /*Copy an existing object*/
    else {
    	lv_cont_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->hfit_en = copy_ext->hfit_en;
    	ext->vfit_en = copy_ext->vfit_en;
    	ext->layout = copy_ext->layout;

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_rect);

    }

    return new_rect;
}

/**
 * Signal function of the container
 * @param cont pointer to a container object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_cont_signal(lv_obj_t * cont, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_obj_signal(cont, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	switch(sign) {
    	case LV_SIGNAL_STYLE_CHG: /*Recalculate the padding if the style changed*/
        	lv_cont_refr_layout(cont);
        	lv_cont_refr_autofit(cont);
        	break;
        case LV_SIGNAL_CHILD_CHG:
        	lv_cont_refr_layout(cont);
        	lv_cont_refr_autofit(cont);
        	break;
        case LV_SIGNAL_CORD_CHG:
        	if(lv_obj_get_width(cont) != area_get_width(param) ||
    		  lv_obj_get_height(cont) != area_get_height(param)) {
            	lv_cont_refr_layout(cont);
            	lv_cont_refr_autofit(cont);
        	}
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
 * Set the layout on a container
 * @param cont pointer to a container object
 * @param layout a layout from 'lv_cont_layout_t'
 */
void lv_cont_set_layout(lv_obj_t * cont, lv_cont_layout_t layout)
{
	lv_cont_ext_t * ext = lv_obj_get_ext(cont);
	ext->layout = layout;

	/*Send a signal to refresh the layout*/
	cont->signal_f(cont, LV_SIGNAL_CHILD_CHG, NULL);
}


/**
 * Enable the horizontal or vertical fit.
 * The container size will be set to involve the children horizontally or vertically.
 * @param cont pointer to a container object
 * @param hor_en true: enable the horizontal padding
 * @param ver_en true: enable the vertical padding
 */
void lv_cont_set_fit(lv_obj_t * cont, bool hor_en, bool ver_en)
{
	lv_obj_inv(cont);
	lv_cont_ext_t * ext = lv_obj_get_ext(cont);
	ext->hfit_en = hor_en == false ? 0 : 1;
	ext->vfit_en = ver_en == false ? 0 : 1;

	/*Send a signal to set a new size*/
	cont->signal_f(cont, LV_SIGNAL_CORD_CHG, cont);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the layout of a container
 * @param cont pointer to container object
 * @return the layout from 'lv_cont_layout_t'
 */
lv_cont_layout_t lv_cont_get_layout(lv_obj_t * cont)
{
	lv_cont_ext_t * ext = lv_obj_get_ext(cont);
	return ext->layout;
}

/**
 * Get horizontal fit enable attribute of a container
 * @param cont pointer to a container object
 * @return true: horizontal padding is enabled
 */
bool lv_cont_get_hfit(lv_obj_t * cont)
{
	lv_cont_ext_t * ext = lv_obj_get_ext(cont);
	return ext->hfit_en == 0 ? false : true;
}

/**
 * Get vertical fit enable attribute of a container
 * @param cont pointer to a container object
 * @return true: vertical padding is enabled
 */
bool lv_cont_get_vfit(lv_obj_t * cont)
{
	lv_cont_ext_t * ext = lv_obj_get_ext(cont);
	return ext->vfit_en == 0 ? false : true;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
#if 0
/**
 * Handle the drawing related tasks of the containers
 * @param cont pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_cont_design(lv_obj_t * cont, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {

    	return false;
    } else if(mode == LV_DESIGN_DRAW_MAIN) {


    } else if(mode == LV_DESIGN_DRAW_POST) {

    }
    return true;
}
#endif


/**
 * Refresh the layout of a container
 * @param cont pointer to an object which layout should be refreshed
 */
static void lv_cont_refr_layout(lv_obj_t * cont)
{
	lv_cont_layout_t type = lv_cont_get_layout(cont);

	/*'rect' has to be at least 1 child*/
	if(lv_obj_get_child(cont, NULL) == NULL) return;

	if(type == LV_CONT_LAYOUT_OFF) return;

	if(type == LV_CONT_LAYOUT_CENTER) {
		lv_cont_layout_center(cont);
	} else if(type == LV_CONT_LAYOUT_COL_L || type == LV_CONT_LAYOUT_COL_M || type == LV_CONT_LAYOUT_COL_R) {
		lv_cont_layout_col(cont);
	} else if(type == LV_CONT_LAYOUT_ROW_T || type == LV_CONT_LAYOUT_ROW_M || type == LV_CONT_LAYOUT_ROW_B) {
		lv_cont_layout_row(cont);
	} else if(type == LV_CONT_LAYOUT_PRETTY) {
		lv_cont_layout_pretty(cont);
	}  else if(type == LV_CONT_LAYOUT_GRID) {
		lv_cont_layout_grid(cont);
	}
}

/**
 * Handle column type layouts
 * @param cont pointer to an object which layout should be handled
 */
static void lv_cont_layout_col(lv_obj_t * cont)
{
	lv_cont_layout_t type = lv_cont_get_layout(cont);
	lv_obj_t * child;

	/*Adjust margin and get the alignment type*/
	lv_align_t align;
	lv_style_t * style = lv_obj_get_style(cont);
	cord_t hpad_corr;

	switch(type) {
		case LV_CONT_LAYOUT_COL_L:
            hpad_corr = style->hpad;
			align = LV_ALIGN_IN_TOP_LEFT;
			break;
		case LV_CONT_LAYOUT_COL_M:
			hpad_corr = 0;
			align = LV_ALIGN_IN_TOP_MID;
			break;
		case LV_CONT_LAYOUT_COL_R:
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
	lv_obj_set_protect(cont, LV_PROTECT_CHILD_CHG);
	/* Align the children */
	cord_t last_cord = style->vpad;
	LL_READ_BACK(cont->child_ll, child) {
        if(lv_obj_get_hidden(child) != false ||
           lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

		lv_obj_align(child, cont, align, hpad_corr , last_cord);
		last_cord += lv_obj_get_height(child) + style->opad;
	}

    lv_obj_clr_protect(cont, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle row type layouts
 * @param cont pointer to an object which layout should be handled
 */
static void lv_cont_layout_row(lv_obj_t * cont)
{
	lv_cont_layout_t type = lv_cont_get_layout(cont);
	lv_obj_t * child;

	/*Adjust margin and get the alignment type*/
	lv_align_t align;
	lv_style_t * style = lv_obj_get_style(cont);
	cord_t vpad_corr = style->vpad;

	switch(type) {
		case LV_CONT_LAYOUT_ROW_T:
			vpad_corr = style->vpad;
			align = LV_ALIGN_IN_TOP_LEFT;
			break;
		case LV_CONT_LAYOUT_ROW_M:
			vpad_corr = 0;
			align = LV_ALIGN_IN_LEFT_MID;
			break;
		case LV_CONT_LAYOUT_ROW_B:
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
    lv_obj_set_protect(cont, LV_PROTECT_CHILD_CHG);

	/* Align the children */
	cord_t last_cord = style->hpad;
	LL_READ_BACK(cont->child_ll, child) {
		if(lv_obj_get_hidden(child) != false ||
           lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

		lv_obj_align(child, cont, align, last_cord, vpad_corr);
		last_cord += lv_obj_get_width(child) + style->opad;
	}

    lv_obj_clr_protect(cont, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle the center layout
 * @param cont pointer to an object which layout should be handled
 */
static void lv_cont_layout_center(lv_obj_t * cont)
{
	lv_obj_t * child;
	lv_style_t * style = lv_obj_get_style(cont);
	uint32_t obj_num = 0;
	cord_t h_tot = 0;

	LL_READ(cont->child_ll, child) {
		h_tot += lv_obj_get_height(child) + style->opad;
		obj_num ++;
	}

	if(obj_num == 0) return;

	h_tot -= style->opad;

	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/
    lv_obj_set_protect(cont, LV_PROTECT_CHILD_CHG);

	/* Align the children */
	cord_t last_cord = - (h_tot / 2);
	LL_READ_BACK(cont->child_ll, child) {
        if(lv_obj_get_hidden(child) != false ||
           lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

		lv_obj_align(child, cont, LV_ALIGN_CENTER, 0, last_cord + lv_obj_get_height(child) / 2);
		last_cord += lv_obj_get_height(child) + style->opad;
	}

    lv_obj_clr_protect(cont, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle the pretty layout. Put as many object as possible in row
 * then begin a new row
 * @param cont pointer to an object which layout should be handled
 */
static void lv_cont_layout_pretty(lv_obj_t * cont)
{
	lv_obj_t * child_rs;    /* Row starter child */
	lv_obj_t * child_rc;    /* Row closer child */
	lv_obj_t * child_tmp;   /* Temporary child */
	lv_style_t * style = lv_obj_get_style(cont);
	cord_t w_obj = lv_obj_get_width(cont);
	cord_t act_y = style->vpad;
	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/

	child_rs = ll_get_tail(&cont->child_ll); /*Set the row starter child*/
	if(child_rs == NULL) return;	/*Return if no child*/

    lv_obj_set_protect(cont, LV_PROTECT_CHILD_CHG);

	child_rc = child_rs; /*Initially the the row starter and closer is the same*/
	while(child_rs != NULL) {
		cord_t h_row = 0;
		cord_t w_row = style->hpad * 2; /*The width is at least the left+right hpad*/
		uint32_t obj_num = 0;

		/*Find the row closer object and collect some data*/
		do {
			if(lv_obj_get_hidden(child_rc) == false &&
			   lv_obj_is_protected(child_rc, LV_PROTECT_POS) == false) {
				if(w_row + lv_obj_get_width(child_rc) > w_obj) break; /*If the next object is already not fit then break*/
				w_row += lv_obj_get_width(child_rc) + style->opad; /*Add the object width + opad*/
				h_row = MATH_MAX(h_row, lv_obj_get_height(child_rc)); /*Search the highest object*/
				obj_num ++;
			}
			child_rc = ll_get_prev(&cont->child_ll, child_rc); /*Load the next object*/
			if(obj_num == 0) child_rs = child_rc; /*If the first object was hidden (or too long) then set the next as first */
		}while(child_rc != NULL);

		/*Step back one child because the last already not fit*/
		if(child_rc != NULL  && obj_num != 0) child_rc = ll_get_next(&cont->child_ll, child_rc);

		/*If the object is too long  then align it to the middle*/
		if(obj_num == 0) {
			if(child_rc != NULL) {
				lv_obj_align(child_rc, cont, LV_ALIGN_IN_TOP_MID, 0, act_y);
			}
		}
		/*If here is only one object in the row then align it to the left*/
		else if (obj_num == 1) {
			lv_obj_align(child_rs, cont, LV_ALIGN_IN_TOP_MID, 0, act_y);
		}
		/* Align the children (from child_rs to child_rc)*/
		else {
			w_row -= style->opad * obj_num;
			cord_t new_opad = (w_obj -  w_row) / (obj_num  - 1);
			cord_t act_x = style->hpad; /*x init*/
			child_tmp = child_rs;
			while(child_tmp != NULL) {
				if(lv_obj_get_hidden(child_tmp) == false &&
				   lv_obj_is_protected(child_tmp, LV_PROTECT_POS) == false) {
					lv_obj_align(child_tmp, cont, LV_ALIGN_IN_TOP_LEFT, act_x, act_y);
					act_x += lv_obj_get_width(child_tmp) + new_opad;
				}
				if(child_tmp == child_rc) break;
				child_tmp = ll_get_prev(&cont->child_ll, child_tmp);
			}

		}

		if(child_rc == NULL) break;
		act_y += style->opad + h_row; /*y increment*/
		child_rs = ll_get_prev(&cont->child_ll, child_rc); /*Go to the next object*/
		child_rc = child_rs;
	}
    lv_obj_clr_protect(cont, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle the grid layout. Align same-sized objects in a grid
 * @param cont pointer to an object which layout should be handled
 */
static void lv_cont_layout_grid(lv_obj_t * cont)
{
	lv_obj_t * child;
	lv_style_t * style = lv_obj_get_style(cont);
	cord_t w_tot = lv_obj_get_width(cont);
	cord_t w_obj = lv_obj_get_width(lv_obj_get_child(cont, NULL));
	cord_t h_obj = lv_obj_get_height(lv_obj_get_child(cont, NULL));
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
    lv_obj_set_protect(cont, LV_PROTECT_CHILD_CHG);

	/* Align the children */
	cord_t act_x = style->hpad;
	cord_t act_y = style->vpad;
	uint16_t obj_cnt = 0;
	LL_READ_BACK(cont->child_ll, child) {
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

    lv_obj_clr_protect(cont, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle auto fit. Set the size of the object to involve all children.
 * @param cont pointer to an object which size will be modified
 */
static void lv_cont_refr_autofit(lv_obj_t * cont)
{
	lv_cont_ext_t * ext = lv_obj_get_ext(cont);

	if(ext->hfit_en == 0 &&
	   ext->vfit_en == 0) {
		return;
	}

	area_t new_cords;
	area_t ori;
	lv_style_t * style = lv_obj_get_style(cont);
	lv_obj_t * i;
	cord_t hpad = style->hpad;
	cord_t vpad = style->vpad;

	/*Search the side coordinates of the children*/
	lv_obj_get_cords(cont, &ori);
	lv_obj_get_cords(cont, &new_cords);

	new_cords.x1 = CORD_MAX;
	new_cords.y1 = CORD_MAX;
	new_cords.x2 = CORD_MIN;
	new_cords.y2 = CORD_MIN;

    LL_READ(cont->child_ll, i) {
		if(lv_obj_get_hidden(i) != false) continue;
    	new_cords.x1 = MATH_MIN(new_cords.x1, i->cords.x1);
    	new_cords.y1 = MATH_MIN(new_cords.y1, i->cords.y1);
        new_cords.x2 = MATH_MAX(new_cords.x2, i->cords.x2);
        new_cords.y2 = MATH_MAX(new_cords.y2, i->cords.y2);
    }

    /*If the value is not the init value then the page has >=1 child.*/
    if(new_cords.x1 != CORD_MAX) {
    	if(ext->hfit_en != 0) {
			new_cords.x1 -= hpad;
			new_cords.x2 += hpad;
    	} else {
    		new_cords.x1 = cont->cords.x1;
    		new_cords.x2 = cont->cords.x2;
    	}
    	if(ext->vfit_en != 0) {
			new_cords.y1 -= vpad;
			new_cords.y2 += vpad;
    	} else {
    		new_cords.y1 = cont->cords.y1;
    		new_cords.y2 = cont->cords.y2;
    	}

    	/*Do nothing if the coordinates are not changed*/
    	if(cont->cords.x1 != new_cords.x1 ||
    	   cont->cords.y1 != new_cords.y1 ||
           cont->cords.x2 != new_cords.x2 ||
           cont->cords.y2 != new_cords.y2) {

            lv_obj_inv(cont);
            area_cpy(&cont->cords, &new_cords);
            lv_obj_inv(cont);

            /*Notify the object about its new coordinates*/
            cont->signal_f(cont, LV_SIGNAL_CORD_CHG, &ori);

            /*Inform the parent about the new coordinates*/
            lv_obj_t * par = lv_obj_get_parent(cont);
            par->signal_f(par, LV_SIGNAL_CHILD_CHG, cont);
    	}
    }
}

#endif
