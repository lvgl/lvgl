/**
 * @file lv_btnm.h
 *
 */

#ifndef LV_BTNMATRIX_H
#define LV_BTNMATRIX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_BTNMATRIX != 0

#include "../lv_core/lv_obj.h"
#include "lv_label.h"
#include "lv_btn.h"

/*********************
 *      DEFINES
 *********************/
#define LV_BTNMATRIX_WIDTH_MASK 0x0007
#define LV_BTNMATRIX_BTN_NONE 0xFFFF

LV_EXPORT_CONST_INT(LV_BTNMATRIX_BTN_NONE);

/**********************
 *      TYPEDEFS
 **********************/

/** Type to store button control bits (disabled, hidden etc.)
 * The first 3 bits are used to store the width */
enum {
    LV_BTNMATRIX_CTRL_HIDDEN     = 0x0008, /**< Button hidden */
    LV_BTNMATRIX_CTRL_NO_REPEAT  = 0x0010, /**< Do not repeat press this button. */
    LV_BTNMATRIX_CTRL_DISABLED   = 0x0020, /**< Disable this button. */
    LV_BTNMATRIX_CTRL_CHECKABLE  = 0x0040, /**< Button *can* be toggled. */
    LV_BTNMATRIX_CTRL_CHECK_STATE  = 0x0080, /**< Button is currently toggled (e.g. checked). */
    LV_BTNMATRIX_CTRL_CLICK_TRIG = 0x0100, /**< 1: Send LV_EVENT_SELECTED on CLICK, 0: Send LV_EVENT_SELECTED on PRESS*/
};
typedef uint16_t lv_btnmatrix_ctrl_t;

/*Data of button matrix*/
typedef struct {
    /*No inherited ext.*/ /*Ext. of ancestor*/
    /*New data for this type */
    const char ** map_p;                              /*Pointer to the current map*/
    lv_area_t * button_areas;                         /*Array of areas of buttons*/
    lv_btnmatrix_ctrl_t * ctrl_bits;                       /*Array of control bytes*/
    lv_style_list_t style_btn;                     /*Styles of buttons in each state*/
    uint16_t btn_cnt;                                 /*Number of button in 'map_p'(Handled by the library)*/
    uint16_t btn_id_pr;                               /*Index of the currently pressed button or LV_BTNMATRIX_BTN_NONE*/
    uint16_t btn_id_focused;                          /*Index of the currently focused button or LV_BTNMATRIX_BTN_NONE*/
    uint16_t btn_id_act;    /*Index of the active button (being pressed/released etc) or LV_BTNMATRIX_BTN_NONE */
    uint8_t recolor : 1;    /*Enable button recoloring*/
    uint8_t one_check : 1;  /*Single button toggled at once*/
    uint8_t align : 2;      /*Align type from 'lv_label_align_t'*/
} lv_btnmatrix_ext_t;

enum {
    LV_BTNMATRIX_PART_BG,
    LV_BTNMATRIX_PART_BTN,
};
typedef uint8_t lv_btnmatrix_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a button matrix objects
 * @param par pointer to an object, it will be the parent of the new button matrix
 * @param copy pointer to a button matrix object, if not NULL then the new object will be copied
 * from it
 * @return pointer to the created button matrix
 */
lv_obj_t * lv_btnmatrix_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new map. Buttons will be created/deleted according to the map. The
 * button matrix keeps a reference to the map and so the string array must not
 * be deallocated during the life of the matrix.
 * @param btnm pointer to a button matrix object
 * @param map pointer a string array. The last string has to be: "". Use "\n" to make a line break.
 */
void lv_btnmatrix_set_map(lv_obj_t * btnm, const char * map[]);

/**
 * Set the button control map (hidden, disabled etc.) for a button matrix. The
 * control map array will be copied and so may be deallocated after this
 * function returns.
 * @param btnm pointer to a button matrix object
 * @param ctrl_map pointer to an array of `lv_btn_ctrl_t` control bytes. The
 *                 length of the array and position of the elements must match
 *                 the number and order of the individual buttons (i.e. excludes
 *                 newline entries).
 *                 An element of the map should look like e.g.:
 *                 `ctrl_map[0] = width | LV_BTNMATRIX_CTRL_NO_REPEAT | LV_BTNMATRIX_CTRL_TGL_ENABLE`
 */
void lv_btnmatrix_set_ctrl_map(lv_obj_t * btnm, const lv_btnmatrix_ctrl_t ctrl_map[]);

/**
 * Set the focused button i.e. visually highlight it.
 * @param btnm pointer to button matrix object
 * @param id index of the button to focus(`LV_BTNMATRIX_BTN_NONE` to remove focus)
 */
void lv_btnmatrix_set_focused_btn(lv_obj_t * btnm, uint16_t id);

/**
 * Enable recoloring of button's texts
 * @param btnm pointer to button matrix object
 * @param en true: enable recoloring; false: disable
 */
void lv_btnmatrix_set_recolor(const lv_obj_t * btnm, bool en);

/**
 * Set the attributes of a button of the button matrix
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify. (Not counting new lines)
 */
