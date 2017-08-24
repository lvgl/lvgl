/**
 * @file lv_kb.h
 * 
 */

#ifndef LV_KB_H
#define LV_KB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_KB != 0

#include "../lv_obj/lv_obj.h"
#include "lv_btnm.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_KB_MODE_TXT,
    LV_KB_MODE_NUM,
}lv_kb_mode_t;


/*Data of keyboard*/
typedef struct {
    lv_btnm_ext_t btnm;     /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * ta;
    lv_kb_mode_t mode;
    uint8_t cur_mng    :1;
    lv_action_t     ok_action;
    lv_action_t     close_action;
}lv_kb_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a keyboard objects
 * @param par pointer to an object, it will be the parent of the new keyboard
 * @param copy pointer to a keyboard object, if not NULL then the new object will be copied from it
 * @return pointer to the created keyboard
 */
lv_obj_t * lv_kb_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the keyboard
 * @param kb pointer to a keyboard object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_kb_signal(lv_obj_t * kb, lv_signal_t sign, void * param);

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @param ta pointer to a Text Area object to write there
 */
void lv_kb_set_ta(lv_obj_t * kb, lv_obj_t * ta);

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @param mode the mode from 'lv_kb_mode_t'
 */
void lv_kb_set_mode(lv_obj_t * kb, lv_kb_mode_t mode);

/**
 * Automatically hide or show the cursor of Text Area
 * @param kb pointer to a Keyboard object
 * @param en true: show cursor on the current text area, false: hide cursor
 */
void lv_kb_set_cur_mng(lv_obj_t * kb, bool en);

/**
 * Set call back to call when the "Ok" button is pressed
 * @param kb pointer to Keyboard object
 * @param action a callback with 'lv_action_t' type
 */
void lv_kb_set_ok_action(lv_obj_t * kb, lv_action_t action);

/**
 * Set call back to call when the "Hide" button is pressed
 * @param kb pointer to Keyboard object
 * @param action a callback with 'lv_action_t' type
 */
void lv_kb_set_close_action(lv_obj_t * kb, lv_action_t action);

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @return pointer to the assigned Text Area object
 */
lv_obj_t * lv_kb_get_ta(lv_obj_t * kb);
/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @return the current mode from 'lv_kb_mode_t'
 */
lv_kb_mode_t lv_kb_get_mode(lv_obj_t * kb);


/**
 * Get the current cursor manage mode.
 * @param kb pointer to a Keyboard object
 * @return true: show cursor on the current text area, false: hide cursor
 */
bool lv_kb_get_cur_mng(lv_obj_t * kb, bool en);

/**
 * Get the callback to call when the "Ok" button is pressed
 * @param kb pointer to Keyboard object
 * @return the ok callback
 */
lv_action_t lv_kb_get_ok_action(lv_obj_t * kb, lv_action_t action);

/**
 * Get the callback to call when the "Hide" button is pressed
 * @param kb pointer to Keyboard object
 * @return the close callback
 */
lv_action_t lv_kb_get_close_action(lv_obj_t * kb, lv_action_t action);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_KB*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_KB_H*/
