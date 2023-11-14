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
static void refr_size_form_row(lv_obj_t * obj, uint32_t start_row);
static void refr_cell_size(lv_obj_t * obj, uint32_t row, uint32_t col);
static lv_res_t get_pressed_cell(lv_obj_t * obj, uint16_t * row, uint16_t * col);
static size_t get_cell_txt_len(const char * txt);
static void copy_cell_txt(lv_table_cell_t * dst, const char * txt);
static void get_cell_area(lv_obj_t * obj, uint16_t row, uint16_t col, lv_area_t * area);
static void scroll_to_selected_cell(lv_obj_t * obj);

static inline bool is_cell_empty(void * cell)
{
    return cell == NULL;
}

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
    LV_LOG_INFO("begin");
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
    if(table->cell_data[cell]) ctrl = table->cell_data[cell]->ctrl;

#if LV_USE_USER_DATA
    void * user_data = NULL;

    /*Save the user data*/
    if(table->cell_data[cell]) user_data = table->cell_data[cell]->user_data;
#endif

    size_t to_allocate = get_cell_txt_len(txt);

    table->cell_data[cell] = lv_mem_realloc(table->cell_data[cell], to_allocate);
    LV_ASSERT_MALLOC(table->cell_data[cell]);
    if(table->cell_data[cell] == NULL) return;

    copy_cell_txt(table->cell_data[cell], txt);

    table->cell_data[cell]->ctrl = ctrl;
#if LV_USE_USER_DATA
    table->cell_data[cell]->user_data = user_data;
#endif
    refr_cell_size(obj, row, col);
}

void lv_table_set_cell_value_fmt(lv_obj_t * obj, uint16_t row, uint16_t col, const char * fmt, ...)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(fmt);

    lv_table_t * table = (lv_table_t *)obj;
    if(col >= table->col_cnt) {
        lv_table_set_col_cnt(obj, col + 1);
    }

    /*Auto expand*/
    if(row >= table->row_cnt) {
        lv_table_set_row_cnt(obj, row + 1);
    }

    uint32_t cell = row * table->col_cnt + col;
    lv_table_cell_ctrl_t ctrl = 0;

    /*Save the control byte*/
    if(table->cell_data[cell]) ctrl = table->cell_data[cell]->ctrl;

#if LV_USE_USER_DATA
    void * user_data = NULL;

    /*Save the user_data*/
    if(table->cell_data[cell]) user_data = table->cell_data[cell]->user_data;
#endif

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
    table->cell_data[cell] = lv_mem_realloc(table->cell_data[cell], sizeof(lv_table_cell_t) + len_ap + 1);
    LV_ASSERT_MALLOC(table->cell_data[cell]);
    if(table->cell_data[cell] == NULL) {
        va_end(ap2);
        return;
    }
    _lv_txt_ap_proc(raw_txt, table->cell_data[cell]->txt);

    lv_mem_buf_release(raw_txt);
#else
    table->cell_data[cell] = lv_mem_realloc(table->cell_data[cell],
                                            sizeof(lv_table_cell_t) + len + 1); /*+1: trailing '\0; */
    LV_ASSERT_MALLOC(table->cell_data[cell]);
    if(table->cell_data[cell] == NULL) {
        va_end(ap2);
        return;
    }

    table->cell_data[cell]->txt[len] = 0; /*Ensure NULL termination*/

    lv_vsnprintf(table->cell_data[cell]->txt, len + 1, fmt, ap2);
#endif

    va_end(ap2);

    table->cell_data[cell]->ctrl = ctrl;
#if LV_USE_USER_DATA
    table->cell_data[cell]->user_data = user_data;
#endif
    refr_cell_size(obj, row, col);
}

