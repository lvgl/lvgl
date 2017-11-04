/**
 * @file lv_page.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_PAGE != 0

#include "misc/math/math_base.h"
#include "../lv_obj/lv_group.h"
#include "../lv_objx/lv_page.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_obj/lv_refr.h"
#include "misc/gfx/anim.h"

/*********************
 *      DEFINES
 *********************/
#define LV_PAGE_SB_MIN_SIZE    (LV_DPI / 8)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_page_sb_refresh(lv_obj_t * main);
static bool lv_page_design(lv_obj_t * scrl, const area_t * mask, lv_design_mode_t mode);
static bool lv_scrl_design(lv_obj_t * scrl, const area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_page_signal(lv_obj_t * page, lv_signal_t sign, void * param);
static lv_res_t lv_page_scrollable_signal(lv_obj_t * scrl, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;
static lv_design_func_t ancestor_design;

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
 * Create a page objects
 * @param par pointer to an object, it will be the parent of the new page
 * @param copy pointer to a page object, if not NULL then the new object will be copied from it
 * @return pointer to the created page
 */
lv_obj_t * lv_page_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor object*/
    lv_obj_t * new_page = lv_cont_create(par, copy);
    dm_assert(new_page);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_page);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_page);

    /*Allocate the object type specific extended data*/
    lv_page_ext_t * ext = lv_obj_allocate_ext_attr(new_page, sizeof(lv_page_ext_t));
    dm_assert(ext);
    ext->scrl = NULL;
    ext->pr_action = NULL;
    ext->rel_action = NULL;
    ext->sb.hor_draw = 0;
    ext->sb.ver_draw = 0;
    ext->sb.style = &lv_style_pretty;
    ext->sb.mode = LV_PAGE_SB_MODE_AUTO;

    /*Init the new page object*/
    if(copy == NULL) {
	    ext->scrl = lv_cont_create(new_page, NULL);
	    lv_obj_set_signal_func(ext->scrl, lv_page_scrollable_signal);
        lv_obj_set_design_func(ext->scrl, lv_scrl_design);
		lv_obj_set_drag(ext->scrl, true);
		lv_obj_set_drag_throw(ext->scrl, true);
		lv_obj_set_protect(ext->scrl, LV_PROTECT_PARENT);
		lv_cont_set_fit(ext->scrl, false, true);

		/* Add the signal function only if 'scrolling' is created
		 * because everything has to be ready before any signal is received*/
	    lv_obj_set_signal_func(new_page, lv_page_signal);
	    lv_obj_set_design_func(new_page, lv_page_design);
	    lv_page_set_style(new_page, &lv_style_pretty_color, &lv_style_pretty, &lv_style_pretty_color);
        lv_page_set_sb_mode(new_page, ext->sb.mode);
    } else {
    	lv_page_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
    	ext->scrl = lv_cont_create(new_page, copy_ext->scrl);
	    lv_obj_set_signal_func(ext->scrl, lv_page_scrollable_signal);

        lv_page_set_press_action(new_page, copy_ext->pr_action);
        lv_page_set_release_action(new_page, copy_ext->rel_action);
        lv_page_set_sb_mode(new_page, copy_ext->sb.mode);
        lv_page_set_style(new_page, lv_obj_get_style(copy), lv_obj_get_style(copy_ext->scrl), copy_ext->sb.style);

		/* Add the signal function only if 'scrolling' is created
		 * because everything has to be ready before any signal is received*/
	    lv_obj_set_signal_func(new_page, lv_page_signal);
	    lv_obj_set_design_func(new_page, lv_page_design);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_page);
    }
    
    lv_page_sb_refresh(new_page);
                
    return new_page;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a release action for the page
 * @param page pointer to a page object
 * @param rel_action a function to call when the page is released
 */
void lv_page_set_release_action(lv_obj_t * page, lv_action_t rel_action)
{
	lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
	ext->rel_action = rel_action;
}

/**
 * Set a press action for the page
 * @param page pointer to a page object
 * @param pr_action a function to call when the page is pressed
 */
void lv_page_set_press_action(lv_obj_t * page, lv_action_t pr_action)
{
	lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
	ext->pr_action = pr_action;
}

