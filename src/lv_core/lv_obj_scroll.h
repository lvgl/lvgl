/**
 * @file lv_obj_scroll.h
 *
 */

#ifndef LV_OBJ_SCROLL_H
#define LV_OBJ_SCROLL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_anim.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;

/** Scrollbar modes: shows when should the scrollbars be visible*/
enum {
    LV_SCROLL_MODE_OFF    = 0x0, /**< Never show scroll bars*/
    LV_SCROLL_MODE_ON     = 0x1, /**< Always show scroll bars*/
    LV_SCROLL_MODE_ACTIVE = 0x2, /**< Show scroll bars when object is being scrolled*/
    LV_SCROLL_MODE_AUTO   = 0x3, /**< Show scroll bars when the content is large enough to be scrolled*/
};
typedef uint8_t lv_scroll_mode_t;


enum {
    LV_SCROLL_SNAP_ALIGN_NONE,
    LV_SCROLL_SNAP_ALIGN_START,
    LV_SCROLL_SNAP_ALIGN_END,
    LV_SCROLL_SNAP_ALIGN_CENTER
};
typedef uint8_t lv_scroll_snap_align_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Set how the scrollbars should behave.
 * @param obj pointer to an object
 * @param mode: LV_SCROLL_MODE_ON/OFF/AUTO/ACTIVE
 */
void lv_obj_set_scroll_mode(struct _lv_obj_t * obj, lv_scroll_mode_t mode);


/**
 * Moves all children with horizontally or vertically.
 * It doesn't take into account any limits so any values are possible
 * @param obj pointer to an object whose children should be moved
 * @param x pixel to move horizontally
 * @param y pixels to move vertically
 */
void _lv_obj_scroll_by_raw(struct _lv_obj_t * obj, lv_coord_t x, lv_coord_t y);

/**
 * Moves all children with horizontally or vertically.
 * Limits the scroll to the bounding box of the children.
 * @param obj pointer to an object whose children should be moved
 * @param x pixel to move horizontally
 * @param y pixels to move vertically
 */
void lv_obj_scroll_by(struct _lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en);

/**
 * Scroll the a given x coordinate to the left side of obj.
 * @param obj pointer to an object which should be scrolled
 * @param x the x coordinate to scroll to
 * @param y the y coordinate to scroll to
 */
void lv_obj_scroll_to(struct _lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en);

/**
 * Scroll the a given x coordinate to the left side of obj.
 * @param obj pointer to an object which should be scrolled
 * @param x the x coordinate to scroll to
 */
void lv_obj_scroll_to_x(struct _lv_obj_t * obj, lv_coord_t x, lv_anim_enable_t anim_en);

/**
 * Scroll the a given y coordinate to the top side of obj.
 * @param obj pointer to an object which should be scrolled
 * @param y the y coordinate to scroll to
 */
void lv_obj_scroll_to_y(struct _lv_obj_t * obj, lv_coord_t y, lv_anim_enable_t anim_en);


lv_coord_t lv_obj_get_scroll_x(const struct _lv_obj_t * obj);

lv_coord_t lv_obj_get_scroll_y(const struct _lv_obj_t * obj);
/**
 *
 * Return the height of the area above the parent.
 * That is the number of pixels the object can be scrolled down.
 * Normally positive but can be negative when scrolled inside.
 * @param obj
 * @return
 */
lv_coord_t lv_obj_get_scroll_top(struct _lv_obj_t * obj);

/**
 * Return the height of the area below the parent.
 * That is the number of pixels the object can be scrolled up.
 * Normally positive but can be negative when scrolled inside.
 * @param obj
 * @return
 */
lv_coord_t lv_obj_get_scroll_bottom(struct _lv_obj_t * obj);


/**
 * Return the weight of the area on the left the parent.
 * That is the number of pixels the object can be scrolled down.
 * Normally positive but can be negative when scrolled inside.
 * @param obj
 * @return
 */
lv_coord_t lv_obj_get_scroll_left(struct _lv_obj_t * obj);

/**
 * Return the width of the area below the object.
 * That is the number of pixels the object can be scrolled left.
 * Normally positive but can be negative when scrolled inside.
 * @param obj
 * @return
 */
lv_coord_t lv_obj_get_scroll_right(struct _lv_obj_t * obj);

/**
 * Get the X and Y coordinates where the scrolling would end for this object if a scrolling animation is in progress.
 * In no scrolling animation give the current `left` or `top` scroll position.
 * @param obj pointer to an object
 * @param end poinr to point to store the result
 */
void lv_obj_get_scroll_end(struct _lv_obj_t  * obj, lv_point_t * end);


lv_dir_t lv_obj_get_scroll_dir(const struct _lv_obj_t * obj);
void lv_obj_set_scroll_dir(struct _lv_obj_t * obj, lv_dir_t dir);
lv_scroll_mode_t lv_obj_get_scroll_mode(const struct _lv_obj_t * obj);
lv_scroll_snap_align_t lv_obj_get_snap_align_x(const struct _lv_obj_t * obj);
void lv_obj_set_snap_align_x(struct _lv_obj_t * obj, lv_scroll_snap_align_t align);
lv_scroll_snap_align_t lv_obj_get_snap_align_y(const struct _lv_obj_t * obj);
void lv_obj_set_snap_align_y(struct _lv_obj_t * obj, lv_scroll_snap_align_t align);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJ_SCROLL_H*/
