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
#include "../../lv_conf_internal.h"
#include "../../misc/lv_area.h"

#if LV_USE_FLEX

/*********************
 *      DEFINES
 *********************/

#define _LV_FLEX_COLUMN        (1 << 0)
#define _LV_FLEX_WRAP       (1 << 2)
#define _LV_FLEX_REVERSE    (1 << 3)

/**********************
 *      TYPEDEFS
 **********************/

/*Can't include lv_obj.h because it includes this header file*/

typedef enum {
    LV_FLEX_ALIGN_START,
    LV_FLEX_ALIGN_END,
    LV_FLEX_ALIGN_CENTER,
    LV_FLEX_ALIGN_SPACE_EVENLY,
    LV_FLEX_ALIGN_SPACE_AROUND,
    LV_FLEX_ALIGN_SPACE_BETWEEN,
} lv_flex_align_t;

typedef enum {
    LV_FLEX_FLOW_ROW                 = 0x00,
    LV_FLEX_FLOW_COLUMN              = _LV_FLEX_COLUMN,
    LV_FLEX_FLOW_ROW_WRAP            = LV_FLEX_FLOW_ROW | _LV_FLEX_WRAP,
    LV_FLEX_FLOW_ROW_REVERSE         = LV_FLEX_FLOW_ROW | _LV_FLEX_REVERSE,
    LV_FLEX_FLOW_ROW_WRAP_REVERSE    = LV_FLEX_FLOW_ROW | _LV_FLEX_WRAP | _LV_FLEX_REVERSE,
    LV_FLEX_FLOW_COLUMN_WRAP         = LV_FLEX_FLOW_COLUMN | _LV_FLEX_WRAP,
    LV_FLEX_FLOW_COLUMN_REVERSE      = LV_FLEX_FLOW_COLUMN | _LV_FLEX_REVERSE,
    LV_FLEX_FLOW_COLUMN_WRAP_REVERSE = LV_FLEX_FLOW_COLUMN | _LV_FLEX_WRAP | _LV_FLEX_REVERSE,
} lv_flex_flow_t;

/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a flex layout the default values
 * @param flex pointer to a flex layout descriptor
 */
void lv_flex_init(void);

/**
 * Set how the item should flow
 * @param obj pointer to an object. The parent must have flex layout else nothing will happen.
 * @param flow an element of `lv_flex_flow_t`.
 */
void lv_obj_set_flex_flow(lv_obj_t * obj, lv_flex_flow_t flow);

/**
 * Set how to place (where to align) the items and tracks
 * @param obj pointer to an object. The parent must have flex layout else nothing will happen.
 * @param main_place where to place the items on main axis (in their track). Any value of `lv_flex_align_t`.
 * @param cross_place where to place the item in their track on the cross axis. `LV_FLEX_ALIGN_START/END/CENTER`
 * @param track_cross_place where to place the tracks in the cross direction. Any value of `lv_flex_align_t`.
 */
void lv_obj_set_flex_align(lv_obj_t * obj, lv_flex_align_t main_place, lv_flex_align_t cross_place,
                           lv_flex_align_t track_cross_place);

/**
 * Sets the width or height (on main axis) to grow the object in order fill the free space
 * @param obj pointer to an object. The parent must have flex layout else nothing will happen.
 * @param grow a value to set how much free space to take proportionally to other growing items.
 */
void lv_obj_set_flex_grow(lv_obj_t * obj, uint8_t grow);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_FLEX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FLEX_H*/
