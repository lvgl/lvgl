/**
 * @file lv_list.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_LIST != 0

#include "lv_list.h"
#include "lv_rect.h"
#include "misc/math/math_base.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LIST_LAYOUT_DEF	LV_RECT_LAYOUT_COL_M

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if 0
static bool lv_list_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_lists_t lv_lists_def =
{
	/*Page style*/
	.pages.bg_rects.objs.color = COLOR_MAKE(0x20, 0x50, 0x80), .pages.bg_rects.gcolor = COLOR_SILVER, .pages.bg_rects.bcolor = COLOR_GRAY,
	.pages.bg_rects.bopa = 50, .pages.bg_rects.bwidth = 0 * LV_STYLE_MULT, .pages.bg_rects.round = 2 * LV_STYLE_MULT,
	.pages.bg_rects.empty = 1,
	.pages.bg_rects.vpad = 10 * LV_STYLE_MULT,
	.pages.bg_rects.hpad = 10 * LV_STYLE_MULT,
	.pages.bg_rects.opad = 5 * LV_STYLE_MULT,

	.pages.sb_rects.objs.color = COLOR_BLACK, .pages.sb_rects.gcolor = COLOR_BLACK, .pages.sb_rects.bcolor = COLOR_WHITE,
	.pages.sb_rects.bopa = 50, .pages.sb_rects.bwidth = 1 * LV_STYLE_MULT, .pages.sb_rects.round = 5 * LV_STYLE_MULT,
	.pages.sb_rects.empty = 0, .pages.sb_width= 8 * LV_STYLE_MULT, .pages.sb_opa=50, .pages.sb_mode = LV_PAGE_SB_MODE_AUTO,

	/*List element style*/
	.liste_btns.mcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0xa0, 0xa0, 0xa0), .liste_btns.gcolor[LV_BTN_STATE_REL] = COLOR_WHITE, .liste_btns.bcolor[LV_BTN_STATE_REL] = COLOR_WHITE,
	.liste_btns.mcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0xa0, 0xa0, 0xa0), .liste_btns.gcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0xa0, 0xc0, 0xe0), .liste_btns.bcolor[LV_BTN_STATE_PR] = COLOR_WHITE,
	.liste_btns.mcolor[LV_BTN_STATE_TGL_REL] = COLOR_MAKE(0x60,0x80,0xa0), .liste_btns.gcolor[LV_BTN_STATE_TGL_REL] = COLOR_MAKE(0xc0, 0xd0, 0xf0), .liste_btns.bcolor[LV_BTN_STATE_TGL_REL] = COLOR_WHITE,
	.liste_btns.mcolor[LV_BTN_STATE_TGL_PR] = COLOR_MAKE(0x60, 0x80, 0xa0), .liste_btns.gcolor[LV_BTN_STATE_TGL_PR] = COLOR_MAKE(0x80, 0xa0, 0xc0), .liste_btns.bcolor[LV_BTN_STATE_TGL_PR] = COLOR_WHITE,
	.liste_btns.mcolor[LV_BTN_STATE_INA] = COLOR_SILVER, .liste_btns.gcolor[LV_BTN_STATE_INA] = COLOR_GRAY, .liste_btns.bcolor[LV_BTN_STATE_INA] = COLOR_WHITE,
	.liste_btns.rects.bwidth = 2 * LV_STYLE_MULT, .liste_btns.rects.bopa = 50,
	.liste_btns.rects.empty = 0, .liste_btns.rects.round = 4 * LV_STYLE_MULT,
	.liste_btns.rects.hpad = 10 * LV_STYLE_MULT,
	.liste_btns.rects.vpad = 10 * LV_STYLE_MULT,
	.liste_btns.rects.opad = 20 * LV_STYLE_MULT,

	.liste_labels.objs.color = COLOR_MAKE(0x20,0x20,0x20), .liste_labels.font = LV_FONT_DEFAULT,
	.liste_labels.letter_space = 2 * LV_STYLE_MULT, .liste_labels.line_space = 2 * LV_STYLE_MULT,
	.liste_labels.mid = 0,

	.liste_imgs.recolor_opa = OPA_COVER,

	.liste_layout = LV_RECT_LAYOUT_ROW_M
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
 * Create a list objects
 * @param par_dp pointer to an object, it will be the parent of the new list
 * @param copy_dp pointer to a list object, if not NULL then the new object will be copied from it
 * @return pointer to the created list
 */
lv_obj_t* lv_list_create(lv_obj_t* par_dp, lv_obj_t * copy_dp)
{
    /*Create the ancestor basic object*/
    lv_obj_t * new_obj_dp = lv_page_create(par_dp, copy_dp);
    dm_assert(new_obj_dp);
    lv_list_ext_t * ext_p= lv_obj_alloc_ext(new_obj_dp, sizeof(lv_list_ext_t));
    
    /*Init the new list object*/
    if(copy_dp == NULL) {
    	ext_p->fit = LV_LIST_FIT_LONGEST;
    	lv_obj_set_signal_f(new_obj_dp, lv_list_signal);
		lv_obj_set_style(new_obj_dp, &lv_lists_def);
		lv_rect_set_layout(new_obj_dp, LV_LIST_LAYOUT_DEF);
    }
    
    return new_obj_dp;
}

/**
 * Signal function of the list
 * @param obj_dp pointer to a list object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_list_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_page_signal(obj_dp, sign, param);

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

/**
 * Add a list element to the list
 * @param obj_dp pointer to list object
 * @param img_fn file name of an image before the text (NULL if unused)
 * @param txt text of the list element (NULL if unused)
 * @param rel_action pointer to release action function (like with lv_btn)
 * @return pointer to the new list element which can be customized (a button)
 */
