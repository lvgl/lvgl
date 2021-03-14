/**
 * @file lv_grid.h
 *
 */

#ifndef LV_GRID_H
#define LV_GRID_H

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
#if LV_USE_GRID

/**********************
 *      TYPEDEFS
 **********************/

/* Can't include lv_obj.h because it includes this header file */
struct _lv_obj_t;

typedef enum {
    LV_GRID_START,
    LV_GRID_CENTER,
    LV_GRID_END,
    LV_GRID_STRETCH,
    LV_GRID_SPACE_EVENLY,
    LV_GRID_SPACE_AROUND,
    LV_GRID_SPACE_BETWEEN,
}lv_grid_place_t;

typedef struct {
    lv_layout_dsc_t base; /*The first element must be the update callback*/
    const lv_coord_t * col_dsc;
    const lv_coord_t * row_dsc;
    uint8_t col_dsc_len;
    uint8_t row_dsc_len;
    uint8_t col_place;
    uint8_t row_place;
}lv_grid_t;

typedef struct {
    lv_coord_t * x;
    lv_coord_t * y;
    lv_coord_t * w;
    lv_coord_t * h;
    uint32_t col_num;
    uint32_t row_num;
    lv_coord_t grid_w;
    lv_coord_t grid_h;
}_lv_grid_calc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**
 * Initialize a grid layout the default values
 * @param grid pointer to a grid layout descriptor
 */
void lv_grid_init(lv_grid_t * grid);

/**
 * Set the number of rows and columns and their sizes
 * @param grid pointer to a grid layout descriptor
 * @param col_dsc an array with the column widths
 * @param col_cnt number of columns (max 16)
 * @param row_dsc an array with the row heights
 * @param row_cnt number of rows (max 16)
 * @note `LV_GRID_FR(x)` can be used as track size.
 */
void lv_grid_set_template(lv_grid_t * grid, const lv_coord_t * col_dsc, uint8_t col_cnt, const lv_coord_t * row_dsc, uint8_t row_cnt);

/**
 * Set how to place (where to align) the rows and columns
 * @param grid
 * @param col_place
 * @param row_place
 */
void lv_grid_set_place(lv_grid_t * grid, uint8_t col_place, uint8_t row_place);

/**
 * Set the cell of an object. The object's parent needs to have grid layout, else nothing will happen
 * @param obj pointer to an object
 * @param hor_place the vertical alignment in the cell. `LV_GRID_START/END/CENTER/STRETCH`
 * @param col_pos column ID
 * @param col_span number of columns to take (>= 1)
 * @param ver_place the horizontal alignment in the cell. `LV_GRID_START/END/CENTER/STRETCH`
 * @param row_pos row ID
 * @param row_span number of rows to take (>= 1)
 */
void lv_obj_set_grid_cell(struct _lv_obj_t * obj, lv_grid_place_t hor_place, uint8_t col_pos, uint8_t col_span,
                                          lv_grid_place_t ver_place, uint8_t row_pos, uint8_t row_span);

/**
 * Can be used track size to make the track fill the free space.
 * @param x how much space to take proportionally to other FR tracks
 * @return a special track size
 */
#define LV_GRID_FR(x)          (LV_COORD_SET_LAYOUT(x))

#define LV_GRID_CONTENT        (LV_COORD_SET_LAYOUT(100))

/**
 * Just a wrapper to `LV_GRID_FR` for bindings.
 */
static inline lv_coord_t lv_grid_fr(uint8_t x)
{
    return LV_GRID_FR(x);
}

/**********************
 * GLOBAL VARIABLES
 **********************/
extern const lv_grid_t grid_12;

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_GRID*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_GRID_H*/
