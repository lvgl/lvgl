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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#if USE_LV_KB != 0

/*Testing of dependencies*/
#if USE_LV_BTNM == 0
#error "lv_kb: lv_btnm is required. Enable it in lv_conf.h (USE_LV_BTNM  1) "
#endif

#if USE_LV_TA == 0
#error "lv_kb: lv_ta is required. Enable it in lv_conf.h (USE_LV_TA  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "lv_btnm.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

enum {
    LV_KB_MODE_TEXT,
    LV_KB_MODE_NUM,
};
typedef uint8_t lv_kb_mode_t;

/*Data of keyboard*/
typedef struct {
    lv_btnm_ext_t btnm;     /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t *ta;               /*Pointer to the assigned text area*/
    lv_kb_mode_t mode;          /*Key map type*/
    uint8_t cursor_mng      :1; /*1: automatically show/hide cursor when a text area is assigned or left*/
    lv_action_t  ok_action;     /*Called when the "Ok" button is clicked*/
    lv_action_t  hide_action;  /*Called when the "Hide" button is clicked*/
} lv_kb_ext_t;

enum {
    LV_KB_STYLE_BG,
    LV_KB_STYLE_BTN_REL,
    LV_KB_STYLE_BTN_PR,
    LV_KB_STYLE_BTN_TGL_REL,
    LV_KB_STYLE_BTN_TGL_PR,
    LV_KB_STYLE_BTN_INA,
};
typedef uint8_t lv_kb_style_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a keyboard objects
 * @param par pointer to an object, it will be the parent of the new keyboard
 * @param copy pointer to a keyboard object, if not NULL then the new object will be copied from it
 * @return pointer to the created keyboard
 */
lv_obj_t * lv_kb_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

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
 * Automatically hide or show the cursor of the current Text Area
 * @param kb pointer to a Keyboard object
 * @param en true: show cursor on the current text area, false: hide cursor
 */
void lv_kb_set_cursor_manage(lv_obj_t * kb, bool en);

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
void lv_kb_set_hide_action(lv_obj_t * kb, lv_action_t action);

/**
 * Set a new map for the keyboard
 * @param kb pointer to a Keyboard object
 * @param map pointer to a string array to describe the map.
 *            See 'lv_btnm_set_map()' for more info.
 */
static inline void lv_kb_set_map(lv_obj_t *kb, const char ** map)
{
    lv_btnm_set_map(kb, map);
}

/**
 * Set a style of a keyboard
 * @param kb pointer to a keyboard object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_kb_set_style(lv_obj_t *kb, lv_kb_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @return pointer to the assigned Text Area object
 */
lv_obj_t * lv_kb_get_ta(const lv_obj_t * kb);

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @return the current mode from 'lv_kb_mode_t'
 */
lv_kb_mode_t lv_kb_get_mode(const lv_obj_t * kb);

/**
 * Get the current cursor manage mode.
 * @param kb pointer to a Keyboard object
 * @return true: show cursor on the current text area, false: hide cursor
 */
bool lv_kb_get_cursor_manage(const lv_obj_t * kb);

/**
 * Get the callback to call when the "Ok" button is pressed
 * @param kb pointer to Keyboard object
 * @return the ok callback
 */
lv_action_t lv_kb_get_ok_action(const lv_obj_t * kb);

/**
 * Get the callback to call when the "Hide" button is pressed
 * @param kb pointer to Keyboard object
 * @return the close callback
 */
lv_action_t lv_kb_get_hide_action(const lv_obj_t * kb);

/**
 * Get a style of a keyboard
 * @param kb pointer to a keyboard object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_kb_get_style(const lv_obj_t *kb, lv_kb_style_t type);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_KB*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_KB_H*/
