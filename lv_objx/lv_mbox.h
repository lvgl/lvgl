/**
 * @file lv_mbox.h
 * 
 */

#ifndef LV_MBOX_H
#define LV_MBOX_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_MBOX != 0

/*Testing of dependencies*/
#if USE_LV_CONT == 0
#error "lv_mbox: lv_cont is required. Enable it in lv_conf.h (USE_LV_CONT  1) "
#endif

#if USE_LV_BTN == 0
#error "lv_mbox: lv_btn is required. Enable it in lv_conf.h (USE_LV_BTN  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_mbox: lv_rlabel is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif


#include "../lv_obj/lv_obj.h"
#include <lvgl/lv_objx/lv_cont.h>
#include "lv_btn.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of message box*/
typedef struct
{
    lv_cont_ext_t bg; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * txt;             /*Text of the message box*/
    lv_obj_t * btnh;            /*Holder of the buttons*/
    lv_style_t * style_btn_rel; /*Style of the released buttons*/
    lv_style_t * style_btn_pr;  /*Style of the pressed buttons*/
}lv_mbox_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a message box objects
 * @param par pointer to an object, it will be the parent of the new message box
 * @param copy pointer to a message box object, if not NULL then the new object will be copied from it
 * @return pointer to the created message box
 */
lv_obj_t * lv_mbox_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the message box
 * @param mbox pointer to a message box object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_mbox_signal(lv_obj_t * mbox, lv_signal_t sign, void * param);

/**
 * A release action which can be assigned to a message box button to close it
 * @param btn pointer to the released button
 * @param dispi pointer to the caller display input
 * @return always lv_action_res_t because the button is deleted with the mesage box
 */
lv_action_res_t lv_mbox_close_action(lv_obj_t * btn, lv_dispi_t * dispi);

/**
 * Add a button to the message box
 * @param mbox pointer to message box object
 * @param btn_txt the text of the button
 * @param rel_action a function which will be called when the button is released
 * @return pointer to the created button (lv_btn)
 */
lv_obj_t * lv_mbox_add_btn(lv_obj_t * mbox, const char * btn_txt, lv_action_t rel_action);

/**
 * Set the text of the message box
 * @param mbox pointer to a message box
 * @param txt a '\0' terminated character string which will be the message box text
 */
void lv_mbox_set_text(lv_obj_t * mbox, const char * txt);

/**
 * Set styles of the buttons of a message box in each state
 * @param mbox pointer to a message box object
 * @param rel pointer to a style for releases state
 * @param pr  pointer to a style for pressed state
 * @param trel pointer to a style for toggled releases state
 * @param tpr pointer to a style for toggled pressed state
 * @param ina pointer to a style for inactive state
 */
void lv_mbox_set_styles_btn(lv_obj_t * mbox, lv_style_t * rel, lv_style_t * pr);

/**
 * Automatically delete the message box after a given time
 * @param mbox pointer to a message box object
 * @param tout a time (in milliseconds) to wait before delete the message box
 */
void lv_mbox_start_auto_close(lv_obj_t * mbox, uint16_t tout);

/**
 * Stop the auto. closing of message box
 * @param mbox pointer to a message box object
 */
void lv_mbox_stop_auto_close(lv_obj_t * mbox);

/**
 * Get the text of the message box
 * @param mbox pointer to a message box object
 * @return pointer to the text of the message box
 */
const char * lv_mbox_get_txt(lv_obj_t * mbox);

/**
 * Get the message box object from one of its button.
 * It is useful in the button release actions where only the button is known
 * @param btn pointer to a button of a message box
 * @return pointer to the button's message box
 */
lv_obj_t * lv_mbox_get_from_btn(lv_obj_t * btn);

/**
 * Get the style of the buttons on a message box
 * @param mbox pointer to a message box object
 * @param state a state from 'lv_btn_state_t' in which style should be get
 * @return pointer to the style in the given state
 */
lv_style_t * lv_mbox_get_style_btn(lv_obj_t * mbox, lv_btn_state_t state);


/**********************
 *      MACROS
 **********************/

#endif

#endif