/**
 * Set the scroll bar mode on a page
 * @param page pointer to a page object
 * @param sb.mode the new mode from 'lv_page_sb.mode_t' enum
 */
void lv_page_set_sb_mode(lv_obj_t * page, lv_page_sb_mode_t sb_mode)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    ext->sb.mode = sb_mode;
    ext->sb.hor_draw = 0;
    ext->sb.ver_draw = 0;
    lv_page_sb_refresh(page);
    lv_obj_invalidate(page);
}

/**
 * Set a new styles for the page
 * @param page pointer to a page object
 * @param bg pointer to a style for the background
 * @param scrl pointer to a style for the scrollable area
 * @param sb pointer to a style for the scroll bars
 */
void lv_page_set_style(lv_obj_t *page, lv_style_t *bg, lv_style_t *scrl, lv_style_t *sb)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    if(sb != NULL) {
        ext->sb.style = sb;
        area_set_height(&ext->sb.hor_area, ext->sb.style->body.padding.inner);
        area_set_width(&ext->sb.ver_area, ext->sb.style->body.padding.inner);
        lv_page_sb_refresh(page);
        lv_obj_invalidate(page);
    }
    if(scrl != NULL) lv_obj_set_style(ext->scrl, scrl);
    if(bg != NULL) lv_obj_set_style(page, bg);

}

/**
 * Glue the object to the page. After it the page can be moved (dragged) with this object too.
 * @param obj pointer to an object on a page
 * @param glue true: enable glue, false: disable glue
 */
void lv_page_glue_obj(lv_obj_t * obj, bool glue)
{
    lv_obj_set_drag_parent(obj, glue);
    lv_obj_set_drag(obj, glue);
}

/**
 * Focus on an object. It ensures that the object will be visible on the page.
 * @param page pointer to a page object
 * @param obj pointer to an object to focus (must be on the page)
 * @param anim_time scroll animation time in milliseconds (0: no animation)
 */
