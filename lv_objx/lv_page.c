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
#include "../lv_objx/lv_page.h"
#include "../lv_objx/lv_rect.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_obj/lv_refr.h"
#include "../lv_misc/anim.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_page_sb_refresh(lv_obj_t * main);
static bool lv_page_design(lv_obj_t * page, const area_t * mask, lv_design_mode_t mode);
static bool lv_scrolling_signal(lv_obj_t * page, lv_signal_t sign, void* param);
static void lv_pages_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_pages_t lv_pages_def;
static lv_pages_t lv_pages_transp;
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
 * Create a page objects
 * @param par pointer to an object, it will be the parent of the new page
 * @param copy pointer to a page object, if not NULL then the new object will be copied from it
 * @return pointer to the created page
 */
lv_obj_t * lv_page_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor object*/
    lv_obj_t * new_page = lv_rect_create(par, copy);
    dm_assert(new_page);

    /*Allocate the object type specific extended data*/
    lv_page_ext_t * ext = lv_obj_alloc_ext(new_page, sizeof(lv_page_ext_t));
    dm_assert(ext);

    if(ancestor_design_f == NULL) {
    	ancestor_design_f = lv_obj_get_design_f(new_page);
    }

    /*Init the new page object*/
    if(copy == NULL) {
    	lv_pages_t * pages = lv_pages_get(LV_PAGES_DEF, NULL);
	    ext->scrolling = lv_rect_create(new_page, NULL);
	    lv_obj_set_signal_f(ext->scrolling, lv_scrolling_signal);
		lv_obj_set_drag(ext->scrolling, true);
		lv_obj_set_drag_throw(ext->scrolling, true);
		lv_rect_set_fit(ext->scrolling, true, true);
		lv_obj_set_style(ext->scrolling, &pages->scrable_rects);

		/* Add the signal function only if 'scrolling' is created
		 * because everything has to be ready before any signal is received*/
	    lv_obj_set_signal_f(new_page, lv_page_signal);
	    lv_obj_set_design_f(new_page, lv_page_design);
		lv_obj_set_style(new_page, pages);
    } else {
    	lv_page_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->scrolling = lv_rect_create(new_page, copy_ext->scrolling);
	    lv_obj_set_signal_f(ext->scrolling, lv_scrolling_signal);

		/* Add the signal function only if 'scrolling' is created
		 * because everything has to be ready before any signal is received*/
	    lv_obj_set_signal_f(new_page, lv_page_signal);
	    lv_obj_set_design_f(new_page, lv_page_design);
		lv_obj_set_style(new_page, lv_obj_get_style(copy));
    }
    
    lv_page_sb_refresh(new_page);
                
    return new_page;
}


/**
 * Signal function of the page
 * @param page pointer to a page object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_page_signal(lv_obj_t * page, lv_signal_t sign, void * param)
{
    bool obj_valid = true;

    /* Include the ancient signal function */
    obj_valid = lv_rect_signal(page, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(obj_valid != false) {
        lv_page_ext_t * ext = lv_obj_get_ext(page);
        lv_pages_t * pages = lv_obj_get_style(page);
        lv_obj_t * child;
        switch(sign) {
        	case LV_SIGNAL_CHILD_CHG: /*Be sure, only scrollable object is on the page*/
        		child = lv_obj_get_child(page, NULL);
        		while(child != NULL) {
        			if(child != ext->scrolling) {
        				lv_obj_t * tmp = child;
            			child = lv_obj_get_child(page, child); /*Get the next child before move this*/
        				lv_obj_set_parent(tmp, ext->scrolling);
        			} else {
            			child = lv_obj_get_child(page, child);
        			}
        		}
        		break;
                
            case LV_SIGNAL_STYLE_CHG:
            	area_set_height(&ext->sbh, pages->sb_width);
            	area_set_width(&ext->sbv, pages->sb_width);
            	lv_obj_set_style(ext->scrolling, &pages->scrable_rects);

            	if(pages->sb_mode == LV_PAGE_SB_MODE_ON) {
            		ext->sbh_draw = 1;
            		ext->sbv_draw = 1;
            	} else {
            		ext->sbh_draw = 0;
					ext->sbv_draw = 0;
				}

            	lv_page_sb_refresh(page);
            	break;

            case LV_SIGNAL_CORD_CHG:
            	if(ext->scrolling != NULL) {
            		ext->scrolling->signal_f(ext->scrolling, LV_SIGNAL_CORD_CHG, &ext->scrolling->cords);
            		lv_page_sb_refresh(page);
            	}
            	break;
            default:
                break;
            
        }
    }
    
    return obj_valid;
}

