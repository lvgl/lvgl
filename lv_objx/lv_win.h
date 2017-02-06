/**
 * @file lv_win.h
 * 
 */

#ifndef LV_WIN_H
#define LV_WIN_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_WIN != 0

/*Testing of dependencies*/
#if USE_LV_RECT == 0
#error "lv_win: lv_rect is required. Enable it in lv_conf.h (USE_LV_RECT  1) "
#endif

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

#if USE_LV_PAGE == 0
#error "lv_win: lv_page is required. Enable it in lv_conf.h (USE_LV_PAGE  1) "
#endif

#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"
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
    lv_page_ext_t page; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * header;  /*Pointer to the header rectangle of the window*/
    lv_obj_t * title;   /*Pointer to the title label of the window*/
    lv_obj_t * ctrl_holder; /*Pointer to the control button holder rectangle of the window*/
}lv_win_ext_t;

/*Style of window*/
typedef struct
{
    lv_pages_t pages; /*Style of ancestor*/
	/*New style element for this type */
	/*Header settings*/
	lv_rects_t header;      /*Style of the header rectangle*/
	lv_labels_t title;      /*Style of the window title*/
	lv_rects_t ctrl_holder; /*Style of holder of the control buttons*/
	lv_btns_t ctrl_btn;     /*Style of the control buttons*/
	lv_imgs_t ctrl_img;     /*Style of the image on the control buttons*/
	cord_t ctrl_btn_w;      /*Width of the control buttons*/
	cord_t ctrl_btn_h;      /*Height of the control buttons*/
	opa_t ctrl_btn_opa;     /*Width of the control buttons in the percentage of object opacity (0..100)*/
	opa_t header_opa;       /*Opacity of the header in the percentage of object opacity (0..100)*/
}lv_wins_t;

/*Built-in styles of window*/
typedef enum
{
	LV_WINS_DEF,
}lv_wins_builtin_t;

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
lv_obj_t * lv_win_add_ctrl_btn(lv_obj_t * win, const char * img_path, lv_action_t rel_action);

/**
 * A release action which can be assigned to a window control button to close it
 * @param btn pointer to the released button
 * @param dispi pointer to the caller display input
 * @return always false because the button is deleted with the window
 */
lv_action_res_t  lv_win_close_action(lv_obj_t * btn, lv_dispi_t * dispi);

/**
 * Set the title of a window
 * @param win pointer to a window object
 * @param title string of the new title
 */
void lv_win_set_title(lv_obj_t * win, const char * title);

/**
 * Get the title of a window
 * @param win pointer to a window object
 * @return title string of the window
 */
const char * lv_win_get_title(lv_obj_t * win);

/**
 * Get the pointer of a widow from one of  its control button.
 * It is useful in the action of the control buttons where only button is known.
 * @param ctrl_btn pointer to a control button of a window
 * @return pointer to the window of 'ctrl_btn'
 */
lv_obj_t * lv_win_get_from_ctrl_btn(lv_obj_t * ctrl_btn);

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_wins_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_wins_t style
 */
lv_wins_t * lv_wins_get(lv_wins_builtin_t style, lv_wins_t * copy);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_WIN*/

#endif /*LV_WIN_H*/
