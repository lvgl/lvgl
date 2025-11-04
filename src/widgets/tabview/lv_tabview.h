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
#include "../../lv_conf_internal.h"
#include "../../core/lv_obj.h"

#if LV_USE_TABVIEW

/*********************
 *      DEFINES
 *********************/

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
void lv_tabview_set_tab_text(lv_obj_t * obj, uint32_t idx, const char * new_name);

#if LV_USE_TRANSLATION

/**
 * Add a tab with a translation tag to the tabview.
 * @param obj       pointer to a tabview widget
 * @param tag       translation key used for the tab label; will be displayed on the tab bar
 * @return          the widget where the content of the tab can be created
 */
lv_obj_t * lv_tabview_set_tab_translation_tag(lv_obj_t * obj, const char * tag);

#endif

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
 * @param obj       pointer to tabview widget
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
 * @return          the zero based index of the current tab
 */
uint32_t lv_tabview_get_tab_active(lv_obj_t * obj);

/**
 * Get a given tab button by index
 * @param obj       pointer to a tabview widget
 * @param idx       zero based index of the tab button to get.
 *                  < 0 means start counting tab button from the back (-1 is the last tab button)
 * @return          pointer to the tab button, or NULL if the index was out of range
 */
lv_obj_t * lv_tabview_get_tab_button(lv_obj_t * obj, int32_t idx);

/**
 * Get the widget where the container of each tab is created
 * @param obj       pointer to a tabview widget
 * @return          the main container widget
 */
lv_obj_t * lv_tabview_get_content(lv_obj_t * obj);

/**
 * Get the tab bar where the buttons are created
 * @param obj       pointer to a tabview widget
 * @return          the tab bar
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
