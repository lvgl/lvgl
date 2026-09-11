#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_FLEX && LV_USE_SWITCH

static lv_obj_t * list;

typedef void (*scroll_flag_setter_t)(lv_obj_t *, bool);

static const struct {
    const char * title;
    scroll_flag_setter_t setter;
} scroll_flags[] = {
    { "Scrollable",      lv_obj_set_scrollable },
    { "Scroll chain",    lv_obj_set_scroll_chain },
    { "Elastic scroll",  lv_obj_set_scroll_elastic },
    { "Scroll momentum", lv_obj_set_scroll_momentum },
};

static void flag_switch_cb(lv_event_t * e)
{
    lv_obj_t * sw = lv_event_get_target_obj(e);
    uint32_t idx = (uint32_t)(lv_uintptr_t)lv_event_get_user_data(e);
    scroll_flags[idx].setter(list, lv_obj_has_state(sw, LV_STATE_CHECKED));
}

static void add_flag_row(lv_obj_t * parent, uint32_t idx)
{
    lv_obj_t * row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * label = lv_label_create(row);
    lv_label_set_text(label, scroll_flags[idx].title);
    lv_obj_set_flex_grow(label, 1);

    lv_obj_t * sw = lv_switch_create(row);
    lv_obj_add_state(sw, LV_STATE_CHECKED);          /* flags start enabled */
    scroll_flags[idx].setter(list, true);
    lv_obj_add_event_cb(sw, flag_switch_cb, LV_EVENT_VALUE_CHANGED, (void *)(lv_uintptr_t)idx);
}

/**
 * @title Toggling scroll behaviour flags
 * @brief Switch SCROLLABLE, SCROLL_CHAIN, SCROLL_ELASTIC and SCROLL_MOMENTUM on live.
 *
 * These four flags change how a Widget reacts to scrolling but only show
 * their effect through interaction, so one example covers them together.
 * A scrollable list sits above four switches; each switch toggles the
 * corresponding behaviour on the list with its dedicated setter (e.g.
 * `lv_obj_set_scrollable`), so the change can be felt immediately by dragging
 * the list.
 */
void lv_example_scroll_properties(void)
{
    lv_obj_t * panel = lv_obj_create(lv_screen_active());
    lv_obj_set_size(panel, 300, 220);
    lv_obj_center(panel);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);

    list = lv_obj_create(panel);
    lv_obj_set_size(list, lv_pct(100), 110);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    for(uint32_t i = 1; i <= 8; i++) {
        lv_obj_t * btn = lv_button_create(list);
        lv_obj_set_width(btn, lv_pct(100));
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "Item %" LV_PRIu32, i);
        lv_obj_center(label);
    }

    for(uint32_t i = 0; i < sizeof(scroll_flags) / sizeof(scroll_flags[0]); i++) {
        add_flag_row(panel, i);
    }
}

#endif
