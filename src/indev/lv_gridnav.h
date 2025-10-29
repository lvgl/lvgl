/**
 * @file lv_gridnav.h
 *
 */

#ifndef LV_GRIDNAV_H
#define LV_GRIDNAV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"

#if LV_USE_GRIDNAV

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_GRIDNAV_CTRL_NONE = 0x0,

    /**
     * If there is no next/previous object in a direction,
     * the focus goes to the object in the next/previous row (on left/right keys)
     * or first/last row (on up/down keys)
     */
    LV_GRIDNAV_CTRL_ROLLOVER = 0x1,

    /**
     * If an arrow is pressed and the focused object can be scrolled in that direction
     * then it will be scrolled instead of going to the next/previous object.
     * If there is no more room for scrolling the next/previous object will be focused normally */
    LV_GRIDNAV_CTRL_SCROLL_FIRST = 0x2,

    /**
     * Only use left/right keys for grid navigation. Up/down key events will be sent to the
     * focused object.
    */
    LV_GRIDNAV_CTRL_HORIZONTAL_MOVE_ONLY = 0x4,

    /**
     * Only use up/down keys for grid navigation. Left/right key events will be sent to the
     * focused object.
    */
    LV_GRIDNAV_CTRL_VERTICAL_MOVE_ONLY = 0x8

} lv_gridnav_ctrl_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Add grid navigation feature to an object. It expects the children to be arranged
 * into a grid-like layout. Although it's not required to have pixel perfect alignment.
 * This feature makes possible to use keys to navigate among the children and focus them.
 * The keys other than arrows and press/release related events
 * are forwarded to the focused child.
 * @param obj       pointer to an object on which navigation should be applied.
 * @param ctrl      control flags from `lv_gridnav_ctrl_t`.
 */
void lv_gridnav_add(lv_obj_t * obj, lv_gridnav_ctrl_t ctrl);

/**
 * Remove the grid navigation support from an object
 * @param obj       pointer to an object
 */
void lv_gridnav_remove(lv_obj_t * obj);

/**
 * Manually focus an object on gridnav container
 * @param cont      pointer to a gridnav container
 * @param to_focus  pointer to an object to focus
 * @param anim_en   LV_ANIM_ON/OFF
 */
void lv_gridnav_set_focused(lv_obj_t * cont, lv_obj_t * to_focus, lv_anim_enable_t anim_en);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_GRIDNAV*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_GRIDNAV_H */
