/**
 * @file lv_flex.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_flex.h"
#include "lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t * find_track_end(lv_obj_t * cont, lv_obj_t * item_start, lv_coord_t max_size, lv_coord_t * grow_unit, lv_coord_t * track_cross_size);
static void children_repos(lv_obj_t * cont, lv_obj_t * item_first, lv_obj_t * item_last, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t track_size, lv_coord_t grow_unit);
static void place_content(lv_coord_t place, lv_coord_t max_size, lv_coord_t track_size, lv_coord_t track_cnt, lv_coord_t * start_pos, lv_coord_t * gap);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_obj_set_flex_cont(lv_obj_t * obj, lv_flex_dir_t flex_dir, lv_flex_place_t flex_place)
{
    if(obj->flex_cont.dir == flex_dir && obj->flex_cont.place == flex_place) return;

    obj->flex_cont.dir = flex_dir;
    obj->flex_cont.place = flex_place;

    _lv_flex_refresh(obj);
}

void lv_obj_set_flex_gap(lv_obj_t * obj, lv_coord_t gap)
{

    if(obj->flex_cont.gap == gap) return;

    obj->flex_cont.gap = gap;

    _lv_flex_refresh(obj);
}

void lv_obj_set_flex_item(lv_obj_t * obj, lv_flex_place_t place)
{
    lv_coord_t f  = _LV_COORD_FELX(place);
    lv_obj_set_pos(obj, f, f);
}

void _lv_flex_refresh(lv_obj_t * cont)
{
    if(cont->flex_cont.dir == LV_FLEX_DIR_NONE) return;

    lv_flex_dir_t dir = cont->flex_cont.dir & 0x3;
    bool row = dir == LV_FLEX_DIR_ROW ? true : false;
    /*Count the grow units and free space*/
    lv_coord_t max_main_size = (row ? lv_obj_get_width_fit(cont) : lv_obj_get_height_fit(cont));
    lv_coord_t abs_x = cont->coords.x1 + lv_obj_get_style_pad_left(cont, LV_OBJ_PART_MAIN) - lv_obj_get_scroll_left(cont);
    lv_coord_t abs_y = cont->coords.y1 + lv_obj_get_style_pad_top(cont, LV_OBJ_PART_MAIN) - lv_obj_get_scroll_top(cont);

    lv_coord_t * cross_pos = (row ? &abs_y : &abs_x);

    lv_coord_t place = cont->flex_cont.place;
    if((row && cont->h_set == LV_SIZE_AUTO) ||
       (!row && cont->w_set == LV_SIZE_AUTO))
    {
        place = LV_FLEX_START;
    }

    lv_coord_t all_track_size = 0;
    lv_coord_t gap = 0;
    uint32_t row_cnt = 0;
    lv_coord_t grow_unit;
    lv_coord_t track_size;
    lv_obj_t * track_first_item;
    lv_obj_t * next_track_first_item;
    bool rev = cont->flex_cont.dir & LV_FLEX_REVERSE;

    if(place != LV_FLEX_START) {
        track_first_item =  rev ? _lv_ll_get_head(&cont->child_ll) : _lv_ll_get_tail(&cont->child_ll);

        while(track_first_item) {
            /*Search the first item of the next row */
            next_track_first_item = find_track_end(cont, track_first_item, max_main_size, &grow_unit, &track_size);
            all_track_size += track_size;
            row_cnt++;
            track_first_item = next_track_first_item;
        }

        lv_coord_t max_cross_size = (row ? lv_obj_get_height_fit(cont) : lv_obj_get_width_fit(cont));
        place_content(place, max_cross_size, all_track_size,row_cnt, cross_pos, &gap);
    }

    track_first_item =  rev ? _lv_ll_get_head(&cont->child_ll) : _lv_ll_get_tail(&cont->child_ll);
    while(track_first_item) {
        /*Search the first item of the next row */
        next_track_first_item = find_track_end(cont, track_first_item, max_main_size, &grow_unit, &track_size);

        children_repos(cont, track_first_item, next_track_first_item, abs_x, abs_y, track_size, grow_unit);

        track_first_item = next_track_first_item;
        *cross_pos += track_size + gap;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t * find_track_end(lv_obj_t * cont, lv_obj_t * item_start, lv_coord_t max_size, lv_coord_t * grow_unit, lv_coord_t * track_cross_size)
{
    lv_flex_dir_t dir = cont->flex_cont.dir;
    bool row = (dir & 0x3) == LV_FLEX_DIR_ROW ? true : false;
    lv_coord_t(*get_main_size)(const lv_obj_t *) = (row ? lv_obj_get_width_margin : lv_obj_get_height_margin);
    lv_coord_t(*get_cross_size)(const lv_obj_t *) = (!row ? lv_obj_get_width_margin : lv_obj_get_height_margin);
    void * (*ll_iter)(const lv_ll_t * , const void *) = dir & LV_FLEX_REVERSE ? _lv_ll_get_next : _lv_ll_get_prev;
    bool wrap = dir & LV_FLEX_WRAP ? true : false;

    lv_coord_t grow_sum = 0;
    lv_coord_t used_size = 0;
    lv_coord_t gap = cont->flex_cont.gap;
    uint32_t grow_item_cnt = 0;
    *track_cross_size = 0;
    *grow_unit = 0;

    lv_obj_t * item = item_start;
    while(item) {
        /*Ignore non-flex items*/
        lv_coord_t main_set = (row ? item->x_set : item->y_set);
        if(LV_COORD_IS_FLEX(main_set) == false) {
            item = ll_iter(&cont->child_ll, item);
            continue;
        }

        lv_coord_t main_size = (row ? item->w_set : item->h_set);
        if(_LV_FLEX_GET_GROW(main_size)) {
            grow_sum += _LV_FLEX_GET_GROW(main_size);
            grow_item_cnt++;
        } else {
            lv_coord_t item_size = get_main_size(item) + gap;
            if(wrap && used_size + item_size > max_size) break;
            used_size += item_size;
        }
        *track_cross_size = LV_MATH_MAX(get_cross_size(item), *track_cross_size);

        item = ll_iter(&cont->child_ll, item);
    }

    if(used_size > 0) used_size -= gap; /*There is no gap after the last item*/

    if(grow_item_cnt && grow_sum) {
        lv_coord_t s = max_size - used_size;
        s -= grow_item_cnt * gap;
        *grow_unit =  s / grow_sum;
    } else {
        *grow_unit = 0;
    }

    /*Have at least one item in a row*/
    if(item && item == item_start) {
        item = ll_iter(&cont->child_ll, item);
        if(item) *track_cross_size = get_cross_size(item);
    }

    return item;
}


static void children_repos(lv_obj_t * cont, lv_obj_t * item_first, lv_obj_t * item_last, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t track_size, lv_coord_t grow_unit)
{
    lv_flex_dir_t dir = cont->flex_cont.dir;
    bool row = (dir & 0x3) == LV_FLEX_DIR_ROW ? true : false;
    lv_coord_t(*obj_get_main_size)(const lv_obj_t *) = (row ? lv_obj_get_width_margin : lv_obj_get_height_margin);
    lv_coord_t(*obj_get_cross_size)(const lv_obj_t *) = (!row ? lv_obj_get_width_margin : lv_obj_get_height_margin);
    void (*area_set_main_size)(lv_area_t *, lv_coord_t) = (row ? lv_area_set_width : lv_area_set_height);
    lv_coord_t (*area_get_main_size)(const lv_area_t *) = (!row ? lv_area_get_width : lv_area_get_height);
    lv_style_int_t (*get_margin_start)(const lv_obj_t *, uint8_t part) = (row ? lv_obj_get_style_margin_left : lv_obj_get_style_margin_top);
    lv_style_int_t (*get_margin_end)(const lv_obj_t *, uint8_t part) = (row ? lv_obj_get_style_margin_right : lv_obj_get_style_margin_bottom);
    void * (*ll_iter)(const lv_ll_t * , const void *) = dir & LV_FLEX_REVERSE ? _lv_ll_get_next : _lv_ll_get_prev;

    lv_coord_t gap = cont->flex_cont.gap;
    lv_coord_t main_pos = 0;
    /*Reposition the children*/
    lv_obj_t * item = item_first;   /*Just to use a shorter name*/
    while(item != item_last) {

        /*Ignore non-flex items*/
        lv_coord_t main_set = (row ? item->x_set : item->y_set);
        if(LV_COORD_IS_FLEX(main_set) == false) {
            item = ll_iter(&cont->child_ll, item);
            continue;
        }

        lv_coord_t main_size = (row ? item->w_set : item->h_set);
        if(_LV_FLEX_GET_GROW(main_size)) {
            lv_coord_t s =  _LV_FLEX_GET_GROW(main_size) * grow_unit;
            s -= get_margin_start(item, LV_OBJ_PART_MAIN) + get_margin_end(item, LV_OBJ_PART_MAIN);
            if(s != area_get_main_size(&item->coords)) {
                lv_area_t old_coords;
                lv_area_copy(&old_coords, &item->coords);
                lv_obj_invalidate(item);
                area_set_main_size(&item->coords, s);
                lv_obj_invalidate(item);
                item->signal_cb(item, LV_SIGNAL_COORD_CHG, &old_coords);
            }
        }

        lv_coord_t cross_pos = 0;
        lv_coord_t cross_set = (row ? item->y_set : item->x_set);
        switch(LV_COORD_GET_FLEX(cross_set)) {
        case LV_FLEX_CENTER:
            cross_pos = (track_size - obj_get_cross_size(item)) / 2;
            break;
        case LV_FLEX_END:
            cross_pos = track_size - obj_get_cross_size(item);
            break;
        }

        lv_coord_t diff_x;
        lv_coord_t diff_y;
        if(row) {
            cross_pos += lv_obj_get_style_margin_top(item, LV_OBJ_PART_MAIN);
            main_pos += lv_obj_get_style_margin_left(item, LV_OBJ_PART_MAIN);
            diff_x = abs_x + main_pos - item->coords.x1;
            diff_y = abs_y + cross_pos - item->coords.y1;
        } else {
            main_pos += lv_obj_get_style_margin_top(item, LV_OBJ_PART_MAIN);
            cross_pos += lv_obj_get_style_margin_left(item, LV_OBJ_PART_MAIN);
            diff_x = abs_x + cross_pos - item->coords.x1;
            diff_y = abs_y + main_pos - item->coords.y1;
        }

        if(diff_x || diff_y) {
            item->coords.x1 += diff_x;
            item->coords.x2 += diff_x;
            item->coords.y1 += diff_y;
            item->coords.y2 += diff_y;
            _lv_obj_move_children_by(item, diff_x, diff_y);
        }
        main_pos += obj_get_main_size(item) + gap;
        item = ll_iter(&cont->child_ll, item);
    }
}


static void place_content(lv_coord_t place, lv_coord_t max_size, lv_coord_t track_size, lv_coord_t track_cnt, lv_coord_t * start_pos, lv_coord_t * gap)
{
    if(track_cnt <= 1) {
        switch(place) {
            case LV_FLEX_SPACE_BETWEEN:
            case LV_FLEX_SPACE_AROUND:
            case LV_FLEX_SPACE_EVENLY:
                place = LV_FLEX_CENTER;
                break;
        }
    }

    switch(place) {
    case LV_FLEX_CENTER:
        *start_pos += (max_size - track_size) / 2;
        break;
    case LV_FLEX_END:
        *start_pos += max_size - track_size;
        break;
    case LV_FLEX_SPACE_BETWEEN:
       *gap = (lv_coord_t)(max_size - track_size) / (lv_coord_t)(track_cnt - 1);
       break;
   case LV_FLEX_SPACE_AROUND:
       *gap += (lv_coord_t)(max_size - track_size) / (lv_coord_t)(track_cnt);
       *start_pos += *gap / 2;
       break;
   case LV_FLEX_SPACE_EVENLY:
       *gap = (lv_coord_t)(max_size - track_size) / (lv_coord_t)(track_cnt + 1);
       *start_pos += *gap;
       break;
   default:
       *gap = 0;
    }
}
