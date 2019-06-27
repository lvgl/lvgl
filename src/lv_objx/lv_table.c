/**
 * @file lv_table.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_table.h"
#if LV_USE_TABLE != 0

#include "../lv_misc/lv_txt.h"
#include "../lv_misc/lv_math.h"
#include "../lv_draw/lv_draw_label.h"
#include "../lv_themes/lv_theme.h"

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
static lv_signal_cb_t ancestor_signal;
static lv_design_cb_t ancestor_scrl_design;

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
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_table);
    if(ancestor_scrl_design == NULL) ancestor_scrl_design = lv_obj_get_design_cb(new_table);

    /*Initialize the allocated 'ext' */
    ext->cell_data     = NULL;
    ext->cell_style[0] = &lv_style_plain;
    ext->cell_style[1] = &lv_style_plain;
    ext->cell_style[2] = &lv_style_plain;
    ext->cell_style[3] = &lv_style_plain;
    ext->col_cnt       = 0;
    ext->row_cnt       = 0;

    uint16_t i;
    for(i = 0; i < LV_TABLE_COL_MAX; i++) {
        ext->col_w[i] = LV_DPI;
    }

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(new_table, lv_table_signal);
    lv_obj_set_design_cb(new_table, lv_table_design);

    /*Init the new table table*/
    if(copy == NULL) {
        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_table_set_style(new_table, LV_TABLE_STYLE_BG, th->style.table.bg);
            lv_table_set_style(new_table, LV_TABLE_STYLE_CELL1, th->style.table.cell);
            lv_table_set_style(new_table, LV_TABLE_STYLE_CELL2, th->style.table.cell);
            lv_table_set_style(new_table, LV_TABLE_STYLE_CELL3, th->style.table.cell);
            lv_table_set_style(new_table, LV_TABLE_STYLE_CELL4, th->style.table.cell);
        } else {
            lv_table_set_style(new_table, LV_TABLE_STYLE_BG, &lv_style_plain_color);
        }
        lv_obj_set_click(new_table, false); /*Can be removed if click support is added*/
    }
    /*Copy an existing table*/
    else {
        lv_table_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->cell_style[0]        = copy_ext->cell_style[0];
        ext->cell_style[1]        = copy_ext->cell_style[1];
        ext->cell_style[2]        = copy_ext->cell_style[2];
        ext->cell_style[3]        = copy_ext->cell_style[3];
        ext->col_cnt              = copy_ext->col_cnt;
        ext->row_cnt              = copy_ext->row_cnt;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_table);
    }

    LV_LOG_INFO("table created");

    return new_table;
}

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
void lv_table_set_cell_value(lv_obj_t * table, uint16_t row, uint16_t col, const char * txt)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(row >= ext->row_cnt || col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_value: invalid row or column");
        return;
    }
    uint32_t cell = row * ext->col_cnt + col;
    lv_table_cell_format_t format;

    /*Save the format byte*/
    if(ext->cell_data[cell]) {
        format.format_byte = ext->cell_data[cell][0];
    }
    /*Initialize the format byte*/
    else {
        format.s.align       = LV_LABEL_ALIGN_LEFT;
        format.s.right_merge = 0;
        format.s.type        = 0;
        format.s.crop        = 0;
    }

    ext->cell_data[cell] = lv_mem_realloc(ext->cell_data[cell], strlen(txt) + 2); /*+1: trailing '\0; +1: format byte*/
    strcpy(ext->cell_data[cell] + 1, txt);                                        /*Leave the format byte*/
    ext->cell_data[cell][0] = format.format_byte;
    refr_size(table);
}

/**
 * Set the number of rows
 * @param table table pointer to a Table object
 * @param row_cnt number of rows
 */
