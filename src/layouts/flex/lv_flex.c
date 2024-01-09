/**
 * @file lv_flex.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_flex.h"
#include "../lv_layout.h"
#include "../../core/lv_obj.h"

#if LV_USE_FLEX

#include "../../core/lv_global.h"
/*********************
 *      DEFINES
 *********************/
#define layout_list_def LV_GLOBAL_DEFAULT()->layout_list

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_flex_align_t main_place;
    lv_flex_align_t cross_place;
    lv_flex_align_t track_place;
    uint8_t row : 1;
    uint8_t wrap : 1;
    uint8_t rev : 1;
} flex_t;

typedef struct {
    lv_obj_t * item;
    int32_t min_size;
    int32_t max_size;
    int32_t final_size;
    uint32_t grow_value;
    uint32_t clamped : 1;
} grow_dsc_t;

typedef struct {
    int32_t track_cross_size;
    int32_t track_main_size;         /*For all items*/
    int32_t track_fix_main_size;     /*For non grow items*/
    uint32_t item_cnt;
    grow_dsc_t * grow_dsc;
    uint32_t grow_item_cnt;
    uint32_t grow_dsc_calc : 1;
} track_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flex_update(lv_obj_t * cont, void * user_data);
static int32_t find_track_end(lv_obj_t * cont, flex_t * f, int32_t item_start_id, int32_t max_main_size,
                              int32_t item_gap, track_t * t);
static void children_repos(lv_obj_t * cont, flex_t * f, int32_t item_first_id, int32_t item_last_id, int32_t abs_x,
                           int32_t abs_y, int32_t max_main_size, int32_t item_gap, track_t * t);
static void place_content(lv_flex_align_t place, int32_t max_size, int32_t content_size, int32_t item_cnt,
                          int32_t * start_pos, int32_t * gap);
static lv_obj_t * get_next_item(lv_obj_t * cont, bool rev, int32_t * item_id);
static int32_t lv_obj_get_width_with_margin(const lv_obj_t * obj);
static int32_t lv_obj_get_height_with_margin(const lv_obj_t * obj);

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#if LV_USE_LOG && LV_LOG_TRACE_LAYOUT
    #define LV_TRACE_LAYOUT(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define LV_TRACE_LAYOUT(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*=====================
 * Setter functions
 *====================*/

void lv_flex_init(void)
{
    layout_list_def[LV_LAYOUT_FLEX].cb = flex_update;
    layout_list_def[LV_LAYOUT_FLEX].user_data = NULL;

}

void lv_obj_set_flex_flow(lv_obj_t * obj, lv_flex_flow_t flow)
{
    lv_obj_set_style_flex_flow(obj, flow, 0);
    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, 0);
}

void lv_obj_set_flex_align(lv_obj_t * obj, lv_flex_align_t main_place, lv_flex_align_t cross_place,
                           lv_flex_align_t track_place)
{
    lv_obj_set_style_flex_main_place(obj, main_place, 0);
    lv_obj_set_style_flex_cross_place(obj, cross_place, 0);
    lv_obj_set_style_flex_track_place(obj, track_place, 0);
    lv_obj_set_style_layout(obj, LV_LAYOUT_FLEX, 0);
}

