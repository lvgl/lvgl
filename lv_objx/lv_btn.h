/**
 * @file lv_btn.h
 * 
 */

#ifndef LV_BTN_H
#define LV_BTN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_BTN != 0

/*Testing of dependencies*/
#if USE_LV_CONT == 0
#error "lv_btn: lv_cont is required. Enable it in lv_conf.h (USE_LV_CONT  1) "
#endif

#include "lv_cont.h"
#include "../lv_obj/lv_indev.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Button states*/
typedef enum
{
    LV_BTN_STATE_RELEASED,
    LV_BTN_STATE_PRESSED,
    LV_BTN_STATE_TGL_RELEASED,
    LV_BTN_STATE_TGL_PRESSED,
    LV_BTN_STATE_INACTIVE,
    LV_BTN_STATE_NUM,
}lv_btn_state_t;

typedef enum
{
    LV_BTN_ACTION_RELEASE,
    LV_BTN_ACTION_PRESS,
    LV_BTN_ACTION_LONG_PRESS,
    LV_BTN_ACTION_LONG_PRESS_REPEATE,
    LV_BTN_ACTION_NUM,
}lv_btn_action_t;

/*Data of button*/
typedef struct
{
	lv_cont_ext_t cont; /*Ext. of ancestor*/
	/*New data for this type */
	lv_action_t actions[LV_BTN_ACTION_NUM];
	lv_style_t * styles[LV_BTN_STATE_NUM];        /*Styles in each state*/

    lv_btn_state_t state;                         /*Current state of the button from 'lv_btn_state_t' enum*/
    uint8_t toggle :1;                            /*1: Toggle enabled*/
    uint8_t long_press_action_executed :1;        /*1: Long press action executed (Handled by the library)*/
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
void lv_btn_set_toggle(lv_obj_t * btn, bool tgl);

/**
 * Set the state of the button
 * @param btn pointer to a button object
 * @param state the new state of the button (from lv_btn_state_t enum)
 */
void lv_btn_set_state(lv_obj_t * btn, lv_btn_state_t state);

/**
 * Toggle the state of the button (ON->OFF, OFF->ON)
 * @param btn pointer to a button object
 */
void lv_btn_toggle(lv_obj_t * btn);

/**
 * Set a function to call when the button is pressed
 * @param btn pointer to a button object
 * @param pr_action pointer to function
 */
void lv_btn_set_action(lv_obj_t * btn, lv_btn_action_t type, lv_action_t action);

/**
 * Set styles of a button is each state. Use NULL for any style to leave it unchanged
 * @param btn pointer to button object
 * @param rel_style pointer to a style for releases state
 * @param pr_style  pointer to a style for pressed state
 * @param tgl_rel_style pointer to a style for toggled releases state
 * @param tgl_pr_style pointer to a style for toggled pressed state
 * @param inactive_style pointer to a style for inactive state
 */
void lv_btn_set_styles(lv_obj_t * btn, lv_style_t *rel_style, lv_style_t *pr_style,
                                       lv_style_t *tgl_rel_style, lv_style_t *tgl_pr_style,
                                       lv_style_t *inactive_style);

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
bool lv_btn_get_toggle(lv_obj_t * btn);

/**
 * Get the release action of a button
 * @param btn pointer to a button object
 * @return pointer to the release action function
 */
lv_action_t lv_btn_get_action(lv_obj_t * btn, lv_btn_action_t type);

/**
 * Get the style of a button in a given state
 * @param btn pointer to a button object
 * @param state a state from 'lv_btn_state_t' in which style should be get
 * @return pointer to the style in the given state
 */
lv_style_t * lv_btn_get_style(lv_obj_t * btn, lv_btn_state_t state);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_BUTTON*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*lv_btn_H*/
