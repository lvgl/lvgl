/**
 * @file lv_textarea.h
 *
 */

#ifndef LV_TEXTAREA_H
#define LV_TEXTAREA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../label/lv_label.h"

#if LV_USE_TEXTAREA != 0

/*Testing of dependencies*/
#if LV_USE_LABEL == 0
#error "lv_textarea: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL 1)"
#endif

/*********************
 *      DEFINES
 *********************/
#define LV_TEXTAREA_CURSOR_LAST (0x7FFF) /*Put the cursor after the last character*/
LV_EXPORT_CONST_INT(LV_TEXTAREA_CURSOR_LAST);

#define LV_PART_TEXTAREA_PLACEHOLDER LV_PART_CUSTOM_FIRST
LV_EXPORT_CONST_INT(LV_PART_TEXTAREA_PLACEHOLDER);

/**********************
 *      TYPEDEFS
 **********************/

#if LV_USE_OBJ_PROPERTY
enum _lv_property_textarea_id_t {
    LV_PROPERTY_ID(TEXTAREA, TEXT,              LV_PROPERTY_TYPE_TEXT,  0),
    LV_PROPERTY_ID(TEXTAREA, PLACEHOLDER_TEXT,  LV_PROPERTY_TYPE_TEXT,  1),
    LV_PROPERTY_ID(TEXTAREA, CURSOR_POS,        LV_PROPERTY_TYPE_INT,   2),
    LV_PROPERTY_ID(TEXTAREA, CURSOR_CLICK_POS,  LV_PROPERTY_TYPE_INT,   3),
    LV_PROPERTY_ID(TEXTAREA, PASSWORD_MODE,     LV_PROPERTY_TYPE_INT,   4),
    LV_PROPERTY_ID(TEXTAREA, PASSWORD_BULLET,   LV_PROPERTY_TYPE_TEXT,  5),
    LV_PROPERTY_ID(TEXTAREA, ONE_LINE,          LV_PROPERTY_TYPE_BOOL,  6),
    LV_PROPERTY_ID(TEXTAREA, ACCEPTED_CHARS,    LV_PROPERTY_TYPE_TEXT,  7),
    LV_PROPERTY_ID(TEXTAREA, MAX_LENGTH,        LV_PROPERTY_TYPE_INT,   8),
    LV_PROPERTY_ID(TEXTAREA, INSERT_REPLACE,    LV_PROPERTY_TYPE_TEXT,  9),
    LV_PROPERTY_ID(TEXTAREA, TEXT_SELECTION,    LV_PROPERTY_TYPE_BOOL,  10),
    LV_PROPERTY_ID(TEXTAREA, PASSWORD_SHOW_TIME, LV_PROPERTY_TYPE_INT,   11),
    LV_PROPERTY_ID(TEXTAREA, LABEL,             LV_PROPERTY_TYPE_OBJ,   12),
    LV_PROPERTY_ID(TEXTAREA, TEXT_IS_SELECTED,  LV_PROPERTY_TYPE_INT,   13),
    LV_PROPERTY_ID(TEXTAREA, CURRENT_CHAR,      LV_PROPERTY_TYPE_INT,   14),
    LV_PROPERTY_TEXTAREA_END,
};
#endif

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_textarea_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a text area object
 * @param parent    pointer to an object, it will be the parent of the new text area
 * @return          pointer to the created text area
 */
lv_obj_t * lv_textarea_create(lv_obj_t * parent);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Insert a character to the current cursor position.
 * To add a wide char, e.g. 'Á' use `lv_text_encoded_conv_wc('Á')`
 * @param obj       pointer to a text area object
 * @param c         a character (e.g. 'a')
 */
void lv_textarea_add_char(lv_obj_t * obj, uint32_t c);

/**
 * Insert a text to the current cursor position
 * @param obj       pointer to a text area object
 * @param txt       a '\0' terminated string to insert
 */
void lv_textarea_add_text(lv_obj_t * obj, const char * txt);

/**
 * Delete a the left character from the current cursor position
 * @param obj       pointer to a text area object
 */
void lv_textarea_delete_char(lv_obj_t * obj);

/**
 * Delete the right character from the current cursor position
 * @param obj       pointer to a text area object
 */
void lv_textarea_delete_char_forward(lv_obj_t * obj);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a text area
 * @param obj       pointer to a text area object
 * @param txt       pointer to the text
 */
void lv_textarea_set_text(lv_obj_t * obj, const char * txt);

/**
 * Set the placeholder text of a text area
 * @param obj       pointer to a text area object
 * @param txt       pointer to the text
 */
void lv_textarea_set_placeholder_text(lv_obj_t * obj, const char * txt);

/**
 * Set the cursor position
 * @param obj       pointer to a text area object
 * @param pos       the new cursor position in character index
 *                  < 0 : index from the end of the text
 *                  LV_TEXTAREA_CURSOR_LAST: go after the last character
 */
void lv_textarea_set_cursor_pos(lv_obj_t * obj, int32_t pos);

/**
 * Enable/Disable the positioning of the cursor by clicking the text on the text area.
 * @param obj       pointer to a text area object
 * @param en        true: enable click positions; false: disable
 */
void lv_textarea_set_cursor_click_pos(lv_obj_t * obj, bool en);

/**
 * Enable/Disable password mode
 * @param obj       pointer to a text area object
 * @param en        true: enable, false: disable
 */
void lv_textarea_set_password_mode(lv_obj_t * obj, bool en);

/**
 * Set the replacement characters to show in password mode
 * @param obj       pointer to a text area object
 * @param bullet    pointer to the replacement text
 */
void lv_textarea_set_password_bullet(lv_obj_t * obj, const char * bullet);

/**
 * Configure the text area to one line or back to normal
 * @param obj       pointer to a text area object
 * @param en        true: one line, false: normal
 */