void lv_table_set_row_cnt(lv_obj_t * table, uint16_t row_cnt)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    uint16_t old_row_cnt = ext->row_cnt;
    ext->row_cnt         = row_cnt;

    if(ext->row_cnt > 0 && ext->col_cnt > 0) {
        ext->cell_data = lv_mem_realloc(ext->cell_data, ext->row_cnt * ext->col_cnt * sizeof(char *));

        /*Initilize the new fields*/
        if(old_row_cnt < row_cnt) {
            uint16_t old_cell_cnt = old_row_cnt * ext->col_cnt;
            uint32_t new_cell_cnt = ext->col_cnt * ext->row_cnt;
            memset(&ext->cell_data[old_cell_cnt], 0, (new_cell_cnt - old_cell_cnt) * sizeof(ext->cell_data[0]));
        }
    } else {
        lv_mem_free(ext->cell_data);
        ext->cell_data = NULL;
    }

    refr_size(table);
}

/**
 * Set the number of columns
 * @param table table pointer to a Table object
 * @param col_cnt number of columns. Must be < LV_TABLE_COL_MAX
 */
void lv_table_set_col_cnt(lv_obj_t * table, uint16_t col_cnt)
{

    if(col_cnt >= LV_TABLE_COL_MAX) {
        LV_LOG_WARN("lv_table_set_col_cnt: too many columns. Must be < LV_TABLE_COL_MAX.");
        return;
    }

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    uint16_t old_col_cnt = ext->col_cnt;
    ext->col_cnt         = col_cnt;

    if(ext->row_cnt > 0 && ext->col_cnt > 0) {
        ext->cell_data = lv_mem_realloc(ext->cell_data, ext->row_cnt * ext->col_cnt * sizeof(char *));
        /*Initilize the new fields*/
        if(old_col_cnt < col_cnt) {
            uint16_t old_cell_cnt = old_col_cnt * ext->row_cnt;
            uint32_t new_cell_cnt = ext->col_cnt * ext->row_cnt;
            memset(&ext->cell_data[old_cell_cnt], 0, (new_cell_cnt - old_cell_cnt) * sizeof(ext->cell_data[0]));
        }

    } else {
        lv_mem_free(ext->cell_data);
        ext->cell_data = NULL;
    }
    refr_size(table);
}

/**
 * Set the width of a column
 * @param table table pointer to a Table object
 * @param col_id id of the column [0 .. LV_TABLE_COL_MAX -1]
 * @param w width of the column
 */
void lv_table_set_col_width(lv_obj_t * table, uint16_t col_id, lv_coord_t w)
{
    if(col_id >= LV_TABLE_COL_MAX) {
        LV_LOG_WARN("lv_table_set_col_width: too big 'col_id'. Must be < LV_TABLE_COL_MAX.");
        return;
    }

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    ext->col_w[col_id]   = w;
    refr_size(table);
}

/**
 * Set the text align in a cell
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param align LV_LABEL_ALIGN_LEFT or LV_LABEL_ALIGN_CENTER or LV_LABEL_ALIGN_RIGHT
 */
void lv_table_set_cell_align(lv_obj_t * table, uint16_t row, uint16_t col, lv_label_align_t align)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(row >= ext->row_cnt || col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_align: invalid row or column");
        return;
    }
    uint32_t cell = row * ext->col_cnt + col;

    if(ext->cell_data[cell] == NULL) {
        ext->cell_data[cell]    = lv_mem_alloc(2); /*+1: trailing '\0; +1: format byte*/
        ext->cell_data[cell][0] = 0;
        ext->cell_data[cell][1] = '\0';
    }

    lv_table_cell_format_t format;
    format.format_byte      = ext->cell_data[cell][0];
    format.s.align          = align;
    ext->cell_data[cell][0] = format.format_byte;
}

/**
 * Set the type of a cell.
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param type 1,2,3 or 4. The cell style will be chosen accordingly.
 */
