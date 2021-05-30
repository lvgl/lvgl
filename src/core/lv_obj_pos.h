/**
 * @file lv_obj_pos.h
 *
 */

#ifndef LV_OBJ_POS_H
#define LV_OBJ_POS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_obj_t;

typedef void (*lv_layout_update_cb_t)(struct _lv_obj_t *, void * user_data);
typedef struct {
    lv_layout_update_cb_t cb;
    void * user_data;
}lv_layout_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Set the relative the position of an object (relative to the parent's top left corner)
 * @param obj       pointer to an object
 * @param x         new distance from the left side of the parent plus the parent's left padding
 * @param y         new distance from the top side of the parent  plus the parent's right padding
 */
void lv_obj_set_pos(struct _lv_obj_t * obj, lv_coord_t x, lv_coord_t y);

/**
 * Set the x coordinate of a object
 * @param obj       pointer to an object
 * @param x         new distance from the left side from the parent plus the parent's left padding
 */
void lv_obj_set_x(struct _lv_obj_t * obj, lv_coord_t x);

/**
 * Set the y coordinate of a object
 * @param obj       pointer to an object
 * @param y         new distance from the top of the parent  plus the parent's top padding
 */
void lv_obj_set_y(struct _lv_obj_t * obj, lv_coord_t y);

/**
 * Set the size of an object.
 * @param obj       pointer to an object
 * @param w         the new width
 * @param h         the new height
 * @note            possible values are:
 *                  pixel               simple set the size accordingly
 *                  LV_SIZE_CONTENT     set the size to involve all children in the given direction
 *                  LV_SIZE_PCT(x)     to set size in percentage of the parent's content area size (the size without paddings).
 *                                      x should be in [0..1000]% range
 */
void lv_obj_set_size(struct _lv_obj_t * obj, lv_coord_t w, lv_coord_t h);

/**
 * Recalculate the size of the object
 * @param obj       pointer to an object
 * @return          true: the size has been changed
 */
bool lv_obj_refr_size(struct _lv_obj_t * obj);

/**
 * Set the width of an object
 * @param obj       pointer to an object
 * @param w         the new width
 * @note            possible values are:
 *                  pixel               simple set the size accordingly
 *                  LV_SIZE_CONTENT     set the size to involve all children in the given direction
 *                  lv_pct(x)           to set size in percentage of the parent's content area size (the size without paddings).
 *                                      x should be in [0..1000]% range
 */
void lv_obj_set_width(struct _lv_obj_t * obj, lv_coord_t w);

/**
 * Set the height of an object
 * @param obj       pointer to an object
 * @param h         the new height
 * @note            possible values are:
 *                  pixel               simple set the size accordingly
 *                  LV_SIZE_CONTENT     set the size to involve all children in the given direction
 *                  lv_pct(x)           to set size in percentage of the parent's content area size (the size without paddings).
 *                                      x should be in [0..1000]% range
 */
void lv_obj_set_height(struct _lv_obj_t * obj, lv_coord_t h);

/**
 * Set the width reduced by the left and right padding and the border width.
 * @param obj       pointer to an object
 * @param w         the width without paddings in pixels
 */
void lv_obj_set_content_width(struct _lv_obj_t * obj, lv_coord_t w);

/**
 * Set the height reduced by the top and bottom padding and the border width.
 * @param obj       pointer to an object
 * @param h         the height without paddings in pixels
 */
void lv_obj_set_content_height(struct _lv_obj_t * obj, lv_coord_t h);

/**
 * Set a layout for an object
 * @param obj       pointer to an object
 * @param layout    pointer to a layout descriptor to set
 */
void lv_obj_set_layout(struct _lv_obj_t * obj, uint32_t layout);

/**
 * Test whether the and object is positioned by a layout or not
 * @param obj       pointer to an object to test
 * @return true:    positioned by a layout; false: not positioned by a layout
 */
bool lv_obj_is_layout_positioned(const struct _lv_obj_t * obj);

/**
 * Mark the object for layout update.
 * @param obj      pointer to an object whose children needs to be updated
 */
void lv_obj_mark_layout_as_dirty(struct _lv_obj_t * obj);

/**
 * Update the layout of an object.
 * @param obj      pointer to an object whose children needs to be updated
 */
void lv_obj_update_layout(const struct _lv_obj_t * obj);

/**
 * Regsiter a new layout
 * @param cb        the layout update callback
 * @param user_data custom data that will be passed to `cb`
 * @return          the ID of the new layout
 */
uint32_t lv_layout_register(lv_layout_update_cb_t cb, void * user_data);

/**
 * Change the alignment of an object.
 * @param obj       pointer to an object to align
 * @param align     type of alignment (see 'lv_align_t' enum) `LV_ALIGN_OUT_...` can't be used.
 */
void lv_obj_set_align(struct _lv_obj_t * obj, lv_align_t align);

/**
 * Change the alignment of an object and set new coordinates.
 * Equivalent to:
 * lv_obj_set_align(obj, align);
 * lv_obj_set_pos(obj, x_ofs, y_ofs);
 * @param obj       pointer to an object to align
 * @param align     type of alignment (see 'lv_align_t' enum) `LV_ALIGN_OUT_...` can't be used.
 * @param x_ofs     x coordinate offset after alignment
 * @param y_ofs     y coordinate offset after alignment
 */
void lv_obj_align(struct _lv_obj_t * obj, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs);

/**
 * Align an object to an other object.
 * @param obj       pointer to an object to align
 * @param base      pointer to an other object (if NULL `obj`s parent is used). 'obj' will be aligned to it.
 * @param align     type of alignment (see 'lv_align_t' enum)
 * @param x_ofs     x coordinate offset after alignment
 * @param y_ofs     y coordinate offset after alignment
 * @note            if the position or size of `base` changes `obj` needs to be aligned manually again
 */
void lv_obj_align_to(struct _lv_obj_t * obj, const struct _lv_obj_t * base, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs);

/**
 * Align an object to the center on its parent.
 * @param obj       pointer to an object to align
 * @note            if the parent size changes `obj` needs to be aligned manually again
 */
static inline void lv_obj_center(struct _lv_obj_t * obj)
{
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
}


/**
 * Copy the coordinates of an object to an area
 * @param obj       pointer to an object
 * @param coords    pointer to an area to store the coordinates
 */
void lv_obj_get_coords(const struct _lv_obj_t * obj, lv_area_t * coords);

/**
 * Get the x coordinate of object.
 * @param obj       pointer to an object
 * @return          distance of `obj` from the left side of its parent plus the parent's left padding
 * @note            Zero return value means the object is on the left padding of the parent, and not on the left edge.
 * @note            Scrolling of the parent doesn't change the returned value.
 * @note            The returned value is always the distance from the parent even if `obj` is positioned by a layout.
 */
lv_coord_t lv_obj_get_x(const struct _lv_obj_t * obj);

/**
 * Get the x2 coordinate of object.
 * @param obj       pointer to an object
 * @return          distance of `obj` from the right side of its parent plus the parent's right padding
 * @note            Zero return value means the object is on the right padding of the parent, and not on the right edge.
 * @note            Scrolling of the parent doesn't change the returned value.
 * @note            The returned value is always the distance from the parent even if `obj` is positioned by a layout.
 */
lv_coord_t lv_obj_get_x2(const struct _lv_obj_t * obj);

/**
 * Get the y coordinate of object.
 * @param obj       pointer to an object
 * @return          distance of `obj` from the top side of its parent plus the parent's top padding
 * @note            Zero return value means the object is on the top padding of the parent, and not on the top edge.
 * @note            Scrolling of the parent doesn't change the returned value.
 * @note            The returned value is always the distance from the parent even if `obj` is positioned by a layout.
 */
lv_coord_t lv_obj_get_y(const struct _lv_obj_t * obj);

/**
 * Get the y2 coordinate of object.
 * @param obj       pointer to an object
 * @return          distance of `obj` from the bottom side of its parent plus the parent's bottom padding
 * @note            Zero return value means the object is on the bottom padding of the parent, and not on the bottom edge.
 * @note            Scrolling of the parent doesn't change the returned value.
 * @note            The returned value is always the distance from the parent even if `obj` is positioned by a layout.
 */
lv_coord_t lv_obj_get_y2(const struct _lv_obj_t * obj);

/**
 * Get the width of an object
 * @param obj       pointer to an object
 * @return          the width in pixels
 */
lv_coord_t lv_obj_get_width(const struct _lv_obj_t * obj);

/**
 * Get the height of an object
 * @param obj       pointer to an object
 * @return          the height in pixels
 */
lv_coord_t lv_obj_get_height(const struct _lv_obj_t * obj);

/**
 * Get the width reduced by the left and right padding and the border width.
 * @param obj       pointer to an object
 * @return          the width which still fits into its parent without causing overflow (making the parent scrollable)
 */
lv_coord_t lv_obj_get_content_width(const struct _lv_obj_t * obj);

/**
 * Get the height reduced by the top an bottom padding and the border width.
 * @param obj       pointer to an object
 * @return          the height which still fits into the parent without causing overflow (making the parent scrollable)
 */
lv_coord_t lv_obj_get_content_height(const struct _lv_obj_t * obj);

/**
 * Get the area reduced by the paddings and the border width.
 * @param obj       pointer to an object
 * @param area      the area which still fits into the parent without causing overflow (making the parent scrollable)
 */
void lv_obj_get_content_coords(const struct _lv_obj_t * obj, lv_area_t * area);

/**
 * Get the width occupied by the "parts" of the widget. E.g. the width of all columns of a table.
 * @param obj       pointer to an objects
 * @return          the width of the virtually drawn content
 * @note            This size independent from the real size of the widget.
 *                  It just tells how large the internal ("virtual") content is.
 */
lv_coord_t lv_obj_get_self_width(const struct _lv_obj_t * obj);

/**
 * Get the height occupied by the "parts" of the widget. E.g. the height of all rows of a table.
 * @param obj       pointer to an objects
 * @return          the width of the virtually drawn content
 * @note            This size independent from the real size of the widget.
 *                  It just tells how large the internal ("virtual") content is.
 */
lv_coord_t lv_obj_get_self_height(const struct _lv_obj_t * obj);

/**
 * Handle if the size of the internal ("virtual") content of an object has changed.
 * @param obj       pointer to an object
 * @return          false: nothing happened; true: refresh happened
 */
bool lv_obj_refresh_self_size(struct _lv_obj_t * obj);

void lv_obj_refr_pos(struct _lv_obj_t * obj);

void lv_obj_move_to(struct _lv_obj_t * obj, lv_coord_t x, lv_coord_t y);


void lv_obj_move_children_by(struct _lv_obj_t * obj, lv_coord_t x_diff, lv_coord_t y_diff, bool ignore_floating);

/**
 * Mark an area of an object as invalid.
 * The area will be truncated to the object's area and marked for redraw.
 * @param obj       pointer to an object
 * @param           area the area to redraw
 */
void lv_obj_invalidate_area(const struct _lv_obj_t * obj, const lv_area_t * area);

/**
 * Mark the object as invalid to redrawn its area
 * @param obj       pointer to an object
 */
void lv_obj_invalidate(const struct _lv_obj_t * obj);

/**
 * Tell whether an area of an object is visible (even partially) now or not
 * @param obj       pointer to an object
 * @param area      the are to check. The visible part of the area will be written back here.
 * @return true     visible; false not visible (hidden, out of parent, on other screen, etc)
 */
bool lv_obj_area_is_visible(const struct _lv_obj_t * obj, lv_area_t * area);

/**
 * Tell whether an object is visible (even partially) now or not
 * @param obj       pointer to an object
 * @return      true: visible; false not visible (hidden, out of parent, on other screen, etc)
 */
bool lv_obj_is_visible(const struct _lv_obj_t * obj);

/**
 * Set the size of an extended clickable area
 * @param obj       pointer to an object
 * @param size      extended clickable area in all 4 directions [px]
 */
void lv_obj_set_ext_click_area(struct _lv_obj_t * obj, lv_coord_t size);

/**
 * Get the an area where to object can be clicked.
 * It's the object's normal area plus the extended click area.
 * @param obj       pointer to an object
 * @param area      store the result area here
 */
void lv_obj_get_click_area(const struct _lv_obj_t * obj, lv_area_t * area);

/**
 * Hit-test an object given a particular point in screen space.
 * @param obj       object to hit-test
 * @param point     screen-space point (absolute coordinate)
 * @return          true: if the object is considered under the point
 */
bool lv_obj_hit_test(struct _lv_obj_t * obj, const lv_point_t * point);

/**
 * Clamp a width between min and max width. If the min/max width is in percentage value use the ref_width
 * @param width         width to clamp
 * @param min_width     the minimal width
 * @param max_width     the maximal width
 * @param ref_width     the reference width used when min/max width is in percentage
 * @return              the clampled width
 */
lv_coord_t lv_clamp_width(lv_coord_t width, lv_coord_t min_width, lv_coord_t max_width, lv_coord_t ref_width);

/**
 * Clamp a height between min and max height. If the min/max height is in percentage value use the ref_height
 * @param height         height to clamp
 * @param min_height     the minimal height
 * @param max_height     the maximal height
 * @param ref_height     the reference height used when min/max height is in percentage
 * @return              the clampled height
 */
lv_coord_t lv_clamp_height(lv_coord_t height, lv_coord_t min_height, lv_coord_t max_height, lv_coord_t ref_height);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_POS_H*/
