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
static bool lv_btn_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);
static void lv_btn_style_load(lv_obj_t * obj_dp, lv_rects_t * rects_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/*----------------- 
 * Style definition
 *-----------------*/
static lv_btns_t lv_btns_def =
{
	.mcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0x40, 0x60, 0x80),
	.gcolor[LV_BTN_STATE_REL] = COLOR_BLACK,
	.bcolor[LV_BTN_STATE_REL] = COLOR_WHITE,

	.mcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0x60, 0x80, 0xa0),
	.gcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0x20, 0x30, 0x40),
	.bcolor[LV_BTN_STATE_PR] = COLOR_WHITE,

	.mcolor[LV_BTN_STATE_TGL_REL] = COLOR_MAKE(0x80,0x00,0x00),
	.gcolor[LV_BTN_STATE_TGL_REL] = COLOR_MAKE(0x20, 0x20, 0x20),
	.bcolor[LV_BTN_STATE_TGL_REL] = COLOR_WHITE,

	.mcolor[LV_BTN_STATE_TGL_PR] = COLOR_MAKE(0xf0, 0x26, 0x26),
	.gcolor[LV_BTN_STATE_TGL_PR] = COLOR_MAKE(0x40, 0x40, 0x40),
	.bcolor[LV_BTN_STATE_TGL_PR] = COLOR_WHITE,

	.mcolor[LV_BTN_STATE_INA] = COLOR_SILVER,
	.gcolor[LV_BTN_STATE_INA] = COLOR_GRAY,
	.bcolor[LV_BTN_STATE_INA] = COLOR_WHITE,

	.rects.bwidth = 2 * LV_STYLE_MULT,
	.rects.bopa = 50,
	.rects.empty = 0,
	.rects.round = 4 * LV_STYLE_MULT,
	.rects.hpad = 10 * LV_STYLE_MULT,
	.rects.vpad = 15 * LV_STYLE_MULT,
	.rects.opad = 3 * LV_STYLE_MULT,
};
static lv_btns_t lv_btns_transp =
{
	.rects.objs.transp = 1,
	.rects.bwidth = 0,
	.rects.empty = 1,
	.rects.hpad = 10 * LV_STYLE_MULT,
	.rects.vpad = 15 * LV_STYLE_MULT,
	.rects.opad = 5 * LV_STYLE_MULT,
};

