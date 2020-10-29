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

void lv_obj_set_flex_cont(lv_obj_t * obj, lv_flex_dir_t flex_dir, lv_flex_place_t flex_place);
void lv_obj_set_flex_gap(lv_obj_t * obj, lv_coord_t gap);
void lv_obj_set_flex_item(lv_obj_t * obj, lv_flex_place_t place);

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
