/**
 * @file lv_table.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_table.h"
#if LV_USE_TABLE != 0

#include "../core/lv_indev.h"
#include "../misc/lv_assert.h"
#include "../misc/lv_txt.h"
#include "../misc/lv_txt_ap.h"
#include "../misc/lv_math.h"
#include "../misc/lv_printf.h"
#include "../draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_table_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_table_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_table_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_table_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_main(lv_event_t * e);
static lv_coord_t get_row_height(lv_obj_t * obj, uint16_t row_id, const lv_font_t * font,
                                 lv_coord_t letter_space, lv_coord_t line_space,
                                 lv_coord_t cell_left, lv_coord_t cell_right, lv_coord_t cell_top, lv_coord_t cell_bottom);
static void refr_size(lv_obj_t * obj, uint32_t strat_row);
static lv_res_t get_pressed_cell(lv_obj_t * obj, uint16_t * row, uint16_t * col);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_table_class  = {
    .constructor_cb = lv_table_constructor,
    .destructor_cb = lv_table_destructor,
    .event_cb = lv_table_event,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .base_class = &lv_obj_class,
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .instance_size = sizeof(lv_table_t),
};
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_table_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin")
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_table_set_cell_value(lv_obj_t * obj, uint16_t row, uint16_t col, const char * txt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(txt);

    lv_table_t * table = (lv_table_t *)obj;

    /*Auto expand*/
    if(col >= table->col_cnt) lv_table_set_col_cnt(obj, col + 1);
    if(row >= table->row_cnt) lv_table_set_row_cnt(obj, row + 1);

    uint32_t cell = row * table->col_cnt + col;
    lv_table_cell_ctrl_t ctrl = 0;

    /*Save the control byte*/
    if(table->cell_data[cell]) ctrl = table->cell_data[cell][0];

#if LV_USE_ARABIC_PERSIAN_CHARS
    /*Get the size of the Arabic text and process it*/
    size_t len_ap = _lv_txt_ap_calc_bytes_cnt(txt);
    table->cell_data[cell] = lv_mem_realloc(table->cell_data[cell], len_ap + 1);
    LV_ASSERT_MALLOC(table->cell_data[cell]);
    if(table->cell_data[cell] == NULL) return;

    _lv_txt_ap_proc(txt, &table->cell_data[cell][1]);
#else
    table->cell_data[cell] = lv_mem_realloc(table->cell_data[cell], strlen(txt) + 2); /*+1: trailing '\0; +1: format byte*/
    LV_ASSERT_MALLOC(table->cell_data[cell]);

    strcpy(table->cell_data[cell] + 1, txt);  /*+1 to skip the format byte*/
#endif

    table->cell_data[cell][0] = ctrl;
    refr_size(obj, row);

    lv_obj_invalidate(obj);
}

void lv_table_set_cell_value_fmt(lv_obj_t * obj, uint16_t row, uint16_t col, const char * fmt, ...)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(fmt);

    lv_table_t * table = (lv_table_t *)obj;
    if(col >= table->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_value: invalid column");
        return;
    }

    /*Auto expand*/
    if(row >= table->row_cnt) {
        lv_table_set_row_cnt(obj, row + 1);
    }

    uint32_t cell = row * table->col_cnt + col;
    lv_table_cell_ctrl_t ctrl = 0;

    /*Save the control byte*/
    if(table->cell_data[cell]) ctrl= table->cell_data[cell][0];

    va_list ap, ap2;
    va_start(ap, fmt);
    va_copy(ap2, ap);

    /*Allocate space for the new text by using trick from C99 standard section 7.19.6.12*/
    uint32_t len = lv_vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);

