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
#include "../config/lv_conf_internal.h"
#include "../core/lv_area.h"

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

/**
 * Set a grid layout on a Widget by describing its column and row track sizes.
 *
 * Each element of the descriptor arrays is either a size in pixels,
 * `LV_GRID_FR(x)` to take a proportional share of the free space, or
 * `LV_GRID_CONTENT` to fit the largest child on that track. Both arrays must be
 * closed with `LV_GRID_TEMPLATE_LAST`.
 *
 * Only the pointers are saved, so the arrays must be static, global or
 * dynamically allocated. They must outlive the Widget.
 *
 * Calling this function also sets the Widget's layout to `LV_LAYOUT_GRID`, so
 * its children will be positioned by the cells assigned with
 * `lv_obj_set_grid_cell()`.
 *
 * Passing `NULL` for either array turns that axis into a sub-grid: the tracks
 * are taken from the parent Grid container, sliced to the cell this Widget
 * occupies (its own position and span on that axis). This makes a wrapper
 * Widget "transparent" so that its children align to the outer grid's tracks.
 *
 * @param obj       pointer to a Widget which will be the grid container
 * @param col_dsc   array of column track sizes closed by LV_GRID_TEMPLATE_LAST. @nullable
 *                  Pass NULL to inherit the parent's columns (sub-grid)
 * @param row_dsc   array of row track sizes closed by LV_GRID_TEMPLATE_LAST. @nullable
 *                  Pass NULL to inherit the parent's rows (sub-grid)
 *
 * @note Sub-grids are resolved only one level deep: a sub-grid cannot itself
 *       have a sub-grid child.
 * @note An inherited `LV_GRID_CONTENT` track is measured from the sub-grid's own
 *       children, so it can end up a different size than in the parent. Use
 *       fixed or `LV_GRID_FR(x)` tracks on the inherited axis to keep the
 *       sub-grid aligned.
 * @note If both this Widget and its parent have a NULL descriptor on the same
 *       axis, no layout is performed and a warning is logged.
 */
void lv_obj_set_grid_dsc_array(lv_obj_t * obj, const int32_t col_dsc[], const int32_t row_dsc[]);

/**
 * Set how the Grid's tracks are distributed inside the container's content area.
 *
 * This aligns the grid as a whole, similar to CSS's `justify-content`/`align-content`,
 * not of `justify-self`/`align-self`. To align an individual item inside its cell,
 * use the align parameters of `lv_obj_set_grid_cell()`.
 *
 * It only has a visible effect if there is free space left over after the
 * tracks are sized. A grid built purely from `LV_GRID_FR(x)` tracks consumes
 * all available space, so alignment will appear to do nothing.
 *
 * Accepted values on both axes:
 *  - `LV_GRID_ALIGN_START`: tracks are packed to the left/top (default)
 *  - `LV_GRID_ALIGN_CENTER`: tracks are centered
 *  - `LV_GRID_ALIGN_END`: tracks are packed to the right/bottom
 *  - `LV_GRID_ALIGN_SPACE_BETWEEN`: free space is split between the tracks,
 *     none at the edges
 *  - `LV_GRID_ALIGN_SPACE_AROUND`: equal space around each track, so edge
 *     space is half of the space between tracks
 *  - `LV_GRID_ALIGN_SPACE_EVENLY`: gaps between tracks and at the edges are
 *     all equal
 *
 * @param obj           pointer to a Grid container
 * @param column_align  how to distribute the columns horizontally
 * @param row_align     how to distribute the rows vertically
 *
 * @note This function does not set the Widget's layout to `LV_LAYOUT_GRID`
 * @note `LV_GRID_ALIGN_STRETCH` is meaningful only for items in a cell and is
 *       not a valid track alignment here.
 * @note The `SPACE_*` modes compute the spacing from the remaining free space
 *       and therefore override `pad_column`/`pad_row`. With a single track they
 *       behave like `LV_GRID_ALIGN_CENTER`.
 * @note If the container's size is `LV_SIZE_CONTENT` on an axis, there is no
 *       free space by definition and the alignment on that axis is ignored.
 * @note With `LV_BASE_DIR_RTL` on the container the column tracks are laid out
 *       right-to-left, so `START` means right-most.
 */
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
int32_t lv_grid_fr(uint8_t x);

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
