/**
 * @file lv_example_split_vertical.c
 */

#include "../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

#define SPLIT_DIVIDER_H 12
#define SPLIT_MIN_PANE  60

/* Drag the divider: resize the top pane; the bottom pane grows to fill the rest. */
static void splitter_pressing_v_cb(lv_event_t * e)
{
    lv_obj_t * divider = lv_event_get_target_obj(e);
    lv_obj_t * cont = lv_obj_get_parent(divider);
    lv_obj_t * pane_top = lv_obj_get_child(cont, 0);

    lv_indev_t * indev = lv_indev_active();
    if(indev == NULL) return;

    lv_point_t p;
    lv_indev_get_point(indev, &p);

    lv_area_t content;
    lv_obj_get_content_coords(cont, &content);
    int32_t avail = lv_area_get_height(&content) - SPLIT_DIVIDER_H;
    if(avail <= 2 * SPLIT_MIN_PANE) return;

    int32_t top_h = (p.y - content.y1) - SPLIT_DIVIDER_H / 2;
    if(top_h < SPLIT_MIN_PANE) top_h = SPLIT_MIN_PANE;
    if(top_h > avail - SPLIT_MIN_PANE) top_h = avail - SPLIT_MIN_PANE;

    lv_obj_set_height(pane_top, top_h);
}

/**
 * @title Vertical split view with a draggable divider
 * @brief Two stacked panes separated by a divider you can drag to resize them.
 *
 * Same idea as the horizontal split, but arranged in a flex column: a top pane
 * (fixed height), a draggable divider, and a bottom pane that grows to fill the
 * rest. Dragging the divider updates the top pane's height; the bottom pane
 * follows. A minimum height keeps either pane from collapsing.
 */
void lv_example_split_vertical(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollable(cont, false);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

    /* Top pane: fixed height (changes while dragging) */
    lv_obj_t * pane_top = lv_obj_create(cont);
    lv_obj_set_width(pane_top, lv_pct(100));
    lv_obj_set_height(pane_top, lv_pct(50));
    lv_obj_set_style_bg_color(pane_top, lv_color_hex(0xe8f0fe), 0);
    lv_obj_t * label_top = lv_label_create(pane_top);
    lv_label_set_text(label_top, "Top\n(drag the divider)");
    lv_obj_center(label_top);

    /* Divider: the draggable handle */
    lv_obj_t * divider = lv_obj_create(cont);
    lv_obj_set_size(divider, lv_pct(100), SPLIT_DIVIDER_H);
    lv_obj_set_scrollable(divider, false);
    lv_obj_set_style_radius(divider, 0, 0);
    lv_obj_set_style_border_width(divider, 0, 0);
    lv_obj_set_style_bg_color(divider, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_add_event_cb(divider, splitter_pressing_v_cb, LV_EVENT_PRESSING, NULL);

    /* Bottom pane: grows to fill the remaining space */
    lv_obj_t * pane_bottom = lv_obj_create(cont);
    lv_obj_set_width(pane_bottom, lv_pct(100));
    lv_obj_set_flex_grow(pane_bottom, 1);
    lv_obj_set_style_bg_color(pane_bottom, lv_color_hex(0xe6f4ea), 0);
    lv_obj_t * label_bottom = lv_label_create(pane_bottom);
    lv_label_set_text(label_bottom, "Bottom");
    lv_obj_center(label_bottom);
}

#endif
