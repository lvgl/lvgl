/**
 * @file lv_tabview.h
 * 
 */

#ifndef LV_TABVIEW_H
#define LV_TABVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_TABVIEW != 0


/*Testing of dependencies*/
#if USE_LV_BTNM == 0
#error "lv_tabview: lv_btnm is required. Enable it in lv_conf.h (USE_LV_BTNM  1) "
#endif

#if USE_LV_PAGE == 0
#error "lv_tabview: lv_page is required. Enable it in lv_conf.h (USE_LV_PAGE  1) "
#endif

#include "../lv_obj/lv_obj.h"
#include "../lv_objx/lv_win.h"
#include "../lv_objx/lv_page.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of tab*/
typedef struct
{
   /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * tabs;
    lv_obj_t * indic;
    lv_obj_t * content;   /*A rectangle to show the current tab*/
    const char ** tab_name_ptr;
    point_t point_last;
    uint16_t tab_act;
    uint16_t tab_cnt;
    uint8_t draging :1;
    uint8_t drag_h :1;

}lv_tabview_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a Tab view objects
 * @param par pointer to an object, it will be the parent of the new tab
 * @param copy pointer to a tab object, if not NULL then the new object will be copied from it
 * @return pointer to the created tab
 */
lv_obj_t * lv_tabview_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the Tab view
 * @param tab pointer to a tab object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_tabview_signal(lv_obj_t * tab, lv_signal_t sign, void * param);

/**
 * Realign and resize the elements of Tab view
 * @param tabview pointer to a Tab view object
 */
void lv_tabview_realign(lv_obj_t * tabview);
/**
 * Set a new tab
 * @param tabview pointer to Tab view object
 * @param id index of a tab to load
 * @param anim_en true: set with sliding animation; false: set immediately
 */
void lv_tabview_set_act(lv_obj_t * tabview, uint16_t id, bool anim_en);

/**
 * Get the index of the currently active tab
 * @param tabview pointer to Tab view object
 * @return the active tab index
 */
uint16_t lv_tabview_get_tab_act(lv_obj_t * tabview);
/**
 * Get the number of tabs
 * @param tabview pointer to Tab view object
 * @return tab count
 */
uint16_t lv_tabview_get_tab_cnt(lv_obj_t * tabview);

/**
 * Get the page (content area) of a tab
 * @param tabview pointer to Tab view object
 * @param id index of the tab (>= 0)
 * @return pointer to page (lv_page) object
 */
lv_obj_t * lv_tabview_get_tab_page(lv_obj_t * tabview, uint16_t id);

/**
 * Get the tab button matrix (lv_btnm) of a Tab view
 * @param tabview pointer to Tab view object
 * @return pointer to button matrix (lv_btnm) object which is the tab buttons
 */
lv_obj_t * lv_tabview_get_tabs(lv_obj_t * tabview);
/**
 * Get the indicator rectangle (lv_obj) of a Tab view
 * @param tabview pointer to Tab view object
 * @return pointer to Base object (lv_obj) which is the indicator rectangle on the tab buttons
 */
lv_obj_t * lv_tabview_get_indic(lv_obj_t * tabview);

/**
 * Add a new tab with the given name
 * @param tabview pointer to Tab view object where to ass the new tab
 * @param name the text on the tab button
 * @return pointer to page object (lv_page) which is the containter of the contet
 */
lv_obj_t * lv_tabview_add_tab(lv_obj_t * tabview, const char * name);


/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_TABVIEW*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_TAB_H*/
