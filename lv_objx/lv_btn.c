/**
 * @file lv_btn.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_conf.h"
#if USE_LV_BTN != 0

#include <string.h>
#include "lv_btn.h"
#include "../lv_obj/lv_group.h"
#include "../lv_draw/lv_draw.h"
#include "misc/gfx/area.h"
#include "misc/gfx/color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if 0
static bool lv_btn_design(lv_obj_t * btn, const area_t * mask, lv_design_mode_t mode);
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

/**
 * Create a button objects
 * @param par pointer to an object, it will be the parent of the new button
 * @param copy pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
lv_obj_t * lv_btn_create(lv_obj_t * par, lv_obj_t * copy)
{
    lv_obj_t * new_btn;
    
    new_btn = lv_cont_create(par, copy);
    dm_assert(new_btn);
    /*Allocate the extended data*/
    lv_btn_ext_t * ext = lv_obj_allocate_ext_attr(new_btn, sizeof(lv_btn_ext_t));
    dm_assert(ext);
    ext->state = LV_BTN_STATE_OFF_RELEASED;

    ext->actions[LV_BTN_ACTION_PRESS] = NULL;
    ext->actions[LV_BTN_ACTION_RELEASE] = NULL;
    ext->actions[LV_BTN_ACTION_LONG_PRESS] = NULL;
    ext->actions[LV_BTN_ACTION_LONG_PRESS_REPEATE] = NULL;

    ext->styles[LV_BTN_STATE_OFF_RELEASED] = lv_style_get(LV_STYLE_BUTTON_OFF_RELEASED);
    ext->styles[LV_BTN_STATE_OFF_PRESSED] = lv_style_get(LV_STYLE_BUTTON_OFF_PRESSED);
    ext->styles[LV_BTN_STATE_ON_RELEASED] = lv_style_get(LV_STYLE_BUTTON_ON_RELEASED);
    ext->styles[LV_BTN_STATE_ON_PRESSED] = lv_style_get(LV_STYLE_BUTTON_ON_PRESSED);
    ext->styles[LV_BTN_STATE_INACTIVE] = lv_style_get(LV_STYLE_BUTTON_INACTIVE);

    ext->long_press_action_executed = 0;
    ext->toggle = 0;

    lv_obj_set_signal_func(new_btn, lv_btn_signal);
    
    /*If no copy do the basic initialization*/
    if(copy == NULL) {
	    lv_cont_set_layout(new_btn, LV_CONT_LAYOUT_CENTER);
	    lv_obj_set_style(new_btn, ext->styles[LV_BTN_STATE_OFF_RELEASED]);
    }
    /*Copy 'copy'*/
    else {
    	lv_btn_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
    	ext->state = copy_ext->state;
        ext->toggle = copy_ext->toggle;
    	memcpy(ext->actions, copy_ext->actions, sizeof(ext->actions));
        memcpy(ext->styles, copy_ext->styles, sizeof(ext->styles));

    	/*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_btn);
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
    valid = lv_cont_signal(btn, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);
    	lv_btn_state_t state = lv_btn_get_state(btn);
        bool tgl = lv_btn_get_toggle(btn);

        if(sign == LV_SIGNAL_PRESSED) {
            /*Refresh the state*/
            if(ext->state == LV_BTN_STATE_OFF_RELEASED) {
                lv_btn_set_state(btn, LV_BTN_STATE_OFF_PRESSED);
            } else if(ext->state == LV_BTN_STATE_ON_RELEASED) {
                lv_btn_set_state(btn, LV_BTN_STATE_ON_PRESSED);
            }

            ext->long_press_action_executed = 0;
            /*Call the press action, 'param' is the caller indev_proc*/
            if(ext->actions[LV_BTN_ACTION_PRESS] && state != LV_BTN_STATE_INACTIVE) {
                valid = ext->actions[LV_BTN_ACTION_PRESS](btn);
            }
        }
        else if(sign ==  LV_SIGNAL_PRESS_LOST) {
            /*Refresh the state*/
            if(ext->state == LV_BTN_STATE_OFF_PRESSED) lv_btn_set_state(btn, LV_BTN_STATE_OFF_RELEASED);
            else if(ext->state == LV_BTN_STATE_ON_PRESSED) lv_btn_set_state(btn, LV_BTN_STATE_ON_RELEASED);
        }
        else if(sign == LV_SIGNAL_PRESSING) {
            /*When the button begins to drag revert pressed states to released*/
            if(lv_indev_is_dragging(param) != false) {
                if(ext->state == LV_BTN_STATE_OFF_PRESSED) lv_btn_set_state(btn, LV_BTN_STATE_OFF_RELEASED);
                else if(ext->state == LV_BTN_STATE_ON_PRESSED) lv_btn_set_state(btn, LV_BTN_STATE_ON_RELEASED);
            }
        }
        else if(sign == LV_SIGNAL_RELEASED) {
            /*If not dragged and it was not long press action then
             *change state and run the action*/
            if(lv_indev_is_dragging(param) == false && ext->long_press_action_executed == 0) {
                if(ext->state == LV_BTN_STATE_OFF_PRESSED && tgl == false) {
                    lv_btn_set_state(btn, LV_BTN_STATE_OFF_RELEASED);
                } else if(ext->state == LV_BTN_STATE_ON_PRESSED && tgl == false) {
                    lv_btn_set_state(btn, LV_BTN_STATE_ON_RELEASED);
                } else if(ext->state == LV_BTN_STATE_OFF_PRESSED && tgl == true) {
                    lv_btn_set_state(btn, LV_BTN_STATE_ON_RELEASED);
                } else if(ext->state == LV_BTN_STATE_ON_PRESSED && tgl == true) {
                    lv_btn_set_state(btn, LV_BTN_STATE_OFF_RELEASED);
                }

                if(ext->actions[LV_BTN_ACTION_RELEASE] && state != LV_BTN_STATE_INACTIVE) {
                    valid = ext->actions[LV_BTN_ACTION_RELEASE](btn);
                }
            } else { /*If dragged change back the state*/
                if(ext->state == LV_BTN_STATE_OFF_PRESSED) {
                    lv_btn_set_state(btn, LV_BTN_STATE_OFF_RELEASED);
                } else if(ext->state == LV_BTN_STATE_ON_PRESSED) {
                    lv_btn_set_state(btn, LV_BTN_STATE_ON_RELEASED);
                }
            }
        }
        else if(sign == LV_SIGNAL_LONG_PRESS) {
                if(ext->actions[LV_BTN_ACTION_LONG_PRESS] && state != LV_BTN_STATE_INACTIVE) {
                	ext->long_press_action_executed = 1;
                	valid = ext->actions[LV_BTN_ACTION_LONG_PRESS](btn);
                }
        }
        else if(sign == LV_SIGNAL_LONG_PRESS_REP) {
            if(ext->actions[LV_BTN_ACTION_LONG_PRESS_REPEATE] && state != LV_BTN_STATE_INACTIVE) {
                valid = ext->actions[LV_BTN_ACTION_LONG_PRESS_REPEATE](btn);
            }
        }
        else if(sign == LV_SIGNAL_CONTROLL) {
            char c = *((char*)param);
            if(c == LV_GROUP_KEY_RIGHT || c == LV_GROUP_KEY_UP) {
                if(lv_btn_get_toggle(btn) != false) lv_btn_set_state(btn, LV_BTN_STATE_ON_RELEASED);
                if(ext->actions[LV_BTN_ACTION_RELEASE] && lv_btn_get_state(btn) != LV_BTN_STATE_INACTIVE) {
                    valid = ext->actions[LV_BTN_ACTION_RELEASE];
                }
            } else if(c == LV_GROUP_KEY_LEFT || c == LV_GROUP_KEY_DOWN) {
                if(lv_btn_get_toggle(btn) != false) lv_btn_set_state(btn, LV_BTN_STATE_OFF_RELEASED);
                if(ext->actions[LV_BTN_ACTION_RELEASE] && lv_btn_get_state(btn) != LV_BTN_STATE_INACTIVE) {
                    valid = ext->actions[LV_BTN_ACTION_RELEASE](btn);
                }
            } else if(c == LV_GROUP_KEY_ENTER) {
                if(lv_btn_get_toggle(btn) != false) {
                    lv_btn_state_t state = lv_btn_get_state(btn);
                    if(state == LV_BTN_STATE_OFF_RELEASED) lv_btn_set_state(btn, LV_BTN_STATE_ON_RELEASED);
                    else if(state == LV_BTN_STATE_OFF_PRESSED) lv_btn_set_state(btn, LV_BTN_STATE_ON_PRESSED);
                    else if(state == LV_BTN_STATE_ON_RELEASED) lv_btn_set_state(btn, LV_BTN_STATE_OFF_RELEASED);
                    else if(state == LV_BTN_STATE_ON_PRESSED) lv_btn_set_state(btn, LV_BTN_STATE_OFF_PRESSED);
                }
                if(ext->actions[LV_BTN_ACTION_RELEASE] && lv_btn_get_state(btn) != LV_BTN_STATE_INACTIVE) {
                    valid = ext->actions[LV_BTN_ACTION_RELEASE](btn);
                }
            }
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
void lv_btn_set_toggle(lv_obj_t * btn, bool tgl)
{
    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);
    
    ext->toggle = tgl != false ? 1 : 0;
}