static lv_btns_t lv_btns_border =
{
	.bcolor[LV_BTN_STATE_REL] = COLOR_BLACK,
	.bcolor[LV_BTN_STATE_PR] = COLOR_BLACK,
	.bcolor[LV_BTN_STATE_TGL_REL] = COLOR_BLACK,
	.bcolor[LV_BTN_STATE_TGL_PR] = COLOR_BLACK,
	.bcolor[LV_BTN_STATE_INA] = COLOR_GRAY,
	.rects.bwidth = 2 * LV_STYLE_MULT,
	.rects.empty = 1,
	.rects.bopa = 50,
	.rects.round = 4 * LV_STYLE_MULT,
	.rects.hpad = 10 * LV_STYLE_MULT,
	.rects.vpad = 10 * LV_STYLE_MULT,
	.rects.vpad = 5 * LV_STYLE_MULT,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a button objects
 * @param par_dp pointer to an object, it will be the parent of the new button
 * @param copy_dp pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
lv_obj_t* lv_btn_create(lv_obj_t* par_dp, lv_obj_t * copy_dp)
{
    lv_obj_t* new_obj_dp;
    
    new_obj_dp = lv_rect_create(par_dp, copy_dp);
    /*Allocate the extended data*/
    lv_obj_alloc_ext(new_obj_dp, sizeof(lv_btn_ext_t));
    lv_obj_set_signal_f(new_obj_dp, lv_btn_signal);
    lv_obj_set_design_f(new_obj_dp, lv_btn_design);
    
    lv_btn_ext_t * btn_ext_dp = lv_obj_get_ext(new_obj_dp);
    btn_ext_dp->lpr_exec = 0;

    /*If no copy do the basic initialization*/
    if(copy_dp == NULL)
    {
		btn_ext_dp->state = LV_BTN_STATE_REL;
		btn_ext_dp->pr_action = NULL;
		btn_ext_dp->rel_action = NULL;
		btn_ext_dp->lpr_action = NULL;
		btn_ext_dp->tgl = 0;
	    lv_obj_set_style(new_obj_dp, &lv_btns_def);
	    lv_rect_set_layout(new_obj_dp, LV_RECT_LAYOUT_CENTER);
    }
    /*Copy 'copy_dp'*/
    else{
    	lv_btn_ext_t * ori_btn_ext = lv_obj_get_ext(copy_dp);
    	btn_ext_dp->state = ori_btn_ext->state;
    	btn_ext_dp->pr_action = ori_btn_ext->pr_action;
    	btn_ext_dp->rel_action = ori_btn_ext->rel_action;
    	btn_ext_dp->lpr_action = ori_btn_ext->lpr_action;
    	btn_ext_dp->tgl = ori_btn_ext->tgl;
    }
    
    return new_obj_dp;
}

/**
 * Signal function of the button
 * @param obj_dp pointer to a button object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_btn_signal(lv_obj_t * obj_dp, lv_signal_t sign, void* param)
{   
    bool valid;

    /* Include the ancient signal function */
    valid = lv_rect_signal(obj_dp, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	lv_btn_state_t state = lv_btn_get_state(obj_dp);
    	lv_btn_ext_t * btn_ext_dp = lv_obj_get_ext(obj_dp);
        bool tgl = lv_btn_get_tgl(obj_dp);

        switch (sign){
            case LV_SIGNAL_PRESSED:
                /*Refresh the state*/
                if(btn_ext_dp->state == LV_BTN_STATE_REL) {
                	lv_btn_set_state(obj_dp, LV_BTN_STATE_PR);
                } else if(btn_ext_dp->state == LV_BTN_STATE_TGL_REL) {
                	lv_btn_set_state(obj_dp, LV_BTN_STATE_TGL_PR);
                }
                lv_obj_inv(obj_dp);

                btn_ext_dp->lpr_exec = 0;
                /*Call the press action, here 'param' is the caller dispi*/
                if(btn_ext_dp->pr_action != NULL && state != LV_BTN_STATE_INA) {
                	valid = btn_ext_dp->pr_action(obj_dp, param);
                }
                break;

            case LV_SIGNAL_PRESS_LOST:
                /*Refresh the state*/
                if(btn_ext_dp->state == LV_BTN_STATE_PR) {
                	lv_btn_set_state(obj_dp, LV_BTN_STATE_REL);
                } else if(btn_ext_dp->state == LV_BTN_STATE_TGL_PR) {
                	lv_btn_set_state(obj_dp, LV_BTN_STATE_TGL_REL);
                }
                lv_obj_inv(obj_dp);
                break;

            case LV_SIGNAL_RELEASED:
                /*If not dragged and it was not long press action then
                 *change state and run the action*/
                if(lv_dispi_is_dragging(param) == false && btn_ext_dp->lpr_exec == 0) {
                    if(btn_ext_dp->state == LV_BTN_STATE_PR && tgl == false) {
                    	lv_btn_set_state(obj_dp, LV_BTN_STATE_REL);
                    } else if(btn_ext_dp->state == LV_BTN_STATE_TGL_PR && tgl == false) {
                    	lv_btn_set_state(obj_dp, LV_BTN_STATE_TGL_REL);
                    } else if(btn_ext_dp->state == LV_BTN_STATE_PR && tgl == true) {
                    	lv_btn_set_state(obj_dp, LV_BTN_STATE_TGL_REL);
                    } else if(btn_ext_dp->state == LV_BTN_STATE_TGL_PR && tgl == true) {
                    	lv_btn_set_state(obj_dp, LV_BTN_STATE_REL);
                    }


                    if(btn_ext_dp->rel_action != NULL && state != LV_BTN_STATE_INA) {
                    	valid = btn_ext_dp->rel_action(obj_dp, param);
                    }
                } else { /*If dragged change back the state*/
                    if(btn_ext_dp->state == LV_BTN_STATE_PR) {
                        lv_btn_set_state(obj_dp, LV_BTN_STATE_REL);
                    } else if(btn_ext_dp->state == LV_BTN_STATE_TGL_PR) {
                    	lv_btn_set_state(obj_dp, LV_BTN_STATE_TGL_REL);
                    }
                }

                lv_obj_inv(obj_dp);
                break;
            case LV_SIGNAL_LONG_PRESS:
                /*Call the long press action, here 'param' is the caller dispi*/
                if(btn_ext_dp->lpr_action != NULL && state != LV_BTN_STATE_INA) {
                	 btn_ext_dp->lpr_exec = 1;
                	valid = btn_ext_dp->lpr_action(obj_dp, param);
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
 * @param obj_dp pointer to a button object
 * @param tgl true: enable toggled states, false: disable
 */
void lv_btn_set_tgl(lv_obj_t* obj_dp, bool tgl)
{
    lv_btn_ext_t * btn_p = lv_obj_get_ext(obj_dp);
    
    btn_p->tgl = tgl != false ? 1 : 0;
}

/**
 * Set the state of the button
 * @param obj_dp pointer to a button object
 * @param state the new state of the button (from lv_btn_state_t enum)
 */
void lv_btn_set_state(lv_obj_t* obj_dp, lv_btn_state_t state)
{
    lv_btn_ext_t * btn_p = lv_obj_get_ext(obj_dp);
    
    btn_p->state = state;
    lv_obj_inv(obj_dp);
}

/**
 * Set a function to call when the button is pressed
 * @param obj_dp pointer to a button object
 * @param pr_action pointer to function
 */
void lv_btn_set_pr_action(lv_obj_t* obj_dp, bool (*pr_action)(lv_obj_t*, lv_dispi_t *))
{
    lv_btn_ext_t * btn_p = lv_obj_get_ext(obj_dp);
    
    btn_p->pr_action = pr_action;
}

/**
 * Set a function to call when the button is released
 * @param obj_dp pointer to a button object
 * @param pr_action pointer to function
 */
void lv_btn_set_rel_action(lv_obj_t* obj_dp, bool (*rel_action)(lv_obj_t*, lv_dispi_t *))
{
    lv_btn_ext_t * btn_p = lv_obj_get_ext(obj_dp);
    
    btn_p->rel_action = rel_action;
}

/**
 * Set a function to call when the button is long pressed
 * @param obj_dp pointer to a button object
 * @param pr_action pointer to function
 */
void lv_btn_set_lpr_action(lv_obj_t* obj_dp, bool (*lpr_action)(lv_obj_t*, lv_dispi_t *))
{
    lv_btn_ext_t * btn_p = lv_obj_get_ext(obj_dp);
    
    btn_p->lpr_action = lpr_action;
}

/*=====================
 * Getter functions 
 *====================*/

/**
 * Get the current state of the button
 * @param obj_dp pointer to a button object
 * @return the state of the button (from lv_btn_state_t enum)
 */
lv_btn_state_t lv_btn_get_state(lv_obj_t* obj_dp)
{
    lv_btn_ext_t * btn_p = lv_obj_get_ext(obj_dp);
    
    return btn_p->state;
}

/**
 * Get the toggle enable attribute of the button
 * @param obj_dp pointer to a button object
 * @return ture: toggle enabled, false: disabled
 */
bool lv_btn_get_tgl(lv_obj_t* obj_dp)
{
    lv_btn_ext_t * btn_p = lv_obj_get_ext(obj_dp);
    
    return btn_p->tgl != 0 ? true : false;
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_btns_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_btns_t style
 */
lv_btns_t * lv_btns_get(lv_btns_builtin_t style, lv_btns_t * copy_p)
{
	lv_btns_t  *style_p;

	switch(style) {
		case LV_BTNS_DEF:
			style_p = &lv_btns_def;
			break;
		case LV_BTNS_TRANSP:
			style_p = &lv_btns_transp;
			break;
		case LV_BTNS_BORDER:
			style_p = &lv_btns_border;
			break;
		default:
			style_p = &lv_btns_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_btns_t));
		else memcpy(copy_p, &lv_btns_def, sizeof(lv_btns_t));
	}

	return style_p;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the buttons
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'        
 */
static bool lv_btn_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    lv_btns_t * btns_p = lv_obj_get_style(obj_dp);

    /* Because of the radius it is not sure the area is covered*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	uint16_t r = btns_p->rects.round;
    	area_t area_tmp;

    	/*Check horizontally without radius*/
    	lv_obj_get_cords(obj_dp, &area_tmp);
    	area_tmp.x1 += r;
    	area_tmp.x2 -= r;
    	if(area_is_in(mask_p, &area_tmp) == true) return true;

    	/*Check vertically without radius*/
    	lv_obj_get_cords(obj_dp, &area_tmp);
    	area_tmp.y1 += r;
    	area_tmp.y2 -= r;
    	if(area_is_in(mask_p, &area_tmp) == true) return true;

    	return false;
    }

    opa_t opa = lv_obj_get_opa(obj_dp);
    area_t area;
    lv_obj_get_cords(obj_dp, &area);

    lv_rects_t rects_tmp;

    lv_btn_style_load(obj_dp, &rects_tmp);

    /*Draw the rectangle*/
    lv_draw_rect(&area, mask_p, &rects_tmp, opa);
    
    return true;
}

/**
 * Load the corresponding style according to the state to 'rects' in 'lv_btns_t'
 * @param obj_dp pointer to a button object
 */
static void lv_btn_style_load(lv_obj_t * obj_dp, lv_rects_t * rects_p)
{
    lv_btn_state_t state = lv_btn_get_state(obj_dp);
    lv_btns_t * btns_p = lv_obj_get_style(obj_dp);

    /*Load the style*/
    memcpy(rects_p, &btns_p->rects, sizeof(lv_rects_t));
    rects_p->objs.color = btns_p->mcolor[state];
    rects_p->gcolor = btns_p->gcolor[state];
    rects_p->bcolor = btns_p->bcolor[state];
}

#endif
