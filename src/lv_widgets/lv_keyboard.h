/**
 * @file lv_keyboard.h
 *
 */

#ifndef LV_KEYBOARD_H
#define LV_KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_KEYBOARD != 0

/*Testing of dependencies*/
#if LV_USE_BTNMATRIX == 0
#error "lv_kb: lv_btnm is required. Enable it in lv_conf.h (LV_USE_BTNMATRIX 1)"
#endif

#if LV_USE_TEXTAREA == 0
#error "lv_kb: lv_ta is required. Enable it in lv_conf.h (LV_USE_TEXTAREA 1)"
#endif

#include "../lv_core/lv_obj.h"
#include "lv_btnmatrix.h"

/*********************
 *      DEFINES
 *********************/
#define LV_KEYBOARD_CTRL_BTN_FLAGS (LV_BTNMATRIX_CTRL_NO_REPEAT | LV_BTNMATRIX_CTRL_CLICK_TRIG)

/**********************
 *      TYPEDEFS
 **********************/

/** Current keyboard mode. */
enum {
    LV_KEYBOARD_MODE_TEXT_LOWER,
    LV_KEYBOARD_MODE_TEXT_UPPER,
    LV_KEYBOARD_MODE_SPECIAL,
    LV_KEYBOARD_MODE_NUM
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    , LV_KEYBOARD_MODE_TEXT_ARABIC
#endif
};
typedef uint8_t lv_keyboard_mode_t;

/*Data of keyboard*/
typedef struct {
    lv_btnmatrix_ext_t btnm; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * ta;          /*Pointer to the assigned text area*/
    lv_keyboard_mode_t mode;      /*Key map type*/
    uint8_t cursor_mng : 1; /*1: automatically show/hide cursor when a text area is assigned or left*/
} lv_keyboard_ext_t;

enum {
    LV_KEYBOARD_PART_BG,
    LV_KEYBOARD_PART_BTN,
};
typedef uint8_t lv_keyboard_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a keyboard objects
 * @param par pointer to an object, it will be the parent of the new keyboard
 * @param copy pointer to a keyboard object, if not NULL then the new object will be copied from it
 * @return pointer to the created keyboard
 */
lv_obj_t * lv_keyboard_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @param ta pointer to a Text Area object to write there
 */
void lv_keyboard_set_textarea(lv_obj_t * kb, lv_obj_t * ta);

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @param mode the mode from 'lv_keyboard_mode_t'
 */
void lv_keyboard_set_mode(lv_obj_t * kb, lv_keyboard_mode_t mode);

/**
 * Automatically hide or show the cursor of the current Text Area
 * @param kb pointer to a Keyboard object
 * @param en true: show cursor on the current text area, false: hide cursor
 */
void lv_keyboard_set_cursor_manage(lv_obj_t * kb, bool en);

/**
 * Set a new map for the keyboard
 * @param kb pointer to a Keyboard object
 * @param mode keyboard map to alter 'lv_keyboard_mode_t'
 * @param map pointer to a string array to describe the map.
 *            See 'lv_btnmatrix_set_map()' for more info.
 */
void lv_keyboard_set_map(lv_obj_t * kb, lv_keyboard_mode_t mode, const char * map[]);

/**
 * Set the button control map (hidden, disabled etc.) for the keyboard. The
 * control map array will be copied and so may be deallocated after this
 * function returns.
 * @param kb pointer to a keyboard object
 * @param mode keyboard ctrl map to alter 'lv_keyboard_mode_t'
 * @param ctrl_map pointer to an array of `lv_btn_ctrl_t` control bytes.
 *                 See: `lv_btnmatrix_set_ctrl_map` for more details.
 */
void lv_keyboard_set_ctrl_map(lv_obj_t * kb, lv_keyboard_mode_t mode, const lv_btnmatrix_ctrl_t ctrl_map[]);

/*=====================
 * Getter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @return pointer to the assigned Text Area object
 */
lv_obj_t * lv_keyboard_get_textarea(const lv_obj_t * kb);

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @return the current mode from 'lv_keyboard_mode_t'
 */
lv_keyboard_mode_t lv_keyboard_get_mode(const lv_obj_t * kb);

/**
 * Get the current cursor manage mode.
 * @param kb pointer to a Keyboard object
 * @return true: show cursor on the current text area, false: hide cursor
 */
bool lv_keyboard_get_cursor_manage(const lv_obj_t * kb);

/**
 * Get the current map of a keyboard
 * @param kb pointer to a keyboard object
 * @return the current map
 */
static inline const char ** lv_keyboard_get_map_array(const lv_obj_t * kb)
{
    return lv_btnmatrix_get_map_array(kb);
}

/*=====================
 * Other functions
 *====================*/

/**
 * Default keyboard event to add characters to the Text area and change the map.
 * If a custom `event_cb` is added to the keyboard this function be called from it to handle the
 * button clicks
 * @param kb pointer to a keyboard
 * @param event the triggering event
 */
void lv_keyboard_def_event_cb(lv_obj_t * kb, lv_event_t event);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_KEYBOARD*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_KEYBOARD_H*/
