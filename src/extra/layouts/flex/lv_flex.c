/**
 * @file lv_flex.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_flex.h"

#if LV_USE_FLEX

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_coord_t grow_unit;
    lv_coord_t track_cross_size;
    lv_coord_t track_main_size;
    uint32_t item_cnt;
}track_t;


/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flex_update(lv_obj_t * cont, lv_obj_t * item);
static int32_t find_track_end(lv_obj_t * cont, int32_t item_start_id, lv_coord_t item_gap, lv_coord_t max_main_size, track_t * t);
static void children_repos(lv_obj_t * cont, int32_t item_first_id, int32_t item_last_id, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t max_main_size, lv_coord_t item_gap, track_t * t);
static void place_content(lv_flex_place_t place, lv_coord_t max_size, lv_coord_t content_size, lv_coord_t item_cnt, lv_coord_t * start_pos, lv_coord_t * gap);
static lv_obj_t * get_next_item(lv_obj_t * cont, bool rev, int32_t * item_id);

/**********************
 *  GLOBAL VARIABLES
 **********************/

const lv_flex_t lv_flex_inline = {
        .base.update_cb = flex_update,
        .item_main_place = LV_FLEX_PLACE_START,
        .item_cross_place = LV_FLEX_PLACE_CENTER,
        .track_cross_place = LV_FLEX_PLACE_START,
        .dir = LV_FLEX_FLOW_ROW,
        .wrap = 1
};


const lv_flex_t lv_flex_center_row = {
        .base.update_cb = flex_update,
        .item_main_place = LV_FLEX_PLACE_CENTER,
        .item_cross_place = LV_FLEX_PLACE_CENTER,
        .track_cross_place = LV_FLEX_PLACE_CENTER,
        .dir = LV_FLEX_FLOW_ROW,
        .wrap = 1
};

const lv_flex_t lv_flex_center_column = {
        .base.update_cb = flex_update,
        .item_main_place = LV_FLEX_PLACE_CENTER,
        .item_cross_place = LV_FLEX_PLACE_CENTER,
        .track_cross_place = LV_FLEX_PLACE_CENTER,
        .dir = LV_FLEX_FLOW_COLUMN,
        .wrap = 1
};

const lv_flex_t lv_flex_stacked = {
        .base.update_cb = flex_update,
        .item_main_place = LV_FLEX_PLACE_START,
        .item_cross_place = LV_FLEX_PLACE_CENTER,
        .track_cross_place = LV_FLEX_PLACE_START,
        .dir = LV_FLEX_FLOW_COLUMN
};

const lv_flex_t lv_flex_queue = {
        .base.update_cb = flex_update,
        .item_main_place = LV_FLEX_PLACE_START,
        .item_cross_place = LV_FLEX_PLACE_START,
        .track_cross_place = LV_FLEX_PLACE_START,
        .dir = LV_FLEX_FLOW_ROW
};

const lv_flex_t lv_flex_even = {
        .base.update_cb = flex_update,
        .item_main_place = LV_FLEX_PLACE_SPACE_EVENLY,
        .item_cross_place = LV_FLEX_PLACE_CENTER,
        .track_cross_place = LV_FLEX_PLACE_CENTER,
        .dir = LV_FLEX_FLOW_ROW,
        .wrap = 1
};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*=====================
 * Setter functions
 *====================*/

void lv_flex_init(lv_flex_t * flex)
{
    lv_memset_00(flex, sizeof(lv_flex_t));
    flex->base.update_cb = flex_update;
    flex->dir = LV_FLEX_FLOW_ROW;
    flex->item_main_place = LV_FLEX_PLACE_START;
    flex->track_cross_place = LV_FLEX_PLACE_START;
    flex->item_cross_place = LV_FLEX_PLACE_START;
}

void lv_flex_set_flow(lv_flex_t * flex, lv_flex_flow_t flow)
{
    flex->dir = flow & 0x3;
    flex->wrap = flow & _LV_FLEX_WRAP ? 1 : 0;
    flex->rev = flow & _LV_FLEX_REVERSE ? 1 : 0;
}

void lv_flex_set_place(lv_flex_t * flex, lv_flex_place_t item_main_place, lv_flex_place_t item_cross_place, lv_flex_place_t track_cross_place)
{
    flex->item_main_place = item_main_place;
    flex->track_cross_place = track_cross_place;
    flex->item_cross_place = item_cross_place;
}

