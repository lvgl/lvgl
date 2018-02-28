/**
 * @file lv_btn.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf.h"
#if USE_LV_BTN != 0

#include <string.h>
#include "lv_btn.h"
#include "../lv_core/lv_group.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_signal_func_t ancestor_signal;

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_res_t lv_btn_signal(lv_obj_t * btn, lv_signal_t sign, void * param);

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
    lv_mem_assert(new_btn);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_btn);

    /*Allocate the extended data*/
    lv_btn_ext_t * ext = lv_obj_allocate_ext_attr(new_btn, sizeof(lv_btn_ext_t));
    lv_mem_assert(ext);
    ext->state = LV_BTN_STATE_REL;

    ext->actions[LV_BTN_ACTION_PR] = NULL;
    ext->actions[LV_BTN_ACTION_CLICK] = NULL;
    ext->actions[LV_BTN_ACTION_LONG_PR] = NULL;
    ext->actions[LV_BTN_ACTION_LONG_PR_REPEAT] = NULL;

    ext->styles[LV_BTN_STATE_REL] = &lv_style_btn_rel;
    ext->styles[LV_BTN_STATE_PR] = &lv_style_btn_pr;
    ext->styles[LV_BTN_STATE_TGL_REL] = &lv_style_btn_tgl_rel;
    ext->styles[LV_BTN_STATE_TGL_PR] = &lv_style_btn_tgl_pr;
    ext->styles[LV_BTN_STATE_INA] = &lv_style_btn_ina;

    ext->long_pr_action_executed = 0;
    ext->toggle = 0;

    lv_obj_set_signal_func(new_btn, lv_btn_signal);
    
    /*If no copy do the basic initialization*/
    if(copy == NULL) {
        /*Set layout if the button is not a screen*/
        if(par != NULL) {
            lv_btn_set_layout(new_btn, LV_LAYOUT_CENTER);
        }

        lv_obj_set_click(new_btn, true);        /*Be sure the button is clickable*/

	    /*Set the default styles*/
        lv_theme_t *th = lv_theme_get_current();
        if(th) {
            lv_btn_set_style(new_btn, LV_BTN_STYLE_REL, th->btn.rel);
            lv_btn_set_style(new_btn, LV_BTN_STYLE_PR, th->btn.pr);
            lv_btn_set_style(new_btn, LV_BTN_STYLE_TGL_REL, th->btn.tgl_rel);
            lv_btn_set_style(new_btn, LV_BTN_STYLE_TGL_PR, th->btn.tgl_pr);
            lv_btn_set_style(new_btn, LV_BTN_STYLE_INA, th->btn.ina);
        } else {
            lv_obj_set_style(new_btn, ext->styles[LV_BTN_STATE_REL]);
        }
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
        case LV_BTN_STATE_REL: lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL); break;
        case LV_BTN_STATE_PR: lv_btn_set_state(btn, LV_BTN_STATE_TGL_PR); break;
        case LV_BTN_STATE_TGL_REL: lv_btn_set_state(btn, LV_BTN_STATE_REL); break;
        case LV_BTN_STATE_TGL_PR: lv_btn_set_state(btn, LV_BTN_STATE_PR); break;
        default: break;
    }
}

/**
 * Set a function to call when the button event happens
 * @param btn pointer to a button object
 * @param action type of event form 'lv_action_t' (press, release, long press, long press repeat)
 */
void lv_btn_set_action(lv_obj_t * btn, lv_btn_action_t type, lv_action_t action)
{
    if(type >= LV_BTN_ACTION_NUM) return;

    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);
    ext->actions[type] = action;
}