/**
 * Set the state of the button
 * @param btn pointer to a button object
 * @param state the new state of the button (from lv_btn_state_t enum)
 */
void lv_btn_set_state(lv_obj_t * btn, lv_btn_state_t state)
{
    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);
    if(ext->state != state) {
        ext->state = state;
        lv_obj_set_style(btn, ext->styles[state]);
    }
}

/**
 * Toggle the state of the button (ON->OFF, OFF->ON)
 * @param btn pointer to a button object
 */
void lv_btn_toggle(lv_obj_t * btn)
{
    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);
    switch(ext->state) {
        case LV_BTN_STATE_OFF_RELEASED: lv_btn_set_state(btn, LV_BTN_STATE_ON_RELEASED); break;
        case LV_BTN_STATE_OFF_PRESSED: lv_btn_set_state(btn, LV_BTN_STATE_ON_PRESSED); break;
        case LV_BTN_STATE_ON_RELEASED: lv_btn_set_state(btn, LV_BTN_STATE_OFF_RELEASED); break;
        case LV_BTN_STATE_ON_PRESSED: lv_btn_set_state(btn, LV_BTN_STATE_OFF_PRESSED); break;
        default: break;
    }
}

/**
 * Set a function to call when the button is pressed
 * @param btn pointer to a button object
 * @param pr_action pointer to function
 */