void lv_page_focus(lv_obj_t * page, lv_obj_t * obj, uint16_t anim_time)
{
	lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
	lv_style_t * style = lv_page_get_style_bg(page);
    lv_style_t * style_scrl = lv_page_get_style_scrl(page);

	cord_t obj_y = obj->coords.y1 - ext->scrl->coords.y1;
	cord_t obj_h = lv_obj_get_height(obj);
	cord_t scrlable_y = lv_obj_get_y(ext->scrl);
	cord_t page_h = lv_obj_get_height(page);

	cord_t top_err = -(scrlable_y + obj_y);
	cord_t bot_err = scrlable_y + obj_y + obj_h - page_h;

	/*If obj is higher then the page focus where the "error" is smaller*/

	/*Out of the page on the top*/
	if((obj_h <= page_h && top_err > 0) ||
	   (obj_h > page_h && top_err < bot_err)) {
		/*Calculate a new position and let some space above*/
		scrlable_y = -(obj_y - style_scrl->body.padding.ver - style->body.padding.ver);
		scrlable_y += style_scrl->body.padding.ver;
	}
	/*Out of the page on the bottom*/
	else if((obj_h <= page_h && bot_err > 0) ||
			(obj_h > page_h && top_err >= bot_err)) {
        /*Calculate a new position and let some space below*/
		scrlable_y = -obj_y;
		scrlable_y += page_h - obj_h;
        scrlable_y -= style_scrl->body.padding.ver;
	} else {
		/*Already in focus*/
		return;
	}

    if(anim_time == 0) {
		lv_obj_set_y(ext->scrl, scrlable_y);
    }
    else {
        anim_t a;
        a.act_time = 0;
        a.start = lv_obj_get_y(ext->scrl);
        a.end = scrlable_y;
        a.time = anim_time;
        a.end_cb = NULL;
        a.playback = 0;
        a.repeat = 0;
        a.var = ext->scrl;
        a.path = anim_get_path(ANIM_PATH_LIN);
        a.fp = (anim_fp_t) lv_obj_set_y;
        anim_create(&a);
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the scrollable object of a page
 * @param page pointer to a page object
 * @return pointer to a container which is the scrollable part of the page
 */
lv_obj_t * lv_page_get_scrl(lv_obj_t * page)
{
	lv_page_ext_t * ext = lv_obj_get_ext_attr(page);

	return ext->scrl;
}

/**
 * Set the scroll bar mode on a page
 * @param page pointer to a page object
 * @return the mode from 'lv_page_sb.mode_t' enum
 */
lv_page_sb_mode_t lv_page_get_sb_mode(lv_obj_t * page)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    return ext->sb.mode;
}

/**
* Get the style of the scrollable part of a page
* @param page pointer to a page object
* @return pointer to the style of the scrollale part
*/
lv_style_t * lv_page_get_style_scrl(lv_obj_t * page)
{
    return lv_obj_get_style(lv_page_get_scrl(page));
}

/**
* Get the style of the scrolbars of a page
* @param page pointer to a page object
* @return pointer to the style of the scrollbars
*/
lv_style_t * lv_page_get_style_sb(lv_obj_t * page)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    return ext->sb.style;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the pages
 * @param page pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_page_design(lv_obj_t * scrl, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	return ancestor_design(scrl, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		ancestor_design(scrl, mask, mode);
	} else if(mode == LV_DESIGN_DRAW_POST) { /*Draw the scroll bars finally*/
		ancestor_design(scrl, mask, mode);
		lv_page_ext_t * ext = lv_obj_get_ext_attr(scrl);

		/*Draw the scrollbars*/
		area_t sb_area;
		if(ext->sb.hor_draw) {
		    /*Convert the relative coordinates to absolute*/
            area_cpy(&sb_area, &ext->sb.hor_area);
		    sb_area.x1 += scrl->coords.x1;
            sb_area.y1 += scrl->coords.y1;
            sb_area.x2 += scrl->coords.x1;
            sb_area.y2 += scrl->coords.y1;
			lv_draw_rect(&sb_area, mask, ext->sb.style);
		}

		if(ext->sb.ver_draw) {
            /*Convert the relative coordinates to absolute*/
            area_cpy(&sb_area, &ext->sb.ver_area);
            sb_area.x1 += scrl->coords.x1;
            sb_area.y1 += scrl->coords.y1;
            sb_area.x2 += scrl->coords.x1;
            sb_area.y2 += scrl->coords.y1;
			lv_draw_rect(&sb_area, mask, ext->sb.style);
		}
	}

	return true;
}

/**
 * Handle the drawing related tasks of the scrollable object
 * @param scrl pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_scrl_design(lv_obj_t * scrl, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_design(scrl, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
#if LV_OBJ_GROUP != 0
        /* If the page is the active in a group and
         * the background (page) is not visible (transparent or empty)
         * then activate the style of the scrollable*/
        lv_style_t * style_ori = lv_obj_get_style(scrl);
        lv_obj_t * page = lv_obj_get_parent(scrl);
        lv_style_t * style_page = lv_obj_get_style(page);
        lv_group_t * g = lv_obj_get_group(page);
        if(style_page->body.empty != 0 || style_page->body.opa == OPA_TRANSP) { /*Background is visible?*/
            if(lv_group_get_focused(g) == page) {
                lv_style_t * style_mod;
                style_mod = lv_group_mod_style(g, style_ori);
                scrl->style_p = style_mod;  /*Temporally change the style to the activated */
            }
        }
#endif
        ancestor_design(scrl, mask, mode);

#if LV_OBJ_GROUP != 0
        scrl->style_p = style_ori;  /*Revert the style*/
#endif
    } else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design(scrl, mask, mode);
    }

    return true;
}

