/**
 * @file lv_table.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_table.h"
#if LV_USE_TABLE != 0

#include "../lv_core/lv_indev.h"
#include "../lv_misc/lv_debug.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_misc/lv_txt_ap.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_printf.h"
#include "../lv_draw/lv_draw_label.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_table"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_table_design(lv_obj_t * table, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_table_signal(lv_obj_t * table, lv_signal_t sign, void * param);
static lv_style_list_t * lv_table_get_style(lv_obj_t * table, uint8_t part);
static lv_coord_t get_row_height(lv_obj_t * table, uint16_t row_id, const lv_font_t ** font,
                                 lv_style_int_t * letter_space, lv_style_int_t * line_space,
                                 lv_style_int_t * cell_left, lv_style_int_t * cell_right, lv_style_int_t * cell_top, lv_style_int_t * cell_bottom);
static void refr_size(lv_obj_t * table);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_design_cb_t ancestor_design;

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
    lv_obj_t * table = lv_obj_create(par, copy);
    LV_ASSERT_MEM(table);
    if(table == NULL) return NULL;

    /*Allocate the table type specific extended data*/
    lv_table_ext_t * ext = lv_obj_allocate_ext_attr(table, sizeof(lv_table_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(table);
        return NULL;
    }

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(table);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(table);

    /*Initialize the allocated 'ext' */
    ext->cell_data     = NULL;
    ext->col_cnt       = 0;
    ext->row_cnt       = 0;
    ext->row_h         = NULL;
    ext->cell_types    = 1;

    uint16_t i;
    for(i = 0; i < LV_TABLE_CELL_STYLE_CNT; i++) {
        lv_style_list_init(&ext->cell_style[i]);
    }

    for(i = 0; i < LV_TABLE_COL_MAX; i++) {
        ext->col_w[i] = LV_DPI;
    }

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(table, lv_table_signal);
    lv_obj_set_design_cb(table, lv_table_design);

    /*Init the new table table*/
    if(copy == NULL) {
        lv_theme_apply(table, LV_THEME_TABLE);
    }
    /*Copy an existing table*/
    else {
        lv_table_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        for(i = 0; i < LV_TABLE_CELL_STYLE_CNT; i++) {
            lv_style_list_copy(&ext->cell_style[i], &copy_ext->cell_style[i]);
        }
        lv_table_set_row_cnt(table, copy_ext->row_cnt);
        lv_table_set_col_cnt(table, copy_ext->col_cnt);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(table, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    LV_LOG_INFO("table created");

    return table;
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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);
    LV_ASSERT_NULL(txt);

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_value: invalid column");
        return;
    }

    /*Auto expand*/
    if(row >= ext->row_cnt) {
        lv_table_set_row_cnt(table, row + 1);
    }

    uint32_t cell = row * ext->col_cnt + col;
    lv_table_cell_format_t format;

    /*Save the format byte*/
    if(ext->cell_data[cell]) {
        format.format_byte = ext->cell_data[cell][0];
    }
    /*Initialize the format byte*/
    else {
        lv_bidi_dir_t base_dir = lv_obj_get_base_dir(table);
        if(base_dir == LV_BIDI_DIR_LTR) format.s.align = LV_LABEL_ALIGN_LEFT;
        else if(base_dir == LV_BIDI_DIR_RTL) format.s.align = LV_LABEL_ALIGN_RIGHT;
        else if(base_dir == LV_BIDI_DIR_AUTO)
#if LV_USE_BIDI
            format.s.align = _lv_bidi_detect_base_dir(txt);
#else
            format.s.align = LV_LABEL_ALIGN_LEFT;
#endif
        format.s.right_merge = 0;
        format.s.type        = 0;
        format.s.crop        = 0;
    }

#if LV_USE_ARABIC_PERSIAN_CHARS
    /*Get the size of the Arabic text and process it*/
    size_t len_ap = _lv_txt_ap_calc_bytes_cnt(txt);
    ext->cell_data[cell] = lv_mem_realloc(ext->cell_data[cell], len_ap + 1);
    LV_ASSERT_MEM(ext->cell_data[cell]);
    if(ext->cell_data[cell] == NULL) return;

    _lv_txt_ap_proc(txt, &ext->cell_data[cell][1]);
#else
    ext->cell_data[cell] = lv_mem_realloc(ext->cell_data[cell], strlen(txt) + 2); /*+1: trailing '\0; +1: format byte*/
    LV_ASSERT_MEM(ext->cell_data[cell]);
    if(ext->cell_data[cell] == NULL) return;

    strcpy(ext->cell_data[cell] + 1, txt);  /*+1 to skip the format byte*/
#endif

    ext->cell_data[cell][0] = format.format_byte;
    refr_size(table);
}


