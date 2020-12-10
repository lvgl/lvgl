/**
 * @file lv_list.h
 *
 */

#ifndef LV_LIST_H
#define LV_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_LIST != 0

/*Testing of dependencies*/
#if LV_USE_PAGE == 0
#error "lv_list: lv_page is required. Enable it in lv_conf.h (LV_USE_PAGE  1) "
#endif

#if LV_USE_BTN == 0
#error "lv_list: lv_btn is required. Enable it in lv_conf.h (LV_USE_BTN  1) "
#endif

#if LV_USE_LABEL == 0
#error "lv_list: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "lv_page.h"
#include "lv_btn.h"
#include "lv_label.h"
#include "lv_img.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of list*/
typedef struct {
    lv_page_ext_t page; /*Ext. of ancestor*/
    /*New data for this type */

#if LV_USE_GROUP
    lv_obj_t * last_sel_btn;     /* The last selected button. It will be reverted when the list is focused again */
#endif
    lv_obj_t * act_sel_btn; /* The button is currently being selected*/
} lv_list_ext_t;

/** List styles. */
enum {
    LV_LIST_PART_BG = LV_PAGE_PART_BG, /**< List background style */
    LV_LIST_PART_SCROLLBAR = LV_PAGE_PART_SCROLLBAR, /**< List scrollbar style. */
    LV_LIST_PART_EDGE_FLASH = LV_PAGE_PART_EDGE_FLASH, /**< List edge flash style. */
    _LV_LIST_PART_VIRTUAL_LAST = _LV_PAGE_PART_VIRTUAL_LAST,
    LV_LIST_PART_SCROLLABLE = LV_PAGE_PART_SCROLLABLE, /**< List scrollable area style. */
    _LV_LIST_PART_REAL_LAST = _LV_PAGE_PART_REAL_LAST,
};
typedef uint8_t lv_list_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a list objects
 * @param par pointer to an object, it will be the parent of the new list
 * @param copy pointer to a list object, if not NULL then the new object will be copied from it
 * @return pointer to the created list
 */
lv_obj_t * lv_list_create(lv_obj_t * par, const lv_obj_t * copy);

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param list pointer to an object
 */
void lv_list_clean(lv_obj_t * list);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add a list element to the list
 * @param list pointer to list object
 * @param img_fn file name of an image before the text (NULL if unused)
 * @param txt text of the list element (NULL if unused)
 * @return pointer to the new list element which can be customized (a button)
 */
lv_obj_t * lv_list_add_btn(lv_obj_t * list, const void * img_src, const char * txt);

/**
 * Remove the index of the button in the list
 * @param list pointer to a list object
 * @param index pointer to a the button's index in the list, index must be 0 <= index <
 * lv_list_ext_t.size
 * @return true: successfully deleted
 */
bool lv_list_remove(const lv_obj_t * list, uint16_t index);

/*=====================
 * Setter functions
 *====================*/

/**
 * Make a button selected
 * @param list pointer to a list object
 * @param btn pointer to a button to select
 *            NULL to not select any buttons
 */
void lv_list_focus_btn(lv_obj_t * list, lv_obj_t * btn);

/**
 * Set the scroll bar mode of a list
 * @param list pointer to a list object
 * @param sb_mode the new mode from 'lv_page_sb_mode_t' enum
 */
static inline void lv_list_set_scrollbar_mode(lv_obj_t * list, lv_scrollbar_mode_t mode)
{
    lv_page_set_scrollbar_mode(list, mode);
}

/**
 * Enable the scroll propagation feature. If enabled then the List will move its parent if there is
 * no more space to scroll.
 * @param list pointer to a List
 * @param en true or false to enable/disable scroll propagation
 */
static inline void lv_list_set_scroll_propagation(lv_obj_t * list, bool en)
{
    lv_page_set_scroll_propagation(list, en);
}

/**
 * Enable the edge flash effect. (Show an arc when the an edge is reached)
 * @param list pointer to a List
 * @param en true or false to enable/disable end flash
 */
static inline void lv_list_set_edge_flash(lv_obj_t * list, bool en)
{
    lv_page_set_edge_flash(list, en);
}

/**
 * Set scroll animation duration on 'list_up()' 'list_down()' 'list_focus()'
 * @param list pointer to a list object
 * @param anim_time duration of animation [ms]
 */
