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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#if LV_USE_TA != 0

/*Testing of dependencies*/
#if LV_USE_PAGE == 0
#error "lv_ta: lv_page is required. Enable it in lv_conf.h (LV_USE_PAGE  1) "
#endif

#if LV_USE_LABEL == 0
#error "lv_ta: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
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

enum {
    LV_CURSOR_NONE,
    LV_CURSOR_LINE,
    LV_CURSOR_BLOCK,
    LV_CURSOR_OUTLINE,
    LV_CURSOR_UNDERLINE,
    LV_CURSOR_HIDDEN = 0x08, /*Or it to any value to hide the cursor temporally*/
};
typedef uint8_t lv_cursor_type_t;

/*Data of text area*/
typedef struct
{
    lv_page_ext_t page; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * label;            /*Label of the text area*/
    lv_obj_t * placeholder;      /*Place holder label. only visible if text is an empty string*/
    char * pwd_tmp;              /*Used to store the original text in password mode*/
    const char * accapted_chars; /*Only these characters will be accepted. NULL: accept all*/
    uint16_t max_length;         /*The max. number of characters. 0: no limit*/
    uint16_t pwd_show_time;      /*Time to show characters in password mode before change them to '*' */
    struct
    {
        const lv_style_t * style;  /* Style of the cursor (NULL to use label's style)*/
        lv_coord_t valid_x;        /* Used when stepping up/down to a shorter line.
                                    * (Used by the library)*/
        uint16_t pos;              /* The current cursor position
                                    * (0: before 1st letter; 1: before 2nd letter ...)*/
        uint16_t blink_time;       /*Blink period*/
        lv_area_t area;            /* Cursor area relative to the Text Area*/
        uint16_t txt_byte_pos;     /* Byte index of the letter after (on) the cursor*/
        lv_cursor_type_t type : 4; /* Shape of the cursor*/
        uint8_t state : 1;         /*Cursor is visible now or not (Handled by the library)*/
        uint8_t click_pos : 1;     /*1: Enable positioning the cursor by clicking the text area*/
    } cursor;
#if LV_LABEL_TEXT_SEL
    uint16_t tmp_sel_start;       /*Temporary value*/
    uint16_t tmp_sel_end;         /*Temporary value*/
    uint8_t text_sel_in_prog : 1; /*User is in process of selecting */
    uint8_t text_sel_en : 1;      /*Text can be selected on this text area*/
#endif
    uint8_t pwd_mode : 1; /*Replace characters with '*' */
    uint8_t one_line : 1; /*One line mode (ignore line breaks)*/
} lv_ta_ext_t;

enum {
    LV_TA_STYLE_BG,
    LV_TA_STYLE_SB,
    LV_TA_STYLE_CURSOR,
    LV_TA_STYLE_EDGE_FLASH,
    LV_TA_STYLE_PLACEHOLDER,
};
typedef uint8_t lv_ta_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a text area objects
 * @param par pointer to an object, it will be the parent of the new text area
 * @param copy pointer to a text area object, if not NULL then the new object will be copied from it
 * @return pointer to the created text area
 */
