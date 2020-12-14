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

/* Can't include lv_obj.h because it includes this header file */
struct _lv_obj_t;

/** Scrollbar modes: shows when should the scrollbars be visible*/
enum {
    LV_SCROLLBAR_MODE_OFF    = 0x0, /**< Never show scroll bars*/
    LV_SCROLLBAR_MODE_ON     = 0x1, /**< Always show scroll bars*/
    LV_SCROLLBAR_MODE_ACTIVE = 0x2, /**< Show scroll bars when object is being scrolled*/
    LV_SCROLLBAR_MODE_AUTO   = 0x3, /**< Show scroll bars when the content is large enough to be scrolled*/
};
typedef uint8_t lv_scrollbar_mode_t;


enum {
    LV_SCROLL_SNAP_ALIGN_NONE,
    LV_SCROLL_SNAP_ALIGN_START,
    LV_SCROLL_SNAP_ALIGN_END,
    LV_SCROLL_SNAP_ALIGN_CENTER
};
typedef uint8_t lv_snap_align_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set how the scrollbars should behave.
 * @param obj pointer to an object
 * @param mode: LV_SCROLL_MODE_ON/OFF/AUTO/ACTIVE
 */
void lv_obj_set_scrollbar_mode(struct _lv_obj_t * obj, lv_scrollbar_mode_t mode);

/**
 * Set the object in which directions can be scrolled
 * @param obj pointer to an object
 * @param dir the allow scroll directions. An element or OR-ed values of `lv_dir_t`
 */
void lv_obj_set_scroll_dir(struct _lv_obj_t * obj, lv_dir_t dir);

/**
 * Set where to snap the children when scrolling ends horizontally
 * @param obj pointer to an object
 * @param align the snap align to set from `lv_snap_align_t`
 */
void lv_obj_set_snap_align_x(struct _lv_obj_t * obj, lv_snap_align_t align);

/**
 * Set where to snap the children when scrolling ends vertically
 * @param obj pointer to an object
 * @param align the snap align to set from `lv_snap_align_t`
 */
void lv_obj_set_snap_align_y(struct _lv_obj_t * obj, lv_snap_align_t align);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current scroll mode (when to hide the scrollbars)
 * @param obj pointer to an object
 * @return the current scroll mode from `lv_scroll_mode_t`
 */
lv_scrollbar_mode_t lv_obj_get_scrollbar_mode(const struct _lv_obj_t * obj);

/**
 * Get the object in which directions can be scrolled
 * @param obj pointer to an object
 * @param dir the allow scroll directions. An element or OR-ed values of `lv_dir_t`
 */
lv_dir_t lv_obj_get_scroll_dir(const struct _lv_obj_t * obj);

/**
 * Get where to snap the children when scrolling ends horizontally
 * @param obj pointer to an object
 * @return the current snap align from `lv_snap_align_t`
 */
lv_snap_align_t lv_obj_get_snap_align_x(const struct _lv_obj_t * obj);

/**
 * Get where to snap the children when scrolling ends vertically
 * @param obj pointer to an object
 * @return the current snap align from `lv_snap_align_t`
 */
lv_snap_align_t lv_obj_get_snap_align_y(const struct _lv_obj_t * obj);

/**
 * Get current X scroll position.
 * @param obj pointer to an object
 * @return the current scroll position from the left edge.
 *   If the object is not scrolled return 0
 *   If scrolled return > 0
 *   If scrolled in (elastic scroll) return < 0
 */
lv_coord_t lv_obj_get_scroll_x(const struct _lv_obj_t * obj);

/**
 * Get current Y scroll position.
 * @param obj pointer to an object
 * @return the current scroll position from the top edge.
 *   If the object is not scrolled return 0
 *   If scrolled return > 0
 *   If scrolled inside return < 0
 */
lv_coord_t lv_obj_get_scroll_y(const struct _lv_obj_t * obj);

