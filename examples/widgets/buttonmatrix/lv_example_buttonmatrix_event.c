#include "../../lv_examples.h"
#if LV_USE_BUTTONMATRIX && LV_USE_LABEL && LV_BUILD_EXAMPLES

static lv_obj_t * status_label;

static void event_cb(lv_event_t * e)
{
    lv_obj_t * bm = lv_event_get_target_obj(e);
    uint32_t id = lv_buttonmatrix_get_selected_button(bm);
    const char * text = lv_buttonmatrix_get_button_text(bm, id);
    LV_LOG_USER("buttonmatrix: pressed %u (\"%s\")", (unsigned)id, text ? text : "");
    lv_label_set_text_fmt(status_label, "Pressed: %s", text ? text : "");
}

/**
 * @title Button matrix value-changed event
 * @brief Show which virtual button was pressed in a sibling label.
 *
 * `LV_EVENT_VALUE_CHANGED` fires on every press.
 * `lv_buttonmatrix_get_selected_button` returns the zero-based id;
 * `_get_button_text` resolves it to the label string. The handler writes
 * that string into a sibling label via `lv_label_set_text_fmt`.
 */
void lv_example_buttonmatrix_event(void)
{
    static const char * map[] = {"1", "2", "3", "\n", "4", "5", "6", "\n", "7", "8", "9", NULL};

    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(scr, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(scr, 12, 0);

    lv_obj_t * bm = lv_buttonmatrix_create(scr);
    lv_obj_set_size(bm, 200, 180);
    lv_buttonmatrix_set_map(bm, map);
    lv_obj_add_event_cb(bm, event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    status_label = lv_label_create(scr);
    lv_label_set_text(status_label, "Pressed: —");
}

#endif
