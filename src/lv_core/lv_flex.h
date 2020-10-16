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

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_FLEX_START = _LV_COORD_FELX(0),
    LV_FLEX_END = _LV_COORD_FELX(1),
    LV_FLEX_CENTER = _LV_COORD_FELX(2),
    LV_FLEX_SPACE_EVENLY = _LV_COORD_FELX(3),
    LV_FLEX_SPACE_AROUND = _LV_COORD_FELX(4),
    LV_FLEX_SPACE_BETWEEN = _LV_COORD_FELX(5),
}lv_flex_place_t;

typedef enum {
    LV_FLEX_DIR_NONE,
    LV_FLEX_DIR_ROW,
    LV_FLEX_DIR_ROW_WRAP,
    LV_FLEX_DIR_COLUMN,
    LV_FLEX_DIR_COLUMN_WRAP,
} lv_flex_dir_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_obj_set_flex_cont(lv_obj_t * obj, lv_flex_dir_t flex_dir);
void lv_obj_set_flex_item(lv_obj_t * obj, lv_flex_place_t place);
void _lv_flex_refresh(lv_obj_t * cont);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_FLEX_H*/
