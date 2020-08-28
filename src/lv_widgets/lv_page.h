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
#include "../lv_conf_internal.h"

#if LV_USE_PAGE != 0

/*Testing of dependencies*/
#if LV_USE_CONT == 0
#error "lv_page: lv_cont is required. Enable it in lv_conf.h (LV_USE_CONT  1) "
#endif

#include "lv_cont.h"
#include "../lv_core/lv_indev.h"
#include "../lv_misc/lv_anim.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Scrollbar modes: shows when should the scrollbars be visible*/
enum {
    LV_SCROLLBAR_MODE_OFF    = 0x0, /**< Never show scroll bars*/
    LV_SCROLLBAR_MODE_ON     = 0x1, /**< Always show scroll bars*/
    LV_SCROLLBAR_MODE_DRAG   = 0x2, /**< Show scroll bars when page is being dragged*/
    LV_SCROLLBAR_MODE_AUTO   = 0x3, /**< Show scroll bars when the scrollable container is large enough to be scrolled*/
    LV_SCROLLBAR_MODE_HIDE   = 0x4, /**< Hide the scroll bar temporally*/
    LV_SCROLLBAR_MODE_UNHIDE = 0x8, /**< Unhide the previously hidden scroll bar. Recover original mode too*/
};
typedef uint8_t lv_scrollbar_mode_t;

/** Edges: describes the four edges of the page*/
enum { LV_PAGE_EDGE_LEFT = 0x1, LV_PAGE_EDGE_TOP = 0x2, LV_PAGE_EDGE_RIGHT = 0x4, LV_PAGE_EDGE_BOTTOM = 0x8 };
typedef uint8_t lv_page_edge_t;

/*Data of page*/
typedef struct {
    lv_cont_ext_t bg; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * scrl; /*The scrollable object on the background*/
    struct {
        lv_style_list_t style; /*Style of scrollbars*/
        lv_area_t hor_area;       /*Horizontal scrollbar area relative to the page. (Handled by the library) */
        lv_area_t ver_area;       /*Vertical scrollbar area relative to the page (Handled by the library)*/
        uint8_t hor_draw : 1;     /*1: horizontal scrollbar is visible now (Handled by the library)*/
        uint8_t ver_draw : 1;     /*1: vertical scrollbar is visible now (Handled by the library)*/
        lv_scrollbar_mode_t mode : 3;    /*Scrollbar visibility from 'lv_scrollbar_mode_t'*/
    } scrlbar;
#if LV_USE_ANIMATION
    struct {
        lv_anim_value_t state;    /*Store the current size of the edge flash effect*/
        lv_style_list_t style; /*Style of edge flash effect (usually homogeneous circle)*/
        uint8_t enabled : 1;      /*1: Show a flash animation on the edge*/
        uint8_t top_ip : 1;       /*Used internally to show that top most position is reached (flash is In
                                     Progress)*/
        uint8_t bottom_ip : 1;    /*Used internally to show that bottom most position is reached (flash
                                     is In Progress)*/
        uint8_t right_ip : 1;     /*Used internally to show that right most position is reached (flash
                                     is In Progress)*/
        uint8_t left_ip : 1;      /*Used internally to show that left most position is reached (flash is
                                     In Progress)*/
    } edge_flash;

    uint16_t anim_time; /*Scroll animation time*/
#endif
    lv_obj_t * scroll_prop_obj;          /*Pointer to child page from where the scroll is being propagated */
    uint8_t scroll_prop : 1;   /*The direction of the scroll propagation*/
} lv_page_ext_t;

enum {
    LV_PAGE_PART_BG = LV_CONT_PART_MAIN,
    LV_PAGE_PART_SCROLLBAR = _LV_OBJ_PART_VIRTUAL_LAST,
    LV_PAGE_PART_EDGE_FLASH,
    _LV_PAGE_PART_VIRTUAL_LAST,

    LV_PAGE_PART_SCROLLABLE = _LV_OBJ_PART_REAL_LAST,
    _LV_PAGE_PART_REAL_LAST,
};
typedef uint8_t lv_part_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a page objects
 * @param par pointer to an object, it will be the parent of the new page
 * @param copy pointer to a page object, if not NULL then the new object will be copied from it
 * @return pointer to the created page
 */
lv_obj_t * lv_page_create(lv_obj_t * par, const lv_obj_t * copy);

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param page pointer to an object
 */
void lv_page_clean(lv_obj_t * page);

/**
 * Get the scrollable object of a page
 * @param page pointer to a page object
 * @return pointer to a container which is the scrollable part of the page
 */
lv_obj_t * lv_page_get_scrollable(const lv_obj_t * page);

/**
 * Get the animation time
 * @param page pointer to a page object
 * @return the animation time in milliseconds
 */
