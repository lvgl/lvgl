/**
 * @file lv_flex.h
 *
 */

#ifndef LV_FLEX_H
#define LV_FLEX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_grid.h"

/*********************
 *      DEFINES
 *********************/

/** Can be set as width or height (on main axis) to grow the object in order fill the free space*/
#define LV_FLEX_GROW(grow)    (_LV_COORD_FELX(grow))

#define _LV_FLEX_GET_GROW(v) (LV_COORD_IS_FLEX(v) ? LV_COORD_GET_FLEX(v) : 0)

#define LV_FLEX_WRAP       (1 << 2)
#define LV_FLEX_REVERSE    (1 << 3)

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_FLEX_START,
    LV_FLEX_END,
    LV_FLEX_CENTER,
    LV_FLEX_SPACE_EVENLY,
    LV_FLEX_SPACE_AROUND,
    LV_FLEX_SPACE_BETWEEN,
}lv_flex_place_t;

typedef enum {
    LV_FLEX_DIR_NONE,
    LV_FLEX_DIR_ROW,
    LV_FLEX_DIR_COLUMN,
}lv_flex_dir_t;

typedef struct {
    lv_coord_t gap;
    uint8_t dir        :2;
    uint8_t wrap       :1;
    uint8_t rev        :1;
    uint8_t place      :3;
}lv_flex_cont_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_obj_set_flex_dir(lv_obj_t * obj, lv_flex_dir_t flex_dir);

/**
 * Set how to place the tracks below/next to each other.
 * For ROW direction it means how to place the rows vertically.
 * For COLUMN direction it means how to place the column horizontally.
 * @param obj point to a flex container
 * @param place the placement type. Can be any element of `lv_flex_place_t`.
 * @note if the base direction is RTL and the direction is ROW, LV_FLEX_START means the right side
 */
void lv_obj_set_flex_track_place(lv_obj_t * obj, lv_flex_place_t place);

/**
 * Set a gap in the main direction.
 * For ROW direction it means adding gap horizontally between the items.
 * For COLUMN direction it means adding gap vertically between the items.
 * @param obj pointer to an object (flex container)
 * @param gap the gap in pixels
 * @note By default the objects are packed tightly after each other
 */
void lv_obj_set_flex_gap(lv_obj_t * obj, lv_coord_t gap);

/**
 * Make an object flex item, i.e. allow setting it's coordinate according to the parent's flex settings.
 * @param obj pointer to an object
 */
void lv_obj_set_flex_item(lv_obj_t * obj, bool en);

/**
 * Set how the place the item in it's track in the cross direction.
 * It has a visible effect only if the objects in the same track has different size in the cross direction.
 * For ROW direction it means how to place the objects vertically in their row.
 * For COLUMN direction it means how to place the objects horizontally in their column.
 * @param obj pointer to a flex item
 * @param place:
 *   - `LV_FLEX_START` top/left (in case of RTL base direction right)
 *   - `LV_FLEX_CENTER` center
 *   - `LV_FLEX_END` bottom/right (in case of RTL base direction left)
 */
void lv_obj_set_flex_item_place(lv_obj_t * obj, lv_flex_place_t place);

lv_flex_dir_t lv_obj_get_flex_dir(const lv_obj_t * obj);
lv_flex_place_t lv_obj_get_flex_place(const lv_obj_t * obj);
bool lv_obj_get_flex_wrap(const lv_obj_t * obj);
bool lv_obj_get_flex_reverse(const lv_obj_t * obj);
lv_coord_t lv_obj_get_flex_gap(const lv_obj_t * obj);
void _lv_flex_refresh(lv_obj_t * cont);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_FLEX_H*/
