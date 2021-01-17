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
#error "lv_table: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL 1)"
#endif

#include "../lv_core/lv_obj.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_TABLE_COL_MAX
#define LV_TABLE_COL_MAX 12
#endif

/*
   Maximum allowable value of LV_TABLE_CELL_STYLE_CNT is 16
   because of restriction of lv_table_cell_format_t.type to no more than
   4 bits so that lv_table_cell_format_t.s will not exceed 8 bits
*/
#ifndef LV_TABLE_CELL_STYLE_CNT
#  define LV_TABLE_CELL_STYLE_CNT 4
#endif
#if (LV_TABLE_CELL_STYLE_CNT > 16)
#  error LV_TABLE_CELL_STYLE_CNT cannot exceed 16
#endif
/**********************
 *      TYPEDEFS
 **********************/

/**
 * Internal table cell format structure.
 *
 * Use the `lv_table` APIs instead.
 */
typedef union {
    struct {
        uint8_t align : 2;
        uint8_t right_merge : 1;
        uint8_t type : 4; // up to 16 values
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
    lv_style_list_t cell_style[LV_TABLE_CELL_STYLE_CNT];
    lv_coord_t col_w[LV_TABLE_COL_MAX];
uint16_t cell_types :
    LV_TABLE_CELL_STYLE_CNT; /*Keep track which cell types exists to avoid dealing with unused ones*/
} lv_table_ext_t;

/*Parts of the table*/
enum {
    LV_TABLE_PART_BG,     /* Because of this member, LV_PART.*CELL1 has enum value of 1,        */
    LV_TABLE_PART_CELL1,  /*   LV_PART.*CELL2 has an enum value of 2 and so on up to the maximum */
    LV_TABLE_PART_CELL2,  /*   number of styles specified by LV_TABLE_CELL_STYLE_CNT            */
    LV_TABLE_PART_CELL3,
    LV_TABLE_PART_CELL4,  /* CELL 5-16 are not needed to be defined, the values in this enum
                             are there for backward compatibility */
};

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
 * @param align LV_LABEL_ALIGN_LEFT or LV_LABEL_ALIGN_CENTER or LV_LABEL_ALIGN_RIGHT
 */
void lv_table_set_cell_align(lv_obj_t * table, uint16_t row, uint16_t col, lv_label_align_t align);

/**
 * Set the type of a cell.
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param type 1,2,3 or 4. The cell style will be chosen accordingly.
 */
void lv_table_set_cell_type(lv_obj_t * table, uint16_t row, uint16_t col, uint8_t type);

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
 * Get the text align of a cell
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return LV_LABEL_ALIGN_LEFT (default in case of error) or LV_LABEL_ALIGN_CENTER or
 * LV_LABEL_ALIGN_RIGHT
 */
lv_label_align_t lv_table_get_cell_align(lv_obj_t * table, uint16_t row, uint16_t col);

/**
 * Get the type of a cell
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return 1,2,3 or 4
 */
lv_label_align_t lv_table_get_cell_type(lv_obj_t * table, uint16_t row, uint16_t col);

/**
 * Get the crop property of a cell
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return true: text crop enabled; false: disabled
 */
lv_label_align_t lv_table_get_cell_crop(lv_obj_t * table, uint16_t row, uint16_t col);

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
