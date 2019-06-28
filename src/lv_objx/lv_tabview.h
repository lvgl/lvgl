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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#if LV_USE_TABVIEW != 0

/*Testing of dependencies*/
#if LV_USE_BTNM == 0
#error "lv_tabview: lv_btnm is required. Enable it in lv_conf.h (LV_USE_BTNM  1) "
#endif

#if LV_USE_PAGE == 0
#error "lv_tabview: lv_page is required. Enable it in lv_conf.h (LV_USE_PAGE  1) "
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

/** Position of tabview buttons. */
enum { LV_TABVIEW_BTNS_POS_TOP, LV_TABVIEW_BTNS_POS_BOTTOM, LV_TABVIEW_BTNS_POS_LEFT, LV_TABVIEW_BTNS_POS_RIGHT };
typedef uint8_t lv_tabview_btns_pos_t;

/*Data of tab*/
typedef struct
{
    /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * btns;
    lv_obj_t * indic;
    lv_obj_t * content; /*A rectangle to show the current tab*/
    const char ** tab_name_ptr;
    lv_point_t point_last;
    uint16_t tab_cur;
    uint16_t tab_cnt;
#if LV_USE_ANIMATION
    uint16_t anim_time;
#endif
    uint8_t slide_enable : 1; /*1: enable horizontal sliding by touch pad*/
    uint8_t draging : 1;
    uint8_t drag_hor : 1;
    uint8_t scroll_ver : 1;
    uint8_t btns_hide : 1;
    lv_tabview_btns_pos_t btns_pos : 2;
} lv_tabview_ext_t;

enum {
    LV_TABVIEW_STYLE_BG,
    LV_TABVIEW_STYLE_INDIC,
    LV_TABVIEW_STYLE_BTN_BG,
    LV_TABVIEW_STYLE_BTN_REL,
    LV_TABVIEW_STYLE_BTN_PR,
    LV_TABVIEW_STYLE_BTN_TGL_REL,
    LV_TABVIEW_STYLE_BTN_TGL_PR,
};
typedef uint8_t lv_tabview_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a Tab view object
 * @param par pointer to an object, it will be the parent of the new tab
 * @param copy pointer to a tab object, if not NULL then the new object will be copied from it
 * @return pointer to the created tab
 */
lv_obj_t * lv_tabview_create(lv_obj_t * par, const lv_obj_t * copy);

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param obj pointer to an object
 */
void lv_tabview_clean(lv_obj_t * obj);

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
 * @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
 */
void lv_tabview_set_tab_act(lv_obj_t * tabview, uint16_t id, lv_anim_enable_t anim);

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
void lv_tabview_set_style(lv_obj_t * tabview, lv_tabview_style_t type, const lv_style_t * style);

/**
 * Set the position of tab select buttons
 * @param tabview pointer to a tab view object
 * @param btns_pos which button position
 */
void lv_tabview_set_btns_pos(lv_obj_t * tabview, lv_tabview_btns_pos_t btns_pos);

/**
 * Set whether tab buttons are hidden
 * @param tabview pointer to a tab view object
 * @param en whether tab buttons are hidden
 */
void lv_tabview_set_btns_hidden(lv_obj_t * tabview, bool en);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the index of the currently active tab
 * @param tabview pointer to Tab view object
 * @return the active tab index
 */
uint16_t lv_tabview_get_tab_act(const lv_obj_t * tabview);

/**
 * Get the number of tabs
 * @param tabview pointer to Tab view object
 * @return tab count
 */
uint16_t lv_tabview_get_tab_count(const lv_obj_t * tabview);
/**
 * Get the page (content area) of a tab
 * @param tabview pointer to Tab view object
 * @param id index of the tab (>= 0)
 * @return pointer to page (lv_page) object
 */
lv_obj_t * lv_tabview_get_tab(const lv_obj_t * tabview, uint16_t id);

/**
 * Get horizontal sliding is enabled or not
 * @param tabview pointer to Tab view object
 * @return true: enable sliding; false: disable sliding
 */
bool lv_tabview_get_sliding(const lv_obj_t * tabview);

/**
 * Get the animation time of tab view when a new tab is loaded
 * @param tabview pointer to Tab view object
 * @return time of animation in milliseconds
 */
uint16_t lv_tabview_get_anim_time(const lv_obj_t * tabview);

/**
 * Get a style of a tab view
 * @param tabview pointer to a ab view object
 * @param type which style should be get
 * @return style pointer to a style
 */
const lv_style_t * lv_tabview_get_style(const lv_obj_t * tabview, lv_tabview_style_t type);

/**
 * Get position of tab select buttons
 * @param tabview pointer to a ab view object
 */
lv_tabview_btns_pos_t lv_tabview_get_btns_pos(const lv_obj_t * tabview);

/**
 * Get whether tab buttons are hidden
 * @param tabview pointer to a tab view object
 * @return whether tab buttons are hidden
 */
bool lv_tabview_get_btns_hidden(const lv_obj_t * tabview);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TABVIEW*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TABVIEW_H*/
