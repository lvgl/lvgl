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
static lv_res_t lv_list_signal(lv_obj_t *list, lv_signal_t sign, void *param);
static lv_obj_t * get_next_btn(lv_obj_t *list, lv_obj_t *prev_btn);
static void refr_btn_width(lv_obj_t *list);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t btn_signal;
static lv_signal_func_t img_signal;
static lv_signal_func_t label_signal;
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

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
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_list);

    lv_list_ext_t * ext = lv_obj_allocate_ext_attr(new_list, sizeof(lv_list_ext_t));
    dm_assert(ext);

    ext->style_img = NULL;
    ext->styles_btn[LV_BTN_STATE_REL] = &lv_style_btn_rel;
    ext->styles_btn[LV_BTN_STATE_PR] = &lv_style_btn_pr;
    ext->styles_btn[LV_BTN_STATE_TGL_REL] = &lv_style_btn_tgl_rel;
    ext->styles_btn[LV_BTN_STATE_PR] = &lv_style_btn_tgl_pr;
    ext->styles_btn[LV_BTN_STATE_INA] = &lv_style_btn_ina;
    ext->anim_time = LV_LIST_FOCUS_TIME;

	lv_obj_set_signal_func(new_list, lv_list_signal);

    /*Init the new list object*/
    if(copy == NULL) {
    	lv_obj_set_size(new_list, 2 * LV_DPI, 3 * LV_DPI);
		lv_page_set_scrl_layout(new_list, LV_LIST_LAYOUT_DEF);
		lv_page_set_sb_mode(new_list, LV_PAGE_SB_MODE_DRAG);
        lv_list_set_style(new_list, LV_LIST_STYLE_BG, &lv_style_transp_fit);
        lv_list_set_style(new_list, LV_LIST_STYLE_SCRL, &lv_style_pretty);
    } else {
        lv_list_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        lv_obj_t *copy_btn = lv_obj_get_child_back(lv_page_get_scrl(copy), NULL);
        lv_obj_t *new_btn;
        while(copy_btn) {
            new_btn = lv_btn_create(new_list, copy_btn);
            lv_obj_t *copy_img = lv_list_get_btn_img(copy_btn);
            if(copy_img) lv_img_create(new_btn, copy_img);
            lv_label_create(new_btn, lv_list_get_btn_label(copy_btn));
            copy_btn = lv_obj_get_child_back(lv_page_get_scrl(copy), copy_btn);
        }

        lv_list_set_style(new_list, LV_LIST_STYLE_BTN_REL, copy_ext->styles_btn[LV_BTN_STATE_REL]);
        lv_list_set_style(new_list, LV_LIST_STYLE_BTN_PR, copy_ext->styles_btn[LV_BTN_STATE_PR]);
        lv_list_set_style(new_list, LV_LIST_STYLE_BTN_TGL_REL, copy_ext->styles_btn[LV_BTN_STATE_TGL_REL]);
        lv_list_set_style(new_list, LV_LIST_STYLE_BTN_TGL_PR, copy_ext->styles_btn[LV_BTN_STATE_TGL_REL]);
        lv_list_set_style(new_list, LV_LIST_STYLE_BTN_INA, copy_ext->styles_btn[LV_BTN_STATE_INA]);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_list);
    }
    
    return new_list;
}

/*======================
 * Add/remove functions
 *=====================*/

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
    lv_list_ext_t * ext = lv_obj_get_ext_attr(list);

	/*Create a list element with the image an the text*/
	lv_obj_t * liste;
	liste = lv_btn_create(list, NULL);
	lv_btn_set_style(liste, LV_BTN_STYLE_REL, ext->styles_btn[LV_BTN_STATE_REL]);
    lv_btn_set_style(liste, LV_BTN_STYLE_PR, ext->styles_btn[LV_BTN_STATE_PR]);
    lv_btn_set_style(liste, LV_BTN_STYLE_TGL_REL, ext->styles_btn[LV_BTN_STATE_TGL_REL]);
    lv_btn_set_style(liste, LV_BTN_STYLE_TGL_PR, ext->styles_btn[LV_BTN_STATE_TGL_PR]);
    lv_btn_set_style(liste, LV_BTN_STYLE_INA, ext->styles_btn[LV_BTN_STATE_INA]);

	lv_btn_set_action(liste, LV_BTN_ACTION_RELEASE, rel_action);
	lv_page_glue_obj(liste, true);
	lv_btn_set_layout(liste, LV_CONT_LAYOUT_ROW_M);
	lv_btn_set_fit(liste, false, true);
    if(btn_signal == NULL) btn_signal = lv_obj_get_signal_func(liste);

    /*Make the size adjustment*/
    cord_t w = lv_obj_get_width(list);
    lv_style_t *  style_scrl = lv_obj_get_style(lv_page_get_scrl(list));
    cord_t pad_hor_tot = style->body.padding.hor + style_scrl->body.padding.hor;
    w -= pad_hor_tot * 2;

    lv_obj_set_width(liste, w);
