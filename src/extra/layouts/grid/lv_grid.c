/**
 * @file lv_grid.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_layouts.h"

#if LV_USE_GRID

/*********************
 *      DEFINES
 *********************/
/**
 * Some helper defines
 */
#define IS_FR(x)       (x >= LV_COORD_MAX - 100)
#define IS_CONTENT(x)  (x == LV_COORD_MAX - 101)
#define GET_FR(x)      (x - (LV_COORD_MAX - 100))

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint32_t col;
    uint32_t row;
    lv_point_t grid_abs;
} item_repos_hint_t;

typedef struct {
    lv_coord_t * x;
    lv_coord_t * y;
    lv_coord_t * w;
    lv_coord_t * h;
    uint32_t col_num;
    uint32_t row_num;
    lv_coord_t grid_w;
    lv_coord_t grid_h;
} _lv_grid_calc_t;


/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void grid_update(lv_obj_t * cont, void * user_data);
static void calc(lv_obj_t * obj, _lv_grid_calc_t * calc);
static void calc_free(_lv_grid_calc_t * calc);
static void calc_cols(lv_obj_t * cont, _lv_grid_calc_t * c);
static void calc_rows(lv_obj_t * cont, _lv_grid_calc_t * c);
static void item_repos(lv_obj_t * item, _lv_grid_calc_t * c, item_repos_hint_t * hint);
static lv_coord_t grid_align(lv_coord_t cont_size,  bool auto_size, uint8_t align, lv_coord_t gap, uint32_t track_num,
                             lv_coord_t * size_array, lv_coord_t * pos_array, bool reverse);
static uint32_t count_tracks(const lv_coord_t * templ);

static inline const lv_coord_t * get_col_dsc(lv_obj_t * obj)
{
    return lv_obj_get_style_grid_column_dsc_array(obj, 0);
}
static inline const lv_coord_t * get_row_dsc(lv_obj_t * obj)
{
    return lv_obj_get_style_grid_row_dsc_array(obj, 0);
}
static inline uint8_t get_col_pos(lv_obj_t * obj)
{
    return lv_obj_get_style_grid_cell_column_pos(obj, 0);
}
static inline uint8_t get_row_pos(lv_obj_t * obj)
{
    return lv_obj_get_style_grid_cell_row_pos(obj, 0);
}
static inline uint8_t get_col_span(lv_obj_t * obj)
{
    return lv_obj_get_style_grid_cell_column_span(obj, 0);
}
static inline uint8_t get_row_span(lv_obj_t * obj)
{
    return lv_obj_get_style_grid_cell_row_span(obj, 0);
}
static inline uint8_t get_cell_col_align(lv_obj_t * obj)
{
    return lv_obj_get_style_grid_cell_x_align(obj, 0);
}
static inline uint8_t get_cell_row_align(lv_obj_t * obj)
{
    return lv_obj_get_style_grid_cell_y_align(obj, 0);
}
static inline uint8_t get_grid_col_align(lv_obj_t * obj)
{
    return lv_obj_get_style_grid_column_align(obj, 0);
}
static inline uint8_t get_grid_row_align(lv_obj_t * obj)
{
    return lv_obj_get_style_grid_row_align(obj, 0);
}

/**********************
 *  GLOBAL VARIABLES
 **********************/
uint32_t LV_LAYOUT_GRID;
lv_style_prop_t LV_STYLE_GRID_COLUMN_DSC_ARRAY;
lv_style_prop_t LV_STYLE_GRID_COLUMN_ALIGN;
lv_style_prop_t LV_STYLE_GRID_ROW_DSC_ARRAY;
lv_style_prop_t LV_STYLE_GRID_ROW_ALIGN;
lv_style_prop_t LV_STYLE_GRID_CELL_COLUMN_POS;
lv_style_prop_t LV_STYLE_GRID_CELL_COLUMN_SPAN;
lv_style_prop_t LV_STYLE_GRID_CELL_X_ALIGN;
lv_style_prop_t LV_STYLE_GRID_CELL_ROW_POS;
lv_style_prop_t LV_STYLE_GRID_CELL_ROW_SPAN;
lv_style_prop_t LV_STYLE_GRID_CELL_Y_ALIGN;

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


