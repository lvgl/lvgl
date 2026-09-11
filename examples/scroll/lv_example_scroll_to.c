#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

static lv_obj_t * panel;
static int32_t saved_scroll_y;

static void jump_cb(lv_event_t * e)
{
    /* user_data is the target scroll-y, or -1 for "scroll to the bottom". */
    int32_t target = (int32_t)(lv_intptr_t)lv_event_get_user_data(e);
    if(target < 0) target = lv_obj_get_scroll_bottom(panel) + lv_obj_get_scroll_y(panel);
    lv_obj_scroll_to_y(panel, target, LV_ANIM_ON);
}

static void save_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    saved_scroll_y = lv_obj_get_scroll_y(panel);
}

static void restore_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_obj_scroll_to_y(panel, saved_scroll_y, LV_ANIM_ON);
}

static lv_obj_t * make_button(lv_obj_t * parent, const char * txt, lv_event_cb_t cb, void * data)
{
    lv_obj_t * btn = lv_button_create(parent);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, data);
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, txt);
    lv_obj_center(label);
    return btn;
}

/**
 * @title Scrolling programmatically
 * @brief Jump, save and restore the scroll position from code with animation.
 *
 * The panel scrolls because its content overflows. Four buttons drive it
 * without any user gesture: "Top" and "Bottom" animate to the extremes
 * via `lv_obj_scroll_to_y` with `LV_ANIM_ON`; "Save" records the current
 * `lv_obj_get_scroll_y`; "Restore" animates back to the saved offset.
 */
void lv_example_scroll_to(void)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(scr, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(scr, 10, 0);

    lv_obj_t * bar = lv_obj_create(scr);
    lv_obj_set_size(bar, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bar, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    make_button(bar, "Top", jump_cb, (void *)(lv_intptr_t)0);
    make_button(bar, "Bottom", jump_cb, (void *)(lv_intptr_t) - 1);
    make_button(bar, "Save", save_cb, NULL);
    make_button(bar, "Restore", restore_cb, NULL);

    panel = lv_obj_create(scr);
    lv_obj_set_size(panel, 260, 150);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);

    for(uint32_t i = 1; i <= 12; i++) {
        lv_obj_t * btn = lv_button_create(panel);
        lv_obj_set_width(btn, lv_pct(100));
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "Row %" LV_PRIu32, i);
        lv_obj_center(label);
    }
}

#endif