#if USE_LV_IMG != 0 && USE_FSINT != 0
	if(img_fn != NULL && img_fn[0] != '\0') {
		lv_obj_t * img = lv_img_create(liste, NULL);
		lv_img_set_file(img, img_fn);
		lv_obj_set_style(img, ext->style_img);
		lv_obj_set_click(img, false);
		if(img_signal == NULL) img_signal = lv_obj_get_signal_func(img);
	}
#endif
	if(txt != NULL) {
		lv_obj_t * label = lv_label_create(liste, NULL);
		lv_label_set_text(label, txt);
		lv_obj_set_click(label, false);
        lv_label_set_long_mode(label, LV_LABEL_LONG_ROLL);
		lv_obj_set_width(label, liste->coords.x2 - label->coords.x1);
        if(label_signal == NULL) label_signal = lv_obj_get_signal_func(label);
	}

	return liste;
}

/*=====================
 * Setter functions 
 *====================*/

/**
 * Set scroll animation duration on 'list_up()' 'list_down()' 'list_focus()'
 * @param list pointer to a list object
 * @param anim_time duration of animation [ms]
 */
void lv_list_set_anim_time(lv_obj_t *list, uint16_t anim_time)
{
    lv_list_ext_t * ext = lv_obj_get_ext_attr(list);
    ext->anim_time = anim_time;
}

/**
 * Set a style of a list
 * @param list pointer to a list object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_list_set_style(lv_obj_t *list, lv_list_style_t type, lv_style_t *style)
{
    lv_list_ext_t *ext = lv_obj_get_ext_attr(list);

    switch (type) {
        case LV_LIST_STYLE_BG:
            lv_page_set_style(list, LV_PAGE_STYLE_BG, style);
            break;
        case LV_LIST_STYLE_SCRL:
            lv_page_set_style(list, LV_PAGE_STYLE_SCRL, style);
            break;
        case LV_LIST_STYLE_SB:
            lv_page_set_style(list, LV_PAGE_STYLE_SB, style);
            break;
        case LV_LIST_STYLE_BTN_REL:
            ext->styles_btn[LV_BTN_STATE_REL] = style;
            break;
        case LV_LIST_STYLE_BTN_PR:
            ext->styles_btn[LV_BTN_STATE_PR] = style;
            break;
        case LV_LIST_STYLE_BTN_TGL_REL:
            ext->styles_btn[LV_BTN_STATE_TGL_REL] = style;
            break;
        case LV_LIST_STYLE_BTN_TGL_PR:
            ext->styles_btn[LV_BTN_STATE_TGL_PR] = style;
            break;
        case LV_LIST_STYLE_BTN_INA:
            ext->styles_btn[LV_BTN_STATE_INA] = style;
            break;
    }


    /*Refresh existing buttons' style*/
    if(type == LV_LIST_STYLE_BTN_PR || type == LV_LIST_STYLE_BTN_REL ||
      type == LV_LIST_STYLE_BTN_TGL_REL || type == LV_LIST_STYLE_BTN_TGL_PR ||
      type == LV_LIST_STYLE_BTN_INA)
    {
        lv_obj_t * liste = get_next_btn(list, NULL);
        while(liste != NULL) {
            lv_btn_set_style(liste, LV_BTN_STYLE_REL, ext->styles_btn[LV_BTN_STYLE_REL]);
            lv_btn_set_style(liste, LV_BTN_STYLE_PR, ext->styles_btn[LV_BTN_STYLE_PR]);
            lv_btn_set_style(liste, LV_BTN_STYLE_TGL_REL, ext->styles_btn[LV_BTN_STYLE_TGL_REL]);
            lv_btn_set_style(liste, LV_BTN_STYLE_TGL_PR, ext->styles_btn[LV_BTN_STYLE_TGL_PR]);
            lv_btn_set_style(liste, LV_BTN_STYLE_INA, ext->styles_btn[LV_BTN_STYLE_INA]);
            liste = get_next_btn(list, liste);
        }
    }
}

/*=====================
 * Getter functions 
 *====================*/

/**
 * Get the text of a list element
 * @param btn pointer to list element
 * @return pointer to the text
 */
const char * lv_list_get_btn_text(lv_obj_t * btn)
{
    lv_obj_t * label = lv_list_get_btn_label(btn);
    if(label == NULL) return "";
    return lv_label_get_text(label);
}

/**
 * Get the label object from a list element
 * @param btn pointer to a list element (button)
 * @return pointer to the label from the list element or NULL if not found
 */
