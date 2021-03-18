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
 * */
#define CELL_SHIFT        4
#define CELL_POS_MASK     ((1 << CELL_SHIFT) - 1)
#define CELL_SPAN_MASK    (CELL_POS_MASK << CELL_SHIFT)
#define CELL_FLAG_MASK    (CELL_POS_MASK << (2 * CELL_SHIFT))

#define IS_FR(x)       (LV_COORD_IS_LAYOUT(x) && _LV_COORD_PLAIN(x) < 100)
#define IS_CONTENT(x)  (LV_COORD_IS_LAYOUT(x) && _LV_COORD_PLAIN(x) == 100)
#define GET_FR(x)      (_LV_COORD_PLAIN(x))


/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint32_t col;
    uint32_t row;
    lv_point_t grid_abs;
}item_repos_hint_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/
void lv_obj_move_to(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, bool notify_parent);

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void grid_update(lv_obj_t * cont);
static void full_refresh(lv_obj_t * cont);
static void calc(struct _lv_obj_t * obj, _lv_grid_calc_t * calc);
static void calc_free(_lv_grid_calc_t * calc);
static void calc_cols(lv_obj_t * cont, _lv_grid_calc_t * c);
static void calc_rows(lv_obj_t * cont, _lv_grid_calc_t * c);
static void item_repos(lv_obj_t * item, _lv_grid_calc_t * c, item_repos_hint_t * hint);
static lv_coord_t grid_place(lv_coord_t cont_size,  bool auto_size, uint8_t place, lv_coord_t gap, uint32_t track_num, lv_coord_t * size_array, lv_coord_t * pos_array, bool reverse);

/**********************
 *  GLOBAL VARIABLES
 **********************/
static const lv_coord_t grid_12_template[12] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
                                                LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1)};
const lv_grid_t grid_12 = {.base.update_cb = grid_update, .col_dsc = grid_12_template, .col_dsc_len = 12, .row_dsc = grid_12_template, .row_dsc_len = 12};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#define GET_CELL_POS(c)    ((c) & CELL_POS_MASK)
#define GET_CELL_SPAN(c)   (((c) & CELL_SPAN_MASK) >> CELL_SHIFT)
#define GET_CELL_PLACE(c)  ((c) >> (CELL_SHIFT * 2) & 0x7)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_grid_init(lv_grid_t * grid)
{
    lv_memset_00(grid,sizeof(lv_grid_t));
    grid->base.update_cb = grid_update;
    grid->col_place = LV_GRID_START;
    grid->row_place = LV_GRID_START;
}

void lv_grid_set_template(lv_grid_t * grid, const lv_coord_t * col_dsc, uint8_t col_cnt, const lv_coord_t * row_dsc, uint8_t row_cnt)
{
    grid->col_dsc = col_dsc;
    grid->col_dsc_len = col_cnt;
    grid->row_dsc = row_dsc;
    grid->row_dsc_len = row_cnt;
}


void lv_grid_set_place(lv_grid_t * grid, uint8_t col_place, uint8_t row_place)
{
    grid->col_place = col_place;
    grid->row_place = row_place;
}

void lv_obj_set_grid_cell(lv_obj_t * obj, lv_grid_place_t hor_place, uint8_t col_pos, uint8_t col_span,
                                          lv_grid_place_t ver_place, uint8_t row_pos, uint8_t row_span)

{
    if(!lv_obj_is_layout_positioned(obj)) return;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent->spec_attr->layout_dsc->update_cb  != grid_update) return;

    obj->x_set = LV_COORD_SET_LAYOUT(col_pos | (col_span << CELL_SHIFT) | (hor_place << (CELL_SHIFT * 2)));
    obj->y_set = LV_COORD_SET_LAYOUT(row_pos | (row_span << CELL_SHIFT) | (ver_place << (CELL_SHIFT * 2)));

    lv_obj_mark_layout_as_dirty(parent);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void grid_update(lv_obj_t * cont)
{
    if(cont->spec_attr == NULL) return;
    if(cont->spec_attr->layout_dsc == NULL) return;

    LV_LOG_INFO("update 0x%p container", cont);

    full_refresh(cont);

    LV_TRACE_LAYOUT("finished");
}

/**
 * Refresh the all grid item on a container
 * @param cont pointer to a grid container object
 */
