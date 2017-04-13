/**
 * @file lv_btn.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/

#include <lvgl/lv_misc/area.h>
#include <lvgl/lv_obj/lv_obj.h>
#include <misc/others/color.h>
#include <stdbool.h>

#if USE_LV_BTN != 0

#include <string.h>
#include "lv_btn.h"
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_btn_design(lv_obj_t * btn, const area_t * mask, lv_design_mode_t mode);


/**********************
 *  STATIC VARIABLES
 **********************/

static lv_design_f_t ancestor_design_f;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a button objects
 * @param par pointer to an object, it will be the parent of the new button
 * @param copy pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
lv_obj_t * lv_btn_create(lv_obj_t * par, lv_obj_t * copy)
{
    lv_obj_t * new_btn;
    
    new_btn = lv_rect_create(par, copy);
    dm_assert(new_btn);
    /*Allocate the extended data*/
    lv_btn_ext_t * ext = lv_obj_alloc_ext(new_btn, sizeof(lv_btn_ext_t));
    dm_assert(ext);
    ext->state = LV_BTN_STATE_REL;
    ext->pr_action = NULL;
    ext->rel_action = NULL;
    ext->lpr_action = NULL;
    ext->lpr_rep_action = NULL;
    ext->styles[LV_BTN_STATE_REL] = lv_style_get(LV_STYLE_BTN_REL, NULL);
    ext->styles[LV_BTN_STATE_PR] = lv_style_get(LV_STYLE_BTN_PR, NULL);
    ext->styles[LV_BTN_STATE_TREL] = lv_style_get(LV_STYLE_BTN_TREL, NULL);
    ext->styles[LV_BTN_STATE_TPR] = lv_style_get(LV_STYLE_BTN_TPR, NULL);
    ext->styles[LV_BTN_STATE_INA] = lv_style_get(LV_STYLE_BTN_INA, NULL);
    ext->lpr_exec = 0;
    ext->tgl = 0;

    if(ancestor_design_f  == NULL) ancestor_design_f = lv_obj_get_design_f(new_btn);

    lv_obj_set_signal_f(new_btn, lv_btn_signal);
    lv_obj_set_design_f(new_btn, lv_btn_design);
    
    /*If no copy do the basic initialization*/
    if(copy == NULL) {
	    lv_rect_set_layout(new_btn, LV_RECT_LAYOUT_CENTER);
	    lv_obj_set_style(new_btn, ext->styles[LV_BTN_STATE_REL]);
    }
    /*Copy 'copy'*/
    else {
    	lv_btn_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->state = copy_ext->state;
    	ext->pr_action = copy_ext->pr_action;
    	ext->rel_action = copy_ext->rel_action;
    	ext->lpr_action = copy_ext->lpr_action;
        ext->lpr_rep_action = copy_ext->lpr_action;
        ext->styles[LV_BTN_STATE_REL] = copy_ext->styles[LV_BTN_STATE_REL];
        ext->styles[LV_BTN_STATE_PR] = copy_ext->styles[LV_BTN_STATE_PR];
        ext->styles[LV_BTN_STATE_TREL] = copy_ext->styles[LV_BTN_STATE_TREL];
        ext->styles[LV_BTN_STATE_TPR] = copy_ext->styles[LV_BTN_STATE_TPR];
        ext->styles[LV_BTN_STATE_INA] = copy_ext->styles[LV_BTN_STATE_INA];
    	ext->tgl = copy_ext->tgl;

    	/*Refresh the style with new signal function*/
        lv_obj_refr_style(new_btn);
    }
    
    return new_btn;
}