#if LV_USE_ARABIC_PERSIAN_CHARS
    /*Put together the text according to the format string*/
    char * raw_txt = lv_mem_buf_get(len + 1);
    LV_ASSERT_MALLOC(raw_txt);
    if(raw_txt == NULL) {
        va_end(ap2);
        return;
    }

    lv_vsnprintf(raw_txt, len + 1, fmt, ap2);

    /*Get the size of the Arabic text and process it*/
    size_t len_ap = _lv_txt_ap_calc_bytes_cnt(raw_txt);
    table->cell_data[cell] = lv_mem_realloc(table->cell_data[cell], len_ap + 1);
    LV_ASSERT_MALLOC(table->cell_data[cell]);
    if(table->cell_data[cell] == NULL) {
        va_end(ap2);
        return;
    }
    _lv_txt_ap_proc(raw_txt, &table->cell_data[cell][1]);

    lv_mem_buf_release(raw_txt);
#else
    table->cell_data[cell] = lv_mem_realloc(table->cell_data[cell], len + 2); /*+1: trailing '\0; +1: format byte*/
    LV_ASSERT_MALLOC(table->cell_data[cell]);
    if(table->cell_data[cell] == NULL) {
        va_end(ap2);
        return;
    }

    table->cell_data[cell][len + 1] = 0; /*Ensure NULL termination*/

    lv_vsnprintf(&table->cell_data[cell][1], len + 1, fmt, ap2);
#endif

    va_end(ap2);

    table->cell_data[cell][0] = ctrl;

    /*Refresh the row height*/
    lv_coord_t cell_left = lv_obj_get_style_pad_left(obj, LV_PART_ITEMS);
    lv_coord_t cell_right = lv_obj_get_style_pad_right(obj, LV_PART_ITEMS);
    lv_coord_t cell_top = lv_obj_get_style_pad_top(obj, LV_PART_ITEMS);
    lv_coord_t cell_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_ITEMS);

    lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_ITEMS);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_ITEMS);
    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_ITEMS);

    lv_coord_t h = get_row_height(obj, row, font, letter_space, line_space,
                                       cell_left, cell_right, cell_top, cell_bottom);


    lv_coord_t minh = lv_obj_get_style_min_height(obj, LV_PART_ITEMS);
    lv_coord_t maxh = lv_obj_get_style_max_height(obj, LV_PART_ITEMS);

    table->row_h[row] = LV_CLAMP(minh, h, maxh);

    lv_obj_invalidate(obj);
}

void lv_table_set_row_cnt(lv_obj_t * obj, uint16_t row_cnt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;
    uint16_t old_row_cnt = table->row_cnt;
    table->row_cnt         = row_cnt;

    table->row_h = lv_mem_realloc(table->row_h, table->row_cnt * sizeof(table->row_h[0]));
    LV_ASSERT_MALLOC(table->row_h);
    if(table->row_h == NULL) return;

    /*Free the unused cells*/
    if(old_row_cnt > row_cnt) {
        uint16_t old_cell_cnt = old_row_cnt * table->col_cnt;
        uint32_t new_cell_cnt = table->col_cnt * table->row_cnt;
        uint32_t i;
        for(i = new_cell_cnt; i < old_cell_cnt; i++) {
            lv_mem_free(table->cell_data[i]);
        }
    }

    table->cell_data = lv_mem_realloc(table->cell_data, table->row_cnt * table->col_cnt * sizeof(char *));
    LV_ASSERT_MALLOC(table->cell_data);
    if(table->cell_data == NULL) return;

    /*Initialize the new fields*/
    if(old_row_cnt < row_cnt) {
        uint32_t old_cell_cnt = old_row_cnt * table->col_cnt;
        uint32_t new_cell_cnt = table->col_cnt * table->row_cnt;
        lv_memset_00(&table->cell_data[old_cell_cnt], (new_cell_cnt - old_cell_cnt) * sizeof(table->cell_data[0]));
    }

    refr_size(obj, 0) ;
}

