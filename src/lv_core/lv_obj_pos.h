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

/* Can't include lv_obj.h because it includes this header file */
struct _lv_obj_t;
typedef struct _lv_obj_t lv_obj_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Set relative the position of an object (relative to the parent)
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent
 * @param y new distance from the top of the parent
 */
void lv_obj_set_pos(lv_obj_t * obj, lv_coord_t x, lv_coord_t y);

/**
 * Set the x coordinate of a object
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent
 */
void lv_obj_set_x(lv_obj_t * obj, lv_coord_t x);

/**
 * Set the y coordinate of a object
 * @param obj pointer to an object
 * @param y new distance from the top of the parent
 */
void lv_obj_set_y(lv_obj_t * obj, lv_coord_t y);


void _lv_obj_calc_auto_size(lv_obj_t * obj, lv_coord_t * w, lv_coord_t * h);

/**
 * Set the size of an object
 * @param obj pointer to an object
 * @param w new width
 * @param h new height
 */
void lv_obj_set_size(lv_obj_t * obj, lv_coord_t w, lv_coord_t h);

/**
 * Set the width of an object
 * @param obj pointer to an object
 * @param w new width
 */
void lv_obj_set_width(lv_obj_t * obj, lv_coord_t w);

/**
 * Set the height of an object
 * @param obj pointer to an object
 * @param h new height
 */
void lv_obj_set_height(lv_obj_t * obj, lv_coord_t h);

/**
 * Set the width reduced by the left and right padding.
 * @param obj pointer to an object
 * @param w the width without paddings
 */
void lv_obj_set_content_width(lv_obj_t * obj, lv_coord_t w);

/**
 * Set the height reduced by the top and bottom padding.
 * @param obj pointer to an object
 * @param h the height without paddings
 */
void lv_obj_set_content_height(lv_obj_t * obj, lv_coord_t h);

/**
 * Set the width of an object by taking the left and right margin into account.
 * The object width will be `obj_w = w - margin_left - margin_right`
 * @param obj pointer to an object
 * @param w new height including margins
 */
void lv_obj_set_width_margin(lv_obj_t * obj, lv_coord_t w);

/**
 * Set the height of an object by taking the top and bottom margin into account.
 * The object height will be `obj_h = h - margin_top - margin_bottom`
 * @param obj pointer to an object
 * @param h new height including margins
 */
void lv_obj_set_height_margin(lv_obj_t * obj, lv_coord_t h);

/**
 * Align an object to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_ofs x coordinate offset after alignment
 * @param y_ofs y coordinate offset after alignment
 */
void lv_obj_align(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs);


/**
 * Copy the coordinates of an object to an area
 * @param obj pointer to an object
 * @param cords_p pointer to an area to store the coordinates
 */
void lv_obj_get_coords(const lv_obj_t * obj, lv_area_t * cords_p);

/**
 * Reduce area retried by `lv_obj_get_coords()` the get graphically usable area of an object.
 * (Without the size of the border or other extra graphical elements)
 * @param coords_p store the result area here
 */
void lv_obj_get_inner_coords(const lv_obj_t * obj, lv_area_t * coords_p);

/**
 * Get the x coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the left side of its parent
 */
lv_coord_t lv_obj_get_x(const lv_obj_t * obj);

/**
 * Get the y coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the top of its parent
 */
lv_coord_t lv_obj_get_y(const lv_obj_t * obj);

/**
 * Get the width of an object
 * @param obj pointer to an object
 * @return the width
 */
lv_coord_t lv_obj_get_width(const lv_obj_t * obj);

/**
 * Get the height of an object
 * @param obj pointer to an object
 * @return the height
 */
lv_coord_t lv_obj_get_height(const lv_obj_t * obj);

/**
 * Get that width reduced by the left and right padding.
 * @param obj pointer to an object
 * @return the width which still fits into the container
 */
lv_coord_t lv_obj_get_width_fit(const lv_obj_t * obj);

/**
 * Get that height reduced by the top an bottom padding.
 * @param obj pointer to an object
 * @return the height which still fits into the container
 */
lv_coord_t lv_obj_get_height_fit(const lv_obj_t * obj);

/**
 * Get the height of an object by taking the top and bottom margin into account.
 * The returned height will be `obj_h + margin_top + margin_bottom`
 * @param obj pointer to an object
 * @return the height including thee margins
 */
lv_coord_t lv_obj_get_height_margin(lv_obj_t * obj);

/**
 * Get the width of an object by taking the left and right margin into account.
 * The returned width will be `obj_w + margin_left + margin_right`
 * @param obj pointer to an object
 * @return the height including thee margins
 */
lv_coord_t lv_obj_get_width_margin(lv_obj_t * obj);


/**
 * Check if a given screen-space point is on an object's coordinates.
 *
 * This method is intended to be used mainly by advanced hit testing algorithms to check
 * whether the point is even within the object (as an optimization).
 * @param obj object to check
 * @param point screen-space point
 */
bool lv_obj_is_point_on_coords(lv_obj_t * obj, const lv_point_t * point);

/**
 * Divide the width of the object and get the width of a given number of columns.
 * Take paddings into account.
 * @param obj pointer to an object
 * @param div indicates how many columns are assumed.
 * If 1 the width will be set the the parent's width
 * If 2 only half parent width - inner padding of the parent
 * If 3 only third parent width - 2 * inner padding of the parent
 * @param span how many columns are combined
 * @return the width according to the given parameters
 */
lv_coord_t lv_obj_get_width_grid(lv_obj_t * obj, uint8_t div, uint8_t span);

/**
 * Divide the height of the object and get the width of a given number of columns.
 * Take paddings into account.
 * @param obj pointer to an object
 * @param div indicates how many rows are assumed.
 * If 1 the height will be set the the parent's height
 * If 2 only half parent height - inner padding of the parent
 * If 3 only third parent height - 2 * inner padding of the parent
 * @param span how many rows are combined
 * @return the height according to the given parameters
 */
lv_coord_t lv_obj_get_height_grid(lv_obj_t * obj, uint8_t div, uint8_t span);

void _lv_obj_move_to(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, bool notify_parent);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJ_POS_H*/