void lv_table_set_row_cnt(lv_obj_t * obj, uint16_t row_cnt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;

    if(table->row_cnt == row_cnt) return;

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
#if LV_USE_USER_DATA
            if(table->cell_data[i]->user_data) {
                lv_mem_free(table->cell_data[i]->user_data);
                table->cell_data[i]->user_data = NULL;
            }
#endif
            lv_mem_free(table->cell_data[i]);
        }
    }

    table->cell_data = lv_mem_realloc(table->cell_data, table->row_cnt * table->col_cnt * sizeof(lv_table_cell_t *));
    LV_ASSERT_MALLOC(table->cell_data);
    if(table->cell_data == NULL) return;

    /*Initialize the new fields*/
    if(old_row_cnt < row_cnt) {
        uint32_t old_cell_cnt = old_row_cnt * table->col_cnt;
        uint32_t new_cell_cnt = table->col_cnt * table->row_cnt;
        lv_memset_00(&table->cell_data[old_cell_cnt], (new_cell_cnt - old_cell_cnt) * sizeof(table->cell_data[0]));
    }

    refr_size_form_row(obj, 0);
}

void lv_table_set_col_cnt(lv_obj_t * obj, uint16_t col_cnt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;

    if(table->col_cnt == col_cnt) return;

    uint16_t old_col_cnt = table->col_cnt;
    table->col_cnt         = col_cnt;

    lv_table_cell_t ** new_cell_data = lv_mem_alloc(table->row_cnt * table->col_cnt * sizeof(lv_table_cell_t *));
    LV_ASSERT_MALLOC(new_cell_data);
    if(new_cell_data == NULL) return;
    uint32_t new_cell_cnt = table->col_cnt * table->row_cnt;

    lv_memset_00(new_cell_data, new_cell_cnt * sizeof(table->cell_data[0]));

    /*The new column(s) messes up the mapping of `cell_data`*/
    uint32_t old_col_start;
    uint32_t new_col_start;
    uint32_t min_col_cnt = LV_MIN(old_col_cnt, col_cnt);
    uint32_t row;
    for(row = 0; row < table->row_cnt; row++) {
        old_col_start = row * old_col_cnt;
        new_col_start = row * col_cnt;

        lv_memcpy_small(&new_cell_data[new_col_start], &table->cell_data[old_col_start],
                        sizeof(new_cell_data[0]) * min_col_cnt);

        /*Free the old cells (only if the table becomes smaller)*/
        int32_t i;
        for(i = 0; i < (int32_t)old_col_cnt - col_cnt; i++) {
            uint32_t idx = old_col_start + min_col_cnt + i;
#if LV_USE_USER_DATA
            if(table->cell_data[idx]->user_data) {
                lv_mem_free(table->cell_data[idx]->user_data);
                table->cell_data[idx]->user_data = NULL;
            }
#endif
            lv_mem_free(table->cell_data[idx]);
            table->cell_data[idx] = NULL;
        }
    }

    lv_mem_free(table->cell_data);
    table->cell_data = new_cell_data;

    /*Initialize the new column widths if any*/
    table->col_w = lv_mem_realloc(table->col_w, col_cnt * sizeof(table->col_w[0]));
    LV_ASSERT_MALLOC(table->col_w);
    if(table->col_w == NULL) return;

    uint32_t col;
    for(col = old_col_cnt; col < col_cnt; col++) {
        table->col_w[col] = LV_DPI_DEF;
    }


    refr_size_form_row(obj, 0) ;
}

void lv_table_set_col_width(lv_obj_t * obj, uint16_t col_id, lv_coord_t w)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;

    /*Auto expand*/
    if(col_id >= table->col_cnt) lv_table_set_col_cnt(obj, col_id + 1);

    table->col_w[col_id] = w;
    refr_size_form_row(obj, 0);
}

void lv_table_add_cell_ctrl(lv_obj_t * obj, uint16_t row, uint16_t col, lv_table_cell_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;

    /*Auto expand*/
    if(col >= table->col_cnt) lv_table_set_col_cnt(obj, col + 1);
    if(row >= table->row_cnt) lv_table_set_row_cnt(obj, row + 1);

    uint32_t cell = row * table->col_cnt + col;

    if(is_cell_empty(table->cell_data[cell])) {
        table->cell_data[cell]    = lv_mem_alloc(sizeof(lv_table_cell_t) + 1); /*+1: trailing '\0 */
        LV_ASSERT_MALLOC(table->cell_data[cell]);
        if(table->cell_data[cell] == NULL) return;

        table->cell_data[cell]->ctrl = 0;
#if LV_USE_USER_DATA
        table->cell_data[cell]->user_data = NULL;
#endif
        table->cell_data[cell]->txt[0] = '\0';
    }

    table->cell_data[cell]->ctrl |= ctrl;
}

