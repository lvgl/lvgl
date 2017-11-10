/**
 * @file lv_cb.h
 * 
 */

#ifndef LV_CB_H
#define LV_CB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_CB != 0

/*Testing of dependencies*/
#if USE_LV_BTN == 0
#error "lv_cb: lv_btn is required. Enable it in lv_conf.h (USE_LV_BTN  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_cb: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif

#include "../lv_obj/lv_obj.h"
#include "lv_btn.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of check box*/
typedef struct
{
    lv_btn_ext_t bg_btn; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * bullet;  /*Pointer to button*/
    lv_obj_t * label;   /*Pointer to label*/
}lv_cb_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a check box objects
 * @param par pointer to an object, it will be the parent of the new check box
 * @param copy pointer to a check box object, if not NULL then the new object will be copied from it
 * @return pointer to the created check box
 */
lv_obj_t * lv_cb_create(lv_obj_t * par, lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a check box
 * @param cb pointer to a check box
 * @param txt the text of the check box
 */
void lv_cb_set_text(lv_obj_t * cb, const char * txt);

/**
 * Set the state of the check box
 * @param cb pointer to a check box object
 * @param checked true: make the check box checked; false: make it unchecked
 */
static inline void lv_cb_set_checked(lv_obj_t * cb, bool checked)
{
    lv_btn_set_state(cb, checked ? LV_BTN_STATE_TGL_RELEASED : LV_BTN_STATE_RELEASED);
}

/**
 * Make the check box inactive (disabled)
 * @param cb pointer to a check box object
 */
static inline void lv_cb_set_inactive(lv_obj_t * cb)
{
    lv_btn_set_state(cb, LV_BTN_STATE_INACTIVE);
}

/**
 * Set a function to call when the button is pressed
 * @param cb pointer to a check box object
 */
static inline void lv_cb_set_action(lv_obj_t * cb, lv_action_t action)
{
    lv_btn_set_action(cb, LV_BTN_ACTION_RELEASE, action);
}

/**
 * Set styles of a checkbox's background is each state. Use NULL for any style to leave it unchanged
 * @param cb pointer to check box object
 * @param rel pointer to a style for releases state
 * @param pr  pointer to a style for pressed state
 * @param tgl_rel pointer to a style for toggled releases state
 * @param tgl_pr pointer to a style for toggled pressed state
 * @param ina pointer to a style for inactive state
 */
static inline void lv_cb_set_style(lv_obj_t *cb, lv_style_t *bg)
{
    lv_btn_set_style(cb, bg, bg, bg, bg, bg);
}

/**
 * Set styles of a checkbox's bullet in each state. Use NULL for any style to leave it unchanged
 * @param cb pointer to check box object
 * @param rel pointer to a style for releases state
 * @param pr  pointer to a style for pressed state
 * @param tgl_rel pointer to a style for toggled releases state
 * @param tgl_pr pointer to a style for toggled pressed state
 * @param ina pointer to a style for inactive state
 */
void lv_cb_set_style_bullet(lv_obj_t *cb, lv_style_t *rel, lv_style_t *pr,
                                          lv_style_t *tgl_rel, lv_style_t *tgl_pr,
                                          lv_style_t *ina);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a check box
 * @param cb pointer to check box object
 * @return pointer to the text of the check box
 */
const char * lv_cb_get_text(lv_obj_t * cb);

/**
 * Get the current state of the check box
 * @param cb pointer to a check box object
 * @return true: checked; false: not checked
 */
static inline bool lv_cb_get_checked(lv_obj_t * cb)
{
    return lv_btn_get_state(cb) == LV_BTN_STATE_RELEASED ? true : false;
}

/**
 * Get the action of a check box
 * @param cb pointer to a button object
 * @return pointer to the action function
 */
static inline lv_action_t lv_cb_get_action(lv_obj_t * cb)
{
    return lv_btn_get_action(cb, LV_BTN_ACTION_RELEASE);
}

/**
 * Get the style of a check box's background in a given state
 * @param cb pointer to a check box object
 * @param state a state from 'lv_btn_state_t' in which style should be get
 * @return pointer to the style in the given state
 */
static inline lv_style_t * lv_cb_get_style_bg(lv_obj_t * cb)
{
    return lv_btn_get_style(cb, lv_btn_get_state(cb));
}

/**
 * Get styles of a checkbox's bullet in a  state.
 * @param state a state from 'lv_btn_state_t' in which style should be get
 * @return pointer to the style in the given state
 */
lv_style_t * lv_cb_get_style_bullet(lv_obj_t *cb, lv_btn_state_t state);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_CB*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_CB_H*/