lv_obj_t * lv_list_get_btn_label(lv_obj_t * btn)
{
    lv_obj_t * label = lv_obj_get_child(btn, NULL);
    if(label == NULL) return NULL;

    while(label->signal_func != label_signal) {
        label = lv_obj_get_child(btn, label);
        if(label == NULL) break;
    }

    return label;
}

/**
 * Get the image object from a list element
 * @param btn pointer to a list element (button)
 * @return pointer to the image from the list element or NULL if not found
 */
lv_obj_t * lv_list_get_btn_img(lv_obj_t * btn)
{
#if USE_LV_IMG != 0 && USE_FSINT != 0
    lv_obj_t * img = lv_obj_get_child(btn, NULL);
    if(img == NULL) return NULL;

    while(img->signal_func != img_signal) {
        img = lv_obj_get_child(btn, img);
        if(img == NULL) break;
    }

    return img;
#else
    return NULL;
#endif
}

/**
 * Get scroll animation duration
 * @param list pointer to a list object
 * @return duration of animation [ms]
 */
uint16_t lv_list_get_anim_time(lv_obj_t *list)
{
    lv_list_ext_t * ext = lv_obj_get_ext_attr(list);
    return ext->anim_time;
}

/**
 * Get a style of a list
 * @param list pointer to a list object
 * @param type which style should be get
 * @return style pointer to a style
 *  */
