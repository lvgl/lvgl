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
#include "../lv_misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**
 * Some helper defines
 * */
#define _LV_GRID_CELL_SHIFT        4
#define _LV_GRID_CELL_POS_MASK     ((1 << _LV_GRID_CELL_SHIFT) - 1)
#define _LV_GRID_CELL_SPAN_MASK    (_LV_GRID_CELL_POS_MASK << _LV_GRID_CELL_SHIFT)
#define _LV_GRID_CELL_FLAG_MASK    (_LV_GRID_CELL_POS_MASK << (2 * _LV_GRID_CELL_SHIFT))
#define _LV_GRID_CELL_PLACE(b)      ((b) << (_LV_GRID_CELL_SHIFT * 2))

#define LV_GRID_START               0
#define LV_GRID_CENTER              1
#define LV_GRID_END                 2
#define LV_GRID_STRETCH             3
#define LV_GRID_SPACE_EVENLY        4
#define LV_GRID_SPACE_AROUND        5
#define LV_GRID_SPACE_BETWEEN       6

#define LV_GRID_CELL_START(pos, span)   (_LV_COORD_GRID((pos) | ((span) << (_LV_GRID_CELL_SHIFT)) | _LV_GRID_CELL_PLACE(LV_GRID_START)))
#define LV_GRID_CELL_END(pos, span)     (_LV_COORD_GRID((pos) | ((span) << (_LV_GRID_CELL_SHIFT)) | _LV_GRID_CELL_PLACE(LV_GRID_END)))
#define LV_GRID_CELL_CENTER(pos, span)  (_LV_COORD_GRID((pos) | ((span) << (_LV_GRID_CELL_SHIFT)) | _LV_GRID_CELL_PLACE(LV_GRID_CENTER)))
#define LV_GRID_CELL_STRETCH(pos, span) (_LV_COORD_GRID((pos) | ((span) << (_LV_GRID_CELL_SHIFT)) | _LV_GRID_CELL_PLACE(LV_GRID_STRETCH)))

#define LV_GRID_GET_CELL_POS(c)   ((c) & _LV_GRID_CELL_POS_MASK)
#define LV_GRID_GET_CELL_SPAN(c)  (((c) & _LV_GRID_CELL_SPAN_MASK) >> _LV_GRID_CELL_SHIFT)
#define LV_GRID_GET_CELL_PLACE(c)  ((c) >> (_LV_GRID_CELL_SHIFT * 2) & 0x7)

#define LV_GRID_FR(x)          (_LV_COORD_GRID(x))
#define LV_GRID_IS_FR(x)       (LV_COORD_IS_GRID(x))
#define LV_GRID_GET_FR(x)      (LV_COORD_GET_GRID(x))

/**********************
 *      TYPEDEFS
 **********************/

/* Can't include lv_obj.h because it includes this header file */
struct _lv_obj_t;
typedef struct _lv_obj_t lv_obj_t;

typedef struct {
    const lv_coord_t * col_dsc;
    const lv_coord_t * row_dsc;
    uint8_t col_dsc_len;
    uint8_t row_dsc_len;
    lv_coord_t col_gap;
    lv_coord_t row_gap;
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

void lv_grid_init(lv_grid_t * grid);

void lv_grid_set_template(lv_grid_t * grid, const lv_coord_t * col_dsc, uint8_t col_cnt, const lv_coord_t * row_dsc, uint8_t row_cnt);

void lv_grid_set_place(lv_grid_t * grid, uint8_t col_place, uint8_t row_place);

void lv_grid_set_gap(lv_grid_t * grid, lv_coord_t col_gap, uint8_t row_gap);

/**
 * Set a grid for an object
 * @param obj pointer to an object
 * @param grid the grid to set
 */
void lv_obj_set_grid(lv_obj_t * obj, const lv_grid_t * grid);

/**
 * Get the grid of an object
 * @param obj pointer to an object
 * @return the grid, NULL if no grid
 */
const lv_grid_t * lv_obj_get_grid(lv_obj_t * obj);

void lv_obj_set_grid_cell(lv_obj_t * obj, lv_coord_t col_pos, lv_coord_t row_pos);

/**
 * Notify all object if a style is modified
 * @param grid pointer to a grid. Only the objects with this grid will be notified
 *               (NULL to notify all objects with any grid)
 */
void lv_obj_report_grid_change(const lv_grid_t * grid);

void _lv_grid_calc(struct _lv_obj_t * obj, _lv_grid_calc_t * calc);

void _lv_grid_calc_free(_lv_grid_calc_t * calc);

bool _lv_grid_has_fr_col(struct _lv_obj_t * obj);

bool _lv_grid_has_fr_row(struct _lv_obj_t * obj);


void _lv_grid_full_refresh(lv_obj_t * cont);

void lv_grid_item_refr_pos(lv_obj_t * item);


/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_GRID_H*/
