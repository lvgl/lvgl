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
#include "misc/gfx/anim.h"
#include "misc/math/math_base.h"

/*********************
 *      DEFINES
 *********************/
/*Test configurations*/
#ifndef LV_MBOX_ANIM_TIME
#define LV_MBOX_ANIM_TIME   250 /*How fast animate out the message box in auto close. 0: no animation [ms]*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if 0 /*Unused*/
static bool lv_mbox_design(lv_obj_t * mbox, const area_t * mask, lv_design_mode_t mode);
#endif
static void lv_mbox_realign(lv_obj_t * mbox);
static void lv_mbox_disable_fit(lv_obj_t  * mbox);

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
    
    /*Allocate the message box type specific extended data*/
    lv_mbox_ext_t * ext = lv_obj_alloc_ext(new_mbox, sizeof(lv_mbox_ext_t));
    dm_assert(ext);
    ext->txt = NULL;
    ext->btnh = NULL;
    ext->style_btn_rel = lv_style_get(LV_STYLE_BTN_REL, NULL);
    ext->style_btn_pr = lv_style_get(LV_STYLE_BTN_PR, NULL);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_f(new_mbox, lv_mbox_signal);

    /*Init the new message box message box*/
    if(copy == NULL) {
    	lv_cont_set_layout(new_mbox, LV_CONT_LAYOUT_COL_L);
    	lv_cont_set_fit(new_mbox, true, true);

    	ext->txt = lv_label_create(new_mbox, NULL);
    	lv_label_set_text(ext->txt, "Text of the message box");

    	lv_obj_set_style(new_mbox, lv_style_get(LV_STYLE_PRETTY, NULL));
    }
    /*Copy an existing message box*/
    else {
        lv_mbox_ext_t * copy_ext = lv_obj_get_ext(copy);

        ext->txt = lv_label_create(new_mbox, copy_ext->txt);

        /*Copy the buttons and the label on them*/
        if(copy_ext->btnh != NULL) {
            lv_obj_t * btn_copy;
            const char * btn_txt_copy;
            lv_btn_ext_t * btn_ext_copy;
            LL_READ_BACK(copy_ext->btnh->child_ll, btn_copy) {
                btn_txt_copy = lv_label_get_text(lv_obj_get_child(btn_copy, NULL));
                btn_ext_copy = lv_obj_get_ext(btn_copy);
                lv_mbox_add_btn(new_mbox, btn_txt_copy, btn_ext_copy->rel_action);
            }
        }
        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_mbox);
    }
    
    lv_mbox_realign(new_mbox);

    return new_mbox;
}

/**
 * Signal function of the message box
 * @param mbox pointer to a message box object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_mbox_signal(lv_obj_t * mbox, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_cont_signal(mbox, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	lv_mbox_ext_t * ext = lv_obj_get_ext(mbox);

    	if(sign == LV_SIGNAL_CORD_CHG) {
            /*If the size is changed refresh the message box*/
            if(area_get_width(param) != lv_obj_get_width(mbox) ||
               area_get_height(param) != lv_obj_get_height(mbox)) {
                lv_mbox_realign(mbox);
            }
        }
    	else if(sign == LV_SIGNAL_LONG_PRESS) {
#if LV_MBOX_ANIM_TIME != 0
            lv_mbox_start_auto_close(mbox, 0);
#else
            lv_obj_del(mbox);
            valid = false;
#endif
            lv_dispi_wait_release(param);
        }
    	else if(sign == LV_SIGNAL_STYLE_CHG) {
            /*Refresh all the buttons*/
            if(ext->btnh != NULL) {
                lv_obj_t * btn;
                btn = lv_obj_get_child(ext->btnh, NULL);
                while(btn != NULL) {
                    /*Refresh the next button's style*/
                    lv_btn_set_styles(btn, ext->style_btn_rel, ext->style_btn_pr, NULL, NULL, NULL);

                    btn = lv_obj_get_child(ext->btnh, btn);
                }
            }
        }
    }
    
    return valid;
}

/**
 * A release action which can be assigned to a message box button to close it
 * @param btn pointer to the released button
 * @param dispi pointer to the caller display input
 * @return always lv_action_res_t because the button is deleted with the mesage box
 */
