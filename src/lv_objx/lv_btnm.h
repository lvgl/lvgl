/**
 * @file lv_btnm.h
 *
 */

#ifndef LV_BTNM_H
#define LV_BTNM_H

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

#if LV_USE_BTNM != 0

#include "../lv_core/lv_obj.h"
#include "lv_label.h"
#include "lv_btn.h"

/*********************
 *      DEFINES
 *********************/
#define LV_BTNM_WIDTH_MASK 0x0007
#define LV_BTNM_BTN_NONE 0xFFFF

LV_EXPORT_CONST_INT(LV_BTNM_BTN_NONE);

/**********************
 *      TYPEDEFS
 **********************/

/** Type to store button control bits (disabled, hidden etc.) */
enum {
    LV_BTNM_CTRL_HIDDEN     = 0x0008, /**< Button hidden */
    LV_BTNM_CTRL_NO_REPEAT  = 0x0010, /**< Do not repeat press this button. */
    LV_BTNM_CTRL_INACTIVE   = 0x0020, /**< Disable this button. */
    LV_BTNM_CTRL_TGL_ENABLE = 0x0040, /**< Button *can* be toggled. */
    LV_BTNM_CTRL_TGL_STATE  = 0x0080, /**< Button is currently toggled (e.g. checked). */
    LV_BTNM_CTRL_CLICK_TRIG = 0x0100, /**< 1: Send LV_EVENT_SELECTED on CLICK, 0: Send LV_EVENT_SELECTED on PRESS*/
};
typedef uint16_t lv_btnm_ctrl_t;

/*Data of button matrix*/
typedef struct
{
    /*No inherited ext.*/ /*Ext. of ancestor*/
    /*New data for this type */
    const char ** map_p;                              /*Pointer to the current map*/
    lv_area_t * button_areas;                         /*Array of areas of buttons*/
    lv_btnm_ctrl_t * ctrl_bits;                       /*Array of control bytes*/
    const lv_style_t * styles_btn[_LV_BTN_STATE_NUM]; /*Styles of buttons in each state*/
    uint16_t btn_cnt;                                 /*Number of button in 'map_p'(Handled by the library)*/
    uint16_t btn_id_pr;                               /*Index of the currently pressed button or LV_BTNM_BTN_NONE*/
    uint16_t btn_id_act;    /*Index of the active button (being pressed/released etc) or LV_BTNM_BTN_NONE */
    uint8_t recolor : 1;    /*Enable button recoloring*/
    uint8_t one_toggle : 1; /*Single button toggled at once*/
} lv_btnm_ext_t;

enum {
    LV_BTNM_STYLE_BG,
    LV_BTNM_STYLE_BTN_REL,
    LV_BTNM_STYLE_BTN_PR,
    LV_BTNM_STYLE_BTN_TGL_REL,
    LV_BTNM_STYLE_BTN_TGL_PR,
    LV_BTNM_STYLE_BTN_INA,
};
typedef uint8_t lv_btnm_style_t;

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
lv_obj_t * lv_btnm_create(lv_obj_t * par, const lv_obj_t * copy);

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
void lv_btnm_set_map(const lv_obj_t * btnm, const char * map[]);

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
 *                 `ctrl_map[0] = width | LV_BTNM_CTRL_NO_REPEAT |  LV_BTNM_CTRL_TGL_ENABLE`
 */
void lv_btnm_set_ctrl_map(const lv_obj_t * btnm, const lv_btnm_ctrl_t ctrl_map[]);

/**
 * Set the pressed button i.e. visually highlight it.
 * Mainly used a when the btnm is in a group to show the selected button
 * @param btnm pointer to button matrix object
 * @param id index of the currently pressed button (`LV_BTNM_BTN_NONE` to unpress)
 */
void lv_btnm_set_pressed(const lv_obj_t * btnm, uint16_t id);

/**
 * Set a style of a button matrix
 * @param btnm pointer to a button matrix object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_btnm_set_style(lv_obj_t * btnm, lv_btnm_style_t type, const lv_style_t * style);

/**
 * Enable recoloring of button's texts
 * @param btnm pointer to button matrix object
 * @param en true: enable recoloring; false: disable
 */
void lv_btnm_set_recolor(const lv_obj_t * btnm, bool en);

/**
 * Set the attributes of a button of the button matrix
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify. (Not counting new lines)
 */
void lv_btnm_set_btn_ctrl(const lv_obj_t * btnm, uint16_t btn_id, lv_btnm_ctrl_t ctrl);

