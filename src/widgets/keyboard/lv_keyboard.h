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
enum _lv_property_keyboard_id_t {
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
 * @return          pointer to the created keyboard object
 */
lv_obj_t * lv_keyboard_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Assign a text area to the keyboard. Pressed characters will be inserted there.
 * @param kb        pointer to a keyboard object
 * @param ta        pointer to a text area object to write into
 */
void lv_keyboard_set_textarea(lv_obj_t * kb, lv_obj_t * ta);

/**
 * Set a new mode (e.g., text, number, special characters).
 * @param kb        pointer to a keyboard object
 * @param mode      the desired mode (see 'lv_keyboard_mode_t')
 */
void lv_keyboard_set_mode(lv_obj_t * kb, lv_keyboard_mode_t mode);

/**
 * Enable or disable popovers showing button titles on press.
 * @param kb        pointer to a keyboard object
 * @param en        true to enable popovers; false to disable
 */
void lv_keyboard_set_popovers(lv_obj_t * kb, bool en);

/**
 * Set a custom button map for the keyboard.
 * @param kb        pointer to a keyboard object
 * @param mode      the mode to assign the new map to (see 'lv_keyboard_mode_t')
 * @param map       pointer to a string array describing the button map
 *                  see 'lv_buttonmatrix_set_map()' for more details
 * @param ctrl_map  pointer to the control map. See 'lv_buttonmatrix_set_ctrl_map()'

 */
void lv_keyboard_set_map(lv_obj_t * kb, lv_keyboard_mode_t mode, const char * const map[],
                         const lv_buttonmatrix_ctrl_t ctrl_map[]);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text area currently assigned to the keyboard.
 * @param kb        pointer to a keyboard object
 * @return          pointer to the assigned text area object
 */
lv_obj_t * lv_keyboard_get_textarea(const lv_obj_t * kb);

/**
 * Get the current mode of the keyboard.
 * @param kb        pointer to a keyboard object
 * @return          the current mode (see 'lv_keyboard_mode_t')
 */
lv_keyboard_mode_t lv_keyboard_get_mode(const lv_obj_t * kb);

/**
 * Check whether popovers are enabled on the keyboard.
 * @param obj       pointer to a keyboard object
 * @return          true if popovers are enabled; false otherwise
 */
bool lv_keyboard_get_popovers(const lv_obj_t * obj);

/**
 * Get the current button map of the keyboard.
 * @param kb        pointer to a keyboard object
 * @return          pointer to the map array
 */
const char * const * lv_keyboard_get_map_array(const lv_obj_t * kb);

/**
 * Get the index of the last selected button (pressed, released, focused, etc.).
 * Useful in the `event_cb` to retrieve button text or properties.
 * @param obj       pointer to a keyboard object
 * @return          index of the last interacted button
 *                  returns LV_BUTTONMATRIX_BUTTON_NONE if not set
 */
uint32_t lv_keyboard_get_selected_button(const lv_obj_t * obj);

/**
 * Get the text of a button by index.
 * @param obj       pointer to a keyboard object
 * @param btn_id    index of the button (excluding newline characters)
 * @return          pointer to the text of the button
 */
const char * lv_keyboard_get_button_text(const lv_obj_t * obj, uint32_t btn_id);

/*=====================
 * Other functions
 *====================*/

/**
 * Default keyboard event callback to handle button presses.
 * Adds characters to the text area and switches map if needed.
 * If a custom `event_cb` is used, this function can be called within it.
 * @param e         the triggering event
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