/**
 * Set a style of a button
 * @param btn pointer to a button object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_btn_set_style(lv_obj_t *btn, lv_btn_style_t type, lv_style_t *style)
{
    lv_btn_ext_t *ext = lv_obj_get_ext_attr(btn);

    switch (type) {
        case LV_BTN_STYLE_REL:
            ext->styles[LV_BTN_STATE_REL] = style;
            break;
        case LV_BTN_STYLE_PR:
            ext->styles[LV_BTN_STATE_PR] = style;
            break;
        case LV_BTN_STYLE_TGL_REL:
            ext->styles[LV_BTN_STATE_TGL_REL] = style;
            break;
        case LV_BTN_STYLE_TGL_PR:
            ext->styles[LV_BTN_STATE_TGL_PR] = style;
            break;
        case LV_BTN_STYLE_INA:
            ext->styles[LV_BTN_STATE_INA] = style;
            break;
    }

    /*Refresh the object with the new style*/
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
 * Get a style of a button
 * @param btn pointer to a button object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_btn_get_style(lv_obj_t *btn, lv_btn_style_t type)
{
    lv_btn_ext_t *ext = lv_obj_get_ext_attr(btn);

    switch (type) {
        case LV_BTN_STYLE_REL:     return ext->styles[LV_BTN_STATE_REL];
        case LV_BTN_STYLE_PR:      return ext->styles[LV_BTN_STATE_PR];
        case LV_BTN_STYLE_TGL_REL: return ext->styles[LV_BTN_STATE_TGL_REL];
        case LV_BTN_STYLE_TGL_PR:  return ext->styles[LV_BTN_STATE_TGL_PR];
        case LV_BTN_STYLE_INA:     return ext->styles[LV_BTN_STATE_INA];
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the button
 * @param btn pointer to a button object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_btn_signal(lv_obj_t * btn, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(btn, sign, param);
    if(res != LV_RES_OK) return res;

    lv_btn_ext_t * ext = lv_obj_get_ext_attr(btn);
    lv_btn_state_t state = lv_btn_get_state(btn);
    bool tgl = lv_btn_get_toggle(btn);

    if(sign == LV_SIGNAL_PRESSED) {
        /*Refresh the state*/
        if(ext->state == LV_BTN_STATE_REL) {
            lv_btn_set_state(btn, LV_BTN_STATE_PR);
        } else if(ext->state == LV_BTN_STATE_TGL_REL) {
            lv_btn_set_state(btn, LV_BTN_STATE_TGL_PR);
        }

        ext->long_pr_action_executed = 0;
        /*Call the press action, 'param' is the caller indev_proc*/
        if(ext->actions[LV_BTN_ACTION_PR] && state != LV_BTN_STATE_INA) {
            res = ext->actions[LV_BTN_ACTION_PR](btn);
        }
    }
    else if(sign ==  LV_SIGNAL_PRESS_LOST) {
        /*Refresh the state*/
        if(ext->state == LV_BTN_STATE_PR) lv_btn_set_state(btn, LV_BTN_STATE_REL);
        else if(ext->state == LV_BTN_STATE_TGL_PR) lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL);
    }
    else if(sign == LV_SIGNAL_PRESSING) {
        /*When the button begins to drag revert pressed states to released*/
        if(lv_indev_is_dragging(param) != false) {
            if(ext->state == LV_BTN_STATE_PR) lv_btn_set_state(btn, LV_BTN_STATE_REL);
            else if(ext->state == LV_BTN_STATE_TGL_PR) lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL);
        }
    }
    else if(sign == LV_SIGNAL_RELEASED) {
        /*If not dragged and it was not long press action then
         *change state and run the action*/
        if(lv_indev_is_dragging(param) == false && ext->long_pr_action_executed == 0) {
            if(ext->state == LV_BTN_STATE_PR && tgl == false) {
                lv_btn_set_state(btn, LV_BTN_STATE_REL);
            } else if(ext->state == LV_BTN_STATE_TGL_PR && tgl == false) {
                lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL);
            } else if(ext->state == LV_BTN_STATE_PR && tgl == true) {
                lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL);
            } else if(ext->state == LV_BTN_STATE_TGL_PR && tgl == true) {
                lv_btn_set_state(btn, LV_BTN_STATE_REL);
            }

            if(ext->actions[LV_BTN_ACTION_CLICK] && state != LV_BTN_STATE_INA) {
                res = ext->actions[LV_BTN_ACTION_CLICK](btn);
            }
        } else { /*If dragged change back the state*/
            if(ext->state == LV_BTN_STATE_PR) {
                lv_btn_set_state(btn, LV_BTN_STATE_REL);
            } else if(ext->state == LV_BTN_STATE_TGL_PR) {
                lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL);
            }
        }
    }
    else if(sign == LV_SIGNAL_LONG_PRESS) {
            if(ext->actions[LV_BTN_ACTION_LONG_PR] && state != LV_BTN_STATE_INA) {
                ext->long_pr_action_executed = 1;
                res = ext->actions[LV_BTN_ACTION_LONG_PR](btn);
            }
    }
    else if(sign == LV_SIGNAL_LONG_PRESS_REP) {
        if(ext->actions[LV_BTN_ACTION_LONG_PR_REPEAT] && state != LV_BTN_STATE_INA) {
            res = ext->actions[LV_BTN_ACTION_LONG_PR_REPEAT](btn);
        }
    }
    else if(sign == LV_SIGNAL_CONTROLL) {
        char c = *((char*)param);
        if(c == LV_GROUP_KEY_RIGHT || c == LV_GROUP_KEY_UP) {
            if(lv_btn_get_toggle(btn) != false) lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL);
            if(ext->actions[LV_BTN_ACTION_CLICK] && lv_btn_get_state(btn) != LV_BTN_STATE_INA) {
                res = ext->actions[LV_BTN_ACTION_CLICK](btn);
            }
        } else if(c == LV_GROUP_KEY_LEFT || c == LV_GROUP_KEY_DOWN) {
            if(lv_btn_get_toggle(btn) != false) lv_btn_set_state(btn, LV_BTN_STATE_REL);
            if(ext->actions[LV_BTN_ACTION_CLICK] && lv_btn_get_state(btn) != LV_BTN_STATE_INA) {
                res = ext->actions[LV_BTN_ACTION_CLICK](btn);
            }
        } else if(c == LV_GROUP_KEY_ENTER) {
            if(!ext->long_pr_action_executed) {
                if(lv_btn_get_toggle(btn)) {
                    if(state == LV_BTN_STATE_REL) lv_btn_set_state(btn, LV_BTN_STATE_TGL_REL);
                    else if(state == LV_BTN_STATE_PR) lv_btn_set_state(btn, LV_BTN_STATE_TGL_PR);
                    else if(state == LV_BTN_STATE_TGL_REL) lv_btn_set_state(btn, LV_BTN_STATE_REL);
                    else if(state == LV_BTN_STATE_TGL_PR) lv_btn_set_state(btn, LV_BTN_STATE_PR);
                }
                if(ext->actions[LV_BTN_ACTION_CLICK] && state != LV_BTN_STATE_INA) {
                    res = ext->actions[LV_BTN_ACTION_CLICK](btn);
                }
            }
            ext->long_pr_action_executed  = 0;
        }
        else if(c == LV_GROUP_KEY_ENTER_LONG) {
            if(ext->actions[LV_BTN_ACTION_LONG_PR] && state != LV_BTN_STATE_INA) {
                res = ext->actions[LV_BTN_ACTION_LONG_PR](btn);
                ext->long_pr_action_executed = 1;
            }
        }
    }
    else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_btn";
    }

    return res;
}


#endif