void lv_table_set_col_cnt(lv_obj_t * obj, uint16_t col_cnt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;
    uint16_t old_col_cnt = table->col_cnt;
    table->col_cnt         = col_cnt;
    table->col_w = lv_mem_realloc(table->col_w, col_cnt * sizeof(table->row_h[0]));
    LV_ASSERT_MALLOC(table->col_w);
    if(table->col_w == NULL) return;

    /*Free the unused cells*/
    if(old_col_cnt > col_cnt) {
       uint16_t old_cell_cnt = old_col_cnt * table->row_cnt;
       uint32_t new_cell_cnt = table->col_cnt * table->row_cnt;
       uint32_t i;
       for(i = new_cell_cnt; i < old_cell_cnt; i++) {
           lv_mem_free(table->cell_data[i]);
       }
   }

    char ** new_cell_data = lv_mem_alloc(table->row_cnt * table->col_cnt * sizeof(char *));
    LV_ASSERT_MALLOC(new_cell_data);
    if(new_cell_data == NULL) return;
    uint32_t new_cell_cnt = table->col_cnt * table->row_cnt;
    lv_memset_00(new_cell_data, new_cell_cnt * sizeof(table->cell_data[0]));

    /*Initialize the new fields*/
    if(old_col_cnt < col_cnt) {
        uint32_t col;
        for(col = old_col_cnt; col < col_cnt; col++) {
            table->col_w[col] = LV_DPI_DEF;
        }
    }

    /*The new column(s) messes up the mapping of `cell_data`*/
    uint32_t old_col_start;
    uint32_t new_col_start;
    uint32_t min_col_cnt = LV_MIN(old_col_cnt, col_cnt);
    uint32_t row;
    for(row = 0; row < table->row_cnt; row++) {
        old_col_start = row * old_col_cnt;
        new_col_start = row * col_cnt;

        lv_memcpy_small(&new_cell_data[new_col_start], &table->cell_data[old_col_start], sizeof(new_cell_data[0]) * min_col_cnt);
    }

    lv_mem_free(table->cell_data);
    table->cell_data = new_cell_data;


    refr_size(obj, 0) ;
}

void lv_table_set_col_width(lv_obj_t * obj, uint16_t col_id, lv_coord_t w)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;

    /*Auto expand*/
    if(col_id >= table->col_cnt) lv_table_set_col_cnt(obj, col_id + 1);

    table->col_w[col_id] = w;
    refr_size(obj, 0) ;
}

void lv_table_add_cell_ctrl(lv_obj_t * obj, uint16_t row, uint16_t col, lv_table_cell_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;

    /*Auto expand*/
    if(col >= table->col_cnt) lv_table_set_col_cnt(obj, col + 1);
    if(row >= table->row_cnt) lv_table_set_row_cnt(obj, row + 1);

    uint32_t cell = row * table->col_cnt + col;

    if(table->cell_data[cell] == NULL) {
        table->cell_data[cell]    = lv_mem_alloc(2); /*+1: trailing '\0; +1: format byte*/
        LV_ASSERT_MALLOC(table->cell_data[cell]);
        if(table->cell_data[cell] == NULL) return;

        table->cell_data[cell][0] = 0;
        table->cell_data[cell][1] = '\0';
    }

    table->cell_data[cell][0] |= ctrl;
}

void lv_table_clear_cell_ctrl(lv_obj_t * obj, uint16_t row, uint16_t col, lv_table_cell_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;

    /*Auto expand*/
    if(col >= table->col_cnt) lv_table_set_col_cnt(obj, col + 1);
    if(row >= table->row_cnt) lv_table_set_row_cnt(obj, row + 1);

    uint32_t cell = row * table->col_cnt + col;

    if(table->cell_data[cell] == NULL) {
        table->cell_data[cell]    = lv_mem_alloc(2); /*+1: trailing '\0; +1: format byte*/
        LV_ASSERT_MALLOC(table->cell_data[cell]);
        if(table->cell_data[cell] == NULL) return;

        table->cell_data[cell][0] = 0;
        table->cell_data[cell][1] = '\0';
    }

    table->cell_data[cell][0] &= (~ctrl);
}

/*=====================
 * Getter functions
 *====================*/