void lv_grid_init(void)
{
    LV_LAYOUT_GRID = lv_layout_register(grid_update, NULL);

    LV_STYLE_GRID_COLUMN_DSC_ARRAY = lv_style_register_prop() | LV_STYLE_PROP_LAYOUT_REFR;
    LV_STYLE_GRID_ROW_DSC_ARRAY = lv_style_register_prop() | LV_STYLE_PROP_LAYOUT_REFR;
    LV_STYLE_GRID_COLUMN_ALIGN = lv_style_register_prop() | LV_STYLE_PROP_LAYOUT_REFR;
    LV_STYLE_GRID_ROW_ALIGN = lv_style_register_prop() | LV_STYLE_PROP_LAYOUT_REFR;

    LV_STYLE_GRID_CELL_ROW_SPAN = lv_style_register_prop() | LV_STYLE_PROP_LAYOUT_REFR;
    LV_STYLE_GRID_CELL_ROW_POS = lv_style_register_prop() | LV_STYLE_PROP_LAYOUT_REFR;
    LV_STYLE_GRID_CELL_COLUMN_SPAN = lv_style_register_prop() | LV_STYLE_PROP_LAYOUT_REFR;
    LV_STYLE_GRID_CELL_COLUMN_POS = lv_style_register_prop() | LV_STYLE_PROP_LAYOUT_REFR;
    LV_STYLE_GRID_CELL_X_ALIGN = lv_style_register_prop() | LV_STYLE_PROP_LAYOUT_REFR;
    LV_STYLE_GRID_CELL_Y_ALIGN = lv_style_register_prop() | LV_STYLE_PROP_LAYOUT_REFR;
}

void lv_obj_set_grid_dsc_array(lv_obj_t * obj, const lv_coord_t col_dsc[], const lv_coord_t row_dsc[])
{
    lv_obj_set_style_grid_column_dsc_array(obj, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(obj, row_dsc, 0);
    lv_obj_set_style_layout(obj, LV_LAYOUT_GRID, 0);
}

void lv_obj_set_grid_align(lv_obj_t * obj, lv_grid_align_t column_align, lv_grid_align_t row_align)
{
    lv_obj_set_style_grid_column_align(obj, column_align, 0);
    lv_obj_set_style_grid_row_align(obj, row_align, 0);

}

void lv_obj_set_grid_cell(lv_obj_t * obj, lv_grid_align_t x_align, uint8_t col_pos, uint8_t col_span,
                          lv_grid_align_t y_align, uint8_t row_pos, uint8_t row_span)

{
    lv_obj_set_style_grid_cell_column_pos(obj, col_pos, 0);
    lv_obj_set_style_grid_cell_row_pos(obj, row_pos, 0);
    lv_obj_set_style_grid_cell_x_align(obj, x_align, 0);
    lv_obj_set_style_grid_cell_column_span(obj, col_span, 0);
    lv_obj_set_style_grid_cell_row_span(obj, row_span, 0);
    lv_obj_set_style_grid_cell_y_align(obj, y_align, 0);

    lv_obj_mark_layout_as_dirty(lv_obj_get_parent(obj));
}


void lv_style_set_grid_row_dsc_array(lv_style_t * style, const lv_coord_t value[])
{
    lv_style_value_t v = {
        .ptr = (const void *)value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_ROW_DSC_ARRAY, v);
}

void lv_style_set_grid_column_dsc_array(lv_style_t * style, const lv_coord_t value[])
{
    lv_style_value_t v = {
        .ptr = (const void *)value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_COLUMN_DSC_ARRAY, v);
}

void lv_style_set_grid_row_align(lv_style_t * style, lv_grid_align_t value)
{
    lv_style_value_t v = {
        .num = (lv_grid_align_t)value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_ROW_ALIGN, v);
}

void lv_style_set_grid_column_align(lv_style_t * style, lv_grid_align_t value)
{
    lv_style_value_t v = {
        .num = (lv_grid_align_t)value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_COLUMN_ALIGN, v);
}


void lv_style_set_grid_cell_column_pos(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_CELL_COLUMN_POS, v);
}

