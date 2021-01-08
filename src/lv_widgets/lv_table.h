/**
 * @file lv_table.h
 *
 */

#ifndef LV_TABLE_H
#define LV_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_TABLE != 0

/*Testing of dependencies*/
#if LV_USE_LABEL == 0
#error "lv_table: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef bool (*lv_table_cell_drawer_cb_t)(lv_obj_t * table, uint32_t row, uint32_t cell, lv_draw_rect_dsc_t * rect_draw_dsc, lv_draw_label_dsc_t * label_draw_dsc, const lv_area_t * draw_area, const lv_area_t * clip_area);

/**
 * Internal table cell format structure.
 *
 * Use the `lv_table` APIs instead.
 */
typedef union {
    struct {
        uint8_t align : 2;
        uint8_t right_merge : 1;
        uint8_t crop : 1;
    } s;
    uint8_t format_byte;
} lv_table_cell_format_t;

/*Data of table*/
typedef struct {
    /*New data for this type */
    uint16_t col_cnt;
    uint16_t row_cnt;
    char ** cell_data;
    lv_coord_t * row_h;
    lv_coord_t * col_w;
    lv_table_cell_drawer_cb_t drawer_cb;
} lv_table_ext_t;

extern const lv_obj_class_t lv_table;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a table object
 * @param par pointer to an object, it will be the parent of the new table
 * @param copy pointer to a table object, if not NULL then the new object will be copied from it
 * @return pointer to the created table
 */
lv_obj_t * lv_table_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the value of a cell.
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param txt text to display in the cell. It will be copied and saved so this variable is not
 * required after this function call.
 */
void lv_table_set_cell_value(lv_obj_t * table, uint16_t row, uint16_t col, const char * txt);

/**
 * Set the value of a cell.  Memory will be allocated to store the text by the table.
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param fmt `printf`-like format
 */
void lv_table_set_cell_value_fmt(lv_obj_t * table, uint16_t row, uint16_t col, const char * fmt, ...);

/**
 * Set the number of rows
 * @param table table pointer to a Table object
 * @param row_cnt number of rows
 */
void lv_table_set_row_cnt(lv_obj_t * table, uint16_t row_cnt);

/**
 * Set the number of columns
 * @param table table pointer to a Table object
 * @param col_cnt number of columns. Must be < LV_TABLE_COL_MAX
 */
void lv_table_set_col_cnt(lv_obj_t * table, uint16_t col_cnt);

/**
 * Set the width of a column
 * @param table table pointer to a Table object
 * @param col_id id of the column [0 .. LV_TABLE_COL_MAX -1]
 * @param w width of the column
 */
void lv_table_set_col_width(lv_obj_t * table, uint16_t col_id, lv_coord_t w);

/**
 * Set the text align in a cell
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param align LV_TEXT_ALIGN_LEFT or LV_TEXT_ALIGN_CENTER or LV_TEXT_ALIGN_RIGHT
 */
void lv_table_set_cell_align(lv_obj_t * table, uint16_t row, uint16_t col, lv_text_align_t align);

/**
 * Set the cell crop. (Don't adjust the height of the cell according to its content)
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param crop true: crop the cell content; false: set the cell height to the content.
 */
void lv_table_set_cell_crop(lv_obj_t * table, uint16_t row, uint16_t col, bool crop);

/**
 * Merge a cell with the right neighbor. The value of the cell to the right won't be displayed.
 * @param table table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param en true: merge right; false: don't merge right
 */
void lv_table_set_cell_merge_right(lv_obj_t * table, uint16_t row, uint16_t col, bool en);

void lv_table_set_cell_drawer(lv_obj_t * obj, lv_table_cell_drawer_cb_t drawer_cb);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a cell.
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return text in the cell
 */
const char * lv_table_get_cell_value(lv_obj_t * table, uint16_t row, uint16_t col);

/**
 * Get the number of rows.
 * @param table table pointer to a Table object
 * @return number of rows.
 */
uint16_t lv_table_get_row_cnt(lv_obj_t * table);

/**
 * Get the number of columns.
 * @param table table pointer to a Table object
 * @return number of columns.
 */
uint16_t lv_table_get_col_cnt(lv_obj_t * table);

/**
 * Get the width of a column
 * @param table table pointer to a Table object
 * @param col_id id of the column [0 .. LV_TABLE_COL_MAX -1]
 * @return width of the column
 */
lv_coord_t lv_table_get_col_width(lv_obj_t * table, uint16_t col_id);

/**
 * Get the crop property of a cell
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return true: text crop enabled; false: disabled
 */
bool lv_table_get_cell_crop(lv_obj_t * table, uint16_t row, uint16_t col);

/**
 * Get the cell merge attribute.
 * @param table table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return true: merge right; false: don't merge right
 */
bool lv_table_get_cell_merge_right(lv_obj_t * table, uint16_t row, uint16_t col);

/**
 * Get the last pressed or being pressed cell
 * @param table pointer to a table object
 * @param row pointer to variable to store the pressed row
 * @param col pointer to variable to store the pressed column
 * @return LV_RES_OK: a valid pressed cell was found, LV_RES_INV: no valid cell is pressed
 */
lv_res_t lv_table_get_pressed_cell(lv_obj_t * table, uint16_t * row, uint16_t * col);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TABLE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TABLE_H*/
