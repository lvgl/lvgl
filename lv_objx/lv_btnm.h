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
#include "../../lv_conf.h"
#endif

#if LV_USE_BTNM != 0

#include "../lv_core/lv_obj.h"
#include "lv_label.h"
#include "lv_btn.h"

/*********************
 *      DEFINES
 *********************/

/*Control byte*/
#define LV_BTNM_WIDTH_MASK              0x07
#define LV_BTNM_BTN_HIDDEN              0x08
#define LV_BTNM_BTN_NO_REPEAT           0x10
#define LV_BTNM_BTN_INACTIVE            0x20
#define LV_BTNM_BTN_TOGGLE              0x40
#define LV_BTNM_BTN_TOGGLE_STATE        0x80

#define LV_BTNM_BTN_NONE         0xFFFF
/**********************
 *      TYPEDEFS
 **********************/

/* Type to store button control bits (disabled, hidden etc.) */
typedef uint8_t lv_btnm_ctrl_t;

/*Data of button matrix*/
typedef struct
{
    /*No inherited ext.*/ /*Ext. of ancestor*/
    /*New data for this type */
    const char ** map_p;                        /*Pointer to the current map*/
    lv_area_t *button_areas;                    /*Array of areas of buttons*/
    lv_btnm_ctrl_t *ctrl_bits;                   /*Array of control bytes*/
    lv_style_t *styles_btn[LV_BTN_STATE_NUM];   /*Styles of buttons in each state*/
    uint16_t btn_cnt;                           /*Number of button in 'map_p'(Handled by the library)*/
    uint16_t btn_id_pr;                         /*Index of the currently pressed button or LV_BTNM_BTN_NONE*/
    uint16_t btn_id_act;                        /*Index of the active button (being pressed/released etc) or LV_BTNM_BTN_NONE */
    uint8_t	recolor    :1;                      /*Enable button recoloring*/
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
 * @param copy pointer to a button matrix object, if not NULL then the new object will be copied from it
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
void lv_btnm_set_map(const lv_obj_t * btnm, const char ** map);

/**
 * Set the button control map (hidden, disabled etc.) for a button matrix. The
 * control map array will be copied and so may be deallocated after this
 * function returns.
 * @param btnm pointer to a button matrix object
 * @param ctrl_map pointer to an array of `lv_btn_ctrl_t` control bytes. The
 *                 length of the array and position of the elements must match
 *                 the number and order of the individual buttons (i.e. excludes
 *                 newline entries).
 *                 The control bits are:
 *                 - bit 5   : 1 = inactive (disabled)
 *                 - bit 4   : 1 = no repeat (on long press)
 *                 - bit 3   : 1 = hidden
 *                 - bit 2..0: Relative width compared to the buttons in the
 *                             same row. [1..7]
 */
void lv_btnm_set_ctrl_map(const lv_obj_t * btnm, const lv_btnm_ctrl_t * ctrl_map);

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
void lv_btnm_set_style(lv_obj_t * btnm, lv_btnm_style_t type, lv_style_t * style);

/**
 * Enable recoloring of button's texts
 * @param btnm pointer to button matrix object
 * @param en true: enable recoloring; false: disable
 */
void lv_btnm_set_recolor(const lv_obj_t * btnm, bool en);

/**
 * Show/hide a single button in the matrix
 * @param btnm pointer to button matrix object
 * @param btn_idx 0 based index of the button to modify.
 * @param hidden true: hide the button
 */
void lv_btnm_set_btn_hidden(const lv_obj_t * btnm, uint16_t btn_idx, bool hidden);

/**
 * Enable/disable a single button in the matrix
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify.
 * @param ina true: make the button inactive
 */
void lv_btnm_set_btn_inactive(const lv_obj_t * btnm, uint16_t btn_id, bool ina);

/**
 * Enable/disable long press for a single button in the matrix
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify.
 * @param no_rep true: disable repeat
 */
void lv_btnm_set_btn_no_repeat(const lv_obj_t * btnm, uint16_t btn_id, bool no_rep);

/**
 * Enable/disable toggling a single button in the matrix
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify.
 * @param tgl true: toggle enable
 */
void lv_btnm_set_btn_toggle(const lv_obj_t * btnm, uint16_t btn_id, bool tgl);

/**
 * Make the a single button button toggled or not toggled.
 * @param btnm pointer to button matrix object
 * @param btn_id index of button (not counting "\n")
 * @param state true: toggled; false: not toggled
 */
void lv_btnm_set_btn_toggle_state(lv_obj_t * btnm, uint16_t btn_id, bool toggle);

/**
 * Set hidden/disabled/repeat flags for a single button.
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify.
 * @param hidden true: hide the button
 * @param inactive true: disable the button
 * @param no_repeat true: disable repeat
 * @param toggle true: enable toggling
 * @param toggled_state true: set toggled state
 */
void lv_btnm_set_btn_flags(const lv_obj_t * btnm, uint16_t btn_id, bool hidden, bool inactive, bool no_repeat, bool toggle, bool toggle_state);

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
 * Set the toggle state of all buttons
 * @param btnm pointer to a button matrix object
 * @param state true: toggled; false: not toggled
 */
void lv_btnm_set_btn_toggle_state_all(lv_obj_t * btnm, bool state);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current map of a button matrix
 * @param btnm pointer to a button matrix object
 * @return the current map
 */
const char ** lv_btnm_get_map(const lv_obj_t * btnm);

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
 * @param btn_index the index a button not counting new line characters. (The return value of lv_btnm_get_pressed/released)
 * @return  text of btn_index` button
 */
const char * lv_btnm_get_btn_text(const lv_obj_t * btnm, uint16_t btn_id);

/**
 * Check whether "no repeat" for a button is set or not.
 * The `LV_EVENT_LONG_PRESS_REPEAT` will be sent anyway but it can be ignored by the user if this function returns `true`
 * @param btnm pointer to a button matrix object
 * @param btn_index the index a button not counting new line characters. (The return value of lv_btnm_get_pressed/released)
 * @return true: long press repeat is disabled; false: long press repeat enabled
 */
bool lv_btnm_get_btn_no_repeate(lv_obj_t * btnm, uint16_t btn_id);

/**
 * Check whether a button for a button is hidden or not.
 * Events will be sent anyway but they can be ignored by the user if this function returns `true`
 * @param btnm pointer to a button matrix object
 * @param btn_id the index a button not counting new line characters. (The return value of lv_btnm_get_pressed/released)
 * @return true: hidden; false: not hidden
 */
bool lv_btnm_get_btn_hidden(lv_obj_t * btnm, uint16_t btn_id);

/**
 * Check whether a button for a button is inactive or not.
 * Events will be sent anyway but they can be ignored by the user if this function returns `true`
 * @param btnm pointer to a button matrix object
 * @param btn_id the index a button not counting new line characters. (The return value of lv_btnm_get_pressed/released)
 * @return true: inactive; false: not inactive
 */
bool lv_btnm_get_btn_inactive(lv_obj_t * btnm, uint16_t btn_id);

/**
 * Check if the button can be toggled or not
 * @param btnm pointer to button matrix object
 * @return  btn_id index a of a button not counting "\n". (The return value of lv_btnm_get_pressed/released)
 */
bool lv_btnm_get_btn_toggle(const lv_obj_t * btnm, int16_t btn_id);

/**
 * Check if the button is toggled or not
 * @param btnm pointer to button matrix object
 * @return  btn_id index a of a button not counting "\n". (The return value of lv_btnm_get_pressed/released)
 */
bool lv_btnm_get_btn_toggle_state(const lv_obj_t * btnm, int16_t btn_id);

/**
 * Get a style of a button matrix
 * @param btnm pointer to a button matrix object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_btnm_get_style(const lv_obj_t * btnm, lv_btnm_style_t type);
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BTNM*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BTNM_H*/
