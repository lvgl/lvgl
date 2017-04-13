/**
 * @file lv_btn.h
 * 
 */

#ifndef LV_BTN_H
#define LV_BTN_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_BTN != 0

/*Testing of dependencies*/
#if USE_LV_RECT == 0
#error "lv_btn: lv_rect is required. Enable it in lv_conf.h (USE_LV_RECT  1) "
#endif

#include <lvgl/lv_objx/lv_cont.h>
#include "../lv_obj/lv_dispi.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Button states*/
typedef enum
{
    LV_BTN_STATE_REL,
    LV_BTN_STATE_PR,
    LV_BTN_STATE_TREL,
    LV_BTN_STATE_TPR,
    LV_BTN_STATE_INA,
    LV_BTN_STATE_NUM,
}lv_btn_state_t;

/*Data of button*/
typedef struct
{
	lv_cont_ext_t cont; /*Ext. of ancestor*/
	/*New data for this type */
	lv_action_t pr_action;      /*A function to call when the button is pressed (NULL if unused)*/
	lv_action_t rel_action;     /*A function to call when the button is released (NULL if unused)*/
	lv_action_t lpr_action;     /*A function to call when the button is long pressed (NULL if unused)*/
	lv_action_t lpr_rep_action; /*A function to call periodically after long press (NULL if unused)*/

	lv_style_t * styles[LV_BTN_STATE_NUM];    /*Styles in each state*/

    lv_btn_state_t state;       /*Current state of the button from 'lv_btn_state_t' enum*/
    uint8_t tgl :1;             /*1: Toggle enabled*/
    uint8_t lpr_exec :1;        /*1: Long press action executed (Handled by the library)*/
}lv_btn_ext_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a button objects
 * @param par pointer to an object, it will be the parent of the new button
 * @param copy pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
lv_obj_t * lv_btn_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the button
 * @param btn pointer to a button object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_btn_signal(lv_obj_t * btn, lv_signal_t sign, void * param);

/**
 * Enable the toggled states
 * @param btn pointer to a button object
 * @param tgl true: enable toggled states, false: disable
 */
void lv_btn_set_tgl(lv_obj_t * btn, bool tgl);

/**
 * Set the state of the button
 * @param btn pointer to a button object
 * @param state the new state of the button (from lv_btn_state_t enum)
 */
void lv_btn_set_state(lv_obj_t * btn, lv_btn_state_t state);

/**
 * Set a function to call when the button is pressed
 * @param btn pointer to a button object
 * @param pr_action pointer to function
 */
void lv_btn_set_pr_action(lv_obj_t * btn, lv_action_t pr_action);

/**
 * Set a function to call when the button is released
 * @param btn pointer to a button object
 * @param rel_action pointer to functionREL
 */
void lv_btn_set_rel_action(lv_obj_t * btn, lv_action_t rel_action);

/**
 * Set a function to call when the button is long pressed
 * @param btn pointer to a button object
 * @param lpr_action pointer to function
 */
void lv_btn_set_lpr_action(lv_obj_t * btn, lv_action_t lpr_action);

/**
 * Set a function to called periodically after long press.
 * @param btn pointer to a button object
 * @param lpr_rep_action pointer to function
 */
void lv_btn_set_lpr_rep_action(lv_obj_t * btn, lv_action_t lpr_rep_action);

void lv_btn_set_styles(lv_obj_t * btn, lv_style_t * rel, lv_style_t * pr, lv_style_t * trel, lv_style_t * tpr, lv_style_t * ina);

/**
 * Get the current state of the button
 * @param btn pointer to a button object
 * @return the state of the button (from lv_btn_state_t enum)
 */
lv_btn_state_t lv_btn_get_state(lv_obj_t * btn);

/**
 * Get the toggle enable attribute of the button
 * @param btn pointer to a button object
 * @return ture: toggle enabled, false: disabled
 */
bool lv_btn_get_tgl(lv_obj_t * btn);

lv_style_t * lv_btn_get_style(lv_obj_t * btn, lv_btn_state_t state);
/**********************
 *      MACROS
 **********************/

#endif

#endif
