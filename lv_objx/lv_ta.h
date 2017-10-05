/**
 * @file lv_ta.h
 * 
 */

#ifndef LV_TA_H
#define LV_TA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_TA != 0

/*Testing of dependencies*/
#if USE_LV_PAGE == 0
#error "lv_ta: lv_page is required. Enable it in lv_conf.h (USE_LV_PAGE  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_ta: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif

#include "../lv_obj/lv_obj.h"
#include "lv_page.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/
#define LV_TA_CUR_LAST (0x7FFF) /*Put the cursor after the last character*/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
	LV_TA_CURSOR_LINE,
	LV_TA_CURSOR_BLOCK,
	LV_TA_CURSOR_OUTLINE,
	LV_TA_CURSOR_UNDERLINE,
}lv_ta_cursor_type_t;

/*Data of text area*/
typedef struct
{
    lv_page_ext_t page; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * label;           /*Label of the text area*/
    lv_style_t * cursor_style;	/*Style of the cursor (NULL to use label's style)*/
    char * pwd_tmp;             /*Used to store the original text in password mode*/
    cord_t cursor_valid_x;      /*Used when stepping up/down in text area when stepping to a shorter line. (Handled by the library)*/
    uint16_t cursor_pos;        /*The current cursor position (0: before 1. letter; 1: before 2. letter etc.)*/
    lv_ta_cursor_type_t cursor_type;	/*Shape of the cursor*/
    uint8_t cursor_show :1;     /*Show or hide cursor */
    uint8_t pwd_mode :1;        /*Replace characters with '*' */
    uint8_t one_line :1;        /*One line mode (ignore line breaks)*/
    uint8_t cursor_state :1;    /*Indicates that the cursor is visible now or not (Handled by the library)*/
}lv_ta_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a text area objects
 * @param par pointer to an object, it will be the parent of the new text area
 * @param copy pointer to a text area object, if not NULL then the new object will be copied from it
 * @return pointer to the created text area
 */
lv_obj_t * lv_ta_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the text area
 * @param ta pointer to a text area object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_ta_signal(lv_obj_t * ta, lv_signal_t sign, void * param);

/**
 * Signal function of the scrollable part of the text area
 * @param scrl pointer to scrollable part of a text area object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_ta_scrl_signal(lv_obj_t * scrl, lv_signal_t sign, void * param);

/**
 * Insert a character to the current cursor position
 * @param ta pointer to a text area object
 * @param c a character
 */
void lv_ta_add_char(lv_obj_t * ta, uint32_t c);

/**
 * Insert a text to the current cursor position
 * @param ta pointer to a text area object
 * @param txt a '\0' terminated string to insert
 */
void lv_ta_add_text(lv_obj_t * ta, const char * txt);

/**
 * Set the text of a text area
 * @param ta pointer to a text area
 * @param txt pointer to the text
 */
void lv_ta_set_text(lv_obj_t * ta, const char * txt);

/**
 * Delete a the left character from the current cursor position
 * @param ta pointer to a text area object
 */
void lv_ta_del(lv_obj_t * ta);

/**
 * Set the cursor position
 * @param obj pointer to a text area object
 * @param pos the new cursor position in character index
 *             < 0 : index from the end of the text
 *             LV_TA_CUR_LAST: go after the last character
 */
void lv_ta_set_cursor_pos(lv_obj_t * ta, int16_t pos);

/**
 * Move the cursor one character right
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_right(lv_obj_t * ta);

/**
 * Move the cursor one character left
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_left(lv_obj_t * ta);

/**
 * Move the cursor one line down
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_down(lv_obj_t * ta);

/**
 * Move the cursor one line up
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_up(lv_obj_t * ta);

/**
 * Get the current cursor visibility.
 * @param ta pointer to a text area object
 * @return show true: show the cursor and blink it, false: hide cursor
 */
void lv_ta_set_cursor_show(lv_obj_t * ta, bool show);

/**
 * Set the cursor type.
 * @param ta pointer to a text area object
 * @return cur_type: element of 'lv_ta_cursor_type_t'
 */
void lv_ta_set_cursor_type(lv_obj_t * ta, lv_ta_cursor_type_t cur_type);

/**
 * Set the style of the cursor (NULL to use label's style)
 * @param ta pointer to a text area object
 * @param style pointer to the new cursor style
 */
void lv_ta_set_cursor_style(lv_obj_t * ta, lv_style_t * style);

/**
 * Enable/Disable password mode
 * @param ta ointer to a text area object
 * @param en true: enable, false: disable
 */
void lv_ta_set_pwd_mode(lv_obj_t * ta, bool en);

/**
 * Configure the Text area to one line or back to normal
 * @param ta pointer to a text area object
 * @param en true: one line, false: normal
 */
void lv_ta_set_one_line(lv_obj_t * ta, bool en);

/**
 * Get the text of the i the text area
 * @param ta obj pointer to a text area object
 * @return pointer to the text
 */
const char * lv_ta_get_txt(lv_obj_t * ta);

/**
 * Get the label of a text area
 * @param ta pointer to a text area object
 * @return pointer to the label object
 */
lv_obj_t * lv_ta_get_label(lv_obj_t * ta);

/**
 * Get the current cursor position in character index
 * @param ta pointer to a text area object
 * @return the cursor position
 */
uint16_t lv_ta_get_cursor_pos(lv_obj_t * ta);

/**
 * Get the current cursor visibility.
 * @param ta pointer to a text area object
 * @return true: the cursor is drawn, false: the cursor is hidden
 */
bool lv_ta_get_cursor_show(lv_obj_t * ta);

/**
 * Get the current cursor type.
 * @param ta pointer to a text area object
 * @return element of 'lv_ta_cursor_type_t'
 */
lv_ta_cursor_type_t lv_ta_get_cursor_type(lv_obj_t * ta);

/**
 * Get the style of the cursor
 * @param ta pointer to a text area object
 * @return style pointer to the new cursor style
 */
lv_style_t *  lv_ta_get_cursor_style(lv_obj_t * ta);

/**
 * Get the password mode
 * @param ta pointer to a text area object
 * @return true: password mode is enabled, false: disabled
 */
bool lv_ta_get_pwd_mode(lv_obj_t * ta);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_TA_H*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_TA_H*/