void lv_table_set_cell_type(lv_obj_t * table, uint16_t row, uint16_t col, uint8_t type)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(row >= ext->row_cnt || col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_type: invalid row or column");
        return;
    }
    uint32_t cell = row * ext->col_cnt + col;

    if(ext->cell_data[cell] == NULL) {
        ext->cell_data[cell]    = lv_mem_alloc(2); /*+1: trailing '\0; +1: format byte*/
        ext->cell_data[cell][0] = 0;
        ext->cell_data[cell][1] = '\0';
    }

    if(type > 0) type--; /*User gives 1,2,3,4 but easier to handle 0, 1, 2, 3*/
    if(type >= LV_TABLE_CELL_STYLE_CNT) type = LV_TABLE_CELL_STYLE_CNT - 1;

    lv_table_cell_format_t format;
    format.format_byte      = ext->cell_data[cell][0];
    format.s.type           = type;
    ext->cell_data[cell][0] = format.format_byte;
}

/**
 * Set the cell crop. (Don't adjust the height of the cell according to its content)
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param crop true: crop the cell content; false: set the cell height to the content.
 */
void lv_table_set_cell_crop(lv_obj_t * table, uint16_t row, uint16_t col, bool crop)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(row >= ext->row_cnt || col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_crop: invalid row or column");
        return;
    }
    uint32_t cell = row * ext->col_cnt + col;

    if(ext->cell_data[cell] == NULL) {
        ext->cell_data[cell]    = lv_mem_alloc(2); /*+1: trailing '\0; +1: format byte*/
        ext->cell_data[cell][0] = 0;
        ext->cell_data[cell][1] = '\0';
    }

    lv_table_cell_format_t format;
    format.format_byte      = ext->cell_data[cell][0];
    format.s.crop           = crop;
    ext->cell_data[cell][0] = format.format_byte;
}

/**
 * Merge a cell with the right neighbor. The value of the cell to the right won't be displayed.
 * @param table table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param en true: merge right; false: don't merge right
 */
void lv_table_set_cell_merge_right(lv_obj_t * table, uint16_t row, uint16_t col, bool en)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(row >= ext->row_cnt || col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_merge_right: invalid row or column");
        return;
    }

    uint32_t cell = row * ext->col_cnt + col;

    if(ext->cell_data[cell] == NULL) {
        ext->cell_data[cell]    = lv_mem_alloc(2); /*+1: trailing '\0; +1: format byte*/
        ext->cell_data[cell][0] = 0;
        ext->cell_data[cell][1] = '\0';
    }

    lv_table_cell_format_t format;
    format.format_byte      = ext->cell_data[cell][0];
    format.s.right_merge    = en ? 1 : 0;
    ext->cell_data[cell][0] = format.format_byte;
    refr_size(table);
}

/**
 * Set a style of a table.
 * @param table pointer to table object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_table_set_style(lv_obj_t * table, lv_table_style_t type, const lv_style_t * style)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);

    switch(type) {
        case LV_TABLE_STYLE_BG:
            lv_obj_set_style(table, style);
            refr_size(table);
            break;
        case LV_TABLE_STYLE_CELL1:
            ext->cell_style[0] = style;
            refr_size(table);
            break;
        case LV_TABLE_STYLE_CELL2:
            ext->cell_style[1] = style;
            refr_size(table);
            break;
        case LV_TABLE_STYLE_CELL3:
            ext->cell_style[2] = style;
            refr_size(table);
            break;
        case LV_TABLE_STYLE_CELL4:
            ext->cell_style[3] = style;
            refr_size(table);
            break;
    }
}

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
const char * lv_table_get_cell_value(lv_obj_t * table, uint16_t row, uint16_t col)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(row >= ext->row_cnt || col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_value: invalid row or column");
        return "";
    }
    uint32_t cell = row * ext->col_cnt + col;

    if(ext->cell_data[cell] == NULL) return "";

    return &ext->cell_data[cell][1]; /*Skip the format byte*/
}

/**
 * Get the number of rows.
 * @param table table pointer to a Table object
 * @return number of rows.
 */
uint16_t lv_table_get_row_cnt(lv_obj_t * table)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    return ext->row_cnt;
}

/**
 * Get the number of columns.
 * @param table table pointer to a Table object
 * @return number of columns.
 */
uint16_t lv_table_get_col_cnt(lv_obj_t * table)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    return ext->col_cnt;
}

