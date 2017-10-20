/**
 * @file lv_win.h
 * 
 */

#ifndef LV_WIN_H
#define LV_WIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_WIN != 0

/*Testing of dependencies*/
#if USE_LV_BTN == 0
#error "lv_win: lv_btn is required. Enable it in lv_conf.h (USE_LV_BTN  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_win: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif

#if USE_LV_IMG == 0
#error "lv_win: lv_img is required. Enable it in lv_conf.h (USE_LV_IMG  1) "
#endif


#if USE_LV_PAGE == 0
#error "lv_win: lv_page is required. Enable it in lv_conf.h (USE_LV_PAGE  1) "
#endif

#include "../lv_obj/lv_obj.h"
#include "lv_cont.h"
#include "lv_btn.h"
#include "lv_label.h"
#include "lv_img.h"
#include "lv_page.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of window*/
typedef struct
{
    /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * page;                /*Pointer to a page which holds the content*/
    lv_obj_t * header;              /*Pointer to the header container of the window*/
    lv_obj_t * title;               /*Pointer to the title label of the window*/
    lv_obj_t * btnh;                /*Pointer to the control button holder container of the window*/
    lv_style_t * style_header;      /*Style of the header container*/
    lv_style_t * style_cbtn_rel;    /*Control button releases style*/
    lv_style_t * style_cbtn_pr;     /*Control button pressed style*/
    cord_t cbtn_size;               /*Size of the control buttons (square)*/
}lv_win_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a window objects
 * @param par pointer to an object, it will be the parent of the new window
 * @param copy pointer to a window object, if not NULL then the new object will be copied from it
 * @return pointer to the created window
 */
lv_obj_t * lv_win_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the window
 * @param win pointer to a window object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_win_signal(lv_obj_t * win, lv_signal_t sign, void * param);

/**
 * Add control button to the header of the window
 * @param win pointer to a window object
 * @param img_path path of an image on the control button
 * @param rel_action a function pointer to call when the button is released
 * @return pointer to the created button object
 */
lv_obj_t * lv_win_add_cbtn(lv_obj_t * win, const char * img_path, lv_action_t rel_action);

/**
 * A release action which can be assigned to a window control button to close it
 * @param btn pointer to the released button
 * @param indev_proc pointer to the caller input device
 * @return always LV_ACTION_RES_INV because the button is deleted with the window
 */
lv_action_res_t lv_win_close_action(lv_obj_t * btn);

/**
 * Set the title of a window
 * @param win pointer to a window object
 * @param title string of the new title
 */
void lv_win_set_title(lv_obj_t * win, const char * title);

/**
 * Set the control button size of a window
 * @param win pointer to a window object
 * @return control button size
 */
void lv_win_set_cbtn_size(lv_obj_t * win, cord_t size);

/**
 * Set the styles of the window  control buttons in a given state
 * @param win pointer to a window object
 * @param rel pointer to the style in released state
 * @param pr pointer to the style in pressed state
 */
void lv_win_set_styles_cbtn(lv_obj_t * win, lv_style_t *  rel, lv_style_t *  pr);

/**
 * Get the title of a window
 * @param win pointer to a window object
 * @return title string of the window
 */
const char * lv_win_get_title(lv_obj_t * win);

/**
 * Get the page of a window
 * @param win pointer to a window object
 * @return page pointer to the page object of the window
 */
lv_obj_t * lv_win_get_page(lv_obj_t * win);

/**
 * Get the s window header
 * @param win pointer to a window object
 * @return pointer to the window header object (lv_rect)
 */
lv_obj_t * lv_win_get_header(lv_obj_t * win);

/**
 * Get the control button size of a window
 * @param win pointer to a window object
 * @return control button size
 */
cord_t lv_win_get_cbtn_size(lv_obj_t * win);

/**
 * Get width of the content area (page scrollable) of the window
 * @param win pointer to a window object
 * @return the width of the content area
 */
cord_t lv_win_get_width(lv_obj_t * win);

/**
 * Get the pointer of a widow from one of  its control button.
 * It is useful in the action of the control buttons where only button is known.
 * @param ctrl_btn pointer to a control button of a window
 * @return pointer to the window of 'ctrl_btn'
 */
lv_obj_t * lv_win_get_from_cbtn(lv_obj_t * ctrl_btn);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_WIN*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_WIN_H*/
