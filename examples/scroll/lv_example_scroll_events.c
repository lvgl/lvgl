#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

static void scroll_event_cb(lv_event_t * e)
{
    lv_obj_t * panel = lv_event_get_target_obj(e);
    lv_obj_t * label = (lv_obj_t *)lv_event_get_user_data(e);

    /* Any scroll coordinate can be queried while the panel is moving. */
    lv_label_set_text_fmt(label,
                          "y: %" LV_PRId32 "   top: %" LV_PRId32 "   bottom: %" LV_PRId32,
                          lv_obj_get_scroll_y(panel),
                          lv_obj_get_scroll_top(panel),
                          lv_obj_get_scroll_bottom(panel));
}

/**
 * @title Reacting to scroll events
 * @brief Read the live scroll position from an LV_EVENT_SCROLL callback.
 *
 * A panel overflows with buttons so it can be scrolled. On every
 * `LV_EVENT_SCROLL` the handler reads `lv_obj_get_scroll_y`,
 * `lv_obj_get_scroll_top` and `lv_obj_get_scroll_bottom` and writes them
 * into a label, so the numbers update continuously as the panel is
 * dragged — the typical way to drive UI from the scroll position.
 */
void lv_example_scroll_events(void)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(scr, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(scr, 12, 0);

    lv_obj_t * label = lv_label_create(scr);
    lv_label_set_text(label, "Scroll the panel");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t * panel = lv_obj_create(scr);
    lv_obj_set_size(panel, 240, 170);
    lv_obj_center(panel);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(panel, scroll_event_cb, LV_EVENT_SCROLL, label);

    for(uint32_t i = 1; i <= 8; i++) {
        lv_obj_t * btn = lv_button_create(panel);
        lv_obj_set_width(btn, lv_pct(100));
        lv_obj_t * btn_label = lv_label_create(btn);
        lv_label_set_text_fmt(btn_label, "Item %" LV_PRIu32, i);
        lv_obj_center(btn_label);
    }
}

#endif