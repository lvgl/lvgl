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
#include "lv_conf.h"
#if USE_LV_LIST != 0

/*Testing of dependencies*/
#if USE_LV_PAGE == 0
#error "lv_list: lv_page is required. Enable it in lv_conf.h (USE_LV_PAGE  1) "
#endif

#if USE_LV_BTN == 0
#error "lv_list: lv_btn is required. Enable it in lv_conf.h (USE_LV_BTN  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_list: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif


#include "../lv_obj/lv_obj.h"
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
typedef struct
{
    lv_page_ext_t page; /*Ext. of ancestor*/
    /*New data for this type */
    uint16_t anim_time;                          /*Scroll animation time*/
    lv_style_t *styles_btn[LV_BTN_STATE_NUM];    /*Styles of the list element buttons*/
    lv_style_t *style_img;                       /*Style of the list element images on buttons*/
}lv_list_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a list objects
 * @param par pointer to an object, it will be the parent of the new list
 * @param copy pointer to a list object, if not NULL then the new object will be copied from it
 * @return pointer to the created list
 */
lv_obj_t * lv_list_create(lv_obj_t *par, lv_obj_t * copy);

/**
 * Add a list element to the list
 * @param list pointer to list object
 * @param img_fn file name of an image before the text (NULL if unused)
 * @param txt text of the list element (NULL if unused)
 * @param rel_action pointer to release action function (like with lv_btn)
 * @return pointer to the new list element which can be customized (a button)
 */
lv_obj_t * lv_list_add(lv_obj_t *list, const char * img_fn, const char * txt, lv_action_t rel_action);

/**
 * Move the list elements up by one
 * @param list pointer a to list object
 */
void lv_list_up(lv_obj_t *list);

/**
 * Move the list elements down by one
 * @param list pointer to a list object
 */
void lv_list_down(lv_obj_t *list);

/**
 * Focus on a list button. It ensures that the button will be visible on the list.
 * @param btn pointer to a list button to focus
 * @param anim_en true: scroll with animation, false: without animation
 */
void lv_list_focus(lv_obj_t *btn, bool anim_en);

/**
 * Set styles of the list elements of a list in each state
 * @param list pointer to list object
 * @param rel pointer to a style for releases state
 * @param pr  pointer to a style for pressed state
 * @param trel pointer to a style for toggled releases state
 * @param tpr pointer to a style for toggled pressed state
 * @param ina pointer to a style for inactive state
 */
void lv_list_set_style_btn(lv_obj_t * list, lv_style_t * rel, lv_style_t * pr,
                            lv_style_t * trel, lv_style_t * tpr,
                            lv_style_t * ina);

/**
 * Set scroll animation duration on 'list_up()' 'list_down()' 'list_focus()'
 * @param list pointer to a list object
 * @param anim_time duration of animation [ms]
 */
void lv_list_set_anim_time(lv_obj_t *list, uint16_t anim_time);

/**
 * Get the text of a list element
 * @param liste pointer to list element
 * @return pointer to the text
 */
const char * lv_list_get_element_text(lv_obj_t * liste);

/**
 * Get the label object from a list element
 * @param liste pointer to a list element (button)
 * @return pointer to the label from the list element or NULL if not found
 */
lv_obj_t * lv_list_get_btn_label(lv_obj_t * liste);

/**
 * Get the image object from a list element
 * @param liste pointer to a list element (button)
 * @return pointer to the image from the list element or NULL if not found
 */
lv_obj_t * lv_list_get_btn_img(lv_obj_t * liste);

/**
 * Get the scroll bar outside attribute
 * @param list pointer to list object
 * @param en true: scroll bar outside the buttons, false: scroll bar inside
 */
bool lv_list_get_sb_out(lv_obj_t * list, bool en);

/**
 * Get the style of the list elements in a given state
 * @param list pointer to a list object
 * @param state a state from 'lv_btn_state_t' in which style should be get
 * @return pointer to the style in the given state
 */
lv_style_t * lv_list_get_style_btn(lv_obj_t * list, lv_btn_state_t state);

/**
 * Get scroll animation duration
 * @param list pointer to a list object
 * @return duration of animation [ms]
 */
uint16_t lv_list_get_anim_time(lv_obj_t *list);

/****************************
 * TRANSPARENT API FUNCTIONS
 ***************************/

/**
 * Set a new styles for the list
 * @param list pointer to a list object
 * @param bg pointer to a style for the background (typically transparent)
 * @param scrl pointer to a style for the scrollable area
 * @param sb pointer to a style for the scroll bars
 */
static inline void lv_list_set_style(lv_obj_t *list, lv_style_t *bg, lv_style_t *scrl, lv_style_t *sb)
{
    lv_page_set_style(list, bg, scrl, sb);
}

/**
 * Set the scroll bar mode of a list
 * @param list pointer to a list object
 * @param sb_mode the new mode from 'lv_page_sb_mode_t' enum
 */
static inline void lv_list_set_sb_mode(lv_obj_t * list, lv_page_sb_mode_t mode)
{
    lv_page_set_sb_mode(list, mode);
}

/**
 * Get the scroll bar mode of a list
 * @param list pointer to a list object
 * @return scrollbar mode from 'lv_page_sb_mode_t' enum
 */
static inline lv_page_sb_mode_t lv_list_get_sb_mode(lv_obj_t * list)
{
    return lv_page_get_sb_mode(list);
}

/**
 * Get a style of a list's background
 * @param list pointer to a list object
 * @return pointer to the background's style
 */
static inline lv_style_t * lv_list_get_style_bg(lv_obj_t *list)
{
    return lv_page_get_style_bg(list);
}

/**
 * Get a style of a list's scrollable part
 * @param list pointer to a list object
 * @return pointer to the scrollable"s style
 */
static inline lv_style_t * lv_list_get_style_scrl(lv_obj_t *list)
{
    return lv_page_get_style_scrl(list);
}

/**
* Get the style of the scrollbars of a list
* @param list pointer to a list object
* @return pointer to the style of the scrollbars
*/
static inline lv_style_t * lv_list_get_style_sb(lv_obj_t *list)
{
    return lv_page_get_style_sb(list);
}

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_LIST*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_LIST_H*/
