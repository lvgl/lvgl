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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_page_sb_refresh(lv_obj_t* main_dp);
static bool lv_page_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);
static bool lv_scrolling_signal(lv_obj_t* obj_dp, lv_signal_t sign, void* param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_f_t ancestor_design_f;

static lv_pages_t lv_pages_def =
{
	.bg_rects.objs.color = COLOR_MAKE(0x50, 0x70, 0x90),
	.bg_rects.gcolor = COLOR_MAKE(0x70, 0xA0, 0xC0),
	.bg_rects.bcolor = COLOR_WHITE,
	.bg_rects.bopa = 50,
	.bg_rects.bwidth = 2 * LV_STYLE_MULT,
	.bg_rects.round = 4 * LV_STYLE_MULT,
	.bg_rects.empty = 0,
	.bg_rects.hpad = 10 * LV_STYLE_MULT,
	.bg_rects.vpad = 10 * LV_STYLE_MULT,
	.bg_rects.opad = 5 * LV_STYLE_MULT,

	.scrable_rects.objs.color = COLOR_WHITE,
	.scrable_rects.gcolor = COLOR_SILVER,
	.scrable_rects.bcolor = COLOR_GRAY,
	.scrable_rects.bopa = 50,
	.scrable_rects.bwidth = 0 * LV_STYLE_MULT,
	.scrable_rects.round = 2 * LV_STYLE_MULT,
	.scrable_rects.empty = 0,
	.scrable_rects.hpad = 10 * LV_STYLE_MULT,
	.scrable_rects.vpad = 10 * LV_STYLE_MULT,
	.scrable_rects.opad = 5 * LV_STYLE_MULT,

	.sb_rects.objs.color = COLOR_BLACK,
	.sb_rects.gcolor = COLOR_BLACK,
	.sb_rects.bcolor = COLOR_WHITE,
	.sb_rects.bopa = 50,
	.sb_rects.bwidth = 1 * LV_STYLE_MULT,
	.sb_rects.round = 5 * LV_STYLE_MULT,
	.sb_rects.empty = 0,
	.sb_rects.hpad = 0,
	.sb_rects.vpad = 0,
	.sb_rects.opad = 0,

	.sb_width= 8 * LV_STYLE_MULT,
	.sb_opa=50,
	.sb_mode = LV_PAGE_SB_MODE_AUTO,

};

static lv_pages_t lv_pages_transp =
{
	.bg_rects.objs.color = COLOR_MAKE(0x50, 0x70, 0x90),
	.bg_rects.gcolor = COLOR_MAKE(0x70, 0xA0, 0xC0),
	.bg_rects.bcolor = COLOR_WHITE,
	.bg_rects.bopa = 50,
	.bg_rects.bwidth = 0 * LV_STYLE_MULT,
	.bg_rects.round = 2 * LV_STYLE_MULT,
	.bg_rects.empty = 0,
	.bg_rects.hpad = 10 * LV_STYLE_MULT,
	.bg_rects.vpad = 10 * LV_STYLE_MULT,
	.bg_rects.opad = 5 * LV_STYLE_MULT,

	.scrable_rects.objs.transp = 1,
	.scrable_rects.empty = 1,
	.scrable_rects.bwidth = 0,
	.scrable_rects.hpad = 10 * LV_STYLE_MULT,
	.scrable_rects.vpad = 10 * LV_STYLE_MULT,
	.scrable_rects.vpad = 10 * LV_STYLE_MULT,

	.sb_rects.objs.color = COLOR_BLACK,
	.sb_rects.gcolor = COLOR_BLACK,
	.sb_rects.bcolor = COLOR_WHITE,
	.sb_rects.bopa = 0,
	.sb_rects.bwidth = 1 * LV_STYLE_MULT,
	.sb_rects.round = 5 * LV_STYLE_MULT,
	.sb_rects.empty = 0,
	.sb_rects.hpad = 0,
	.sb_rects.vpad = 0,
	.sb_rects.opad = 0,

	.sb_width = 8 * LV_STYLE_MULT,
	.sb_opa = 50,
	.sb_mode = LV_PAGE_SB_MODE_AUTO,

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
 * Create a page objects
 * @param par_dp pointer to an object, it will be the parent of the new page
 * @param copy_dp pointer to a page object, if not NULL then the new object will be copied from it
 * @return pointer to the created page
 */
lv_obj_t* lv_page_create(lv_obj_t * par_dp, lv_obj_t * copy_dp)
{
    /*Create the ancestor object*/
    lv_obj_t * new_obj_dp = lv_rect_create(par_dp, copy_dp);
    dm_assert(new_obj_dp);

    /*Allocate the object type specific extended data*/
    lv_page_ext_t * ext_dp = lv_obj_alloc_ext(new_obj_dp, sizeof(lv_page_ext_t));
    dm_assert(ext_dp);

    if(ancestor_design_f == NULL) {
    	ancestor_design_f = lv_obj_get_design_f(new_obj_dp);
    }

    /*Init the new page object*/
    if(copy_dp == NULL) {
	    ext_dp->scrolling_dp = lv_rect_create(new_obj_dp, NULL);
	    lv_obj_set_signal_f(ext_dp->scrolling_dp, lv_scrolling_signal);
		lv_obj_set_drag(ext_dp->scrolling_dp, true);
		lv_obj_set_drag_throw(ext_dp->scrolling_dp, true);
		lv_rect_set_fit(ext_dp->scrolling_dp, true, true);
		lv_obj_set_style(ext_dp->scrolling_dp, &lv_pages_def.scrable_rects);

		/* Add the signal function only if 'scrolling_dp' is created
		 * because everything has to be ready before any signal is received*/
	    lv_obj_set_signal_f(new_obj_dp, lv_page_signal);
	    lv_obj_set_design_f(new_obj_dp, lv_page_design);
		lv_obj_set_style(new_obj_dp, &lv_pages_def);
    } else {
    	lv_page_ext_t * copy_ext_dp = lv_obj_get_ext(copy_dp);
    	ext_dp->scrolling_dp = lv_rect_create(new_obj_dp, copy_ext_dp->scrolling_dp);
	    lv_obj_set_signal_f(ext_dp->scrolling_dp, lv_scrolling_signal);

		/* Add the signal function only if 'scrolling_dp' is created
		 * because everything has to be ready before any signal is received*/
	    lv_obj_set_signal_f(new_obj_dp, lv_page_signal);
	    lv_obj_set_design_f(new_obj_dp, lv_page_design);
		lv_obj_set_style(new_obj_dp, lv_obj_get_style(copy_dp));
    }
    
    lv_page_sb_refresh(new_obj_dp);
                
    return new_obj_dp;
}


/**
 * Signal function of the page
 * @param obj_dp pointer to a page object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_page_signal(lv_obj_t* obj_dp, lv_signal_t sign, void* param)
{
    bool obj_valid = true;

    /* Include the ancient signal function */
    obj_valid = lv_rect_signal(obj_dp, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(obj_valid != false) {
        lv_page_ext_t * page_ext_p = lv_obj_get_ext(obj_dp);
        lv_pages_t * pages_p = lv_obj_get_style(obj_dp);
        lv_obj_t * child;
        switch(sign) {
        	case LV_SIGNAL_CHILD_CHG: /*Be sure, only scrollable object is on the page*/
        		child = lv_obj_get_child(obj_dp, NULL);
        		while(child != NULL) {
        			if(child != page_ext_p->scrolling_dp) {
        				lv_obj_t * tmp = child;
            			child = lv_obj_get_child(obj_dp, child); /*Get the next child before move this*/
        				lv_obj_set_parent(tmp, page_ext_p->scrolling_dp);
        			} else {
            			child = lv_obj_get_child(obj_dp, child);
        			}
        		}
        		break;
                
            case LV_SIGNAL_STYLE_CHG:
            	area_set_height(&page_ext_p->sbh, pages_p->sb_width);
            	area_set_width(&page_ext_p->sbv, pages_p->sb_width);
            	lv_obj_set_style(page_ext_p->scrolling_dp, &pages_p->scrable_rects);

            	if(pages_p->sb_mode == LV_PAGE_SB_MODE_ON) {
            		page_ext_p->sbh_draw = 1;
            		page_ext_p->sbv_draw = 1;
            	} else {
            		page_ext_p->sbh_draw = 0;
					page_ext_p->sbv_draw = 0;
				}

            	lv_page_sb_refresh(obj_dp);
            	break;

            case LV_SIGNAL_CORD_CHG:
            	lv_page_sb_refresh(obj_dp);
            	break;
            default:
                break;
            
        }
    }
    
    return obj_valid;
}

/**
 * Signal function of the scrollable part of a page
 * @param obj_dp pointer to the scrollable object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
static bool lv_scrolling_signal(lv_obj_t* obj_dp, lv_signal_t sign, void* param)
{
    bool obj_valid = true;

    /* Include the ancient signal function */
    obj_valid = lv_rect_signal(obj_dp, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(obj_valid != false) {

        cord_t new_x;
        cord_t new_y;
        bool refr_x = false;
        bool refr_y = false;
        area_t page_cords;
        area_t obj_cords;
        lv_obj_t * page_p = lv_obj_get_parent(obj_dp);
        lv_pages_t * pages_p = lv_obj_get_style(page_p);
        lv_page_ext_t * page_ext_dp = lv_obj_get_ext(page_p);
        cord_t hpad = pages_p->bg_rects.hpad;
        cord_t vpad = pages_p->bg_rects.vpad;

        switch(sign) {
            case LV_SIGNAL_CORD_CHG:
                new_x = lv_obj_get_x(obj_dp);
                new_y = lv_obj_get_y(obj_dp);
                lv_obj_get_cords(obj_dp, &obj_cords);
                lv_obj_get_cords(page_p, &page_cords);

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
                    lv_obj_set_pos(obj_dp, new_x, new_y);
                }

                lv_page_sb_refresh(page_p);
                break;

            case LV_SIGNAL_DRAG_BEGIN:
            	if(pages_p->sb_mode == LV_PAGE_SB_MODE_AUTO ) {
					if(area_get_height(&page_ext_dp->sbv) < lv_obj_get_height(obj_dp) - pages_p->sb_width) {
						page_ext_dp->sbv_draw = 1;
						lv_inv_area(&page_ext_dp->sbv);
					}
					if(area_get_width(&page_ext_dp->sbh) < lv_obj_get_width(obj_dp) - pages_p->sb_width) {
						page_ext_dp->sbh_draw = 1;
						lv_inv_area(&page_ext_dp->sbh);
					}
            	}
                break;

            case LV_SIGNAL_DRAG_END:
            	if(pages_p->sb_mode == LV_PAGE_SB_MODE_AUTO) {
					page_ext_dp->sbh_draw = 0;
					page_ext_dp->sbv_draw = 0;
					lv_inv_area(&page_ext_dp->sbh);
					lv_inv_area(&page_ext_dp->sbv);
            	}
                break;
            default:
                break;

        }
    }

    return obj_valid;
}

/**
 * Glue the object to the page. After it the page can be moved (dragged) with this object too.
 * @param obj_dp pointer to an object on a page
 * @param en true: enable glue, false: disable glue
 */
void lv_page_glue_obj(lv_obj_t* obj_dp, bool en)
{
    lv_obj_set_drag_parent(obj_dp, en);
    lv_obj_set_drag(obj_dp, en);
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_pages_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_pages_t style
 */
lv_pages_t * lv_pages_get(lv_pages_builtin_t style, lv_pages_t * to_copy)
{
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

	if(to_copy != NULL) {
		if(style_p != NULL) memcpy(to_copy, style_p, sizeof(lv_pages_t));
		else memcpy(to_copy, &lv_pages_def, sizeof(lv_pages_t));
	}

	return style_p;
}




/*=====================
 * Setter functions 
 *====================*/

/*=====================
 * Getter functions 
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the pages
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_page_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	return ancestor_design_f(obj_dp, mask_p, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		ancestor_design_f(obj_dp, mask_p, mode);
	} else if(mode == LV_DESIGN_DRAW_POST) { /*Draw the scroll bars finally*/
		lv_page_ext_t * page_ext_dp = lv_obj_get_ext(obj_dp);
		lv_pages_t * pages_p = lv_obj_get_style(obj_dp);
		opa_t sb_opa = lv_obj_get_opa(obj_dp) * pages_p->sb_opa /100;

		/*Draw the scrollbars*/
		if(page_ext_dp->sbh_draw != 0) {
			lv_draw_rect(&page_ext_dp->sbh, mask_p, &pages_p->sb_rects, sb_opa);
		}

		if(page_ext_dp->sbv_draw != 0) {
			lv_draw_rect(&page_ext_dp->sbv, mask_p, &pages_p->sb_rects, sb_opa);
		}
	}

	return true;
}

