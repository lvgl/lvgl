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

/**********************
 *      TYPEDEFS
 **********************/

/*Data of text area*/
typedef struct {
    lv_obj_t obj;
    lv_obj_t * label;            /*Label of the text area*/
    char * placeholder_txt;      /*Place holder label. only visible if text is an empty string*/
    char * pwd_tmp;              /*Used to store the original text in password mode*/
    char * pwd_bullet;           /*Replacement characters displayed in password mode*/
    const char * accepted_chars; /*Only these characters will be accepted. NULL: accept all*/
    uint32_t max_length;         /*The max. number of characters. 0: no limit*/
    uint16_t pwd_show_time;      /*Time to show characters in password mode before change them to '*'*/
    struct {
        lv_coord_t valid_x;        /*Used when stepping up/down to a shorter line.
                                    *(Used by the library)*/
        uint32_t pos;              /*The current cursor position
                                    *(0: before 1st letter; 1: before 2nd letter ...)*/
        lv_area_t area;            /*Cursor area relative to the Text Area*/
        uint32_t txt_byte_pos;     /*Byte index of the letter after (on) the cursor*/
        uint8_t show : 1;          /*Cursor is visible now or not (Handled by the library)*/
        uint8_t click_pos : 1;     /*1: Enable positioning the cursor by clicking the text area*/
    } cursor;
#if LV_LABEL_TEXT_SELECTION
    uint32_t sel_start;  /*Temporary values for text selection*/
    uint32_t sel_end;
    uint8_t text_sel_in_prog : 1; /*User is in process of selecting*/
    uint8_t text_sel_en : 1;      /*Text can be selected on this text area*/
#endif
    uint8_t pwd_mode : 1; /*Replace characters with '*'*/
    uint8_t one_line : 1; /*One line mode (ignore line breaks)*/
} lv_textarea_t;

extern const lv_obj_class_t lv_textarea_class;

enum {
    LV_PART_TEXTAREA_PLACEHOLDER = LV_PART_CUSTOM_FIRST,
};

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
 * To add a wide char, e.g. 'Á' use `_lv_txt_encoded_conv_wc('Á')`
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
void lv_textarea_del_char(lv_obj_t * obj);

/**
 * Delete the right character from the current cursor position
 * @param obj       pointer to a text area object
 */
void lv_textarea_del_char_forward(lv_obj_t * obj);

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
 * In `LV_EVENT_INSERT` the text which planned to be inserted can be replaced by an other text.
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
void lv_textarea_set_password_show_time(lv_obj_t * obj, uint16_t time);

/**
 * Deprecated: use the normal text_align style property instead
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
uint16_t lv_textarea_get_password_show_time(lv_obj_t * obj);

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
