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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#if USE_LV_MBOX != 0

/*Testing of dependencies*/
#if USE_LV_CONT == 0
#error "lv_mbox: lv_cont is required. Enable it in lv_conf.h (USE_LV_CONT  1) "
#endif

#if USE_LV_BTNM == 0
#error "lv_mbox: lv_btnm is required. Enable it in lv_conf.h (USE_LV_BTNM  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_mbox: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif


#include "../lv_core/lv_obj.h"
#include "lv_cont.h"
#include "lv_btnm.h"
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
    lv_obj_t *text;             /*Text of the message box*/
    lv_obj_t *btnm;            /*Button matrix for the buttons*/
    uint16_t anim_time;         /*Duration of close animation [ms] (0: no animation)*/
} lv_mbox_ext_t;

enum {
    LV_MBOX_STYLE_BG,
    LV_MBOX_STYLE_BTN_BG,
    LV_MBOX_STYLE_BTN_REL,
    LV_MBOX_STYLE_BTN_PR,
    LV_MBOX_STYLE_BTN_TGL_REL,
    LV_MBOX_STYLE_BTN_TGL_PR,
    LV_MBOX_STYLE_BTN_INA,
};
typedef uint8_t lv_mbox_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a message box objects
 * @param par pointer to an object, it will be the parent of the new message box
 * @param copy pointer to a message box object, if not NULL then the new object will be copied from it
 * @return pointer to the created message box
 */
lv_obj_t * lv_mbox_create(lv_obj_t * par, const lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add button to the message box
 * @param mbox pointer to message box object
 * @param btn_map button descriptor (button matrix map).
 *                E.g.  a const char *txt[] = {"ok", "close", ""} (Can not be local variable)
 * @param action a function which will be called when a button is released
 */
void lv_mbox_add_btns(lv_obj_t * mbox, const char **btn_map, lv_btnm_action_t action);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of the message box
 * @param mbox pointer to a message box
 * @param txt a '\0' terminated character string which will be the message box text
 */
void lv_mbox_set_text(lv_obj_t * mbox, const char * txt);

/**
 * Stop the action to call when button is released
 * @param mbox pointer to a message box object
 * @param pointer to an 'lv_btnm_action_t' action. In the action you need to use `lv_mbox_get_from_btn()` to get the `mbox`.
 */
void lv_mbox_set_action(lv_obj_t * mbox, lv_btnm_action_t action);

/**
 * Set animation duration
 * @param mbox pointer to a message box object
 * @param anim_time animation length in  milliseconds (0: no animation)
 */
void lv_mbox_set_anim_time(lv_obj_t * mbox, uint16_t anim_time);

/**
 * Automatically delete the message box after a given time
 * @param mbox pointer to a message box object
 * @param delay a time (in milliseconds) to wait before delete the message box
 */
void lv_mbox_start_auto_close(lv_obj_t * mbox, uint16_t delay);

/**
 * Stop the auto. closing of message box
 * @param mbox pointer to a message box object
 */
void lv_mbox_stop_auto_close(lv_obj_t * mbox);

/**
 * Set a style of a message box
 * @param mbox pointer to a message box object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_mbox_set_style(lv_obj_t *mbox, lv_mbox_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of the message box
 * @param mbox pointer to a message box object
 * @return pointer to the text of the message box
 */
const char * lv_mbox_get_text(const lv_obj_t * mbox);

/**
 * Get the message box object from one of its button.
 * It is useful in the button release actions where only the button is known
 * @param btn pointer to a button of a message box
 * @return pointer to the button's message box
 */
lv_obj_t * lv_mbox_get_from_btn(const lv_obj_t * btn);

/**
 * Get the animation duration (close animation time)
 * @param mbox pointer to a message box object
 * @return animation length in  milliseconds (0: no animation)
 */
uint16_t lv_mbox_get_anim_time(const lv_obj_t * mbox);


/**
 * Get a style of a message box
 * @param mbox pointer to a message box object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_mbox_get_style(const lv_obj_t *mbox, lv_mbox_style_t type);

/**********************
 *      MACROS
 **********************/


#endif  /*USE_LV_MBOX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_MBOX_H*/
