#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_FLEX

/* Applies to item width (row) and item height (column). */
#define ITEM_SIZE 80

static int32_t content_size(lv_obj_t * cont, bool horizontal)
{
    int32_t total = (int32_t)lv_obj_get_child_count(cont);
    int32_t gap = horizontal ? lv_obj_get_style_pad_column(cont, LV_PART_MAIN)
                  : lv_obj_get_style_pad_row(cont, LV_PART_MAIN);
    int32_t s = 0;
    for(int32_t i = 0; i < total; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, i);
        s += horizontal ? lv_obj_get_width(child) : lv_obj_get_height(child);
        if(i < total - 1) s += gap;
    }
    if(horizontal) {
        return s + lv_obj_get_style_pad_left(cont, LV_PART_MAIN) + lv_obj_get_style_pad_right(cont, LV_PART_MAIN);
    }
    return s + lv_obj_get_style_pad_top(cont, LV_PART_MAIN) + lv_obj_get_style_pad_bottom(cont, LV_PART_MAIN);
}

static void wrap_scroll_cb(lv_event_t * e)
{
    static bool adjusting = false;
    if(adjusting) return;

    lv_obj_t * cont = lv_event_get_current_target_obj(e);
    bool horizontal = lv_obj_get_style_flex_flow(cont, LV_PART_MAIN) == LV_FLEX_FLOW_ROW;

    int32_t scroll = horizontal ? lv_obj_get_scroll_x(cont) : lv_obj_get_scroll_y(cont);
    int32_t view = horizontal ? lv_obj_get_width(cont) : lv_obj_get_height(cont);
    int32_t content = content_size(cont, horizontal);
    int32_t last = (int32_t)lv_obj_get_child_count(cont) - 1;

    adjusting = true;
    if(scroll <= 0) {
        /* Reached the start: move the last child to the front. */
        lv_obj_move_to_index(lv_obj_get_child(cont, last), 0);
        if(horizontal) lv_obj_scroll_to_x(cont, scroll + ITEM_SIZE, LV_ANIM_OFF);
        else lv_obj_scroll_to_y(cont, scroll + ITEM_SIZE, LV_ANIM_OFF);
    }
    else if(scroll > content - view) {
        /* Reached the end: move the first child to the back. */
        lv_obj_move_to_index(lv_obj_get_child(cont, 0), last);
        if(horizontal) lv_obj_scroll_to_x(cont, scroll - ITEM_SIZE, LV_ANIM_OFF);
        else lv_obj_scroll_to_y(cont, scroll - ITEM_SIZE, LV_ANIM_OFF);
    }
    adjusting = false;
}

static lv_obj_t * make_strip(lv_obj_t * parent, lv_flex_flow_t flow, int32_t w, int32_t h)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, w, h);
    lv_obj_set_flex_flow(cont, flow);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(cont, wrap_scroll_cb, LV_EVENT_SCROLL, NULL);

    bool horizontal = flow == LV_FLEX_FLOW_ROW;
    for(uint32_t i = 1; i <= 10; i++) {
        lv_obj_t * btn = lv_button_create(cont);
        lv_obj_set_size(btn, horizontal ? ITEM_SIZE : LV_PCT(100),
                        horizontal ? LV_PCT(100) : ITEM_SIZE);
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "Item %" LV_PRIu32, i);
        lv_obj_center(label);
    }
    return cont;
}

/**
 * @title Endless circular scrolling
 * @brief Wrap items from one edge to the other so a strip scrolls without limits.
 *
 * A row strip and a column strip each hold ten buttons. Their
 * `LV_EVENT_SCROLL` callback detects when either edge is reached and uses
 * `lv_obj_move_to_index` to move the boundary child across, then
 * compensates with `lv_obj_scroll_to_x/y` so the visible content does not
 * jump — making the finite list feel infinite in both directions.
 */
void lv_example_scroll_circular(void)
{
    lv_obj_t * scr = lv_screen_active();

    lv_obj_t * row = make_strip(scr, LV_FLEX_FLOW_ROW, 300, 75);
    lv_obj_align(row, LV_ALIGN_TOP_MID, 0, 5);

    lv_obj_t * col = make_strip(scr, LV_FLEX_FLOW_COLUMN, 200, 150);
    lv_obj_align_to(col, row, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
}

#endif