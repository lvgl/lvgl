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
#include "../misc/lv_area.h"
#include "../misc/lv_anim.h"
#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Can't include lv_obj.h because it includes this header file*/

/** Scrollbar modes: shows when should the scrollbars be visible*/
typedef enum {
    LV_SCROLLBAR_MODE_OFF,      /**< Never show scrollbars*/
    LV_SCROLLBAR_MODE_ON,       /**< Always show scrollbars*/
    LV_SCROLLBAR_MODE_ACTIVE,   /**< Show scroll bars when Widget is being scrolled*/
    LV_SCROLLBAR_MODE_AUTO,     /**< Show scroll bars when the content is large enough to be scrolled*/
} lv_scrollbar_mode_t;

/** Scroll span align options. Tells where to align the snappable children when scroll stops.*/
typedef enum {
    LV_SCROLL_SNAP_NONE,    /**< Do not align, leave where it is*/
    LV_SCROLL_SNAP_START,   /**< Align to the left/top*/
    LV_SCROLL_SNAP_END,     /**< Align to the right/bottom*/
    LV_SCROLL_SNAP_CENTER   /**< Align to the center*/
} lv_scroll_snap_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*=====================
 * Setter functions
 *====================*/

/**
 * Set how the scrollbars should behave.
 * @param obj       pointer to Widget
 * @param mode      LV_SCROLL_MODE_ON/OFF/AUTO/ACTIVE
 */
void lv_obj_set_scrollbar_mode(lv_obj_t * obj, lv_scrollbar_mode_t mode);

/**
 * Set direction Widget can be scrolled
 * @param obj       pointer to Widget
 * @param dir       one or more bit-wise OR-ed values of `lv_dir_t` enumeration
 */
void lv_obj_set_scroll_dir(lv_obj_t * obj, lv_dir_t dir);

/**
 * Set where to snap the children when scrolling ends horizontally
 * @param obj       pointer to Widget
 * @param align     value from `lv_scroll_snap_t` enumeration
 */
void lv_obj_set_scroll_snap_x(lv_obj_t * obj, lv_scroll_snap_t align);

/**
 * Set where to snap the children when scrolling ends vertically
 * @param obj       pointer to Widget
 * @param align     value from `lv_scroll_snap_t` enumeration
 */
void lv_obj_set_scroll_snap_y(lv_obj_t * obj, lv_scroll_snap_t align);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current scroll mode (when to hide the scrollbars)
 * @param obj       pointer to Widget
 * @return          the current scroll mode from `lv_scrollbar_mode_t`
 */
lv_scrollbar_mode_t lv_obj_get_scrollbar_mode(const lv_obj_t * obj);

/**
 * Get directions Widget can be scrolled (set with `lv_obj_set_scroll_dir()`)
 * @param obj       pointer to Widget
 * @return          current scroll direction bit(s)
 */
lv_dir_t lv_obj_get_scroll_dir(const lv_obj_t * obj);

/**
 * Get where to snap child Widgets when horizontal scrolling ends.
 * @param obj       pointer to Widget
 * @return          current snap value from `lv_scroll_snap_t`
 */
lv_scroll_snap_t lv_obj_get_scroll_snap_x(const lv_obj_t * obj);

/**
 * Get where to snap child Widgets when vertical scrolling ends.
 * @param  obj      pointer to Widget
 * @return          current snap value from `lv_scroll_snap_t`
 */
lv_scroll_snap_t lv_obj_get_scroll_snap_y(const lv_obj_t * obj);

/**
 * Get current X scroll position.  Identical to `lv_obj_get_scroll_left()`.
 * @param obj       pointer to scrollable container Widget
 * @return          current scroll position from left edge
 *                      - If Widget is not scrolled return 0.
 *                      - If scrolled return > 0.
 *                      - If scrolled inside (elastic scroll) return < 0.
 */
int32_t lv_obj_get_scroll_x(const lv_obj_t * obj);

