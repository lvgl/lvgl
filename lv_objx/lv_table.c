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
static void refr_size(lv_obj_t * table);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;
static lv_design_func_t ancestor_scrl_design;

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
    ext->cell_data = NULL;
    ext->cell_style = &lv_style_pretty;
    ext->col_cnt = 0;
    ext->row_cnt = 0;
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

void lv_table_set_cell_value(lv_obj_t * table, uint16_t row, uint16_t col, const char * txt)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(row >= ext->row_cnt || col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_value: invalid row or column");
        return;
    }
    uint32_t cell = row * ext->col_cnt + col;
    ext->cell_data[cell] = lv_mem_realloc(ext->cell_data[cell], strlen(txt) + 2);   /*+1: trailing '\0; +1: format byte*/
    strcpy(ext->cell_data[cell] + 1, txt);   /*Leave the format byte*/
    refr_size(table);
}

void lv_table_set_cell_format(lv_obj_t * table, uint16_t row, uint16_t col, lv_table_cell_align_t align)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
     if(row >= ext->row_cnt || col >= ext->col_cnt) {
         LV_LOG_WARN("lv_table_set_cell_format: invalid row or column");
         return;
     }
     uint32_t cell = row * ext->col_cnt + col;

     if(ext->cell_data[cell] == NULL) {
         ext->cell_data[cell] = lv_mem_alloc(2);        /*+1: trailing '\0; +1: format byte*/
         ext->cell_data[1] = '\0';
     }

     lv_table_cell_format_t format;
     format.format_byte = ext->cell_data[cell][0];
     format.align = align;
     ext->cell_data[cell][0] = format.format_byte;
}

void lv_table_set_cell_merge_right(lv_obj_t * table, uint16_t row, uint16_t col, bool en)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
     if(row >= ext->row_cnt || col >= ext->col_cnt) {
         LV_LOG_WARN("lv_table_set_cell_merge_right: invalid row or column");
         return;
     }

     uint32_t cell = row * ext->col_cnt + col;

     if(ext->cell_data[cell] == NULL) {
         ext->cell_data[cell] = lv_mem_alloc(2);        /*+1: trailing '\0; +1: format byte*/
         ext->cell_data[1] = '\0';
     }

     lv_table_cell_format_t format;
     format.format_byte = ext->cell_data[cell][0];
     format.right_merge = en ? 1 : 0;
     ext->cell_data[cell][0] = format.format_byte;
}



void lv_table_set_row_cnt(lv_obj_t * table, uint16_t row_cnt)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    ext->row_cnt = row_cnt;

    if(ext->row_cnt > 0 && ext->col_cnt > 0) {
        ext->cell_data = lv_mem_realloc(ext->cell_data, ext->row_cnt * ext->col_cnt * sizeof(char*));
    }
    else {
        lv_mem_free(ext->cell_data);
        ext->cell_data = NULL;
    }

    refr_size(table);
}