void lv_btn_set_action(lv_obj_t * btn, lv_btn_action_t type, lv_action_t action)
{
    if(type >= LV_BTN_ACTION_NUM) return;

    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);
    ext->actions[type] = action;
}
/**
 * Set styles of a button is each state. Use NULL for any style which are not be changed.
 * @param btn pointer to button object
 * @param rel pointer to a style for releases state
 * @param pr  pointer to a style for pressed state
 * @param trel pointer to a style for toggled releases state
 * @param tpr pointer to a style for toggled pressed state
 * @param ina pointer to a style for inactive state
 */
void lv_btn_set_style(lv_obj_t * btn, lv_btn_state_t state, lv_style_t * style)
{
    if(state >= LV_BTN_STATE_NUM) return;

    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);
    ext->styles[state] = style;

    if(ext->state == state) lv_obj_set_style(btn, ext->styles[ext->state]);
}


void lv_btn_set_style_all(lv_obj_t * btn, lv_style_t ** style_array)
{
    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);
    memcpy(ext->styles, style_array, sizeof(ext->styles));

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
    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);
    return ext->state;
}

/**
 * Get the toggle enable attribute of the button
 * @param btn pointer to a button object
 * @return ture: toggle enabled, false: disabled
 */
bool lv_btn_get_toggle(lv_obj_t * btn)
{
    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);
    
    return ext->toggle != 0 ? true : false;
}

/**
 * Get the release action of a button
 * @param btn pointer to a button object
 * @return pointer to the release action function
 */
lv_action_t lv_btn_get_action(lv_obj_t * btn, lv_btn_action_t type)
{
    if(type >= LV_BTN_ACTION_NUM) return NULL;

    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);
    return ext->actions[type];
}

/**
 * Get the style of a button in a given state
 * @param btn pointer to a button object
 * @param state a state from 'lv_btn_state_t' in which style should be get
 * @return pointer to the style in the given state
 */
lv_style_t * lv_btn_get_style(lv_obj_t * btn, lv_btn_state_t state)
{
    if(state >= LV_BTN_STATE_NUM) return lv_obj_get_style(lv_obj_get_parent(btn));

    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);
    if(ext->styles[state] == NULL) return lv_obj_get_style(lv_obj_get_parent(btn));
    return ext->styles[state];
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#if 0
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
        return false;

    } else if(mode == LV_DESIGN_DRAW_MAIN || mode == LV_DESIGN_DRAW_POST) {

    }
    return true;
}
#endif
#endif