/**
 * Set the value of a cell.  Memory will be allocated to store the text by the table.
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param fmt `printf`-like format
 */
void lv_table_set_cell_value_fmt(lv_obj_t * table, uint16_t row, uint16_t col, const char * fmt, ...)
{
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);
    LV_ASSERT_STR(fmt);

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_value: invalid column");
        return;
    }

    /*Auto expand*/
    if(row >= ext->row_cnt) {
        lv_table_set_row_cnt(table, row + 1);
    }

    uint32_t cell = row * ext->col_cnt + col;
    lv_table_cell_format_t format;

    /*Save the format byte*/
    if(ext->cell_data[cell]) {
        format.format_byte = ext->cell_data[cell][0];
    }
    /*Initialize the format byte*/
    else {
        lv_bidi_dir_t base_dir = lv_obj_get_base_dir(table);
        if(base_dir == LV_BIDI_DIR_LTR) format.s.align = LV_LABEL_ALIGN_LEFT;
        else if(base_dir == LV_BIDI_DIR_RTL) format.s.align = LV_LABEL_ALIGN_RIGHT;
        else if(base_dir == LV_BIDI_DIR_AUTO)
#if LV_USE_BIDI
            format.s.align = _lv_bidi_detect_base_dir(fmt);
#else
            format.s.align = LV_LABEL_ALIGN_LEFT;
#endif
        format.s.right_merge = 0;
        format.s.type        = 0;
        format.s.crop        = 0;
    }

    va_list ap, ap2;
    va_start(ap, fmt);
    va_copy(ap2, ap);

    /*Allocate space for the new text by using trick from C99 standard section 7.19.6.12 */
    uint32_t len = lv_vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);

#if LV_USE_ARABIC_PERSIAN_CHARS
    /*Put together the text according to the format string*/
    char * raw_txt = _lv_mem_buf_get(len + 1);
    LV_ASSERT_MEM(raw_txt);
    if(raw_txt == NULL) {
        va_end(ap2);
        return;
    }

    lv_vsnprintf(raw_txt, len + 1, fmt, ap2);

    /*Get the size of the Arabic text and process it*/
    size_t len_ap = _lv_txt_ap_calc_bytes_cnt(raw_txt);
    ext->cell_data[cell] = lv_mem_realloc(ext->cell_data[cell], len_ap + 1);
    LV_ASSERT_MEM(ext->cell_data[cell]);
    if(ext->cell_data[cell] == NULL) {
        va_end(ap2);
        return;
    }
    _lv_txt_ap_proc(raw_txt, &ext->cell_data[cell][1]);

    _lv_mem_buf_release(raw_txt);
#else
    ext->cell_data[cell] = lv_mem_realloc(ext->cell_data[cell], len + 2); /*+1: trailing '\0; +1: format byte*/
    LV_ASSERT_MEM(ext->cell_data[cell]);
    if(ext->cell_data[cell] == NULL) {
        va_end(ap2);
        return;
    }

    ext->cell_data[cell][len + 1] = 0; /* Ensure NULL termination */

    lv_vsnprintf(&ext->cell_data[cell][1], len + 1, fmt, ap2);
