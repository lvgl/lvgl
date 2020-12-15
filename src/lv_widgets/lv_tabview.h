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
#include "../lv_conf_internal.h"

#if LV_USE_TABVIEW != 0

/*Testing of dependencies*/
#if LV_USE_BTNMATRIX == 0
#error "lv_tabview: lv_btnm is required. Enable it in lv_conf.h (LV_USE_BTNMATRIX  1) "
#endif

#if LV_USE_PAGE == 0
#error "lv_tabview: lv_page is required. Enable it in lv_conf.h (LV_USE_PAGE  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "../lv_widgets/lv_win.h"
#include "../lv_widgets/lv_page.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Position of tabview buttons. */
enum {
    LV_TABVIEW_TAB_POS_NONE,
    LV_TABVIEW_TAB_POS_TOP,
    LV_TABVIEW_TAB_POS_BOTTOM,
    LV_TABVIEW_TAB_POS_LEFT,
    LV_TABVIEW_TAB_POS_RIGHT
};
typedef uint8_t lv_tabview_btns_pos_t;

/*Data of tab*/
typedef struct {
    /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * btns;
    lv_obj_t * indic;
    lv_obj_t * content; /*A background page which holds tab's pages*/
    const char ** tab_name_ptr;
    lv_point_t point_last;
    uint16_t tab_cur;
    uint16_t tab_cnt;
#if LV_USE_ANIMATION
    uint16_t anim_time;
#endif
    lv_tabview_btns_pos_t btns_pos : 3;
} lv_tabview_ext_t;

enum {
    LV_TABVIEW_PART_BG = LV_OBJ_PART_MAIN,
    _LV_TABVIEW_PART_VIRTUAL_LAST = _LV_OBJ_PART_VIRTUAL_LAST,

    LV_TABVIEW_PART_BG_SCROLLABLE = _LV_OBJ_PART_REAL_LAST,
    LV_TABVIEW_PART_TAB_BG,
    LV_TABVIEW_PART_TAB_BTN,
    LV_TABVIEW_PART_INDIC,
    _LV_TABVIEW_PART_REAL_LAST,
};
typedef uint8_t lv_tabview_part_t;

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

/**
 * Delete all children of a tab created by `lv_tabview_add_tab`.
 * @param tab pointer to a tab
 */
void lv_tabview_clean_tab(lv_obj_t * tab);

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
 * Set the name of a tab.
 * @param tabview pointer to Tab view object
 * @param id index of the tab the name should be set
 * @param name new tab name
 */
void lv_tabview_set_tab_name(lv_obj_t * tabview, uint16_t id, char * name);

/**
 * Set the animation time of tab view when a new tab is loaded
 * @param tabview pointer to Tab view object
 * @param anim_time time of animation in milliseconds
 */
void lv_tabview_set_anim_time(lv_obj_t * tabview, uint16_t anim_time);

/**
 * Set the position of tab select buttons
 * @param tabview pointer to a tab view object
 * @param btns_pos which button position
 */
void lv_tabview_set_btns_pos(lv_obj_t * tabview, lv_tabview_btns_pos_t btns_pos);

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
 * Get the animation time of tab view when a new tab is loaded
 * @param tabview pointer to Tab view object
 * @return time of animation in milliseconds
 */
uint16_t lv_tabview_get_anim_time(const lv_obj_t * tabview);

/**
 * Get position of tab select buttons
 * @param tabview pointer to a ab view object
 */
lv_tabview_btns_pos_t lv_tabview_get_btns_pos(const lv_obj_t * tabview);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TABVIEW*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TABVIEW_H*/
