/**
 * @file lv_page.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_PAGE != 0

#include "../../misc/math/math_base.h"
#include "../lv_objx/lv_page.h"
#include "../lv_objx/lv_rect.h"


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

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_pages_t lv_pages_def =
{
	.bg_rects.mcolor = COLOR_WHITE,
	.bg_rects.gcolor = COLOR_SILVER,
	.bg_rects.bcolor = COLOR_GRAY,
	.bg_rects.bopa = 50,
	.bg_rects.bwidth = 0 * LV_STYLE_MULT,
	.bg_rects.round = 2 * LV_STYLE_MULT,
	.bg_rects.empty = 0,

	.sb_rects.mcolor = COLOR_BLACK,
	.sb_rects.gcolor = COLOR_BLACK,
	.sb_rects.bcolor = COLOR_WHITE,
	.sb_rects.bopa = 50,
	.sb_rects.bwidth = 1 * LV_STYLE_MULT,
	.sb_rects.round = 5 * LV_STYLE_MULT,
	.sb_rects.empty = 0,

	.sb_width= 8 * LV_STYLE_MULT,
	.sb_opa=50,
	.sb_mode = LV_PAGE_SB_MODE_ON,

	.margin_hor = 10 * LV_STYLE_MULT,
	.margin_ver = 10 * LV_STYLE_MULT,
	.padding_hor = 10 * LV_STYLE_MULT,
	.padding_ver = 10 * LV_STYLE_MULT,

};

static lv_pages_t lv_pages_paper =
{
	.bg_rects.mcolor = COLOR_WHITE,
	.bg_rects.gcolor = COLOR_WHITE,
	.bg_rects.bcolor = COLOR_GRAY,
	.bg_rects.bopa = 100,
	.bg_rects.bwidth = 2 * LV_STYLE_MULT,
	.bg_rects.round = 0 * LV_STYLE_MULT,
	.bg_rects.empty = 0,

	.sb_rects.mcolor = COLOR_BLACK,
	.sb_rects.gcolor = COLOR_BLACK,
	.sb_rects.bcolor = COLOR_SILVER,
	.sb_rects.bopa = 100,
	.sb_rects.bwidth = 1 * LV_STYLE_MULT,
	.sb_rects.round = 5 * LV_STYLE_MULT,
	.sb_rects.empty = 0,

	.sb_width = 10 * LV_STYLE_MULT,
	.sb_opa=50,
	.sb_mode = LV_PAGE_SB_MODE_ON,

	.margin_hor = 15 * LV_STYLE_MULT,
	.margin_ver = 15 * LV_STYLE_MULT,
	.padding_hor = 10 * LV_STYLE_MULT,
	.padding_ver = 10 * LV_STYLE_MULT,

};

static lv_pages_t lv_pages_transp =
{
	.bg_rects.empty = 1,

	.sb_rects.mcolor = COLOR_BLACK,
	.sb_rects.gcolor = COLOR_BLACK,
	.sb_rects.bcolor = COLOR_WHITE,
	.sb_rects.bopa = 0,
	.sb_rects.bwidth = 1 * LV_STYLE_MULT,
	.sb_rects.round = 5 * LV_STYLE_MULT,
	.sb_rects.empty = 0,

	.sb_width = 8 * LV_STYLE_MULT,
	.sb_opa = 50,
	.sb_mode = LV_PAGE_SB_MODE_AUTO,

	.margin_hor = 0 * LV_STYLE_MULT,
	.margin_ver = 0 * LV_STYLE_MULT,

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
 * @return pointer to the created page
 */
