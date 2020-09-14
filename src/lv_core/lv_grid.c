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
#define LV_OBJX_NAME "lv_obj"

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
static lv_coord_t grid_place(lv_coord_t cont_size,  bool auto_size, uint8_t place, lv_coord_t gap, uint32_t track_num, lv_coord_t * size_array, lv_coord_t * pos_array);
static void report_grid_change_core(const lv_grid_t * grid, lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
static const lv_coord_t lv_grid_1_dsc[1] = {LV_GRID_FR(1)};
const lv_grid_t lv_grid_center = {
        .col_dsc = lv_grid_1_dsc,
        .col_dsc_len = 1,
        .col_place = LV_GRID_CENTER,
        .row_place = LV_GRID_CENTER};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Set a grid for an object
 * @param obj pointer to an object
 * @param grid the grid to set
 */
void lv_obj_set_grid(lv_obj_t * obj, const lv_grid_t * grid)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->grid = grid;
    _lv_grid_full_refresh(obj);
}

/**
 * Get the grid of an object
 * @param obj pointer to an object
 * @return the grid, NULL if no grid
 */
const lv_grid_t * lv_obj_get_grid(lv_obj_t * obj, const lv_grid_t * grid)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->grid;
}


/**
 * Notify all object if a style is modified
 * @param grid pointer to a grid. Only the objects with this grid will be notified
 *               (NULL to notify all objects with any grid)
 */
void lv_obj_report_grid_change(const lv_grid_t * grid)
{
    lv_disp_t * d = lv_disp_get_next(NULL);

    while(d) {
        lv_obj_t * i;
        _LV_LL_READ(d->scr_ll, i) {
            report_grid_change_core(grid, i);
        }
        d = lv_disp_get_next(d);
    }
}

/**
 * Calculate the grid cells coordinates
 * @param cont an object that has a grid
 * @param calc store the calculated cells sizes here
 * @note `_lv_grid_calc_free(calc_out)` needs to be called when `calc_out` is not needed anymore
 */
void _lv_grid_calc(struct _lv_obj_t * cont, _lv_grid_calc_t * calc_out)
{
    if(cont->grid == NULL) return;
    if(lv_obj_get_child(cont, NULL) == NULL) {
        _lv_memset_00(calc_out, sizeof(_lv_grid_calc_t));
        return;
    }
//    printf("calc: %d, %d\n", obj->grid->col_dsc_len, obj->grid->row_dsc_len);

    if(cont->grid->col_dsc && cont->grid->row_dsc) {
        calc_explicit_rows(cont, calc_out);
        calc_explicit_cols(cont, calc_out);
    }
    else if(cont->grid->col_dsc && !cont->grid->row_dsc) {
        calc_explicit_cols(cont, calc_out);
        calc_implicit_rows(cont, calc_out);
    }
    else if(!cont->grid->col_dsc && cont->grid->row_dsc) {
        calc_implicit_cols(cont, calc_out);
        calc_explicit_rows(cont, calc_out);
    }

    bool auto_w = cont->w_set == LV_SIZE_AUTO ? true : false;
    lv_coord_t cont_w = lv_obj_get_width_fit(cont);
    calc_out->grid_w = grid_place(cont_w, auto_w, cont->grid->col_place, cont->grid->col_gap, calc_out->col_num, calc_out->w, calc_out->x);

    bool auto_h = cont->h_set == LV_SIZE_AUTO ? true : false;
    lv_coord_t cont_h = lv_obj_get_height_fit(cont);
    calc_out->grid_h = grid_place(cont_h, auto_h, cont->grid->row_place, cont->grid->row_gap, calc_out->row_num, calc_out->h, calc_out->y);

    LV_ASSERT_MEM_INTEGRITY();
}

/**
 * Free the a grid calculation's data
 * @param calc pointer to the calculated gtrid cell coordinates
 */
