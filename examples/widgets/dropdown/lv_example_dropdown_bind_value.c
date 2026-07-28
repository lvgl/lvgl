/**
 * @file lv_example_dropdown_bind_value.c
 */

#include "../../lv_examples.h"
#if LV_USE_DROPDOWN && LV_BUILD_EXAMPLES

/**
 * @title Dropdown bind value
 * @brief Dropdown bound to a subject; a button jumps it programmatically.
 *
 * The dropdown's `bind_value` is wired to `subject_index`. A "Pick Cherry" button uses
 * `subject_set_int_event` to write `2` into the subject on click — the dropdown
 * picks up the change and updates the selection without any direct call into
 * the dropdown widget.
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

    /* 💡 Open the dropdown to pick an option, or click the button to snap to a fixed one through the subject. */
    lv_obj_t * label_1 = lv_label_create(screen);
    lv_label_bind_text(label_1, &subject_index, "Selected option: %d");

    lv_obj_t * dropdown = lv_dropdown_create(screen);
    lv_obj_set_width(dropdown, 160);
    lv_dropdown_set_options(dropdown, "Apple\nBanana\nCherry\nDate\nElderberry");
    lv_dropdown_bind_value(dropdown, &subject_index);

    lv_obj_t * button = lv_button_create(screen);
    lv_obj_t * label_2 = lv_label_create(button);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Pick Cherry");

    lv_obj_add_subject_set_int_event(button, &subject_index, LV_EVENT_CLICKED, 2);
}
#endif