void lv_obj_set_flex_grow(lv_obj_t * obj, uint8_t grow)
{
    lv_obj_set_style_flex_grow(obj, grow, 0);
    lv_obj_mark_layout_as_dirty(lv_obj_get_parent(obj));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flex_update(lv_obj_t * cont, void * user_data)
{
    LV_LOG_INFO("update %p container", (void *)cont);
    LV_UNUSED(user_data);

    flex_t f;
    lv_flex_flow_t flow = lv_obj_get_style_flex_flow(cont, LV_PART_MAIN);
    f.row = flow & _LV_FLEX_COLUMN ? 0 : 1;
    f.wrap = flow & _LV_FLEX_WRAP ? 1 : 0;
    f.rev = flow & _LV_FLEX_REVERSE ? 1 : 0;
    f.main_place = lv_obj_get_style_flex_main_place(cont, LV_PART_MAIN);
    f.cross_place = lv_obj_get_style_flex_cross_place(cont, LV_PART_MAIN);
    f.track_place = lv_obj_get_style_flex_track_place(cont, LV_PART_MAIN);

    bool rtl = lv_obj_get_style_base_dir(cont, LV_PART_MAIN) == LV_BASE_DIR_RTL;
    int32_t track_gap = !f.row ? lv_obj_get_style_pad_column(cont, LV_PART_MAIN) : lv_obj_get_style_pad_row(cont,
                                                                                                            LV_PART_MAIN);
    int32_t item_gap = f.row ? lv_obj_get_style_pad_column(cont, LV_PART_MAIN) : lv_obj_get_style_pad_row(cont,
                                                                                                          LV_PART_MAIN);
    int32_t max_main_size = (f.row ? lv_obj_get_content_width(cont) : lv_obj_get_content_height(cont));
    int32_t abs_y = cont->coords.y1 + lv_obj_get_style_space_top(cont,
                                                                 LV_PART_MAIN) - lv_obj_get_scroll_y(cont);
    int32_t abs_x = cont->coords.x1 + lv_obj_get_style_space_left(cont,
                                                                  LV_PART_MAIN) - lv_obj_get_scroll_x(cont);

    lv_flex_align_t track_cross_place = f.track_place;
    int32_t * cross_pos = (f.row ? &abs_y : &abs_x);

    int32_t w_set = lv_obj_get_style_width(cont, LV_PART_MAIN);
    int32_t h_set = lv_obj_get_style_height(cont, LV_PART_MAIN);

    /*Content sized objects should squeeze the gap between the children, therefore any alignment will look like `START`*/
    if((f.row && h_set == LV_SIZE_CONTENT && cont->h_layout == 0) ||
       (!f.row && w_set == LV_SIZE_CONTENT && cont->w_layout == 0)) {
        track_cross_place = LV_FLEX_ALIGN_START;
    }

    if(rtl && !f.row) {
        if(track_cross_place == LV_FLEX_ALIGN_START) track_cross_place = LV_FLEX_ALIGN_END;
        else if(track_cross_place == LV_FLEX_ALIGN_END) track_cross_place = LV_FLEX_ALIGN_START;
    }

    int32_t total_track_cross_size = 0;
    int32_t gap = 0;
    uint32_t track_cnt = 0;
    int32_t track_first_item;
    int32_t next_track_first_item;

    if(track_cross_place != LV_FLEX_ALIGN_START) {
        track_first_item = f.rev ? cont->spec_attr->child_cnt - 1 : 0;
        track_t t;
        while(track_first_item < (int32_t)cont->spec_attr->child_cnt && track_first_item >= 0) {
            /*Search the first item of the next row*/
            t.grow_dsc_calc = 0;
            next_track_first_item = find_track_end(cont, &f, track_first_item, max_main_size, item_gap, &t);
            total_track_cross_size += t.track_cross_size + track_gap;
            track_cnt++;
            track_first_item = next_track_first_item;
        }

        if(track_cnt) total_track_cross_size -= track_gap;   /*No gap after the last track*/

        /*Place the tracks to get the start position*/
        int32_t max_cross_size = (f.row ? lv_obj_get_content_height(cont) : lv_obj_get_content_width(cont));
        place_content(track_cross_place, max_cross_size, total_track_cross_size, track_cnt, cross_pos, &gap);
    }

    track_first_item = f.rev ? cont->spec_attr->child_cnt - 1 : 0;

    if(rtl && !f.row) {
        *cross_pos += total_track_cross_size;
    }

    while(track_first_item < (int32_t)cont->spec_attr->child_cnt && track_first_item >= 0) {
        track_t t;
        t.grow_dsc_calc = 1;
        /*Search the first item of the next row*/
        next_track_first_item = find_track_end(cont, &f, track_first_item, max_main_size, item_gap, &t);

        if(rtl && !f.row) {
            *cross_pos -= t.track_cross_size;
        }
        children_repos(cont, &f, track_first_item, next_track_first_item, abs_x, abs_y, max_main_size, item_gap, &t);
        track_first_item = next_track_first_item;
        lv_free(t.grow_dsc);
        t.grow_dsc = NULL;
        if(rtl && !f.row) {
            *cross_pos -= gap + track_gap;
        }
        else {
            *cross_pos += t.track_cross_size + gap + track_gap;
        }
    }
    LV_ASSERT_MEM_INTEGRITY();

    if(w_set == LV_SIZE_CONTENT || h_set == LV_SIZE_CONTENT) {
        lv_obj_refr_size(cont);
    }

    lv_obj_send_event(cont, LV_EVENT_LAYOUT_CHANGED, NULL);

    LV_TRACE_LAYOUT("finished");
}

/**
 * Find the last item of a track
 */
static int32_t find_track_end(lv_obj_t * cont, flex_t * f, int32_t item_start_id, int32_t max_main_size,
                              int32_t item_gap, track_t * t)
{
    int32_t w_set = lv_obj_get_style_width(cont, LV_PART_MAIN);
    int32_t h_set = lv_obj_get_style_height(cont, LV_PART_MAIN);

    /*Can't wrap if the size is auto (i.e. the size depends on the children)*/
    if(f->wrap && ((f->row && w_set == LV_SIZE_CONTENT) || (!f->row && h_set == LV_SIZE_CONTENT))) {
        f->wrap = false;
    }
    int32_t(*get_main_size)(const lv_obj_t *) = (f->row ? lv_obj_get_width_with_margin : lv_obj_get_height_with_margin);
    int32_t(*get_cross_size)(const lv_obj_t *) = (!f->row ? lv_obj_get_width_with_margin :
                                                  lv_obj_get_height_with_margin);

    t->track_main_size = 0;
    t->track_fix_main_size = 0;
    t->grow_item_cnt = 0;
    t->track_cross_size = 0;
    t->item_cnt = 0;
    t->grow_dsc = NULL;

    int32_t item_id = item_start_id;

    lv_obj_t * item = lv_obj_get_child(cont, item_id);
    while(item) {
        if(item_id != item_start_id && lv_obj_has_flag(item, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK)) break;

        if(!lv_obj_has_flag_any(item, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) {
            uint8_t grow_value = lv_obj_get_style_flex_grow(item, LV_PART_MAIN);
            if(grow_value) {
                t->grow_item_cnt++;
                t->track_fix_main_size += item_gap;
                if(t->grow_dsc_calc) {
                    grow_dsc_t * new_dsc = lv_realloc(t->grow_dsc, sizeof(grow_dsc_t) * (t->grow_item_cnt));
                    LV_ASSERT_MALLOC(new_dsc);
                    if(new_dsc == NULL) return item_id;

                    new_dsc[t->grow_item_cnt - 1].item = item;
                    new_dsc[t->grow_item_cnt - 1].min_size = f->row ? lv_obj_get_style_min_width(item, LV_PART_MAIN)
                                                             : lv_obj_get_style_min_height(item, LV_PART_MAIN);
                    new_dsc[t->grow_item_cnt - 1].max_size = f->row ? lv_obj_get_style_max_width(item, LV_PART_MAIN)
                                                             : lv_obj_get_style_max_height(item, LV_PART_MAIN);
                    new_dsc[t->grow_item_cnt - 1].grow_value = grow_value;
                    new_dsc[t->grow_item_cnt - 1].clamped = 0;
                    t->grow_dsc = new_dsc;
                }
            }
            else {
                int32_t item_size = get_main_size(item);
                if(f->wrap && t->track_fix_main_size + item_size > max_main_size) break;
                t->track_fix_main_size += item_size + item_gap;
            }

            t->track_cross_size = LV_MAX(get_cross_size(item), t->track_cross_size);
            t->item_cnt++;
        }

        item_id += f->rev ? -1 : +1;
        if(item_id < 0) break;
        item = lv_obj_get_child(cont, item_id);
    }

    if(t->track_fix_main_size > 0) t->track_fix_main_size -= item_gap; /*There is no gap after the last item*/

    /*If there is at least one "grow item" the track takes the full space*/
    t->track_main_size = t->grow_item_cnt ? max_main_size : t->track_fix_main_size;

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
static void children_repos(lv_obj_t * cont, flex_t * f, int32_t item_first_id, int32_t item_last_id, int32_t abs_x,
                           int32_t abs_y, int32_t max_main_size, int32_t item_gap, track_t * t)
{
    void (*area_set_main_size)(lv_area_t *, int32_t) = (f->row ? lv_area_set_width : lv_area_set_height);
    int32_t (*area_get_main_size)(const lv_area_t *) = (f->row ? lv_area_get_width : lv_area_get_height);
    int32_t (*area_get_cross_size)(const lv_area_t *) = (!f->row ? lv_area_get_width : lv_area_get_height);

    typedef int32_t (*margin_func_t)(const lv_obj_t *, uint32_t);
    margin_func_t get_margin_main_start = (f->row ? lv_obj_get_style_margin_left : lv_obj_get_style_margin_top);
    margin_func_t get_margin_main_end = (f->row ? lv_obj_get_style_margin_right : lv_obj_get_style_margin_bottom);
    margin_func_t get_margin_cross_start = (!f->row ? lv_obj_get_style_margin_left : lv_obj_get_style_margin_top);
    margin_func_t get_margin_cross_end = (!f->row ? lv_obj_get_style_margin_right : lv_obj_get_style_margin_bottom);

    /*Calculate the size of grow items first*/
    uint32_t i;
    bool grow_reiterate  = true;
    while(grow_reiterate && t->grow_item_cnt) {
        grow_reiterate = false;
        int32_t grow_value_sum = 0;
        int32_t grow_max_size = t->track_main_size - t->track_fix_main_size;
        for(i = 0; i < t->grow_item_cnt; i++) {
            if(t->grow_dsc[i].clamped == 0) {
                grow_value_sum += t->grow_dsc[i].grow_value;
            }
            else {
                grow_max_size -= t->grow_dsc[i].final_size;
            }
        }
        int32_t grow_unit;

        for(i = 0; i < t->grow_item_cnt; i++) {
            if(t->grow_dsc[i].clamped == 0) {
                LV_ASSERT(grow_value_sum != 0);
                grow_unit = grow_max_size / grow_value_sum;
                int32_t size = grow_unit * t->grow_dsc[i].grow_value;
                int32_t size_clamp = LV_CLAMP(t->grow_dsc[i].min_size, size, t->grow_dsc[i].max_size);

                if(size_clamp != size) {
                    t->grow_dsc[i].clamped = 1;
                    grow_reiterate = true;
                }
                t->grow_dsc[i].final_size = size_clamp;
                grow_value_sum -= t->grow_dsc[i].grow_value;
                grow_max_size  -= t->grow_dsc[i].final_size;
            }
        }
    }

    bool rtl = lv_obj_get_style_base_dir(cont, LV_PART_MAIN) == LV_BASE_DIR_RTL;

    int32_t main_pos = 0;

    int32_t place_gap = 0;
    place_content(f->main_place, max_main_size, t->track_main_size, t->item_cnt, &main_pos, &place_gap);
    if(f->row && rtl) main_pos += lv_obj_get_content_width(cont);

    lv_obj_t * item = lv_obj_get_child(cont, item_first_id);
    /*Reposition the children*/
    while(item && item_first_id != item_last_id) {
        if(lv_obj_has_flag_any(item, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) {
            item = get_next_item(cont, f->rev, &item_first_id);
            continue;
        }
        int32_t grow_size = lv_obj_get_style_flex_grow(item, LV_PART_MAIN);
        if(grow_size) {
            int32_t s = 0;
            for(i = 0; i < t->grow_item_cnt; i++) {
                if(t->grow_dsc[i].item == item) {
                    s = t->grow_dsc[i].final_size;
                    break;
                }
            }

            if(f->row) {
                item->w_layout = 1;
                item->h_layout = 0;
            }
            else {
                item->h_layout = 1;
                item->w_layout = 0;
            }

            if(s != area_get_main_size(&item->coords)) {
                lv_obj_invalidate(item);

                lv_area_t old_coords;
                lv_area_copy(&old_coords, &item->coords);
                area_set_main_size(&item->coords, s);
                lv_obj_send_event(item, LV_EVENT_SIZE_CHANGED, &old_coords);
                lv_obj_send_event(lv_obj_get_parent(item), LV_EVENT_CHILD_CHANGED, item);
                lv_obj_invalidate(item);
            }
        }
        else {
            item->w_layout = 0;
            item->h_layout = 0;
        }

        int32_t cross_pos = 0;
        switch(f->cross_place) {
            case LV_FLEX_ALIGN_CENTER:
                /*Round up the cross size to avoid rounding error when dividing by 2
                 *The issue comes up e,g, with column direction with center cross direction if an element's width changes*/
                cross_pos = (((t->track_cross_size + 1) & (~1)) - area_get_cross_size(&item->coords)) / 2;
                cross_pos += (get_margin_cross_start(item, LV_PART_MAIN) - get_margin_cross_end(item, LV_PART_MAIN)) / 2;
                break;
            case LV_FLEX_ALIGN_END:
                cross_pos = t->track_cross_size - area_get_cross_size(&item->coords);
                cross_pos -= get_margin_cross_end(item, LV_PART_MAIN);
                break;
            default:
                cross_pos += get_margin_cross_start(item, LV_PART_MAIN);
                break;
        }

        if(f->row && rtl) main_pos -= area_get_main_size(&item->coords);

        /*Handle percentage value of translate*/
        int32_t tr_x = lv_obj_get_style_translate_x(item, LV_PART_MAIN);
        int32_t tr_y = lv_obj_get_style_translate_y(item, LV_PART_MAIN);
        int32_t w = lv_obj_get_width(item);
        int32_t h = lv_obj_get_height(item);
        if(LV_COORD_IS_PCT(tr_x)) tr_x = (w * LV_COORD_GET_PCT(tr_x)) / 100;
        if(LV_COORD_IS_PCT(tr_y)) tr_y = (h * LV_COORD_GET_PCT(tr_y)) / 100;

        int32_t diff_x = abs_x - item->coords.x1 + tr_x;
        int32_t diff_y = abs_y - item->coords.y1 + tr_y;
        diff_x += f->row ? main_pos + get_margin_main_start(item, LV_PART_MAIN) : cross_pos;
        diff_y += f->row ? cross_pos : main_pos + get_margin_main_start(item, LV_PART_MAIN);

        if(diff_x || diff_y) {
            lv_obj_invalidate(item);
            item->coords.x1 += diff_x;
            item->coords.x2 += diff_x;
            item->coords.y1 += diff_y;
            item->coords.y2 += diff_y;
            lv_obj_invalidate(item);
            lv_obj_move_children_by(item, diff_x, diff_y, false);
        }

        if(!(f->row && rtl)) main_pos += area_get_main_size(&item->coords) + item_gap + place_gap
                                             + get_margin_main_start(item, LV_PART_MAIN)
                                             + get_margin_main_end(item, LV_PART_MAIN);
        else main_pos -= item_gap + place_gap;

        item = get_next_item(cont, f->rev, &item_first_id);
    }
}

/**
 * Tell a start coordinate and gap for a placement type.
 */
static void place_content(lv_flex_align_t place, int32_t max_size, int32_t content_size, int32_t item_cnt,
                          int32_t * start_pos, int32_t * gap)
{
    if(item_cnt <= 1) {
        switch(place) {
            case LV_FLEX_ALIGN_SPACE_BETWEEN:
            case LV_FLEX_ALIGN_SPACE_AROUND:
            case LV_FLEX_ALIGN_SPACE_EVENLY:
                place = LV_FLEX_ALIGN_CENTER;
                break;
            default:
                break;
        }
    }

    switch(place) {
        case LV_FLEX_ALIGN_CENTER:
            *gap = 0;
            *start_pos += (max_size - content_size) / 2;
            break;
        case LV_FLEX_ALIGN_END:
            *gap = 0;
            *start_pos += max_size - content_size;
            break;
        case LV_FLEX_ALIGN_SPACE_BETWEEN:
            *gap = (int32_t)(max_size - content_size) / (int32_t)(item_cnt - 1);
            break;
        case LV_FLEX_ALIGN_SPACE_AROUND:
            *gap += (int32_t)(max_size - content_size) / (int32_t)(item_cnt);
            *start_pos += *gap / 2;
            break;
        case LV_FLEX_ALIGN_SPACE_EVENLY:
            *gap = (int32_t)(max_size - content_size) / (int32_t)(item_cnt + 1);
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
    }
    else {
        (*item_id)++;
        if((*item_id) < (int32_t)cont->spec_attr->child_cnt) return cont->spec_attr->children[*item_id];
        else return NULL;
    }
}

static int32_t lv_obj_get_width_with_margin(const lv_obj_t * obj)
{
    return lv_obj_get_style_margin_left(obj, LV_PART_MAIN)
           + lv_obj_get_width(obj)
           + lv_obj_get_style_margin_right(obj, LV_PART_MAIN);
}

static int32_t lv_obj_get_height_with_margin(const lv_obj_t * obj)
{
    return lv_obj_get_style_margin_top(obj, LV_PART_MAIN)
           + lv_obj_get_height(obj)
           + lv_obj_get_style_margin_bottom(obj, LV_PART_MAIN);
}

#endif /*LV_USE_FLEX*/
