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
#include "../../../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/
#define _LV_FLEX_GET_GROW(v) (LV_COORD_IS_LAYOUT(v) ? _LV_COORD_PLAIN(v) : 0)

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
    LV_FLEX_PLACE_SPACE_EVENLY,
    LV_FLEX_PLACE_SPACE_AROUND,
    LV_FLEX_PLACE_SPACE_BETWEEN,
}lv_flex_place_t;

typedef enum {
    LV_FLEX_FLOW_ROW                 = 0x01,
    LV_FLEX_FLOW_COLUMN              = 0x02,
    LV_FLEX_FLOW_ROW_WRAP            = LV_FLEX_FLOW_ROW | _LV_FLEX_WRAP,
    LV_FLEX_FLOW_ROW_REVERSE         = LV_FLEX_FLOW_ROW | _LV_FLEX_REVERSE,
    LV_FLEX_FLOW_ROW_WRAP_REVERSE    = LV_FLEX_FLOW_ROW | _LV_FLEX_WRAP | _LV_FLEX_REVERSE,
    LV_FLEX_FLOW_COLUMN_WRAP         = LV_FLEX_FLOW_COLUMN | _LV_FLEX_WRAP,
    LV_FLEX_FLOW_COLUMN_REVERSE      = LV_FLEX_FLOW_COLUMN | _LV_FLEX_REVERSE,
    LV_FLEX_FLOW_COLUMN_WRAP_REVERSE = LV_FLEX_FLOW_COLUMN | _LV_FLEX_WRAP | _LV_FLEX_REVERSE,
}lv_flex_flow_t;

typedef struct {
    lv_layout_dsc_t base;
    uint32_t dir          :2;
    uint32_t wrap         :1;
    uint32_t rev          :1;
    uint32_t item_main_place   :3;
    uint32_t track_cross_place  :3;
    uint32_t item_cross_place   :3;
}lv_flex_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a felx layout the default values
 * @param flex: pointer to a flex layout descriptor
 */
void lv_flex_init(lv_flex_t * flex);

/**
 * Set hot the item should flow
 * @param flex: pointer to a flex layout descriptor
 * @param flow: an element of `lv_flex_flow_t`.
 */
void lv_flex_set_flow(lv_flex_t * flex, lv_flex_flow_t flow);

/**
 * Set how to place (where to align) the items an tracks
 * @param flex pointer: to a flex layout descriptor
 * @param item_main_place: where to place the items on main axis (in their track). Any value of `lv_flex_place_t`.
 * @param item_cross_place: where to place the item in their track on the cross axis. `LV_FLEX_PLACE_START/END/CENTER`
 * @param track_place: how to place the tracks in the cross direction. Any value of `lv_flex_place_t`.
 */
void lv_flex_set_place(lv_flex_t * flex, lv_flex_place_t item_main_place, lv_flex_place_t item_cross_place, lv_flex_place_t track_cross_place);

/**
 * Sets the width or height (on main axis) to grow the object in order fill the free space
 * @param obj: pointer to an object. The parent must have flex layout else nothing will happen.
 * @param grow: a value to set how much free space to take proportionally to other growing items.
 */
void lv_obj_set_flex_grow(struct _lv_obj_t * obj, uint8_t grow);

/**********************
 *   GLOBAL VARIABLES
 **********************/

/**
 * Predefines flex layouts
 */
extern const lv_flex_t lv_flex_inline;          /**< Just put the items next to each other with wrap*/
extern const lv_flex_t lv_flex_center_row;      /**< Center in a row with wrap*/
extern const lv_flex_t lv_flex_center_column;   /**< Center in a column with wrap*/
extern const lv_flex_t lv_flex_stacked;         /**< Stack the items vertically without wrapping*/
extern const lv_flex_t lv_flex_queue;           /**< Put the items next to each other without wrap*/
extern const lv_flex_t lv_flex_even;            /**< Place the items evenly in row with wrapping and vertical centering*/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_FLEX_H*/
