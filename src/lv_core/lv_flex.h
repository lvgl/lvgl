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
#include "../lv_misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/** Can be set as width or height (on main axis) to grow the object in order fill the free space*/
#define LV_FLEX_GROW(grow)    (_LV_COORD_FELX(grow))

#define _LV_FLEX_GET_GROW(v) (LV_COORD_IS_FLEX(v) ? LV_COORD_GET_FLEX(v) : 0)

#define _LV_FLEX_WRAP       (1 << 2)
#define _LV_FLEX_REVERSE    (1 << 3)

/**********************
 *      TYPEDEFS
 **********************/

/* Can't include lv_obj.h because it includes this header file */
struct _lv_obj_t;

typedef enum {
    LV_FLEX_PLACE_NONE,
    LV_FLEX_PLACE_START,
    LV_FLEX_PLACE_END,
    LV_FLEX_PLACE_CENTER,
    LV_FLEX_PLACE_STRETCH,
    LV_FLEX_PLACE_SPACE_EVENLY,
    LV_FLEX_PLACE_SPACE_AROUND,
    LV_FLEX_PLACE_SPACE_BETWEEN,
}lv_flex_place_t;

typedef enum {
    LV_FLEX_DIR_NONE,
    LV_FLEX_DIR_ROW                 = 0x01,
    LV_FLEX_DIR_COLUMN              = 0x02,
    LV_FLEX_DIR_ROW_WRAP            = LV_FLEX_DIR_ROW | _LV_FLEX_WRAP,
    LV_FLEX_DIR_ROW_REVERSE         = LV_FLEX_DIR_ROW | _LV_FLEX_REVERSE,
    LV_FLEX_DIR_ROW_WRAP_REVERSE    = LV_FLEX_DIR_ROW | _LV_FLEX_WRAP | _LV_FLEX_REVERSE,
    LV_FLEX_DIR_COLUMN_WRAP         = LV_FLEX_DIR_COLUMN | _LV_FLEX_WRAP,
    LV_FLEX_DIR_COLUMN_REVERSE      = LV_FLEX_DIR_COLUMN | _LV_FLEX_REVERSE,
    LV_FLEX_DIR_COLUMN_WRAP_REVERSE = LV_FLEX_DIR_COLUMN | _LV_FLEX_WRAP | _LV_FLEX_REVERSE,
}lv_flex_dir_t;

typedef struct {
    lv_coord_t gap;
    uint8_t dir        :2;
    uint8_t wrap       :1;
    uint8_t rev        :1;
    uint8_t main_place      :3;
    uint8_t cross_place      :3;
}lv_flex_cont_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*=====================
 * Setter functions
 *====================*/

/**
 * Makes an object a flex container by setting the direction of the layout.
 * The children explicitly needs to be made flex items with
 * `lv_obj_set_flex_item()` or `lv_obj_set_flex_item_place()`
 * @param obj pointer to an object
 * @param flex_dir the flex direction, an element of `lv_flex_dir_t`
 */
void lv_obj_set_flex_dir(struct _lv_obj_t * obj, lv_flex_dir_t flex_dir);

/**
 * Set how to place the items and the tracks
 * @param obj pointer to a flex container
 * @param main_place tells how to distribute the free space among the items in the same track
 * @param cross_place tells how to distribute the free space among the tracks
 * @note if the base direction is RTL and the direction is ROW, LV_FLEX_START means the right side
 */
void lv_obj_set_flex_place(struct _lv_obj_t * obj, lv_flex_place_t main_place, lv_flex_place_t cross_place);

/**
 * Set a minimal gap between items in the main direction.
 * @param obj pointer to a flex container
 * @param gap the gap in pixels
 */
void lv_obj_set_flex_gap(struct _lv_obj_t * obj, lv_coord_t gap);

/**
 * Make an object flex item, i.e. allow setting it's coordinate according to the parent's flex settings.
 * @param obj pointer to an object
 */
void lv_obj_set_flex_item(struct _lv_obj_t * obj, bool en);

/**
 * Set how the place the item in it's track in the cross direction.
 * It has a visible effect only if the objects in the same track has different size in the cross direction.
 * For ROW direction it means how to place the objects vertically in their row.
 * For COLUMN direction it means how to place the objects horizontally in their column.
 * @param obj pointer to a flex item
 * @param place:
 *   - `LV_FLEX_PLACE_START` top/left (in case of RTL base direction right)
 *   - `LV_FLEX_PLACE_CENTER` center
 *   - `LV_FLEX_PLACE_END` bottom/right (in case of RTL base direction left)
 */
void lv_obj_set_flex_item_place(struct _lv_obj_t * obj, lv_flex_place_t place);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the flex direction of an object
 * @param obj pointer to an object
 * @return the flex direction of `obj`
 */
lv_flex_dir_t lv_obj_get_flex_dir(const struct _lv_obj_t * obj);

/**
 * Get the item placement of a flex container
 * @param obj pointer to an object
 * @return the item placement
 */
lv_flex_place_t lv_obj_get_flex_item_place(const struct _lv_obj_t * obj);

/**
 * Get the track placement of a flex container
 * @param obj pointer to an object
 * @return the track placement
 */
lv_flex_place_t lv_obj_get_flex_track_place(const struct _lv_obj_t * obj);

/**
 * Get the minimal gap between flex item in the main direction
 * @param obj pointer to a flex container
 * @return the gap
 */
lv_coord_t lv_obj_get_flex_gap(const struct _lv_obj_t * obj);
/**
 * Get how the flex item is placed in its track in the cross direction.
 * For ROW direction it means how the item is placed vertically in its row.
 * For COLUMN direction it means how the item is placed horizontally in its column.
 * @param obj pointer to a flex item
 * @return `LV_FLEX_PLACE_NONE/START/CENTER/END`
 */
lv_flex_place_t lv_obj_get_flex_self_place(struct _lv_obj_t * obj);
/**
 * Rearrange the flex items of a flex container
 * @param cont pointer to a flex container object
 */
void _lv_flex_refresh(struct _lv_obj_t * cont);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_FLEX_H*/