uint16_t lv_page_get_anim_time(const lv_obj_t * page);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the scroll bar mode on a page
 * @param page pointer to a page object
 * @param sb_mode the new mode from 'lv_page_sb.mode_t' enum
 */
void lv_page_set_scrollbar_mode(lv_obj_t * page, lv_scrollbar_mode_t sb_mode);

/**
 * Set the animation time for the page
 * @param page pointer to a page object
 * @param anim_time animation time in milliseconds
 */
void lv_page_set_anim_time(lv_obj_t * page, uint16_t anim_time);

/**
 * Enable the scroll propagation feature. If enabled then the page will move its parent if there is
 * no more space to scroll.
 * The page needs to have a page-like parent (e.g. `lv_page`, `lv_tabview` tab, `lv_win` content area etc)
 * If enabled drag direction will be changed `LV_DRAG_DIR_ONE` automatically to allow scrolling only in one direction at one time.
 * @param page pointer to a Page
 * @param en true or false to enable/disable scroll propagation
 */
void lv_page_set_scroll_propagation(lv_obj_t * page, bool en);

/**
 * Enable the edge flash effect. (Show an arc when the an edge is reached)
 * @param page pointer to a Page
 * @param en true or false to enable/disable end flash
 */
void lv_page_set_edge_flash(lv_obj_t * page, bool en);

/**
 * Set the fit policy in all 4 directions separately.
 * It tell how to change the page size automatically.
 * @param page pointer to a page object
 * @param left left fit policy from `lv_fit_t`
 * @param right right fit policy from `lv_fit_t`
 * @param top bottom fit policy from `lv_fit_t`
 * @param bottom bottom fit policy from `lv_fit_t`
 */
static inline void lv_page_set_scrollable_fit4(lv_obj_t * page, lv_fit_t left, lv_fit_t right, lv_fit_t top,
                                               lv_fit_t bottom)
{
    lv_cont_set_fit4(lv_page_get_scrollable(page), left, right, top, bottom);
}

/**
 * Set the fit policy horizontally and vertically separately.
 * It tell how to change the page size automatically.
 * @param page pointer to a page object
 * @param hot horizontal fit policy from `lv_fit_t`
 * @param ver vertical fit policy from `lv_fit_t`
 */
static inline void lv_page_set_scrollable_fit2(lv_obj_t * page, lv_fit_t hor, lv_fit_t ver)
{
    lv_cont_set_fit2(lv_page_get_scrollable(page), hor, ver);
}

/**
 * Set the fit policy in all 4 direction at once.
 * It tell how to change the page size automatically.
 * @param page pointer to a button object
 * @param fit fit policy from `lv_fit_t`
 */
static inline void lv_page_set_scrollable_fit(lv_obj_t * page, lv_fit_t fit)
{
    lv_cont_set_fit(lv_page_get_scrollable(page), fit);
}

/**
 * Set width of the scrollable part of a page
 * @param page pointer to a page object
 * @param w the new width of the scrollable (it ha no effect is horizontal fit is enabled)
 */
static inline void lv_page_set_scrl_width(lv_obj_t * page, lv_coord_t w)
{
    lv_obj_set_width(lv_page_get_scrollable(page), w);
}

/**
 * Set height of the scrollable part of a page
 * @param page pointer to a page object
 * @param h the new height of the scrollable (it ha no effect is vertical fit is enabled)
 */
static inline void lv_page_set_scrl_height(lv_obj_t * page, lv_coord_t h)
{
    lv_obj_set_height(lv_page_get_scrollable(page), h);
}

/**
 * Set the layout of the scrollable part of the page
 * @param page pointer to a page object
 * @param layout a layout from 'lv_cont_layout_t'
 */