void lv_style_set_grid_cell_column_span(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_CELL_COLUMN_SPAN, v);
}

void lv_style_set_grid_cell_row_pos(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_CELL_ROW_POS, v);
}

void lv_style_set_grid_cell_row_span(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_CELL_ROW_SPAN, v);
}

void lv_style_set_grid_cell_x_align(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_CELL_X_ALIGN, v);
}

void lv_style_set_grid_cell_y_align(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_style_set_prop(style, LV_STYLE_GRID_CELL_Y_ALIGN, v);
}

void lv_obj_set_style_grid_row_dsc_array(lv_obj_t * obj, const lv_coord_t value[], lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = (const void *)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_GRID_ROW_DSC_ARRAY, v, selector);
}

void lv_obj_set_style_grid_column_dsc_array(lv_obj_t * obj, const lv_coord_t value[], lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = (const void *)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_GRID_COLUMN_DSC_ARRAY, v, selector);
}


void lv_obj_set_style_grid_row_align(lv_obj_t * obj, lv_grid_align_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t) value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_GRID_ROW_ALIGN, v, selector);
}

void lv_obj_set_style_grid_column_align(lv_obj_t * obj, lv_grid_align_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t) value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_GRID_COLUMN_ALIGN, v, selector);
}


void lv_obj_set_style_grid_cell_column_pos(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_GRID_CELL_COLUMN_POS, v, selector);
}

void lv_obj_set_style_grid_cell_column_span(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_GRID_CELL_COLUMN_SPAN, v, selector);
}

void lv_obj_set_style_grid_cell_row_pos(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_GRID_CELL_ROW_POS, v, selector);
}

void lv_obj_set_style_grid_cell_row_span(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_GRID_CELL_ROW_SPAN, v, selector);
}

void lv_obj_set_style_grid_cell_x_align(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_GRID_CELL_X_ALIGN, v, selector);
}

void lv_obj_set_style_grid_cell_y_align(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_GRID_CELL_Y_ALIGN, v, selector);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void grid_update(lv_obj_t * cont, void * user_data)
{
    LV_LOG_INFO("update %p container", cont);
    LV_UNUSED(user_data);

    const lv_coord_t * col_templ = get_col_dsc(cont);
    const lv_coord_t * row_templ = get_row_dsc(cont);
    if(col_templ == NULL || row_templ == NULL) return;

    _lv_grid_calc_t c;
    calc(cont, &c);

    item_repos_hint_t hint;
    lv_memset_00(&hint, sizeof(hint));

    /*Calculate the grids absolute x and y coordinates.
     *It will be used as helper during item repositioning to avoid calculating this value for every children*/
    lv_coord_t border_widt = lv_obj_get_style_border_width(cont, LV_PART_MAIN);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(cont, LV_PART_MAIN) + border_widt;
    lv_coord_t pad_top = lv_obj_get_style_pad_top(cont, LV_PART_MAIN) + border_widt;
    hint.grid_abs.x = pad_left + cont->coords.x1 - lv_obj_get_scroll_x(cont);
    hint.grid_abs.y = pad_top + cont->coords.y1 - lv_obj_get_scroll_y(cont);

    uint32_t i;
    for(i = 0; i < cont->spec_attr->child_cnt; i++) {
        lv_obj_t * item = cont->spec_attr->children[i];
        item_repos(item, &c, &hint);
    }
    calc_free(&c);

    lv_coord_t w_set = lv_obj_get_style_width(cont, LV_PART_MAIN);
    lv_coord_t h_set = lv_obj_get_style_height(cont, LV_PART_MAIN);
    if(w_set == LV_SIZE_CONTENT || h_set == LV_SIZE_CONTENT) {
        lv_obj_refr_size(cont);
    }

    lv_event_send(cont, LV_EVENT_LAYOUT_CHANGED, NULL);

    LV_TRACE_LAYOUT("finished");
}

