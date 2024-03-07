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
#include "../../lv_conf_internal.h"
#include "../../misc/lv_area.h"

#if LV_USE_GRID

/*********************
 *      DEFINES
 *********************/
/**
 * Can be used track size to make the track fill the free space.
 * @param x how much space to take proportionally to other FR tracks
 * @return a special track size
 */
#define LV_GRID_FR(x)          (LV_COORD_MAX - 100 + x)

#define LV_GRID_CONTENT        (LV_COORD_MAX - 101)
LV_EXPORT_CONST_INT(LV_GRID_CONTENT);

#define LV_GRID_TEMPLATE_LAST  (LV_COORD_MAX)
LV_EXPORT_CONST_INT(LV_GRID_TEMPLATE_LAST);

/**********************
 *      TYPEDEFS
 **********************/

/*Can't include lv_obj.h because it includes this header file*/

typedef enum {
    LV_GRID_ALIGN_START,
    LV_GRID_ALIGN_CENTER,
    LV_GRID_ALIGN_END,
    LV_GRID_ALIGN_STRETCH,
    LV_GRID_ALIGN_SPACE_EVENLY,
    LV_GRID_ALIGN_SPACE_AROUND,
    LV_GRID_ALIGN_SPACE_BETWEEN,
} lv_grid_align_t;

/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_grid_init(void);

void lv_obj_set_grid_dsc_array(lv_obj_t * obj, const int32_t col_dsc[], const int32_t row_dsc[]);

void lv_obj_set_grid_align(lv_obj_t * obj, lv_grid_align_t column_align, lv_grid_align_t row_align);

/**
 * Set the cell of an object. The object's parent needs to have grid layout, else nothing will happen
 * @param obj pointer to an object
 * @param column_align the vertical alignment in the cell. `LV_GRID_START/END/CENTER/STRETCH`
 * @param col_pos column ID
 * @param col_span number of columns to take (>= 1)
 * @param row_align the horizontal alignment in the cell. `LV_GRID_START/END/CENTER/STRETCH`
 * @param row_pos row ID
 * @param row_span number of rows to take (>= 1)
 */
void lv_obj_set_grid_cell(lv_obj_t * obj, lv_grid_align_t column_align, int32_t col_pos, int32_t col_span,
                          lv_grid_align_t row_align, int32_t row_pos, int32_t row_span);

/**
 * Just a wrapper to `LV_GRID_FR` for bindings.
 */
static inline int32_t lv_grid_fr(uint8_t x)
{
    return LV_GRID_FR(x);
}

/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GRID*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GRID_H*/