static inline void lv_page_set_scrl_layout(lv_obj_t * page, lv_layout_t layout)
{
    lv_cont_set_layout(lv_page_get_scrollable(page), layout);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Set the scroll bar mode on a page
 * @param page pointer to a page object
 * @return the mode from 'lv_page_sb.mode_t' enum
 */
lv_scrollbar_mode_t lv_page_get_scrollbar_mode(const lv_obj_t * page);

/**
 * Get the scroll propagation property
 * @param page pointer to a Page
 * @return true or false
 */
bool lv_page_get_scroll_propagation(lv_obj_t * page);

/**
 * Get the edge flash effect property.
 * @param page pointer to a Page
 * return true or false
 */
bool lv_page_get_edge_flash(lv_obj_t * page);

/**
 * Get that width which can be set to the children to still not cause overflow (show scrollbars)
 * @param page pointer to a page object
 * @return the width which still fits into the page
 */
lv_coord_t lv_page_get_width_fit(lv_obj_t * page);

/**
 * Get that height which can be set to the children to still not cause overflow (show scrollbars)
 * @param page pointer to a page object
 * @return the height which still fits into the page
 */
lv_coord_t lv_page_get_height_fit(lv_obj_t * page);

/**
 * Divide the width of the object and get the width of a given number of columns.
 * Take into account the paddings of the background and scrollable too.
 * @param page pointer to an object
 * @param div indicates how many columns are assumed.
 * If 1 the width will be set the the parent's width
 * If 2 only half parent width - inner padding of the parent
 * If 3 only third parent width - 2 * inner padding of the parent
 * @param span how many columns are combined
 * @return the width according to the given parameters
 */
lv_coord_t lv_page_get_width_grid(lv_obj_t * page, uint8_t div, uint8_t span);

/**
 * Divide the height of the object and get the width of a given number of columns.
 * Take into account the paddings of the background and scrollable too.
 * @param page pointer to an object
 * @param div indicates how many rows are assumed.
 * If 1 the height will be set the the parent's height
 * If 2 only half parent height - inner padding of the parent
 * If 3 only third parent height - 2 * inner padding of the parent
 * @param span how many rows are combined
 * @return the height according to the given parameters
 */
lv_coord_t lv_page_get_height_grid(lv_obj_t * page, uint8_t div, uint8_t span);

/**
 * Get width of the scrollable part of a page
 * @param page pointer to a page object
 * @return the width of the scrollable
 */
static inline lv_coord_t lv_page_get_scrl_width(const lv_obj_t * page)
{
    return lv_obj_get_width(lv_page_get_scrollable(page));
}

/**
 * Get height of the scrollable part of a page
 * @param page pointer to a page object
 * @return the height of the scrollable
 */
static inline lv_coord_t lv_page_get_scrl_height(const lv_obj_t * page)
{
    return lv_obj_get_height(lv_page_get_scrollable(page));
}

/**
 * Get the layout of the scrollable part of a page
 * @param page pointer to page object
 * @return the layout from 'lv_cont_layout_t'
 */
static inline lv_layout_t lv_page_get_scrl_layout(const lv_obj_t * page)
{
    return lv_cont_get_layout(lv_page_get_scrollable(page));
}

/**
 * Get the left fit mode
 * @param page pointer to a page object
 * @return an element of `lv_fit_t`
 */
static inline lv_fit_t lv_page_get_scrl_fit_left(const lv_obj_t * page)
{
    return lv_cont_get_fit_left(lv_page_get_scrollable(page));
}

/**
 * Get the right fit mode
 * @param page pointer to a page object
 * @return an element of `lv_fit_t`
 */
static inline lv_fit_t lv_page_get_scrl_fit_right(const lv_obj_t * page)
{
    return lv_cont_get_fit_right(lv_page_get_scrollable(page));
}

/**
 * Get the top fit mode
 * @param page pointer to a page object
 * @return an element of `lv_fit_t`
 */
static inline lv_fit_t lv_page_get_scrl_fit_top(const lv_obj_t * page)
{
    return lv_cont_get_fit_top(lv_page_get_scrollable(page));
}

/**
 * Get the bottom fit mode
 * @param page pointer to a page object
 * @return an element of `lv_fit_t`
 */
static inline lv_fit_t lv_page_get_scrl_fit_bottom(const lv_obj_t * page)
{
    return lv_cont_get_fit_bottom(lv_page_get_scrollable(page));
}

/*=====================
 * Other functions
 *====================*/

/**
 * Find whether the page has been scrolled to a certain edge.
 * @param page Page object
 * @param edge Edge to check
 * @return true if the page is on the specified edge
 */
bool lv_page_on_edge(lv_obj_t * page, lv_page_edge_t edge);

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
 * @param anim_en LV_ANIM_ON to focus with animation; LV_ANIM_OFF to focus without animation
 */
void lv_page_focus(lv_obj_t * page, const lv_obj_t * obj, lv_anim_enable_t anim_en);

/**
 * Scroll the page horizontally
 * @param page pointer to a page object
 * @param dist the distance to scroll (< 0: scroll left; > 0 scroll right)
 */
void lv_page_scroll_hor(lv_obj_t * page, lv_coord_t dist);

/**
 * Scroll the page vertically
 * @param page pointer to a page object
 * @param dist the distance to scroll (< 0: scroll down; > 0 scroll up)
 */
void lv_page_scroll_ver(lv_obj_t * page, lv_coord_t dist);

/**
 * Not intended to use directly by the user but by other object types internally.
 * Start an edge flash animation.
 * @param page
 * @param edge the edge to flash. Can be `LV_PAGE_EDGE_LEFT/RIGHT/TOP/BOTTOM`
 */
void lv_page_start_edge_flash(lv_obj_t * page, lv_page_edge_t edge);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_PAGE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_PAGE_H*/
