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

static void lv_btns_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_btns_t lv_btns_def;
static lv_btns_t lv_btns_border;
static lv_btns_t lv_btns_transp;

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
    ext->lpr_exec = 0;
    ext->tgl = 0;

    if(ancestor_design_f  == NULL) ancestor_design_f = lv_obj_get_design_f(new_btn);

    lv_obj_set_signal_f(new_btn, lv_btn_signal);
    lv_obj_set_design_f(new_btn, lv_btn_design);
    
    /*If no copy do the basic initialization*/
    if(copy == NULL) {
	    lv_rect_set_layout(new_btn, LV_RECT_LAYOUT_CENTER);
	    lv_obj_set_style(new_btn, lv_btns_get(LV_BTNS_DEF, NULL));
    }
    /*Copy 'copy'*/
    else {
    	lv_btn_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->state = copy_ext->state;
    	ext->pr_action = copy_ext->pr_action;
    	ext->rel_action = copy_ext->rel_action;
    	ext->lpr_action = copy_ext->lpr_action;
        ext->lpr_rep_action = copy_ext->lpr_action;
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

    /*On style change preload the style for the rectangle signal*/
    lv_btn_ext_t * ext = lv_obj_get_ext(btn);
    lv_btns_t * style = lv_obj_get_style(btn);
    if(sign == LV_SIGNAL_STYLE_CHG) {
        memcpy(&style->current, &style->state_style[ext->state], sizeof(lv_rects_t));
    }

    /* Include the ancient signal function */
    valid = lv_rect_signal(btn, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
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
        lv_btns_t * style = lv_obj_get_style(btn);
        memcpy(&style->current, &style->state_style[ext->state], sizeof(lv_rects_t));
        lv_obj_refr_style(btn);
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
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_btns_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_btns_t style
 */
lv_btns_t * lv_btns_get(lv_btns_builtin_t style, lv_btns_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_btns_init();
		style_inited = true;
	}

	lv_btns_t * style_p;

	switch(style) {
		case LV_BTNS_DEF:
			style_p = &lv_btns_def;
			break;
        case LV_BTNS_BORDER:
            style_p = &lv_btns_border;
            break;
		case LV_BTNS_TRANSP:
			style_p = &lv_btns_transp;
			break;
		default:
			style_p = &lv_btns_def;
	}

	if(copy != NULL) memcpy(copy, style_p, sizeof(lv_btns_t));

	return style_p;
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
        /*Temporally set a rectangle style for the button to look like as rectangle*/
        lv_rects_t rects_tmp;
        lv_btns_t * btns_tmp = lv_obj_get_style(btn);
        lv_btn_ext_t * ext = lv_obj_get_ext(btn);
        bool ret = false;
        memcpy(&rects_tmp, &btns_tmp->state_style[ext->state], sizeof(lv_rects_t));
        btn->style_p = &rects_tmp;
        ret = ancestor_design_f(btn, mask, mode); /*Draw the rectangle*/
        btn->style_p = btns_tmp;            /*Reload the original button style*/

    	return ret;
    } else if(mode == LV_DESIGN_DRAW_MAIN || mode == LV_DESIGN_DRAW_POST) {
		area_t area;
		lv_obj_get_cords(btn, &area);

		/*Temporally  set a rectangle style for the button to draw it as rectangle*/
		lv_rects_t rects_tmp;
		lv_btns_t * btns_tmp = lv_obj_get_style(btn);
		lv_btn_ext_t * ext = lv_obj_get_ext(btn);
        memcpy(&rects_tmp, &btns_tmp->state_style[ext->state], sizeof(lv_rects_t));
        btn->style_p = &rects_tmp;
        ancestor_design_f(btn, mask, mode);	/*Draw the rectangle*/
        btn->style_p = btns_tmp;			/*Reload the original button style*/

    }
    return true;
}



/**
 * Initialize the button styles
 */