void lv_table_clear_cell_ctrl(lv_obj_t * obj, uint16_t row, uint16_t col, lv_table_cell_ctrl_t ctrl)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;

    /*Auto expand*/
    if(col >= table->col_cnt) lv_table_set_col_cnt(obj, col + 1);
    if(row >= table->row_cnt) lv_table_set_row_cnt(obj, row + 1);

    uint32_t cell = row * table->col_cnt + col;

    if(is_cell_empty(table->cell_data[cell])) {
        table->cell_data[cell]    = lv_mem_alloc(sizeof(lv_table_cell_t) + 1); /*+1: trailing '\0 */
        LV_ASSERT_MALLOC(table->cell_data[cell]);
        if(table->cell_data[cell] == NULL) return;

        table->cell_data[cell]->ctrl = 0;
#if LV_USE_USER_DATA
        table->cell_data[cell]->user_data = NULL;
#endif
        table->cell_data[cell]->txt[0] = '\0';
    }

    table->cell_data[cell]->ctrl &= (~ctrl);
}

#if LV_USE_USER_DATA
void lv_table_set_cell_user_data(lv_obj_t * obj, uint16_t row, uint16_t col, void * user_data)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;

    /*Auto expand*/
    if(col >= table->col_cnt) lv_table_set_col_cnt(obj, col + 1);
    if(row >= table->row_cnt) lv_table_set_row_cnt(obj, row + 1);

    uint32_t cell = row * table->col_cnt + col;

    if(is_cell_empty(table->cell_data[cell])) {
        table->cell_data[cell]    = lv_mem_alloc(sizeof(lv_table_cell_t) + 1); /*+1: trailing '\0 */
        LV_ASSERT_MALLOC(table->cell_data[cell]);
        if(table->cell_data[cell] == NULL) return;

        table->cell_data[cell]->ctrl = 0;
        table->cell_data[cell]->user_data = NULL;
        table->cell_data[cell]->txt[0] = '\0';
    }

    if(table->cell_data[cell]->user_data) {
        lv_mem_free(table->cell_data[cell]->user_data);
    }

    table->cell_data[cell]->user_data = user_data;
}
#endif

/*=====================
 * Getter functions
 *====================*/

const char * lv_table_get_cell_value(lv_obj_t * obj, uint16_t row, uint16_t col)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;
    if(row >= table->row_cnt || col >= table->col_cnt) {
        LV_LOG_WARN("invalid row or column");
        return "";
    }
    uint32_t cell = row * table->col_cnt + col;

    if(is_cell_empty(table->cell_data[cell])) return "";

    return table->cell_data[cell]->txt;
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

    if(is_cell_empty(table->cell_data[cell])) return false;
    else return (table->cell_data[cell]->ctrl & ctrl) == ctrl;
}

void lv_table_get_selected_cell(lv_obj_t * obj, uint16_t * row, uint16_t * col)
{
    lv_table_t * table = (lv_table_t *)obj;
    *row = table->row_act;
    *col = table->col_act;
}

#if LV_USE_USER_DATA
void * lv_table_get_cell_user_data(lv_obj_t * obj, uint16_t row, uint16_t col)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_table_t * table = (lv_table_t *)obj;
    if(row >= table->row_cnt || col >= table->col_cnt) {
        LV_LOG_WARN("invalid row or column");
        return NULL;
    }
    uint32_t cell = row * table->col_cnt + col;

    if(is_cell_empty(table->cell_data[cell])) return NULL;

    return table->cell_data[cell]->user_data;
}
#endif

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
    table->cell_data = lv_mem_realloc(table->cell_data, table->row_cnt * table->col_cnt * sizeof(lv_table_cell_t *));
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
#if LV_USE_USER_DATA
            if(table->cell_data[i]->user_data) {
                lv_mem_free(table->cell_data[i]->user_data);
                table->cell_data[i]->user_data = NULL;
            }
