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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

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

#include "../lv_core/lv_obj.h"
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
    lv_coord_t btn_size;               /*Size of the control buttons (square)*/
} lv_win_ext_t;

enum {
    LV_WIN_STYLE_BG,
    LV_WIN_STYLE_CONTENT_BG,
    LV_WIN_STYLE_CONTENT_SCRL,
    LV_WIN_STYLE_SB,
    LV_WIN_STYLE_HEADER,
    LV_WIN_STYLE_BTN_REL,
    LV_WIN_STYLE_BTN_PR,
};
typedef uint8_t lv_win_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a window objects
 * @param par pointer to an object, it will be the parent of the new window
 * @param copy pointer to a window object, if not NULL then the new object will be copied from it
 * @return pointer to the created window
 */
lv_obj_t * lv_win_create(lv_obj_t * par, const lv_obj_t * copy);

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param obj pointer to an object
 */
void lv_win_clean(lv_obj_t *obj);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add control button to the header of the window
 * @param win pointer to a window object
 * @param img_src an image source ('lv_img_t' variable, path to file or a symbol)
 * @param rel_action a function pointer to call when the button is released
 * @return pointer to the created button object
 */
lv_obj_t * lv_win_add_btn(lv_obj_t * win, const void * img_src, lv_action_t rel_action);

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
void lv_win_set_btn_size(lv_obj_t * win, lv_coord_t size);

/**
 * Set the layout of the window
 * @param win pointer to a window object
 * @param layout the layout from 'lv_layout_t'
 */
void lv_win_set_layout(lv_obj_t *win, lv_layout_t layout);

/**
 * Set the scroll bar mode of a window
 * @param win pointer to a window object
 * @param sb_mode the new scroll bar mode from  'lv_sb_mode_t'
 */
void lv_win_set_sb_mode(lv_obj_t *win, lv_sb_mode_t sb_mode);

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
const char * lv_win_get_title(const lv_obj_t * win);

/**
* Get the content holder object of window (`lv_page`) to allow additional customization
* @param win pointer to a window object
* @return the Page object where the window's content is
*/
lv_obj_t * lv_win_get_content(const lv_obj_t * win);

/**
 * Get the control button size of a window
 * @param win pointer to a window object
 * @return control button size
 */
lv_coord_t lv_win_get_btn_size(const lv_obj_t * win);

/**
 * Get the pointer of a widow from one of  its control button.
 * It is useful in the action of the control buttons where only button is known.
 * @param ctrl_btn pointer to a control button of a window
 * @return pointer to the window of 'ctrl_btn'
 */
lv_obj_t * lv_win_get_from_btn(const lv_obj_t * ctrl_btn);

/**
 * Get the layout of a window
 * @param win pointer to a window object
 * @return the layout of the window (from 'lv_layout_t')
 */
lv_layout_t lv_win_get_layout(lv_obj_t *win);

/**
 * Get the scroll bar mode of a window
 * @param win pointer to a window object
 * @return the scroll bar mode of the window (from 'lv_sb_mode_t')
 */
lv_sb_mode_t lv_win_get_sb_mode(lv_obj_t *win);

/**
 * Get width of the content area (page scrollable) of the window
 * @param win pointer to a window object
 * @return the width of the content area
 */
lv_coord_t lv_win_get_width(lv_obj_t * win);

/**
 * Get a style of a window
 * @param win pointer to a button object
 * @param type which style window be get
 * @return style pointer to a style
 */
lv_style_t * lv_win_get_style(const lv_obj_t *win, lv_win_style_t type);

/*=====================
 * Other functions
 *====================*/

/**
 * Focus on an object. It ensures that the object will be visible in the window.
 * @param win pointer to a window object
 * @param obj pointer to an object to focus (must be in the window)
 * @param anim_time scroll animation time in milliseconds (0: no animation)
 */
void lv_win_focus(lv_obj_t * win, lv_obj_t * obj, uint16_t anim_time);

/**
 * Scroll the window horizontally
 * @param win pointer to a window object
 * @param dist the distance to scroll (< 0: scroll right; > 0 scroll left)
 */
static inline void lv_win_scroll_hor(lv_obj_t * win, lv_coord_t dist)
{
    lv_win_ext_t * ext = (lv_win_ext_t *)lv_obj_get_ext_attr(win);
    lv_page_scroll_hor(ext->page, dist);
}
/**
 * Scroll the window vertically
 * @param win pointer to a window object
 * @param dist the distance to scroll (< 0: scroll down; > 0 scroll up)
 */
static inline void lv_win_scroll_ver(lv_obj_t * win, lv_coord_t dist)
{
    lv_win_ext_t * ext = (lv_win_ext_t *)lv_obj_get_ext_attr(win);
    lv_page_scroll_ver(ext->page, dist);
}

/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_WIN*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_WIN_H*/
