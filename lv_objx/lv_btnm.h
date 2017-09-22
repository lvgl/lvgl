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

/**********************
 *      TYPEDEFS
 **********************/

/* Type of callback function which is called when a button is released on the button matrix
 * Parameters: button matrix, released button index in the map string
 * return LV_ACTION_RES_INV:  the button matrix is deleted else LV_ACTION_RES_OK*/
typedef lv_action_res_t (*lv_btnm_callback_t) (lv_obj_t *, uint16_t);

/*Data of button matrix*/
typedef struct
{
    lv_cont_ext_t bg; /*Ext. of ancestor*/
    /*New data for this type */
    const char ** map_p;    /*Pointer to the current map*/
    area_t * btn_areas;     /*Array of areas for the buttons (Handled by the library)*/
    uint16_t btn_cnt;       /*Number of button in 'map_p'(Handled by the library)*/
    uint16_t btn_pr;        /*Index of the currently pressed button or LV_BTNM_PR_NONE (Handled by the library)*/
    uint16_t btn_tgl;       /*Index of the currently toggled button or LV_BTNM_PR_NONE (Handled by the library)*/
    lv_btnm_callback_t cb;  /*A function to call when a button is releases*/
    lv_style_t * style_btn_rel; /*Style of the released buttons*/
    lv_style_t * style_btn_pr;  /*Style of the pressed buttons*/
    lv_style_t * style_btn_trel; /*Style of the toggled released buttons*/
    lv_style_t * style_btn_tpr;  /*Style of the toggled pressed buttons*/
    lv_style_t * style_btn_ina;  /*Style of the inactive buttons*/
    uint8_t tgl     :1;         /*Enable toggling*/
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
 *            Use octal numbers (e.g. "\003") to set the relative
 *            width of a button. (max. 9 -> \011)
 *            (e.g. const char * str[] = {"a", "b", "\n", "\004c", "d", ""}).
 *            The button do not copy the array so it can not be a local variable.
 */
void lv_btnm_set_map(lv_obj_t * btnm, const char ** map);

/**
 * Set a new callback function for the buttons (It will be called when a button is released)
 * @param btnm: pointer to button matrix object
 * @param cb pointer to a callback function
 */
void lv_btnm_set_action(lv_obj_t * btnm, lv_btnm_callback_t cb);

/**
 * Enable or disable button toggling
 * @param btnm pointer to button matrix object
 * @param en true: enable toggling; false: disable toggling
 * @param id index of the currently toggled button (ignored if 'en' == false)
 */
void lv_btnm_set_tgl(lv_obj_t * btnm, bool en, uint16_t id);


/**
 * Set the styles of the buttons of the button matrix
 * @param btnm pointer to a button matrix object
 * @param rel pointer to a style for releases state
 * @param pr  pointer to a style for pressed state
 * @param trel pointer to a style for toggled releases state
 * @param tpr pointer to a style for toggled pressed state
 * @param ina pointer to a style for inactive state
 */
void lv_btnm_set_styles_btn(lv_obj_t * btnm, lv_style_t *  rel, lv_style_t *  pr,
                            lv_style_t *  trel, lv_style_t *  tpr, lv_style_t *  ina);

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
lv_btnm_callback_t lv_btnm_get_action(lv_obj_t * btnm);

/**
 * Get the style of buttons in button matrix
 * @param btnm pointer to a button matrix object
 * @param state style in this state (LV_BTN_STATE_PR or LV_BTN_STATE_REL)
 * @return pointer the button style in the given state
 */
lv_style_t * lv_btnm_get_style_btn(lv_obj_t * btnm, lv_btn_state_t state);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_BTNM*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BTNM_H*/
