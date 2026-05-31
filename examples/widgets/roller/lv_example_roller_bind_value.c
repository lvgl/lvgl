/**
 * @file lv_example_roller_bind_value.c
 */

#include "../../lv_examples.h"
#if LV_USE_ROLLER && LV_BUILD_EXAMPLES

/**
 * @title Roller bind value
 * @brief Roller bound to a subject; a button jumps it programmatically.
 *
 * The roller's `bind_value` is wired to `subject_index`. A "Wednesday" button uses
 * `subject_set_int_event` to write `3` into the subject on click — the roller
 * picks up the change and animates to that option without any direct call into
 * the roller widget.
 */
void lv_example_roller_bind_value(void)
{
    static lv_subject_t subject_index;

    static bool inited = false;

    if(!inited) {
        lv_subject_init_int(&subject_index, 0);
        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Scroll the roller, or click the button to snap to a fixed option through the subject. */
    lv_obj_t * roller = lv_roller_create(screen);
    lv_obj_set_width(roller, 160);
    lv_roller_set_visible_row_count(roller, 3);
    lv_roller_set_options(roller, "Mon\nTue\nWed\nThu\nFri\nSat\nSun", LV_ROLLER_MODE_NORMAL);
    lv_roller_bind_value(roller, &subject_index);

    lv_obj_t * button = lv_button_create(screen);
    lv_obj_t * label = lv_label_create(button);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "Jump to Wed");

    lv_obj_add_subject_set_int_event(button, &subject_index, LV_EVENT_CLICKED, 2);
}
#endif