const char * lv_table_get_cell_value(lv_obj_t * obj, uint16_t row, uint16_t col)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;
    if(row >= table->row_cnt || col >= table->col_cnt) {
        LV_LOG_WARN("lv_table_set_cell_value: invalid row or column");
        return "";
    }
    uint32_t cell = row * table->col_cnt + col;

    if(table->cell_data[cell] == NULL) return "";

    return &table->cell_data[cell][1]; /*Skip the format byte*/
}

uint16_t lv_table_get_row_cnt(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;
    return table->row_cnt;
}

uint16_t lv_table_get_col_cnt(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;
    return table->col_cnt;
}

lv_coord_t lv_table_get_col_width(lv_obj_t * obj, uint16_t col)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;

    if(col >= table->col_cnt) {
        LV_LOG_WARN("lv_table_set_col_width: too big 'col_id'. Must be < LV_TABLE_COL_MAX.");
        return 0;
    }

    return table->col_w[col];
}

bool lv_table_has_cell_ctrl(lv_obj_t * obj, uint16_t row, uint16_t col, lv_table_cell_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;
    if(row >= table->row_cnt || col >= table->col_cnt) {
        LV_LOG_WARN("lv_table_get_cell_crop: invalid row or column");
        return false;
    }
    uint32_t cell = row * table->col_cnt + col;

    if(table->cell_data[cell] == NULL) return false;
    else return (table->cell_data[cell][0] & ctrl) == ctrl ? true : false;
}

void lv_table_get_selected_cell(lv_obj_t * obj, uint16_t * row, uint16_t * col)
{
    lv_table_t * table = (lv_table_t *)obj;
    *row = table->row_act;
    *col = table->col_act;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_table_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_table_t * table = (lv_table_t *)obj;

    table->col_cnt = 1;
    table->row_cnt = 1;
    table->col_w = lv_mem_alloc(table->col_cnt * sizeof(table->col_w[0]));
    table->row_h = lv_mem_alloc(table->row_cnt * sizeof(table->row_h[0]));
    table->col_w[0] = LV_DPI_DEF;
    table->row_h[0] = LV_DPI_DEF;
    table->cell_data = lv_mem_realloc(table->cell_data, table->row_cnt * table->col_cnt * sizeof(char *));
    table->cell_data[0] = NULL;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_table_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_table_t * table = (lv_table_t *)obj;
    /*Free the cell texts*/
    uint16_t i;
    for(i = 0; i < table->col_cnt * table->row_cnt; i++) {
        if(table->cell_data[i]) {
            lv_mem_free(table->cell_data[i]);
            table->cell_data[i] = NULL;
        }
    }

    if(table->cell_data) lv_mem_free(table->cell_data);
    if(table->row_h) lv_mem_free(table->row_h);
}