#endif
            lv_mem_free(table->cell_data[i]);
            table->cell_data[i] = NULL;
        }
    }

    if(table->cell_data) lv_mem_free(table->cell_data);
    if(table->row_h) lv_mem_free(table->row_h);
    if(table->col_w) lv_mem_free(table->col_w);
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
        refr_size_form_row(obj, 0);
    }
    else if(code == LV_EVENT_GET_SELF_SIZE) {
        lv_point_t * p = lv_event_get_param(e);
        uint32_t i;
        lv_coord_t w = 0;
        for(i = 0; i < table->col_cnt; i++) w += table->col_w[i];

        lv_coord_t h = 0;
        for(i = 0; i < table->row_cnt; i++) h += table->row_h[i];

        p->x = w - 1;
        p->y = h - 1;
    }
    else if(code == LV_EVENT_PRESSED || code == LV_EVENT_PRESSING) {
        uint16_t col;
        uint16_t row;
        lv_res_t pr_res = get_pressed_cell(obj, &row, &col);

        if(pr_res == LV_RES_OK && (table->col_act != col || table->row_act != row)) {
            table->col_act = col;
            table->row_act = row;
            lv_obj_invalidate(obj);
        }
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
    }
    else if(code == LV_EVENT_KEY) {
        int32_t c = *((int32_t *)lv_event_get_param(e));
        int32_t col = table->col_act;
        int32_t row = table->row_act;
        if(col == LV_TABLE_CELL_NONE || row == LV_TABLE_CELL_NONE) {
            table->col_act = 0;
            table->row_act = 0;
            scroll_to_selected_cell(obj);
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
            }
            else {
                col = table->col_cnt - 1;
            }
        }
        else if(col < 0) {
            if(row != 0) {
                col = table->col_cnt - 1;
                row--;
            }
            else {
                col = 0;
            }
        }

        if(row >= table->row_cnt) {
            row = table->row_cnt - 1;
        }
        else if(row < 0) {
            row = 0;
        }

        if(table->col_act != col || table->row_act != row) {
            table->col_act = col;
            table->row_act = row;
            lv_obj_invalidate(obj);

            scroll_to_selected_cell(obj);

            res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return;
        }
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        draw_main(e);
    }
}


