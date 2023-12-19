/**
 * @file lv_templ.h
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
#include "../../../lvgl.h"

#if LV_USE_TABVIEW

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t obj;
    uint32_t tab_cur;
    lv_dir_t tab_pos;
} lv_tabview_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_tabview_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a tabview widget
 * @param parent    pointer to a parent widget
 * @return          the created tabview
 */
lv_obj_t * lv_tabview_create(lv_obj_t * parent);

/**
 * Add a tab to the tabview
 * @param obj       pointer to a tabview widget
 * @param name      the name of the tab, it will be displayed on the tab bar
 * @return          the widget where the content of the tab can be created
 */
lv_obj_t * lv_tabview_add_tab(lv_obj_t * obj, const char * name);

/**
 * Change the name of the tab
 * @param obj       pointer to a tabview widget
 * @param idx       the index of the tab to rename
 * @param new_name  the new name as a string
 */
void lv_tabview_rename_tab(lv_obj_t * obj, uint32_t idx, const char * new_name);

/**
 * Show a tab
 * @param obj       pointer to a tabview widget
 * @param idx       the index of the tab to show
 * @param anim_en   LV_ANIM_ON/OFF
 */
void lv_tabview_set_active(lv_obj_t * obj, uint32_t idx, lv_anim_enable_t anim_en);

/**
 * Set the position of the tab bar
 * @param obj       pointer to a tabview widget
 * @param dir       LV_DIR_TOP/BOTTOM/LEFT/RIGHT
 */
void lv_tabview_set_tab_bar_position(lv_obj_t * obj, lv_dir_t dir);

/**
 * Set the width or height of the tab bar
 * @param size      size of the tab bar in pixels or percentage.
 *                  will be used as width or height based on the position of the tab bar)
 */
void lv_tabview_set_tab_bar_size(lv_obj_t * obj, int32_t size);

/**
 * Get the number of tabs
 * @param obj       pointer to a tabview widget
 * @return          the number of tabs
 */
uint32_t lv_tabview_get_tab_count(lv_obj_t * obj);

/**
 * Get the current tab's index
 * @param obj       pointer to a tabview widget
 * @return          the zero based indoex of the current tab
 */
uint32_t lv_tabview_get_tab_active(lv_obj_t * obj);

/**
 * Get the widget where the container of each tab is created
 * @param obj       pointer to a tabview widget
 * @return          the main container widget
 */
lv_obj_t * lv_tabview_get_content(lv_obj_t * obj);

/**
 * Get the tab bar where the buttons are created
 * @param obj       pointer to a tabview widget
 * @return          the tabbar
 */
lv_obj_t * lv_tabview_get_tab_bar(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TABVIEW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TABVIEW_H*/
