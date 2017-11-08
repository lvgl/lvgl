/**
 * @file lv_mbox.h
 * 
 */

#ifndef LV_MBOX_H
#define LV_MBOX_H

#ifdef __cplusplus
extern "C" {
#endif

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
#include "lv_cont.h"
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
    uint16_t anim_time;         /*Duration of close animation [ms] (0: no animation)*/
    cord_t btn_width;           /*Button width (0: to auto fit)*/
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
 * A release action which can be assigned to a message box button to close it
 * @param btn pointer to the released button
 * @return always lv_action_res_t because the button is deleted with the mesage box
 */
lv_res_t lv_mbox_close_action(lv_obj_t * btn);

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
 * Set the width of the buttons
 * @param mbox pointer to message box object
 * @param w width of the buttons or 0 to use auto fit
 */
void lv_mbox_set_btn_width(lv_obj_t *mbox, cord_t w);

/**
 * Set the styles of a message box
 * @param mbox pointer to a message box object
 * @param bg pointer to the new background style
 * @param btnh pointer to the new button holder style
 */
void lv_mbox_set_style(lv_obj_t *mbox, lv_style_t *bg, lv_style_t *btnh);

/**
 * Set styles of the buttons of a message box in each state
 * @param mbox pointer to a message box object
 * @param rel pointer to a style for releases state
 * @param pr  pointer to a style for pressed state
 */
void lv_mbox_set_style_btn(lv_obj_t * mbox, lv_style_t * rel, lv_style_t * pr);

/**
 * Set close animation duration
 * @param mbox pointer to a message box object
 * @param time animation length in  milliseconds (0: no animation)
 */
void lv_mbox_set_anim_time(lv_obj_t * mbox, uint16_t time);

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
const char * lv_mbox_get_text(lv_obj_t * mbox);

/**
 * Get width of the buttons
 * @param mbox pointer to a message box object
 * @return width of the buttons (0: auto fit enabled)
 */
cord_t lv_mbox_get_btn_width(lv_obj_t * mbox);

/**
 * Get the message box object from one of its button.
 * It is useful in the button release actions where only the button is known
 * @param btn pointer to a button of a message box
 * @return pointer to the button's message box
 */
lv_obj_t * lv_mbox_get_from_btn(lv_obj_t * btn);

/**
 * Get the close animation duration
 * @param mbox pointer to a message box object
 * @return animation length in  milliseconds (0: no animation)
 */
uint16_t lv_mbox_get_anim_time(lv_obj_t * mbox );

/**
 * Get the style of the buttons on a message box
 * @param mbox pointer to a message box object
 * @param state a state from 'lv_btn_state_t' in which style should be get
 * @return pointer to the style in the given state
 */
lv_style_t * lv_mbox_get_style_btn(lv_obj_t * mbox, lv_btn_state_t state);

/**
 * Get the style of a message box's button holder
 * @param mbox pointer to a message box object
 * @return pointer to the message box's background style
 */
lv_style_t * lv_mbox_get_style_btnh(lv_obj_t *mbox);

/******************************
 *  TRANSPARENT API FUNCTIONS
 ******************************/

/**
 * Get the style of a message box's background
 * @param mbox pointer to a message box object
 * @return pointer to the message box's background style
 */
static inline lv_style_t * lv_mbox_get_style_bg(lv_obj_t *mbox)
{
    return lv_obj_get_style(mbox);
}


/**********************
 *      MACROS
 **********************/


#endif  /*USE_LV_MBOX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_MBOX_H*/