void lv_textarea_set_one_line(lv_obj_t * obj, bool en);

/**
 * Set a list of characters. Only these characters will be accepted by the text area
 * @param obj       pointer to a text area object
 * @param list      list of characters. Only the pointer is saved. E.g. "+-.,0123456789"
 */
void lv_textarea_set_accepted_chars(lv_obj_t * obj, const char * list);

/**
 * Set max length of a Text Area.
 * @param obj       pointer to a text area object
 * @param num       the maximal number of characters can be added (`lv_textarea_set_text` ignores it)
 */
void lv_textarea_set_max_length(lv_obj_t * obj, uint32_t num);

/**
 * In `LV_EVENT_INSERT` the text which planned to be inserted can be replaced by another text.
 * It can be used to add automatic formatting to the text area.
 * @param obj       pointer to a text area object
 * @param txt       pointer to a new string to insert. If `""` no text will be added.
 *                  The variable must be live after the `event_cb` exists. (Should be `global` or `static`)
 */
void lv_textarea_set_insert_replace(lv_obj_t * obj, const char * txt);

/**
 * Enable/disable selection mode.
 * @param obj       pointer to a text area object
 * @param en        true or false to enable/disable selection mode
 */
void lv_textarea_set_text_selection(lv_obj_t * obj, bool en);

/**
 * Set how long show the password before changing it to '*'
 * @param obj       pointer to a text area object
 * @param time      show time in milliseconds. 0: hide immediately.
 */
void lv_textarea_set_password_show_time(lv_obj_t * obj, uint32_t time);

/**
 * @deprecated Use the normal text_align style property instead
 * Set the label's alignment.
 * It sets where the label is aligned (in one line mode it can be smaller than the text area)
 * and how the lines of the area align in case of multiline text area
 * @param obj       pointer to a text area object
 * @param align     the align mode from ::lv_text_align_t
 */
void lv_textarea_set_align(lv_obj_t * obj, lv_text_align_t align);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a text area. In password mode it gives the real text (not '*'s).
 * @param obj       pointer to a text area object
 * @return          pointer to the text
 */
const char * lv_textarea_get_text(const lv_obj_t * obj);

/**
 * Get the placeholder text of a text area
 * @param obj       pointer to a text area object
 * @return          pointer to the text
 */
const char * lv_textarea_get_placeholder_text(lv_obj_t * obj);

/**
 * Get the label of a text area
 * @param obj       pointer to a text area object
 * @return          pointer to the label object
 */
lv_obj_t * lv_textarea_get_label(const lv_obj_t * obj);

/**
 * Get the current cursor position in character index
 * @param obj       pointer to a text area object
 * @return          the cursor position
 */
uint32_t lv_textarea_get_cursor_pos(const lv_obj_t * obj);

/**
 * Get whether the cursor click positioning is enabled or not.
 * @param obj       pointer to a text area object
 * @return          true: enable click positions; false: disable
 */
bool lv_textarea_get_cursor_click_pos(lv_obj_t * obj);

/**
 * Get the password mode attribute
 * @param obj       pointer to a text area object
 * @return          true: password mode is enabled, false: disabled
 */
bool lv_textarea_get_password_mode(const lv_obj_t * obj);

/**
 * Get the replacement characters to show in password mode
 * @param obj       pointer to a text area object
 * @return          pointer to the replacement text
 */
const char * lv_textarea_get_password_bullet(lv_obj_t * obj);

/**
 * Get the one line configuration attribute
 * @param obj       pointer to a text area object
 * @return          true: one line configuration is enabled, false: disabled
 */
bool lv_textarea_get_one_line(const lv_obj_t * obj);

/**
 * Get a list of accepted characters.
 * @param obj       pointer to a text area object
 * @return          list of accented characters.
 */
const char * lv_textarea_get_accepted_chars(lv_obj_t * obj);

/**
 * Get max length of a Text Area.
 * @param obj       pointer to a text area object
 * @return          the maximal number of characters to be add
 */
uint32_t lv_textarea_get_max_length(lv_obj_t * obj);

/**
 * Find whether text is selected or not.
 * @param obj       pointer to a text area object
 * @return          whether text is selected or not
 */
bool lv_textarea_text_is_selected(const lv_obj_t * obj);

/**
 * Find whether selection mode is enabled.
 * @param obj       pointer to a text area object
 * @return          true: selection mode is enabled, false: disabled
 */
bool lv_textarea_get_text_selection(lv_obj_t * obj);

/**
 * Set how long show the password before changing it to '*'
 * @param obj       pointer to a text area object
 * @return          show time in milliseconds. 0: hide immediately.
 */
uint32_t lv_textarea_get_password_show_time(lv_obj_t * obj);

/**
 * Get a the character from the current cursor position
 * @param obj       pointer to a text area object
 * @return          a the character or 0
 */
uint32_t lv_textarea_get_current_char(lv_obj_t * obj);

/*=====================
 * Other functions
 *====================*/

/**
 * Clear the selection on the text area.
 * @param obj       pointer to a text area object
 */
void lv_textarea_clear_selection(lv_obj_t * obj);

/**
 * Move the cursor one character right
 * @param obj       pointer to a text area object
 */
void lv_textarea_cursor_right(lv_obj_t * obj);

/**
 * Move the cursor one character left
 * @param obj       pointer to a text area object
 */
void lv_textarea_cursor_left(lv_obj_t * obj);

/**
 * Move the cursor one line down
 * @param obj       pointer to a text area object
 */
void lv_textarea_cursor_down(lv_obj_t * obj);

/**
 * Move the cursor one line up
 * @param obj       pointer to a text area object
 */
void lv_textarea_cursor_up(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEXTAREA_H*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEXTAREA_H*/
