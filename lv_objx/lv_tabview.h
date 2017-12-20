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
#include "../../lv_conf.h"
#if USE_LV_TABVIEW != 0


/*Testing of dependencies*/
#if USE_LV_BTNM == 0
#error "lv_tabview: lv_btnm is required. Enable it in lv_conf.h (USE_LV_BTNM  1) "
#endif

#if USE_LV_PAGE == 0
#error "lv_tabview: lv_page is required. Enable it in lv_conf.h (USE_LV_PAGE  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "../lv_objx/lv_win.h"
#include "../lv_objx/lv_page.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* parametes: pointer to a tabview object, tab_id*/
typedef void (*lv_tabview_action_t)(lv_obj_t *, uint16_t);

/*Data of tab*/
typedef struct
{
   /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * btns;
    lv_obj_t * indic;
    lv_obj_t * content;   /*A rectangle to show the current tab*/
    const char ** tab_name_ptr;
    lv_point_t point_last;
    uint16_t tab_cur;
    uint16_t tab_cnt;
    uint16_t anim_time;
    uint8_t slide_enable :1;    /*1: enable horizontal sliding by touch pad*/
    uint8_t draging :1;
    uint8_t drag_hor :1;
    lv_tabview_action_t tab_load_action;
}lv_tabview_ext_t;

typedef enum {
    LV_TABVIEW_STYLE_BG,
    LV_TABVIEW_STYLE_INDIC,
    LV_TABVIEW_STYLE_BTN_BG,
    LV_TABVIEW_STYLE_BTN_REL,
    LV_TABVIEW_STYLE_BTN_PR,
    LV_TABVIEW_STYLE_BTN_TGL_REL,
    LV_TABVIEW_STYLE_BTN_TGL_PR,
}lv_tabview_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**
 * Create a Tab view object
 * @param par pointer to an object, it will be the parent of the new tab
 * @param copy pointer to a tab object, if not NULL then the new object will be copied from it
 * @return pointer to the created tab
 */
lv_obj_t * lv_tabview_create(lv_obj_t * par, lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add a new tab with the given name
 * @param tabview pointer to Tab view object where to ass the new tab
 * @param name the text on the tab button
 * @return pointer to the created page object (lv_page). You can create your content here
 */
lv_obj_t * lv_tabview_add_tab(lv_obj_t * tabview, const char * name);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new tab
 * @param tabview pointer to Tab view object
 * @param id index of a tab to load
 * @param anim_en true: set with sliding animation; false: set immediately
 */
void lv_tabview_set_tab_act(lv_obj_t * tabview, uint16_t id, bool anim_en);

/**
 * Set an action to call when a tab is loaded (Good to create content only if required)
 * lv_tabview_get_act() still gives the current (old) tab (to remove content from here)
 * @param tabview pointer to a tabview object
 * @param action pointer to a function to call when a tab is loaded
 */
void lv_tabview_set_tab_load_action(lv_obj_t *tabview, lv_tabview_action_t action);

/**
 * Enable horizontal sliding with touch pad
 * @param tabview pointer to Tab view object
 * @param en true: enable sliding; false: disable sliding
 */
void lv_tabview_set_sliding(lv_obj_t * tabview, bool en);

/**
 * Set the animation time of tab view when a new tab is loaded
 * @param tabview pointer to Tab view object
 * @param anim_time time of animation in milliseconds
 */
void lv_tabview_set_anim_time(lv_obj_t * tabview, uint16_t anim_time);

/**
 * Set the style of a tab view
 * @param tabview pointer to a tan view object
 * @param type which style should be set
 * @param style pointer to the new style
 */
void lv_tabview_set_style(lv_obj_t *tabview, lv_tabview_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

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
uint16_t lv_tabview_get_tab_count(lv_obj_t * tabview);
/**
 * Get the page (content area) of a tab
 * @param tabview pointer to Tab view object
 * @param id index of the tab (>= 0)
 * @return pointer to page (lv_page) object
 */
lv_obj_t * lv_tabview_get_tab(lv_obj_t * tabview, uint16_t id);

/**
 * Get the tab load action
 * @param tabview pointer to a tabview object
 * @param return the current tab load action
 */
lv_tabview_action_t lv_tabview_get_tab_load_action(lv_obj_t *tabview);

/**
 * Get horizontal sliding is enabled or not
 * @param tabview pointer to Tab view object
 * @return true: enable sliding; false: disable sliding
 */
bool lv_tabview_get_sliding(lv_obj_t * tabview);

/**
 * Get the animation time of tab view when a new tab is loaded
 * @param tabview pointer to Tab view object
 * @return time of animation in milliseconds
 */
uint16_t lv_tabview_get_anim_time(lv_obj_t * tabview);

/**
 * Get a style of a tab view
 * @param tabview pointer to a ab view object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_tabview_get_style(lv_obj_t *tabview, lv_tabview_style_t type);


/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_TABVIEW*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_TABVIEW_H*/
