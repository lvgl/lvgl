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

#if USE_LV_BTNM != 0

#include "../lv_core/lv_obj.h"
#include "lv_label.h"
#include "lv_btn.h"

/*********************
 *      DEFINES
 *********************/

/*Control byte*/
#define LV_BTNM_CTRL_CODE       0x80    /*The control byte has to begin (if present) with 0b10xxxxxx*/
#define LV_BTNM_CTRL_MASK       0xC0
#define LV_BTNM_WIDTH_MASK      0x07
#define LV_BTNM_HIDE_MASK       0x08
#define LV_BTNM_REPEAT_DISABLE_MASK     0x10
#define LV_BTNM_INACTIVE_MASK   0x20


#define LV_BTNM_PR_NONE         0xFFFF
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
    /*No inherited ext.*/ /*Ext. of ancestor*/
    /*New data for this type */
    const char ** map_p;                        /*Pointer to the current map*/
    lv_area_t *button_areas;                    /*Array of areas of buttons*/
    lv_btnm_action_t action;                    /*A function to call when a button is releases*/
    lv_style_t *styles_btn[LV_BTN_STATE_NUM];   /*Styles of buttons in each state*/
    uint16_t btn_cnt;                           /*Number of button in 'map_p'(Handled by the library)*/
    uint16_t btn_id_pr;                         /*Index of the currently pressed button (in `button_areas`) or LV_BTNM_PR_NONE*/
    uint16_t btn_id_tgl;                        /*Index of the currently toggled button (in `button_areas`) or LV_BTNM_PR_NONE */
    uint8_t toggle     :1;                      /*Enable toggling*/
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
 * Set a new map. Buttons will be created/deleted according to the map.
 * @param btnm pointer to a button matrix object
 * @param map pointer a string array. The last string has to be: "".
 *            Use "\n" to begin a new line.
 *            The first byte can be a control data:
 *             - bit 7: always 1
 *             - bit 6: always 0
 *             - bit 5: inactive (disabled)
 *             - bit 4: no repeat (on long press)
 *             - bit 3: hidden
 *             - bit 2..0: button relative width
 *             Example (practically use octal numbers): "\224abc": "abc" text with 4 width and no long press
 */
void lv_btnm_set_map(lv_obj_t * btnm, const char ** map);

/**
 * Set a new callback function for the buttons (It will be called when a button is released)
 * @param btnm: pointer to button matrix object
 * @param action pointer to a callback function
 */
void lv_btnm_set_action(lv_obj_t * btnm, lv_btnm_action_t action);

/**
 * Enable or disable button toggling
 * @param btnm pointer to button matrix object
 * @param en true: enable toggling; false: disable toggling
 * @param id index of the currently toggled button (ignored if 'en' == false)
 */
void lv_btnm_set_toggle(lv_obj_t * btnm, bool en, uint16_t id);

/**
 * Set a style of a button matrix
 * @param btnm pointer to a button matrix object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_btnm_set_style(lv_obj_t *btnm, lv_btnm_style_t type, lv_style_t *style);

/**
 * Set whether recoloring is enabled
 * @param btnm pointer to button matrix object
 * @param en whether recoloring is enabled
 */
void lv_btnm_set_recolor(const lv_obj_t * btnm, bool en);

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
 * Get a the callback function of the buttons on a button matrix
 * @param btnm: pointer to button matrix object
 * @return pointer to the callback function
 */
lv_btnm_action_t lv_btnm_get_action(const lv_obj_t * btnm);

/**
 * Get the pressed button
 * @param btnm pointer to button matrix object
 * @return  index of the currently pressed button (LV_BTNM_PR_NONE: if unset)
 */
uint16_t lv_btnm_get_pressed(const lv_obj_t * btnm);

/**
 * Get the toggled button
 * @param btnm pointer to button matrix object
 * @return  index of the currently toggled button (LV_BTNM_PR_NONE: if unset)
 */
uint16_t lv_btnm_get_toggled(const lv_obj_t * btnm);

/**
 * Get a style of a button matrix
 * @param btnm pointer to a button matrix object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_btnm_get_style(const lv_obj_t *btnm, lv_btnm_style_t type);

/**
 * Find whether recoloring is enabled
 * @param btnm pointer to button matrix object
 * @return whether recoloring is enabled
 */
bool lv_btnm_get_recolor(const lv_obj_t * btnm);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_BTNM*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BTNM_H*/
