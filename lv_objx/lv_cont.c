/**
 * @file lv_cont.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"

#if USE_LV_CONT != 0

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "lv_cont.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_draw/lv_draw_vbasic.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_cont_signal(lv_obj_t * cont, lv_signal_t sign, void * param);
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
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a container objects
 * @param par pointer to an object, it will be the parent of the new container
 * @param copy pointer to a container object, if not NULL then the new object will be copied from it
 * @return pointer to the created container
 */
lv_obj_t * lv_cont_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create a basic object*/
    lv_obj_t * new_cont = lv_obj_create(par, copy);
    lv_mem_assert(new_cont);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_cont);

    lv_obj_allocate_ext_attr(new_cont, sizeof(lv_cont_ext_t));
    lv_cont_ext_t * ext = lv_obj_get_ext_attr(new_cont);
    lv_mem_assert(ext);
    ext->hor_fit = 0;
    ext->ver_fit = 0;
    ext->layout = LV_LAYOUT_OFF;

    lv_obj_set_signal_func(new_cont, lv_cont_signal);

    /*Init the new container*/
    if(copy == NULL) {
        /*Set the default styles*/
        lv_theme_t *th = lv_theme_get_current();
        if(th) {
            lv_cont_set_style(new_cont, th->cont);
        } else {
            lv_cont_set_style(new_cont, &lv_style_pretty);
        }
    }
    /*Copy an existing object*/
    else {
    	lv_cont_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
    	ext->hor_fit = copy_ext->hor_fit;
    	ext->ver_fit = copy_ext->ver_fit;
    	ext->layout = copy_ext->layout;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_cont);
    }

    return new_cont;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a layout on a container
 * @param cont pointer to a container object
 * @param layout a layout from 'lv_cont_layout_t'
 */
void lv_cont_set_layout(lv_obj_t * cont, lv_layout_t layout)
{
	lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);
	if(ext->layout == layout) return;

	ext->layout = layout;

	/*Send a signal to refresh the layout*/
	cont->signal_func(cont, LV_SIGNAL_CHILD_CHG, NULL);
}


/**
 * Enable the horizontal or vertical fit.
 * The container size will be set to involve the children horizontally or vertically.
 * @param cont pointer to a container object
 * @param hor_en true: enable the horizontal fit
 * @param ver_en true: enable the vertical fit
 */
void lv_cont_set_fit(lv_obj_t * cont, bool hor_en, bool ver_en)
{
	lv_obj_invalidate(cont);
	lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);
	if(ext->hor_fit == hor_en && ext->ver_fit == ver_en) return;

	ext->hor_fit = hor_en == false ? 0 : 1;
	ext->ver_fit = ver_en == false ? 0 : 1;

	/*Send a signal to set a new size*/
	lv_area_t area;
	lv_obj_get_coords(cont, &area);
	cont->signal_func(cont, LV_SIGNAL_CORD_CHG, &area);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the layout of a container
 * @param cont pointer to container object
 * @return the layout from 'lv_cont_layout_t'
 */
lv_layout_t lv_cont_get_layout(lv_obj_t * cont)
{
	lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);
	return ext->layout;
}

/**
 * Get horizontal fit enable attribute of a container
 * @param cont pointer to a container object
 * @return true: horizontal fit is enabled; false: disabled
 */
bool lv_cont_get_hor_fit(lv_obj_t * cont)
{
	lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);
	return ext->hor_fit == 0 ? false : true;
}

/**
 * Get vertical fit enable attribute of a container
 * @param cont pointer to a container object
 * @return true: vertical fit is enabled; false: disabled
 */
bool lv_cont_get_ver_fit(lv_obj_t * cont)
{
	lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);
	return ext->ver_fit == 0 ? false : true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the container
 * @param cont pointer to a container object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_cont_signal(lv_obj_t * cont, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(cont, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_STYLE_CHG) { /*Recalculate the padding if the style changed*/
        lv_cont_refr_layout(cont);
        lv_cont_refr_autofit(cont);
    } else if(sign == LV_SIGNAL_CHILD_CHG) {
        lv_cont_refr_layout(cont);
        lv_cont_refr_autofit(cont);
    } else if(sign == LV_SIGNAL_CORD_CHG) {
        if(lv_obj_get_width(cont) != lv_area_get_width(param) ||
           lv_obj_get_height(cont) != lv_area_get_height(param)) {
            lv_cont_refr_layout(cont);
            lv_cont_refr_autofit(cont);
        }
    }
    else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_cont";
    }

    return res;
}