/**
 * Get current Y scroll position.  Identical to `lv_obj_get_scroll_top()`.
 * @param obj       pointer to scrollable container Widget
 * @return          current scroll position from top edge
 *                      - If Widget is not scrolled return 0.
 *                      - If scrolled return > 0.
 *                      - If scrolled inside (elastic scroll) return < 0.
 */
int32_t lv_obj_get_scroll_y(const lv_obj_t * obj);

/**
 * Number of pixels a scrollable container Widget can be scrolled down
 * before its top edge appears.  When LV_OBJ_FLAG_SCROLL_ELASTIC flag
 * is set in Widget, this value can go negative while Widget is being
 * dragged below its normal top-edge boundary.
 * @param obj       pointer to scrollable container Widget
 * @return          pixels Widget can be scrolled down before its top edge appears
 */
int32_t lv_obj_get_scroll_top(const lv_obj_t * obj);

/**
 * Number of pixels a scrollable container Widget can be scrolled up
 * before its bottom edge appears.  When LV_OBJ_FLAG_SCROLL_ELASTIC flag
 * is set in Widget, this value can go negative while Widget is being
 * dragged above its normal bottom-edge boundary.
 * @param obj       pointer to scrollable container Widget
 * @return          pixels Widget can be scrolled up before its bottom edge appears
 */
int32_t lv_obj_get_scroll_bottom(const lv_obj_t * obj);

/**
 * Number of pixels a scrollable container Widget can be scrolled right
 * before its left edge appears.  When LV_OBJ_FLAG_SCROLL_ELASTIC flag
 * is set in Widget, this value can go negative while Widget is being
 * dragged farther right than its normal left-edge boundary.
 * @param obj       pointer to scrollable container Widget
 * @return          pixels Widget can be scrolled right before its left edge appears
 */
int32_t lv_obj_get_scroll_left(const lv_obj_t * obj);

/**
 * Number of pixels a scrollable container Widget can be scrolled left
 * before its right edge appears.  When LV_OBJ_FLAG_SCROLL_ELASTIC flag
 * is set in Widget, this value can go negative while Widget is being
 * dragged farther left than its normal right-edge boundary.
 * @param obj       pointer to scrollable container Widget
 * @return          pixels Widget can be scrolled left before its right edge appears
 */
int32_t lv_obj_get_scroll_right(const lv_obj_t * obj);

/**
 * Get the X and Y coordinates where the scrolling will end for Widget if a scrolling animation is in progress.
 * If no scrolling animation, give the current `x` or `y` scroll position.
 * @param obj       pointer to scrollable Widget
 * @param end       pointer to `lv_point_t` object in which to store result
 */
void lv_obj_get_scroll_end(lv_obj_t * obj, lv_point_t * end);

/*=====================
 * Other functions
 *====================*/

/**
 * Scroll by given amount of pixels.
 * @param obj       pointer to scrollable Widget to scroll
 * @param dx        pixels to scroll horizontally
 * @param dy        pixels to scroll vertically
 * @param anim_en   LV_ANIM_ON: scroll with animation; LV_ANIM_OFF: scroll immediately
 * @note            > 0 value means scroll right/bottom (show the more content on the right/bottom)
 * @note            e.g. dy = -20 means scroll down 20 px
 */
void lv_obj_scroll_by(lv_obj_t * obj, int32_t dx, int32_t dy, lv_anim_enable_t anim_en);

/**
 * Scroll by given amount of pixels.
 * `dx` and `dy` will be limited internally to allow scrolling only on the content area.
 * @param obj       pointer to scrollable Widget to scroll
 * @param dx        pixels to scroll horizontally
 * @param dy        pixels to scroll vertically
 * @param anim_en   LV_ANIM_ON: scroll with animation; LV_ANIM_OFF: scroll immediately
 * @note            e.g. dy = -20 means scroll down 20 px
 */