void lv_obj_set_flex_grow(struct _lv_obj_t * obj, uint8_t grow)
{
    if(!lv_obj_is_layout_positioned(obj)) return;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent->spec_attr->layout_dsc->update_cb  != flex_update) return;
    const lv_flex_t * f = (const lv_flex_t *) parent->spec_attr->layout_dsc;

    if(f->dir == LV_FLEX_FLOW_ROW) lv_obj_set_width(obj, (LV_COORD_SET_LAYOUT(grow)));
    else lv_obj_set_height(obj, (LV_COORD_SET_LAYOUT(grow)));

    lv_obj_update_layout(parent, obj);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flex_update(lv_obj_t * cont, lv_obj_t * item)
{
    LV_UNUSED(item);

    if(cont->spec_attr == NULL) return;
    const lv_flex_t * f = (const lv_flex_t *)cont->spec_attr->layout_dsc;

    LV_LOG_INFO("Flex update on 0x%p triggered by 0x%p", cont, item);

    bool rtl = lv_obj_get_base_dir(cont) == LV_BIDI_DIR_RTL ? true : false;
    bool row = f->dir == LV_FLEX_FLOW_ROW ? true : false;
    lv_coord_t track_gap = !row ? lv_obj_get_style_pad_column(cont, LV_PART_MAIN) : lv_obj_get_style_pad_row(cont, LV_PART_MAIN);
    lv_coord_t item_gap = row ? lv_obj_get_style_pad_column(cont, LV_PART_MAIN) : lv_obj_get_style_pad_row(cont, LV_PART_MAIN);
    lv_coord_t max_main_size = (row ? lv_obj_get_width_fit(cont) : lv_obj_get_height_fit(cont));
    lv_coord_t abs_y = cont->coords.y1 + lv_obj_get_style_pad_top(cont, LV_PART_MAIN) - lv_obj_get_scroll_y(cont);
    lv_coord_t abs_x = cont->coords.x1 + lv_obj_get_style_pad_left(cont, LV_PART_MAIN) - lv_obj_get_scroll_x(cont);

    lv_flex_place_t track_cross_place = f->track_cross_place;
    lv_coord_t * cross_pos = (row ? &abs_y : &abs_x);

    if((row && cont->h_set == LV_SIZE_CONTENT) ||
       (!row && cont->w_set == LV_SIZE_CONTENT))
    {
        track_cross_place = LV_FLEX_PLACE_START;
    }

    if(rtl && !row) {
        if(track_cross_place == LV_FLEX_PLACE_START) track_cross_place = LV_FLEX_PLACE_END;
        else if(track_cross_place == LV_FLEX_PLACE_END) track_cross_place = LV_FLEX_PLACE_START;
    }

    lv_coord_t total_track_cross_size = 0;
    lv_coord_t gap = 0;
    uint32_t track_cnt = 0;
    int32_t track_first_item;
    int32_t next_track_first_item;

    if(track_cross_place != LV_FLEX_PLACE_START) {
        track_first_item = f->rev ? cont->spec_attr->child_cnt - 1 : 0;
        track_t t;
        while(track_first_item < (int32_t)cont->spec_attr->child_cnt && track_first_item >= 0) {
            /*Search the first item of the next row */
            next_track_first_item = find_track_end(cont, track_first_item, max_main_size, item_gap, &t);
            total_track_cross_size += t.track_cross_size + track_gap;
            track_cnt++;
            track_first_item = next_track_first_item;
        }

        if(track_cnt) total_track_cross_size -= track_gap;   /*No gap after the last track*/

        /* Place the tracks to get the start position
         * If the the height of the tracks is larger than the available space
         * always use the gap = 0 and start position = 0 to avoid unintuitive scrolling*/
        lv_coord_t max_cross_size = (row ? lv_obj_get_height_fit(cont) : lv_obj_get_width_fit(cont));
        if(total_track_cross_size < max_cross_size){
            place_content(track_cross_place, max_cross_size, total_track_cross_size, track_cnt, cross_pos, &gap);
        }
        else if(rtl && !row) {
            /* For RTL columns set the cross_pos to the left side manually.
             * It's not at x = 0 because with RTL the most right column is at cont->x2*/
            *cross_pos = max_cross_size - total_track_cross_size + lv_obj_get_style_pad_left(cont, LV_PART_MAIN);
        }
    }

    track_first_item =  f->rev ? cont->spec_attr->child_cnt - 1 : 0;

    if(rtl && !row) {
         *cross_pos += total_track_cross_size;
    }

    while(track_first_item < (int32_t)cont->spec_attr->child_cnt && track_first_item >= 0) {
        track_t t;
        /*Search the first item of the next row */
        next_track_first_item = find_track_end(cont, track_first_item, max_main_size, item_gap, &t);

        if(rtl && !row) {
            *cross_pos -= t.track_cross_size;
        }
        children_repos(cont, track_first_item, next_track_first_item, abs_x, abs_y, max_main_size, item_gap, &t);
        track_first_item = next_track_first_item;

        if(rtl && !row) {
            *cross_pos -= gap + track_gap;
        } else {
            *cross_pos += t.track_cross_size + gap + track_gap;
        }
    }
    LV_ASSERT_MEM_INTEGRITY();

    if(cont->w_set == LV_SIZE_CONTENT || cont->h_set == LV_SIZE_CONTENT) {
        lv_obj_set_size(cont, cont->w_set, cont->h_set);
    }

    LV_LOG_TRACE("finished");
}

