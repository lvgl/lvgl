/**
 * @file lv_mbox.c
 * 
 */


/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_MBOX != 0

#include "lv_mbox.h"
#include "../lv_obj/lv_group.h"
#include "misc/gfx/anim.h"
#include "misc/math/math_base.h"

/*********************
 *      DEFINES
 *********************/
#define LV_MBOX_CLOSE_ANIM_TIME     200 /*Default close anim. time [ms]*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_mbox_signal(lv_obj_t * mbox, lv_signal_t sign, void * param);
static void btnh_resize(lv_obj_t *mbox);

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

/*----------------- 
 * Create function
 *-----------------*/

/**
 * Create a message box objects
 * @param par pointer to an object, it will be the parent of the new message box
 * @param copy pointer to a message box object, if not NULL then the new object will be copied from it
 * @return pointer to the created message box
 */
lv_obj_t * lv_mbox_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor message box*/
	lv_obj_t * new_mbox = lv_cont_create(par, copy);
    dm_assert(new_mbox);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_mbox);
    
    /*Allocate the message box type specific extended data*/
    lv_mbox_ext_t * ext = lv_obj_allocate_ext_attr(new_mbox, sizeof(lv_mbox_ext_t));
    dm_assert(ext);
    ext->txt = NULL;
    ext->btnh = NULL;
    ext->style_btn_rel = &lv_style_btn_off_released;
    ext->style_btn_pr = &lv_style_btn_off_pressed;
    ext->anim_time = LV_MBOX_CLOSE_ANIM_TIME;
    ext->btn_width = 0;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_mbox, lv_mbox_signal);

    /*Init the new message box message box*/
    if(copy == NULL) {
    	lv_cont_set_layout(new_mbox, LV_CONT_LAYOUT_COL_M);
    	lv_cont_set_fit(new_mbox, true, true);

    	ext->txt = lv_label_create(new_mbox, NULL);
    	lv_label_set_align(ext->txt, LV_LABEL_ALIGN_CENTER);
    	lv_label_set_text(ext->txt, "Message");

    	lv_obj_set_style(new_mbox, &lv_style_pretty);
    }
    /*Copy an existing message box*/
    else {
        lv_mbox_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        ext->txt = lv_label_create(new_mbox, copy_ext->txt);
        ext->btn_width = copy_ext->btn_width;

        /*Copy the buttons and the label on them*/
        if(copy_ext->btnh != NULL) {
            lv_obj_t * btn_copy;
            const char * btn_txt_copy;
            LL_READ_BACK(copy_ext->btnh->child_ll, btn_copy) {
                btn_txt_copy = lv_label_get_text(lv_obj_get_child(btn_copy, NULL));
                lv_mbox_add_btn(new_mbox, btn_txt_copy, lv_btn_get_action(btn_copy, LV_BTN_ACTION_RELEASE));
            }
        }

        lv_mbox_set_style(new_mbox, lv_mbox_get_style_bg(copy), lv_mbox_get_style_btnh(copy));
        lv_mbox_set_style_btn(new_mbox, copy_ext->style_btn_rel, copy_ext->style_btn_pr);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_mbox);
    }
    
    return new_mbox;
}

/**
 * A release action which can be assigned to a message box button to close it
 * @param btn pointer to the released button
 * @return always lv_action_res_t because the button is deleted with the mesage box
 */
lv_res_t lv_mbox_close_action(lv_obj_t * btn)
{
    lv_obj_t * mbox = lv_mbox_get_from_btn(btn);
    bool deleted = lv_mbox_get_anim_time(mbox) ? false : true;

    lv_mbox_start_auto_close(mbox, 0);

    return deleted ? LV_RES_INV : LV_RES_OK;
}

/**
 * Add a button to the message box
 * @param mbox pointer to message box object
 * @param btn_txt the text of the button
 * @param rel_action a function which will be called when the button is released
 * @return pointer to the created button (lv_btn)
 */
