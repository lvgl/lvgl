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
#include "../lv_obj/lv_group.h"
#include "lv_cont.h"
#include "misc/gfx/anim.h"
#include "misc/math/math_base.h"

/*********************
 *      DEFINES
 *********************/
#define LV_LIST_LAYOUT_DEF	LV_CONT_LAYOUT_COL_M
#ifndef LV_LIST_FOCUS_TIME
#define LV_LIST_FOCUS_TIME  100 /*Animation time of focusing to the a list element [ms] (0: no animation)  */
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if 0
static bool lv_list_design(lv_obj_t * list, const area_t * mask, lv_design_mode_t mode);
#endif

static lv_obj_t * lv_list_get_next_btn(lv_obj_t * list, lv_obj_t * prev_btn);

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

    ext->sb_out = 0;
    ext->style_img = NULL;
    ext->styles_btn[LV_BTN_STATE_REL] = lv_style_get(LV_STYLE_BTN_REL, NULL);
    ext->styles_btn[LV_BTN_STATE_PR] = lv_style_get(LV_STYLE_BTN_PR, NULL);
    ext->styles_btn[LV_BTN_STATE_TREL] = lv_style_get(LV_STYLE_BTN_TREL, NULL);
    ext->styles_btn[LV_BTN_STATE_PR] = lv_style_get(LV_STYLE_BTN_TPR, NULL);
    ext->styles_btn[LV_BTN_STATE_INA] = lv_style_get(LV_STYLE_BTN_INA, NULL);

	lv_obj_set_signal_f(new_list, lv_list_signal);

    /*Init the new list object*/
    if(copy == NULL) {
    	lv_obj_set_size(new_list, 2 * LV_DPI, 3 * LV_DPI);
		lv_cont_set_layout(ext->page.scrl, LV_LIST_LAYOUT_DEF);
		lv_obj_set_style(new_list, lv_style_get(LV_STYLE_TRANSP_TIGHT, NULL));
		lv_obj_set_style(lv_page_get_scrl(new_list), lv_style_get(LV_STYLE_PRETTY, NULL));
		lv_page_set_sb_mode(new_list, LV_PAGE_SB_MODE_AUTO);
    } else {
        lv_list_ext_t * copy_ext = lv_obj_get_ext(copy);

        lv_list_set_styles_btn(new_list, copy_ext->styles_btn[LV_BTN_STATE_REL],
                                         copy_ext->styles_btn[LV_BTN_STATE_PR],
                                         copy_ext->styles_btn[LV_BTN_STATE_TREL],
                                         copy_ext->styles_btn[LV_BTN_STATE_TPR],
                                         copy_ext->styles_btn[LV_BTN_STATE_INA]);
        lv_list_set_style_img(new_list, copy_ext->style_img);

        lv_list_set_sb_out(new_list, copy_ext->sb_out);

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
    
    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        if(sign == LV_SIGNAL_FOCUS) {
            /*Get the first button*/
            lv_obj_t * btn = NULL;
            lv_obj_t * btn_prev = NULL;
            btn = lv_list_get_next_btn(list, btn);
            while(btn != NULL) {
                btn_prev = btn;
                btn = lv_list_get_next_btn(list, btn);
            }
            if(btn_prev != NULL) {
                lv_btn_set_state(btn_prev, LV_BTN_STATE_PR);
            }
        } else if(sign == LV_SIGNAL_DEFOCUS) {
            /*Get the 'pressed' button*/
            lv_obj_t * btn = NULL;
            btn = lv_list_get_next_btn(list, btn);
            while(btn != NULL) {
                if(lv_btn_get_state(btn) == LV_BTN_STATE_PR) break;
                btn = lv_list_get_next_btn(list, btn);
            }

            if(btn != NULL) {
                lv_btn_set_state(btn, LV_BTN_STATE_REL);
            }
        } else if(sign == LV_SIGNAL_CONTROLL) {
            char c = *((char*)param);
            if(c == LV_GROUP_KEY_RIGHT || c == LV_GROUP_KEY_DOWN) {
                /*Get the last pressed button*/
                lv_obj_t * btn = NULL;
                lv_obj_t * btn_prev = NULL;
                btn = lv_list_get_next_btn(list, btn);
                while(btn != NULL) {
                    if(lv_btn_get_state(btn) == LV_BTN_STATE_PR) break;
                    btn_prev = btn;
                    btn = lv_list_get_next_btn(list, btn);
                }

                if(btn_prev != NULL && btn != NULL) {
                    lv_btn_set_state(btn, LV_BTN_STATE_REL);
                    lv_btn_set_state(btn_prev, LV_BTN_STATE_PR);
                    lv_page_focus(list, btn_prev, LV_LIST_FOCUS_TIME);
                }
            } else if(c == LV_GROUP_KEY_LEFT || c == LV_GROUP_KEY_UP) {
                /*Get the last pressed button*/
                lv_obj_t * btn = NULL;
                btn = lv_list_get_next_btn(list, btn);
                while(btn != NULL) {
                    if(lv_btn_get_state(btn) == LV_BTN_STATE_PR) break;
                    btn = lv_list_get_next_btn(list, btn);
                }

                if(btn != NULL) {
                    lv_obj_t * btn_prev = lv_list_get_next_btn(list, btn);
                    if(btn_prev != NULL) {
                        lv_btn_set_state(btn, LV_BTN_STATE_REL);
                        lv_btn_set_state(btn_prev, LV_BTN_STATE_PR);
                        lv_page_focus(list, btn_prev, LV_LIST_FOCUS_TIME);
                    }
                }
            } else if(c == LV_GROUP_KEY_ENTER) {
                /*Get the 'pressed' button*/
                lv_obj_t * btn = NULL;
                btn = lv_list_get_next_btn(list, btn);
                while(btn != NULL) {
                    if(lv_btn_get_state(btn) == LV_BTN_STATE_PR) break;
                    btn = lv_list_get_next_btn(list, btn);
                }

                if(btn != NULL) {
                    lv_action_t rel_action;
                    rel_action = lv_btn_get_rel_action(btn);
                    if(rel_action != NULL) rel_action(btn);
                }
            }
        }
    }
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
	lv_btn_set_styles(liste, ext->styles_btn[LV_BTN_STATE_REL], ext->styles_btn[LV_BTN_STATE_PR],
	                         ext->styles_btn[LV_BTN_STATE_TREL], ext->styles_btn[LV_BTN_STATE_TPR],
	                         ext->styles_btn[LV_BTN_STATE_INA]);

	lv_btn_set_rel_action(liste, rel_action);
	lv_page_glue_obj(liste, true);
	lv_cont_set_layout(liste, LV_CONT_LAYOUT_ROW_M);
	lv_cont_set_fit(liste, false, true);

    /*Make the size adjustment*/
    cord_t w = lv_obj_get_width(list);
    lv_style_t *  style_scrl = lv_obj_get_style(lv_page_get_scrl(list));
    cord_t hpad_tot = style->hpad + style_scrl->hpad;
    w -= hpad_tot * 2;

    /*Make place for the scrollbar if hpad_tot is too small*/
    if(ext->sb_out != 0) {
        if(hpad_tot < ext->page.sb_width) w -= ext->page.sb_width - hpad_tot;
    }
    lv_obj_set_width(liste, w);