/**
 * Signal function of the page
 * @param page pointer to a page object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_page_signal(lv_obj_t * page, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(page, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(res == LV_RES_OK) {
        lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
        lv_style_t * style = lv_obj_get_style(page);
        lv_obj_t * child;
        if(sign == LV_SIGNAL_CHILD_CHG) { /*Automatically move children to the scrollable object*/
            child = lv_obj_get_child(page, NULL);
            while(child != NULL) {
                if(lv_obj_is_protected(child, LV_PROTECT_PARENT) == false) {
                    lv_obj_t * tmp = child;
                    child = lv_obj_get_child(page, child); /*Get the next child before move this*/
                    lv_obj_set_parent(tmp, ext->scrl);
                } else {
                    child = lv_obj_get_child(page, child);
                }
            }
        }
        else if(sign == LV_SIGNAL_STYLE_CHG) {
            /*If no hor_fit enabled set the scrollable's width to the page's width*/
            if(lv_cont_get_hor_fit(ext->scrl) == false) {
                lv_obj_set_width(ext->scrl, lv_obj_get_width(page) - 2 * style->body.padding.hor);
            } else {
                ext->scrl->signal_func(ext->scrl, LV_SIGNAL_CORD_CHG, &ext->scrl->coords);
            }

            /*The scrollbars are important only if they are visible now*/
            if(ext->sb.hor_draw || ext->sb.ver_draw) lv_page_sb_refresh(page);

        }
        else if(sign == LV_SIGNAL_CORD_CHG) {
            /*Refresh the scrollbar and notify the scrl if the size is changed*/
            if(ext->scrl != NULL && (lv_obj_get_width(page) != area_get_width(param) ||
                                     lv_obj_get_height(page) != area_get_height(param)))
            {
                /*If no hor_fit enabled set the scrollable's width to the page's width*/
                if(lv_cont_get_hor_fit(ext->scrl) == false) {
                    lv_obj_set_width(ext->scrl, lv_obj_get_width(page) - 2 * style->body.padding.hor);
                }

                ext->scrl->signal_func(ext->scrl, LV_SIGNAL_CORD_CHG, &ext->scrl->coords);

                /*The scrollbars are important only if they are visible now*/
                if(ext->sb.hor_draw || ext->sb.ver_draw) lv_page_sb_refresh(page);
            }
        }
        else if(sign == LV_SIGNAL_PRESSED) {
            if(ext->pr_action != NULL) {
                ext->pr_action(page);
            }
        }
        else if(sign == LV_SIGNAL_RELEASED) {
            if(lv_indev_is_dragging(lv_indev_get_act()) == false) {
                if(ext->rel_action != NULL) {
                    ext->rel_action(page);
                }
            }
        }
    }

    return res;
}

