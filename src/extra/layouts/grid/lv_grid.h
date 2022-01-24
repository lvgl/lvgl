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
#include "../../../core/lv_obj.h"
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
struct _lv_obj_t;

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

extern uint32_t LV_LAYOUT_GRID;
extern lv_style_prop_t LV_STYLE_GRID_COLUMN_DSC_ARRAY;
extern lv_style_prop_t LV_STYLE_GRID_COLUMN_ALIGN;
extern lv_style_prop_t LV_STYLE_GRID_ROW_DSC_ARRAY;
extern lv_style_prop_t LV_STYLE_GRID_ROW_ALIGN;
extern lv_style_prop_t LV_STYLE_GRID_CELL_COLUMN_POS;
extern lv_style_prop_t LV_STYLE_GRID_CELL_COLUMN_SPAN;
extern lv_style_prop_t LV_STYLE_GRID_CELL_X_ALIGN;
extern lv_style_prop_t LV_STYLE_GRID_CELL_ROW_POS;
extern lv_style_prop_t LV_STYLE_GRID_CELL_ROW_SPAN;
extern lv_style_prop_t LV_STYLE_GRID_CELL_Y_ALIGN;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_grid_init(void);

void lv_obj_set_grid_dsc_array(lv_obj_t * obj, const lv_coord_t col_dsc[], const lv_coord_t row_dsc[]);

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
void lv_obj_set_grid_cell(lv_obj_t * obj, lv_grid_align_t column_align, uint8_t col_pos, uint8_t col_span,
                          lv_grid_align_t row_align, uint8_t row_pos, uint8_t row_span);

/**
 * Just a wrapper to `LV_GRID_FR` for bindings.
 */
static inline lv_coord_t lv_grid_fr(uint8_t x)
{
    return LV_GRID_FR(x);
}

void lv_style_set_grid_row_dsc_array(lv_style_t * style, const lv_coord_t value[]);
void lv_style_set_grid_column_dsc_array(lv_style_t * style, const lv_coord_t value[]);
void lv_style_set_grid_row_align(lv_style_t * style, lv_grid_align_t value);
void lv_style_set_grid_column_align(lv_style_t * style, lv_grid_align_t value);
void lv_style_set_grid_cell_column_pos(lv_style_t * style, lv_coord_t value);
void lv_style_set_grid_cell_column_span(lv_style_t * style, lv_coord_t value);
void lv_style_set_grid_cell_row_pos(lv_style_t * style, lv_coord_t value);
void lv_style_set_grid_cell_row_span(lv_style_t * style, lv_coord_t value);
void lv_style_set_grid_cell_x_align(lv_style_t * style, lv_coord_t value);
void lv_style_set_grid_cell_y_align(lv_style_t * style, lv_coord_t value);

void lv_obj_set_style_grid_row_dsc_array(lv_obj_t * obj, const lv_coord_t value[], lv_style_selector_t selector);
void lv_obj_set_style_grid_column_dsc_array(lv_obj_t * obj, const lv_coord_t value[], lv_style_selector_t selector);
void lv_obj_set_style_grid_row_align(lv_obj_t * obj, lv_grid_align_t value, lv_style_selector_t selector);
void lv_obj_set_style_grid_column_align(lv_obj_t * obj, lv_grid_align_t value, lv_style_selector_t selector);
void lv_obj_set_style_grid_cell_column_pos(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_grid_cell_column_span(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_grid_cell_row_pos(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_grid_cell_row_span(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_grid_cell_x_align(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_grid_cell_y_align(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);

static inline const lv_coord_t * lv_obj_get_style_grid_row_dsc_array(const lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_ROW_DSC_ARRAY);
    return (const lv_coord_t *)v.ptr;
}

static inline const lv_coord_t * lv_obj_get_style_grid_column_dsc_array(const lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_COLUMN_DSC_ARRAY);
    return (const lv_coord_t *)v.ptr;
}

static inline lv_grid_align_t lv_obj_get_style_grid_row_align(const lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_ROW_ALIGN);
    return (lv_grid_align_t)v.num;
}

static inline lv_grid_align_t lv_obj_get_style_grid_column_align(const lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_COLUMN_ALIGN);
    return (lv_grid_align_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_grid_cell_column_pos(const lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_COLUMN_POS);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_grid_cell_column_span(const lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_COLUMN_SPAN);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_grid_cell_row_pos(const lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_ROW_POS);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_grid_cell_row_span(const lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_ROW_SPAN);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_grid_cell_x_align(const lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_X_ALIGN);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_grid_cell_y_align(const lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_Y_ALIGN);
    return (lv_coord_t)v.num;
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