/**
 * Clear the attributes of a button of the button matrix
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify. (Not counting new lines)
 */
void lv_btnm_clear_btn_ctrl(const lv_obj_t * btnm, uint16_t btn_id, lv_btnm_ctrl_t ctrl);

/**
 * Set the attributes of all buttons of a button matrix
 * @param btnm pointer to a button matrix object
 * @param ctrl attribute(s) to set from `lv_btnm_ctrl_t`. Values can be ORed.
 */
void lv_btnm_set_btn_ctrl_all(lv_obj_t * btnm, lv_btnm_ctrl_t ctrl);

/**
 * Clear the attributes of all buttons of a button matrix
 * @param btnm pointer to a button matrix object
 * @param ctrl attribute(s) to set from `lv_btnm_ctrl_t`. Values can be ORed.
 * @param en true: set the attributes; false: clear the attributes
 */
void lv_btnm_clear_btn_ctrl_all(lv_obj_t * btnm, lv_btnm_ctrl_t ctrl);

/**
 * Set a single buttons relative width.
 * This method will cause the matrix be regenerated and is a relatively
 * expensive operation. It is recommended that initial width be specified using
 * `lv_btnm_set_ctrl_map` and this method only be used for dynamic changes.
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify.
 * @param width Relative width compared to the buttons in the same row. [1..7]
 */
void lv_btnm_set_btn_width(const lv_obj_t * btnm, uint16_t btn_id, uint8_t width);

/**
 * Make the button matrix like a selector widget (only one button may be toggled at a time).
 *
 * Toggling must be enabled on the buttons you want to be selected with `lv_btnm_set_ctrl` or
 * `lv_btnm_set_btn_ctrl_all`.
 *
 * @param btnm Button matrix object
 * @param one_toggle Whether "one toggle" mode is enabled
 */
void lv_btnm_set_one_toggle(lv_obj_t * btnm, bool one_toggle);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current map of a button matrix
 * @param btnm pointer to a button matrix object
 * @return the current map
 */
const char ** lv_btnm_get_map_array(const lv_obj_t * btnm);

/**
 * Check whether the button's text can use recolor or not
 * @param btnm pointer to button matrix object
 * @return true: text recolor enable; false: disabled
 */
bool lv_btnm_get_recolor(const lv_obj_t * btnm);

/**
 * Get the index of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb` to get the text of the button, check if hidden etc.
 * @param btnm pointer to button matrix object
 * @return  index of the last released button (LV_BTNM_BTN_NONE: if unset)
 */
uint16_t lv_btnm_get_active_btn(const lv_obj_t * btnm);

/**
 * Get the text of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`
 * @param btnm pointer to button matrix object
 * @return text of the last released button (NULL: if unset)
 */
const char * lv_btnm_get_active_btn_text(const lv_obj_t * btnm);

/**
 * Get the pressed button's index.
 * The button be really pressed by the user or manually set to pressed with `lv_btnm_set_pressed`
 * @param btnm pointer to button matrix object
 * @return  index of the pressed button (LV_BTNM_BTN_NONE: if unset)
 */
uint16_t lv_btnm_get_pressed_btn(const lv_obj_t * btnm);

/**
 * Get the button's text
 * @param btnm pointer to button matrix object
 * @param btn_id the index a button not counting new line characters. (The return value of
 * lv_btnm_get_pressed/released)
 * @return  text of btn_index` button
 */
const char * lv_btnm_get_btn_text(const lv_obj_t * btnm, uint16_t btn_id);

/**
 * Get the whether a control value is enabled or disabled for button of a button matrix
 * @param btnm pointer to a button matrix object
 * @param btn_id the index a button not counting new line characters. (E.g. the return value of
 * lv_btnm_get_pressed/released)
 * @param ctrl control values to check (ORed value can be used)
 * @return true: long press repeat is disabled; false: long press repeat enabled
 */
bool lv_btnm_get_btn_ctrl(lv_obj_t * btnm, uint16_t btn_id, lv_btnm_ctrl_t ctrl);

/**
 * Get a style of a button matrix
 * @param btnm pointer to a button matrix object
 * @param type which style should be get
 * @return style pointer to a style
 */
const lv_style_t * lv_btnm_get_style(const lv_obj_t * btnm, lv_btnm_style_t type);

/**
 * Find whether "one toggle" mode is enabled.
 * @param btnm Button matrix object
 * @return whether "one toggle" mode is enabled
 */
bool lv_btnm_get_one_toggle(const lv_obj_t * btnm);
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BTNM*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BTNM_H*/
