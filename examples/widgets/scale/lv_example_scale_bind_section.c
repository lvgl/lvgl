/**
 * @file lv_example_scale_bind_section.c
 */

#include "../../lv_examples.h"
#if LV_USE_SCALE && LV_BUILD_EXAMPLES

/**
 * @title Scale section bind min/max
 * @brief Drive a scale section's range live from two subjects.
 *
 * The scale has a single styled section whose `bind_min_value`/`bind_max_value`
 * are wired to `subject_value2` and `subject_value` respectively. Two sliders
 * edit those subjects, so dragging them moves the start and end of the
 * highlighted band on the scale — useful for "safe range" indicators on gauges.
 */
void lv_example_scale_bind_section(void)
{
    static lv_style_t style_section_main;
    static lv_style_t style_section_items;
    static lv_style_t style_section_indicator;

    static lv_subject_t subject_value;
    static lv_subject_t subject_value2;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_section_main);
        lv_style_set_line_color(&style_section_main, lv_color_hex(0xef4444));
        lv_style_set_line_width(&style_section_main, 4);
        lv_style_set_text_color(&style_section_main, lv_color_hex(0xef4444));

        lv_style_init(&style_section_items);
        lv_style_set_line_color(&style_section_items, lv_color_hex(0xef4444));
        lv_style_set_line_width(&style_section_items, 2);
        lv_style_set_text_color(&style_section_items, lv_color_hex(0xef4444));

        lv_style_init(&style_section_indicator);
        lv_style_set_line_color(&style_section_indicator, lv_color_hex(0xef4444));
        lv_style_set_line_width(&style_section_indicator, 4);
        lv_style_set_text_color(&style_section_indicator, lv_color_hex(0xef4444));

        lv_subject_init_int(&subject_value, 50);
        lv_subject_set_min_value_int(&subject_value, 0);
        lv_subject_set_max_value_int(&subject_value, 100);
        lv_subject_init_int(&subject_value2, 20);
        lv_subject_set_min_value_int(&subject_value2, 0);
        lv_subject_set_max_value_int(&subject_value2, 100);
        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Move the sliders; the red section on the scale grows and shrinks because its bounds are bound to the subjects. */
    lv_obj_t * scale = lv_scale_create(screen);
    lv_obj_set_size(scale, 280, 80);
    lv_scale_set_mode(scale, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_total_tick_count(scale, 11);
    lv_scale_set_major_tick_every(scale, 2);
    lv_scale_set_label_show(scale, true);
    lv_scale_set_min_value(scale, 0);
    lv_scale_set_max_value(scale, 100);
    lv_scale_section_t * lv_scale_section_0 = lv_scale_add_section(scale);
    lv_scale_set_section_style_main(scale, lv_scale_section_0, &style_section_main);
    lv_scale_set_section_style_items(scale, lv_scale_section_0, &style_section_items);
    lv_scale_set_section_style_indicator(scale, lv_scale_section_0, &style_section_indicator);
    lv_scale_bind_section_min_value(scale, lv_scale_section_0, &subject_value2);
    lv_scale_bind_section_max_value(scale, lv_scale_section_0, &subject_value);

    lv_obj_t * label_1 = lv_label_create(screen);
    lv_label_set_text(label_1, "Section start (subject_value2)");

    lv_obj_t * slider_1 = lv_slider_create(screen);
    lv_obj_set_width(slider_1, 240);
    lv_slider_bind_value(slider_1, &subject_value2);

    lv_obj_t * label_2 = lv_label_create(screen);
    lv_label_set_text(label_2, "Section end (subject_value)");

    lv_obj_t * slider_2 = lv_slider_create(screen);
    lv_obj_set_width(slider_2, 240);
    lv_slider_bind_value(slider_2, &subject_value);
}
#endif
