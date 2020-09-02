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

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void calc_explicit_cols(lv_obj_t * cont, _lv_grid_calc_t * calc);
static void calc_explicit_rows(lv_obj_t * cont, _lv_grid_calc_t * calc);
static void calc_implicit_cols(lv_obj_t * cont, _lv_grid_calc_t * calc);
static void calc_implicit_rows(lv_obj_t * cont, _lv_grid_calc_t * calc);

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

void grid_calc(struct _lv_obj_t * obj, _lv_grid_calc_t * calc)
{
    if(obj->grid == NULL) return;

//    static uint32_t cnt = 0;
//    cnt++;
//    printf("calc: %d (size: %d)\n", cnt, obj->grid->col_dsc_len);

    if(obj->grid->col_dsc && obj->grid->row_dsc) {
        calc_explicit_rows(obj, calc);
        calc_explicit_cols(obj, calc);
    }
    else if(obj->grid->col_dsc && !obj->grid->row_dsc) {
        calc_explicit_cols(obj, calc);
        calc_implicit_rows(obj, calc);
    }
    else if(!obj->grid->col_dsc && obj->grid->row_dsc) {
        calc_implicit_cols(obj, calc);
        calc_explicit_rows(obj, calc);
    }

}

void grid_calc_free(_lv_grid_calc_t * calc)
{
    _lv_mem_buf_release(calc->col_dsc);
    _lv_mem_buf_release(calc->row_dsc);
}


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
