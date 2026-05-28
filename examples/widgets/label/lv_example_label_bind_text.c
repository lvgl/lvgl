/**
 * @file lv_example_label_bind_text.c
 */

#include "../../lv_examples.h"
#if LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * @title Label bind text
 * @brief Bind a label to a string subject; buttons rewrite the subject on click.
 *
 * `subject_text` is a string subject from `examples/xml_project/globals.xml`. The
 * label uses `bind_text` to listen for changes. Each button carries a
 * `<subject_set_string_event>` child that, on click, writes a fixed value into
 * `subject_text` — and the label re-renders automatically.
 */
void lv_example_label_bind_text(void)
{
    static lv_subject_t subject_text;
    static char subject_text_buf[256];
    static char subject_text_prev_buf[256];

    static bool inited = false;

    if(!inited) {
        lv_subject_init_string(&subject_text,
                               subject_text_buf,
                               subject_text_prev_buf,
                               256,
                               "Hello");
        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Click either button; the label updates because it reads `subject_text` live. */
    lv_obj_t * label_1 = lv_label_create(screen);
    lv_label_bind_text(label_1, &subject_text, NULL);

    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(container, 12, 0);
    lv_obj_set_style_bg_opa(container, 0, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_size(container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_t * button_1 = lv_button_create(container);
    lv_obj_t * label_2 = lv_label_create(button_1);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Idle");

    lv_obj_add_subject_set_string_event(button_1, &subject_text, LV_EVENT_CLICKED, "Idle");

    lv_obj_t * button_2 = lv_button_create(container);
    lv_obj_t * label_3 = lv_label_create(button_2);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "Running");

    lv_obj_add_subject_set_string_event(button_2, &subject_text, LV_EVENT_CLICKED, "Running");
}
#endif
