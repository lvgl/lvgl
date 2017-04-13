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
#include <lvgl/lv_objx/lv_cont.h>
#include "misc/math/math_base.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LIST_LAYOUT_DEF	LV_CONT_LAYOUT_COL_M

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if 0
static bool lv_list_design(lv_obj_t * list, const area_t * mask, lv_design_mode_t mode);
#endif

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
 * Create a list objects
 * @param par pointer to an object, it will be the parent of the new list
 * @param copy pointer to a list object, if not NULL then the new object will be copied from it
 * @return pointer to the created list
 */
lv_obj_t * lv_list_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor basic object*/
    lv_obj_t * new_list = lv_page_create(par, copy);
    dm_assert(new_list);
    lv_list_ext_t * ext = lv_obj_alloc_ext(new_list, sizeof(lv_list_ext_t));
    dm_assert(ext);

    ext->width_sb = 0;
    ext->styles_liste[LV_BTN_STATE_REL] = lv_style_get(LV_STYLE_BTN_REL, NULL);
    ext->styles_liste[LV_BTN_STATE_PR] = lv_style_get(LV_STYLE_BTN_PR, NULL);
    ext->styles_liste[LV_BTN_STATE_TREL] = lv_style_get(LV_STYLE_BTN_TREL, NULL);
    ext->styles_liste[LV_BTN_STATE_PR] = lv_style_get(LV_STYLE_BTN_TPR, NULL);
    ext->styles_liste[LV_BTN_STATE_INA] = lv_style_get(LV_STYLE_BTN_INA, NULL);

	lv_obj_set_signal_f(new_list, lv_list_signal);

    /*Init the new list object*/
    if(copy == NULL) {
    	lv_obj_set_size_us(new_list, 2 * LV_DPI, 3 * LV_DPI);
		lv_cont_set_layout(ext->page.scrl, LV_LIST_LAYOUT_DEF);
		lv_obj_set_style(new_list, lv_style_get(LV_STYLE_TRANSP_TIGHT, NULL));
		lv_obj_set_style(lv_page_get_scrl(new_list), lv_style_get(LV_STYLE_PRETTY, NULL));
		lv_page_set_sb_mode(new_list, LV_PAGE_SB_MODE_AUTO);
    } else {
        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_list);
    }
    
    return new_list;
}

/**
 * Signal function of the list
 * @param list pointer to a list object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_list_signal(lv_obj_t * list, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_page_signal(list, sign, param);
    
    return valid;
}

/**
 * Add a list element to the list
 * @param list pointer to list object
 * @param img_fn file name of an image before the text (NULL if unused)
 * @param txt text of the list element (NULL if unused)
 * @param rel_action pointer to release action function (like with lv_btn)
 * @return pointer to the new list element which can be customized (a button)
 */
lv_obj_t * lv_list_add(lv_obj_t * list, const char * img_fn, const char * txt, lv_action_t rel_action)
{
	lv_style_t * style = lv_obj_get_style(list);
    lv_list_ext_t * ext = lv_obj_get_ext(list);

	/*Create a list element with the image an the text*/
	lv_obj_t * liste;
	liste = lv_btn_create(list, NULL);
	lv_btn_set_styles(liste, ext->styles_liste[LV_BTN_STATE_REL], ext->styles_liste[LV_BTN_STATE_PR],
	                         ext->styles_liste[LV_BTN_STATE_TREL], ext->styles_liste[LV_BTN_STATE_TPR],
	                         ext->styles_liste[LV_BTN_STATE_INA]);

	lv_btn_set_rel_action(liste, rel_action);
	lv_page_glue_obj(liste, true);
	lv_cont_set_layout(liste, LV_CONT_LAYOUT_ROW_M);
	lv_cont_set_fit(liste, false, true);

	if(img_fn != NULL && img_fn[0] != '\0') {
		lv_obj_t * img = lv_img_create(liste, NULL);
		lv_img_set_file(img, img_fn);
		lv_obj_set_style(img, ext->styles_liste[LV_BTN_STATE_REL]);
		lv_obj_set_click(img, false);
	}

	if(txt != NULL) {
		lv_obj_t * label = lv_label_create(liste, NULL);
		lv_label_set_text(label, txt);
		lv_obj_set_style(label, ext->styles_liste[LV_BTN_STATE_REL]);
		lv_obj_set_click(label, false);
	}

	/*Make the size adjustment*/
    cord_t w = lv_obj_get_width(list);
    lv_style_t *  style_scrl = lv_obj_get_style(lv_page_get_scrl(list));
    cord_t hpad_tot = style->hpad + style_scrl->hpad;
    w -= hpad_tot * 2;

    /*Make place for the scrollbar if hpad_tot is too small*/
    if(ext->width_sb != 0) {
        if(hpad_tot < ext->page.sb_width) w -= ext->page.sb_width - hpad_tot;
    }
    lv_obj_set_width(liste, w);

	return liste;
}