lv_obj_t * lv_mbox_add_btn(lv_obj_t * mbox, const char * btn_txt, lv_action_t rel_action)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    /*Create a button holder if it is not existed yet*/
    if(ext->btnh == NULL) {
        ext->btnh = lv_cont_create(mbox, NULL);
        lv_obj_set_style(ext->btnh, &lv_style_transp_fit);
        lv_obj_set_click(ext->btnh, false);
        lv_cont_set_fit(ext->btnh, false, true);
        lv_cont_set_layout(ext->btnh, LV_CONT_LAYOUT_PRETTY);
    }

    lv_obj_t *btn = lv_btn_create(ext->btnh, NULL);
    lv_btn_set_action(btn, LV_BTN_ACTION_RELEASE, rel_action);
    lv_btn_set_style(btn, ext->style_btn_rel, ext->style_btn_pr, NULL, NULL, NULL);

    if(ext->btn_width) {
        lv_btn_set_fit(btn, false, true);
        lv_obj_set_width(btn, ext->btn_width);
    } else {
        lv_btn_set_fit(btn, true, true);
    }

    lv_obj_t *label = lv_label_create(btn, NULL);
    lv_label_set_text(label, btn_txt);

    btnh_resize(mbox);

    return btn;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of the message box
 * @param mbox pointer to a message box
 * @param txt a '\0' terminated character string which will be the message box text
 */
void lv_mbox_set_text(lv_obj_t * mbox, const char * txt)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    lv_label_set_text(ext->txt, txt);

    btnh_resize(mbox);
}

/**
 * Set the width of the buttons
 * @param mbox pointer to message box object
 * @param w width of the buttons or 0 to use auto fit
 */
void lv_mbox_set_btn_width(lv_obj_t *mbox, cord_t w)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    ext->btn_width = w;
    if(ext->btnh == NULL) return;

    lv_obj_t *btn = lv_obj_get_child(ext->btnh, NULL);
    while(btn != NULL) {
        if(w) {
            lv_btn_set_fit(btn, false, true);
            lv_obj_set_width(btn, w);
        } else {
            lv_btn_set_fit(btn, true, true);
        }
        btn = lv_obj_get_child(ext->btnh, btn);
    }

    btnh_resize(mbox);
}

/**
 * Set the styles of a message box
 * @param mbox pointer to a message box object
 * @param bg pointer to the new background style
 * @param btnh pointer to the new button holder style
 */
void lv_mbox_set_style(lv_obj_t *mbox, lv_style_t *bg, lv_style_t *btnh)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    lv_obj_set_style(ext->btnh, btnh);
    lv_obj_set_style(mbox, bg);
}

/**
 * Set styles of the buttons of a message box in each state
 * @param mbox pointer to a message box object
 * @param rel pointer to a style for releases state
 * @param pr  pointer to a style for pressed state
 */
void lv_mbox_set_style_btn(lv_obj_t * mbox, lv_style_t * rel, lv_style_t * pr)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    ext->style_btn_rel = rel;
    ext->style_btn_pr = pr;

    if(ext->btnh != NULL) {
        lv_obj_t * btn = lv_obj_get_child(ext->btnh, NULL);

        while(btn != NULL) {
            lv_btn_set_style(btn, rel, pr, NULL, NULL, NULL);
            btn = lv_obj_get_child(mbox, btn);
        }
    }

    btnh_resize(mbox);
}

/**
 * Set animation duration
 * @param mbox pointer to a message box object
 * @param time animation length in  milliseconds (0: no animation)
 */
void lv_mbox_set_anim_time(lv_obj_t * mbox, uint16_t time)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    ext->anim_time = time;
}

/**
 * Automatically delete the message box after a given time
 * @param mbox pointer to a message box object
 * @param delay a time (in milliseconds) to wait before delete the message box
 */
void lv_mbox_start_auto_close(lv_obj_t * mbox, uint16_t delay)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

    if(ext->anim_time != 0) {
        /*Add shrinking animations*/
        lv_obj_animate(mbox, LV_ANIM_GROW_H| ANIM_OUT, ext->anim_time, delay, NULL);
        lv_obj_animate(mbox, LV_ANIM_GROW_V| ANIM_OUT, ext->anim_time, delay, lv_obj_del);

        /*Disable fit to let shrinking work*/
        lv_cont_set_fit(mbox, false, false);
    } else {
        lv_obj_animate(mbox, LV_ANIM_NONE, ext->anim_time, delay, lv_obj_del);
    }
}