static void lv_table_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_table_t * table = (lv_table_t *)obj;

    if(code == LV_EVENT_STYLE_CHANGED) {
        refr_size(obj, 0);
    }
    else if(code == LV_EVENT_GET_SELF_SIZE) {
        lv_point_t * p = lv_event_get_param(e);
        uint32_t i;
        lv_coord_t w = 0;
        for(i = 0; i < table->col_cnt; i++) w += table->col_w[i];

        lv_coord_t h = 0;
        for(i = 0; i < table->row_cnt; i++) h += table->row_h[i];

        p->x = w;
        p->y = h;
    }
    else if(code == LV_EVENT_PRESSED || code == LV_EVENT_PRESSING) {
        uint16_t col;
        uint16_t row;
        lv_res_t pr_res = get_pressed_cell(obj, &row, &col);

        if(pr_res == LV_RES_OK && (table->col_act != col || table->row_act != row)) {
            table->col_act = col;
            table->row_act = row;
        }
        lv_obj_invalidate(obj);
    }
    else if(code == LV_EVENT_RELEASED) {
        lv_obj_invalidate(obj);
        lv_indev_t * indev = lv_indev_get_act();
        lv_obj_t * scroll_obj = lv_indev_get_scroll_obj(indev);
        if(table->col_act != LV_TABLE_CELL_NONE && table->row_act != LV_TABLE_CELL_NONE && scroll_obj == NULL) {
            res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return;
        }

        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());
        if(indev_type == LV_INDEV_TYPE_POINTER || indev_type == LV_INDEV_TYPE_BUTTON) {
            table->col_act = LV_TABLE_CELL_NONE;
            table->row_act = LV_TABLE_CELL_NONE;
        }
    }
    else if(code == LV_EVENT_FOCUSED) {
        lv_obj_invalidate(obj);
    } else if(code == LV_EVENT_KEY) {
        int32_t c = *((int32_t *)lv_event_get_param(e));
        int32_t col = table->col_act;
        int32_t row = table->row_act;
        if(col == LV_TABLE_CELL_NONE || row == LV_TABLE_CELL_NONE) {
            table->col_act = 0;
            table->row_act = 0;
            lv_obj_invalidate(obj);
            return;
        }

        if(col >= table->col_cnt) col = 0;
        if(row >= table->row_cnt) row = 0;

        if(c == LV_KEY_LEFT) col--;
        else if(c == LV_KEY_RIGHT) col++;
        else if(c == LV_KEY_UP) row--;
        else if(c == LV_KEY_DOWN) row++;
        else return;

        if(col >= table->col_cnt) {
            if(row < table->row_cnt - 1) {
                col = 0;
                row++;
            } else {
                col = table->col_cnt - 1;
            }
        } else if (col < 0) {
            if(row != 0) {
                col = table->col_cnt - 1;
                row--;
            } else {
                col = 0;
            }
        }

        if(row >= table->row_cnt) {
            row = table->row_cnt - 1;
        } else if(row < 0) {
            row = 0;
        }

        if(table->col_act != col || table->row_act != row) {
            table->col_act = col;
            table->row_act = row;
            lv_obj_invalidate(obj);

            res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return;
        }
    } else if(code == LV_EVENT_DRAW_MAIN) {
        draw_main(e);
    }
}


