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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#if USE_LV_TABLE != 0

#include "../lv_core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/
#define LV_TABLE_COL_MAX    12

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_TABLE_CELL_ALIGN_LEFT,
    LV_TABLE_CELL_ALIGN_RIGHT,
    LV_TABLE_CELL_ALIGN_CENTER,
}lv_table_cell_align_t;

typedef union {
    struct {
        uint8_t align:2;
        uint8_t right_merge:1;
    };
    uint8_t format_byte;
}lv_table_cell_format_t;

/*Data of table*/
typedef struct {
    /*New data for this type */
    uint16_t col_cnt;
    uint16_t row_cnt;
    char ** cell_data;
    lv_style_t * cell_style;
    lv_coord_t col_w[LV_TABLE_COL_MAX];
} lv_table_ext_t;


/*Styles*/
enum {
    LV_TABLE_STYLE_X,
    LV_TABLE_STYLE_Y,
};
typedef uint8_t lv_table_style_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a table objects
 * @param par pointer to an object, it will be the parent of the new table
 * @param copy pointer to a table object, if not NULL then the new object will be copied from it
 * @return pointer to the created table
 */
lv_obj_t * lv_table_create(lv_obj_t * par, const lv_obj_t * copy);

void lv_table_set_cell_value(lv_obj_t * table, uint16_t row, uint16_t col, const char * txt);

void lv_table_set_cell_format(lv_obj_t * table, uint16_t row, uint16_t col, lv_table_cell_align_t align);

void lv_table_set_cell_merge_right(lv_obj_t * table, uint16_t row, uint16_t col, bool en);

void lv_table_set_row_cnt(lv_obj_t * table, uint16_t row_cnt);

void lv_table_set_col_cnt(lv_obj_t * table, uint16_t col_cnt);

/*======================
 * Add/remove functions
 *=====================*/


/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a table.
 * @param table pointer to table object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_table_set_style(lv_obj_t * table, lv_table_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get style of a table.
 * @param table pointer to table object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_table_get_style(const lv_obj_t * table, lv_table_style_t type);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_TABLE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_TABLE_H*/
