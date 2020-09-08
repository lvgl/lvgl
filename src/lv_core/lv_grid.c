/**
 * @file lv_align.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_grid.h"
#include "lv_obj.h"

/*********************
 *      DEFINES
 *********************/
#define CALC_DSC_BUF_SIZE   8

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint32_t col;
    uint32_t row;
    lv_point_t grid_abs;
}item_repos_hint_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void calc_explicit_cols(lv_obj_t * cont, _lv_grid_calc_t * calc);
static void calc_explicit_rows(lv_obj_t * cont, _lv_grid_calc_t * calc);
static void calc_implicit_cols(lv_obj_t * cont, _lv_grid_calc_t * calc);
static void calc_implicit_rows(lv_obj_t * cont, _lv_grid_calc_t * calc);
static void item_repos(lv_obj_t * cont, lv_obj_t * item, _lv_grid_calc_t * calc, item_repos_hint_t * hint);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_coord_t col_dsc_calc_buf[CALC_DSC_BUF_SIZE];
static lv_coord_t row_dsc_calc_buf[CALC_DSC_BUF_SIZE];
static bool col_dsc_buf_used;
static bool row_dsc_buf_used;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Calculate the grid cells coordinates
 * @param obj an object that has a grid
 * @param calc store the calculated cells sizes here
 * @note `_lv_grid_calc_free(calc_out)` needs to be called when `calc_out` is not needed anymore
 */
void _lv_grid_calc(struct _lv_obj_t * obj, _lv_grid_calc_t * calc_out)
{
    if(obj->grid == NULL) return;
//    printf("calc: %d, %d\n", obj->grid->col_dsc_len, obj->grid->row_dsc_len);

    if(obj->grid->col_dsc && obj->grid->row_dsc) {
        calc_explicit_rows(obj, calc_out);
        calc_explicit_cols(obj, calc_out);
    }
    else if(obj->grid->col_dsc && !obj->grid->row_dsc) {
        calc_explicit_cols(obj, calc_out);
        calc_implicit_rows(obj, calc_out);
    }
    else if(!obj->grid->col_dsc && obj->grid->row_dsc) {
        calc_implicit_cols(obj, calc_out);
        calc_explicit_rows(obj, calc_out);
    }

}

/**
 * Free the a grid calculation's data
 * @param calc pointer to the calculated gtrid cell coordinates
 */
void _lv_grid_calc_free(_lv_grid_calc_t * calc)
{
    _lv_mem_buf_release(calc->col_dsc);
    _lv_mem_buf_release(calc->row_dsc);
}

/**
 * Check if the object's grid columns has FR cells or not
 * @param obj pointer to an object
 * @return true: has FR; false: has no FR
 */
bool _lv_grid_has_fr_col(struct _lv_obj_t * obj)
{
    if(obj->grid == NULL) return false;
    if(obj->grid->col_dsc == NULL)  return false;

    uint32_t i;
    for(i = 0; i < obj->grid->col_dsc_len; i++) {
        if(_GRID_IS_FR(obj->grid->col_dsc[i])) return true;
    }

    return false;
}

/**
 * Check if the object's grid rows has FR cells or not
 * @param obj pointer to an object
 * @return true: has FR; false: has no FR
 */
bool _lv_grid_has_fr_row(struct _lv_obj_t * obj)
{
    if(obj->grid == NULL) return false;
    if(obj->grid->row_dsc == NULL)  return false;

    uint32_t i;
    for(i = 0; i < obj->grid->row_dsc_len; i++) {
        if(_GRID_IS_FR(obj->grid->row_dsc[i])) return true;
    }

    return false;
}

/**
 * Refresh the all grid item on a container
 * @param cont pointer to a grid container object
 */