static void draw_main(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_table_t * table = (lv_table_t *)obj;
    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);
    lv_area_t clip_area;
    if(!_lv_area_intersect(&clip_area, &obj->coords, draw_ctx->clip_area)) return;

    const lv_area_t * clip_area_ori = draw_ctx->clip_area;
    draw_ctx->clip_area = &clip_area;

    lv_point_t txt_size;
    lv_area_t cell_area;

    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t bg_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t bg_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
    lv_coord_t bg_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t bg_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);

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

    cell_area.y2 = obj->coords.y1 + bg_top - 1 - lv_obj_get_scroll_y(obj) + border_width;
    lv_coord_t scroll_x = lv_obj_get_scroll_x(obj) ;
    bool rtl = lv_obj_get_style_base_dir(obj, LV_PART_MAIN) == LV_BASE_DIR_RTL;

    /*Handle custom drawer*/
    lv_obj_draw_part_dsc_t part_draw_dsc;
    lv_obj_draw_dsc_init(&part_draw_dsc, draw_ctx);
    part_draw_dsc.part = LV_PART_ITEMS;
    part_draw_dsc.class_p = MY_CLASS;
    part_draw_dsc.type = LV_TABLE_DRAW_PART_CELL;
    part_draw_dsc.rect_dsc = &rect_dsc_act;
    part_draw_dsc.label_dsc = &label_dsc_act;

    for(row = 0; row < table->row_cnt; row++) {
        lv_coord_t h_row = table->row_h[row];

        cell_area.y1 = cell_area.y2 + 1;
        cell_area.y2 = cell_area.y1 + h_row - 1;

        if(cell_area.y1 > clip_area.y2) break;

        if(rtl) cell_area.x1 = obj->coords.x2 - bg_right - 1 - scroll_x - border_width;
        else cell_area.x2 = obj->coords.x1 + bg_left - 1 - scroll_x + border_width;

        for(col = 0; col < table->col_cnt; col++) {
            lv_table_cell_ctrl_t ctrl = 0;
            if(table->cell_data[cell]) ctrl = table->cell_data[cell]->ctrl;

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
                lv_table_cell_t * next_cell_data = table->cell_data[cell + col_merge];

                if(is_cell_empty(next_cell_data)) break;

                lv_table_cell_ctrl_t merge_ctrl = (lv_table_cell_ctrl_t) next_cell_data->ctrl;
                if(merge_ctrl & LV_TABLE_CELL_CTRL_MERGE_RIGHT) {
                    lv_coord_t offset = table->col_w[col + col_merge + 1];

                    if(rtl) cell_area.x1 -= offset;
                    else cell_area.x2 += offset;
                }
                else {
                    break;
                }
            }

            if(cell_area.y2 < clip_area.y1) {
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

            part_draw_dsc.draw_area = &cell_area_border;
            part_draw_dsc.id = row * table->col_cnt + col;
            lv_event_send(obj, LV_EVENT_DRAW_PART_BEGIN, &part_draw_dsc);

            lv_draw_rect(draw_ctx, &rect_dsc_act, &cell_area_border);

            if(table->cell_data[cell]) {
                const lv_coord_t cell_left = lv_obj_get_style_pad_left(obj, LV_PART_ITEMS);
                const lv_coord_t cell_right = lv_obj_get_style_pad_right(obj, LV_PART_ITEMS);
                const lv_coord_t cell_top = lv_obj_get_style_pad_top(obj, LV_PART_ITEMS);
                const lv_coord_t cell_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_ITEMS);
                lv_text_flag_t txt_flags = LV_TEXT_FLAG_NONE;
                lv_area_t txt_area;

                txt_area.x1 = cell_area.x1 + cell_left;
                txt_area.x2 = cell_area.x2 - cell_right;
                txt_area.y1 = cell_area.y1 + cell_top;
                txt_area.y2 = cell_area.y2 - cell_bottom;

                /*Align the content to the middle if not cropped*/
                bool crop = ctrl & LV_TABLE_CELL_CTRL_TEXT_CROP ? true : false;
                if(crop) txt_flags = LV_TEXT_FLAG_EXPAND;

                lv_txt_get_size(&txt_size, table->cell_data[cell]->txt, label_dsc_def.font,
                                label_dsc_act.letter_space, label_dsc_act.line_space,
                                lv_area_get_width(&txt_area), txt_flags);

                /*Align the content to the middle if not cropped*/
                if(!crop) {
                    txt_area.y1 = cell_area.y1 + h_row / 2 - txt_size.y / 2;
                    txt_area.y2 = cell_area.y1 + h_row / 2 + txt_size.y / 2;
                }

                lv_area_t label_clip_area;
                bool label_mask_ok;
                label_mask_ok = _lv_area_intersect(&label_clip_area, &clip_area, &cell_area);
                if(label_mask_ok) {
                    draw_ctx->clip_area = &label_clip_area;
                    lv_draw_label(draw_ctx, &label_dsc_act, &txt_area, table->cell_data[cell]->txt, NULL);
                    draw_ctx->clip_area = &clip_area;
                }
            }

            lv_event_send(obj, LV_EVENT_DRAW_PART_END, &part_draw_dsc);

            cell += col_merge + 1;
            col += col_merge;
        }
    }

    draw_ctx->clip_area = clip_area_ori;
}