lv_action_res_t lv_mbox_close_action(lv_obj_t * btn, lv_dispi_t * dispi)
{
    lv_obj_t * mbox = lv_mbox_get_from_btn(btn);

    lv_obj_del(mbox);

    return LV_ACTION_RES_INV;
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
    lv_mbox_ext_t * ext = lv_obj_get_ext(mbox);

    /*Create a button if it is not existed yet*/
    if(ext->btnh == NULL) {
        ext->btnh = lv_cont_create(mbox, NULL);
        lv_obj_set_style(ext->btnh, lv_style_get(LV_STYLE_TRANSP, NULL));
        lv_obj_set_click(ext->btnh, false);
        lv_cont_set_fit(ext->btnh, false, true);
        lv_cont_set_layout(ext->btnh, LV_CONT_LAYOUT_PRETTY);
    }

    lv_obj_t * btn;
    btn = lv_btn_create(ext->btnh, NULL);
    lv_btn_set_rel_action(btn, rel_action);
    lv_btn_set_styles(btn, ext->style_btn_rel, ext->style_btn_pr, NULL, NULL, NULL);
    lv_cont_set_fit(btn, true, true);

    lv_obj_t * label;
    label = lv_label_create(btn, NULL);
    lv_label_set_text(label, btn_txt);

    lv_mbox_realign(mbox);

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
    lv_mbox_ext_t * ext = lv_obj_get_ext(mbox);

    lv_label_set_text(ext->txt, txt);
    lv_mbox_realign(mbox);
}

/**
 * Set styles of the buttons of a message box in each state
 * @param mbox pointer to a message box object
 * @param rel pointer to a style for releases state
 * @param pr  pointer to a style for pressed state
 * @param trel pointer to a style for toggled releases state
 * @param tpr pointer to a style for toggled pressed state
 * @param ina pointer to a style for inactive state
 */
void lv_mbox_set_styles_btn(lv_obj_t * mbox, lv_style_t * rel, lv_style_t * pr)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext(mbox);

    ext->style_btn_rel = rel;
    ext->style_btn_pr = pr;
    lv_obj_t * btn = lv_obj_get_child(ext->btnh, NULL);

    while(btn != NULL) {
        lv_btn_set_styles(btn, rel, pr, NULL, NULL, NULL);
        btn = lv_obj_get_child(mbox, btn);
    }
}

/**
 * Automatically delete the message box after a given time
 * @param mbox pointer to a message box object
 * @param tout a time (in milliseconds) to wait before delete the message box
 */
void lv_mbox_start_auto_close(lv_obj_t * mbox, uint16_t tout)
{
#if LV_MBOX_ANIM_TIME != 0
    /*Add shrinking animations*/
    lv_obj_anim(mbox, LV_ANIM_GROW_H| ANIM_OUT, LV_MBOX_ANIM_TIME, tout, NULL);
	lv_obj_anim(mbox, LV_ANIM_GROW_V| ANIM_OUT, LV_MBOX_ANIM_TIME, tout, lv_obj_del);

    /*When the animations start disable fit to let shrinking work*/
    lv_obj_anim(mbox, LV_ANIM_NONE, 1, tout, lv_mbox_disable_fit);

#else
    lv_obj_anim(mbox, LV_ANIM_NONE, LV_MBOX_ANIM_TIME, tout, lv_obj_del);
#endif
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
const char * lv_mbox_get_txt(lv_obj_t * mbox)
{
	lv_mbox_ext_t * ext = lv_obj_get_ext(mbox);

	return lv_label_get_text(ext->txt);
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
 * Get the style of the buttons on a message box
 * @param mbox pointer to a message box object
 * @param state a state from 'lv_btn_state_t' in which style should be get
 * @return pointer to the style in the given state
 */
lv_style_t * lv_mbox_get_style_btn(lv_obj_t * mbox, lv_btn_state_t state)
{
    lv_btn_ext_t * ext = lv_obj_get_ext(mbox);

    if(ext->styles[state] == NULL) return lv_obj_get_style(mbox);

    return ext->styles[state];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if 0 /*Not used*/
/**
 * Handle the drawing related tasks of the message boxs
 * @param mbox pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_mbox_design(lv_obj_t * mbox, const area_t * mask, lv_design_mode_t mode)
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
 * Realign the elements of the message box
 * @param mbox pointer to message box object
 */
static void lv_mbox_realign(lv_obj_t * mbox)
{
    lv_mbox_ext_t * ext = lv_obj_get_ext(mbox);

    if(ext->txt == NULL) return;

    /*Set the button holder width to the width of the text and title*/
    if(ext->btnh != NULL) {
        cord_t txt_w = lv_obj_get_width(ext->txt);
        cord_t btn_w = 0;
        lv_obj_t * btn;
        btn = lv_obj_get_child(ext->btnh, NULL);
        while(btn != NULL) {
            btn_w = MATH_MAX(lv_obj_get_width(btn), btn_w);
            btn = lv_obj_get_child(ext->btnh, btn);
        }

        cord_t w = MATH_MAX(txt_w, btn_w);
        lv_obj_set_width(ext->btnh, w );
    }
}

/**
 * Called when the close animations starts to disable the recargle's fit
 * @param mbox ppointer to message box object
 */
static void lv_mbox_disable_fit(lv_obj_t  * mbox)
{
    lv_cont_set_fit(mbox, false, false);
}

#endif