lv_style_t * lv_list_get_style(lv_obj_t *list, lv_list_style_t type)
{
    lv_list_ext_t *ext = lv_obj_get_ext_attr(list);

    switch (type) {
        case LV_LIST_STYLE_BG:          return lv_page_get_style(list, LV_PAGE_STYLE_BG);
        case LV_LIST_STYLE_SCRL:        return lv_page_get_style(list, LV_PAGE_STYLE_SB);
        case LV_LIST_STYLE_SB:          return lv_page_get_style(list, LV_PAGE_STYLE_SCRL);
        case LV_LIST_STYLE_BTN_REL:     return ext->styles_btn[LV_BTN_STATE_REL];
        case LV_LIST_STYLE_BTN_PR:      return ext->styles_btn[LV_BTN_STATE_PR];
        case LV_LIST_STYLE_BTN_TGL_REL: return ext->styles_btn[LV_BTN_STATE_TGL_REL];
        case LV_LIST_STYLE_BTN_TGL_PR:  return ext->styles_btn[LV_BTN_STATE_TGL_PR];
        case LV_LIST_STYLE_BTN_INA:     return ext->styles_btn[LV_BTN_STATE_INA];
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
}
/*=====================
 * Other functions
 *====================*/

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
    e = get_next_btn(list, NULL);
    while(e != NULL) {
        if(e->coords.y2 <= list->coords.y2) {
            if(e_prev != NULL) {
                cord_t new_y = lv_obj_get_height(list) - (lv_obj_get_y(e_prev) + lv_obj_get_height(e_prev));
                lv_list_ext_t *ext = lv_obj_get_ext_attr(list);
                if(ext->anim_time == 0) {
                    lv_obj_set_y(scrl, new_y);
                } else {
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
                }
            }
            break;
        }
        e_prev = e;
        e = get_next_btn(list, e);
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
    e = get_next_btn(list, NULL);
    while(e != NULL) {
        if(e->coords.y1 < list->coords.y1) {
            cord_t new_y = -lv_obj_get_y(e);
            lv_list_ext_t *ext = lv_obj_get_ext_attr(list);
            if(ext->anim_time == 0) {
                lv_obj_set_y(scrl, new_y);
            } else {
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
            }
            break;
        }
        e = get_next_btn(list, e);
    }
}

/**
 * Focus on a list button. It ensures that the button will be visible on the list.
 * @param btn pointer to a list button to focus
 * @param anim_en true: scroll with animation, false: without animation
 */
void lv_list_focus(lv_obj_t *btn, bool anim_en)
{
    lv_obj_t *list = lv_obj_get_parent(lv_obj_get_parent(btn));

    lv_page_focus(list, btn, anim_en == false ? 0 :lv_list_get_anim_time(list));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the list
 * @param list pointer to a list object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_list_signal(lv_obj_t * list, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(list, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_CORD_CHG) {
        /*Be sure the width of the buttons are correct*/
        cord_t w = lv_obj_get_width(list);
        if(w != area_get_width(param)) {   /*Width changed*/
           refr_btn_width(list);
        }
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        /*Because of the possible change of horizontal and vertical padding refresh buttons width */
        refr_btn_width(list);
    }
    else if(sign == LV_SIGNAL_FOCUS) {
        /*Get the first button*/
        lv_obj_t * btn = NULL;
        lv_obj_t * btn_prev = NULL;
        btn = get_next_btn(list, btn);
        while(btn != NULL) {
            btn_prev = btn;
            btn = get_next_btn(list, btn);
        }
        if(btn_prev != NULL) {
            lv_btn_set_state(btn_prev, LV_BTN_STATE_PR);
        }
    }
    else if(sign == LV_SIGNAL_DEFOCUS) {
        /*Get the 'pressed' button*/
        lv_obj_t * btn = NULL;
        btn = get_next_btn(list, btn);
        while(btn != NULL) {
            if(lv_btn_get_state(btn) == LV_BTN_STATE_PR) break;
            btn = get_next_btn(list, btn);
        }

        if(btn != NULL) {
            lv_btn_set_state(btn, LV_BTN_STATE_REL);
        }
    }
    else if(sign == LV_SIGNAL_CONTROLL) {
        char c = *((char*)param);
        if(c == LV_GROUP_KEY_RIGHT || c == LV_GROUP_KEY_DOWN) {
            /*Get the last pressed button*/
            lv_obj_t * btn = NULL;
            lv_obj_t * btn_prev = NULL;
            lv_list_ext_t *ext = lv_obj_get_ext_attr(list);
            btn = get_next_btn(list, btn);
            while(btn != NULL) {
                if(lv_btn_get_state(btn) == LV_BTN_STATE_PR) break;
                btn_prev = btn;
                btn = get_next_btn(list, btn);
            }

            if(btn_prev != NULL && btn != NULL) {
                lv_btn_set_state(btn, LV_BTN_STATE_REL);
                lv_btn_set_state(btn_prev, LV_BTN_STATE_PR);
                lv_page_focus(list, btn_prev, ext->anim_time);
            }
        }
        else if(c == LV_GROUP_KEY_LEFT || c == LV_GROUP_KEY_UP) {
            /*Get the last pressed button*/
            lv_obj_t * btn = NULL;
            lv_list_ext_t *ext = lv_obj_get_ext_attr(list);
            btn = get_next_btn(list, btn);
            while(btn != NULL) {
                if(lv_btn_get_state(btn) == LV_BTN_STATE_PR) break;
                btn = get_next_btn(list, btn);
            }

            if(btn != NULL) {
                lv_obj_t * btn_prev = get_next_btn(list, btn);
                if(btn_prev != NULL) {
                    lv_btn_set_state(btn, LV_BTN_STATE_REL);
                    lv_btn_set_state(btn_prev, LV_BTN_STATE_PR);
                    lv_page_focus(list, btn_prev, ext->anim_time);
                }
            }
        } else if(c == LV_GROUP_KEY_ENTER) {
            /*Get the 'pressed' button*/
            lv_obj_t * btn = NULL;
            btn = get_next_btn(list, btn);
            while(btn != NULL) {
                if(lv_btn_get_state(btn) == LV_BTN_STATE_PR) break;
                btn = get_next_btn(list, btn);
            }

            if(btn != NULL) {
                lv_action_t rel_action;
                rel_action = lv_btn_get_action(btn, LV_BTN_ACTION_RELEASE);
                if(rel_action != NULL) rel_action(btn);
            }
        }
    }
    return res;
}

/**
 * Get the next button from list
 * @param list pointer to a list object
 * @param prev_btn pointer to button. Search the next after it.
 * @return pointer to the next button or NULL
 */
static lv_obj_t * get_next_btn(lv_obj_t * list, lv_obj_t * prev_btn)
{
    /* Not a good practice but user can add/create objects to the lists manually.
     * When getting the next button try to be sure that it is at least a button */

    lv_obj_t * btn ;
    lv_obj_t * scrl = lv_page_get_scrl(list);

    btn = lv_obj_get_child(scrl, prev_btn);
    if(btn == NULL) return NULL;

    while(btn->signal_func != btn_signal) {
        btn = lv_obj_get_child(scrl, prev_btn);
        if(btn == NULL) break;
    }

    return btn;
}

static void refr_btn_width(lv_obj_t *list)
{
   lv_style_t *style = lv_list_get_style(list, LV_LIST_STYLE_BG);
   lv_style_t *style_scrl = lv_obj_get_style(lv_page_get_scrl(list));
   cord_t w = lv_obj_get_width(list);
   cord_t btn_w = w - (style->body.padding.hor + style_scrl->body.padding.hor) * 2;

   lv_obj_t *btn = get_next_btn(list, NULL);
   while(btn) {
       /*Make the size adjustment for each buttons*/
       if(lv_obj_get_width(btn) != btn_w) {
           lv_obj_set_width(btn, btn_w);
           /*Set the label size to roll its text*/
           lv_obj_t *label = lv_list_get_btn_label(btn);
           lv_obj_set_width(label, btn->coords.x2 - label->coords.x1);
           lv_label_set_text(label, NULL);
       }
       btn = get_next_btn(list, btn);
   }
}


#endif