#if USE_LV_IMG != 0 && USE_FSINT != 0
	if(img_fn != NULL && img_fn[0] != '\0') {
		lv_obj_t * img = lv_img_create(liste, NULL);
		lv_img_set_file(img, img_fn);
		lv_obj_set_style(img, ext->style_img);
		lv_obj_set_click(img, false);
	}
#endif
	if(txt != NULL) {
		lv_obj_t * label = lv_label_create(liste, NULL);
		lv_label_set_text(label, txt);
		lv_obj_set_click(label, false);
		lv_obj_set_width(label, liste->cords.x2 - label->cords.x1);
        lv_label_set_long_mode(label, LV_LABEL_LONG_ROLL);
	}

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
    lv_obj_t * scrl = lv_page_get_scrl(list);
	lv_obj_t * e;
	lv_obj_t * e_prev = NULL;
	e = lv_list_get_next_btn(list, NULL);
	while(e != NULL) {
		if(e->cords.y2 <= list->cords.y2) {
			if(e_prev != NULL) {
			    cord_t new_y = lv_obj_get_height(list) - (lv_obj_get_y(e_prev) + lv_obj_get_height(e_prev));
#if LV_LIST_FOCUS_TIME == 0
			    lv_obj_set_y(scrl, new_y);
#else
                anim_t a;
                a.var = scrl;
                a.start = lv_obj_get_y(scrl);
                a.end = new_y;
                a.fp = (anim_fp_t)lv_obj_set_y;
                a.path = anim_get_path(ANIM_PATH_LIN);
                a.end_cb = NULL;
                a.act_time = 0;
                a.time = LV_LIST_FOCUS_TIME;
                a.playback = 0;
                a.playback_pause = 0;
                a.repeat = 0;
                a.repeat_pause = 0;
                anim_create(&a);
#endif
			}
			break;
		}
		e_prev = e;
		e = lv_list_get_next_btn(list, e);
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
	lv_obj_t * scrl = lv_page_get_scrl(list);
	lv_obj_t * e;
	e = lv_list_get_next_btn(list, NULL);
	while(e != NULL) {
		if(e->cords.y1 < list->cords.y1) {
            cord_t new_y = -lv_obj_get_y(e);
#if LV_LIST_FOCUS_TIME == 0
			lv_obj_set_y(scrl, new_y);
#else
            anim_t a;
            a.var = scrl;
            a.start = lv_obj_get_y(scrl);
            a.end = new_y;
            a.fp = (anim_fp_t)lv_obj_set_y;
            a.path = anim_get_path(ANIM_PATH_LIN);
            a.end_cb = NULL;
            a.act_time = 0;
            a.time = LV_LIST_FOCUS_TIME;
            a.playback = 0;
            a.playback_pause = 0;
            a.repeat = 0;
            a.repeat_pause = 0;
            anim_create(&a);
#endif
			break;
		}
		e = lv_list_get_next_btn(list, e);
	}
}