static void draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_table_t * table = (lv_table_t *)obj;
    const lv_area_t * clip_area = lv_event_get_param(e);

    lv_point_t txt_size;
    lv_area_t cell_area;
    lv_area_t txt_area;
    lv_text_flag_t txt_flags;

    lv_coord_t bg_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t bg_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
    lv_coord_t bg_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t bg_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);

    lv_coord_t cell_left = lv_obj_get_style_pad_left(obj, LV_PART_ITEMS);
    lv_coord_t cell_right = lv_obj_get_style_pad_right(obj, LV_PART_ITEMS);
    lv_coord_t cell_top = lv_obj_get_style_pad_top(obj, LV_PART_ITEMS);
    lv_coord_t cell_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_ITEMS);

    lv_state_t state_ori = obj->state;
    obj->state = LV_STATE_DEFAULT;
    obj->skip_trans = 1;
    lv_draw_rect_dsc_t rect_dsc_def;
    lv_draw_rect_dsc_t rect_dsc_act; /*Passed to the event to modify it*/
    lv_draw_rect_dsc_init(&rect_dsc_def);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_ITEMS, &rect_dsc_def);

    lv_draw_label_dsc_t label_dsc_def;
    lv_draw_label_dsc_t label_dsc_act;  /*Passed to the event to modify it*/
    lv_draw_label_dsc_init(&label_dsc_def);
    lv_obj_init_draw_label_dsc(obj, LV_PART_ITEMS, &label_dsc_def);
    obj->state = state_ori;
    obj->skip_trans = 0;

    uint16_t col;
    uint16_t row;
    uint16_t cell = 0;

    cell_area.y2 = obj->coords.y1 + bg_top - 1 - lv_obj_get_scroll_y(obj) ;
    lv_coord_t scroll_x = lv_obj_get_scroll_x(obj) ;
    bool rtl = lv_obj_get_style_base_dir(obj, LV_PART_MAIN) == LV_BASE_DIR_RTL ? true : false;

    /*Handle custom drawer*/
    lv_obj_draw_part_dsc_t dsc;
    lv_obj_draw_dsc_init(&dsc, clip_area);
    dsc.part = LV_PART_ITEMS;
    dsc.rect_dsc = &rect_dsc_act;
    dsc.label_dsc = &label_dsc_act;

    for(row = 0; row < table->row_cnt; row++) {
        lv_coord_t h_row = table->row_h[row];

        cell_area.y1 = cell_area.y2 + 1;
        cell_area.y2 = cell_area.y1 + h_row - 1;

        if(cell_area.y1 > clip_area->y2) return;

        if(rtl) cell_area.x1 = obj->coords.x2 - bg_right - 1 - scroll_x;
        else cell_area.x2 = obj->coords.x1 + bg_left - 1 - scroll_x;

        for(col = 0; col < table->col_cnt; col++) {
            lv_table_cell_ctrl_t ctrl = 0;
            if(table->cell_data[cell]) ctrl = table->cell_data[cell][0];

            if(rtl) {
                cell_area.x2 = cell_area.x1 - 1;
                cell_area.x1 = cell_area.x2 - table->col_w[col] + 1;
            }
            else {
                cell_area.x1 = cell_area.x2 + 1;
                cell_area.x2 = cell_area.x1 + table->col_w[col] - 1;
            }

            uint16_t col_merge = 0;
            for(col_merge = 0; col_merge + col < table->col_cnt - 1; col_merge++) {
                if(table->cell_data[cell + col_merge]) {
                    char * next_cell_data = table->cell_data[cell + col_merge];
                    if(next_cell_data) ctrl = next_cell_data[0];
                    if(ctrl & LV_TABLE_CELL_CTRL_MERGE_RIGHT)
                        if(rtl) cell_area.x1 -= table->col_w[col + col_merge + 1];
                        else cell_area.x2 += table->col_w[col + col_merge + 1];
                    else {
                        break;
                    }
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

            /*Expand the cell area with a half border to avoid drawing 2 borders next to each other*/
            lv_area_t cell_area_border;
            lv_area_copy(&cell_area_border, &cell_area);
            if((rect_dsc_def.border_side & LV_BORDER_SIDE_LEFT) && cell_area_border.x1 > obj->coords.x1 + bg_left) {
                cell_area_border.x1 -= rect_dsc_def.border_width / 2;
            }
            if((rect_dsc_def.border_side & LV_BORDER_SIDE_TOP) && cell_area_border.y1 > obj->coords.y1 + bg_top) {
                cell_area_border.y1 -= rect_dsc_def.border_width / 2;
            }
            if((rect_dsc_def.border_side & LV_BORDER_SIDE_RIGHT) && cell_area_border.x2 < obj->coords.x2 - bg_right - 1) {
                cell_area_border.x2 += rect_dsc_def.border_width / 2 + (rect_dsc_def.border_width & 0x1);
            }
            if((rect_dsc_def.border_side & LV_BORDER_SIDE_BOTTOM) &&
                    cell_area_border.y2 < obj->coords.y2 - bg_bottom - 1) {
                cell_area_border.y2 += rect_dsc_def.border_width / 2 + (rect_dsc_def.border_width & 0x1);
            }

            lv_state_t cell_state = LV_STATE_DEFAULT;
            if(row == table->row_act && col == table->col_act) {
                if(!(obj->state & LV_STATE_SCROLLED) && (obj->state & LV_STATE_PRESSED)) cell_state |= LV_STATE_PRESSED;
                if(obj->state & LV_STATE_FOCUSED) cell_state |= LV_STATE_FOCUSED;
                if(obj->state & LV_STATE_FOCUS_KEY) cell_state |= LV_STATE_FOCUS_KEY;
                if(obj->state & LV_STATE_EDITED) cell_state |= LV_STATE_EDITED;
            }

            /*Set up the draw descriptors*/
            if(cell_state == LV_STATE_DEFAULT) {
                lv_memcpy(&rect_dsc_act, &rect_dsc_def, sizeof(lv_draw_rect_dsc_t));
                lv_memcpy(&label_dsc_act, &label_dsc_def, sizeof(lv_draw_label_dsc_t));
            }
            /*In other cases get the styles directly without caching them*/
            else {
                obj->state = cell_state;
                obj->skip_trans = 1;
                lv_draw_rect_dsc_init(&rect_dsc_act);
                lv_draw_label_dsc_init(&label_dsc_act);
                lv_obj_init_draw_rect_dsc(obj, LV_PART_ITEMS, &rect_dsc_act);
                lv_obj_init_draw_label_dsc(obj, LV_PART_ITEMS, &label_dsc_act);
                obj->state = state_ori;
                obj->skip_trans = 0;
            }

            dsc.draw_area = &cell_area_border;
            dsc.id = row * table->col_cnt + col;
            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &dsc);

            lv_draw_rect(&cell_area_border, clip_area, &rect_dsc_act);

            if(table->cell_data[cell]) {
                txt_area.x1 = cell_area.x1 + cell_left;
                txt_area.x2 = cell_area.x2 - cell_right;
                txt_area.y1 = cell_area.y1 + cell_top;
                txt_area.y2 = cell_area.y2 - cell_bottom;

                /*Align the content to the middle if not cropped*/
                bool crop = ctrl & LV_TABLE_CELL_CTRL_TEXT_CROP ? true : false;
                if(crop) txt_flags = LV_TEXT_FLAG_EXPAND;
                else txt_flags = LV_TEXT_FLAG_NONE;

                lv_txt_get_size(&txt_size, table->cell_data[cell] + 1, label_dsc_def.font,
                        label_dsc_act.letter_space, label_dsc_act.line_space,
                        lv_area_get_width(&txt_area), txt_flags);

                /*Align the content to the middle if not cropped*/
                if(!crop) {
                    txt_area.y1 = cell_area.y1 + h_row / 2 - txt_size.y / 2;
                    txt_area.y2 = cell_area.y1 + h_row / 2 + txt_size.y / 2;
                }

                lv_area_t label_mask;
                bool label_mask_ok;
                label_mask_ok = _lv_area_intersect(&label_mask, clip_area, &cell_area);
                if(label_mask_ok) {
                    lv_draw_label(&txt_area, &label_mask, &label_dsc_act, table->cell_data[cell] + 1, NULL);
                }
            }

            lv_event_send(obj, LV_EVENT_DRAW_PART_END, &dsc);

            cell += col_merge + 1;
            col += col_merge;
        }
    }
}