void _lv_grid_full_refresh(lv_obj_t * cont)
{
    /*Calculate the grid*/
    if(cont->grid == NULL) return;
    _lv_grid_calc_t calc;
    _lv_grid_calc(cont, &calc);


    item_repos_hint_t hint;
    _lv_memset_00(&hint, sizeof(hint));

    /* Calculate the grids absolute x and y coordinates.
     * It will be used as helper during item repositioning to avoid calculating this value for every children*/
    lv_coord_t pad_left = lv_obj_get_style_pad_left(cont, LV_OBJ_PART_MAIN);
    lv_coord_t pad_top = lv_obj_get_style_pad_top(cont, LV_OBJ_PART_MAIN);
    hint.grid_abs.x = pad_left + cont->coords.x1 - lv_obj_get_scroll_left(cont);
    hint.grid_abs.y = pad_top + cont->coords.y1 - lv_obj_get_scroll_top(cont);

    lv_obj_t * item = lv_obj_get_child_back(cont, NULL);
    while(item) {
        if(_GRID_IS_CELL(item->x_set) && _GRID_IS_CELL(item->y_set)) {
            item_repos(cont, item, &calc, &hint);
        }
        item = lv_obj_get_child_back(cont, item);
    }
    _lv_grid_calc_free(&calc);

    if(cont->w_set == LV_SIZE_AUTO || cont->h_set == LV_SIZE_AUTO) {
        lv_obj_set_size(cont, cont->w_set, cont->h_set);
    }
}

/**
 * Refresh the position of a grid item
 * @param item pointer to a grid item
 */