static inline void lv_list_set_anim_time(lv_obj_t * list, uint16_t anim_time)
{
    lv_page_set_anim_time(list, anim_time);
}

/**
 * Set layout of a list
 * @param list pointer to a list object
 * @param layout which layout should be used
 */
void lv_list_set_layout(lv_obj_t * list, lv_layout_t layout);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a list element
 * @param btn pointer to list element
 * @return pointer to the text
 */
const char * lv_list_get_btn_text(const lv_obj_t * btn);
/**
 * Get the label object from a list element
 * @param btn pointer to a list element (button)
 * @return pointer to the label from the list element or NULL if not found
 */
lv_obj_t * lv_list_get_btn_label(const lv_obj_t * btn);

/**
 * Get the image object from a list element
 * @param btn pointer to a list element (button)
 * @return pointer to the image from the list element or NULL if not found
 */
lv_obj_t * lv_list_get_btn_img(const lv_obj_t * btn);

/**
 * Get the next button from list. (Starts from the bottom button)
 * @param list pointer to a list object
 * @param prev_btn pointer to button. Search the next after it.
 * @return pointer to the next button or NULL when no more buttons
 */
lv_obj_t * lv_list_get_prev_btn(const lv_obj_t * list, lv_obj_t * prev_btn);

/**
 * Get the previous button from list. (Starts from the top button)
 * @param list pointer to a list object
 * @param prev_btn pointer to button. Search the previous before it.
 * @return pointer to the previous button or NULL when no more buttons
 */
lv_obj_t * lv_list_get_next_btn(const lv_obj_t * list, lv_obj_t * prev_btn);

/**
 * Get the index of the button in the list
 * @param list pointer to a list object. If NULL, assumes btn is part of a list.
 * @param btn pointer to a list element (button)
 * @return the index of the button in the list, or -1 of the button not in this list
 */
int32_t lv_list_get_btn_index(const lv_obj_t * list, const lv_obj_t * btn);

/**
 * Get the number of buttons in the list
 * @param list pointer to a list object
 * @return the number of buttons in the list
 */
uint16_t lv_list_get_size(const lv_obj_t * list);

#if LV_USE_GROUP
/**
 * Get the currently selected button. Can be used while navigating in the list with a keypad.
 * @param list pointer to a list object
 * @return pointer to the selected button
 */
lv_obj_t * lv_list_get_btn_selected(const lv_obj_t * list);
#endif

/**
 * Get layout of a list
 * @param list pointer to a list object
 * @return layout of the list object
 */
lv_layout_t lv_list_get_layout(lv_obj_t * list);

/**
 * Get the scroll bar mode of a list
 * @param list pointer to a list object
 * @return scrollbar mode from 'lv_scrollbar_mode_t' enum
 */
static inline lv_scrollbar_mode_t lv_list_get_scrollbar_mode(const lv_obj_t * list)
{
    return lv_page_get_scrollbar_mode(list);
}

/**
 * Get the scroll propagation property
 * @param list pointer to a List
 * @return true or false
 */
static inline bool lv_list_get_scroll_propagation(lv_obj_t * list)
{
    return lv_page_get_scroll_propagation(list);
}

/**
 * Get the scroll propagation property
 * @param list pointer to a List
 * @return true or false
 */
static inline bool lv_list_get_edge_flash(lv_obj_t * list)
{
    return lv_page_get_edge_flash(list);
}

/**
 * Get scroll animation duration
 * @param list pointer to a list object
 * @return duration of animation [ms]
 */
static inline uint16_t lv_list_get_anim_time(const lv_obj_t * list)
{
    return lv_page_get_anim_time(list);
}

/*=====================
 * Other functions
 *====================*/

/**
 * Move the list elements up by one
 * @param list pointer a to list object
 */
void lv_list_up(const lv_obj_t * list);
/**
 * Move the list elements down by one
 * @param list pointer to a list object
 */
void lv_list_down(const lv_obj_t * list);

/**
 * Focus on a list button. It ensures that the button will be visible on the list.
 * @param btn pointer to a list button to focus
 * @param anim LV_ANIM_ON: scroll with animation, LV_ANIM_OFF: without animation
 */
void lv_list_focus(const lv_obj_t * btn, lv_anim_enable_t anim);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LIST*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LIST_H*/