lv_obj_t* lv_page_create(lv_obj_t * par_dp, lv_obj_t * ori_dp)
{
    lv_obj_t* new_dp = NULL;
    
    /*Create a basic object. Page elements will be stored here*/
    new_dp = lv_rect_create(par_dp, ori_dp);
    
    /*Init the new object*/
    lv_page_t * page_p = lv_obj_alloc_ext(new_dp, sizeof(lv_page_t));

    /*Init the main rectangle if it is not copied*/
    if(ori_dp == NULL) {
		lv_obj_set_size(new_dp, 100,200);
		lv_obj_set_drag(new_dp, true);
		lv_obj_set_drag_throw(new_dp, true);
		lv_obj_set_style(new_dp, &lv_pages_def);
    } else {
		lv_obj_set_style(new_dp, lv_obj_get_style(ori_dp));
    }

    lv_pages_t * pages_p = lv_obj_get_style(new_dp);
    
    /*Create horizontal scroll bar*/
    page_p->sbh_dp = lv_rect_create(par_dp, NULL);
    lv_obj_set_height(page_p->sbh_dp, pages_p->sb_width);
    lv_obj_set_style(page_p->sbh_dp,  &pages_p->sb_rects);
    if(pages_p->sb_mode == LV_PAGE_SB_MODE_ON) {
    	lv_obj_set_opa(page_p->sbh_dp, (pages_p->sb_opa * OPA_COVER) / 100);
    } else {
    	lv_obj_set_opa(page_p->sbh_dp, OPA_TRANSP);
    }

    /*Create vertical scroll bar*/
    page_p->sbv_dp =lv_rect_create(par_dp, NULL);
    lv_obj_set_width(page_p->sbv_dp, pages_p->sb_width);
    lv_obj_set_style(page_p->sbv_dp, &pages_p->sb_rects);
    if(lv_pages_def.sb_mode == LV_PAGE_SB_MODE_ON) {
    	lv_obj_set_opa(page_p->sbv_dp, (pages_p->sb_opa * OPA_COVER) / 100);
    } else {
    	lv_obj_set_opa(page_p->sbv_dp, OPA_TRANSP);
    }

    lv_obj_set_signal_f(new_dp, lv_page_signal);
    
    lv_page_sb_refresh(new_dp);
                
    return new_dp;
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
        
        cord_t new_x;
        cord_t new_y;
        bool refr_x = false;
        bool refr_y = false;
        area_t page_cords;
        area_t par_cords;
        lv_obj_t * i;
        
        lv_obj_t * par_dp = lv_obj_get_parent(obj_dp);
        lv_page_t * page_ext_p = lv_obj_get_ext(obj_dp);
        lv_pages_t * pages_p = lv_obj_get_style(obj_dp);

        switch(sign) {
            case LV_SIGNAL_CORD_CHG:
                new_x = lv_obj_get_x(obj_dp);
                new_y = lv_obj_get_y(obj_dp);
                lv_obj_get_cords(par_dp, &par_cords);
                lv_obj_get_cords(obj_dp, &page_cords);

                /*page width smaller then parent width? -> align to left*/
                if(area_get_width(&page_cords) <= area_get_width(&par_cords) - pages_p->margin_hor * 2) {
                    if(page_cords.x1 - pages_p->margin_hor != par_cords.x1) {
                        new_x = pages_p->margin_hor;
                        refr_x = true;
                    }
                } else {
                    if(page_cords.x2 + pages_p->margin_hor < par_cords.x2) {
                       new_x =  area_get_width(&par_cords) - area_get_width(&page_cords) - pages_p->margin_hor;   /* Right align */
                       refr_x = true;
                    } 
                    if (page_cords.x1 - pages_p->margin_hor > par_cords.x1) {
                        new_x = pages_p->margin_hor;  /*Left align*/
                        refr_x = true;
                    }
                }
                /*Wrong in y?*/
                if(area_get_height(&page_cords) <= area_get_height(&par_cords) - pages_p->margin_ver * 2) {
                    if(page_cords.y1 - pages_p->margin_ver != par_cords.y1) {
                        new_y = pages_p->margin_ver;
                        refr_y = true;
                    }
                } else {
                    if(page_cords.y2 + pages_p->margin_ver < par_cords.y2) {
                       new_y =  area_get_height(&par_cords) - area_get_height(&page_cords) - pages_p->margin_ver;   /* Bottom align */
                       refr_y = true;
                    } 
                    if (page_cords.y1 - pages_p->margin_ver > par_cords.y1) {
                        new_y = pages_p->margin_ver;  /*Top align*/
                        refr_y = true;
                    }
                }      
                if(refr_x != false || refr_y != false) {
                    lv_obj_set_pos(obj_dp, new_x, new_y);
                }
                
                lv_page_sb_refresh(obj_dp);
                break;
                
            case LV_SIGNAL_DRAG_BEGIN:
            	if(pages_p->sb_mode == LV_PAGE_SB_MODE_AUTO ) {
					if(lv_obj_get_height(page_ext_p->sbv_dp) < lv_obj_get_height(par_dp) - pages_p->sb_width) {
						lv_obj_set_opa(page_ext_p->sbv_dp, (pages_p->sb_opa * OPA_COVER) / 100);
					}
					if(lv_obj_get_width(page_ext_p->sbh_dp) < lv_obj_get_width(par_dp) - pages_p->sb_width) {
						lv_obj_set_opa(page_ext_p->sbh_dp, (pages_p->sb_opa * OPA_COVER) / 100);
					}
            	}
                break;
                
            case LV_SIGNAL_DRAG_END:
            	if(pages_p->sb_mode == LV_PAGE_SB_MODE_AUTO) {
					lv_obj_set_opa(page_ext_p->sbh_dp, OPA_TRANSP);
					lv_obj_set_opa(page_ext_p->sbv_dp, OPA_TRANSP);
            	}
                break;
                
            case LV_SIGNAL_CHILD_CHG:
                page_cords.x1 = LV_CORD_MAX;
                page_cords.y1 = LV_CORD_MAX;
                page_cords.x2 = LV_CORD_MIN;
                page_cords.y2 = LV_CORD_MIN;
                
                LL_READ(obj_dp->child_ll, i) {
                    page_cords.x1 = min(page_cords.x1, i->cords.x1);
                    page_cords.y1 = min(page_cords.y1, i->cords.y1);
                    page_cords.x2 = max(page_cords.x2, i->cords.x2);
                    page_cords.y2 = max(page_cords.y2, i->cords.y2);
                }
                
                /*If the value is not the init value then the page has >=1 child.*/
                if(page_cords.x1 != LV_CORD_MAX) {
                	page_cords.x1 -= pages_p->padding_hor;
                	page_cords.x2 += pages_p->padding_hor;
                	page_cords.y1 -= pages_p->padding_ver;
                	page_cords.y2 += pages_p->padding_ver;
                    area_cpy(&obj_dp->cords, &page_cords);
                    
                    lv_obj_set_pos(obj_dp, lv_obj_get_x(obj_dp),
                                           lv_obj_get_y(obj_dp));
                } else {
                    lv_obj_set_size(obj_dp, 10, 10);
                }
                
                lv_obj_inv(par_dp);
                
                break;
            case LV_SIGNAL_STYLE_CHG:

            	/* Set the styles only if they are different else infinite loop
            	 * will be created from lv_obj_set_style*/
            	if(lv_obj_get_style(page_ext_p->sbh_dp) != &pages_p->sb_rects) {
					lv_obj_set_style(page_ext_p->sbh_dp, &pages_p->sb_rects);
					lv_obj_set_style(page_ext_p->sbv_dp, &pages_p->sb_rects);
					lv_obj_set_style(obj_dp, &pages_p->bg_rects);
            	}
            	if(pages_p->sb_mode == LV_PAGE_SB_MODE_ON) {
                	lv_obj_set_opa(page_ext_p->sbv_dp, (pages_p->sb_opa * OPA_COVER) / 100);
                	lv_obj_set_opa(page_ext_p->sbh_dp, (pages_p->sb_opa * OPA_COVER) / 100);
                } else {
                	lv_obj_set_opa(page_ext_p->sbv_dp, OPA_TRANSP);
                	lv_obj_set_opa(page_ext_p->sbh_dp, OPA_TRANSP);
                }

            	lv_page_sb_refresh(obj_dp);
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
		case LV_PAGES_PAPER:
			style_p = &lv_pages_paper;
			break;
		case LV_PAGES_TRANSP:
			style_p = &lv_pages_transp;
			break;
		default:
			style_p = NULL;
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
 * @param return true/false, depends on 'mode'
 */
static void lv_page_sb_refresh(lv_obj_t* page_dp)
{
    lv_page_t * page_p = lv_obj_get_ext(page_dp);
    lv_pages_t * pages_p = lv_obj_get_style(page_dp);
    lv_obj_t* par_dp = lv_obj_get_parent(page_dp);
    cord_t size_tmp;
    cord_t page_w = lv_obj_get_width(page_dp) + 2 * pages_p->margin_hor;
    cord_t page_h =  lv_obj_get_height(page_dp) + 2 * pages_p->margin_ver;
    cord_t par_w = lv_obj_get_width(par_dp);
    cord_t par_h = lv_obj_get_height(par_dp);
            
    /*Horizontal scrollbar*/
    if(page_w <= par_w) {        /*Full sized scroll bar*/
        lv_obj_set_width(page_p->sbh_dp,  par_w - pages_p->sb_width);
        lv_obj_set_pos(page_p->sbh_dp, 0, par_h - pages_p->sb_width);
		lv_obj_set_opa(page_p->sbh_dp, OPA_TRANSP);
    } else {                       
    	if(pages_p->sb_mode == LV_PAGE_SB_MODE_ON) {
    		lv_obj_set_opa(page_p->sbh_dp, (pages_p->sb_opa * OPA_COVER) / 100);
    	}

        size_tmp = ((par_w * (par_w - pages_p->sb_width)) / page_w);
        lv_obj_set_width(page_p->sbh_dp,  size_tmp);

        lv_obj_set_pos(page_p->sbh_dp,
                ( -(lv_obj_get_x(page_dp) - pages_p->margin_hor) * (par_w - size_tmp - pages_p->sb_width)) /
                                      (page_w - par_w),
                                       par_h - pages_p->sb_width);
    }
    
    /*Vertical scrollbar*/
    if(page_h <= par_h) {        /*Full sized scroll bar*/
        lv_obj_set_height(page_p->sbv_dp,  par_h - pages_p->sb_width);
        lv_obj_set_pos(page_p->sbv_dp, par_w - pages_p->sb_width, 0);
		lv_obj_set_opa(page_p->sbv_dp, OPA_TRANSP);
    } else {
    	if(pages_p->sb_mode == LV_PAGE_SB_MODE_ON) {
    		lv_obj_set_opa(page_p->sbv_dp, (pages_p->sb_opa * OPA_COVER) / 100);
    	}
        size_tmp = ((par_h * (par_h - pages_p->sb_width)) / page_h);
        lv_obj_set_height(page_p->sbv_dp,  size_tmp);

        lv_obj_set_pos(page_p->sbv_dp, 
                       par_w - pages_p->sb_width,
                   (-(lv_obj_get_y(page_dp) - pages_p->margin_ver) * (par_h - size_tmp - pages_p->sb_width)) /
                                      (page_h - par_h));
    }
    
}
#endif