void lv_obj_scroll_by_bounded(lv_obj_t * obj, int32_t dx, int32_t dy, lv_anim_enable_t anim_en);

/**
 * Scroll to given coordinate on Widget.
 * `x` and `y` will be limited internally to allow scrolling only on the content area.
 * @param obj       pointer to scrollable Widget to scroll
 * @param x         pixels to scroll horizontally
 * @param y         pixels to scroll vertically
 * @param anim_en   LV_ANIM_ON: scroll with animation; LV_ANIM_OFF: scroll immediately
 */
void lv_obj_scroll_to(lv_obj_t * obj, int32_t x, int32_t y, lv_anim_enable_t anim_en);

/**
 * Scroll to X coordinate on Widget.
 * `x` will be limited internally to allow scrolling only on the content area.
 * @param obj       pointer to scrollable Widget to scroll
 * @param x         pixels to scroll horizontally
 * @param anim_en   LV_ANIM_ON: scroll with animation; LV_ANIM_OFF: scroll immediately
 */
void lv_obj_scroll_to_x(lv_obj_t * obj, int32_t x, lv_anim_enable_t anim_en);

/**
 * Scroll to Y coordinate on Widget.
 * `y` will be limited internally to allow scrolling only on the content area.
 * @param obj       pointer to scrollable Widget to scroll
 * @param y         pixels to scroll vertically
 * @param anim_en   LV_ANIM_ON: scroll with animation; LV_ANIM_OFF: scroll immediately
 */
void lv_obj_scroll_to_y(lv_obj_t * obj, int32_t y, lv_anim_enable_t anim_en);

/**
 * Scroll `obj`'s parent Widget until `obj` becomes visible.
 * @param obj       pointer to Widget to scroll into view
 * @param anim_en   LV_ANIM_ON: scroll with animation; LV_ANIM_OFF: scroll immediately
 */
void lv_obj_scroll_to_view(lv_obj_t * obj, lv_anim_enable_t anim_en);

/**
 * Scroll `obj`'s parent Widgets recursively until `obj` becomes visible.
 * Widget will be scrolled into view even it has nested scrollable parents.
 * @param obj       pointer to Widget to scroll into view
 * @param anim_en   LV_ANIM_ON: scroll with animation; LV_ANIM_OFF: scroll immediately
 */
void lv_obj_scroll_to_view_recursive(lv_obj_t * obj, lv_anim_enable_t anim_en);

/**
 * Tell whether Widget is being scrolled or not at this moment
 * @param obj   pointer to Widget
 * @return      true: `obj` is being scrolled
 */
bool lv_obj_is_scrolling(const lv_obj_t * obj);

/**
 * Stop scrolling the current object
 *
 * @param obj The object being scrolled
 */
void lv_obj_stop_scroll_anim(const lv_obj_t * obj);

/**
 * Check children of `obj` and scroll `obj` to fulfill scroll_snap settings.
 * @param obj       Widget whose children need to be checked and snapped
 * @param anim_en   LV_ANIM_ON/OFF
 */
void lv_obj_update_snap(lv_obj_t * obj, lv_anim_enable_t anim_en);

/**
 * Get the area of the scrollbars
 * @param obj   pointer to Widget
 * @param hor   pointer to store the area of the horizontal scrollbar
 * @param ver   pointer to store the area of the vertical  scrollbar
 */
void lv_obj_get_scrollbar_area(lv_obj_t * obj, lv_area_t * hor, lv_area_t * ver);

/**
 * Invalidate the area of the scrollbars
 * @param obj       pointer to Widget
 */
void lv_obj_scrollbar_invalidate(lv_obj_t * obj);

/**
 * Checks if the content is scrolled "in" and adjusts it to a normal position.
 * @param obj       pointer to Widget
 * @param anim_en   LV_ANIM_ON/OFF
 */
void lv_obj_readjust_scroll(lv_obj_t * obj, lv_anim_enable_t anim_en);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_SCROLL_H*/