/**
 * Signal function of the button
 * @param btn pointer to a button object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_btn_signal(lv_obj_t * btn, lv_signal_t sign, void * param)
{   
    bool valid;

    /* Include the ancient signal function */
    valid = lv_rect_signal(btn, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        lv_btn_ext_t * ext = lv_obj_get_ext(btn);
    	lv_btn_state_t state = lv_btn_get_state(btn);
        bool tgl = lv_btn_get_tgl(btn);

        switch (sign) {
            case LV_SIGNAL_PRESSED:
                /*Refresh the state*/
                if(ext->state == LV_BTN_STATE_REL) {
                	lv_btn_set_state(btn, LV_BTN_STATE_PR);
                } else if(ext->state == LV_BTN_STATE_TREL) {
                	lv_btn_set_state(btn, LV_BTN_STATE_TPR);
                }

                ext->lpr_exec = 0;
                /*Call the press action, here 'param' is the caller dispi*/
                if(ext->pr_action != NULL && state != LV_BTN_STATE_INA) {
                	valid = ext->pr_action(btn, param);
                }
                break;

            case LV_SIGNAL_PRESS_LOST:
                /*Refresh the state*/
                if(ext->state == LV_BTN_STATE_PR) lv_btn_set_state(btn, LV_BTN_STATE_REL);
                else if(ext->state == LV_BTN_STATE_TPR) lv_btn_set_state(btn, LV_BTN_STATE_TREL);

                break;
            case LV_SIGNAL_PRESSING:
                /*When the button begins to drag revert pressed states to released*/
                if(lv_dispi_is_dragging(param) != false) {
                    if(ext->state == LV_BTN_STATE_PR) lv_btn_set_state(btn, LV_BTN_STATE_REL);
                    else if(ext->state == LV_BTN_STATE_TPR) lv_btn_set_state(btn, LV_BTN_STATE_TREL);
                }
                break;

            case LV_SIGNAL_RELEASED:
                /*If not dragged and it was not long press action then
                 *change state and run the action*/
                if(lv_dispi_is_dragging(param) == false && ext->lpr_exec == 0) {
                    if(ext->state == LV_BTN_STATE_PR && tgl == false) {
                    	lv_btn_set_state(btn, LV_BTN_STATE_REL);
                    } else if(ext->state == LV_BTN_STATE_TPR && tgl == false) {
                    	lv_btn_set_state(btn, LV_BTN_STATE_TREL);
                    } else if(ext->state == LV_BTN_STATE_PR && tgl == true) {
                    	lv_btn_set_state(btn, LV_BTN_STATE_TREL);
                    } else if(ext->state == LV_BTN_STATE_TPR && tgl == true) {
                    	lv_btn_set_state(btn, LV_BTN_STATE_REL);
                    }

                    if(ext->rel_action != NULL && state != LV_BTN_STATE_INA) {
                    	valid = ext->rel_action(btn, param);
                    }
                } else { /*If dragged change back the state*/
                    if(ext->state == LV_BTN_STATE_PR) {
                        lv_btn_set_state(btn, LV_BTN_STATE_REL);
                    } else if(ext->state == LV_BTN_STATE_TPR) {
                    	lv_btn_set_state(btn, LV_BTN_STATE_TREL);
                    }
                }

                break;
            case LV_SIGNAL_LONG_PRESS:
                /*Call the long press action, here 'param' is the caller dispi*/
                if(ext->lpr_action != NULL && state != LV_BTN_STATE_INA) {
                	ext->lpr_exec = 1;
                	valid = ext->lpr_action(btn, param);
                }
            	break;
            case LV_SIGNAL_LONG_PRESS_REP:
                /*Call the release action, here 'param' is the caller dispi*/
                if(ext->lpr_rep_action != NULL && state != LV_BTN_STATE_INA) {
                	valid = ext->lpr_rep_action(btn, param);
                }
            	break;
            default:
                /*Do nothing*/
                break;
        }
    }
    
    return valid;
}

/*=====================
 * Setter functions 
 *====================*/

/**
 * Enable the toggled states
 * @param btn pointer to a button object
 * @param tgl true: enable toggled states, false: disable
 */
void lv_btn_set_tgl(lv_obj_t * btn, bool tgl)
{
    lv_btn_ext_t * ext = lv_obj_get_ext(btn);
    
    ext->tgl = tgl != false ? 1 : 0;
}

/**
 * Set the state of the button
 * @param btn pointer to a button object
 * @param state the new state of the button (from lv_btn_state_t enum)
 */
void lv_btn_set_state(lv_obj_t * btn, lv_btn_state_t state)
{
    lv_btn_ext_t * ext = lv_obj_get_ext(btn);
    if(ext->state != state) {
        ext->state = state;
        lv_obj_set_style(btn, ext->styles[state]);
    }
}