/* Refreshes size of the table starting from @start_row row */
static void refr_size_form_row(lv_obj_t * obj, uint32_t start_row)
{
    const lv_coord_t cell_pad_left = lv_obj_get_style_pad_left(obj, LV_PART_ITEMS);
    const lv_coord_t cell_pad_right = lv_obj_get_style_pad_right(obj, LV_PART_ITEMS);
    const lv_coord_t cell_pad_top = lv_obj_get_style_pad_top(obj, LV_PART_ITEMS);
    const lv_coord_t cell_pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_ITEMS);

    lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_ITEMS);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_ITEMS);
    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_ITEMS);

    const lv_coord_t minh = lv_obj_get_style_min_height(obj, LV_PART_ITEMS);
    const lv_coord_t maxh = lv_obj_get_style_max_height(obj, LV_PART_ITEMS);

    lv_table_t * table = (lv_table_t *)obj;
    uint32_t i;
    for(i = start_row; i < table->row_cnt; i++) {
        lv_coord_t calculated_height = get_row_height(obj, i, font, letter_space, line_space,
                                                      cell_pad_left, cell_pad_right, cell_pad_top, cell_pad_bottom);
        table->row_h[i] = LV_CLAMP(minh, calculated_height, maxh);
    }

    lv_obj_refresh_self_size(obj);
    lv_obj_invalidate(obj);
}


static void refr_cell_size(lv_obj_t * obj, uint32_t row, uint32_t col)
{
    const lv_coord_t cell_pad_left = lv_obj_get_style_pad_left(obj, LV_PART_ITEMS);
    const lv_coord_t cell_pad_right = lv_obj_get_style_pad_right(obj, LV_PART_ITEMS);
    const lv_coord_t cell_pad_top = lv_obj_get_style_pad_top(obj, LV_PART_ITEMS);
    const lv_coord_t cell_pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_ITEMS);

    lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_ITEMS);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_ITEMS);
    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_ITEMS);

    const lv_coord_t minh = lv_obj_get_style_min_height(obj, LV_PART_ITEMS);
    const lv_coord_t maxh = lv_obj_get_style_max_height(obj, LV_PART_ITEMS);

    lv_table_t * table = (lv_table_t *)obj;
    lv_coord_t calculated_height = get_row_height(obj, row, font, letter_space, line_space,
                                                  cell_pad_left, cell_pad_right, cell_pad_top, cell_pad_bottom);

    lv_coord_t prev_row_size = table->row_h[row];
    table->row_h[row] = LV_CLAMP(minh, calculated_height, maxh);

    /*If the row height havn't changed invalidate only this cell*/
    if(prev_row_size == table->row_h[row]) {
        lv_area_t cell_area;
        get_cell_area(obj, row, col, &cell_area);
        lv_area_move(&cell_area, obj->coords.x1, obj->coords.y1);
        lv_obj_invalidate_area(obj, &cell_area);
    }
    else {
        lv_obj_refresh_self_size(obj);
        lv_obj_invalidate(obj);
    }
}

