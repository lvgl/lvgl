/**
 * @file lv_align.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_align.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void grid_calc(lv_grid_t * grid, _lv_grid_calc_t * calc)
{
    calc->col_dsc_len = grid->col_dsc_len + 1;
    calc->col_dsc = _lv_mem_buf_get(sizeof(lv_coord_t) * calc->col_dsc_len);
    calc->row_dsc_len = grid->row_dsc_len + 1;
    calc->row_dsc = _lv_mem_buf_get(sizeof(lv_coord_t) * calc->row_dsc_len);

    calc->col_dsc[0] = 0;
    calc->row_dsc[0] = 0;

    uint8_t i;
    for(i = 0; i < grid->col_dsc_len; i++) {
        calc->col_dsc[i + 1] = calc->col_dsc[i] + grid->col_dsc[i];
    }
    for(i = 0; i < grid->row_dsc_len; i++) {
        calc->row_dsc[i + 1] = calc->row_dsc[i] + grid->row_dsc[i];
    }
}

void grid_calc_free(_lv_grid_calc_t * calc)
{
    _lv_mem_buf_release(calc->col_dsc);
    _lv_mem_buf_release(calc->row_dsc);
}

uint8_t _lv_grid_get_col_type(lv_grid_t * grid, uint32_t col)
{
    lv_coord_t x;
    /*If explicitly defined get the column size else use the first column*/
    if(col < grid->col_dsc_len) x = grid->col_dsc[col];
    else x = grid->col_dsc[0];

    if(_GRID_IS_FR(x)) return _GRID_CELL_SIZE_FR;
    else if(_GRID_IS_AUTO(x)) return _GRID_CELL_SIZE_AUTO;
    else return _GRID_CELL_SIZE_PX;
}

uint8_t _lv_grid_get_row_type(lv_grid_t * grid, uint32_t row)
{
    lv_coord_t x;
    /*If explicitly defined get the row size else use the first row*/
    if(row < grid->row_dsc_len) x = grid->row_dsc[row];
    else x = grid->row_dsc[0];

    if(_GRID_IS_FR(x)) return _GRID_CELL_SIZE_FR;
    else if(_GRID_IS_AUTO(x)) return _GRID_CELL_SIZE_AUTO;
    else return _GRID_CELL_SIZE_PX;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
