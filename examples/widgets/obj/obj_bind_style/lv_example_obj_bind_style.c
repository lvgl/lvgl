/**
 * @file lv_example_obj_bind_style.c
 */

#include "../../../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Obj bind style (theme toggle)
 * @brief Apply a named style only when a subject equals a reference value.
 *
 * The "dark" style is attached to the panel via `<bind_style>` with
 * `subject="subject_flag"` and `ref_value="1"`: it's applied whenever
 * `subject_flag` is 1 and removed otherwise. The toggle button flips
 * `subject_flag` through `subject_toggle_event`. This is the canonical
 * "binding-driven theme switch" pattern — no event callback in user code.
 */
void lv_example_obj_bind_style(void)
{
    static lv_style_t style_dark;

    static lv_subject_t subject_flag;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_dark);
        lv_style_set_bg_color(&style_dark, lv_color_hex(0x1f2937));
        lv_style_set_text_color(&style_dark, lv_color_hex(0xf9fafb));
        lv_style_set_border_color(&style_dark, lv_color_hex(0x4b5563));

        lv_subject_init_int(&subject_flag, 0);
        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Click the toggle to flip `subject_flag` between 0 and 1; the panel restyles via `bind_style`. */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_size(container, 240, 120);
    lv_obj_set_style_pad_all(container, 16, 0);
    lv_obj_set_style_radius(container, 8, 0);
    lv_obj_set_style_border_width(container, 1, 0);
    lv_obj_bind_style(container, &style_dark, 0, &subject_flag, 1);
    lv_obj_t * label_1 = lv_label_create(container);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Panel content");

    lv_obj_t * button = lv_button_create(screen);
    lv_obj_t * label_2 = lv_label_create(button);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Toggle theme");

    lv_obj_add_subject_toggle_event(button, &subject_flag, LV_EVENT_CLICKED);
}
#endif