void _lv_grid_calc_free(_lv_grid_calc_t * calc)
{
    _lv_mem_buf_release(calc->x);
    _lv_mem_buf_release(calc->y);
    _lv_mem_buf_release(calc->w);
    _lv_mem_buf_release(calc->h);
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
    const lv_grid_t * grid = cont->grid;
    uint32_t i;

    calc->col_num = grid->col_dsc_len;
    calc->x = _lv_mem_buf_get(sizeof(lv_coord_t) * calc->col_num);
    calc->w = _lv_mem_buf_get(sizeof(lv_coord_t) * calc->col_num);

    uint32_t col_fr_cnt = 0;
    lv_coord_t grid_w = 0;
    bool auto_w = cont->w_set == LV_SIZE_AUTO ? true : false;

    for(i = 0; i < grid->col_dsc_len; i++) {
        lv_coord_t x = grid->col_dsc[i];
        if(_GRID_IS_FR(x)) col_fr_cnt += _GRID_GET_FR(x);
        else {
            calc->w[i] = x;
            grid_w += x;
        }
    }

    lv_coord_t cont_w = lv_obj_get_width_fit(cont) - grid->col_gap * (grid->col_dsc_len - 1);
    lv_coord_t free_w = cont_w - grid_w;

    for(i = 0; i < grid->col_dsc_len; i++) {
        lv_coord_t x = grid->col_dsc[i];
        if(_GRID_IS_FR(x)) {
            if(auto_w) calc->w[i] = 0;   /*Fr is considered zero if the cont has auto width*/
            else {
                lv_coord_t f = _GRID_GET_FR(x);
                calc->w[i] = (free_w * f) / col_fr_cnt;
            }
        }
    }
}

static void calc_explicit_rows(lv_obj_t * cont, _lv_grid_calc_t * calc)
{
    const lv_grid_t * grid = cont->grid;
    uint32_t i;

    calc->row_num = grid->row_dsc_len;
    calc->y = _lv_mem_buf_get(sizeof(lv_coord_t) * calc->row_num);
    calc->h = _lv_mem_buf_get(sizeof(lv_coord_t) * calc->row_num);

    uint32_t row_fr_cnt = 0;
    lv_coord_t grid_h = 0;

    bool auto_h = cont->h_set == LV_SIZE_AUTO ? true : false;


    for(i = 0; i < grid->row_dsc_len; i++) {
        lv_coord_t x = grid->row_dsc[i];
        if(_GRID_IS_FR(x)) row_fr_cnt += _GRID_GET_FR(x);
        else {
            calc->h[i] = x;
            grid_h += x;
        }
    }

    lv_coord_t cont_h = lv_obj_get_height_fit(cont) - grid->row_gap * (grid->row_dsc_len - 1);
    lv_coord_t free_h = cont_h - grid_h;

    for(i = 0; i < grid->row_dsc_len; i++) {
        lv_coord_t x = grid->row_dsc[i];
        if(_GRID_IS_FR(x)) {
            if(auto_h) calc->h[i] = 0;   /*Fr is considered zero if the obj has auto height*/
            else {
                lv_coord_t f = _GRID_GET_FR(x);
                calc->h[i] = (free_h * f) / row_fr_cnt;
            }
        }
    }
}


static void calc_implicit_cols(lv_obj_t * cont, _lv_grid_calc_t * calc)
{
    const lv_grid_t * grid = cont->grid;

    uint32_t child_cnt = lv_obj_count_children(cont);

    calc->col_num = ((child_cnt + grid->row_dsc_len - 1) / grid->row_dsc_len); /*+ grid->row_dsc_len - 1 to round up*/
    calc->w  = _lv_mem_buf_get(sizeof(lv_coord_t) * (calc->col_num + 1));   /*+1 to allow some limit checks later*/
    calc->x = _lv_mem_buf_get(sizeof(lv_coord_t) * calc->col_num);

    lv_obj_t * child = lv_obj_get_child_back(cont, NULL);
    uint32_t row_i = 0;
    uint32_t col_i = 0;
    calc->w[0] = 0;
    while(child) {
        if(_GRID_IS_CELL(child->x_set) && _GRID_IS_CELL(child->y_set)) {
            lv_coord_t w;
            if(_GRID_GET_CELL_FLAG(child->x_set) == LV_GRID_STRETCH) _lv_obj_calc_auto_size(child,  &w, NULL);
            else w = lv_obj_get_width(child);
            calc->w[col_i] = LV_MATH_MAX(calc->w[col_i], w);
            row_i++;
            if(row_i == grid->row_dsc_len) {
                row_i = 0;
                col_i++;
                calc->w[col_i] = 0;
            }
        }
        child = lv_obj_get_child_back(cont, child);
    }
}


