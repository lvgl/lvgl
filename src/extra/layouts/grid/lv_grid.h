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
#define LV_GRID_TEMPLATE_LAST  (LV_COORD_MAX)

/**********************
 *      TYPEDEFS
 **********************/

/*Can't include lv_obj.h because it includes this header file*/
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

/**********************
 * GLOBAL VARIABLES
 **********************/

extern uint32_t LV_LAYOUT_GRID;
extern lv_style_prop_t LV_STYLE_GRID_COL_TEMPLATE;
extern lv_style_prop_t LV_STYLE_GRID_COL_PLACE;
extern lv_style_prop_t LV_STYLE_GRID_ROW_TEMPLATE;
extern lv_style_prop_t LV_STYLE_GRID_ROW_PLACE;
extern lv_style_prop_t LV_STYLE_GRID_CELL_COL_POS;
extern lv_style_prop_t LV_STYLE_GRID_CELL_COL_SPAN;
extern lv_style_prop_t LV_STYLE_GRID_CELL_COL_PLACE;
extern lv_style_prop_t LV_STYLE_GRID_CELL_ROW_POS;
extern lv_style_prop_t LV_STYLE_GRID_CELL_ROW_SPAN;
extern lv_style_prop_t LV_STYLE_GRID_CELL_ROW_PLACE;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_grid_init(void);

void lv_obj_set_grid_template(lv_obj_t * obj, const lv_coord_t * col_templ, const lv_coord_t * row_templ);

void lv_obj_set_grid_place(lv_obj_t * obj, lv_grid_place_t hor_place, lv_grid_place_t ver_place);

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
 * Just a wrapper to `LV_GRID_FR` for bindings.
 */
static inline lv_coord_t lv_grid_fr(uint8_t x)
{
    return LV_GRID_FR(x);
}

static inline void lv_style_set_grid_row_template(lv_style_t * style, const lv_coord_t * value)
{
    lv_style_value_t v = {
        .ptr = (const void *)value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_ROW_TEMPLATE, v);
}

static inline void lv_style_set_grid_column_template(lv_style_t * style, const lv_coord_t * value)
{
    lv_style_value_t v = {
        .ptr = (const void *)value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_COL_TEMPLATE, v);
}

static inline void lv_style_set_grid_row_place(lv_style_t * style, lv_grid_place_t value)
{
    lv_style_value_t v = {
        .num = (lv_grid_place_t)value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_ROW_PLACE, v);
}

static inline void lv_style_set_grid_col_place(lv_style_t * style, lv_grid_place_t value)
{
    lv_style_value_t v = {
        .num = (lv_grid_place_t)value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_COL_PLACE, v);
}


static inline void lv_style_set_grid_cell_column_pos(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_CELL_COL_POS, v);
}

static inline void lv_style_set_grid_cell_column_span(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_CELL_COL_SPAN, v);
}

static inline void lv_style_set_grid_cell_row_pos(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_CELL_ROW_POS, v);
}

static inline void lv_style_set_grid_cell_row_span(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_CELL_ROW_SPAN, v);
}

static inline void lv_style_set_grid_cell_x_place(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_CELL_COL_PLACE, v);
}

static inline void lv_style_set_grid_cell_y_place(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_CELL_ROW_PLACE, v);
}

static inline void lv_obj_set_style_grid_row_template(lv_obj_t * obj, lv_part_t part, lv_state_t state, const lv_coord_t * value)
{
    lv_style_value_t v = {
        .ptr = (const void *)value
    };
    lv_obj_set_local_style_prop(obj, part, state,LV_STYLE_GRID_ROW_TEMPLATE, v);
}

static inline void lv_obj_set_style_grid_column_template(lv_obj_t * obj, lv_part_t part, lv_state_t state, const lv_coord_t * value)
{
    lv_style_value_t v = {
        .ptr = (const void *)value
    };
    lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_GRID_COL_TEMPLATE, v);
}


static inline void lv_obj_set_style_grid_row_place(lv_obj_t * obj, lv_part_t part, lv_state_t state, lv_grid_place_t value)
{
    lv_style_value_t v = {
        .num = (int32_t) value
    };
    lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_GRID_ROW_PLACE, v);
}

static inline void lv_obj_set_style_grid_column_place(lv_obj_t * obj, lv_part_t part, lv_state_t state, lv_grid_place_t value)
{
    lv_style_value_t v = {
        .num = (int32_t) value
    };
    lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_GRID_COL_PLACE, v);
}


static inline void lv_obj_set_style_grid_cell_column_pos(lv_obj_t * obj, lv_part_t part, lv_state_t state, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_obj_set_local_style_prop(obj, part, state,LV_STYLE_GRID_CELL_COL_POS, v);
}

static inline void lv_obj_set_style_grid_cell_column_span(lv_obj_t * obj, lv_part_t part, lv_state_t state, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_obj_set_local_style_prop(obj, part, state,LV_STYLE_GRID_CELL_COL_SPAN, v);
}

static inline void lv_obj_set_style_grid_cell_row_pos(lv_obj_t * obj, lv_part_t part, lv_state_t state, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_obj_set_local_style_prop(obj, part, state,LV_STYLE_GRID_CELL_ROW_POS, v);
}

static inline void lv_obj_set_style_grid_cell_row_span(lv_obj_t * obj, lv_part_t part, lv_state_t state, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_GRID_CELL_ROW_SPAN, v);
}

static inline void lv_obj_set_style_grid_cell_x_place(lv_obj_t * obj, lv_part_t part, lv_state_t state, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_GRID_CELL_COL_PLACE, v);
}

static inline void lv_obj_set_style_grid_cell_y_place(lv_obj_t * obj, lv_part_t part, lv_state_t state, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_GRID_CELL_ROW_PLACE, v);
}




static inline const lv_coord_t * lv_obj_get_style_grid_row_template(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_ROW_TEMPLATE);
    return (const lv_coord_t *)v.ptr;
}

static inline const lv_coord_t * lv_obj_get_style_grid_column_template(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_COL_TEMPLATE);
    return (const lv_coord_t *)v.ptr;
}

static inline lv_grid_place_t lv_obj_get_style_grid_row_place(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_ROW_PLACE);
    return (lv_grid_place_t)v.num;
}

static inline lv_grid_place_t lv_obj_get_style_grid_column_place(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_COL_PLACE);
    return (lv_grid_place_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_grid_cell_column_pos(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_COL_POS);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_grid_cell_column_span(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_COL_SPAN);
    return (lv_coord_t)v.num;
}


static inline lv_coord_t lv_obj_get_style_grid_cell_row_pos(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_ROW_POS);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_grid_cell_row_span(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_ROW_SPAN);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_grid_cell_x_place(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_COL_PLACE);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_grid_cell_y_place(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_GRID_CELL_ROW_PLACE);
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
