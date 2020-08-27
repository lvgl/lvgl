/**
 * @file lv_templ.h
 *
 */

#ifndef LV_ALIGN_H
#define LV_ALIGN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    const lv_coord_t * col_dsc;
    const lv_coord_t * row_dsc;
    lv_coord_t gap_col;
    lv_coord_t gap_row;
    uint8_t flow;
    uint8_t place_content;
    uint8_t place_items;
}lv_grid_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_grid_init(lv_grid_t * grid);

void lv_grid_set_template(lv_grid_t * grid, const lv_coord_t * col_dsc, const lv_coord_t * row_dsc);

void lv_grid_set_place_content(lv_grid_t * grid, uint8_t col_mode, uint8_t row_mode);

void lv_grid_set_place_items(lv_grid_t * grid, uint8_t col_mode, uint8_t row_mode);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_ALIGN_H*/