#endif

    va_end(ap2);

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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    uint16_t old_row_cnt = ext->row_cnt;
    ext->row_cnt         = row_cnt;

    if(ext->row_cnt > 0) {
        ext->row_h = lv_mem_realloc(ext->row_h, ext->row_cnt * sizeof(ext->row_h[0]));
        LV_ASSERT_MEM(ext->row_h);
        if(ext->row_h == NULL) return;
    }
    else {
        lv_mem_free(ext->row_h);
        ext->row_h = NULL;
    }

    if(ext->row_cnt > 0 && ext->col_cnt > 0) {
        ext->cell_data = lv_mem_realloc(ext->cell_data, ext->row_cnt * ext->col_cnt * sizeof(char *));
        LV_ASSERT_MEM(ext->cell_data);
        if(ext->cell_data == NULL) return;

        /*Initialize the new fields*/
        if(old_row_cnt < row_cnt) {
            uint16_t old_cell_cnt = old_row_cnt * ext->col_cnt;
            uint32_t new_cell_cnt = ext->col_cnt * ext->row_cnt;
            _lv_memset_00(&ext->cell_data[old_cell_cnt], (new_cell_cnt - old_cell_cnt) * sizeof(ext->cell_data[0]));
        }
    }
    else {
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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

    if(col_cnt >= LV_TABLE_COL_MAX) {
        LV_LOG_WARN("lv_table_set_col_cnt: too many columns. Must be < LV_TABLE_COL_MAX.");
        return;
    }

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    uint16_t old_col_cnt = ext->col_cnt;
    ext->col_cnt         = col_cnt;

    if(ext->row_cnt > 0 && ext->col_cnt > 0) {
        ext->cell_data = lv_mem_realloc(ext->cell_data, ext->row_cnt * ext->col_cnt * sizeof(char *));
        LV_ASSERT_MEM(ext->cell_data);
        if(ext->cell_data == NULL) return;

        /*Initialize the new fields*/
        if(old_col_cnt < col_cnt) {
            uint16_t old_cell_cnt = old_col_cnt * ext->row_cnt;
            uint32_t new_cell_cnt = ext->col_cnt * ext->row_cnt;
            _lv_memset_00(&ext->cell_data[old_cell_cnt], (new_cell_cnt - old_cell_cnt) * sizeof(ext->cell_data[0]));
        }

    }
    else {
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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_align: invalid column");
        return;
    }

    /*Auto expand*/
    if(row >= ext->row_cnt) {
        lv_table_set_row_cnt(table, row + 1);
    }
    uint32_t cell = row * ext->col_cnt + col;

    if(ext->cell_data[cell] == NULL) {
        ext->cell_data[cell]    = lv_mem_alloc(2); /*+1: trailing '\0; +1: format byte*/
        LV_ASSERT_MEM(ext->cell_data[cell]);
        if(ext->cell_data[cell] == NULL) return;

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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_type: invalid column");
        return;
    }

    /*Auto expand*/
    if(row >= ext->row_cnt) {
        lv_table_set_row_cnt(table, row + 1);
    }

    uint32_t cell = row * ext->col_cnt + col;

    if(ext->cell_data[cell] == NULL) {
        ext->cell_data[cell]    = lv_mem_alloc(2); /*+1: trailing '\0; +1: format byte*/
        LV_ASSERT_MEM(ext->cell_data[cell]);
        if(ext->cell_data[cell] == NULL) return;

        ext->cell_data[cell][0] = 0;
        ext->cell_data[cell][1] = '\0';
    }

    if(type > 0) type--; /*User gives 1,2,3,4 but easier to handle 0, 1, 2, 3*/
    if(type >= LV_TABLE_CELL_STYLE_CNT) type = LV_TABLE_CELL_STYLE_CNT - 1;

    lv_table_cell_format_t format;
    format.format_byte      = ext->cell_data[cell][0];
    format.s.type           = type;
    ext->cell_data[cell][0] = format.format_byte;

    ext->cell_types |= 1 << type;

    lv_obj_invalidate(table);
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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_crop: invalid column");
        return;
    }

    /*Auto expand*/
    if(row >= ext->row_cnt) {
        lv_table_set_row_cnt(table, row + 1);
    }

    uint32_t cell = row * ext->col_cnt + col;

    if(ext->cell_data[cell] == NULL) {
        ext->cell_data[cell]    = lv_mem_alloc(2); /*+1: trailing '\0; +1: format byte*/
        LV_ASSERT_MEM(ext->cell_data[cell]);
        if(ext->cell_data[cell] == NULL) return;

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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(col >= ext->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_merge_right: invalid column");
        return;
    }

    /*Auto expand*/
    if(row >= ext->row_cnt) {
        lv_table_set_row_cnt(table, row + 1);
    }

    uint32_t cell = row * ext->col_cnt + col;

    if(ext->cell_data[cell] == NULL) {
        ext->cell_data[cell]    = lv_mem_alloc(2); /*+1: trailing '\0; +1: format byte*/
        LV_ASSERT_MEM(ext->cell_data[cell]);
        if(ext->cell_data[cell] == NULL) return;

        ext->cell_data[cell][0] = 0;
        ext->cell_data[cell][1] = '\0';
    }

    lv_table_cell_format_t format;
    format.format_byte      = ext->cell_data[cell][0];
    format.s.right_merge    = en ? 1 : 0;
    ext->cell_data[cell][0] = format.format_byte;
    refr_size(table);
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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

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
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

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
 * Get the last pressed or being pressed cell
 * @param table pointer to a table object
 * @param row pointer to variable to store the pressed row
 * @param col pointer to variable to store the pressed column
 * @return LV_RES_OK: a valid pressed cell was found, LV_RES_INV: no valid cell is pressed
 */
lv_res_t lv_table_get_pressed_cell(lv_obj_t * table, uint16_t * row, uint16_t * col)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);

    lv_indev_type_t type = lv_indev_get_type(lv_indev_get_act());
    if(type != LV_INDEV_TYPE_POINTER && type != LV_INDEV_TYPE_BUTTON) {
        if(col) *col = 0xFFFF;
        if(row) *row = 0xFFFF;
        return LV_RES_INV;
    }

    lv_point_t p;
    lv_indev_get_point(lv_indev_get_act(), &p);

    lv_coord_t tmp;
    if(col) {
        lv_coord_t x = p.x;
        x -= table->coords.x1;
        x -= lv_obj_get_style_pad_left(table, LV_TABLE_PART_BG);
        *col = 0;
        tmp = 0;
        for(*col = 0; *col < ext->col_cnt; (*col)++) {
            tmp += ext->col_w[*col];
            if(x < tmp) break;
        }
    }

    if(row) {
        lv_coord_t y = p.y;
        y -= table->coords.y1;
        y -= lv_obj_get_style_pad_top(table, LV_TABLE_PART_BG);

        *row = 0;
        tmp = 0;

        for(*row = 0; *row < ext->row_cnt; (*row)++) {
            tmp += ext->row_h[*row];
            if(y < tmp) break;
        }
    }

    return LV_RES_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the tables
 * @param table pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_table_design(lv_obj_t * table, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_design(table, clip_area, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        /*Draw the background*/
        ancestor_design(table, clip_area, mode);

        lv_table_ext_t * ext        = lv_obj_get_ext_attr(table);

        lv_point_t txt_size;
        lv_area_t cell_area;
        lv_area_t txt_area;
        lv_txt_flag_t txt_flags;

        lv_style_int_t bg_top = lv_obj_get_style_pad_top(table, LV_TABLE_PART_BG);
        lv_style_int_t bg_bottom = lv_obj_get_style_pad_bottom(table, LV_TABLE_PART_BG);
        lv_style_int_t bg_left = lv_obj_get_style_pad_left(table, LV_TABLE_PART_BG);
        lv_style_int_t bg_right = lv_obj_get_style_pad_right(table, LV_TABLE_PART_BG);

        lv_draw_rect_dsc_t rect_dsc[LV_TABLE_CELL_STYLE_CNT];
        lv_draw_label_dsc_t label_dsc[LV_TABLE_CELL_STYLE_CNT];
        lv_draw_line_dsc_t line_dsc[LV_TABLE_CELL_STYLE_CNT];
        lv_style_int_t cell_left[LV_TABLE_CELL_STYLE_CNT];
        lv_style_int_t cell_right[LV_TABLE_CELL_STYLE_CNT];
        lv_style_int_t cell_top[LV_TABLE_CELL_STYLE_CNT];
        lv_style_int_t cell_bottom[LV_TABLE_CELL_STYLE_CNT];

        uint16_t i;
        for(i = 0; i < LV_TABLE_CELL_STYLE_CNT; i++) {
            if((ext->cell_types & (1 << i)) == 0) continue; /*Skip unused cell types*/
            lv_draw_rect_dsc_init(&rect_dsc[i]);
            lv_obj_init_draw_rect_dsc(table, LV_TABLE_PART_CELL1 + i, &rect_dsc[i]);

            lv_draw_label_dsc_init(&label_dsc[i]);
            lv_obj_init_draw_label_dsc(table, LV_TABLE_PART_CELL1 + i, &label_dsc[i]);

            lv_draw_line_dsc_init(&line_dsc[i]);
            lv_obj_init_draw_line_dsc(table, LV_TABLE_PART_CELL1 + i, &line_dsc[i]);

            cell_left[i] = lv_obj_get_style_pad_left(table, LV_TABLE_PART_CELL1 + i);
            cell_right[i] = lv_obj_get_style_pad_right(table, LV_TABLE_PART_CELL1 + i);
            cell_top[i] = lv_obj_get_style_pad_top(table, LV_TABLE_PART_CELL1 + i);
            cell_bottom[i] = lv_obj_get_style_pad_bottom(table, LV_TABLE_PART_CELL1 + i);
        }

        uint16_t col;
        uint16_t row;
        uint16_t cell = 0;

        bool rtl = lv_obj_get_base_dir(table) == LV_BIDI_DIR_RTL ? true : false;

        cell_area.y2 = table->coords.y1 + bg_top - 1;
        for(row = 0; row < ext->row_cnt; row++) {
            lv_coord_t h_row = ext->row_h[row];

            cell_area.y1 = cell_area.y2 + 1;
            cell_area.y2 = cell_area.y1 + h_row - 1;

            if(cell_area.y1 > clip_area->y2) return LV_DESIGN_RES_OK;

            if(rtl) cell_area.x1 = table->coords.x2 - bg_right - 1;
            else cell_area.x2 = table->coords.x1 + bg_left - 1;

            for(col = 0; col < ext->col_cnt; col++) {

                lv_table_cell_format_t format;
                if(ext->cell_data[cell]) {
                    format.format_byte = ext->cell_data[cell][0];
                }
                else {
                    format.s.right_merge = 0;
                    format.s.align       = LV_LABEL_ALIGN_LEFT;
                    format.s.type        = 0;
                    format.s.crop        = 1;
                }

                if(rtl) {
                    cell_area.x2 = cell_area.x1 - 1;
                    cell_area.x1 = cell_area.x2 - ext->col_w[col] + 1;
                }
                else {
                    cell_area.x1 = cell_area.x2 + 1;
                    cell_area.x2 = cell_area.x1 + ext->col_w[col] - 1;
                }

                uint16_t col_merge = 0;
                for(col_merge = 0; col_merge + col < ext->col_cnt - 1; col_merge++) {
                    if(ext->cell_data[cell + col_merge] != NULL) {
                        format.format_byte = ext->cell_data[cell + col_merge][0];
                        if(format.s.right_merge)
                            if(rtl) cell_area.x1 -= ext->col_w[col + col_merge + 1];
                            else cell_area.x2 += ext->col_w[col + col_merge + 1];
                        else
                            break;
                    }
                    else {
                        break;
                    }
                }

                if(cell_area.y2 < clip_area->y1) {
                    cell += col_merge + 1;
                    col += col_merge;
                    continue;
                }

                uint8_t cell_type = format.s.type;

                /*Expand the cell area with a half border to avoid drawing 2 borders next to each other*/
                lv_area_t cell_area_border;
                lv_area_copy(&cell_area_border, &cell_area);
                if((rect_dsc[cell_type].border_side & LV_BORDER_SIDE_LEFT) && cell_area_border.x1 > table->coords.x1 + bg_left) {
                    cell_area_border.x1 -= rect_dsc[cell_type].border_width / 2;
                }
                if((rect_dsc[cell_type].border_side & LV_BORDER_SIDE_TOP) && cell_area_border.y1 > table->coords.y1 + bg_top) {
                    cell_area_border.y1 -= rect_dsc[cell_type].border_width / 2;
                }
                if((rect_dsc[cell_type].border_side & LV_BORDER_SIDE_RIGHT) && cell_area_border.x2 < table->coords.x2 - bg_right - 1) {
                    cell_area_border.x2 += rect_dsc[cell_type].border_width / 2 + (rect_dsc[cell_type].border_width & 0x1);
                }
                if((rect_dsc[cell_type].border_side & LV_BORDER_SIDE_BOTTOM) &&
                   cell_area_border.y2 < table->coords.y2 - bg_bottom - 1) {
                    cell_area_border.y2 += rect_dsc[cell_type].border_width / 2 + (rect_dsc[cell_type].border_width & 0x1);
                }

                lv_draw_rect(&cell_area_border, clip_area, &rect_dsc[cell_type]);

                if(ext->cell_data[cell]) {
                    txt_area.x1 = cell_area.x1 + cell_left[cell_type];
                    txt_area.x2 = cell_area.x2 - cell_right[cell_type];
                    txt_area.y1 = cell_area.y1 + cell_top[cell_type];
                    txt_area.y2 = cell_area.y2 - cell_bottom[cell_type];

                    /*Align the content to the middle if not cropped*/
                    if(format.s.crop == 0) {
                        txt_flags = LV_TXT_FLAG_NONE;
                    }
                    else {
                        txt_flags = LV_TXT_FLAG_EXPAND;
                    }

                    _lv_txt_get_size(&txt_size, ext->cell_data[cell] + 1, label_dsc[cell_type].font,
                                     label_dsc[cell_type].letter_space, label_dsc[cell_type].line_space,
                                     lv_area_get_width(&txt_area), txt_flags);

                    label_dsc[cell_type].flag = 0;
                    /*Align the content to the middle if not cropped*/
                    if(format.s.crop == 0) {
                        txt_area.y1 = cell_area.y1 + h_row / 2 - txt_size.y / 2;
                        txt_area.y2 = cell_area.y1 + h_row / 2 + txt_size.y / 2;
                    }

                    switch(format.s.align) {
                        default:
                        case LV_LABEL_ALIGN_LEFT:
                            label_dsc[cell_type].flag |= LV_TXT_FLAG_NONE;
                            break;
                        case LV_LABEL_ALIGN_RIGHT:
                            label_dsc[cell_type].flag |= LV_TXT_FLAG_RIGHT;
                            break;
                        case LV_LABEL_ALIGN_CENTER:
                            label_dsc[cell_type].flag |= LV_TXT_FLAG_CENTER;
                            break;
                    }

                    lv_area_t label_mask;
                    bool label_mask_ok;
                    label_mask_ok = _lv_area_intersect(&label_mask, clip_area, &cell_area);
                    if(label_mask_ok) {
                        lv_draw_label(&txt_area, &label_mask, &label_dsc[cell_type], ext->cell_data[cell] + 1, NULL);
                    }

                    /*Draw lines after '\n's*/
                    lv_point_t p1;
                    lv_point_t p2;
                    p1.x = cell_area.x1;
                    p2.x = cell_area.x2;
                    for(i = 1; ext->cell_data[cell][i] != '\0'; i++) {
                        if(ext->cell_data[cell][i] == '\n') {
                            ext->cell_data[cell][i] = '\0';
                            _lv_txt_get_size(&txt_size, ext->cell_data[cell] + 1, label_dsc[cell_type].font,
                                             label_dsc[cell_type].letter_space, label_dsc[cell_type].line_space,
                                             lv_area_get_width(&txt_area), txt_flags);

                            p1.y = txt_area.y1 + txt_size.y + label_dsc[cell_type].line_space / 2;
                            p2.y = txt_area.y1 + txt_size.y + label_dsc[cell_type].line_space / 2;
                            lv_draw_line(&p1, &p2, clip_area, &line_dsc[cell_type]);

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
        ancestor_design(table, clip_area, mode);
    }

    return LV_DESIGN_RES_OK;
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
    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_table_get_style(table, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(table, sign, param);
    }

    /* Include the ancient signal function */
    res = ancestor_signal(table, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Free the cell texts*/
        lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
        uint16_t i;
        for(i = 0; i < ext->col_cnt * ext->row_cnt; i++) {
            if(ext->cell_data[i]) {
                lv_mem_free(ext->cell_data[i]);
                ext->cell_data[i] = NULL;
            }
        }

        if(ext->cell_data) lv_mem_free(ext->cell_data);
        if(ext->row_h) lv_mem_free(ext->row_h);

        for(i = 0; i < LV_TABLE_CELL_STYLE_CNT; i++) {
            lv_obj_clean_style_list(table, LV_TABLE_PART_CELL1 + i);
        }
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        refr_size(table);
    }

    return res;
}


/**
 * Get the style descriptor of a part of the object
 * @param table pointer the object
 * @param part the part from. (LV_TABLE_PART_...)
 * @return pointer to the style descriptor of the specified part
 */
static lv_style_list_t * lv_table_get_style(lv_obj_t * table, uint8_t part)
{
    LV_ASSERT_OBJ(table, LV_OBJX_NAME);

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);

    /* Because of the presence of LV_TABLE_PART_BG, LV_TABLE_PART_CELL<i> has an integer value
       of <i>. This comes in useful to extend above code with more cell types as follows */
    if(part == LV_TABLE_PART_BG) {
        return &table->style_list;
    }
    else if(part >= 1 && part <= LV_TABLE_CELL_STYLE_CNT) {
        return &ext->cell_style[part - 1];
    }

    return NULL;
}

static void refr_size(lv_obj_t * table)
{
    lv_coord_t h = 0;
    lv_coord_t w = 0;

    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    if(ext->row_cnt == 0 || ext->col_cnt == 0) {
        lv_obj_set_size(table, w, h);
        return;
    }

    uint16_t i;
    for(i = 0; i < ext->col_cnt; i++) {
        w += ext->col_w[i];
    }

    lv_style_int_t cell_left[LV_TABLE_CELL_STYLE_CNT];
    lv_style_int_t cell_right[LV_TABLE_CELL_STYLE_CNT];
    lv_style_int_t cell_top[LV_TABLE_CELL_STYLE_CNT];
    lv_style_int_t cell_bottom[LV_TABLE_CELL_STYLE_CNT];
    lv_style_int_t letter_space[LV_TABLE_CELL_STYLE_CNT];
    lv_style_int_t line_space[LV_TABLE_CELL_STYLE_CNT];
    const lv_font_t * font[LV_TABLE_CELL_STYLE_CNT];

    for(i = 0; i < LV_TABLE_CELL_STYLE_CNT; i++) {
        if((ext->cell_types & (1 << i)) == 0) continue; /*Skip unused cell types*/
        cell_left[i] = lv_obj_get_style_pad_left(table, LV_TABLE_PART_CELL1 + i);
        cell_right[i] = lv_obj_get_style_pad_right(table, LV_TABLE_PART_CELL1 + i);
        cell_top[i] = lv_obj_get_style_pad_top(table, LV_TABLE_PART_CELL1 + i);
        cell_bottom[i] = lv_obj_get_style_pad_bottom(table, LV_TABLE_PART_CELL1 + i);
        letter_space[i] = lv_obj_get_style_text_letter_space(table, LV_TABLE_PART_CELL1 + i);
        line_space[i] = lv_obj_get_style_text_line_space(table, LV_TABLE_PART_CELL1 + i);
        font[i] = lv_obj_get_style_text_font(table, LV_TABLE_PART_CELL1 + i);
    }


    for(i = 0; i < ext->row_cnt; i++) {
        ext->row_h[i] = get_row_height(table, i, font, letter_space, line_space,
                                       cell_left, cell_right, cell_top, cell_bottom);
        h += ext->row_h[i];
    }

    lv_style_int_t bg_top = lv_obj_get_style_pad_top(table, LV_TABLE_PART_BG);
    lv_style_int_t bg_bottom = lv_obj_get_style_pad_bottom(table, LV_TABLE_PART_BG);
    lv_style_int_t bg_left = lv_obj_get_style_pad_left(table, LV_TABLE_PART_BG);
    lv_style_int_t bg_right = lv_obj_get_style_pad_right(table, LV_TABLE_PART_BG);
    w += bg_left + bg_right;
    h += bg_top + bg_bottom;

    lv_obj_set_size(table, w + 1, h + 1);
    lv_obj_invalidate(table); /*Always invalidate even if the size hasn't changed*/
}

static lv_coord_t get_row_height(lv_obj_t * table, uint16_t row_id, const lv_font_t ** font,
                                 lv_style_int_t * letter_space, lv_style_int_t * line_space,
                                 lv_style_int_t * cell_left, lv_style_int_t * cell_right, lv_style_int_t * cell_top, lv_style_int_t * cell_bottom)
{
    lv_table_ext_t * ext = lv_obj_get_ext_attr(table);
    lv_point_t txt_size;
    lv_coord_t txt_w;

    uint16_t row_start = row_id * ext->col_cnt;
    uint16_t cell;
    uint16_t col;
    lv_coord_t h_max = lv_font_get_line_height(font[0]) + cell_top[0] + cell_bottom[0];

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
                }
                else {
                    break;
                }
            }

            lv_table_cell_format_t format;
            format.format_byte = ext->cell_data[cell][0];
            uint8_t cell_type  = format.s.type;

            /*With text crop assume 1 line*/
            if(format.s.crop) {
                h_max = LV_MATH_MAX(lv_font_get_line_height(font[cell_type]) + cell_top[cell_type] + cell_bottom[cell_type],
                                    h_max);
            }
            /*Without text crop calculate the height of the text in the cell*/
            else {
                txt_w -= cell_left[cell_type] + cell_right[cell_type];

                _lv_txt_get_size(&txt_size, ext->cell_data[cell] + 1, font[cell_type],
                                 letter_space[cell_type], line_space[cell_type], txt_w, LV_TXT_FLAG_NONE);

                h_max = LV_MATH_MAX(txt_size.y + cell_top[cell_type] + cell_bottom[cell_type], h_max);
                cell += col_merge;
                col += col_merge;
            }
        }
    }

    return h_max;
}

#endif
