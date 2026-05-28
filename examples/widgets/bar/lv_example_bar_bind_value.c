/**
 * @file lv_example_bar_bind_value.c
 */

#include "../../lv_examples.h"
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
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Tap +/- to step; hold to repeat. Reach the limit to see rollover. */
    lv_obj_t * bar = lv_bar_create(screen);
    lv_obj_set_size(bar, lv_pct(90), 20);
    lv_bar_bind_value(bar, &subject_value);

    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_cross_place(container, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_main_place(container, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_set_style_pad_column(container, 12, 0);
    lv_obj_set_style_bg_opa(container, 0, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_size(container, 180, LV_SIZE_CONTENT);
    lv_obj_t * button_1 = lv_button_create(container);
    lv_obj_t * label_2 = lv_label_create(button_1);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "-");

    lv_subject_increment_dsc_t * subject_increment_event_0 = lv_obj_add_subject_increment_event(button_1, &subject_value,
                                                                                                LV_EVENT_CLICKED, -5);
    lv_obj_set_subject_increment_event_min_value(button_1, subject_increment_event_0, 0);
    lv_obj_set_subject_increment_event_max_value(button_1, subject_increment_event_0, 100);
    lv_obj_set_subject_increment_event_rollover(button_1, subject_increment_event_0, true);
    lv_subject_increment_dsc_t * subject_increment_event_1 = lv_obj_add_subject_increment_event(button_1, &subject_value,
                                                                                                LV_EVENT_LONG_PRESSED_REPEAT, -5);
    lv_obj_set_subject_increment_event_min_value(button_1, subject_increment_event_1, 0);
    lv_obj_set_subject_increment_event_max_value(button_1, subject_increment_event_1, 100);
    lv_obj_set_subject_increment_event_rollover(button_1, subject_increment_event_1, true);

    lv_obj_t * label_1 = lv_label_create(container);
    lv_label_bind_text(label_1, &subject_value, "%d%%");

    lv_obj_t * button_2 = lv_button_create(container);
    lv_obj_t * label_3 = lv_label_create(button_2);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "+");

    lv_subject_increment_dsc_t * subject_increment_event_2 = lv_obj_add_subject_increment_event(button_2, &subject_value,
                                                                                                LV_EVENT_CLICKED, 5);
    lv_obj_set_subject_increment_event_min_value(button_2, subject_increment_event_2, 0);
    lv_obj_set_subject_increment_event_max_value(button_2, subject_increment_event_2, 100);
    lv_obj_set_subject_increment_event_rollover(button_2, subject_increment_event_2, true);
    lv_subject_increment_dsc_t * subject_increment_event_3 = lv_obj_add_subject_increment_event(button_2, &subject_value,
                                                                                                LV_EVENT_LONG_PRESSED_REPEAT, 5);
    lv_obj_set_subject_increment_event_min_value(button_2, subject_increment_event_3, 0);
    lv_obj_set_subject_increment_event_max_value(button_2, subject_increment_event_3, 100);
    lv_obj_set_subject_increment_event_rollover(button_2, subject_increment_event_3, true);
}
#endif