static void full_refresh(lv_obj_t * cont)
{
    const lv_grid_t * g = (const lv_grid_t *)cont->spec_attr->layout_dsc;
    /*Calculate the grid*/
    if(g == NULL) return;
    _lv_grid_calc_t c;
    calc(cont, &c);


    item_repos_hint_t hint;
    lv_memset_00(&hint, sizeof(hint));

    /* Calculate the grids absolute x and y coordinates.
     * It will be used as helper during item repositioning to avoid calculating this value for every children*/
    lv_coord_t pad_left = lv_obj_get_style_pad_left(cont, LV_PART_MAIN);
    lv_coord_t pad_top = lv_obj_get_style_pad_top(cont, LV_PART_MAIN);
    hint.grid_abs.x = pad_left + cont->coords.x1 - lv_obj_get_scroll_x(cont);
    hint.grid_abs.y = pad_top + cont->coords.y1 - lv_obj_get_scroll_y(cont);

    uint32_t i;
    for(i = 0; i < cont->spec_attr->child_cnt; i++) {
        lv_obj_t * item = cont->spec_attr->children[i];
        if(LV_COORD_IS_LAYOUT(item->x_set) && LV_COORD_IS_LAYOUT(item->y_set)) {
            item_repos(item, &c, &hint);
        }
    }
    calc_free(&c);

    if(cont->w_set == LV_SIZE_CONTENT || cont->h_set == LV_SIZE_CONTENT) {
        lv_obj_set_size(cont, cont->w_set, cont->h_set);
    }
}

/**
 * Calculate the grid cells coordinates
 * @param cont an object that has a grid
 * @param calc store the calculated cells sizes here
 * @note `_lv_grid_calc_free(calc_out)` needs to be called when `calc_out` is not needed anymore
 */