/**
 * Signal function of the scrollable part of a page
 * @param scrl pointer to the scrollable object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_page_scrollable_signal(lv_obj_t * scrl, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(scrl, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(res != false) {

        lv_obj_t * page = lv_obj_get_parent(scrl);
        lv_style_t * page_style = lv_obj_get_style(page);
        lv_page_ext_t * page_ext = lv_obj_get_ext_attr(page);

        if(sign == LV_SIGNAL_CORD_CHG) {
            /*Be sure the width of the scrollable is correct*/
            if(lv_cont_get_hor_fit(scrl) == false) {
                lv_obj_set_width(scrl, lv_obj_get_width(page) - 2 * page_style->body.padding.hor);
            }

            /*Limit the position of the scrollable object to be always visible
             * (Do not let its edge inner then its parent respective edge)*/
            cord_t new_x;
            cord_t new_y;
            bool refr_x = false;
            bool refr_y = false;
            area_t page_cords;
            area_t scrl_cords;
            cord_t hpad = page_style->body.padding.hor;
            cord_t vpad = page_style->body.padding.ver;

            new_x = lv_obj_get_x(scrl);
            new_y = lv_obj_get_y(scrl);
            lv_obj_get_coords(scrl, &scrl_cords);
            lv_obj_get_coords(page, &page_cords);

            /*scrollable width smaller then page width? -> align to left*/
            if(area_get_width(&scrl_cords) + 2 * hpad < area_get_width(&page_cords)) {
                if(scrl_cords.x1 != page_cords.x1 + hpad) {
                    new_x = hpad;
                    refr_x = true;
                }
            } else {
                /*The edges of the scrollable can not be in the page (minus hpad) */
                if(scrl_cords.x2  < page_cords.x2 - hpad) {
                   new_x =  area_get_width(&page_cords) - area_get_width(&scrl_cords) - hpad;   /* Right align */
                   refr_x = true;
                }
                if (scrl_cords.x1 > page_cords.x1 + hpad) {
                    new_x = hpad;  /*Left align*/
                    refr_x = true;
                }
            }

            /*scrollable height smaller then page height? -> align to left*/
            if(area_get_height(&scrl_cords) + 2 * vpad < area_get_height(&page_cords)) {
                if(scrl_cords.y1 != page_cords.y1 + vpad) {
                    new_y = vpad;
                    refr_y = true;
                }
            } else {
                /*The edges of the scrollable can not be in the page (minus vpad) */
                if(scrl_cords.y2 < page_cords.y2 - vpad) {
                   new_y =  area_get_height(&page_cords) - area_get_height(&scrl_cords) - vpad;   /* Bottom align */
                   refr_y = true;
                }
                if (scrl_cords.y1  > page_cords.y1 + vpad) {
                    new_y = vpad;  /*Top align*/
                    refr_y = true;
                }
            }
            if(refr_x != false || refr_y != false) {
                lv_obj_set_pos(scrl, new_x, new_y);
            }

            lv_page_sb_refresh(page);
        }
        else if(sign == LV_SIGNAL_DRAG_END) {
            /*Hide scrollbars if required*/
            if(page_ext->sb.mode == LV_PAGE_SB_MODE_DRAG) {
                area_t sb_area_tmp;
                if(page_ext->sb.hor_draw) {
                    area_cpy(&sb_area_tmp, &page_ext->sb.hor_area);
                    sb_area_tmp.x1 += page->coords.x1;
                    sb_area_tmp.y1 += page->coords.y1;
                    sb_area_tmp.x2 += page->coords.x2;
                    sb_area_tmp.y2 += page->coords.y2;
                    lv_inv_area(&sb_area_tmp);
                    page_ext->sb.hor_draw = 0;
                }
                if(page_ext->sb.ver_draw)  {
                    area_cpy(&sb_area_tmp, &page_ext->sb.ver_area);
                    sb_area_tmp.x1 += page->coords.x1;
                    sb_area_tmp.y1 += page->coords.y1;
                    sb_area_tmp.x2 += page->coords.x2;
                    sb_area_tmp.y2 += page->coords.y2;
                    lv_inv_area(&sb_area_tmp);
                    page_ext->sb.ver_draw = 0;
                }
            }
        }
        else if(sign == LV_SIGNAL_PRESSED) {
            if(page_ext->pr_action != NULL) {
                page_ext->pr_action(page);
            }
        }
        else if(sign == LV_SIGNAL_RELEASED) {
            if(lv_indev_is_dragging(lv_indev_get_act()) == false) {
                if(page_ext->rel_action != NULL) {
                    page_ext->rel_action(page);
                }
            }
        }
    }

    return res;
}


/**
 * Refresh the position and size of the scroll bars.
 * @param page pointer to a page object
 */
