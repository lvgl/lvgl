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

#define LV_GRID_CENTER              0
#define LV_GRID_START               1
#define LV_GRID_END                 2
#define LV_GRID_STRETCH             3
#define LV_GRID_SPACE_EVENLY        4
#define LV_GRID_SPACE_AROUND        5
#define LV_GRID_SPACE_BETWEEN       6

/**
 * Some helper defines
 * */
#define _GRID_IS_CELL(c)        ((c) > LV_COORD_MAX ? true : false)
#define _GRID_CELL_SHIFT        5
#define _GRID_CELL_MAX          ((1 << _GRID_CELL_SHIFT) - 1)
#define _GRID_CELL_POS_MASK     ((1 << _GRID_CELL_SHIFT) - 1)
#define _GRID_CELL_SPAN_MASK    (_GRID_CELL_POS_MASK << _GRID_CELL_SHIFT)
#define _GRID_CELL_FLAG_MASK    (_GRID_CELL_POS_MASK << (2 * _GRID_CELL_SHIFT))
#define _GRID_CELL_FLAG(b)      ((b) << (_GRID_CELL_SHIFT * 2))
#define _GRID_CELL_AUTO         _GRID_CELL_MAX
#define _GRID_GET_CELL_POS(c)   ((c) & _GRID_CELL_POS_MASK)
#define _GRID_GET_CELL_SPAN(c)  (((c) & _GRID_CELL_SPAN_MASK) >> _GRID_CELL_SHIFT)
#define _GRID_GET_CELL_FLAG(c)  ((c) >> (_GRID_CELL_SHIFT * 2) & 0x3)

#define _GRID_CELL_SIZE_PX    0     /* The cell size is set in pixels*/
#define _GRID_CELL_SIZE_FR    1     /* The cell size is set in free units*/

#define LV_GRID_FR(x)   (LV_COORD_MAX + (x))

#define _GRID_IS_PX(x) ((_GRID_IS_FR(x) == false) && (_GRID_IS_AUTO(x) == false) ? true : false)
#define _GRID_IS_FR(x) (x > LV_COORD_MAX ? true : false)
#define _GRID_IS_AUTO(x) (x == LV_GRID_AUTO ? true : false)
#define _GRID_GET_FR(x) ((x) - LV_COORD_MAX)


/**
 * Define a grid cell with position and span.
 * Can be used like `lv_obj_set_pos(btn, LV_GRID_CELL(3,2), LV_GRID_CELL(2,1))`
 */
#define LV_GRID_CELL_START(pos, span)   ((LV_COORD_MAX + (pos) + (span << _GRID_CELL_SHIFT)) | _GRID_CELL_FLAG(LV_GRID_START))
#define LV_GRID_CELL_END(pos, span)     ((LV_COORD_MAX + (pos) + (span << _GRID_CELL_SHIFT)) | _GRID_CELL_FLAG(LV_GRID_END))
#define LV_GRID_CELL_CENTER(pos, span)  ((LV_COORD_MAX + (pos) + (span << _GRID_CELL_SHIFT)) | _GRID_CELL_FLAG(LV_GRID_CENTER))
#define LV_GRID_CELL_STRETCH(pos, span) ((LV_COORD_MAX + (pos) + (span << _GRID_CELL_SHIFT)) | _GRID_CELL_FLAG(LV_GRID_STRETCH))
/**
 * Special LV_GRID_CELL position to flow the object automatically.
 * Both X (column) and Y (row) value needs to be AUTO or explicitly specified*/
#define LV_GRID_AUTO_START      LV_GRID_CELL_START(_GRID_CELL_AUTO, 0)
#define LV_GRID_AUTO_END        LV_GRID_CELL_END(_GRID_CELL_AUTO, 0)
#define LV_GRID_AUTO_CENTER     LV_GRID_CELL_CENTER(_GRID_CELL_AUTO, 0)
#define LV_GRID_AUTO_STRETCH    LV_GRID_CELL_STRETCH(_GRID_CELL_AUTO, 0)

/**********************
 *      TYPEDEFS
 **********************/

/* Can't include lv_obj.h because it includes this header file */
struct _lv_obj_t;
typedef struct _lv_obj_t lv_obj_t;

/**
 * Describe how to flow LV_GRID_POS_AUTO elements
 */
typedef enum {
    LV_GRID_FLOW_COLUMN, /**Fill each column and add now columns if required*/
    LV_GRID_FLOW_ROW,    /**Fill each row and add now rows if required*/
}lv_grid_flow_t;

typedef struct {
    const lv_coord_t * col_dsc;
    const lv_coord_t * row_dsc;
    uint8_t col_dsc_len;
    uint8_t row_dsc_len;
    lv_coord_t col_gap;
    lv_coord_t row_gap;
    lv_grid_flow_t flow;
    uint8_t place_content;
}lv_grid_t;

typedef struct {
    lv_coord_t * col_dsc;
    lv_coord_t * row_dsc;
    uint32_t col_dsc_len;
    uint32_t row_dsc_len;
}_lv_grid_calc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_grid_init(lv_grid_t * grid);

void lv_grid_set_template(lv_grid_t * grid, const lv_coord_t * col_dsc, const lv_coord_t * row_dsc);

void lv_grid_set_place_content(lv_grid_t * grid, uint8_t col_place, uint8_t row_place);

void _lv_grid_calc(struct _lv_obj_t * obj, _lv_grid_calc_t * calc);

void _lv_grid_calc_free(_lv_grid_calc_t * calc);

bool _lv_grid_has_fr_col(struct _lv_obj_t * obj);

bool _lv_grid_has_fr_row(struct _lv_obj_t * obj);

void _lv_grid_full_refresh(lv_obj_t * cont);

void lv_grid_item_refr_pos(lv_obj_t * item);

bool _lv_obj_is_grid_item(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_GRID_H*/
