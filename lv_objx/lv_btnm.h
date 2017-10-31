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
#include "lv_conf.h"
#if USE_LV_BTNM != 0

#include "../lv_obj/lv_obj.h"
#include "lv_label.h"
#include "lv_btn.h"

/*********************
 *      DEFINES
 *********************/

/*Control byte*/
#define LV_BTNM_CTRL_CODE       0x80    /*The control byte has to begin (if present) with 0b10xxxxxx*/
#define LV_BTNM_CTRL_MASK       0xC0
#define LV_BTNM_WIDTH_MASK      0x07
#define LV_BTNM_HIDE_MASK     0x08
#define LV_BTNM_REPEAT_DISABLE_MASK     0x10
#define LV_BTNM_RESERVED_MASK   0x20

/**********************
 *      TYPEDEFS
 **********************/

/* Type of callback function which is called when a button is released or long pressed on the button matrix
 * Parameters: button matrix, text of the released button
 * return LV_ACTION_RES_INV if  the button matrix is deleted else LV_ACTION_RES_OK*/
typedef lv_res_t (*lv_btnm_action_t) (lv_obj_t *, const char *txt);

/*Data of button matrix*/
typedef struct
{
    lv_cont_ext_t bg; /*Ext. of ancestor*/
    /*New data for this type */
    const char ** map_p;                            /*Pointer to the current map*/
    area_t * button_areas;                          /*Array of areas of buttons*/
    lv_btnm_action_t action;                        /*A function to call when a button is releases*/
    lv_style_t * button_styles[LV_BTN_STATE_NUM];   /*Styles of buttons in each state*/
    uint16_t button_cnt;                            /*Number of button in 'map_p'(Handled by the library)*/
    uint16_t button_id_pressed;                     /*Index of the currently pressed button or LV_BTNM_PR_NONE*/
    uint16_t button_id_toggled;                     /*Index of the currently toggled button or LV_BTNM_PR_NONE */
    uint8_t toggle     :1;                          /*Enable toggling*/
}lv_btnm_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**
 * Create a button matrix objects
 * @param par pointer to an object, it will be the parent of the new button matrix
 * @param copy pointer to a button matrix object, if not NULL then the new object will be copied from it
 * @return pointer to the created button matrix
 */
lv_obj_t * lv_btnm_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the button matrix
 * @param btnm pointer to a button matrix object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_btnm_signal(lv_obj_t * btnm, lv_signal_t sign, void * param);

/**
 * Set a new map. Buttons will be created/deleted according to the map.
 * @param btnm pointer to a button matrix object
 * @param map pointer a string array. The last string has to be: "".
 *            Use "\n" to begin a new line.
 *            The first byte can be a control data:
 *             - bit 7: always 1
 *             - bit 6: always 0
 *             - bit 5: reserved
 *             - bit 4: no repeat (on long press)
 *             - bit 3: hidden
 *             - bit 2..0: button relative width
 *             Example (practically use octal numbers): "\224abc": "abc" text with 4 width and no long press
 */
void lv_btnm_set_map(lv_obj_t * btnm, const char ** map);

/**
 * Set a new callback function for the buttons (It will be called when a button is released)
 * @param btnm: pointer to button matrix object
 * @param cb pointer to a callback function
 */
void lv_btnm_set_action(lv_obj_t * btnm, lv_btnm_action_t cb);

/**
 * Enable or disable button toggling
 * @param btnm pointer to button matrix object
 * @param en true: enable toggling; false: disable toggling
 * @param id index of the currently toggled button (ignored if 'en' == false)
 */
void lv_btnm_set_toggle(lv_obj_t * btnm, bool en, uint16_t id);


/**
 * Set styles of the button is each state. Use NULL for any style to leave it unchanged.
 * @param btnm pointer to button matrix object
 * @param rel_style pointer to a style for releases state
 * @param pr_style  pointer to a style for pressed state
 * @param tgl_rel_style pointer to a style for toggled releases state
 * @param tgl_pr_style pointer to a style for toggled pressed state
 * @param inactive_style pointer to a style for inactive state
 */
void lv_btnm_set_button_style(lv_obj_t *btnm, lv_style_t *rel_style, lv_style_t *pr_style,
                             lv_style_t *tgl_rel_style, lv_style_t *tgl_pr_style,
                             lv_style_t *inactive_style);
/**
 * Get the current map of a button matrix
 * @param btnm pointer to a button matrix object
 * @return the current map
 */
const char ** lv_btnm_get_map(lv_obj_t * btnm);

/**
 * Get a the callback function of the buttons on a button matrix
 * @param btnm: pointer to button matrix object
 * @return pointer to the callback function
 */
lv_btnm_action_t lv_btnm_get_action(lv_obj_t * btnm);

/**
 * Get the style of buttons in button matrix
 * @param btnm pointer to a button matrix object
 * @param state style in this state (LV_BTN_STATE_PR or LV_BTN_STATE_REL)
 * @return pointer the button style in the given state
 */
lv_style_t * lv_btnm_get_button_style(lv_obj_t * btnm, lv_btn_state_t state);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_BTNM*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BTNM_H*/
