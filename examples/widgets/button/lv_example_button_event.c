#include "../../lv_examples.h"
#if LV_USE_BUTTON && LV_USE_LABEL && LV_BUILD_EXAMPLES

static lv_obj_t * status_label;
static uint32_t click_count;

static void event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    click_count++;
    LV_LOG_USER("button clicked (%u)", (unsigned)click_count);
    /* Any LVGL API can be called from an event handler. */
    lv_label_set_text_fmt(status_label, "Clicks: %u", (unsigned)click_count);
}

/**
 * @title Button click event
 * @brief Count clicks and refresh a sibling label from the handler.
 *
 * `LV_EVENT_CLICKED` fires on a short press-release sequence. The handler
 * bumps a counter, logs it, and writes the new count into a label with
 * `lv_label_set_text_fmt`.
 */
void lv_example_button_event(void)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(scr, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(scr, 12, 0);

    lv_obj_t * button = lv_button_create(scr);
    lv_obj_add_event_cb(button, event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * btn_label = lv_label_create(button);
    lv_label_set_text(btn_label, "Click me");
    lv_obj_center(btn_label);

    status_label = lv_label_create(scr);
    lv_label_set_text(status_label, "Clicks: 0");
}

#endif