/*=====================
 * Setter functions 
 *====================*/

/**
 * Enable/Disable to scrollbar outside attribute
 * @param list pointer to list object
 * @param out true: reduce the buttons width therefore scroll bar will be out of the buttons,
 *            false: keep button size and place scroll bar on the buttons
 */
void lv_list_set_sb_out(lv_obj_t * list, bool out)
{
    lv_list_ext_t * ext = lv_obj_get_ext(list);

    ext->sb_out = out == false ? 0 : 1;
}


/**
 * Set styles of the list elements of a list in each state
 * @param list pointer to list object
 * @param rel pointer to a style for releases state
 * @param pr  pointer to a style for pressed state
 * @param trel pointer to a style for toggled releases state
 * @param tpr pointer to a style for toggled pressed state
 * @param ina pointer to a style for inactive state
 */
void lv_list_set_styles_btn(lv_obj_t * list, lv_style_t * rel, lv_style_t * pr,
                                               lv_style_t * trel, lv_style_t * tpr,
                                               lv_style_t * ina)
{
    lv_list_ext_t * ext = lv_obj_get_ext(list);

    ext->styles_btn[LV_BTN_STATE_REL] = rel;
    ext->styles_btn[LV_BTN_STATE_PR] = pr;
    ext->styles_btn[LV_BTN_STATE_TREL] = trel;
    ext->styles_btn[LV_BTN_STATE_TPR] = tpr;
    ext->styles_btn[LV_BTN_STATE_INA] = ina;

    lv_obj_t * liste = lv_list_get_next_btn(list, NULL);
    while(liste != NULL)
    {
        lv_btn_set_styles(liste, rel, pr, trel, tpr, ina);
        liste = lv_list_get_next_btn(list, liste);
    }
}


