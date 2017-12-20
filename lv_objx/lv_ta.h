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
#include "../../lv_conf.h"
#if USE_LV_TA != 0

/*Testing of dependencies*/
#if USE_LV_PAGE == 0
#error "lv_ta: lv_page is required. Enable it in lv_conf.h (USE_LV_PAGE  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_ta: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "lv_page.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/
#define LV_TA_CURSOR_LAST (0x7FFF) /*Put the cursor after the last character*/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_CURSOR_NONE,
	LV_CURSOR_LINE,
	LV_CURSOR_BLOCK,
	LV_CURSOR_OUTLINE,
	LV_CURSOR_UNDERLINE,
    LV_CURSOR_HIDDEN = 0x10,    /*Or it to any value to hide the cursor temporally*/
}lv_cursor_type_t;

/*Data of text area*/
typedef struct
{
    lv_page_ext_t page; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * label;           /*Label of the text area*/
    char * pwd_tmp;             /*Used to store the original text in password mode*/
    uint8_t pwd_mode :1;        /*Replace characters with '*' */
    uint8_t one_line :1;        /*One line mode (ignore line breaks)*/
    struct {
        lv_style_t *style;      /*Style of the cursor (NULL to use label's style)*/
        lv_coord_t valid_x;         /*Used when stepping up/down in text area when stepping to a shorter line. (Handled by the library)*/
        uint16_t pos;           /*The current cursor position (0: before 1. letter; 1: before 2. letter etc.)*/
        lv_cursor_type_t type;  /*Shape of the cursor*/
        uint8_t state :1;       /*Indicates that the cursor is visible now or not (Handled by the library)*/
    }cursor;
}lv_ta_ext_t;

typedef enum {
    LV_TA_STYLE_BG,
    LV_TA_STYLE_SB,
    LV_TA_STYLE_CURSOR,
}lv_ta_style_t;

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


/*======================
 * Add/remove functions
 *=====================*/

/**
 * Insert a character to the current cursor position
 * @param ta pointer to a text area object
 * @param c a character
 */
void lv_ta_add_char(lv_obj_t * ta, char c);

/**
 * Insert a text to the current cursor position
 * @param ta pointer to a text area object
 * @param txt a '\0' terminated string to insert
 */
void lv_ta_add_text(lv_obj_t * ta, const char * txt);

/**
 * Delete a the left character from the current cursor position
 * @param ta pointer to a text area object
 */
void lv_ta_del_char(lv_obj_t * ta);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a text area
 * @param ta pointer to a text area
 * @param txt pointer to the text
 */
void lv_ta_set_text(lv_obj_t * ta, const char * txt);

/**
 * Set the cursor position
 * @param obj pointer to a text area object
 * @param pos the new cursor position in character index
 *             < 0 : index from the end of the text
 *             LV_TA_CURSOR_LAST: go after the last character
 */
void lv_ta_set_cursor_pos(lv_obj_t * ta, int16_t pos);

/**
 * Set the cursor type.
 * @param ta pointer to a text area object
 * @param cur_type: element of 'lv_cursor_type_t'
 */
void lv_ta_set_cursor_type(lv_obj_t * ta, lv_cursor_type_t cur_type);
/**
 * Enable/Disable password mode
 * @param ta pointer to a text area object
 * @param pwd_en true: enable, false: disable
 */
void lv_ta_set_pwd_mode(lv_obj_t * ta, bool pwd_en);

/**
 * Configure the text area to one line or back to normal
 * @param ta pointer to a Text area object
 * @param en true: one line, false: normal
 */
void lv_ta_set_one_line(lv_obj_t * ta, bool en);

/**
 * Set the scroll bar mode of a text area
 * @param ta pointer to a text area object
 * @param sb_mode the new mode from 'lv_page_sb_mode_t' enum
 */
static inline void lv_ta_set_sb_mode(lv_obj_t * ta, lv_sb_mode_t mode)
{
    lv_page_set_sb_mode(ta, mode);
}

/**
 * Set a style of a text area
 * @param ta pointer to a text area object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_ta_set_style(lv_obj_t *ta, lv_ta_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a text area
 * @param ta pointer to a text area object
 * @return pointer to the text
 */
const char * lv_ta_get_text(lv_obj_t * ta);

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
 * @return element of 'lv_cursor_type_t'
 */
lv_cursor_type_t lv_ta_get_cursor_type(lv_obj_t * ta);

/**
 * Get the password mode attribute
 * @param ta pointer to a text area object
 * @return true: password mode is enabled, false: disabled
 */
bool lv_ta_get_pwd_mode(lv_obj_t * ta);

/**
 * Get the one line configuration attribute
 * @param ta pointer to a text area object
 * @return true: one line configuration is enabled, false: disabled
 */
bool lv_ta_get_one_line(lv_obj_t * ta);

/**
 * Get the scroll bar mode of a text area
 * @param ta pointer to a text area object
 * @return scrollbar mode from 'lv_page_sb_mode_t' enum
 */
static inline lv_sb_mode_t lv_ta_get_sb_mode(lv_obj_t * ta)
{
    return lv_page_get_sb_mode(ta);
}

/**
 * Get a style of a text area
 * @param ta pointer to a text area object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_ta_get_style(lv_obj_t *ta, lv_ta_style_t type);

/*=====================
 * Other functions
 *====================*/

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

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_TA_H*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_TA_H*/
