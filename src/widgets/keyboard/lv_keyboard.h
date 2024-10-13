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
#include "../buttonmatrix/lv_buttonmatrix.h"

#if LV_USE_KEYBOARD

/*Testing of dependencies*/
#if LV_USE_BUTTONMATRIX == 0
#error "lv_buttonmatrix is required. Enable it in lv_conf.h (LV_USE_BUTTONMATRIX  1) "
#endif

#if LV_USE_TEXTAREA == 0
#error "lv_textarea is required. Enable it in lv_conf.h (LV_USE_TEXTAREA  1) "
#endif

/*********************
 *      DEFINES
 *********************/
#define LV_KEYBOARD_CTRL_BUTTON_FLAGS (LV_BUTTONMATRIX_CTRL_NO_REPEAT | LV_BUTTONMATRIX_CTRL_CLICK_TRIG | LV_BUTTONMATRIX_CTRL_CHECKED)

/**********************
 *      TYPEDEFS
 **********************/

/** Current keyboard mode.*/
typedef enum {
    LV_KEYBOARD_MODE_TEXT_LOWER,
    LV_KEYBOARD_MODE_TEXT_UPPER,
    LV_KEYBOARD_MODE_SPECIAL,
    LV_KEYBOARD_MODE_NUMBER,
    LV_KEYBOARD_MODE_USER_1,
    LV_KEYBOARD_MODE_USER_2,
    LV_KEYBOARD_MODE_USER_3,
    LV_KEYBOARD_MODE_USER_4,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    LV_KEYBOARD_MODE_TEXT_ARABIC
#endif
} lv_keyboard_mode_t;

#if LV_USE_OBJ_PROPERTY
enum {
    LV_PROPERTY_ID(KEYBOARD, TEXTAREA,            LV_PROPERTY_TYPE_OBJ,   0),
    LV_PROPERTY_ID(KEYBOARD, MODE,                LV_PROPERTY_TYPE_INT,   1),
    LV_PROPERTY_ID(KEYBOARD, POPOVERS,            LV_PROPERTY_TYPE_INT,   2),
    LV_PROPERTY_ID(KEYBOARD, SELECTED_BUTTON,     LV_PROPERTY_TYPE_INT,   3),
    LV_PROPERTY_KEYBOARD_END,
};
#endif

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_keyboard_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a Keyboard object
 * @param parent    pointer to an object, it will be the parent of the new keyboard
 * @return          pointer to the created keyboard
 */
lv_obj_t * lv_keyboard_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb        pointer to a Keyboard object
 * @param ta        pointer to a Text Area object to write there
 */
void lv_keyboard_set_textarea(lv_obj_t * kb, lv_obj_t * ta);

/**
 * Set a new a mode (text or number map)
 * @param kb        pointer to a Keyboard object
 * @param mode      the mode from 'lv_keyboard_mode_t'
 */
void lv_keyboard_set_mode(lv_obj_t * kb, lv_keyboard_mode_t mode);

/**
 * Show the button title in a popover when pressed.
 * @param kb        pointer to a Keyboard object
 * @param en        whether "popovers" mode is enabled
 */
void lv_keyboard_set_popovers(lv_obj_t * kb, bool en);

/**
 * Set a new map for the keyboard
 * @param kb        pointer to a Keyboard object
 * @param mode      keyboard map to alter 'lv_keyboard_mode_t'
 * @param map       pointer to a string array to describe the map.
 *                  See 'lv_buttonmatrix_set_map()' for more info.
 * @param ctrl_map  See 'lv_buttonmatrix_set_ctrl_map()' for more info.

 */
void lv_keyboard_set_map(lv_obj_t * kb, lv_keyboard_mode_t mode, const char * const map[],
                         const lv_buttonmatrix_ctrl_t ctrl_map[]);

/*=====================
 * Getter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb        pointer to a Keyboard object
 * @return          pointer to the assigned Text Area object
 */
lv_obj_t * lv_keyboard_get_textarea(const lv_obj_t * kb);

/**
 * Set a new a mode (text or number map)
 * @param kb        pointer to a Keyboard object
 * @return          the current mode from 'lv_keyboard_mode_t'
 */
lv_keyboard_mode_t lv_keyboard_get_mode(const lv_obj_t * kb);

/**
 * Tell whether "popovers" mode is enabled or not.
 * @param obj       pointer to a Keyboard object
 * @return          true: "popovers" mode is enabled; false: disabled
 */
bool lv_keyboard_get_popovers(const lv_obj_t * obj);

/**
 * Get the current map of a keyboard
 * @param kb        pointer to a keyboard object
 * @return          the current map
 */
const char * const * lv_keyboard_get_map_array(const lv_obj_t * kb);

/**
 * Get the index of the lastly "activated" button by the user (pressed, released, focused etc)
 * Useful in the `event_cb` to get the text of the button, check if hidden etc.
 * @param obj       pointer to button matrix object
 * @return          index of the last released button (LV_BUTTONMATRIX_BUTTON_NONE: if unset)
 */
uint32_t lv_keyboard_get_selected_button(const lv_obj_t * obj);

/**
 * Get the button's text
 * @param obj       pointer to button matrix object
 * @param btn_id    the index a button not counting new line characters.
 * @return          text of btn_index` button
 */
const char * lv_keyboard_get_button_text(const lv_obj_t * obj, uint32_t btn_id);

/*=====================
 * Other functions
 *====================*/

/**
 * Default keyboard event to add characters to the Text area and change the map.
 * If a custom `event_cb` is added to the keyboard this function can be called from it to handle the
 * button clicks
 * @param e the triggering event
 */
void lv_keyboard_def_event_cb(lv_event_t * e);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_KEYBOARD*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_KEYBOARD_H*/