/**
 * Signal function of the scrollable part of a page
 * @param scrolling pointer to the scrollable object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
static bool lv_scrolling_signal(lv_obj_t * scrolling, lv_signal_t sign, void* param)
{
    bool obj_valid = true;

    /* Include the ancient signal function */
    obj_valid = lv_rect_signal(scrolling, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(obj_valid != false) {

        cord_t new_x;
        cord_t new_y;
        bool refr_x = false;
        bool refr_y = false;
        area_t page_cords;
        area_t obj_cords;
        lv_obj_t * page = lv_obj_get_parent(scrolling);
        lv_pages_t * pages = lv_obj_get_style(page);
        lv_page_ext_t * page_ext = lv_obj_get_ext(page);
        cord_t hpad = pages->bg_rects.hpad;
        cord_t vpad = pages->bg_rects.vpad;

        switch(sign) {
            case LV_SIGNAL_CORD_CHG:
                new_x = lv_obj_get_x(scrolling);
                new_y = lv_obj_get_y(scrolling);
                lv_obj_get_cords(scrolling, &obj_cords);
                lv_obj_get_cords(page, &page_cords);

                /*scrollable width smaller then page width? -> align to left*/
                if(area_get_width(&obj_cords) + 2 * hpad < area_get_width(&page_cords)) {
                    if(obj_cords.x1 != page_cords.x1 + hpad) {
                        new_x = hpad;
                        refr_x = true;
                    }
                } else {
                	/*The edges of the scrollable can not be in the page (minus hpad) */
                    if(obj_cords.x2  < page_cords.x2 - hpad) {
                       new_x =  area_get_width(&page_cords) - area_get_width(&obj_cords) - hpad;   /* Right align */
                       refr_x = true;
                    }
                    if (obj_cords.x1 > page_cords.x1 + hpad) {
                        new_x = hpad;  /*Left align*/
                        refr_x = true;
                    }
                }

                /*scrollable height smaller then page height? -> align to left*/
                if(area_get_height(&obj_cords) + 2 * vpad < area_get_height(&page_cords)) {
                    if(obj_cords.y1 != page_cords.y1 + vpad) {
                        new_y = vpad;
                        refr_y = true;
                    }
                } else {
                	/*The edges of the scrollable can not be in the page (minus vpad) */
                    if(obj_cords.y2 < page_cords.y2 - vpad) {
                       new_y =  area_get_height(&page_cords) - area_get_height(&obj_cords) - vpad;   /* Bottom align */
                       refr_y = true;
                    }
                    if (obj_cords.y1  > page_cords.y1 + vpad) {
                        new_y = vpad;  /*Top align*/
                        refr_y = true;
                    }
                }
                if(refr_x != false || refr_y != false) {
                    lv_obj_set_pos(scrolling, new_x, new_y);
                }

                lv_page_sb_refresh(page);
                break;

            case LV_SIGNAL_DRAG_BEGIN:
            	if(pages->sb_mode == LV_PAGE_SB_MODE_AUTO ) {
					if(area_get_height(&page_ext->sbv) < lv_obj_get_height(scrolling) - pages->sb_width) {
						page_ext->sbv_draw = 1;
						lv_inv_area(&page_ext->sbv);
					}
					if(area_get_width(&page_ext->sbh) < lv_obj_get_width(scrolling) - pages->sb_width) {
						page_ext->sbh_draw = 1;
						lv_inv_area(&page_ext->sbh);
					}
            	}
                break;

            case LV_SIGNAL_DRAG_END:
            	if(pages->sb_mode == LV_PAGE_SB_MODE_AUTO) {
					page_ext->sbh_draw = 0;
					page_ext->sbv_draw = 0;
					lv_inv_area(&page_ext->sbh);
					lv_inv_area(&page_ext->sbv);
            	}
                break;
            case LV_SIGNAL_PRESSED:
            	if(page_ext->pr_action != NULL) {
            		page_ext->pr_action(page, param);
            	}
            	break;
            case LV_SIGNAL_RELEASED:
            	if(lv_dispi_is_dragging(param) == false) {
					if(page_ext->rel_action != NULL) {
						page_ext->rel_action(page, param);
					}
            	}
            	break;
            default:
                break;

        }
    }

    return obj_valid;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a release action for the page
 * @param page pointer to a page object
 * @param rel_action a function to call when the page is released
 */
void lv_page_set_rel_action(lv_obj_t * page, lv_action_t rel_action)
{
	lv_page_ext_t * ext = lv_obj_get_ext(page);
	ext->rel_action = rel_action;
}

/**
 * Set a press action for the page
 * @param page pointer to a page object
 * @param pr_action a function to call when the page is pressed
 */
void lv_page_set_pr_action(lv_obj_t * page, lv_action_t pr_action)
{
	lv_page_ext_t * ext = lv_obj_get_ext(page);
	ext->pr_action = pr_action;
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
 * @param anim_en true: scroll with animation
 */
void lv_page_focus(lv_obj_t * page, lv_obj_t * obj, bool anim_en)
{

	lv_page_ext_t * ext = lv_obj_get_ext(page);
	lv_pages_t * style = lv_obj_get_style(page);

	cord_t obj_y = lv_obj_get_y(obj);
	cord_t obj_h = lv_obj_get_height(obj);
	cord_t scrlable_y = lv_obj_get_y(ext->scrolling);
	cord_t page_h = lv_obj_get_height(page);

	bool refr = false;

	cord_t top_err = -(scrlable_y + obj_y);
	cord_t bot_err = scrlable_y + obj_y + obj_h - page_h;

	/*If obj is higher then the page focus where the "error" is smaller*/
	/*Out of the page on the top*/
	if((obj_h <= page_h && top_err > 0) ||
	   (obj_h > page_h && top_err >= bot_err)) {
		/*Calculate a new position and to let  scrable_rects.vpad space above*/
		scrlable_y = -(obj_y - style->scrable_rects.vpad - style->bg_rects.vpad);
		scrlable_y += style->scrable_rects.vpad;
		refr = true;
	}
	/*Out of the page on the bottom*/
	else if((obj_h <= page_h && bot_err > 0) ||
			(obj_h > page_h && top_err < bot_err)) {
        /*Calculate a new position and to let  scrable_rects.vpad space below*/
		scrlable_y = -obj_y;
		scrlable_y += page_h - obj_h;
        scrlable_y -= style->scrable_rects.vpad;
		refr = true;
	}

	if(refr != false) {
#if LV_PAGE_ANIM_FOCUS_TIME == 0
		lv_obj_set_y(ext->scrolling, scrlable_y);
#else
		if(anim_en == false) {
			lv_obj_set_y(ext->scrolling, scrlable_y);
		} else {
			anim_t a;
			a.act_time = 0;
			a.start = lv_obj_get_y(ext->scrolling);
			a.end = scrlable_y;
			a.time = LV_PAGE_ANIM_FOCUS_TIME;//anim_speed_to_time(LV_PAGE_ANIM_SPEED, a.start, a.end);
			a.end_cb = NULL;
			a.playback = 0;
			a.repeat = 0;
			a.var = ext->scrolling;
			a.path = anim_get_path(ANIM_PATH_LIN);

			a.fp = (anim_fp_t) lv_obj_set_y;
			anim_create(&a);
		}
	}
#endif
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the scrollable object of a page-
 * @param page pointer to page object
 * @return pointer to rectangle which is the scrollable part of the page
 */
lv_obj_t * lv_page_get_scrable(lv_obj_t * page)
{
	lv_page_ext_t * ext = lv_obj_get_ext(page);

	return ext->scrolling;
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_pages_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_pages_t style
 */
lv_pages_t * lv_pages_get(lv_pages_builtin_t style, lv_pages_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_pages_init();
		style_inited = true;
	}

	lv_pages_t * style_p;

	switch(style) {
		case LV_PAGES_DEF:
			style_p = &lv_pages_def;
			break;
		case LV_PAGES_TRANSP:
			style_p = &lv_pages_transp;
			break;
		default:
			style_p = &lv_pages_def;
	}

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_pages_t));
		else memcpy(copy, &lv_pages_def, sizeof(lv_pages_t));
	}

	return style_p;
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
static bool lv_page_design(lv_obj_t * page, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	return ancestor_design_f(page, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		ancestor_design_f(page, mask, mode);
	} else if(mode == LV_DESIGN_DRAW_POST) { /*Draw the scroll bars finally*/
		ancestor_design_f(page, mask, mode);

		lv_page_ext_t * ext = lv_obj_get_ext(page);
		lv_pages_t * style = lv_obj_get_style(page);
		opa_t sb_opa = lv_obj_get_opa(page) * style->sb_opa /100;

		/*Draw the scrollbars*/
		if(ext->sbh_draw != 0) {
			lv_draw_rect(&ext->sbh, mask, &style->sb_rects, sb_opa);
		}

		if(ext->sbv_draw != 0) {
			lv_draw_rect(&ext->sbv, mask, &style->sb_rects, sb_opa);
		}
	}

	return true;
}

/**
 * Refresh the position and size of the scroll bars.
 * @param page pointer to a page object
 */
static void lv_page_sb_refresh(lv_obj_t * page)
{


    lv_page_ext_t * page_ext = lv_obj_get_ext(page);
    lv_pages_t * pages = lv_obj_get_style(page);
    lv_obj_t * scrolling = page_ext->scrolling;
    cord_t size_tmp;
    cord_t scrolling_w = lv_obj_get_width(scrolling);
    cord_t scrolling_h =  lv_obj_get_height(scrolling);
    cord_t hpad = pages->bg_rects.hpad;
    cord_t vpad = pages->bg_rects.vpad;
    cord_t obj_w = lv_obj_get_width(page);
    cord_t obj_h = lv_obj_get_height(page);
    cord_t page_x0 = page->cords.x1;
    cord_t page_y0 = page->cords.y1;


    if(pages->sb_mode == LV_PAGE_SB_MODE_OFF) return;

    /*Invalidate the current (old) scrollbar areas*/
    if(page_ext->sbh_draw != 0) lv_inv_area(&page_ext->sbh);
    if(page_ext->sbv_draw != 0) lv_inv_area(&page_ext->sbv);

    /*Horizontal scrollbar*/
    if(scrolling_w <= obj_w - 2 * hpad) {        /*Full sized scroll bar*/
        area_set_width(&page_ext->sbh, obj_w - pages->sb_width);
        area_set_pos(&page_ext->sbh, page_x0, page_y0 + obj_h - pages->sb_width);
    } else {
    	if(pages->sb_mode == LV_PAGE_SB_MODE_ON) {
    		page_ext->sbh_draw = 1;
    	}
        size_tmp = (((obj_w - hpad) * (obj_w - pages->sb_width)) / scrolling_w);
        area_set_width(&page_ext->sbh,  size_tmp);

        area_set_pos(&page_ext->sbh, page_x0 +
                ( -(lv_obj_get_x(scrolling) - hpad) * (obj_w - size_tmp - pages->sb_width)) /
                                      (scrolling_w - obj_w + 2 * hpad),
                                       page_y0 + obj_h - pages->sb_width);
    }
    
    /*Vertical scrollbar*/
    if(scrolling_h <= obj_h - 2 * vpad) {        /*Full sized scroll bar*/
        area_set_height(&page_ext->sbv,  obj_h - pages->sb_width);
        area_set_pos(&page_ext->sbv, page_x0 + obj_w - pages->sb_width, 0);
    } else {
    	if(pages->sb_mode == LV_PAGE_SB_MODE_ON) {
    		page_ext->sbv_draw = 1;
    	}
        size_tmp = (((obj_h - vpad) * (obj_h - pages->sb_width)) / scrolling_h);
        area_set_height(&page_ext->sbv,  size_tmp);

        area_set_pos(&page_ext->sbv, page_x0 + obj_w - pages->sb_width,
        		    page_y0 +
                   (-(lv_obj_get_y(scrolling) - vpad) * (obj_h - size_tmp - pages->sb_width)) /
                                      (scrolling_h - obj_h + 2 * vpad));
    }

    /*Invalidate the new scrollbar areas*/
    if(page_ext->sbh_draw != 0) lv_inv_area(&page_ext->sbh);
    if(page_ext->sbv_draw != 0) lv_inv_area(&page_ext->sbv);
}

/**
 * Initialize the page styles
 */
static void lv_pages_init(void)
{
	/*Default style*/
	lv_rects_get(LV_RECTS_DEF, &lv_pages_def.bg_rects);

	lv_rects_get(LV_RECTS_DEF, &lv_pages_def.scrable_rects);
	lv_pages_def.scrable_rects.objs.color = COLOR_WHITE;
	lv_pages_def.scrable_rects.gcolor = COLOR_SILVER;
	lv_pages_def.scrable_rects.bcolor = COLOR_GRAY;

	lv_rects_get(LV_RECTS_DEF, &lv_pages_def.sb_rects);
	lv_pages_def.sb_rects.objs.color = COLOR_BLACK;
	lv_pages_def.sb_rects.gcolor = COLOR_BLACK;
	lv_pages_def.sb_rects.bcolor = COLOR_WHITE;
	lv_pages_def.sb_rects.bwidth = 1 * LV_DOWNSCALE;
	lv_pages_def.sb_rects.round = 5 * LV_DOWNSCALE;

	lv_pages_def.sb_width= 8 * LV_DOWNSCALE;
	lv_pages_def.sb_opa=50;
	lv_pages_def.sb_mode = LV_PAGE_SB_MODE_AUTO;

	/*Transparent style*/
	memcpy(&lv_pages_transp, &lv_pages_def, sizeof(lv_pages_t));
	lv_pages_transp.scrable_rects.objs.transp = 1;
	lv_pages_transp.scrable_rects.empty = 1;
	lv_pages_transp.scrable_rects.bwidth = 0;

}
#endif