/**
 * Calculate the grid cells coordinates
 * @param cont an object that has a grid
 * @param calc store the calculated cells sizes here
 * @note `_lv_grid_calc_free(calc_out)` needs to be called when `calc_out` is not needed anymore
 */
static void calc(lv_obj_t * cont, _lv_grid_calc_t * calc_out)
{
    if(lv_obj_get_child(cont, 0) == NULL) {
        lv_memset_00(calc_out, sizeof(_lv_grid_calc_t));
        return;
    }

    calc_rows(cont, calc_out);
    calc_cols(cont, calc_out);

    lv_coord_t col_gap = lv_obj_get_style_pad_column(cont, LV_PART_MAIN);
    lv_coord_t row_gap = lv_obj_get_style_pad_row(cont, LV_PART_MAIN);

    bool rev = lv_obj_get_style_base_dir(cont, LV_PART_MAIN) == LV_BASE_DIR_RTL ? true : false;

    lv_coord_t w_set = lv_obj_get_style_width(cont, LV_PART_MAIN);
    lv_coord_t h_set = lv_obj_get_style_height(cont, LV_PART_MAIN);
    bool auto_w = (w_set == LV_SIZE_CONTENT && !cont->w_layout) ? true : false;
    lv_coord_t cont_w = lv_obj_get_content_width(cont);
    calc_out->grid_w = grid_align(cont_w, auto_w, get_grid_col_align(cont), col_gap, calc_out->col_num, calc_out->w,
                                  calc_out->x, rev);

    bool auto_h = (h_set == LV_SIZE_CONTENT && !cont->h_layout) ? true : false;
    lv_coord_t cont_h = lv_obj_get_content_height(cont);
    calc_out->grid_h = grid_align(cont_h, auto_h, get_grid_row_align(cont), row_gap, calc_out->row_num, calc_out->h,
                                  calc_out->y, false);

    LV_ASSERT_MEM_INTEGRITY();
}

/**
 * Free the a grid calculation's data
 * @param calc pointer to the calculated gtrid cell coordinates
 */
static void calc_free(_lv_grid_calc_t * calc)
{
    lv_mem_buf_release(calc->x);
    lv_mem_buf_release(calc->y);
    lv_mem_buf_release(calc->w);
    lv_mem_buf_release(calc->h);
}