void lv_grid_item_refr_pos(lv_obj_t * item)
{
    /*Calculate the grid*/
    lv_obj_t * cont = lv_obj_get_parent(item);
    if(cont == NULL) return;
    if(cont->grid == NULL) return;
    _lv_grid_calc_t calc;
    _lv_grid_calc(cont, &calc);

    item_repos(cont, item, &calc, NULL);

    _lv_grid_calc_free(&calc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void calc_explicit_cols(lv_obj_t * cont, _lv_grid_calc_t * calc)
{
    lv_grid_t * grid = cont->grid;
    uint32_t i;

    lv_coord_t * cols_w;
    if(!col_dsc_buf_used && grid->col_dsc_len <= CALC_DSC_BUF_SIZE) {
        cols_w = col_dsc_calc_buf;
        col_dsc_buf_used = true;
    } else {
        cols_w = _lv_mem_buf_get(sizeof(lv_coord_t) * grid->col_dsc_len);
    }

    uint32_t col_fr_cnt = 0;
    lv_coord_t grid_w = 0;
    bool auto_w = cont->w_set == LV_SIZE_AUTO ? true : false;

    for(i = 0; i < grid->col_dsc_len; i++) {
        lv_coord_t x = grid->col_dsc[i];
        if(_GRID_IS_FR(x)) col_fr_cnt += _GRID_GET_FR(x);
        else {
            cols_w[i] = x;
            grid_w += x;
        }
    }

    lv_coord_t free_w = lv_obj_get_width_fit(cont) - grid_w;


    for(i = 0; i < grid->col_dsc_len; i++) {
        lv_coord_t x = grid->col_dsc[i];
        if(_GRID_IS_FR(x)) {
            if(auto_w) cols_w[i] = 0;   /*Fr is considered zero if the cont has auto width*/
            else {
                lv_coord_t f = _GRID_GET_FR(x);
                cols_w[i] = (free_w * f) / col_fr_cnt;
            }
        }
    }

    calc->col_dsc_len = grid->col_dsc_len + 1;
    calc->col_dsc = _lv_mem_buf_get(sizeof(lv_coord_t) * calc->col_dsc_len);

    calc->col_dsc[0] = 0;
    for(i = 0; i < grid->col_dsc_len; i++) {
        calc->col_dsc[i + 1] = calc->col_dsc[i] + cols_w[i];
    }

    if(col_dsc_buf_used) col_dsc_buf_used = false ;
    else _lv_mem_buf_release(cols_w);
}

static void calc_explicit_rows(lv_obj_t * cont, _lv_grid_calc_t * calc)
{
    lv_grid_t * grid = cont->grid;
    uint32_t i;

    lv_coord_t * rows_h;
    if(!row_dsc_buf_used && grid->row_dsc_len <= CALC_DSC_BUF_SIZE) {
        rows_h = row_dsc_calc_buf;
        row_dsc_buf_used = true;
    } else {
        rows_h = _lv_mem_buf_get(sizeof(lv_coord_t) * grid->row_dsc_len);
    }

    uint32_t row_fr_cnt = 0;
    lv_coord_t grid_h = 0;

    bool auto_h = cont->h_set == LV_SIZE_AUTO ? true : false;


    for(i = 0; i < grid->row_dsc_len; i++) {
        lv_coord_t x = grid->row_dsc[i];
        if(_GRID_IS_FR(x)) row_fr_cnt += _GRID_GET_FR(x);
        else {
            rows_h[i] = x;
            grid_h += x;
        }
    }

    lv_coord_t free_h = lv_obj_get_height_fit(cont) - grid_h;

    for(i = 0; i < grid->row_dsc_len; i++) {
        lv_coord_t x = grid->row_dsc[i];
        if(_GRID_IS_FR(x)) {
            if(auto_h) rows_h[i] = 0;   /*Fr is considered zero if the obj has auto height*/
            else {
                lv_coord_t f = _GRID_GET_FR(x);
                rows_h[i] = (free_h * f) / row_fr_cnt;
            }
        }
    }

    calc->row_dsc_len = grid->row_dsc_len + 1;
    calc->row_dsc = _lv_mem_buf_get(sizeof(lv_coord_t) * calc->row_dsc_len);

    calc->row_dsc[0] = 0;


    for(i = 0; i < grid->row_dsc_len; i++) {
        calc->row_dsc[i + 1] = calc->row_dsc[i] + rows_h[i];
    }

    if(row_dsc_buf_used) row_dsc_buf_used = false;
    else _lv_mem_buf_release(rows_h);
}


static void calc_implicit_cols(lv_obj_t * cont, _lv_grid_calc_t * calc)
{
    lv_grid_t * grid = cont->grid;

    uint32_t child_cnt = lv_obj_count_children(cont);

    uint32_t col_cnt = ((child_cnt + grid->row_dsc_len - 1) / grid->row_dsc_len) + 1; /*+ grid->row_dsc_len - 1 to round up*/
    /* If `col_dsc_buf_used`, nested a call of this func. will release `col_dsc_buf_used` because it think it taken it.
     * So mark that if the buffer was taken in this call*/
    bool col_dsc_buf_mine = false;
    /*At worst case all children is gird item prepare place for all of them*/
    lv_coord_t * cols_w;
    if(!col_dsc_buf_used && col_cnt <= CALC_DSC_BUF_SIZE) {
        cols_w = col_dsc_calc_buf;
        col_dsc_buf_used = true;
        col_dsc_buf_mine = true;
    } else {
        cols_w = _lv_mem_buf_get(sizeof(lv_coord_t) * col_cnt);
    }

    uint32_t i;

    lv_obj_t * child = lv_obj_get_child_back(cont, NULL);
    uint32_t row_i = 0;
    uint32_t col_i = 0;
    cols_w[0] = 0;
    while(child) {
        if(_GRID_IS_CELL(child->x_set) && _GRID_IS_CELL(child->y_set)) {
            lv_coord_t w;
            if(_GRID_GET_CELL_FLAG(child->x_set) == LV_GRID_STRETCH) _lv_obj_calc_auto_size(child,  &w, NULL);
            else w = lv_obj_get_width(child);
            cols_w[col_i] = LV_MATH_MAX(cols_w[col_i], w);
            row_i++;
            if(row_i == grid->row_dsc_len) {
                row_i = 0;
                col_i++;
                cols_w[col_i] = 0;
            }
        }
        child = lv_obj_get_child_back(cont, child);
    }


    calc->col_dsc_len = col_cnt + 1;
    calc->col_dsc = _lv_mem_buf_get(sizeof(lv_coord_t) * calc->col_dsc_len);

    calc->col_dsc[0] = 0;

    for(i = 0; i < col_cnt; i++) {
        calc->col_dsc[i + 1] = calc->col_dsc[i] + cols_w[i];
    }

    if(col_dsc_buf_used && col_dsc_buf_mine) col_dsc_buf_used = false;
    else _lv_mem_buf_release(cols_w);
}


static void calc_implicit_rows(lv_obj_t * cont, _lv_grid_calc_t * calc)
{

    lv_grid_t * grid = cont->grid;
    uint32_t child_cnt = lv_obj_count_children(cont);
    uint32_t row_cnt = ((child_cnt + grid->col_dsc_len - 1) / grid->col_dsc_len) + 1; /*+ grid->col_dsc_len - 1 to round up*/
    bool row_dsc_buf_mine = false;
    /*At worst case all children is gird item prepare place for all of them*/
    lv_coord_t * rows_h;
    if(!row_dsc_buf_used && row_cnt <= CALC_DSC_BUF_SIZE) {
        rows_h = row_dsc_calc_buf;
        row_dsc_buf_used = true;
        row_dsc_buf_mine = true;
    } else {
        rows_h = _lv_mem_buf_get(sizeof(lv_coord_t) * row_cnt);
    }

    uint32_t i;

    lv_obj_t * child = lv_obj_get_child_back(cont, NULL);
    uint32_t col_i = 0;
    uint32_t row_i = 0;
    rows_h[0] = 0;
    while(child) {
        if(_GRID_IS_CELL(child->x_set) && _GRID_IS_CELL(child->y_set)) {
            lv_coord_t h;
            if(_GRID_GET_CELL_FLAG(child->y_set) == LV_GRID_STRETCH) _lv_obj_calc_auto_size(child,  NULL, &h);
            else h = lv_obj_get_height(child);
            rows_h[row_i] = LV_MATH_MAX(rows_h[row_i], h);
            col_i++;
            if(col_i == grid->col_dsc_len) {
                col_i = 0;
                row_i++;
                rows_h[row_i] = 0;
            }
        }
        child = lv_obj_get_child_back(cont, child);
    }


    calc->row_dsc_len = row_cnt + 1;
    calc->row_dsc = _lv_mem_buf_get(sizeof(lv_coord_t) * calc->row_dsc_len);

    calc->row_dsc[0] = 0;

    for(i = 0; i < row_cnt; i++) {
        calc->row_dsc[i + 1] = calc->row_dsc[i] + rows_h[i];
    }

    if(row_dsc_buf_used && row_dsc_buf_mine) row_dsc_buf_used = false;
    else _lv_mem_buf_release(rows_h);
}


/**
 * Reposition a grid item in its cell
 * @param cont a grid container object
 * @param item a grid item to reposition
 * @param calc the calculated grid of `cont`
 * @param child_id_ext helper value if the ID of the child is know (order from the oldest) else -1
 * @param grid_abs helper value, the absolute position of the grid, NULL if unknown
 */
static void item_repos(lv_obj_t * cont, lv_obj_t * item, _lv_grid_calc_t * calc, item_repos_hint_t * hint)
{
    if(_lv_obj_is_grid_item(item) == false) return;

    uint32_t col_pos;
    uint32_t col_span;
    uint32_t row_pos;
    uint32_t row_span;

    if(cont->grid->row_dsc && cont->grid->col_dsc) {
        col_pos = _GRID_GET_CELL_POS(item->x_set);
        col_span = _GRID_GET_CELL_SPAN(item->x_set);
        row_pos = _GRID_GET_CELL_POS(item->y_set);
        row_span = _GRID_GET_CELL_SPAN(item->y_set);
    } else {
        col_span = 1;
        row_span = 1;

        if(hint) {
            col_pos = hint->col;
            row_pos = hint->row;

            if(cont->grid->row_dsc == NULL) {
                hint->col++;
                if(hint->col >= cont->grid->col_dsc_len) {
                    hint->col = 0;
                    hint->row++;
                }
            } else {
                if(hint->row >= cont->grid->row_dsc_len) {
                    hint->row = 0;
                    hint->col++;
                }
            }
        }
        /*No hint -> find the child ID and calculate its col and row position */
        else {
            uint32_t child_id = 0;
            lv_obj_t * child = lv_obj_get_child_back(cont, NULL);

            while(child) {
                if(child == item) break;
                if(_GRID_IS_CELL(child->x_set) && _GRID_IS_CELL(child->y_set)) {
                    child_id++;
                }
                child = lv_obj_get_child_back(cont, child);
            }

            if(cont->grid->row_dsc == NULL) {
                col_pos = child_id % cont->grid->col_dsc_len;
                row_pos = child_id / cont->grid->col_dsc_len;
            } else {
                col_pos = child_id / cont->grid->row_dsc_len;
                row_pos = child_id % cont->grid->row_dsc_len;
            }
        }
    }

    lv_coord_t col_w = calc->col_dsc[col_pos + col_span] - calc->col_dsc[col_pos];
    lv_coord_t row_h = calc->row_dsc[row_pos + row_span] - calc->row_dsc[row_pos];

    uint8_t x_flag = _GRID_GET_CELL_FLAG(item->x_set);
    uint8_t y_flag = _GRID_GET_CELL_FLAG(item->y_set);

    lv_coord_t x;
    lv_coord_t y;
    lv_coord_t w = lv_obj_get_width(item);
    lv_coord_t h = lv_obj_get_height(item);

    switch(x_flag) {
        case LV_GRID_START:
            x = calc->col_dsc[col_pos];
            break;
        case LV_GRID_STRETCH:
            x = calc->col_dsc[col_pos];
            w = col_w;
            item->w_set = LV_SIZE_STRETCH;
            break;
        case LV_GRID_CENTER:
            x = calc->col_dsc[col_pos] + (col_w - w) / 2;
            break;
        case LV_GRID_END:
            x = calc->col_dsc[col_pos + 1] - lv_obj_get_width(item);
            break;
    }

    switch(y_flag) {
        case LV_GRID_START:
            y = calc->row_dsc[row_pos];
            break;
        case LV_GRID_STRETCH:
            y = calc->row_dsc[row_pos];
            item->h_set = LV_SIZE_STRETCH;
            h = row_h;
            break;
        case LV_GRID_CENTER:
            y = calc->row_dsc[row_pos] + (row_h - h) / 2;
            break;
        case LV_GRID_END:
            y = calc->row_dsc[row_pos + 1] - lv_obj_get_height(item);
            break;
    }

    /*Set a new size if required*/
    if(lv_obj_get_width(item) != w || lv_obj_get_height(item) != h) {
        lv_area_t old_coords;
        lv_area_copy(&old_coords, &item->coords);
        lv_obj_invalidate(item);
        lv_area_set_width(&item->coords, w);
        lv_area_set_height(&item->coords, h);
        lv_obj_invalidate(item);
        item->signal_cb(item, LV_SIGNAL_COORD_CHG, &old_coords);

        /* If a children is a grid container and has an FR field it also needs to be updated
         * because the FR cell size will change with child size change. */
        lv_obj_t * child = lv_obj_get_child(item, NULL);
        while(child) {
            if(_lv_grid_has_fr_col(child) || _lv_grid_has_fr_row(child)) {
                _lv_grid_full_refresh(child);
            }
            child = lv_obj_get_child(item, child);
        }
    }
    bool moved = true;
    if(hint) {
        if(hint->grid_abs.x + x == item->coords.x1 && hint->grid_abs.y + y == item->coords.y1) moved = false;
    }

    if(moved) _lv_obj_move_to(item, x, y, false);
}
