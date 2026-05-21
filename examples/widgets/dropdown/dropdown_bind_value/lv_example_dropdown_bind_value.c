/**
 * @file lv_example_dropdown_bind_value.c
 */

#include "../../../lv_examples.h"
#if LV_USE_DROPDOWN && LV_BUILD_EXAMPLES

/**
 * @title Roller bind value
 * @brief Roller bound to a subject; a button jumps it programmatically.
 *
 * The roller's `bind_value` is wired to `subject_index`. A "Wednesday" button uses
 * `subject_set_int_event` to write `3` into the subject on click — the roller
 * picks up the change and animates to that option without any direct call into
 * the roller widget.
 */
void lv_example_dropdown_bind_value(void)
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
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_label_bind_text(lv_label_0, &subject_index, "Selected option: %d");

    lv_obj_t * lv_roller_0 = lv_roller_create(screen);
    lv_obj_set_width(lv_roller_0, 160);
    lv_roller_set_visible_row_count(lv_roller_0, 3);
    lv_roller_set_options(lv_roller_0, "Mon\nTue\nWed\nThu\nFri\nSat\nSun", LV_ROLLER_MODE_NORMAL);
    lv_roller_bind_value(lv_roller_0, &subject_index);

    lv_obj_t * lv_button_0 = lv_button_create(screen);
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "Jump to Wed");

    lv_obj_add_subject_set_int_event(lv_button_0, &subject_index, LV_EVENT_CLICKED, 2);
}
#endif
