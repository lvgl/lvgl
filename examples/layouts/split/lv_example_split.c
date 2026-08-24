/**
 * @file lv_example_split.c
 */

#include "../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

#define SPLIT_DIVIDER_W 12
#define SPLIT_MIN_PANE  60

/* Drag the divider: resize the left pane; the right pane grows to fill the rest. */
static void splitter_pressing_cb(lv_event_t * e)
{
    lv_obj_t * divider = lv_event_get_target_obj(e);
    lv_obj_t * cont = lv_obj_get_parent(divider);
    lv_obj_t * pane_left = lv_obj_get_child(cont, 0);

    lv_indev_t * indev = lv_indev_active();
    if(indev == NULL) return;

    lv_point_t p;
    lv_indev_get_point(indev, &p);

    lv_area_t content;
    lv_obj_get_content_coords(cont, &content);
    int32_t avail = lv_area_get_width(&content) - SPLIT_DIVIDER_W;
    if(avail <= 2 * SPLIT_MIN_PANE) return;

    int32_t left_w = (p.x - content.x1) - SPLIT_DIVIDER_W / 2;
    if(left_w < SPLIT_MIN_PANE) left_w = SPLIT_MIN_PANE;
    if(left_w > avail - SPLIT_MIN_PANE) left_w = avail - SPLIT_MIN_PANE;

    lv_obj_set_width(pane_left, left_w);
}

/**
 * @title Split view with a draggable divider
 * @brief Two panes separated by a divider you can drag to resize them.
 *
 * A flex row holds a left pane (fixed width), a draggable divider, and a right
 * pane that grows to fill the rest. Dragging the divider updates the left
 * pane's width; the right pane follows automatically. A minimum width keeps
 * either pane from collapsing. No custom widget is needed - just a flex
 * container and a pointer event handler on the divider.
 */
void lv_example_split(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollable(cont, false);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);

    /* Left pane: fixed width (changes while dragging) */
    lv_obj_t * pane_left = lv_obj_create(cont);
    lv_obj_set_height(pane_left, lv_pct(100));
    lv_obj_set_width(pane_left, lv_pct(50));
    lv_obj_set_style_bg_color(pane_left, lv_color_hex(0xe8f0fe), 0);
    lv_obj_t * label_left = lv_label_create(pane_left);
    lv_label_set_text(label_left, "Left\n(drag the divider)");
    lv_obj_center(label_left);

    /* Divider: the draggable handle */
    lv_obj_t * divider = lv_obj_create(cont);
    lv_obj_set_size(divider, SPLIT_DIVIDER_W, lv_pct(100));
    lv_obj_set_scrollable(divider, false);
    lv_obj_set_style_radius(divider, 0, 0);
    lv_obj_set_style_border_width(divider, 0, 0);
    lv_obj_set_style_bg_color(divider, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_add_event_cb(divider, splitter_pressing_cb, LV_EVENT_PRESSING, NULL);

    /* Right pane: grows to fill the remaining space */
    lv_obj_t * pane_right = lv_obj_create(cont);
    lv_obj_set_height(pane_right, lv_pct(100));
    lv_obj_set_flex_grow(pane_right, 1);
    lv_obj_set_style_bg_color(pane_right, lv_color_hex(0xe6f4ea), 0);
    lv_obj_t * label_right = lv_label_create(pane_right);
    lv_label_set_text(label_right, "Right");
    lv_obj_center(label_right);
}

#endif
