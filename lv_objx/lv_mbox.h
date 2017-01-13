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
#if USE_LV_RECT == 0
#error "lv_mbox: lv_rect is required. Enable it in lv_conf.h (USE_LV_RECT  1) "
#endif

#if USE_LV_BTN == 0
#error "lv_mbox: lv_btn is required. Enable it in lv_conf.h (USE_LV_BTN  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_mbox: lv_rlabel is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif


#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"
#include "lv_btn.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of message box*/
typedef struct
{
	lv_rects_t bg; /*Style of ancestor*/
	/*New style element for this type */
	lv_labels_t title;
	lv_labels_t txt;
	lv_rects_t btnh;
	lv_btns_t btn;
	lv_labels_t btn_label;
}lv_mboxs_t;

/*Built-in styles of message box*/
typedef enum
{
	LV_MBOXS_DEF,
    LV_MBOXS_BUBBLE, /*Will be removed (use LV_MBOXS_INFO instead)*/
	LV_MBOXS_INFO,
	LV_MBOXS_WARN,
	LV_MBOXS_ERR,
}lv_mboxs_builtin_t;

/*Data of message box*/
typedef struct
{
	lv_rect_ext_t rect; /*Ext. of ancestor*/
	/*New data for this type */
	lv_obj_t * title;
	lv_obj_t * txt;
	lv_obj_t * btnh;
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
 * Add a button to the message box
 * @param mbox pointer to message box object
 * @param btn_txt the text of the button
 * @param rel_action a function which will be called when the button is relesed
 * @return pointer to the created button (lv_btn)
 */
lv_obj_t * lv_mbox_add_btn(lv_obj_t * mbox, const char * btn_txt, lv_action_t rel_action);

/**
 * A release action which can be assigned to a message box button to close it
 * @param btn pointer to the released button
 * @param dispi pointer to the caller display input
 * @return always false because the button is deleted with the mesage box
 */
bool lv_mbox_close_action(lv_obj_t * btn, lv_dispi_t * dispi);

/**
 * Automatically delete the message box after a given time
 * @param mbox pointer to a message box object
 * @param tout a time (in milliseconds) to wait before delete the message box
 */
void lv_mbox_auto_close(lv_obj_t * mbox, uint16_t tout);

/**
 * Set the title of the message box
 * @param mbox pointer to a message box
 * @param title a '\0' terminated character string which will be the message box title
 */
void lv_mbox_set_title(lv_obj_t * mbox, const char * title);

/**
 * Set the text of the message box
 * @param mbox pointer to a message box
 * @param txt a '\0' terminated character string which will be the message box text
 */
void lv_mbox_set_txt(lv_obj_t * mbox, const char * txt);

/**
 * get the title of the message box
 * @param mbox pointer to a message box object
 * @return pointer to the title of the message box
 */
const char * lv_mbox_get_title(lv_obj_t * mbox);

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
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_mboxs_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_mboxs_t style
 */
lv_mboxs_t * lv_mboxs_get(lv_mboxs_builtin_t style, lv_mboxs_t * copy);

/**********************
 *      MACROS
 **********************/

#endif

#endif