/**
 * Set a function to call when the button is pressed
 * @param btn pointer to a button object
 * @param pr_action pointer to function
 */
void lv_btn_set_pr_action(lv_obj_t * btn, lv_action_t pr_action)
{
    lv_btn_ext_t * ext = lv_obj_get_ext(btn);
    
    ext->pr_action = pr_action;
}

/**
 * Set a function to call when the button is released
 * @param btn pointer to a button object
 * @param rel_action pointer to functionREL
 */
void lv_btn_set_rel_action(lv_obj_t * btn, lv_action_t rel_action)
{
    lv_btn_ext_t * btn_p = lv_obj_get_ext(btn);
    
    btn_p->rel_action = rel_action;
}

/**
 * Set a function to call when the button is long pressed
 * @param btn pointer to a button object
 * @param lpr_action pointer to function
 */
void lv_btn_set_lpr_action(lv_obj_t * btn, lv_action_t lpr_action)
{
    lv_btn_ext_t * ext = lv_obj_get_ext(btn);
    
    ext->lpr_action = lpr_action;
}

/**
 * Set a function to called periodically after long press.
 * @param btn pointer to a button object
 * @param lpr_rep_action pointer to function
 */
void lv_btn_set_lpr_rep_action(lv_obj_t * btn, lv_action_t lpr_rep_action)
{
    lv_btn_ext_t * ext = lv_obj_get_ext(btn);

    ext->lpr_rep_action = lpr_rep_action;
}

/**
 * Set styles of a button is each state
 * @param btn pointer to button object
 * @param rel pointer to a style for releases state
 * @param pr  pointer to a style for pressed state
 * @param trel pointer to a style for toggled releases state
 * @param tpr pointer to a style for toggled pressed state
 * @param ina pointer to a style for inactive state
 */
void lv_btn_set_styles(lv_obj_t * btn, lv_style_t * rel, lv_style_t * pr, lv_style_t * trel, lv_style_t * tpr, lv_style_t * ina)
{
    lv_btn_ext_t * ext = lv_obj_get_ext(btn);
    ext->styles[LV_BTN_STATE_REL] = rel;
    ext->styles[LV_BTN_STATE_PR] = pr;
    ext->styles[LV_BTN_STATE_TREL] = trel;
    ext->styles[LV_BTN_STATE_TPR] = tpr;
    ext->styles[LV_BTN_STATE_INA] = ina;

    lv_obj_set_style(btn, ext->styles[ext->state]);

}

/*=====================
 * Getter functions 
 *====================*/

/**
 * Get the current state of the button
 * @param btn pointer to a button object
 * @return the state of the button (from lv_btn_state_t enum)
 */
lv_btn_state_t lv_btn_get_state(lv_obj_t * btn)
{
    lv_btn_ext_t * ext = lv_obj_get_ext(btn);
    
    return ext->state;
}

/**
 * Get the toggle enable attribute of the button
 * @param btn pointer to a button object
 * @return ture: toggle enabled, false: disabled
 */
bool lv_btn_get_tgl(lv_obj_t * btn)
{
    lv_btn_ext_t * ext = lv_obj_get_ext(btn);
    
    return ext->tgl != 0 ? true : false;
}

/**
 * Get the style of a button in a given state
 * @param btn pointer to a button object
 * @param state a state from 'lv_btn_state_t' in which style should be get
 * @return pointer to the style in the given state
 */
lv_style_t * lv_btn_get_style(lv_obj_t * btn, lv_btn_state_t state)
{
    lv_btn_ext_t * ext = lv_obj_get_ext(btn);

    if(ext->styles[state] == NULL) return lv_obj_get_style(btn->par);

    return ext->styles[state];
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
/**
 * Handle the drawing related tasks of the buttons
 * @param btn pointer to a button object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'        
 */
static bool lv_btn_design(lv_obj_t * btn, const area_t * mask, lv_design_mode_t mode)
{

    /* Because of the radius it is not sure the area is covered*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_design_f(btn, mask, mode); /*Draw the rectangle*/

    } else if(mode == LV_DESIGN_DRAW_MAIN || mode == LV_DESIGN_DRAW_POST) {
        ancestor_design_f(btn, mask, mode);	/*Draw the rectangle*/
    }
    return true;
}

#endif