/**
 * Handle the drawing related tasks of the pages
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'
 */
static void lv_page_sb_refresh(lv_obj_t* page_dp)
{
    lv_page_ext_t * page_ext_dp = lv_obj_get_ext(page_dp);
    lv_pages_t * pages_p = lv_obj_get_style(page_dp);
    lv_obj_t* scrolling_dp = page_ext_dp->scrolling_dp;
    cord_t size_tmp;
    cord_t scrolling_w = lv_obj_get_width(scrolling_dp);
    cord_t scrolling_h =  lv_obj_get_height(scrolling_dp);
    cord_t hpad = pages_p->bg_rects.hpad;
    cord_t vpad = pages_p->bg_rects.vpad;
    cord_t obj_w = lv_obj_get_width(page_dp);
    cord_t obj_h = lv_obj_get_height(page_dp);
    cord_t page_x0 = page_dp->cords.x1;
    cord_t page_y0 = page_dp->cords.y1;

    lv_inv_area(&page_ext_dp->sbh);
    lv_inv_area(&page_ext_dp->sbv);

    /*Horizontal scrollbar*/
    if(scrolling_w <= obj_w) {        /*Full sized scroll bar*/
        area_set_width(&page_ext_dp->sbh, obj_w - pages_p->sb_width);
        area_set_pos(&page_ext_dp->sbh, page_x0, page_y0 + obj_h - pages_p->sb_width);
    } else {
    	if(pages_p->sb_mode == LV_PAGE_SB_MODE_ON) {
    		page_ext_dp->sbh_draw = 1;
    	}
        size_tmp = (((obj_w - hpad) * (obj_w - pages_p->sb_width)) / scrolling_w);
        area_set_width(&page_ext_dp->sbh,  size_tmp);

        area_set_pos(&page_ext_dp->sbh, page_x0 +
                ( -(lv_obj_get_x(scrolling_dp) - hpad) * (obj_w - size_tmp - pages_p->sb_width)) /
                                      (scrolling_w - obj_w + 2 * hpad),
                                       page_y0 + obj_h - pages_p->sb_width);
    }
    
    /*Vertical scrollbar*/
    if(scrolling_h <= obj_h) {        /*Full sized scroll bar*/
        area_set_height(&page_ext_dp->sbv,  obj_h - pages_p->sb_width);
        area_set_pos(&page_ext_dp->sbv, page_x0 + obj_w - pages_p->sb_width, 0);
    } else {
    	if(pages_p->sb_mode == LV_PAGE_SB_MODE_ON) {
    		page_ext_dp->sbv_draw = 1;
    	}
        size_tmp = (((obj_h - vpad) * (obj_h - pages_p->sb_width)) / scrolling_h);
        area_set_height(&page_ext_dp->sbv,  size_tmp);

        area_set_pos(&page_ext_dp->sbv, page_x0 + obj_w - pages_p->sb_width,
        		    page_y0 +
                   (-(lv_obj_get_y(scrolling_dp) - vpad) * (obj_h - size_tmp - pages_p->sb_width)) /
                                      (scrolling_h - obj_h + 2 * vpad));
    }
    
    lv_inv_area(&page_ext_dp->sbh);
    lv_inv_area(&page_ext_dp->sbv);


}
#endif