/**
 * Move the list elements up by one
 * @param list pointer a to list object
 */
void lv_list_up(lv_obj_t * list)
{
	/*Search the first list element which 'y' coordinate is below the parent
	 * and position the list to show this element on the bottom*/
	lv_obj_t * h = lv_obj_get_parent(list);
	lv_obj_t * e;
	lv_obj_t * e_prev = NULL;
	e = lv_obj_get_child(list, NULL);
	while(e != NULL) {
		if(e->cords.y2 <= h->cords.y2) {
			if(e_prev != NULL)
			lv_obj_set_y(list, lv_obj_get_height(h) -
					             (lv_obj_get_y(e_prev) + lv_obj_get_height(e_prev)));
			break;
		}
		e_prev = e;
		e = lv_obj_get_child(list, e);
	}
}

/**
 * Move the list elements down by one
 * @param list pointer to a list object
 */
void lv_list_down(lv_obj_t * list)
{
	/*Search the first list element which 'y' coordinate is above the parent
	 * and position the list to show this element on the top*/
	lv_obj_t * h = lv_obj_get_parent(list);
	lv_obj_t * e;
	e = lv_obj_get_child(list, NULL);
	while(e != NULL) {
		if(e->cords.y1 < h->cords.y1) {
			lv_obj_set_y(list, -lv_obj_get_y(e));
			break;
		}
		e = lv_obj_get_child(list, e);
	}
}


/*=====================
 * Setter functions 
 *====================*/

/**
 * Set styles of the list elements of a list in each state
 * @param list pointer to list object
 * @param rel pointer to a style for releases state
 * @param pr  pointer to a style for pressed state
 * @param trel pointer to a style for toggled releases state
 * @param tpr pointer to a style for toggled pressed state
 * @param ina pointer to a style for inactive state
 */
void lv_list_set_styles_liste(lv_obj_t * list, lv_style_t * rel, lv_style_t * pr, lv_style_t * trel, lv_style_t * tpr, lv_style_t * ina)
{
    lv_list_ext_t * ext = lv_obj_get_ext(list);

    ext->styles_liste[LV_BTN_STATE_REL] = rel;
    ext->styles_liste[LV_BTN_STATE_PR] = pr;
    ext->styles_liste[LV_BTN_STATE_TREL] = trel;
    ext->styles_liste[LV_BTN_STATE_TPR] = tpr;
    ext->styles_liste[LV_BTN_STATE_INA] = ina;

    lv_obj_t * scrl = lv_page_get_scrl(list);
    lv_obj_t * liste = lv_obj_get_child(scrl, NULL);
    while(liste != NULL)
    {
        lv_btn_set_styles(liste, rel, pr, trel, tpr, ina);
        liste = lv_obj_get_child(scrl, liste);
    }

}

/*=====================
 * Getter functions 
 *====================*/

/**
 * Get the text of a list element
 * @param liste pointer to list element
 * @return pointer to the text
 */
const char * lv_list_element_get_txt(lv_obj_t * liste)
{
    /*The last child is the label*/
    lv_obj_t * label = lv_obj_get_child(liste, NULL);
    return lv_label_get_text(label);
}


/**
 * Get the style of the list elements in a given state
 * @param list pointer to a button object
 * @param state a state from 'lv_btn_state_t' in which style should be get
 * @return pointer to the style in the given state
 */
lv_style_t * lv_list_get_style_liste(lv_obj_t * list, lv_btn_state_t state)
{
    lv_list_ext_t * ext = lv_obj_get_ext(list);

    if(ext->styles_liste[state] == NULL) return lv_obj_get_style(list);

    return ext->styles_liste[state];
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#if 0 /*A new design function is not necessary*/
/**
 * Handle the drawing related tasks of the lists
 * @param list pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_list_design(lv_obj_t * list, const area_t * mask, lv_design_mode_t mode)
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
