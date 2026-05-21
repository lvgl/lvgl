/**
 * @file lv_example_bar_bind_value.c
 */

#include "../../../lv_examples.h"
#if LV_USE_BAR && LV_BUILD_EXAMPLES

/**
 * @title Bar bind value with increment buttons
 * @brief Mutate a bound int subject via `subject_increment_event` instead of dragging.
 *
 * The bar listens to `subject_value`. Each +/- button carries two
 * `<subject_increment_event>` children — one on `clicked` and one on
 * `long_pressed_repeat` — so a tap nudges by `step` and a hold keeps repeating.
 * `min_value`/`max_value` clamp the result and `rollover="true"` wraps around
 * once the bound is hit; hold past 100 to see it wrap back to 0.
 */
void lv_example_bar_bind_value(void)
{
    static lv_subject_t subject_value;

    static bool inited = false;

    if(!inited) {
        lv_subject_init_int(&subject_value, 50);
        lv_subject_set_min_value_int(&subject_value, 0);
        lv_subject_set_max_value_int(&subject_value, 100);
        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Tap +/- to step; hold to repeat. Reach the limit to see rollover. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Bar: increment via events");

    lv_obj_t * lv_bar_0 = lv_bar_create(screen);
    lv_obj_set_size(lv_bar_0, lv_pct(90), 20);
    lv_bar_bind_value(lv_bar_0, &subject_value);

    lv_obj_t * lv_label_1 = lv_label_create(screen);
    lv_label_bind_text(lv_label_1, &subject_value, "%d%%");

    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(lv_obj_1, 12, 0);
    lv_obj_set_style_bg_opa(lv_obj_1, 0, 0);
    lv_obj_set_style_border_width(lv_obj_1, 0, 0);
    lv_obj_set_size(lv_obj_1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_1);
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_0);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "-");

    lv_subject_increment_dsc_t * subject_increment_event_0 = lv_obj_add_subject_increment_event(lv_button_0, &subject_value,
                                                                                                LV_EVENT_CLICKED, -5);
    lv_obj_set_subject_increment_event_min_value(lv_button_0, subject_increment_event_0, 0);
    lv_obj_set_subject_increment_event_max_value(lv_button_0, subject_increment_event_0, 100);
    lv_obj_set_subject_increment_event_rollover(lv_button_0, subject_increment_event_0, true);
    lv_subject_increment_dsc_t * subject_increment_event_1 = lv_obj_add_subject_increment_event(lv_button_0, &subject_value,
                                                                                                LV_EVENT_LONG_PRESSED_REPEAT, -5);
    lv_obj_set_subject_increment_event_min_value(lv_button_0, subject_increment_event_1, 0);
    lv_obj_set_subject_increment_event_max_value(lv_button_0, subject_increment_event_1, 100);
    lv_obj_set_subject_increment_event_rollover(lv_button_0, subject_increment_event_1, true);

    lv_obj_t * lv_button_1 = lv_button_create(lv_obj_1);
    lv_obj_t * lv_label_3 = lv_label_create(lv_button_1);
    lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_3, "+");

    lv_subject_increment_dsc_t * subject_increment_event_2 = lv_obj_add_subject_increment_event(lv_button_1, &subject_value,
                                                                                                LV_EVENT_CLICKED, 5);
    lv_obj_set_subject_increment_event_min_value(lv_button_1, subject_increment_event_2, 0);
    lv_obj_set_subject_increment_event_max_value(lv_button_1, subject_increment_event_2, 100);
    lv_obj_set_subject_increment_event_rollover(lv_button_1, subject_increment_event_2, true);
    lv_subject_increment_dsc_t * subject_increment_event_3 = lv_obj_add_subject_increment_event(lv_button_1, &subject_value,
                                                                                                LV_EVENT_LONG_PRESSED_REPEAT, 5);
    lv_obj_set_subject_increment_event_min_value(lv_button_1, subject_increment_event_3, 0);
    lv_obj_set_subject_increment_event_max_value(lv_button_1, subject_increment_event_3, 100);
    lv_obj_set_subject_increment_event_rollover(lv_button_1, subject_increment_event_3, true);
}
#endif
