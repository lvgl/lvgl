/**
 * @file lv_example_bar_img_indicator.c
 */

#include "../../../lv_examples.h"
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
    LV_FONT_DECLARE(font_example_large);

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
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Drag the slider to change subject_value; the bar reveals more of the segmented indicator image as the value rises. */
    lv_obj_t * lv_bar_0 = lv_bar_create(screen);
    lv_obj_set_size(lv_bar_0, 280, 35);
    lv_bar_set_min_value(lv_bar_0, 0);
    lv_bar_set_max_value(lv_bar_0, 20);
    lv_bar_bind_value(lv_bar_0, &subject_value2);
    lv_obj_add_style(lv_bar_0, &style_bar_track, LV_PART_MAIN);
    lv_obj_add_style(lv_bar_0, &style_bar_indicator, LV_PART_INDICATOR);

    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_cross_place(lv_obj_1, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_main_place(lv_obj_1, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_set_style_pad_column(lv_obj_1, 12, 0);
    lv_obj_set_style_bg_opa(lv_obj_1, 0, 0);
    lv_obj_set_style_border_width(lv_obj_1, 0, 0);
    lv_obj_set_size(lv_obj_1, 150, LV_SIZE_CONTENT);
    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_1);
    lv_obj_t * lv_label_0 = lv_label_create(lv_button_0);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_0, "-");
    lv_obj_set_style_text_font(lv_label_0, &font_example_large, 0);

    lv_obj_add_subject_increment_event(lv_button_0, &subject_value2, LV_EVENT_CLICKED, -1);
    lv_obj_add_subject_increment_event(lv_button_0, &subject_value2, LV_EVENT_LONG_PRESSED_REPEAT, -1);

    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
    lv_label_bind_text(lv_label_1, &subject_value2, NULL);
    lv_obj_set_style_text_font(lv_label_1, &font_example_large, 0);

    lv_obj_t * lv_button_1 = lv_button_create(lv_obj_1);
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "+");
    lv_obj_set_style_text_font(lv_label_2, &font_example_large, 0);

    lv_obj_add_subject_increment_event(lv_button_1, &subject_value2, LV_EVENT_CLICKED, 1);
    lv_obj_add_subject_increment_event(lv_button_1, &subject_value2, LV_EVENT_LONG_PRESSED_REPEAT, 1);
}
#endif