static void refr_size(lv_obj_t * obj, uint32_t strat_row)
{
    lv_table_t * table = (lv_table_t *)obj;

    uint32_t i;

    lv_coord_t cell_left = lv_obj_get_style_pad_left(obj, LV_PART_ITEMS);
    lv_coord_t cell_right = lv_obj_get_style_pad_right(obj, LV_PART_ITEMS);
    lv_coord_t cell_top = lv_obj_get_style_pad_top(obj, LV_PART_ITEMS);
    lv_coord_t cell_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_ITEMS);

    lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_ITEMS);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_ITEMS);
    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_ITEMS);

    lv_coord_t minh = lv_obj_get_style_min_height(obj, LV_PART_ITEMS);
    lv_coord_t maxh = lv_obj_get_style_max_height(obj, LV_PART_ITEMS);

    for(i = strat_row; i < table->row_cnt; i++) {
        table->row_h[i] = get_row_height(obj, i, font, letter_space, line_space,
                                       cell_left, cell_right, cell_top, cell_bottom);
        table->row_h[i] = LV_CLAMP(minh, table->row_h[i], maxh);
    }

    lv_obj_refresh_self_size(obj) ;
}

static lv_coord_t get_row_height(lv_obj_t * obj, uint16_t row_id, const lv_font_t * font,
                                 lv_coord_t letter_space, lv_coord_t line_space,
                                 lv_coord_t cell_left, lv_coord_t cell_right, lv_coord_t cell_top, lv_coord_t cell_bottom)
{
    lv_table_t * table = (lv_table_t *)obj;
    lv_point_t txt_size;
    lv_coord_t txt_w;

    uint16_t row_start = row_id * table->col_cnt;
    uint16_t cell;
    uint16_t col;
    lv_coord_t h_max = lv_font_get_line_height(font) + cell_top + cell_bottom;

    for(cell = row_start, col = 0; cell < row_start + table->col_cnt; cell++, col++) {
        if(table->cell_data[cell] != NULL) {
            txt_w              = table->col_w[col];
            uint16_t col_merge = 0;
            for(col_merge = 0; col_merge + col < table->col_cnt - 1; col_merge++) {

                if(table->cell_data[cell + col_merge] != NULL) {
                    lv_table_cell_ctrl_t ctrl = 0;
                    char * next_cell_data = table->cell_data[cell + col_merge];
                    if(next_cell_data) ctrl = next_cell_data[0];
                    if(ctrl & LV_TABLE_CELL_CTRL_MERGE_RIGHT)
                        txt_w += table->col_w[col + col_merge + 1];
                    else
                        break;
                }
                else {
                    break;
                }
            }

            lv_table_cell_ctrl_t ctrl = 0;
            if(table->cell_data[cell]) ctrl = table->cell_data[cell][0];

            /*With text crop assume 1 line*/
            if(ctrl & LV_TABLE_CELL_CTRL_TEXT_CROP) {
                h_max = LV_MAX(lv_font_get_line_height(font) + cell_top + cell_bottom,
                                    h_max);
            }
            /*Without text crop calculate the height of the text in the cell*/
            else {
                txt_w -= cell_left + cell_right;

                lv_txt_get_size(&txt_size, table->cell_data[cell] + 1, font,
                                 letter_space, line_space, txt_w, LV_TEXT_FLAG_NONE);

                h_max = LV_MAX(txt_size.y + cell_top + cell_bottom, h_max);
                cell += col_merge;
                col += col_merge;
            }
        }
    }

    return h_max;
}