/**
 * Return the height of the area above the object.
 * That is the number of pixels the object can be scrolled down.
 * Normally positive but can be negative when scrolled inside.
 * @param obj pointer to an object
 * @return the scrollable area above the object in pixels
 */
lv_coord_t lv_obj_get_scroll_top(struct _lv_obj_t * obj);

/**
 * Return the height of the area below the object.
 * That is the number of pixels the object can be scrolled down.
 * Normally positive but can be negative when scrolled inside.
 * @param obj pointer to an object
 * @return the scrollable area below the object in pixels
 */
lv_coord_t lv_obj_get_scroll_bottom(struct _lv_obj_t * obj);

/**
 * Return the width of the area on the left the object.
 * That is the number of pixels the object can be scrolled down.
 * Normally positive but can be negative when scrolled inside.
 * @param obj pointer to an object
 * @return the scrollable area on the left the object in pixels
 */
lv_coord_t lv_obj_get_scroll_left(struct _lv_obj_t * obj);

/**
 * Return the width of the area on the right the object.
 * That is the number of pixels the object can be scrolled down.
 * Normally positive but can be negative when scrolled inside.
 * @param obj pointer to an object
 * @return the scrollable area on the right the object in pixels
 */
lv_coord_t lv_obj_get_scroll_right(struct _lv_obj_t * obj);

/**
 * Get the X and Y coordinates where the scrolling will end for this object if a scrolling animation is in progress.
 * In no scrolling animation give the current `x` or `y` scroll position.
 * @param obj pointer to an object
 * @param end poinr to point to store the result
 */
void lv_obj_get_scroll_end(struct _lv_obj_t  * obj, lv_point_t * end);

/*=====================
 * Other functions
 *====================*/

/**
 * The most basic function to scroll by a given amount of pixels
 * @param obj pointer to an object to scroll
 * @param x pixels to scroll horizontally
 * @param y pixels to scroll vertically
 * @note > 0 value means scroll right/bottom (show the right/bottom content)
 */
void _lv_obj_scroll_by_raw(struct _lv_obj_t * obj, lv_coord_t x, lv_coord_t y);

/**
 *
 * Scroll by a given amount of pixels
 * @param obj pointer to an object to scroll
 * @param x pixels to scroll horizontally
 * @param y pixels to scroll vertically
 * @param anim_en LV_ANIM_ON: scroll with animation; LV_ANIM_OFF: scroll immediately
 * @note > 0 value means scroll right/bottom (show the right/bottom content)
 */
void lv_obj_scroll_by(struct _lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en);

/**
 * Scroll to a given coordinate on an object
 * @param obj pointer to an object to scroll
 * @param x pixels to scroll horizontally
 * @param y pixels to scroll vertically
 * @param anim_en LV_ANIM_ON: scroll with animation; LV_ANIM_OFF: scroll immediately
 */
void lv_obj_scroll_to(struct _lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en);

/**
 * Scroll to a given X coordinate on an object
 * @param obj pointer to an object to scroll
 * @param x pixels to scroll horizontally
 * @param anim_en LV_ANIM_ON: scroll with animation; LV_ANIM_OFF: scroll immediately
 */
void lv_obj_scroll_to_x(struct _lv_obj_t * obj, lv_coord_t x, lv_anim_enable_t anim_en);

/**
 * Scroll to a given X coordinate on an object
 * @param obj pointer to an object to scroll
 * @param y pixels to scroll vertically
 * @param anim_en LV_ANIM_ON: scroll with animation; LV_ANIM_OFF: scroll immediately
 */
void lv_obj_scroll_to_y(struct _lv_obj_t * obj, lv_coord_t y, lv_anim_enable_t anim_en);

/**
 * Scroll to an object
 * @param obj pointer to an object to scroll into view
 * @param anim_en LV_ANIM_ON: scroll with animation; LV_ANIM_OFF: scroll immediately
 */
void lv_obj_scroll_to_view(struct _lv_obj_t * obj, lv_anim_enable_t anim_en);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJ_SCROLL_H*/
