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

#include "../label/lv_label.h"

#if LV_USE_TABLE != 0

/*Testing of dependencies*/
#if LV_USE_LABEL == 0
#error "lv_table: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL 1)"
#endif

/*********************
 *      DEFINES
 *********************/
#define LV_TABLE_CELL_NONE 0XFFFF
LV_EXPORT_CONST_INT(LV_TABLE_CELL_NONE);

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_TABLE_CELL_CTRL_MERGE_RIGHT = 1 << 0,
    LV_TABLE_CELL_CTRL_TEXT_CROP   = 1 << 1,
    LV_TABLE_CELL_CTRL_CUSTOM_1    = 1 << 4,
    LV_TABLE_CELL_CTRL_CUSTOM_2    = 1 << 5,
    LV_TABLE_CELL_CTRL_CUSTOM_3    = 1 << 6,
    LV_TABLE_CELL_CTRL_CUSTOM_4    = 1 << 7,
} lv_table_cell_ctrl_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_table_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a table object
 * @param parent        pointer to an object, it will be the parent of the new table
 * @return              pointer to the created table
 */
lv_obj_t * lv_table_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the value of a cell.
 * @param obj           pointer to a Table object
 * @param row           id of the row [0 .. row_cnt -1]
 * @param col           id of the column [0 .. col_cnt -1]
 * @param txt           text to display in the cell. It will be copied and saved so this variable is not required after this function call.
 * @note                New roes/columns are added automatically if required
 */
void lv_table_set_cell_value(lv_obj_t * obj, uint32_t row, uint32_t col, const char * txt);

/**
 * Set the value of a cell.  Memory will be allocated to store the text by the table.
 * @param obj           pointer to a Table object
 * @param row           id of the row [0 .. row_cnt -1]
 * @param col           id of the column [0 .. col_cnt -1]
 * @param fmt           `printf`-like format
 * @note                New roes/columns are added automatically if required
 */
void lv_table_set_cell_value_fmt(lv_obj_t * obj, uint32_t row, uint32_t col, const char * fmt,
                                 ...) LV_FORMAT_ATTRIBUTE(4, 5);

/**
 * Set the number of rows
 * @param obj           table pointer to a Table object
 * @param row_cnt       number of rows
 */
void lv_table_set_row_count(lv_obj_t * obj, uint32_t row_cnt);

/**
 * Set the number of columns
 * @param obj       table pointer to a Table object
 * @param col_cnt   number of columns.
 */
void lv_table_set_column_count(lv_obj_t * obj, uint32_t col_cnt);

/**
 * Set the width of a column
 * @param obj       table pointer to a Table object
 * @param col_id    id of the column [0 .. LV_TABLE_COL_MAX -1]
 * @param w         width of the column
 */
void lv_table_set_column_width(lv_obj_t * obj, uint32_t col_id, int32_t w);

/**
 * Add control bits to the cell.
 * @param obj       pointer to a Table object
 * @param row       id of the row [0 .. row_cnt -1]
 * @param col       id of the column [0 .. col_cnt -1]
 * @param ctrl      OR-ed values from ::lv_table_cell_ctrl_t
 */
void lv_table_add_cell_ctrl(lv_obj_t * obj, uint32_t row, uint32_t col, lv_table_cell_ctrl_t ctrl);

/**
 * Clear control bits of the cell.
 * @param obj       pointer to a Table object
 * @param row       id of the row [0 .. row_cnt -1]
 * @param col       id of the column [0 .. col_cnt -1]
 * @param ctrl      OR-ed values from ::lv_table_cell_ctrl_t
 */
void lv_table_clear_cell_ctrl(lv_obj_t * obj, uint32_t row, uint32_t col, lv_table_cell_ctrl_t ctrl);

/**
 * Add custom user data to the cell.
 * @param obj       pointer to a Table object
 * @param row       id of the row [0 .. row_cnt -1]
 * @param col       id of the column [0 .. col_cnt -1]
 * @param user_data pointer to the new user_data.
 *                  Should be allocated by `lv_malloc`,
 *                  and it will be freed automatically when the table is deleted or
 *                  when the cell is dropped due to lower row or column count.
 */
void lv_table_set_cell_user_data(lv_obj_t * obj, uint16_t row, uint16_t col, void * user_data);

/**
 * Set the selected cell
 * @param obj       pointer to a table object
 * @param row       id of the cell row to select
 * @param col       id of the cell column to select
 */
void lv_table_set_selected_cell(lv_obj_t * obj, uint16_t row, uint16_t col);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a cell.
 * @param obj       pointer to a Table object
 * @param row       id of the row [0 .. row_cnt -1]
 * @param col       id of the column [0 .. col_cnt -1]
 * @return          text in the cell
 */
const char * lv_table_get_cell_value(lv_obj_t * obj, uint32_t row, uint32_t col);

/**
 * Get the number of rows.
 * @param obj       table pointer to a Table object
 * @return          number of rows.
 */
uint32_t lv_table_get_row_count(lv_obj_t * obj);

/**
 * Get the number of columns.
 * @param obj       table pointer to a Table object
 * @return          number of columns.
 */
uint32_t lv_table_get_column_count(lv_obj_t * obj);

/**
 * Get the width of a column
 * @param obj       table pointer to a Table object
 * @param col       id of the column [0 .. LV_TABLE_COL_MAX -1]
 * @return          width of the column
 */
int32_t lv_table_get_column_width(lv_obj_t * obj, uint32_t col);

/**
 * Get whether a cell has the control bits
 * @param obj       pointer to a Table object
 * @param row       id of the row [0 .. row_cnt -1]
 * @param col       id of the column [0 .. col_cnt -1]
 * @param ctrl      OR-ed values from ::lv_table_cell_ctrl_t
 * @return          true: all control bits are set; false: not all control bits are set
 */
bool lv_table_has_cell_ctrl(lv_obj_t * obj, uint32_t row, uint32_t col, lv_table_cell_ctrl_t ctrl);

/**
 * Get the selected cell (pressed and or focused)
 * @param obj       pointer to a table object
 * @param row       pointer to variable to store the selected row (LV_TABLE_CELL_NONE: if no cell selected)
 * @param col       pointer to variable to store the selected column  (LV_TABLE_CELL_NONE: if no cell selected)
 */
void lv_table_get_selected_cell(lv_obj_t * obj, uint32_t * row, uint32_t * col);

/**
 * Get custom user data to the cell.
 * @param obj       pointer to a Table object
 * @param row       id of the row [0 .. row_cnt -1]
 * @param col       id of the column [0 .. col_cnt -1]
 */
void * lv_table_get_cell_user_data(lv_obj_t * obj, uint16_t row, uint16_t col);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TABLE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TABLE_H*/