static void calc_cols(lv_obj_t * cont, _lv_grid_calc_t * c)
{
    const lv_coord_t * col_templ = get_col_dsc(cont);
    lv_coord_t cont_w = lv_obj_get_content_width(cont);

    c->col_num = count_tracks(col_templ);
    c->x = lv_mem_buf_get(sizeof(lv_coord_t) * c->col_num);
    c->w = lv_mem_buf_get(sizeof(lv_coord_t) * c->col_num);

    /*Set sizes for CONTENT cells*/
    uint32_t i;
    for(i = 0; i < c->col_num; i++) {
        lv_coord_t size = LV_COORD_MIN;
        if(IS_CONTENT(col_templ[i])) {
            /*Check the size of children of this cell*/
            uint32_t ci;
            for(ci = 0; ci < lv_obj_get_child_cnt(cont); ci++) {
                lv_obj_t * item = lv_obj_get_child(cont, ci);
                if(lv_obj_has_flag_any(item, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) continue;
                uint32_t col_span = get_col_span(item);
                if(col_span != 1) continue;

                uint32_t col_pos = get_col_pos(item);
                if(col_pos != i) continue;

                size = LV_MAX(size, lv_obj_get_width(item));
            }
            if(size >= 0) c->w[i] = size;
            else c->w[i] = 0;
        }
    }

    uint32_t col_fr_cnt = 0;
    lv_coord_t grid_w = 0;

    for(i = 0; i < c->col_num; i++) {
        lv_coord_t x = col_templ[i];
        if(IS_FR(x)) {
            col_fr_cnt += GET_FR(x);
        }
        else if(IS_CONTENT(x)) {
            grid_w += c->w[i];
        }
        else {
            c->w[i] = x;
            grid_w += x;
        }
    }

    lv_coord_t col_gap = lv_obj_get_style_pad_column(cont, LV_PART_MAIN);
    cont_w -= col_gap * (c->col_num - 1);
    lv_coord_t free_w = cont_w - grid_w;
    if(free_w < 0) free_w = 0;

    int32_t last_fr_i = -1;
    int32_t last_fr_x = 0;
    for(i = 0; i < c->col_num; i++) {
        lv_coord_t x = col_templ[i];
        if(IS_FR(x)) {
            lv_coord_t f = GET_FR(x);
            c->w[i] = (free_w * f) / col_fr_cnt;
            last_fr_i = i;
            last_fr_x = f;
        }
    }

    /*To avoid rounding errors set the last FR track to the remaining size */
    if(last_fr_i >= 0) {
        c->w[last_fr_i] = free_w - ((free_w * (col_fr_cnt - last_fr_x)) / col_fr_cnt);
    }
}

static void calc_rows(lv_obj_t * cont, _lv_grid_calc_t * c)
{
    uint32_t i;
    const lv_coord_t * row_templ = get_row_dsc(cont);
    c->row_num = count_tracks(row_templ);
    c->y = lv_mem_buf_get(sizeof(lv_coord_t) * c->row_num);
    c->h = lv_mem_buf_get(sizeof(lv_coord_t) * c->row_num);
    /*Set sizes for CONTENT cells*/
    for(i = 0; i < c->row_num; i++) {
        lv_coord_t size = LV_COORD_MIN;
        if(IS_CONTENT(row_templ[i])) {
            /*Check the size of children of this cell*/
            uint32_t ci;
            for(ci = 0; ci < lv_obj_get_child_cnt(cont); ci++) {
                lv_obj_t * item = lv_obj_get_child(cont, ci);
                if(lv_obj_has_flag_any(item, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) continue;
                uint32_t row_span = get_row_span(item);
                if(row_span != 1) continue;

                uint32_t row_pos = get_row_pos(item);
                if(row_pos != i) continue;

                size = LV_MAX(size, lv_obj_get_height(item));
            }
            if(size >= 0) c->h[i] = size;
            else c->h[i] = 0;
        }
    }

    uint32_t row_fr_cnt = 0;
    lv_coord_t grid_h = 0;

    for(i = 0; i < c->row_num; i++) {
        lv_coord_t x = row_templ[i];
        if(IS_FR(x)) {
            row_fr_cnt += GET_FR(x);
        }
        else if(IS_CONTENT(x)) {
            grid_h += c->h[i];
        }
        else {
            c->h[i] = x;
            grid_h += x;
        }
    }


    lv_coord_t row_gap = lv_obj_get_style_pad_row(cont, LV_PART_MAIN);
    lv_coord_t cont_h = lv_obj_get_content_height(cont) - row_gap * (c->row_num - 1);
    lv_coord_t free_h = cont_h - grid_h;
    if(free_h < 0) free_h = 0;

    int32_t last_fr_i = -1;
    int32_t last_fr_x = 0;
    for(i = 0; i < c->row_num; i++) {
        lv_coord_t x = row_templ[i];
        if(IS_FR(x)) {
            lv_coord_t f = GET_FR(x);
            c->h[i] = (free_h * f) / row_fr_cnt;
        }
    }

    /*To avoid rounding errors set the last FR track to the remaining size */
    if(last_fr_i >= 0) {
        c->h[last_fr_i] = free_h - ((free_h * (row_fr_cnt - last_fr_x)) / row_fr_cnt);
    }
}

/**
 * Reposition a grid item in its cell
 * @param item a grid item to reposition
 * @param calc the calculated grid of `cont`
 * @param child_id_ext helper value if the ID of the child is know (order from the oldest) else -1
 * @param grid_abs helper value, the absolute position of the grid, NULL if unknown
 */
static void item_repos(lv_obj_t * item, _lv_grid_calc_t * c, item_repos_hint_t * hint)
{
    if(lv_obj_has_flag_any(item, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) return;
    uint32_t col_span = get_col_span(item);
    uint32_t row_span = get_row_span(item);
    if(row_span == 0 || col_span == 0) return;

    uint32_t col_pos = get_col_pos(item);
    uint32_t row_pos = get_row_pos(item);
    lv_grid_align_t col_align = get_cell_col_align(item);
    lv_grid_align_t row_align = get_cell_row_align(item);


    lv_coord_t col_x1 = c->x[col_pos];
    lv_coord_t col_x2 = c->x[col_pos + col_span - 1] + c->w[col_pos + col_span - 1];
    lv_coord_t col_w = col_x2 - col_x1;

    lv_coord_t row_y1 = c->y[row_pos];
    lv_coord_t row_y2 = c->y[row_pos + row_span - 1] + c->h[row_pos + row_span - 1];
    lv_coord_t row_h = row_y2 - row_y1;


    /*If the item has RTL base dir switch start and end*/
    if(lv_obj_get_style_base_dir(item, LV_PART_MAIN) == LV_BASE_DIR_RTL) {
        if(col_align == LV_GRID_ALIGN_START) col_align = LV_GRID_ALIGN_END;
        else if(col_align == LV_GRID_ALIGN_END) col_align = LV_GRID_ALIGN_START;
    }

    lv_coord_t x;
    lv_coord_t y;
    lv_coord_t item_w = lv_area_get_width(&item->coords);
    lv_coord_t item_h = lv_area_get_height(&item->coords);

    switch(col_align) {
        default:
        case LV_GRID_ALIGN_START:
            x = c->x[col_pos];
            item->w_layout = 0;
            break;
        case LV_GRID_ALIGN_STRETCH:
            x = c->x[col_pos];
            item_w = col_w;
            item->w_layout = 1;
            break;
        case LV_GRID_ALIGN_CENTER:
            x = c->x[col_pos] + (col_w - item_w) / 2;
            item->w_layout = 0;
            break;
        case LV_GRID_ALIGN_END:
            x = c->x[col_pos] + col_w - lv_obj_get_width(item);
            item->w_layout = 0;
            break;
    }

    switch(row_align) {
        default:
        case LV_GRID_ALIGN_START:
            y = c->y[row_pos];
            item->h_layout = 0;
            break;
        case LV_GRID_ALIGN_STRETCH:
            y = c->y[row_pos];
            item_h = row_h;
            item->h_layout = 1;
            break;
        case LV_GRID_ALIGN_CENTER:
            y = c->y[row_pos] + (row_h - item_h) / 2;
            item->h_layout = 0;
            break;
        case LV_GRID_ALIGN_END:
            y = c->y[row_pos] + row_h - lv_obj_get_height(item);
            item->h_layout = 0;
            break;
    }

    /*Set a new size if required*/
    if(lv_obj_get_width(item) != item_w || lv_obj_get_height(item) != item_h) {
        lv_area_t old_coords;
        lv_area_copy(&old_coords, &item->coords);
        lv_obj_invalidate(item);
        lv_area_set_width(&item->coords, item_w);
        lv_area_set_height(&item->coords, item_h);
        lv_obj_invalidate(item);
        lv_event_send(item, LV_EVENT_SIZE_CHANGED, &old_coords);
        lv_event_send(lv_obj_get_parent(item), LV_EVENT_CHILD_CHANGED, item);

    }

    /*Handle percentage value of translate*/
    lv_coord_t tr_x = lv_obj_get_style_translate_x(item, LV_PART_MAIN);
    lv_coord_t tr_y = lv_obj_get_style_translate_y(item, LV_PART_MAIN);
    lv_coord_t w = lv_obj_get_width(item);
    lv_coord_t h = lv_obj_get_height(item);
    if(LV_COORD_IS_PCT(tr_x)) tr_x = (w * LV_COORD_GET_PCT(tr_x)) / 100;
    if(LV_COORD_IS_PCT(tr_y)) tr_y = (h * LV_COORD_GET_PCT(tr_y)) / 100;

    x += tr_x;
    y += tr_y;

    lv_coord_t diff_x = hint->grid_abs.x + x - item->coords.x1;
    lv_coord_t diff_y = hint->grid_abs.y + y - item->coords.y1;
    if(diff_x || diff_y) {
        lv_obj_invalidate(item);
        item->coords.x1 += diff_x;
        item->coords.x2 += diff_x;
        item->coords.y1 += diff_y;
        item->coords.y2 += diff_y;
        lv_obj_invalidate(item);
        lv_obj_move_children_by(item, diff_x, diff_y, true);
    }
}

/**
 * Place the grid track according to align methods. It keeps the track sizes but sets their position.
 * It can process both columns or rows according to the passed parameters.
 * @param cont_size size of the containers content area (width/height)
 * @param auto_size true: the container has auto size in the current direction
 * @param align align method
 * @param gap grid gap
 * @param track_num number of tracks
 * @param size_array array with the track sizes
 * @param pos_array write the positions of the tracks here
 * @return the total size of the grid
 */
static lv_coord_t grid_align(lv_coord_t cont_size,  bool auto_size, uint8_t align, lv_coord_t gap, uint32_t track_num,
                             lv_coord_t * size_array, lv_coord_t * pos_array, bool reverse)
{
    lv_coord_t grid_size = 0;
    uint32_t i;

    if(auto_size) {
        pos_array[0] = 0;
    }
    else {
        /*With spaced alignment gap will be calculated from the remaining space*/
        if(align == LV_GRID_ALIGN_SPACE_AROUND || align == LV_GRID_ALIGN_SPACE_BETWEEN || align == LV_GRID_ALIGN_SPACE_EVENLY) {
            gap = 0;
            if(track_num == 1) align = LV_GRID_ALIGN_CENTER;
        }

        /*Get the full grid size with gap*/
        for(i = 0; i < track_num; i++) {
            grid_size += size_array[i] + gap;
        }
        grid_size -= gap;

        /*Calculate the position of the first item and set gap is necessary*/
        switch(align) {
            case LV_GRID_ALIGN_START:
                pos_array[0] = 0;
                break;
            case LV_GRID_ALIGN_CENTER:
                pos_array[0] = (cont_size - grid_size) / 2;
                break;
            case LV_GRID_ALIGN_END:
                pos_array[0] = cont_size - grid_size;
                break;
            case LV_GRID_ALIGN_SPACE_BETWEEN:
                pos_array[0] = 0;
                gap = (lv_coord_t)(cont_size - grid_size) / (lv_coord_t)(track_num - 1);
                break;
            case LV_GRID_ALIGN_SPACE_AROUND:
                gap = (lv_coord_t)(cont_size - grid_size) / (lv_coord_t)(track_num);
                pos_array[0] = gap / 2;
                break;
            case LV_GRID_ALIGN_SPACE_EVENLY:
                gap = (lv_coord_t)(cont_size - grid_size) / (lv_coord_t)(track_num + 1);
                pos_array[0] = gap;
                break;

        }
    }

    /*Set the position of all tracks from the start position, gaps and track sizes*/
    for(i = 0; i < track_num - 1; i++) {
        pos_array[i + 1] = pos_array[i] + size_array[i] + gap;
    }

    lv_coord_t total_gird_size = pos_array[track_num - 1] + size_array[track_num - 1] - pos_array[0];

    if(reverse) {
        for(i = 0; i < track_num; i++) {
            pos_array[i] = cont_size - pos_array[i] - size_array[i];
        }

    }

    /*Return the full size of the grid*/
    return total_gird_size;
}

static uint32_t count_tracks(const lv_coord_t * templ)
{
    uint32_t i;
    for(i = 0; templ[i] != LV_GRID_TEMPLATE_LAST; i++);

    return i;
}


#endif /*LV_USE_GRID*/