/**
 * Get the width of a column
 * @param table table pointer to a Table object
 * @param col_id id of the column [0 .. LV_TABLE_COL_MAX -1]
 * @return width of the column
 */
lv_coord_t lv_table_get_col_width(lv_obj_t * table, uint16_t col_id)
{
    if(col_id >= LV_TABLE_COL_MAX) {
        LV_LOG_WARN("lv_table_set_col_width: too big 'col_id'. Must be < LV_TABLE_COL_MAX.");
        return 0;
    }

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    return ext->col_w[col_id];
}

/**
 * Get the text align of a cell
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return LV_LABEL_ALIGN_LEFT (default in case of error) or LV_LABEL_ALIGN_CENTER or
 * LV_LABEL_ALIGN_RIGHT
 */
lv_label_align_t lv_table_get_cell_align(lv_obj_t * table, uint16_t row, uint16_t col)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(row >= ext->row_cnt || col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_align: invalid row or column");
        return LV_LABEL_ALIGN_LEFT; /*Just return with something*/
    }
    uint32_t cell = row * ext->col_cnt + col;

    if(ext->cell_data[cell] == NULL)
        return LV_LABEL_ALIGN_LEFT; /*Just return with something*/
    else {
        lv_table_cell_format_t format;
        format.format_byte = ext->cell_data[cell][0];
        return format.s.align;
    }
}

/**
 * Get the type of a cell
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return 1,2,3 or 4
 */
lv_label_align_t lv_table_get_cell_type(lv_obj_t * table, uint16_t row, uint16_t col)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(row >= ext->row_cnt || col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_get_cell_type: invalid row or column");
        return 1; /*Just return with something*/
    }
    uint32_t cell = row * ext->col_cnt + col;

    if(ext->cell_data[cell] == NULL)
        return 1; /*Just return with something*/
    else {
        lv_table_cell_format_t format;
        format.format_byte = ext->cell_data[cell][0];
        return format.s.type + 1; /*0,1,2,3 is stored but user sees 1,2,3,4*/
    }
}

/**
 * Get the crop property of a cell
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return true: text crop enabled; false: disabled
 */
lv_label_align_t lv_table_get_cell_crop(lv_obj_t * table, uint16_t row, uint16_t col)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(row >= ext->row_cnt || col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_get_cell_crop: invalid row or column");
        return false; /*Just return with something*/
    }
    uint32_t cell = row * ext->col_cnt + col;

    if(ext->cell_data[cell] == NULL)
        return false; /*Just return with something*/
    else {
        lv_table_cell_format_t format;
        format.format_byte = ext->cell_data[cell][0];
        return format.s.crop;
    }
}

/**
 * Get the cell merge attribute.
 * @param table table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return true: merge right; false: don't merge right
 */
bool lv_table_get_cell_merge_right(lv_obj_t * table, uint16_t row, uint16_t col)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(row >= ext->row_cnt || col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_get_cell_merge_right: invalid row or column");
        return false;
    }

    uint32_t cell = row * ext->col_cnt + col;

    if(ext->cell_data[cell] == NULL)
        return false;
    else {
        lv_table_cell_format_t format;
        format.format_byte = ext->cell_data[cell][0];
        return format.s.right_merge ? true : false;
    }
}

/**
 * Get style of a table.
 * @param table pointer to table object
 * @param type which style should be get
 * @return style pointer to the style
 */