/**
 * Set the styles of the list element image (typically to set symbol font)
 * @param list pointer to list object
 * @param style pointer to the new style of the button images
 */
void lv_list_set_style_img(lv_obj_t * list, lv_style_t * style)
{
    lv_list_ext_t * ext = lv_obj_get_ext(list);

    ext->style_img = style;

    lv_obj_t * liste = lv_list_get_next_btn(list, NULL);
    lv_obj_t * img;
    while(liste != NULL)
    {
        img = lv_list_get_element_img(liste);
        if(img != NULL) lv_obj_set_style(img, style);

        liste = lv_list_get_next_btn(list, liste);
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
const char * lv_list_get_element_text(lv_obj_t * liste)
{
    lv_obj_t * label = lv_list_get_element_label(liste);
    if(label == NULL) return "";
    return lv_label_get_text(label);
}

/**
 * Get the label object from a list element
 * @param liste pointer to a list element (button)
 * @return pointer to the label from the list element or NULL if not found
 */
lv_obj_t * lv_list_get_element_label(lv_obj_t * liste)
{
    lv_obj_t * label = lv_obj_get_child(liste, NULL);
    if(label == NULL) return NULL;

    while(label->signal_f != lv_label_signal) {
        label = lv_obj_get_child(liste, NULL);
        if(label == NULL) break;
    }

    return label;
}

/**
 * Get the image object from a list element
 * @param liste pointer to a list element (button)
 * @return pointer to the image from the list element or NULL if not found
 */
lv_obj_t * lv_list_get_element_img(lv_obj_t * liste)
{
#if USE_LV_IMG != 0 && USE_FSINT != 0
    lv_obj_t * img = lv_obj_get_child(liste, NULL);
    if(img == NULL) return NULL;

    while(img->signal_f != lv_img_signal) {
        img = lv_obj_get_child(liste, NULL);
        if(img == NULL) break;
    }

    return img;
#else
    return NULL;
#endif
}

/**
 * Get the scroll bar outside attribute
 * @param list pointer to list object
 * @param en true: scroll bar outside the buttons, false: scroll bar inside
 */
bool lv_list_get_sb_out(lv_obj_t * list, bool en)
{
    lv_list_ext_t * ext = lv_obj_get_ext(list);
    return ext->sb_out == 0 ? false : true;
}

/**
 * Get the style of the list elements in a given state
 * @param list pointer to a list object
 * @param state a state from 'lv_btn_state_t' in which style should be get
 * @return pointer to the style in the given state
 */
lv_style_t * lv_list_get_style_liste(lv_obj_t * list, lv_btn_state_t state)
{
    lv_list_ext_t * ext = lv_obj_get_ext(list);

    if(ext->styles_btn[state] == NULL) return lv_obj_get_style(list);

    return ext->styles_btn[state];
}


/**
 * Get the style of the list elements images
 * @param list pointer to a list object
 * @return pointer to the image style
 */
lv_style_t * lv_list_get_style_img(lv_obj_t * list, lv_btn_state_t state)
{
    lv_list_ext_t * ext = lv_obj_get_ext(list);

    if(ext->style_img == NULL) return lv_list_get_style_liste(list, LV_BTN_STATE_REL);

    return ext->style_img;
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

/**
 * Get the next button from list
 * @param list pointer to a list object
 * @param prev_btn pointer to button. Search the next after it.
 * @return pointer to the next button or NULL
 */
static lv_obj_t * lv_list_get_next_btn(lv_obj_t * list, lv_obj_t * prev_btn)
{
    /* Not a good practice but user can add/create objects to the lists manually.
     * When getting the next button try to be sure that it is at least a button */

    lv_obj_t * btn ;
    lv_obj_t * scrl = lv_page_get_scrl(list);

    btn = lv_obj_get_child(scrl, prev_btn);
    if(btn == NULL) return NULL;

    while(btn->signal_f != lv_btn_signal) {
        btn = lv_obj_get_child(scrl, prev_btn);
        if(btn == NULL) break;
    }

    return btn;
}

#endif