void lv_btnmatrix_set_btn_ctrl(lv_obj_t * btnm, uint16_t btn_id, lv_btnmatrix_ctrl_t ctrl);

/**
 * Clear the attributes of a button of the button matrix
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify. (Not counting new lines)
 */
void lv_btnmatrix_clear_btn_ctrl(const lv_obj_t * btnm, uint16_t btn_id, lv_btnmatrix_ctrl_t ctrl);

/**
 * Set the attributes of all buttons of a button matrix
 * @param btnm pointer to a button matrix object
 * @param ctrl attribute(s) to set from `lv_btnmatrix_ctrl_t`. Values can be ORed.
 */
void lv_btnmatrix_set_btn_ctrl_all(lv_obj_t * btnm, lv_btnmatrix_ctrl_t ctrl);

/**
 * Clear the attributes of all buttons of a button matrix
 * @param btnm pointer to a button matrix object
 * @param ctrl attribute(s) to set from `lv_btnmatrix_ctrl_t`. Values can be ORed.
 * @param en true: set the attributes; false: clear the attributes
 */
void lv_btnmatrix_clear_btn_ctrl_all(lv_obj_t * btnm, lv_btnmatrix_ctrl_t ctrl);

/**
 * Set a single buttons relative width.
 * This method will cause the matrix be regenerated and is a relatively
 * expensive operation. It is recommended that initial width be specified using
 * `lv_btnmatrix_set_ctrl_map` and this method only be used for dynamic changes.
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify.
 * @param width Relative width compared to the buttons in the same row. [1..7]
 */
void lv_btnmatrix_set_btn_width(lv_obj_t * btnm, uint16_t btn_id, uint8_t width);

/**
 * Make the button matrix like a selector widget (only one button may be toggled at a time).
 * `Checkable` must be enabled on the buttons you want to be selected with `lv_btnmatrix_set_ctrl` or
 * `lv_btnmatrix_set_btn_ctrl_all`.
 * @param btnm Button matrix object
 * @param one_chk Whether "one check" mode is enabled
 */
void lv_btnmatrix_set_one_check(lv_obj_t * btnm, bool one_chk);

/**
 * Set the align of the map text (left, right or center)
 * @param btnm pointer to a btnmatrix object
 * @param align LV_LABEL_ALIGN_LEFT, LV_LABEL_ALIGN_RIGHT or LV_LABEL_ALIGN_CENTER
 */
void lv_btnmatrix_set_align(lv_obj_t * btnm, lv_label_align_t align);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current map of a button matrix
 * @param btnm pointer to a button matrix object
 * @return the current map
 */
const char ** lv_btnmatrix_get_map_array(const lv_obj_t * btnm);

/**
 * Check whether the button's text can use recolor or not
 * @param btnm pointer to button matrix object
 * @return true: text recolor enable; false: disabled
 */
bool lv_btnmatrix_get_recolor(const lv_obj_t * btnm);

/**
 * Get the index of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the `event_cb` to get the text of the button, check if hidden etc.
 * @param btnm pointer to button matrix object
 * @return index of the last released button (LV_BTNMATRIX_BTN_NONE: if unset)
 */
uint16_t lv_btnmatrix_get_active_btn(const lv_obj_t * btnm);

/**
 * Get the text of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the `event_cb`
 * @param btnm pointer to button matrix object
 * @return text of the last released button (NULL: if unset)
 */
const char * lv_btnmatrix_get_active_btn_text(const lv_obj_t * btnm);

/**
 * Get the focused button's index.
 * @param btnm pointer to button matrix object
 * @return index of the focused button (LV_BTNMATRIX_BTN_NONE: if unset)
 */
uint16_t lv_btnmatrix_get_focused_btn(const lv_obj_t * btnm);

/**
 * Get the button's text
 * @param btnm pointer to button matrix object
 * @param btn_id the index a button not counting new line characters. (The return value of
 * lv_btnmatrix_get_pressed/released)
 * @return text of btn_index` button
 */
const char * lv_btnmatrix_get_btn_text(const lv_obj_t * btnm, uint16_t btn_id);

/**
 * Get the whether a control value is enabled or disabled for button of a button matrix
 * @param btnm pointer to a button matrix object
 * @param btn_id the index a button not counting new line characters. (E.g. the return value of
 * lv_btnmatrix_get_pressed/released)
 * @param ctrl control values to check (ORed value can be used)
 * @return true: long press repeat is disabled; false: long press repeat enabled
 */
bool lv_btnmatrix_get_btn_ctrl(lv_obj_t * btnm, uint16_t btn_id, lv_btnmatrix_ctrl_t ctrl);

/**
 * Find whether "one toggle" mode is enabled.
 * @param btnm Button matrix object
 * @return whether "one toggle" mode is enabled
 */
bool lv_btnmatrix_get_one_check(const lv_obj_t * btnm);

/**
 * Get the align attribute
 * @param btnm pointer to a btnmatrix object
 * @return LV_LABEL_ALIGN_LEFT, LV_LABEL_ALIGN_RIGHT or LV_LABEL_ALIGN_CENTER
 */
lv_label_align_t lv_btnmatrix_get_align(const lv_obj_t * btnm);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BTNMATRIX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BTNMATRIX_H*/