const lv_style_t * lv_table_get_style(const lv_obj_t * table, lv_table_style_t type)
{
    lv_table_ext_t * ext     = lv_obj_get_ext_attr(table);
    const lv_style_t * style = NULL;

    switch(type) {
        case LV_TABLE_STYLE_BG: style = lv_obj_get_style(table); break;
        case LV_TABLE_STYLE_CELL1: style = ext->cell_style[0]; break;
        case LV_TABLE_STYLE_CELL2: style = ext->cell_style[1]; break;
        case LV_TABLE_STYLE_CELL3: style = ext->cell_style[2]; break;
        case LV_TABLE_STYLE_CELL4: style = ext->cell_style[3]; break;
        default: return NULL;
    }

    return style;
}

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

        lv_table_ext_t * ext        = lv_obj_get_ext_attr(table);
        const lv_style_t * bg_style = lv_obj_get_style(table);
        const lv_style_t * cell_style;
        lv_coord_t h_row;
        lv_point_t txt_size;
        lv_area_t cell_area;
        lv_area_t txt_area;
        lv_txt_flag_t txt_flags;
        lv_opa_t opa_scale = lv_obj_get_opa_scale(table);

        uint16_t col;
        uint16_t row;
        uint16_t cell = 0;

        cell_area.y2 = table->coords.y1 + bg_style->body.padding.top;
        for(row = 0; row < ext->row_cnt; row++) {
            h_row = get_row_height(table, row);

            cell_area.y1 = cell_area.y2;
            cell_area.y2 = cell_area.y1 + h_row;

            cell_area.x2 = table->coords.x1 + bg_style->body.padding.left;

            for(col = 0; col < ext->col_cnt; col++) {

                lv_table_cell_format_t format;
                if(ext->cell_data[cell]) {
                    format.format_byte = ext->cell_data[cell][0];
                } else {
                    format.s.right_merge = 0;
                    format.s.align       = LV_LABEL_ALIGN_LEFT;
                    format.s.type        = 0;
                    format.s.crop        = 1;
                }

                cell_style   = ext->cell_style[format.s.type];
                cell_area.x1 = cell_area.x2;
                cell_area.x2 = cell_area.x1 + ext->col_w[col];

                uint16_t col_merge = 0;
                for(col_merge = 0; col_merge + col < ext->col_cnt - 1; col_merge++) {

                    if(ext->cell_data[cell + col_merge] != NULL) {
                        format.format_byte = ext->cell_data[cell + col_merge][0];
                        if(format.s.right_merge)
                            cell_area.x2 += ext->col_w[col + col_merge + 1];
                        else
                            break;
                    } else {
                        break;
                    }
                }

                lv_draw_rect(&cell_area, mask, cell_style, opa_scale);

                if(ext->cell_data[cell]) {

                    txt_area.x1 = cell_area.x1 + cell_style->body.padding.left;
                    txt_area.x2 = cell_area.x2 - cell_style->body.padding.right;
                    txt_area.y1 = cell_area.y1 + cell_style->body.padding.top;
                    txt_area.y2 = cell_area.y2 - cell_style->body.padding.bottom;
                    /*Align the content to the middle if not cropped*/
                    if(format.s.crop == 0) {
                        txt_flags = LV_TXT_FLAG_NONE;
                    } else {
                        txt_flags = LV_TXT_FLAG_EXPAND;
                    }

                    lv_txt_get_size(&txt_size, ext->cell_data[cell] + 1, cell_style->text.font,
                                    cell_style->text.letter_space, cell_style->text.line_space,
                                    lv_area_get_width(&txt_area), txt_flags);

                    /*Align the content to the middle if not cropped*/
                    if(format.s.crop == 0) {
                        txt_area.y1 = cell_area.y1 + h_row / 2 - txt_size.y / 2;
                        txt_area.y2 = cell_area.y1 + h_row / 2 + txt_size.y / 2;
                    }

                    switch(format.s.align) {
                        default:
                        case LV_LABEL_ALIGN_LEFT: txt_flags |= LV_TXT_FLAG_NONE; break;
                        case LV_LABEL_ALIGN_RIGHT: txt_flags |= LV_TXT_FLAG_RIGHT; break;
                        case LV_LABEL_ALIGN_CENTER: txt_flags |= LV_TXT_FLAG_CENTER; break;
                    }

                    lv_area_t label_mask;
                    bool label_mask_ok;
                    label_mask_ok = lv_area_intersect(&label_mask, mask, &cell_area);
                    if(label_mask_ok) {
                        lv_draw_label(&txt_area, &label_mask, cell_style, opa_scale, ext->cell_data[cell] + 1,
                                      txt_flags, NULL, -1, -1, NULL);
                    }
                    /*Draw lines after '\n's*/
                    lv_point_t p1;
                    lv_point_t p2;
                    p1.x = cell_area.x1;
                    p2.x = cell_area.x2;
                    uint16_t i;
                    for(i = 1; ext->cell_data[cell][i] != '\0'; i++) {
                        if(ext->cell_data[cell][i] == '\n') {
                            ext->cell_data[cell][i] = '\0';
                            lv_txt_get_size(&txt_size, ext->cell_data[cell] + 1, cell_style->text.font,
                                            cell_style->text.letter_space, cell_style->text.line_space,
                                            lv_area_get_width(&txt_area), txt_flags);

                            p1.y = txt_area.y1 + txt_size.y + cell_style->text.line_space / 2;
                            p2.y = txt_area.y1 + txt_size.y + cell_style->text.line_space / 2;
                            lv_draw_line(&p1, &p2, mask, cell_style, opa_scale);

                            ext->cell_data[cell][i] = '\n';
                        }
                    }
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
        /*Free the cell texts*/
        lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
        uint16_t cell;
        for(cell = 0; cell < ext->col_cnt * ext->row_cnt; cell++) {
            if(ext->cell_data[cell]) {
                lv_mem_free(ext->cell_data[cell]);
                ext->cell_data[cell] = NULL;
            }
        }
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
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
    for(i = 0; i < ext->col_cnt; i++) {
        w += ext->col_w[i];
    }
    for(i = 0; i < ext->row_cnt; i++) {
        h += get_row_height(table, i);
    }

    const lv_style_t * bg_style = lv_obj_get_style(table);

    w += bg_style->body.padding.left + bg_style->body.padding.right;
    h += bg_style->body.padding.top + bg_style->body.padding.bottom;

    lv_obj_set_size(table, w + 1, h + 1);
    lv_obj_invalidate(table);
}

static lv_coord_t get_row_height(lv_obj_t * table, uint16_t row_id)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    lv_point_t txt_size;
    lv_coord_t txt_w;
    const lv_style_t * cell_style;

    uint16_t row_start = row_id * ext->col_cnt;
    uint16_t cell;
    uint16_t col;
    lv_coord_t h_max = lv_font_get_line_height(ext->cell_style[0]->text.font) + ext->cell_style[0]->body.padding.top +
                       ext->cell_style[0]->body.padding.bottom;

    for(cell = row_start, col = 0; cell < row_start + ext->col_cnt; cell++, col++) {
        if(ext->cell_data[cell] != NULL) {

            txt_w              = ext->col_w[col];
            uint16_t col_merge = 0;
            for(col_merge = 0; col_merge + col < ext->col_cnt - 1; col_merge++) {

                if(ext->cell_data[cell + col_merge] != NULL) {
                    lv_table_cell_format_t format;
                    format.format_byte = ext->cell_data[cell + col_merge][0];
                    if(format.s.right_merge)
                        txt_w += ext->col_w[col + col_merge + 1];
                    else
                        break;
                } else {
                    break;
                }
            }

            lv_table_cell_format_t format;
            format.format_byte = ext->cell_data[cell][0];
            cell_style         = ext->cell_style[format.s.type];

            /*With text crop assume 1 line*/
            if(format.s.crop) {
                h_max = LV_MATH_MAX(lv_font_get_line_height(cell_style->text.font) + cell_style->body.padding.top +
                                        cell_style->body.padding.bottom,
                                    h_max);
            }
            /*Without text crop calculate the height of the text in the cell*/
            else {
                txt_w -= cell_style->body.padding.left + cell_style->body.padding.right;

                lv_txt_get_size(&txt_size, ext->cell_data[cell] + 1, cell_style->text.font,
                                cell_style->text.letter_space, cell_style->text.line_space, txt_w, LV_TXT_FLAG_NONE);

                h_max = LV_MATH_MAX(txt_size.y + cell_style->body.padding.top + cell_style->body.padding.bottom, h_max);
                cell += col_merge;
                col += col_merge;
            }
        }
    }

    return h_max;
}

#endif