/**
 * Stop the auto. closing of message box
 * @param mbox pointer to a message box object
 */
void lv_mbox_stop_auto_close(lv_obj_t * mbox)
{
    anim_del(mbox, NULL);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of the message box
 * @param mbox pointer to a message box object
 * @return pointer to the text of the message box
 */
const char * lv_mbox_get_text(lv_obj_t * mbox)
{
	lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);

	return lv_label_get_text(ext->txt);
}

/**
 * Get width of the buttons
 * @param mbox pointer to a message box object
 * @return width of the buttons (0: auto fit enabled)
 */
cord_t lv_mbox_get_btn_width(lv_obj_t * mbox)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    return ext->btn_width;
}

/**
 * Get the message box object from one of its button.
 * It is useful in the button release actions where only the button is known
 * @param btn pointer to a button of a message box
 * @return pointer to the button's message box
 */
lv_obj_t * lv_mbox_get_from_btn(lv_obj_t * btn)
{
	lv_obj_t * btnh = lv_obj_get_parent(btn);
	lv_obj_t * mbox = lv_obj_get_parent(btnh);

	return mbox;
}

/**
 * Get the animation duration (close animation time)
 * @param mbox pointer to a message box object
 * @return animation length in  milliseconds (0: no animation)
 */
uint16_t lv_mbox_get_anim_time(lv_obj_t * mbox )
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    return ext->anim_time;
}

/**
 * Get the style of a message box's button holder
 * @param mbox pointer to a message box object
 * @return pointer to the message box's background style
 */
lv_style_t * lv_mbox_get_style_btnh(lv_obj_t *mbox)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    return lv_obj_get_style(ext->btnh);
}


/**
 * Get the style of the buttons on a message box
 * @param mbox pointer to a message box object
 * @param state a state from 'lv_btn_state_t' in which style should be get
 * @return pointer to the style in the given state
 */