static void calc(struct _lv_obj_t * cont, _lv_grid_calc_t * calc_out)
{
    const lv_grid_t * g = (const lv_grid_t *)cont->spec_attr->layout_dsc;
    if(g->col_dsc == NULL || g->row_dsc == NULL) return;
    if(g->col_dsc_len == 0 || g->row_dsc_len == 0) return;

    if(lv_obj_get_child(cont, 0) == NULL) {
        lv_memset_00(calc_out, sizeof(_lv_grid_calc_t));
        return;
    }

    calc_rows(cont, calc_out);
    calc_cols(cont, calc_out);

    lv_coord_t col_gap = lv_obj_get_style_pad_column(cont, LV_PART_MAIN);
    lv_coord_t row_gap = lv_obj_get_style_pad_row(cont, LV_PART_MAIN);

    bool rev = lv_obj_get_base_dir(cont) == LV_BIDI_DIR_RTL ? true : false;
    bool auto_w = cont->w_set == LV_SIZE_CONTENT ? true : false;
    lv_coord_t cont_w = lv_obj_get_width_fit(cont);
    calc_out->grid_w = grid_place(cont_w, auto_w, g->col_place, col_gap, calc_out->col_num, calc_out->w, calc_out->x, rev);

    bool auto_h = cont->h_set == LV_SIZE_CONTENT ? true : false;
    lv_coord_t cont_h = lv_obj_get_height_fit(cont);
    calc_out->grid_h = grid_place(cont_h, auto_h, g->row_place, row_gap, calc_out->row_num, calc_out->h, calc_out->y, false);

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
    const lv_grid_t * grid = (const lv_grid_t *)cont->spec_attr->layout_dsc;
    uint32_t i;

    lv_coord_t cont_w = lv_obj_get_width_fit(cont);

    c->col_num = grid->col_dsc_len;
    c->x = lv_mem_buf_get(sizeof(lv_coord_t) * c->col_num);
    c->w = lv_mem_buf_get(sizeof(lv_coord_t) * c->col_num);

    /*Set sizes for CONTENT cells*/
    for(i = 0; i < c->col_num; i++) {
        lv_coord_t size = LV_COORD_MIN;
        if(IS_CONTENT(grid->col_dsc[i])) {
            /*Check the size of children of this cell*/
            uint32_t ci;
            for(ci = 0; ci < lv_obj_get_child_cnt(cont); ci++) {
                lv_obj_t * item = lv_obj_get_child(cont, ci);
                if(LV_COORD_IS_LAYOUT(item->x_set) == false || LV_COORD_IS_LAYOUT(item->y_set) == false) continue;
                if(lv_obj_has_flag_any(item, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) continue;
                uint32_t col_pos = GET_CELL_POS(item->x_set);
                if(col_pos != i) continue;
                uint32_t col_span = GET_CELL_SPAN(item->x_set);
                if(col_span != 1) continue;

                size = LV_MAX(size, lv_obj_get_width(item));
            }
            if(size >= 0) c->w[i] = size;
            else c->w[i] = 0;
        }
    }

    uint32_t col_fr_cnt = 0;
    lv_coord_t grid_w = 0;

    for(i = 0; i < c->col_num; i++) {
        lv_coord_t x = grid->col_dsc[i];
        if(IS_FR(x)) {
            col_fr_cnt += GET_FR(x);
        }
        else if (IS_CONTENT(x)) {
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

    for(i = 0; i < c->col_num; i++) {
        lv_coord_t x = grid->col_dsc[i];
        if(IS_FR(x)) {
            lv_coord_t f = GET_FR(x);
            c->w[i] = (free_w * f) / col_fr_cnt;
        }
    }
}

static void calc_rows(lv_obj_t * cont, _lv_grid_calc_t * c)
{
    const lv_grid_t * grid = (const lv_grid_t *)cont->spec_attr->layout_dsc;
    uint32_t i;

    c->row_num = grid->row_dsc_len;
    c->y = lv_mem_buf_get(sizeof(lv_coord_t) * c->row_num);
    c->h = lv_mem_buf_get(sizeof(lv_coord_t) * c->row_num);
    /*Set sizes for CONTENT cells*/
    for(i = 0; i < c->row_num; i++) {
        lv_coord_t size = LV_COORD_MIN;
        if(IS_CONTENT(grid->row_dsc[i])) {
            /*Check the size of children of this cell*/
            uint32_t ci;
            for(ci = 0; ci < lv_obj_get_child_cnt(cont); ci++) {
                lv_obj_t * item = lv_obj_get_child(cont, ci);
                if(LV_COORD_IS_LAYOUT(item->x_set) == false || LV_COORD_IS_LAYOUT(item->y_set) == false) continue;
                if(lv_obj_has_flag_any(item, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) continue;
                uint32_t row_pos = GET_CELL_POS(item->y_set);
                if(row_pos != i) continue;
                uint32_t row_span = GET_CELL_SPAN(item->y_set);
                if(row_span != 1) continue;

                size = LV_MAX(size, lv_obj_get_height(item));
            }
            if(size >= 0) c->h[i] = size;
            else c->h[i] = 0;
        }
    }

    uint32_t row_fr_cnt = 0;
    lv_coord_t grid_h = 0;

    for(i = 0; i < c->row_num; i++) {
        lv_coord_t x = grid->row_dsc[i];
        if(IS_FR(x)) {
            row_fr_cnt += GET_FR(x);
        } else if (IS_CONTENT(x)) {
            grid_h += c->h[i];
        } else {
            c->h[i] = x;
            grid_h += x;
        }
    }

    lv_coord_t row_gap = lv_obj_get_style_pad_row(cont, LV_PART_MAIN);
    lv_coord_t cont_h = lv_obj_get_height_fit(cont) - row_gap * (grid->row_dsc_len - 1);
    lv_coord_t free_h = cont_h - grid_h;

    for(i = 0; i < grid->row_dsc_len; i++) {
        lv_coord_t x = grid->row_dsc[i];
        if(IS_FR(x)) {
            lv_coord_t f = GET_FR(x);
            c->h[i] = (free_h * f) / row_fr_cnt;
        }
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
    if(LV_COORD_IS_LAYOUT(item->x_set) == 0 || LV_COORD_IS_LAYOUT(item->y_set) == 0) return;
    if(lv_obj_has_flag_any(item, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) return;
    uint32_t col_span = GET_CELL_SPAN(item->x_set);
    uint32_t row_span = GET_CELL_SPAN(item->y_set);
    if(row_span == 0 || col_span == 0) return;

    uint32_t col_pos = GET_CELL_POS(item->x_set);
    uint32_t row_pos = GET_CELL_POS(item->y_set);

    lv_coord_t col_x1 = c->x[col_pos];
    lv_coord_t col_x2 = c->x[col_pos + col_span - 1] + c->w[col_pos + col_span - 1];
    lv_coord_t col_w = col_x2 - col_x1;

    lv_coord_t row_y1 = c->y[row_pos];
    lv_coord_t row_y2 = c->y[row_pos + row_span - 1] + c->h[row_pos + row_span - 1];
    lv_coord_t row_h = row_y2 - row_y1;

    uint8_t x_flag = GET_CELL_PLACE(item->x_set);
    uint8_t y_flag = GET_CELL_PLACE(item->y_set);

    /*If the item has RTL base dir switch start and end*/
    if(lv_obj_get_base_dir(item) == LV_BIDI_DIR_RTL) {
        if(x_flag == LV_GRID_START) x_flag = LV_GRID_END;
        else if(x_flag == LV_GRID_END) x_flag = LV_GRID_START;
    }

    lv_coord_t x;
    lv_coord_t y;
    lv_coord_t item_w = lv_area_get_width(&item->coords);
    lv_coord_t item_h = lv_area_get_height(&item->coords);

    if(item->w_set == LV_SIZE_LAYOUT) item->w_set = item_w;
    if(item->h_set == LV_SIZE_LAYOUT) item->h_set = item_h;

    switch(x_flag) {
        default:
        case LV_GRID_START:
            x = c->x[col_pos];
            break;
        case LV_GRID_STRETCH:
            x = c->x[col_pos];
            item_w = col_w;
            item->w_set = LV_SIZE_LAYOUT;
            break;
        case LV_GRID_CENTER:
            x = c->x[col_pos] + (col_w - item_w) / 2;
            break;
        case LV_GRID_END:
            x = c->x[col_pos] + col_w - lv_obj_get_width(item);
            break;
    }

    switch(y_flag) {
        default:
        case LV_GRID_START:
            y = c->y[row_pos];
            break;
        case LV_GRID_STRETCH:
            y = c->y[row_pos];
            item_h = row_h;
            item->h_set = LV_SIZE_LAYOUT;
            break;
        case LV_GRID_CENTER:
            y = c->y[row_pos] + (row_h - item_h) / 2;
            break;
        case LV_GRID_END:
            y = c->y[row_pos] + row_h - lv_obj_get_height(item);
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
        lv_event_send(item, LV_EVENT_COORD_CHG, &old_coords);

    }
    bool moved = true;
    if(hint) {
        if(hint->grid_abs.x + x == item->coords.x1 && hint->grid_abs.y + y == item->coords.y1) moved = false;
    }

    if(moved) lv_obj_move_to(item, x, y, false);
}

/**
 * Place the grid track according to place methods. It keeps the track sizes but sets their position.
 * It can process both columns or rows according to the passed parameters.
 * @param cont_size size of the containers content area (width/height)
 * @param auto_size true: the container has auto size in the current direction
 * @param place placeing method
 * @param gap grid gap
 * @param track_num number of tracks
 * @param size_array array with the track sizes
 * @param pos_array write the positions of the tracks here
 * @return the total size of the grid
 */
static lv_coord_t grid_place(lv_coord_t cont_size,  bool auto_size, uint8_t place, lv_coord_t gap, uint32_t track_num, lv_coord_t * size_array, lv_coord_t * pos_array, bool reverse)
{
    lv_coord_t grid_size = 0;
    uint32_t i;

    if(auto_size) {
        pos_array[0] = 0;
    } else {
        /*With spaced placements gap will be calculated from the remaining space*/
        if(place == LV_GRID_SPACE_AROUND || place == LV_GRID_SPACE_BETWEEN || place == LV_GRID_SPACE_EVENLY) {
            gap = 0;
            if(track_num == 1) place = LV_GRID_CENTER;
        }

        /*Get the full grid size with gap*/
        for(i = 0; i < track_num; i++) {
            grid_size += size_array[i] + gap;
        }
        grid_size -= gap;

        /*Calculate the position of the first item and set gap is necessary*/
        switch(place) {
        case LV_GRID_START:
            pos_array[0] = 0;
            break;
        case LV_GRID_CENTER:
            pos_array[0] = (cont_size - grid_size) / 2;
            break;
        case LV_GRID_END:
            pos_array[0] = cont_size - grid_size;
            break;
        case LV_GRID_SPACE_BETWEEN:
            pos_array[0] = 0;
            gap = (lv_coord_t)(cont_size - grid_size) / (lv_coord_t)(track_num - 1);
            break;
        case LV_GRID_SPACE_AROUND:
            gap = (lv_coord_t)(cont_size - grid_size) / (lv_coord_t)(track_num);
            pos_array[0] = gap / 2;
            break;
        case LV_GRID_SPACE_EVENLY:
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

#endif /*LV_USE_GRID*/