static void lv_btns_init(void)
{

    /*Default style*/
    lv_rects_get(LV_RECTS_FANCY, &lv_btns_def.state_style[LV_BTN_STATE_REL]);
    lv_btns_def.state_style[LV_BTN_STATE_REL].base.color = COLOR_MAKE(0xcc, 0xe0, 0xf5);
    lv_btns_def.state_style[LV_BTN_STATE_REL].gcolor = COLOR_MAKE(0xa6, 0xc9, 0xed);
    lv_btns_def.state_style[LV_BTN_STATE_REL].bcolor = COLOR_MAKE(0x33, 0x99, 0xff);
    lv_rects_get(LV_RECTS_FANCY, &lv_btns_def.state_style[LV_BTN_STATE_PR]);
    lv_btns_def.state_style[LV_BTN_STATE_PR].base.color =  COLOR_MAKE(0xa6, 0xc9, 0xed);
    lv_btns_def.state_style[LV_BTN_STATE_PR].gcolor = COLOR_MAKE(0x60, 0x88, 0xb0);
    lv_btns_def.state_style[LV_BTN_STATE_PR].bcolor = COLOR_MAKE(0x33, 0x99, 0xff);
    lv_btns_def.state_style[LV_BTN_STATE_PR].swidth = 3 * lv_btns_def.state_style[LV_BTN_STATE_REL].swidth / 4;
    lv_rects_get(LV_RECTS_FANCY, &lv_btns_def.state_style[LV_BTN_STATE_TREL]);
    lv_btns_def.state_style[LV_BTN_STATE_TREL].base.color = COLOR_MAKE(0xff, 0xed, 0xd3);
    lv_btns_def.state_style[LV_BTN_STATE_TREL].gcolor = COLOR_MAKE(0xfc, 0xc7, 0x7a);
    lv_btns_def.state_style[LV_BTN_STATE_TREL].bcolor = COLOR_MAKE(0xff, 0x90, 0x00);
    lv_rects_get(LV_RECTS_FANCY, &lv_btns_def.state_style[LV_BTN_STATE_TPR]);
    lv_btns_def.state_style[LV_BTN_STATE_TPR].base.color = COLOR_MAKE(0xfc, 0xc7, 0x7a);
    lv_btns_def.state_style[LV_BTN_STATE_TPR].gcolor = COLOR_MAKE(0xdd, 0x8a, 0x4e);
    lv_btns_def.state_style[LV_BTN_STATE_TPR].bcolor = COLOR_MAKE(0xff, 0x90, 0x00);
    lv_btns_def.state_style[LV_BTN_STATE_TPR].swidth = 3 * lv_btns_def.state_style[LV_BTN_STATE_TREL].swidth / 4;
    lv_rects_get(LV_RECTS_FANCY, &lv_btns_def.state_style[LV_BTN_STATE_INA]);
    lv_btns_def.state_style[LV_BTN_STATE_INA].base.color = COLOR_MAKE(0xe0, 0xe0, 0xe0);
    lv_btns_def.state_style[LV_BTN_STATE_INA].gcolor = lv_btns_def.state_style[LV_BTN_STATE_INA].base.color;
    lv_btns_def.state_style[LV_BTN_STATE_INA].bcolor = COLOR_MAKE(0x80, 0x80, 0x80);


    /*Border style*/
    lv_rects_get(LV_RECTS_BORDER, &lv_btns_border.state_style[LV_BTN_STATE_REL]);
    lv_btns_border.state_style[LV_BTN_STATE_REL].bcolor = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_rects_get(LV_RECTS_BORDER, &lv_btns_border.state_style[LV_BTN_STATE_PR]);
    lv_btns_border.state_style[LV_BTN_STATE_PR].bcolor = COLOR_MAKE(0x60, 0x60, 0x60);
    lv_rects_get(LV_RECTS_BORDER, &lv_btns_border.state_style[LV_BTN_STATE_TREL]);
    lv_btns_border.state_style[LV_BTN_STATE_TREL].bcolor = COLOR_MAKE(0x00, 0x33, 0xff);
    lv_rects_get(LV_RECTS_BORDER, &lv_btns_border.state_style[LV_BTN_STATE_TPR]);
    lv_btns_border.state_style[LV_BTN_STATE_TPR].bcolor = COLOR_MAKE(0x00, 0x33, 0x99);
    lv_rects_get(LV_RECTS_BORDER, &lv_btns_border.state_style[LV_BTN_STATE_INA]);
    lv_btns_border.state_style[LV_BTN_STATE_INA].bcolor = COLOR_MAKE(0xa0, 0xa0, 0xa0);

    /*Transparent style*/
    lv_rects_get(LV_RECTS_TRANSP, &lv_btns_transp.state_style[LV_BTN_STATE_REL]);
    lv_rects_get(LV_RECTS_TRANSP, &lv_btns_transp.state_style[LV_BTN_STATE_PR]);
    lv_rects_get(LV_RECTS_TRANSP, &lv_btns_transp.state_style[LV_BTN_STATE_TREL]);
    lv_rects_get(LV_RECTS_TRANSP, &lv_btns_transp.state_style[LV_BTN_STATE_TPR]);
    lv_rects_get(LV_RECTS_TRANSP, &lv_btns_transp.state_style[LV_BTN_STATE_INA]);
}

#endif
