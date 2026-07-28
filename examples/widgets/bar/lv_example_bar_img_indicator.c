/**
 * @file lv_example_bar_img_indicator.c
 */

#include "../../lv_examples.h"
#if LV_USE_BAR && LV_BUILD_EXAMPLES

/**
 * @title Bar image indicator
 * @brief Use bitmaps as the bar track and indicator — the bar clips each image to its part.
 *
 * `bg_image_src` replaces the solid background fill with pixels sampled from a
 * bitmap. Set it on `selector="main"` for the background track and on
 * `selector="indicator"` for the filled portion. As the value rises, the indicator
 * image is revealed left-to-right, showing cyan segments that transition to yellow
 * then red at the high end. Both images are 280×30, matching the bar widget size.
 */
void lv_example_bar_img_indicator(void)
{
    LV_IMAGE_DECLARE(img_bar_bg);
    LV_IMAGE_DECLARE(img_bar_indicator);

    static lv_style_t style_bar_track;
    static lv_style_t style_bar_indicator;

    static lv_subject_t subject_value2;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_bar_track);
        lv_style_set_bg_image_src(&style_bar_track, &img_bar_bg);
        lv_style_set_bg_opa(&style_bar_track, 0);
        lv_style_set_radius(&style_bar_track, 0);

        lv_style_init(&style_bar_indicator);
        lv_style_set_bg_image_src(&style_bar_indicator, &img_bar_indicator);
        lv_style_set_bg_opa(&style_bar_indicator, 0);
        lv_style_set_radius(&style_bar_indicator, 0);

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

    /* 💡 Drag the slider to change subject_value; the bar reveals more of the segmented indicator image as the value rises. */
    lv_obj_t * bar = lv_bar_create(screen);
    lv_obj_set_size(bar, 280, 35);
    lv_bar_set_min_value(bar, 0);
    lv_bar_set_max_value(bar, 20);
    lv_bar_bind_value(bar, &subject_value2);
    lv_obj_add_style(bar, &style_bar_track, LV_PART_MAIN);
    lv_obj_add_style(bar, &style_bar_indicator, LV_PART_INDICATOR);

    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_cross_place(container, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_main_place(container, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_set_style_pad_column(container, 12, 0);
    lv_obj_set_style_bg_opa(container, 0, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_size(container, 180, LV_SIZE_CONTENT);
    lv_obj_t * button_1 = lv_button_create(container);
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "-");

    lv_obj_add_subject_increment_event(button_1, &subject_value2, LV_EVENT_CLICKED, -1);
    lv_obj_add_subject_increment_event(button_1, &subject_value2, LV_EVENT_LONG_PRESSED_REPEAT, -1);

    lv_obj_t * label_2 = lv_label_create(container);
    lv_label_bind_text(label_2, &subject_value2, NULL);

    lv_obj_t * button_2 = lv_button_create(container);
    lv_obj_t * label_3 = lv_label_create(button_2);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "+");

    lv_obj_add_subject_increment_event(button_2, &subject_value2, LV_EVENT_CLICKED, 1);
    lv_obj_add_subject_increment_event(button_2, &subject_value2, LV_EVENT_LONG_PRESSED_REPEAT, 1);
}
#endif
