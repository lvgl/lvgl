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
#include "../lv_misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_obj_t;

typedef void (*lv_layout_update_cb_t)(struct _lv_obj_t * cont, struct _lv_obj_t * item);

typedef union {
    uint32_t num;
    const void *  ptr;
}lv_layout_data_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_obj_update_layout(struct _lv_obj_t * cont, struct _lv_obj_t * item);

/**
 * Set relative the position of an object (relative to the parent)
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent plus the parent's left padding or a grid cell
 * @param y new distance from the top side of the parent  plus the parent's right padding or a grid cell
 * @note Zero value value means place the object on the left/top padding of the parent, and not on the left/top edge.
 * @note For grid cells: `LV_GRID_CELL_START/END/CENTER/STRETCH(pos, span)`
 *       For flex items: `LV_FLEX_POS/START/END/CENTER`
 *       For percentage size: `LV_SIZE_PCT(pct)`
 *       For auto size (to set size to children size): `LV_SIZE_AUTO`
 * @note to use grid placement the parent needs have a defined grid with `lv_obj_set_grid`
 */
void lv_obj_set_pos(struct _lv_obj_t * obj, lv_coord_t x, lv_coord_t y);

/**
 * Set the x coordinate of a object
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent plus the parent's left padding or a grid cell
 */
void lv_obj_set_x(struct _lv_obj_t * obj, lv_coord_t x);

/**
 * Set the y coordinate of a object
 * @param obj pointer to an object
 * @param y new distance from the top of the parent  plus the parent's top padding or a grid cell
 */
void lv_obj_set_y(struct _lv_obj_t * obj, lv_coord_t y);

/**
 * Set the size of an object.
 * @param obj pointer to an object
 * @param w new width in pixels or `LV_SIZE_AUTO` to set the size to involve all children
 * @param h new height  in pixels or `LV_SIZE_AUTO` to set the size to involve all children
 */
void lv_obj_set_size(struct _lv_obj_t * obj, lv_coord_t w, lv_coord_t h);

/**
 * Set the width of an object
 * @param obj pointer to an object
 * @param w new width in pixels or `LV_SIZE_AUTO` to set the size to involve all children
 */
void lv_obj_set_width(struct _lv_obj_t * obj, lv_coord_t w);

/**
 * Set the height of an object
 * @param obj pointer to an object
 * @param h new height in pixels or `LV_SIZE_AUTO` to set the size to involve all children
 */
void lv_obj_set_height(struct _lv_obj_t * obj, lv_coord_t h);

/**
 * Set the width reduced by the left and right padding.
 * @param obj pointer to an object
 * @param w the width without paddings in pixels
 */
void lv_obj_set_content_width(struct _lv_obj_t * obj, lv_coord_t w);

/**
 * Set the height reduced by the top and bottom padding.
 * @param obj pointer to an object
 * @param h the height without paddings in pixels
 */
void lv_obj_set_content_height(struct _lv_obj_t * obj, lv_coord_t h);

/**
 * Align an object to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_ofs x coordinate offset after alignment
 * @param y_ofs y coordinate offset after alignment
 */
void lv_obj_align(struct _lv_obj_t * obj, const struct _lv_obj_t * base, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs);

/**
 * Copy the coordinates of an object to an area
 * @param obj pointer to an object
 * @param coords_out pointer to an area to store the coordinates
 */
void lv_obj_get_coords(const struct _lv_obj_t * obj, lv_area_t * coords_out);

/**
 * Reduce area retried by `lv_obj_get_coords()` the get graphically usable area of an object.
 * (Without the size of the border or other extra graphical elements)
 * @param coords_out store the result area here
 */
void lv_obj_get_inner_coords(const struct _lv_obj_t * obj, lv_area_t * coords_out);

/**
 * Get the x coordinate of object.
 * @param obj pointer to an object
 * @return distance of 'obj' from the left side of its parent plus the parent's left padding
 * @note Zero return value means the object is on the left padding of the parent, and not on the left edge.
 * @note Scrolling of the parent doesn't change the returned value.
 * @note The returned value is always the distance from the parent even if the position is grid cell or other special value.
 */
lv_coord_t lv_obj_get_x(const struct _lv_obj_t * obj);

/**
 * Get the y coordinate of object.
 * @param obj pointer to an object
 * @return distance of 'obj' from the top side of its parent plus the parent's top padding
 * @note Zero return value means the object is on the top padding of the parent, and not on the top edge.
 * @note Scrolling of the parent doesn't change the returned value.
 * @note The returned value is always the distance from the parent even if the position is grid cell or other special value.
 */
lv_coord_t lv_obj_get_y(const struct _lv_obj_t * obj);

/**
 * Get the width of an object
 * @param obj pointer to an object
 * @return the width in pixels
 * @note The returned value is always the width in pixels even if the width is set to `LV_SIZE_AUTO` or other special value.
 */
lv_coord_t lv_obj_get_width(const struct _lv_obj_t * obj);

/**
 * Get the height of an object
 * @param obj pointer to an object
 * @return the height in pixels
 * @note The returned value is always the width in pixels even if the width is set to `LV_SIZE_AUTO` or other special value.
 */
lv_coord_t lv_obj_get_height(const struct _lv_obj_t * obj);

/**
 * Get that width reduced by the left and right padding.
 * @param obj pointer to an object
 * @return the width which still fits into the container without causing overflow
 */
lv_coord_t lv_obj_get_width_fit(const struct _lv_obj_t * obj);

/**
 * Get that height reduced by the top an bottom padding.
 * @param obj pointer to an object
 * @return the height which still fits into the container without causing overflow
 */
lv_coord_t lv_obj_get_height_fit(const struct _lv_obj_t * obj);

/**
 * Get the width of the virtual content of an object
 * @param obj pointer to an objects
 * @return the width of the virtually drawn content
 */
lv_coord_t _lv_obj_get_self_width(struct _lv_obj_t * obj);

/**
 * Get the height of the virtual content of an object
 * @param obj pointer to an objects
 * @return the width of the virtually drawn content
 */
lv_coord_t _lv_obj_get_self_height(struct _lv_obj_t * obj);

/**
 * Handle if the size of the internal (virtual) content of an object has changed.
 * @param obj pointer to an object
 * @return false: nothing happened; true: refresh happened
 */
bool _lv_obj_handle_self_size_chg(struct _lv_obj_t * obj);

/**
 * Calculate the "auto size". It's `auto_size = max(gird_size, children_size)`
 * @param obj pointer to an object
 * @param w_out store the width here. NULL to not calculate width
 * @param h_out store the height here. NULL to not calculate height
 */
void _lv_obj_calc_auto_size(struct _lv_obj_t * obj, lv_coord_t * w_out, lv_coord_t * h_out);

/**
 * Move an object to a given x and y coordinate.
 * It's the core function to move objects but user should use `lv_obj_set_pos/x/y/..` etc.
 * @param obj pointer to an object to move
 * @param x the new x coordinate in pixels
 * @param y the new y coordinate in pixels
 * @param notify_parent true: send `LV_SIGNAL_CHILD_CHG` to the parent if `obj` moved; false: do not notify the parent
 */
void _lv_obj_move_to(struct _lv_obj_t * obj, lv_coord_t x, lv_coord_t y, bool notify_parent);

/**
 * Reposition the children of an object. (Called recursively)
 * @param obj pointer to an object which children will be repositioned
 * @param x_diff x coordinate shift
 * @param y_diff y coordinate shift
 */
void _lv_obj_move_children_by(struct _lv_obj_t * obj, lv_coord_t x_diff, lv_coord_t y_diff);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJ_POS_H*/