static void calc_implicit_rows(lv_obj_t * cont, _lv_grid_calc_t * calc)
{

    const lv_grid_t * grid = cont->grid;
    uint32_t child_cnt = lv_obj_count_children(cont);
    calc->row_num  = ((child_cnt + grid->col_dsc_len - 1) / grid->col_dsc_len); /*+ grid->col_dsc_len - 1 to round up*/
    calc->h = _lv_mem_buf_get(sizeof(lv_coord_t) * (calc->row_num + 1));   /*+1 to allow some limit checks later*/
    calc->y = _lv_mem_buf_get(sizeof(lv_coord_t) * calc->row_num);

    lv_obj_t * child = lv_obj_get_child_back(cont, NULL);
    uint32_t col_i = 0;
    uint32_t row_i = 0;
    calc->h[0] = 0;
    while(child) {
        if(_GRID_IS_CELL(child->x_set) && _GRID_IS_CELL(child->y_set)) {
            lv_coord_t h;
            if(_GRID_GET_CELL_FLAG(child->y_set) == LV_GRID_STRETCH) _lv_obj_calc_auto_size(child,  NULL, &h);
            else h = lv_obj_get_height(child);
            calc->h[row_i] = LV_MATH_MAX(calc->h[row_i], h);
            col_i++;
            if(col_i == grid->col_dsc_len) {
                col_i = 0;
                row_i++;
                calc->h[row_i] = 0;
            }
        }
        child = lv_obj_get_child_back(cont, child);
    }
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

    lv_coord_t col_x1 = calc->x[col_pos];
    lv_coord_t col_x2 = calc->x[col_pos + col_span - 1] + calc->w[col_pos + col_span - 1];
    lv_coord_t col_w = col_x2 - col_x1;

    lv_coord_t row_y1 = calc->y[row_pos];
    lv_coord_t row_y2 = calc->y[row_pos + row_span - 1] + calc->h[col_pos + col_span - 1];
    lv_coord_t row_h = row_y2 - row_y1;

    uint8_t x_flag = _GRID_GET_CELL_FLAG(item->x_set);
    uint8_t y_flag = _GRID_GET_CELL_FLAG(item->y_set);

    lv_coord_t x;
    lv_coord_t y;
    lv_coord_t w = lv_obj_get_width(item);
    lv_coord_t h = lv_obj_get_height(item);

    switch(x_flag) {
        case LV_GRID_START:
            x = calc->x[col_pos];
            break;
        case LV_GRID_STRETCH:
            x = calc->x[col_pos];
            w = col_w;
            item->w_set = LV_SIZE_STRETCH;
            break;
        case LV_GRID_CENTER:
            x = calc->x[col_pos] + (col_w - w) / 2;
            break;
        case LV_GRID_END:
            x = calc->x[col_pos + 1] - lv_obj_get_width(item);
            break;
    }

    switch(y_flag) {
        case LV_GRID_START:
            y = calc->y[row_pos];
            break;
        case LV_GRID_STRETCH:
            y = calc->y[row_pos];
            item->h_set = LV_SIZE_STRETCH;
            h = row_h;
            break;
        case LV_GRID_CENTER:
            y = calc->y[row_pos] + (row_h - h) / 2;
            break;
        case LV_GRID_END:
            y = calc->y[row_pos + 1] - lv_obj_get_height(item);
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
static lv_coord_t grid_place(lv_coord_t cont_size,  bool auto_size, uint8_t place, lv_coord_t gap, uint32_t track_num, lv_coord_t * size_array, lv_coord_t * pos_array)
{
    lv_coord_t grid_size = 0;
    uint32_t i;

    if(auto_size) {
        pos_array[0] = 0;
    } else {

        /*With spaced placements gap will be calculated from the remaining space*/
        if(place == LV_GRID_SPACE_AROUND || place == LV_GRID_SPACE_BETWEEN || place == LV_GRID_SPACE_EVENLY) {
            gap = 0;
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

    /*Return the full size of the grid*/
    return pos_array[track_num - 1] + size_array[track_num - 1] - pos_array[0];
}


/**
 * Refresh the grid of all children of an object. (Called recursively)
 * @param grid refresh objects only with this grid.
 * @param obj pointer to an object
 */
static void report_grid_change_core(const lv_grid_t * grid, lv_obj_t * obj)
{
    if(obj->grid == grid || (obj->grid && grid == NULL)) _lv_grid_full_refresh(obj);

    lv_obj_t * child = lv_obj_get_child(obj, NULL);
    while(child) {
        report_grid_change_core(grid, child);
        child = lv_obj_get_child(obj, child);
    }

}
