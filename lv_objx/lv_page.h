/**
 * @file lv_page.h
 * 
 */

#ifndef LV_PAGE_H
#define LV_PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_PAGE != 0

/*Testing of dependencies*/
#if USE_LV_CONT == 0
#error "lv_page: lv_cont is required. Enable it in lv_conf.h (USE_LV_CONT  1) "
#endif

#include "lv_cont.h"
#include "../lv_core/lv_indev.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Scrollbar modes: shows when should the scrollbars be visible*/
typedef enum
{
    LV_SB_MODE_OFF,    /*Never show scrollbars*/
    LV_SB_MODE_ON,     /*Always show scrollbars*/
    LV_SB_MODE_DRAG,   /*Show scrollbars when page is being dragged*/
    LV_SB_MODE_AUTO,   /*Show scrollbars when the scrollable container is large enough to be scrolled*/
}lv_sb_mode_t;

/*Data of page*/
typedef struct
{
    lv_cont_ext_t bg; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * scrl;            /*The scrollable object on the background*/
    lv_action_t rel_action;     /*Function to call when the page is released*/
    lv_action_t pr_action;      /*Function to call when the page is pressed*/
    struct {
        lv_style_t *style;          /*Style of scrollbars*/
        lv_area_t hor_area;            /*Horizontal scrollbar area relative to the page. (Handled by the library) */
        lv_area_t ver_area;            /*Vertical scrollbar area relative to the page (Handled by the library)*/
        uint8_t hor_draw :1;        /*1: horizontal scrollbar is visible now (Handled by the library)*/
        uint8_t ver_draw :1;        /*1: vertical scrollbar is visible now (Handled by the library)*/
        uint8_t mode     :3;        /*Scrollbar visibility from 'lv_page_sb_mode_t'*/
    }sb;
}lv_page_ext_t;

typedef enum {
    LV_PAGE_STYLE_BG,
    LV_PAGE_STYLE_SCRL,
    LV_PAGE_STYLE_SB,
}lv_page_style_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**
 * Create a page objects
 * @param par pointer to an object, it will be the parent of the new page
 * @param copy pointer to a page object, if not NULL then the new object will be copied from it
 * @return pointer to the created page
 */
lv_obj_t * lv_page_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Get the scrollable object of a page
 * @param page pointer to a page object
 * @return pointer to a container which is the scrollable part of the page
 */
lv_obj_t * lv_page_get_scrl(lv_obj_t * page);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a release action for the page
 * @param page pointer to a page object
 * @param rel_action a function to call when the page is released
 */
void lv_page_set_rel_action(lv_obj_t * page, lv_action_t rel_action);

/**
 * Set a press action for the page
 * @param page pointer to a page object
 * @param pr_action a function to call when the page is pressed
 */
void lv_page_set_pr_action(lv_obj_t * page, lv_action_t pr_action);

/**
 * Set the scroll bar mode on a page
 * @param page pointer to a page object
 * @param sb.mode the new mode from 'lv_page_sb.mode_t' enum
 */
void lv_page_set_sb_mode(lv_obj_t * page, lv_sb_mode_t sb_mode);

/**
 * Set the fit attribute of the scrollable part of a page.
 * It means it can set its size automatically to involve all children.
 * (Can be set separately horizontally and vertically)
 * @param page pointer to a page object
 * @param hor_en true: enable horizontal fit
 * @param ver_en true: enable vertical fit
 */
static inline void lv_page_set_scrl_fit(lv_obj_t *page, bool hor_en, bool ver_en)
{
    lv_cont_set_fit(lv_page_get_scrl(page), hor_en, ver_en);
}

/**
 * Set width of the scrollable part of a page
 * @param page pointer to a page object
 * @param w the new width of the scrollable (it ha no effect is horizontal fit is enabled)
 */
static inline void lv_page_set_scrl_width(lv_obj_t *page, lv_coord_t w)
{
    lv_obj_set_width(lv_page_get_scrl(page), w);
}

/**
 * Set height of the scrollable part of a page
 * @param page pointer to a page object
 * @param h the new height of the scrollable (it ha no effect is vertical fit is enabled)
 */
static inline void lv_page_set_scrl_height(lv_obj_t *page, lv_coord_t h)
{
    lv_obj_set_height(lv_page_get_scrl(page), h);

}

/**
* Set the layout of the scrollable part of the page
* @param page pointer to a page object
* @param layout a layout from 'lv_cont_layout_t'
*/
static inline void lv_page_set_scrl_layout(lv_obj_t * page, lv_layout_t layout)
{
    lv_cont_set_layout(lv_page_get_scrl(page), layout);
}

/**
 * Set a style of a page
 * @param page pointer to a page object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_page_set_style(lv_obj_t *page, lv_page_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Set the scroll bar mode on a page
 * @param page pointer to a page object
 * @return the mode from 'lv_page_sb.mode_t' enum
 */
lv_sb_mode_t lv_page_get_sb_mode(lv_obj_t * page);

/**
 * Get width of the scrollable part of a page
 * @param page pointer to a page object
 * @return the width of the scrollable
 */
static inline lv_coord_t lv_page_get_scrl_width(lv_obj_t *page)
{
    return lv_obj_get_width(lv_page_get_scrl(page));
}

/**
 * Get height of the scrollable part of a page
 * @param page pointer to a page object
 * @return the height of the scrollable
 */
static inline lv_coord_t lv_page_get_scrl_height(lv_obj_t *page)
{
    return lv_obj_get_height(lv_page_get_scrl(page));
}

/**
* Get the layout of the scrollable part of a page
* @param page pointer to page object
* @return the layout from 'lv_cont_layout_t'
*/
static inline lv_layout_t lv_page_get_scrl_layout(lv_obj_t * page)
{
    return lv_cont_get_layout(lv_page_get_scrl(page));
}

/**
* Get horizontal fit attribute of the scrollable part of a page
* @param page pointer to a page object
* @return true: horizontal fit is enabled; false: disabled
*/
static inline bool lv_page_get_scrl_hor_fit(lv_obj_t * page)
{
    return lv_cont_get_hor_fit(lv_page_get_scrl(page));
}

/**
* Get vertical fit attribute of the scrollable part of a page
* @param page pointer to a page object
* @return true: vertical fit is enabled; false: disabled
*/
static inline bool lv_page_get_scrl_fit_ver(lv_obj_t * page)
{
    return lv_cont_get_ver_fit(lv_page_get_scrl(page));
}

/**
 * Get a style of a page
 * @param page pointer to page object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_page_get_style(lv_obj_t *page, lv_page_style_t type);

/*=====================
 * Other functions
 *====================*/

/**
 * Glue the object to the page. After it the page can be moved (dragged) with this object too.
 * @param obj pointer to an object on a page
 * @param glue true: enable glue, false: disable glue
 */
void lv_page_glue_obj(lv_obj_t * obj, bool glue);

/**
 * Focus on an object. It ensures that the object will be visible on the page.
 * @param page pointer to a page object
 * @param obj pointer to an object to focus (must be on the page)
 * @param anim_time scroll animation time in milliseconds (0: no animation)
 */
void lv_page_focus(lv_obj_t * page, lv_obj_t * obj, uint16_t anim_time);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_PAGE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_PAGE_H*/