/**
 * Refresh the layout of a container
 * @param cont pointer to an object which layout should be refreshed
 */
static void lv_cont_refr_layout(lv_obj_t * cont)
{
	lv_layout_t type = lv_cont_get_layout(cont);

	/*'cont' has to be at least 1 child*/
	if(lv_obj_get_child(cont, NULL) == NULL) return;

	if(type == LV_LAYOUT_OFF) return;

	if(type == LV_LAYOUT_CENTER) {
		lv_cont_layout_center(cont);
	} else if(type == LV_LAYOUT_COL_L || type == LV_LAYOUT_COL_M || type == LV_LAYOUT_COL_R) {
		lv_cont_layout_col(cont);
	} else if(type == LV_LAYOUT_ROW_T || type == LV_LAYOUT_ROW_M || type == LV_LAYOUT_ROW_B) {
		lv_cont_layout_row(cont);
	} else if(type == LV_LAYOUT_PRETTY) {
		lv_cont_layout_pretty(cont);
	}  else if(type == LV_LAYOUT_GRID) {
		lv_cont_layout_grid(cont);
	}
}

/**
 * Handle column type layouts
 * @param cont pointer to an object which layout should be handled
 */
static void lv_cont_layout_col(lv_obj_t * cont)
{
	lv_layout_t type = lv_cont_get_layout(cont);
	lv_obj_t * child;

	/*Adjust margin and get the alignment type*/
	lv_align_t align;
	lv_style_t * style = lv_obj_get_style(cont);
	lv_coord_t hpad_corr;

	switch(type) {
		case LV_LAYOUT_COL_L:
            hpad_corr = style->body.padding.hor;
			align = LV_ALIGN_IN_TOP_LEFT;
			break;
		case LV_LAYOUT_COL_M:
			hpad_corr = 0;
			align = LV_ALIGN_IN_TOP_MID;
			break;
		case LV_LAYOUT_COL_R:
			hpad_corr = -style->body.padding.hor;
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
	lv_coord_t last_cord = style->body.padding.ver;
	LL_READ_BACK(cont->child_ll, child) {
        if(lv_obj_get_hidden(child) != false ||
           lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

		lv_obj_align(child, cont, align, hpad_corr , last_cord);
		last_cord += lv_obj_get_height(child) + style->body.padding.inner;
	}

    lv_obj_clear_protect(cont, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle row type layouts
 * @param cont pointer to an object which layout should be handled
 */
static void lv_cont_layout_row(lv_obj_t * cont)
{
	lv_layout_t type = lv_cont_get_layout(cont);
	lv_obj_t * child;

	/*Adjust margin and get the alignment type*/
	lv_align_t align;
	lv_style_t * style = lv_obj_get_style(cont);
	lv_coord_t vpad_corr = style->body.padding.ver;

	switch(type) {
		case LV_LAYOUT_ROW_T:
			vpad_corr = style->body.padding.ver;
			align = LV_ALIGN_IN_TOP_LEFT;
			break;
		case LV_LAYOUT_ROW_M:
			vpad_corr = 0;
			align = LV_ALIGN_IN_LEFT_MID;
			break;
		case LV_LAYOUT_ROW_B:
			vpad_corr = -style->body.padding.ver;
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
	lv_coord_t last_cord = style->body.padding.hor;
	LL_READ_BACK(cont->child_ll, child) {
		if(lv_obj_get_hidden(child) != false ||
           lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

		lv_obj_align(child, cont, align, last_cord, vpad_corr);
		last_cord += lv_obj_get_width(child) + style->body.padding.inner;
	}

    lv_obj_clear_protect(cont, LV_PROTECT_CHILD_CHG);
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
	lv_coord_t h_tot = 0;

	LL_READ(cont->child_ll, child) {
        if(lv_obj_get_hidden(child) != false ||
           lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;
		h_tot += lv_obj_get_height(child) + style->body.padding.inner;
		obj_num ++;
	}

	if(obj_num == 0) return;

	h_tot -= style->body.padding.inner;

	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/
    lv_obj_set_protect(cont, LV_PROTECT_CHILD_CHG);

	/* Align the children */
	lv_coord_t last_cord = - (h_tot / 2);
	LL_READ_BACK(cont->child_ll, child) {
        if(lv_obj_get_hidden(child) != false ||
           lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

		lv_obj_align(child, cont, LV_ALIGN_CENTER, 0, last_cord + lv_obj_get_height(child) / 2);
		last_cord += lv_obj_get_height(child) + style->body.padding.inner;
	}

    lv_obj_clear_protect(cont, LV_PROTECT_CHILD_CHG);
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
	lv_coord_t w_obj = lv_obj_get_width(cont);
	lv_coord_t act_y = style->body.padding.ver;
	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/

	child_rs = lv_ll_get_tail(&cont->child_ll); /*Set the row starter child*/
	if(child_rs == NULL) return;	/*Return if no child*/

    lv_obj_set_protect(cont, LV_PROTECT_CHILD_CHG);

	child_rc = child_rs; /*Initially the the row starter and closer is the same*/
	while(child_rs != NULL) {
		lv_coord_t h_row = 0;
		lv_coord_t w_row = style->body.padding.hor * 2; /*The width is at least the left+right hpad*/
		uint32_t obj_num = 0;

		/*Find the row closer object and collect some data*/
		do {
			if(lv_obj_get_hidden(child_rc) == false &&
			   lv_obj_is_protected(child_rc, LV_PROTECT_POS) == false) {
			    /*If this object is already not fit then break*/
				if(w_row + lv_obj_get_width(child_rc) > w_obj) {
			        /*Step back one child because the last already not fit, so the previous is the closer*/
			        if(child_rc != NULL  && obj_num != 0 ) {
			            child_rc = lv_ll_get_next(&cont->child_ll, child_rc);
			        }
				    break;
				}
				w_row += lv_obj_get_width(child_rc) + style->body.padding.inner; /*Add the object width + opad*/
				h_row = LV_MATH_MAX(h_row, lv_obj_get_height(child_rc)); /*Search the highest object*/
				obj_num ++;
				if(lv_obj_is_protected(child_rc, LV_PROTECT_FOLLOW)) break; /*If can not be followed by an other object then break here*/

			}
			child_rc = lv_ll_get_prev(&cont->child_ll, child_rc); /*Load the next object*/
			if(obj_num == 0) child_rs = child_rc; /*If the first object was hidden (or too long) then set the next as first */
		}while(child_rc != NULL);

		/*If the object is too long  then align it to the middle*/
		if(obj_num == 0) {
			if(child_rc != NULL) {
				lv_obj_align(child_rc, cont, LV_ALIGN_IN_TOP_MID, 0, act_y);
				h_row = lv_obj_get_height(child_rc);    /*Not set previously because of the early break*/
			}
		}
		/*If there is only one object in the row then align it to the middle*/
		else if (obj_num == 1) {
			lv_obj_align(child_rs, cont, LV_ALIGN_IN_TOP_MID, 0, act_y);
		}
        /*If there are two object in the row then align them proportionally*/
        else if (obj_num == 2) {
            lv_obj_t * obj1 = child_rs;
            lv_obj_t * obj2 = lv_ll_get_prev(&cont->child_ll, child_rs);
            w_row = lv_obj_get_width(obj1) + lv_obj_get_width(obj2);
            lv_coord_t pad = (w_obj - w_row) / 3;
            lv_obj_align(obj1, cont, LV_ALIGN_IN_TOP_LEFT, pad, act_y + (h_row - lv_obj_get_height(obj1)) / 2);
            lv_obj_align(obj2, cont, LV_ALIGN_IN_TOP_RIGHT, -pad, act_y + (h_row - lv_obj_get_height(obj2)) / 2);
        }
		/* Align the children (from child_rs to child_rc)*/
		else {
			w_row -= style->body.padding.inner * obj_num;
			lv_coord_t new_opad = (w_obj -  w_row) / (obj_num  - 1);
			lv_coord_t act_x = style->body.padding.hor; /*x init*/
			child_tmp = child_rs;
			while(child_tmp != NULL) {
				if(lv_obj_get_hidden(child_tmp) == false &&
				   lv_obj_is_protected(child_tmp, LV_PROTECT_POS) == false) {
					lv_obj_align(child_tmp, cont, LV_ALIGN_IN_TOP_LEFT, act_x, act_y + (h_row - lv_obj_get_height(child_tmp)) / 2);
					act_x += lv_obj_get_width(child_tmp) + new_opad;
				}
				if(child_tmp == child_rc) break;
				child_tmp = lv_ll_get_prev(&cont->child_ll, child_tmp);
			}

		}

		if(child_rc == NULL) break;
		act_y += style->body.padding.inner + h_row; /*y increment*/
		child_rs = lv_ll_get_prev(&cont->child_ll, child_rc); /*Go to the next object*/
		child_rc = child_rs;
	}
    lv_obj_clear_protect(cont, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle the grid layout. Align same-sized objects in a grid
 * @param cont pointer to an object which layout should be handled
 */
static void lv_cont_layout_grid(lv_obj_t * cont)
{
	lv_obj_t * child;
	lv_style_t * style = lv_obj_get_style(cont);
	lv_coord_t w_tot = lv_obj_get_width(cont);
	lv_coord_t w_obj = lv_obj_get_width(lv_obj_get_child(cont, NULL));
	lv_coord_t h_obj = lv_obj_get_height(lv_obj_get_child(cont, NULL));
	uint16_t obj_row = (w_tot - (2 * style->body.padding.hor)) / (w_obj + style->body.padding.inner); /*Obj. num. in a row*/
	lv_coord_t x_ofs;
	if(obj_row > 1) {
		x_ofs = w_obj + (w_tot - (2 * style->body.padding.hor) - (obj_row * w_obj)) / (obj_row - 1);
	} else {
		x_ofs = w_tot / 2 - w_obj / 2;
	}
	lv_coord_t y_ofs = h_obj + style->body.padding.inner;

	/* Disable child change action because the children will be moved a lot
	 * an unnecessary child change signals could be sent*/
    lv_obj_set_protect(cont, LV_PROTECT_CHILD_CHG);

	/* Align the children */
	lv_coord_t act_x = style->body.padding.hor;
	lv_coord_t act_y = style->body.padding.ver;
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
			act_x = style->body.padding.hor;
			act_y += y_ofs;
		}
	}

    lv_obj_clear_protect(cont, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle auto fit. Set the size of the object to involve all children.
 * @param cont pointer to an object which size will be modified
 */
static void lv_cont_refr_autofit(lv_obj_t * cont)
{
	lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);

	if(ext->hor_fit == 0 &&
	   ext->ver_fit == 0) {
		return;
	}

	lv_area_t new_cords;
	lv_area_t ori;
	lv_style_t * style = lv_obj_get_style(cont);
	lv_obj_t * i;
	lv_coord_t hpad = style->body.padding.hor;
	lv_coord_t vpad = style->body.padding.ver;

	/*Search the side coordinates of the children*/
	lv_obj_get_coords(cont, &ori);
	lv_obj_get_coords(cont, &new_cords);

	new_cords.x1 = LV_COORD_MAX;
	new_cords.y1 = LV_COORD_MAX;
	new_cords.x2 = LV_COORD_MIN;
	new_cords.y2 = LV_COORD_MIN;

    LL_READ(cont->child_ll, i) {
		if(lv_obj_get_hidden(i) != false) continue;
    	new_cords.x1 = LV_MATH_MIN(new_cords.x1, i->coords.x1);
    	new_cords.y1 = LV_MATH_MIN(new_cords.y1, i->coords.y1);
        new_cords.x2 = LV_MATH_MAX(new_cords.x2, i->coords.x2);
        new_cords.y2 = LV_MATH_MAX(new_cords.y2, i->coords.y2);
    }

    /*If the value is not the init value then the page has >=1 child.*/
    if(new_cords.x1 != LV_COORD_MAX) {
    	if(ext->hor_fit != 0) {
			new_cords.x1 -= hpad;
			new_cords.x2 += hpad;
    	} else {
    		new_cords.x1 = cont->coords.x1;
    		new_cords.x2 = cont->coords.x2;
    	}
    	if(ext->ver_fit != 0) {
			new_cords.y1 -= vpad;
			new_cords.y2 += vpad;
    	} else {
    		new_cords.y1 = cont->coords.y1;
    		new_cords.y2 = cont->coords.y2;
    	}

    	/*Do nothing if the coordinates are not changed*/
    	if(cont->coords.x1 != new_cords.x1 ||
    	   cont->coords.y1 != new_cords.y1 ||
           cont->coords.x2 != new_cords.x2 ||
           cont->coords.y2 != new_cords.y2) {

            lv_obj_invalidate(cont);
            lv_area_copy(&cont->coords, &new_cords);
            lv_obj_invalidate(cont);

            /*Notify the object about its new coordinates*/
            cont->signal_func(cont, LV_SIGNAL_CORD_CHG, &ori);

            /*Inform the parent about the new coordinates*/
            lv_obj_t * par = lv_obj_get_parent(cont);
            par->signal_func(par, LV_SIGNAL_CHILD_CHG, cont);
    	}
    }
}

#endif