lv_style_t * lv_mbox_get_style_btn(lv_obj_t * mbox, lv_btn_state_t state)
{
    lv_btn_ext_t * ext = lv_obj_get_ext_attr(mbox);

    if(ext->styles[state] == NULL) return lv_obj_get_style(mbox);

    return ext->styles[state];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the message box
 * @param mbox pointer to a message box object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_mbox_signal(lv_obj_t * mbox, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(mbox, sign, param);
    if(res != LV_RES_OK) return res;

    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    if(sign == LV_SIGNAL_CORD_CHG) {
        if(lv_obj_get_width(mbox) != area_get_width(param)) {
            btnh_resize(mbox);
        }
    }
    if(sign == LV_SIGNAL_LONG_PRESS) {
        lv_mbox_start_auto_close(mbox, 0);
        lv_indev_wait_release(param);
        res = LV_RES_INV;
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        /*Refresh all the buttons*/
        if(ext->btnh != NULL) {
            lv_obj_t * btn;
            btn = lv_obj_get_child(ext->btnh, NULL);
            while(btn != NULL) {
                /*Refresh the next button's style*/
                lv_btn_set_style(btn, ext->style_btn_rel, ext->style_btn_pr, NULL, NULL, NULL);
                btn = lv_obj_get_child(ext->btnh, btn);
            }
        }
    }
    else if(sign == LV_SIGNAL_FOCUS) {
        /*Get the first button*/
        if(ext->btnh != NULL) {
            lv_obj_t * btn = NULL;
            lv_obj_t * btn_prev = NULL;
            btn = lv_obj_get_child(ext->btnh, btn);
            while(btn != NULL) {
                btn_prev = btn;
                btn = lv_obj_get_child(ext->btnh, btn);
            }
            if(btn_prev != NULL) {
                lv_btn_set_state(btn_prev, LV_BTN_STATE_PRESSED);
            }
        }
    }
    else if(sign == LV_SIGNAL_DEFOCUS) {
        /*Get the 'pressed' button*/
        if(ext->btnh != NULL) {
            lv_obj_t * btn = NULL;
            btn = lv_obj_get_child(ext->btnh, btn);
            while(btn != NULL) {
                if(lv_btn_get_state(btn) == LV_BTN_STATE_PRESSED) break;
                btn = lv_obj_get_child(ext->btnh, btn);
            }

            if(btn != NULL) {
                lv_btn_set_state(btn, LV_BTN_STATE_RELEASED);
            }
        }
    }
    else if(sign == LV_SIGNAL_CONTROLL) {
        lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
        char c = *((char*)param);
        if(c == LV_GROUP_KEY_RIGHT || c == LV_GROUP_KEY_UP) {
            /*Get the last pressed button*/
            if(ext->btnh != NULL) {
                lv_obj_t * btn = NULL;
                lv_obj_t * btn_prev = NULL;
                btn = lv_obj_get_child(ext->btnh, btn);
                while(btn != NULL) {
                    if(lv_btn_get_state(btn) == LV_BTN_STATE_PRESSED) break;
                    btn_prev = btn;
                    btn = lv_obj_get_child(ext->btnh, btn);
                }

                if(btn_prev != NULL && btn != NULL) {
                    lv_btn_set_state(btn, LV_BTN_STATE_RELEASED);
                    lv_btn_set_state(btn_prev, LV_BTN_STATE_PRESSED);
                }
            }
        }
        else if(c == LV_GROUP_KEY_LEFT || c == LV_GROUP_KEY_DOWN) {
            /*Get the last pressed button*/
            if(ext->btnh != NULL) {
                lv_obj_t * btn = NULL;
                btn = lv_obj_get_child(ext->btnh, btn);
                while(btn != NULL) {
                    if(lv_btn_get_state(btn) == LV_BTN_STATE_PRESSED) break;
                    btn = lv_obj_get_child(ext->btnh, btn);
                }

                if(btn != NULL) {
                    lv_obj_t * btn_prev = lv_obj_get_child(ext->btnh, btn);
                    if(btn_prev != NULL) {
                        lv_btn_set_state(btn, LV_BTN_STATE_RELEASED);
                        lv_btn_set_state(btn_prev, LV_BTN_STATE_PRESSED);
                    }
                }

            }
        }
        else if(c == LV_GROUP_KEY_ENTER) {
            /*Get the 'pressed' button*/
           if(ext->btnh != NULL) {
               lv_obj_t * btn = NULL;
               btn = lv_obj_get_child(ext->btnh, btn);
               while(btn != NULL) {
                   if(lv_btn_get_state(btn) == LV_BTN_STATE_PRESSED) break;
                   btn = lv_obj_get_child(ext->btnh, btn);
               }

               if(btn != NULL) {
                   lv_action_t rel_action;
                   rel_action = lv_btn_get_action(btn, LV_BTN_ACTION_RELEASE);
                   if(rel_action != NULL) rel_action(btn);
               }
           }
        }
    }

    return LV_RES_OK;
}

/**
 * Resize the button holder to fit
 * @param mbox pointer to message box object
 */
static void btnh_resize(lv_obj_t *mbox)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext_attr(mbox);
    if(ext->btnh == NULL) return;

    lv_style_t *btnh_style = lv_mbox_get_style_bg(ext->btnh);
    cord_t btnh_req_w = 2 * btnh_style->body.padding.hor;

    lv_obj_t *btn = lv_obj_get_child(ext->btnh, NULL);
    while(btn != NULL) {
        btnh_req_w += lv_obj_get_width(btn) + btnh_style->body.padding.inner;
        btn = lv_obj_get_child(ext->btnh, btn);
    }

    btnh_req_w -= btnh_style->body.padding.inner;       /*Trim the last inner padding*/

    cord_t txt_w = lv_obj_get_width(ext->txt);

    lv_obj_set_width(ext->btnh, btnh_req_w > txt_w ? btnh_req_w : txt_w);
}

#endif