static lv_res_t get_pressed_cell(lv_obj_t * obj, uint16_t * row, uint16_t * col)
{
   lv_table_t * table = (lv_table_t *)obj;

    lv_indev_type_t type = lv_indev_get_type(lv_indev_get_act());
    if(type != LV_INDEV_TYPE_POINTER && type != LV_INDEV_TYPE_BUTTON) {
        if(col) *col = LV_TABLE_CELL_NONE;
        if(row) *row = LV_TABLE_CELL_NONE;
        return LV_RES_INV;
    }

    lv_point_t p;
    lv_indev_get_point(lv_indev_get_act(), &p);

    lv_coord_t tmp;
    if(col) {
        lv_coord_t x = p.x + lv_obj_get_scroll_x(obj);

        if(lv_obj_get_style_base_dir(obj, LV_PART_MAIN) == LV_BASE_DIR_RTL) {
            x = obj->coords.x2 - lv_obj_get_style_pad_right(obj, LV_PART_MAIN) - x;
        }
        else {
            x -= obj->coords.x1;
            x -= lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
        }

        *col = 0;
        tmp = 0;
        for(*col = 0; *col < table->col_cnt; (*col)++) {
            tmp += table->col_w[*col];
            if(x < tmp) break;
        }
    }

    if(row) {
        lv_coord_t y = p.y + lv_obj_get_scroll_y(obj);;
        y -= obj->coords.y1;
        y -= lv_obj_get_style_pad_top(obj, LV_PART_MAIN);

        *row = 0;
        tmp = 0;

        for(*row = 0; *row < table->row_cnt; (*row)++) {
            tmp += table->row_h[*row];
            if(y < tmp) break;
        }
    }

    return LV_RES_OK;
}


#endif
