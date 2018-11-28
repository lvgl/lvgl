/**
 * @file lv_table.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_table.h"
#if USE_LV_TABLE != 0

#include "../lv_misc/lv_txt.h"
#include "../lv_misc/lv_math.h"
#include "../lv_draw/lv_draw_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_table_design(lv_obj_t * table, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_table_signal(lv_obj_t * table, lv_signal_t sign, void * param);
static lv_coord_t get_row_height(lv_obj_t * table, uint16_t row_id);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;
static lv_design_func_t ancestor_scrl_design;

static const char * cell_data_example[] = {"r1", "rc11", "rc12",
                                            "ro row 2", "rc21", "rc22",
                                            "row3", "rc31", "rc32",
                                            "row4", "rc41", "rc42"};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a table object
 * @param par pointer to an object, it will be the parent of the new table
 * @param copy pointer to a table object, if not NULL then the new object will be copied from it
 * @return pointer to the created table
 */
lv_obj_t * lv_table_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("table create started");

    /*Create the ancestor of table*/
    lv_obj_t * new_table = lv_obj_create(par, copy);
    lv_mem_assert(new_table);
    if(new_table == NULL) return NULL;

    /*Allocate the table type specific extended data*/
    lv_table_ext_t * ext = lv_obj_allocate_ext_attr(new_table, sizeof(lv_table_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_table);
    if(ancestor_scrl_design == NULL) ancestor_scrl_design = lv_obj_get_design_func(new_table);

    /*Initialize the allocated 'ext' */
    ext->cell_data = cell_data_example;
    ext->cell_style = &lv_style_pretty;
    ext->col_cnt = 3;
    ext->row_cnt = 4;
    ext->col_w[0] = 50;
    ext->col_w[1] = 70;
    ext->col_w[2] = 80;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_table, lv_table_signal);
    lv_obj_set_design_func(new_table, lv_table_design);

    /*Init the new table table*/
    if(copy == NULL) {

    }
    /*Copy an existing table*/
    else {
        lv_table_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_table);
    }

    LV_LOG_INFO("table created");

    return new_table;
}

/*======================
 * Add/remove functions
 *=====================*/

/*
 * New object specific "add" or "remove" functions come here
 */


/*=====================
 * Setter functions
 *====================*/

/*
 * New object specific "set" functions come here
 */


/**
 * Set a style of a table.
 * @param table pointer to table object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_table_set_style(lv_obj_t * table, lv_table_style_t type, lv_style_t * style)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);

    switch(type) {
        case LV_TABLE_STYLE_X:
            break;
        case LV_TABLE_STYLE_Y:
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/*
 * New object specific "get" functions come here
 */

/**
 * Get style of a table.
 * @param table pointer to table object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_table_get_style(const lv_obj_t * table, lv_table_style_t type)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);

    switch(type) {
        case LV_TABLE_STYLE_X:
            return NULL;
        case LV_TABLE_STYLE_Y:
            return NULL;
        default:
            return NULL;
    }

    /*To avoid warning*/
    return NULL;
}

/*=====================
 * Other functions
 *====================*/

/*
 * New object specific "other" functions come here
 */

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the tables
 * @param table pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_table_design(lv_obj_t * table, const lv_area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_scrl_design(table, mask, mode);

        lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
        lv_coord_t h_row;
        lv_point_t txt_size;
        lv_area_t cell_area;
        lv_area_t txt_area;

        uint16_t col;
        uint16_t row;
        uint16_t cell = 0;

        cell_area.y2 = table->coords.y1;
        for(row = 0; row < ext->row_cnt; row++) {
            h_row = get_row_height(table, row);

            cell_area.y1 = cell_area.y2;
            cell_area.y2 = cell_area.y1 + h_row;


            uint16_t col_x = 0;
            for(col = 0; col < ext->col_cnt; col++) {

                cell_area.x1 = table->coords.x1 + col_x;
                cell_area.x2 = cell_area.x1 + ext->col_w[col];

                txt_area.x1 = cell_area.x1 + ext->cell_style->body.padding.hor;
                txt_area.x2 = cell_area.x2 - ext->cell_style->body.padding.hor;
                txt_area.y1 = cell_area.y1 + ext->cell_style->body.padding.ver;
                txt_area.y2 = cell_area.y2 - ext->cell_style->body.padding.ver;

                lv_txt_get_size(&txt_size, ext->cell_data[cell], ext->cell_style->text.font,
                        ext->cell_style->text.letter_space, ext->cell_style->text.line_space, lv_area_get_width(&txt_area), LV_TXT_FLAG_NONE);

                col_x += ext->col_w[col];

                lv_draw_rect(&cell_area, mask, ext->cell_style, LV_OPA_COVER);
                lv_draw_label(&txt_area, mask, ext->cell_style, LV_OPA_COVER, ext->cell_data[cell], LV_TXT_FLAG_NONE, NULL);

                cell++;
            }
        }



    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}

/**
 * Signal function of the table
 * @param table pointer to a table object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_table_signal(lv_obj_t * table, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(table, sign, param);
    if(res != LV_RES_OK) return res;


    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_table";
    }

    return res;
}

static lv_coord_t get_row_height(lv_obj_t * table, uint16_t row_id)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    lv_point_t txt_size;
    lv_coord_t txt_w;

    uint16_t row_start = row_id * ext->col_cnt;
    uint16_t cell;
    uint16_t col;
    lv_coord_t h_max = 0;

    for(cell = row_start, col = 0; cell < row_start + ext->col_cnt; cell++, col ++) {
        txt_w = ext->col_w[col] - 2 * ext->cell_style->body.padding.hor;
        lv_txt_get_size(&txt_size, ext->cell_data[cell], ext->cell_style->text.font,
                ext->cell_style->text.letter_space, ext->cell_style->text.line_space, txt_w, LV_TXT_FLAG_NONE);

        h_max = LV_MATH_MAX(txt_size.y, h_max);
    }

    printf("row:%d, h:%d\n", row_id, h_max);

    return h_max + 2 * ext->cell_style->body.padding.ver;
}

#endif