static void lv_page_sb_refresh(lv_obj_t * page)
{

    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    lv_style_t * style = lv_obj_get_style(page);
    lv_obj_t * scrl = ext->scrl;
    cord_t size_tmp;
    cord_t scrl_w = lv_obj_get_width(scrl);
    cord_t scrl_h =  lv_obj_get_height(scrl);
    cord_t hpad = style->body.padding.hor;
    cord_t vpad = style->body.padding.ver;
    cord_t obj_w = lv_obj_get_width(page);
    cord_t obj_h = lv_obj_get_height(page);

    /*Always let 'scrollbar width' padding above, under, left and right to the scrollbars
     * else:
     * - horizontal and vertical scrollbars can overlap on the corners
     * - if the page has radius the scrollbar can be out of the radius  */
    cord_t sb_hor_pad = MATH_MAX(ext->sb.style->body.padding.inner, style->body.padding.hor);
    cord_t sb_ver_pad = MATH_MAX(ext->sb.style->body.padding.inner, style->body.padding.ver);

    if(ext->sb.mode == LV_PAGE_SB_MODE_OFF) return;

    if(ext->sb.mode == LV_PAGE_SB_MODE_ON) {
        ext->sb.hor_draw = 1;
        ext->sb.ver_draw = 1;
    }

    /*Invalidate the current (old) scrollbar areas*/
    area_t sb_area_tmp;
    if(ext->sb.hor_draw != 0) {
        area_cpy(&sb_area_tmp, &ext->sb.hor_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x2;
        sb_area_tmp.y2 += page->coords.y2;
        lv_inv_area(&sb_area_tmp);
    }
    if(ext->sb.ver_draw != 0)  {
        area_cpy(&sb_area_tmp, &ext->sb.ver_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x2;
        sb_area_tmp.y2 += page->coords.y2;
        lv_inv_area(&sb_area_tmp);
    }


    if(ext->sb.mode == LV_PAGE_SB_MODE_DRAG && lv_indev_is_dragging(lv_indev_get_act()) == false) {
        ext->sb.hor_draw = 0;
        ext->sb.ver_draw = 0;
        return;

    }

    /*Horizontal scrollbar*/
    if(scrl_w <= obj_w - 2 * hpad) {        /*Full sized scroll bar*/
        area_set_width(&ext->sb.hor_area, obj_w - 2 * sb_hor_pad);
        area_set_pos(&ext->sb.hor_area, sb_hor_pad, obj_h - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.ver);
        if(ext->sb.mode == LV_PAGE_SB_MODE_AUTO || ext->sb.mode == LV_PAGE_SB_MODE_DRAG)  ext->sb.hor_draw = 0;
    } else {
        size_tmp = (obj_w * (obj_w - (2 * sb_hor_pad))) / (scrl_w + 2 * hpad);
        if(size_tmp < LV_PAGE_SB_MIN_SIZE) size_tmp = LV_PAGE_SB_MIN_SIZE;
        area_set_width(&ext->sb.hor_area,  size_tmp);

        area_set_pos(&ext->sb.hor_area, sb_hor_pad +
                   (-(lv_obj_get_x(scrl) - hpad) * (obj_w - size_tmp -  2 * sb_hor_pad)) /
                   (scrl_w + 2 * hpad - obj_w ),
                   obj_h - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.ver);

        if(ext->sb.mode == LV_PAGE_SB_MODE_AUTO || ext->sb.mode == LV_PAGE_SB_MODE_DRAG)  ext->sb.hor_draw = 1;
    }
    
    /*Vertical scrollbar*/
    if(scrl_h <= obj_h - 2 * vpad) {        /*Full sized scroll bar*/
        area_set_height(&ext->sb.ver_area,  obj_h - 2 * sb_ver_pad);
        area_set_pos(&ext->sb.ver_area, obj_w - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.hor, sb_ver_pad);
        if(ext->sb.mode == LV_PAGE_SB_MODE_AUTO || ext->sb.mode == LV_PAGE_SB_MODE_DRAG)  ext->sb.ver_draw = 0;
    } else {
        size_tmp = (obj_h * (obj_h - (2 * sb_ver_pad))) / (scrl_h + 2 * vpad);
        if(size_tmp < LV_PAGE_SB_MIN_SIZE) size_tmp = LV_PAGE_SB_MIN_SIZE;
        area_set_height(&ext->sb.ver_area,  size_tmp);

        area_set_pos(&ext->sb.ver_area,  obj_w - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.hor,
        		    sb_ver_pad +
                   (-(lv_obj_get_y(scrl) - vpad) * (obj_h - size_tmp -  2 * sb_ver_pad)) /
                                      (scrl_h + 2 * vpad - obj_h ));

        if(ext->sb.mode == LV_PAGE_SB_MODE_AUTO || ext->sb.mode == LV_PAGE_SB_MODE_DRAG)  ext->sb.ver_draw = 1;
    }

    /*Invalidate the new scrollbar areas*/
    if(ext->sb.hor_draw != 0) {
        area_cpy(&sb_area_tmp, &ext->sb.hor_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x2;
        sb_area_tmp.y2 += page->coords.y2;
        lv_inv_area(&sb_area_tmp);
    }
    if(ext->sb.ver_draw != 0)  {
        area_cpy(&sb_area_tmp, &ext->sb.ver_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x2;
        sb_area_tmp.y2 += page->coords.y2;
        lv_inv_area(&sb_area_tmp);
    }
}

#endif
