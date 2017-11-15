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
    lv_style_t * style_header;      /*Style of the header container*/
    lv_style_t * style_btn_rel;    /*Control button releases style*/
    lv_style_t * style_btn_pr;     /*Control button pressed style*/
    cord_t btn_size;               /*Size of the control buttons (square)*/
}lv_win_ext_t;

typedef enum {
    LV_WIN_STYLE_BG,
    LV_WIN_STYLE_CONTENT,
    LV_WIN_STYLE_SB,
    LV_WIN_STYLE_HEADER,
    LV_WIN_STYLE_BTN_REL,
    LV_WIN_STYLE_BTN_PR,
}lv_win_style_t;

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


/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add control button to the header of the window
 * @param win pointer to a window object
 * @param img_path path of an image on the control button
 * @param rel_action a function pointer to call when the button is released
 * @return pointer to the created button object
 */
lv_obj_t * lv_win_add_btn(lv_obj_t * win, const char * img_path, lv_action_t rel_action);

/*=====================
 * Setter functions
 *====================*/

/**
 * A release action which can be assigned to a window control button to close it
 * @param btn pointer to the released button
 * @return always LV_ACTION_RES_INV because the button is deleted with the window
 */
lv_res_t lv_win_close_action(lv_obj_t * btn);

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
void lv_win_set_btn_size(lv_obj_t * win, cord_t size);

/**
 * Set a style of a window
 * @param win pointer to a window object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_win_set_style(lv_obj_t *win, lv_win_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the title of a window
 * @param win pointer to a window object
 * @return title string of the window
 */
const char * lv_win_get_title(lv_obj_t * win);

/**
 * Get the control button size of a window
 * @param win pointer to a window object
 * @return control button size
 */
cord_t lv_win_get_btn_size(lv_obj_t * win);

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
lv_obj_t * lv_win_get_from_btn(lv_obj_t * ctrl_btn);

/**
 * Get a style of a window
 * @param win pointer to a button object
 * @param type which style window be get
 * @return style pointer to a style
 */
lv_style_t * lv_win_get_style(lv_obj_t *win, lv_win_style_t type);

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_WIN*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_WIN_H*/