/**
 * Find the last item of a track
 */
static int32_t find_track_end(lv_obj_t * cont, int32_t item_start_id, lv_coord_t max_main_size, lv_coord_t item_gap, track_t * t)
{
    const lv_flex_t * f = (const lv_flex_t *)cont->spec_attr->layout_dsc;

    bool row = f->dir == LV_FLEX_FLOW_ROW ? true : false;
    bool wrap = f->wrap;
    /*Can't wrap if the size if auto (i.e. the size depends on the children)*/
    if(wrap && ((row && cont->w_set == LV_SIZE_CONTENT) || (!row && cont->h_set == LV_SIZE_CONTENT))) {
        wrap = false;
    }
    lv_coord_t(*get_main_size)(const lv_obj_t *) = (row ? lv_obj_get_width : lv_obj_get_height);
    lv_coord_t(*get_cross_size)(const lv_obj_t *) = (!row ? lv_obj_get_width : lv_obj_get_height);

    lv_coord_t grow_sum = 0;
    t->track_main_size = 0;
    uint32_t grow_item_cnt = 0;
    t->track_cross_size = 0;
    t->grow_unit = 0;
    t->item_cnt = 0;

    int32_t item_id = item_start_id;

    lv_obj_t * item = lv_obj_get_child(cont, item_id);
    while(item) {
        if(!lv_obj_has_flag_any(item, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) {
            lv_coord_t main_size = (row ? item->w_set : item->h_set);
            if(_LV_FLEX_GET_GROW(main_size)) {
                grow_sum += _LV_FLEX_GET_GROW(main_size);
                grow_item_cnt++;
                t->track_main_size += item_gap;
            } else {
                lv_coord_t item_size = get_main_size(item);
                if(wrap && t->track_main_size + item_size > max_main_size) break;
                t->track_main_size += item_size + item_gap;
            }


            t->track_cross_size = LV_MAX(get_cross_size(item), t->track_cross_size);
            t->item_cnt++;
        }

        item_id += f->rev ? -1 : +1;
        if(item_id < 0) break;
        item = lv_obj_get_child(cont, item_id);
    }

    if(t->track_main_size > 0) t->track_main_size -= item_gap; /*There is no gap after the last item*/

    if(grow_item_cnt && grow_sum) {
        lv_coord_t s = max_main_size - t->track_main_size;	/*The remaining size for grow items*/
        t->grow_unit =  s / grow_sum;
        t->track_main_size = max_main_size;  /*If there is at least one "grow item" the track takes the full space*/
    } else {
        t->grow_unit = 0;
    }

    /*Have at least one item in a row*/
    if(item && item_id == item_start_id) {
        item = cont->spec_attr->children[item_id];
        get_next_item(cont, f->rev, &item_id);
        if(item) {
            t->track_cross_size = get_cross_size(item);
            t->track_main_size = get_main_size(item);
            t->item_cnt = 1;
        }
    }

    return item_id;
}

/**
 * Position the children in the same track
 */
static void children_repos(lv_obj_t * cont, int32_t item_first_id, int32_t item_last_id, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t max_main_size, lv_coord_t item_gap, track_t * t)
{

    const lv_flex_t * f = (const lv_flex_t *)cont->spec_attr->layout_dsc;
    bool row = f->dir == LV_FLEX_FLOW_ROW ? true : false;

    void (*area_set_main_size)(lv_area_t *, lv_coord_t) = (row ? lv_area_set_width : lv_area_set_height);
    lv_coord_t (*area_get_main_size)(const lv_area_t *) = (row ? lv_area_get_width : lv_area_get_height);
    lv_coord_t (*area_get_cross_size)(const lv_area_t *) = (!row ? lv_area_get_width : lv_area_get_height);

    bool rtl = lv_obj_get_base_dir(cont) == LV_BIDI_DIR_RTL ? true : false;


    lv_coord_t main_pos = 0;

    lv_coord_t place_gap = 0;
    place_content(f->item_main_place, max_main_size, t->track_main_size, t->item_cnt, &main_pos, &place_gap);
    if(row && rtl) main_pos += t->track_main_size;

    lv_obj_t * item = lv_obj_get_child(cont, item_first_id);
    /*Reposition the children*/
    while(item && item_first_id != item_last_id) {
        if(lv_obj_has_flag_any(item, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) {
            item = get_next_item(cont, f->rev, &item_first_id);
            continue;
        }
        lv_coord_t main_size = (row ? item->w_set : item->h_set);
        if(_LV_FLEX_GET_GROW(main_size)) {
            lv_area_t old_coords;
            lv_area_copy(&old_coords, &item->coords);

            lv_coord_t s = _LV_FLEX_GET_GROW(main_size) * t->grow_unit;
            area_set_main_size(&item->coords, s);

            if(lv_area_get_height(&old_coords) != area_get_main_size(&item->coords)) {
                lv_obj_invalidate(item);
                lv_signal_send(item, LV_SIGNAL_COORD_CHG, &old_coords);
                lv_obj_invalidate(item);
            }
        }

        lv_coord_t cross_pos = 0;
        switch(f->item_cross_place) {
        case LV_FLEX_PLACE_CENTER:
            /* Round up the cross size to avoid rounding error when dividing by 2
             * The issue comes up e,g, with column direction with center cross direction if an element's width changes*/
            cross_pos = (((t->track_cross_size + 1) & (~1)) - area_get_cross_size(&item->coords)) / 2;
            break;
        case LV_FLEX_PLACE_END:
            cross_pos = t->track_cross_size - area_get_cross_size(&item->coords);
            break;
        default:
            break;
        }

        if(row && rtl) main_pos -= area_get_main_size(&item->coords);


        lv_coord_t diff_x = abs_x - item->coords.x1;
        lv_coord_t diff_y = abs_y - item->coords.y1;
        diff_x += row ? main_pos : cross_pos;
        diff_y += row ? cross_pos : main_pos;

        if(diff_x || diff_y) {
            lv_obj_invalidate(item);
            item->coords.x1 += diff_x;
            item->coords.x2 += diff_x;
            item->coords.y1 += diff_y;
            item->coords.y2 += diff_y;
            lv_obj_invalidate(item);
            lv_obj_move_children_by(item, diff_x, diff_y, true);
        }

        if(!(row && rtl)) main_pos += area_get_main_size(&item->coords) + item_gap + place_gap;
        else main_pos -= item_gap + place_gap;

        item = get_next_item(cont, f->rev, &item_first_id);
    }
}

/**
 * Tell a start coordinate and gap for a placement type.
 */
static void place_content(lv_flex_place_t place, lv_coord_t max_size, lv_coord_t content_size, lv_coord_t item_cnt, lv_coord_t * start_pos, lv_coord_t * gap)
{
    if(item_cnt <= 1) {
        switch(place) {
            case LV_FLEX_PLACE_SPACE_BETWEEN:
            case LV_FLEX_PLACE_SPACE_AROUND:
            case LV_FLEX_PLACE_SPACE_EVENLY:
                place = LV_FLEX_PLACE_CENTER;
                break;
            default:
                break;
        }
    }

    switch(place) {
    case LV_FLEX_PLACE_CENTER:
        *gap = 0;
        *start_pos += (max_size - content_size) / 2;
        break;
    case LV_FLEX_PLACE_END:
        *gap = 0;
        *start_pos += max_size - content_size;
        break;
    case LV_FLEX_PLACE_SPACE_BETWEEN:
       *gap = (lv_coord_t)(max_size - content_size) / (lv_coord_t)(item_cnt - 1);
       break;
   case LV_FLEX_PLACE_SPACE_AROUND:
       *gap += (lv_coord_t)(max_size - content_size) / (lv_coord_t)(item_cnt);
       *start_pos += *gap / 2;
       break;
   case LV_FLEX_PLACE_SPACE_EVENLY:
       *gap = (lv_coord_t)(max_size - content_size) / (lv_coord_t)(item_cnt + 1);
       *start_pos += *gap;
       break;
   default:
       *gap = 0;
    }
}

static lv_obj_t * get_next_item(lv_obj_t * cont, bool rev, int32_t * item_id)
{
    if(rev) {
        (*item_id)--;
        if(*item_id >= 0) return cont->spec_attr->children[*item_id];
        else return NULL;
    } else {
        (*item_id)++;
        if((*item_id) < (int32_t)cont->spec_attr->child_cnt) return cont->spec_attr->children[*item_id];
        else return NULL;
    }
}

#endif /*LV_USE_FLEX*/