lv_obj_t * lv_list_add(lv_obj_t * obj_dp, const char * img_fn, const char * txt, bool (*rel_action)(lv_obj_t*, lv_dispi_t *))
{
	lv_lists_t * lists_p = lv_obj_get_style(obj_dp);
	lv_list_ext_t  * ext_p = lv_obj_get_ext(obj_dp);

	/*Create a list element with the image an the text*/
	lv_obj_t * liste;
	liste = lv_btn_create(obj_dp, NULL);
	lv_obj_set_style(liste, &lists_p->liste_btns);
	lv_btn_set_rel_action(liste, rel_action);
	lv_page_glue_obj(liste, true);
	lv_rect_set_layout(liste, lv_lists_def.liste_layout);
	lv_rect_set_fit(liste, true, true);   /*hor. fit might be disabled later*/

	if(img_fn != NULL) {
		lv_obj_t * img = lv_img_create(liste, NULL);
		lv_img_set_file(img, img_fn);
		lv_obj_set_style(img, &lists_p->liste_imgs);
		lv_obj_set_click(img, false);
	}

	if(txt != NULL) {
		lv_obj_t * label = lv_label_create(liste, NULL);
		lv_label_set_text(label, txt);
		lv_obj_set_style(label,&lists_p->liste_labels);
		lv_obj_set_click(label, false);
	}

	/*Make the adjustment*/
	if(ext_p->fit == LV_LIST_FIT_HOLDER) {
		/*Now the width will be adjusted*/
		lv_rect_set_fit(liste, false, true);
		cord_t w = lv_obj_get_width(lv_obj_get_parent(obj_dp));
		w -= lists_p->pages.bg_rects.hpad * 2;
		lv_obj_set_width(liste, w);
	} else if(ext_p->fit == LV_LIST_FIT_LONGEST) {
		/*Now the width will be adjusted*/
		lv_rect_set_fit(liste, false, true);

		lv_obj_t * e;
		cord_t w = 0;
		/*Get the longest list element*/
		e = lv_obj_get_child(obj_dp, NULL);
		while(e != NULL) {
			w = max(w, lv_obj_get_width(e));
			e = lv_obj_get_child(obj_dp, e);
		}

		/*Set all list element to the longest width*/
		e = lv_obj_get_child(obj_dp, NULL);
		while(e != NULL) {
			if(lv_obj_get_width(e) != w) {
				lv_obj_set_width(e, w);
			}
			e = lv_obj_get_child(obj_dp, e);
		}
	}

	return liste;
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_lists_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_lists_t style
 */
lv_lists_t * lv_lists_get(lv_lists_builtin_t style, lv_lists_t * copy_p)
{
	lv_lists_t  *style_p;

	switch(style) {
		case LV_LISTS_DEF:
			style_p = &lv_lists_def;
			break;
		default:
			style_p = &lv_lists_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_lists_t));
		else memcpy(copy_p, &lv_lists_def, sizeof(lv_lists_t));
	}

	return style_p;
}

/**
 * Move the list elements up by one
 * @param obj_dp pointer a to list object
 */
void lv_list_up(lv_obj_t * obj_dp)
{
	/*Search the first list element which 'y' coordinate is below the parent
	 * and position the list to show this element on the bottom*/
	lv_obj_t * h = lv_obj_get_parent(obj_dp);
	lv_obj_t * e;
	lv_obj_t * e_prev = NULL;
	e = lv_obj_get_child(obj_dp, NULL);
	while(e != NULL) {
		if(e->cords.y2 <= h->cords.y2) {
			if(e_prev != NULL)
			lv_obj_set_y(obj_dp, lv_obj_get_height(h) -
					             (lv_obj_get_y(e_prev) + lv_obj_get_height(e_prev)));
			break;
		}
		e_prev = e;
		e = lv_obj_get_child(obj_dp, e);
	}
}

/**
 * Move the list elements down by one
 * @param obj_dp pointer to a list object
 */
void lv_list_down(lv_obj_t * obj_dp)
{
	/*Search the first list element which 'y' coordinate is above the parent
	 * and position the list to show this element on the top*/
	lv_obj_t * h = lv_obj_get_parent(obj_dp);
	lv_obj_t * e;
	e = lv_obj_get_child(obj_dp, NULL);
	while(e != NULL) {
		if(e->cords.y1 < h->cords.y1) {
			lv_obj_set_y(obj_dp, -lv_obj_get_y(e));
			break;
		}
		e = lv_obj_get_child(obj_dp, e);
	}
}

/*=====================
 * Setter functions 
 *====================*/

/**
 * Set the list element fitting of a list
 * @param obj_dp pointer to a list object
 * @param fit type of fitting (from lv_list_fit_t)
 */
void lv_list_set_fit(lv_obj_t * obj_dp, lv_list_fit_t fit)
{
	lv_list_ext_t * ext_p = lv_obj_get_ext(obj_dp);

	ext_p->fit = fit;
}


/*=====================
 * Getter functions 
 *====================*/

/**
 * Get the fit type of a list
 * @param obj_dp pointer to list object
 * @return the fit (from lv_list_fit_t)
 */
lv_list_fit_t lv_list_get_fit(lv_obj_t * obj_dp)
{
	return LV_EA(obj_dp, lv_list_ext_t)->fit;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if 0 /*A new design function is not necessary*/
/**
 * Handle the drawing related tasks of the lists
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'
 */
static bool lv_list_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return false;
    }

    /*Draw the object*/

    return true;
}
#endif


#endif