void lv_table_set_col_cnt(lv_obj_t * table, uint16_t col_cnt)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    ext->col_cnt = col_cnt;

    if(ext->row_cnt > 0 && ext->col_cnt > 0) {
        ext->cell_data = lv_mem_realloc(ext->cell_data, ext->row_cnt * ext->col_cnt * sizeof(char*));
    }
    else {
        lv_mem_free(ext->cell_data);
        ext->cell_data = NULL;
    }
    refr_size(table);
}


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
        lv_style_t * bg_style = lv_obj_get_style(table);
        lv_coord_t h_row;
        lv_point_t txt_size;
        lv_area_t cell_area;
        lv_area_t txt_area;
        lv_txt_flag_t txt_flags;

        uint16_t col;
        uint16_t row;
        uint16_t cell = 0;

        cell_area.y2 = table->coords.y1 + bg_style->body.padding.ver;
        for(row = 0; row < ext->row_cnt; row++) {
            h_row = get_row_height(table, row);

            cell_area.y1 = cell_area.y2;
            cell_area.y2 = cell_area.y1 + h_row;

            cell_area.x2 = table->coords.x1 + bg_style->body.padding.hor;

            for(col = 0; col < ext->col_cnt; col++) {

                cell_area.x1 = cell_area.x2;
                cell_area.x2 = cell_area.x1 + ext->col_w[col];

                uint16_t col_merge = 0;
                for(col_merge = 0; col_merge + col < ext->col_cnt - 1; col_merge ++) {

                    if(ext->cell_data[cell + col_merge] != NULL) {
                        lv_table_cell_format_t format;
                        format.format_byte = ext->cell_data[cell + col_merge][0];
                        if(format.right_merge) cell_area.x2 += ext->col_w[col + col_merge + 1];
                        else break;
                    } else {
                        break;
                    }
                }


                lv_draw_rect(&cell_area, mask, ext->cell_style, LV_OPA_COVER);

                if(ext->cell_data[cell]) {
                    txt_area.x1 = cell_area.x1 + ext->cell_style->body.padding.hor;
                    txt_area.x2 = cell_area.x2 - ext->cell_style->body.padding.hor;
                    txt_area.y1 = cell_area.y1 + ext->cell_style->body.padding.ver;
                    txt_area.y2 = cell_area.y2 - ext->cell_style->body.padding.ver;

                    lv_table_cell_format_t format;
                    format.format_byte = ext->cell_data[cell][0];

                    switch(format.align) {
                    case LV_TABLE_CELL_ALIGN_LEFT:
                        txt_flags = LV_TXT_FLAG_NONE;
                        break;
                    case LV_TABLE_CELL_ALIGN_RIGHT:
                        txt_flags = LV_TXT_FLAG_RIGHT;
                        break;
                    case LV_TABLE_CELL_ALIGN_CENTER:
                        txt_flags = LV_TXT_FLAG_CENTER;
                        break;
                    }

                    lv_txt_get_size(&txt_size, ext->cell_data[cell] + 1, ext->cell_style->text.font,
                            ext->cell_style->text.letter_space, ext->cell_style->text.line_space, lv_area_get_width(&txt_area), txt_flags);

                    lv_draw_label(&txt_area, mask, ext->cell_style, LV_OPA_COVER, ext->cell_data[cell] + 1, txt_flags, NULL);
                }

                cell += col_merge + 1;
                col += col_merge;
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

static void refr_size(lv_obj_t * table)
{
    lv_coord_t h = 0;
    lv_coord_t w = 0;

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);

    uint16_t i;
    for(i= 0; i < ext->col_cnt; i++) {
        w += ext->col_w[i];
    }
    for(i= 0; i < ext->row_cnt; i++) {
        h += get_row_height(table, i);
    }

    lv_style_t * bg_style = lv_obj_get_style(table);

    w += bg_style->body.padding.hor * 2;
    h += bg_style->body.padding.ver * 2;

    lv_obj_set_size(table, w, h);
    lv_obj_invalidate(table);
}

static lv_coord_t get_row_height(lv_obj_t * table, uint16_t row_id)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    lv_point_t txt_size;
    lv_coord_t txt_w;

    uint16_t row_start = row_id * ext->col_cnt;
    uint16_t cell;
    uint16_t col;
    lv_coord_t h_max = lv_font_get_height(ext->cell_style->text.font);

    for(cell = row_start, col = 0; cell < row_start + ext->col_cnt; cell++, col ++) {
        if(ext->cell_data[cell] != NULL) {

            txt_w = ext->col_w[col];
            uint16_t col_merge = 0;
            for(col_merge = 0; col_merge + col < ext->col_cnt - 1; col_merge ++) {

                if(ext->cell_data[cell + col_merge] != NULL) {
                    lv_table_cell_format_t format;
                    format.format_byte = ext->cell_data[cell + col_merge][0];
                    if(format.right_merge) txt_w += ext->col_w[col + col_merge + 1];
                    else break;
                } else {
                    break;
                }
            }

            txt_w -= 2 * ext->cell_style->body.padding.hor;
            lv_txt_get_size(&txt_size, ext->cell_data[cell] + 1, ext->cell_style->text.font,
                    ext->cell_style->text.letter_space, ext->cell_style->text.line_space, txt_w, LV_TXT_FLAG_NONE);

            h_max = LV_MATH_MAX(txt_size.y, h_max);
        }
    }

    return h_max + 2 * ext->cell_style->body.padding.ver;
}

#endif