static lv_coord_t get_row_height(lv_obj_t * obj, uint16_t row_id, const lv_font_t * font,
                                 lv_coord_t letter_space, lv_coord_t line_space,
                                 lv_coord_t cell_left, lv_coord_t cell_right, lv_coord_t cell_top, lv_coord_t cell_bottom)
{
    lv_table_t * table = (lv_table_t *)obj;

    lv_coord_t h_max = lv_font_get_line_height(font) + cell_top + cell_bottom;
    /* Calculate the cell_data index where to start */
    uint16_t row_start = row_id * table->col_cnt;

    /* Traverse the cells in the row_id row */
    uint16_t cell;
    uint16_t col;
    for(cell = row_start, col = 0; cell < row_start + table->col_cnt; cell++, col++) {
        lv_table_cell_t * cell_data = table->cell_data[cell];

        if(is_cell_empty(cell_data)) {
            continue;
        }

        lv_coord_t txt_w = table->col_w[col];

        /* Traverse the current row from the first until the penultimate column.
         * Increment the text width if the cell has the LV_TABLE_CELL_CTRL_MERGE_RIGHT control,
         * exit the traversal when the current cell control is not LV_TABLE_CELL_CTRL_MERGE_RIGHT */
        uint16_t col_merge = 0;
        for(col_merge = 0; col_merge + col < table->col_cnt - 1; col_merge++) {
            lv_table_cell_t * next_cell_data = table->cell_data[cell + col_merge];

            if(is_cell_empty(next_cell_data)) break;

            lv_table_cell_ctrl_t ctrl = (lv_table_cell_ctrl_t) next_cell_data->ctrl;
            if(ctrl & LV_TABLE_CELL_CTRL_MERGE_RIGHT) {
                txt_w += table->col_w[col + col_merge + 1];
            }
            else {
                break;
            }
        }

        lv_table_cell_ctrl_t ctrl = (lv_table_cell_ctrl_t) cell_data->ctrl;

        /*When cropping the text we can assume the row height is equal to the line height*/
        if(ctrl & LV_TABLE_CELL_CTRL_TEXT_CROP) {
            h_max = LV_MAX(lv_font_get_line_height(font) + cell_top + cell_bottom,
                           h_max);
        }
        /*Else we have to calculate the height of the cell text*/
        else {
            lv_point_t txt_size;
            txt_w -= cell_left + cell_right;

            lv_txt_get_size(&txt_size, table->cell_data[cell]->txt, font,
                            letter_space, line_space, txt_w, LV_TEXT_FLAG_NONE);

            h_max = LV_MAX(txt_size.y + cell_top + cell_bottom, h_max);
            /*Skip until one element after the last merged column*/
            cell += col_merge;
            col += col_merge;
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

/* Returns number of bytes to allocate based on chars configuration */
static size_t get_cell_txt_len(const char * txt)
{
    size_t retval = 0;

#if LV_USE_ARABIC_PERSIAN_CHARS
    retval = sizeof(lv_table_cell_t) + _lv_txt_ap_calc_bytes_cnt(txt) + 1;
#else
    retval = sizeof(lv_table_cell_t) + strlen(txt) + 1;
#endif

    return retval;
}

/* Copy txt into dst skipping the format byte */
static void copy_cell_txt(lv_table_cell_t * dst, const char * txt)
{
#if LV_USE_ARABIC_PERSIAN_CHARS
    _lv_txt_ap_proc(txt, dst->txt);
#else
    strcpy(dst->txt, txt);
#endif
}

static void get_cell_area(lv_obj_t * obj, uint16_t row, uint16_t col, lv_area_t * area)
{
    lv_table_t * table = (lv_table_t *)obj;

    uint32_t c;
    area->x1 = 0;
    for(c = 0; c < col; c++) {
        area->x1 += table->col_w[c];
    }

    bool rtl = lv_obj_get_style_base_dir(obj, LV_PART_MAIN) == LV_BASE_DIR_RTL;
    if(rtl) {
        area->x1 += lv_obj_get_scroll_x(obj);
        lv_coord_t w = lv_obj_get_width(obj);
        area->x2 = w - area->x1 - lv_obj_get_style_pad_right(obj, 0);
        area->x1 = area->x2 - table->col_w[col];
    }
    else {
        area->x1 -= lv_obj_get_scroll_x(obj);
        area->x1 += lv_obj_get_style_pad_left(obj, 0);
        area->x2 = area->x1 + table->col_w[col] - 1;
    }

    uint32_t r;
    area->y1 = 0;
    for(r = 0; r < row; r++) {
        area->y1 += table->row_h[r];
    }

    area->y1 += lv_obj_get_style_pad_top(obj, 0);
    area->y1 -= lv_obj_get_scroll_y(obj);
    area->y2 = area->y1 + table->row_h[row] - 1;

}


static void scroll_to_selected_cell(lv_obj_t * obj)
{
    lv_table_t * table = (lv_table_t *)obj;

    lv_area_t a;
    get_cell_area(obj, table->row_act, table->col_act, &a);
    if(a.x1 < 0) {
        lv_obj_scroll_by_bounded(obj, -a.x1, 0, LV_ANIM_ON);
    }
    else if(a.x2 > lv_obj_get_width(obj)) {
        lv_obj_scroll_by_bounded(obj, lv_obj_get_width(obj) - a.x2, 0, LV_ANIM_ON);
    }

    if(a.y1 < 0) {
        lv_obj_scroll_by_bounded(obj, 0, -a.y1, LV_ANIM_ON);
    }
    else if(a.y2 > lv_obj_get_height(obj)) {
        lv_obj_scroll_by_bounded(obj, 0, lv_obj_get_height(obj) - a.y2, LV_ANIM_ON);
    }

}
#endif