lv_obj_t * lv_ta_create(lv_obj_t * par, const lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Insert a character to the current cursor position.
 * To add a wide char, e.g. 'Á' use `lv_txt_encoded_conv_wc('Á')`
 * @param ta pointer to a text area object
 * @param c a character (e.g. 'a')
 */
void lv_ta_add_char(lv_obj_t * ta, uint32_t c);

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

/**
 * Delete the right character from the current cursor position
 * @param ta pointer to a text area object
 */
void lv_ta_del_char_forward(lv_obj_t * ta);

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
 * Set the placeholder text of a text area
 * @param ta pointer to a text area
 * @param txt pointer to the text
 */
void lv_ta_set_placeholder_text(lv_obj_t * ta, const char * txt);

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
 * Enable/Disable the positioning of the the cursor by clicking the text on the text area.
 * @param ta pointer to a text area object
 * @param en true: enable click positions; false: disable
 */
void lv_ta_set_cursor_click_pos(lv_obj_t * ta, bool en);

/**
 * Enable/Disable password mode
 * @param ta pointer to a text area object
 * @param en true: enable, false: disable
 */
void lv_ta_set_pwd_mode(lv_obj_t * ta, bool en);

/**
 * Configure the text area to one line or back to normal
 * @param ta pointer to a Text area object
 * @param en true: one line, false: normal
 */
void lv_ta_set_one_line(lv_obj_t * ta, bool en);

/**
 * Set the alignment of the text area.
 * In one line mode the text can be scrolled only with `LV_LABEL_ALIGN_LEFT`.
 * This function should be called if the size of text area changes.
 * @param ta pointer to a text are object
 * @param align the desired alignment from `lv_label_align_t`. (LV_LABEL_ALIGN_LEFT/CENTER/RIGHT)
 */
void lv_ta_set_text_align(lv_obj_t * ta, lv_label_align_t align);

/**
 * Set a list of characters. Only these characters will be accepted by the text area
 * @param ta pointer to  Text Area
 * @param list list of characters. Only the pointer is saved. E.g. "+-.,0123456789"
 */
void lv_ta_set_accepted_chars(lv_obj_t * ta, const char * list);

/**
 * Set max length of a Text Area.
 * @param ta pointer to  Text Area
 * @param num the maximal number of characters can be added (`lv_ta_set_text` ignores it)
 */
void lv_ta_set_max_length(lv_obj_t * ta, uint16_t num);

/**
 * In `LV_EVENT_INSERT` the text which planned to be inserted can be replaced by an other text.
 * It can be used to add automatic formatting to the text area.
 * @param ta pointer to a text area.
 * @param txt pointer to a new string to insert. If `""` no text will be added.
 *            The variable must be live after the `event_cb` exists. (Should be `global` or
 * `static`)
 */
void lv_ta_set_insert_replace(lv_obj_t * ta, const char * txt);

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
 * Enable the scroll propagation feature. If enabled then the Text area will move its parent if
 * there is no more space to scroll.
 * @param ta pointer to a Text area
 * @param en true or false to enable/disable scroll propagation
 */
static inline void lv_ta_set_scroll_propagation(lv_obj_t * ta, bool en)
{
    lv_page_set_scroll_propagation(ta, en);
}

/**
 * Enable the edge flash effect. (Show an arc when the an edge is reached)
 * @param page pointer to a Text Area
 * @param en true or false to enable/disable end flash
 */
static inline void lv_ta_set_edge_flash(lv_obj_t * ta, bool en)
{
    lv_page_set_edge_flash(ta, en);
}

/**
 * Set a style of a text area
 * @param ta pointer to a text area object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_ta_set_style(lv_obj_t * ta, lv_ta_style_t type, const lv_style_t * style);

/**
 * Enable/disable selection mode.
 * @param ta pointer to a text area object
 * @param en true or false to enable/disable selection mode
 */
void lv_ta_set_text_sel(lv_obj_t * ta, bool en);

/**
 * Set how long show the password before changing it to '*'
 * @param ta pointer to Text area
 * @param time show time in milliseconds. 0: hide immediately.
 */
void lv_ta_set_pwd_show_time(lv_obj_t * ta, uint16_t time);

/**
 * Set cursor blink animation time
 * @param ta pointer to Text area
 * @param time blink period. 0: disable blinking
 */
void lv_ta_set_cursor_blink_time(lv_obj_t * ta, uint16_t time);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a text area. In password mode it gives the real text (not '*'s).
 * @param ta pointer to a text area object
 * @return pointer to the text
 */
const char * lv_ta_get_text(const lv_obj_t * ta);

/**
 * Get the placeholder text of a text area
 * @param ta pointer to a text area object
 * @return pointer to the text
 */
const char * lv_ta_get_placeholder_text(lv_obj_t * ta);

/**
 * Get the label of a text area
 * @param ta pointer to a text area object
 * @return pointer to the label object
 */
lv_obj_t * lv_ta_get_label(const lv_obj_t * ta);

/**
 * Get the current cursor position in character index
 * @param ta pointer to a text area object
 * @return the cursor position
 */
uint16_t lv_ta_get_cursor_pos(const lv_obj_t * ta);

/**
 * Get the current cursor type.
 * @param ta pointer to a text area object
 * @return element of 'lv_cursor_type_t'
 */
lv_cursor_type_t lv_ta_get_cursor_type(const lv_obj_t * ta);

/**
 * Get whether the cursor click positioning is enabled or not.
 * @param ta pointer to a text area object
 * @return true: enable click positions; false: disable
 */
bool lv_ta_get_cursor_click_pos(lv_obj_t * ta);

/**
 * Get the password mode attribute
 * @param ta pointer to a text area object
 * @return true: password mode is enabled, false: disabled
 */
bool lv_ta_get_pwd_mode(const lv_obj_t * ta);

/**
 * Get the one line configuration attribute
 * @param ta pointer to a text area object
 * @return true: one line configuration is enabled, false: disabled
 */
bool lv_ta_get_one_line(const lv_obj_t * ta);

/**
 * Get a list of accepted characters.
 * @param ta pointer to  Text Area
 * @return list of accented characters.
 */
const char * lv_ta_get_accepted_chars(lv_obj_t * ta);

/**
 * Set max length of a Text Area.
 * @param ta pointer to  Text Area
 * @return the maximal number of characters to be add
 */
uint16_t lv_ta_get_max_length(lv_obj_t * ta);

/**
 * Get the scroll bar mode of a text area
 * @param ta pointer to a text area object
 * @return scrollbar mode from 'lv_page_sb_mode_t' enum
 */
static inline lv_sb_mode_t lv_ta_get_sb_mode(const lv_obj_t * ta)
{
    return lv_page_get_sb_mode(ta);
}

/**
 * Get the scroll propagation property
 * @param ta pointer to a Text area
 * @return true or false
 */
static inline bool lv_ta_get_scroll_propagation(lv_obj_t * ta)
{
    return lv_page_get_scroll_propagation(ta);
}

/**
 * Get the scroll propagation property
 * @param ta pointer to a Text area
 * @return true or false
 */
static inline bool lv_ta_get_edge_flash(lv_obj_t * ta)
{
    return lv_page_get_edge_flash(ta);
}

/**
 * Get a style of a text area
 * @param ta pointer to a text area object
 * @param type which style should be get
 * @return style pointer to a style
 */
const lv_style_t * lv_ta_get_style(const lv_obj_t * ta, lv_ta_style_t type);

/**
 * Find whether text is selected or not.
 * @param ta Text area object
 * @return whether text is selected or not
 */
bool lv_ta_text_is_selected(const lv_obj_t * ta);

/**
 * Find whether selection mode is enabled.
 * @param ta pointer to a text area object
 * @return true: selection mode is enabled, false: disabled
 */
bool lv_ta_get_text_sel_en(lv_obj_t * ta);

/**
 * Set how long show the password before changing it to '*'
 * @param ta pointer to Text area
 * @return show time in milliseconds. 0: hide immediately.
 */
uint16_t lv_ta_get_pwd_show_time(lv_obj_t * ta);

/**
 * Set cursor blink animation time
 * @param ta pointer to Text area
 * @return time blink period. 0: disable blinking
 */
uint16_t lv_ta_get_cursor_blink_time(lv_obj_t * ta);

/*=====================
 * Other functions
 *====================*/

/**
 * Clear the selection on the text area.
 * @param ta Text area object
 */
void lv_ta_clear_selection(lv_obj_t * ta);

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

#endif /*LV_USE_TA_H*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TA_H*/
