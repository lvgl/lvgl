/**
 * @file lv_example_slider_img_indicator.c
 */

#include "../../lv_examples.h"
#if LV_USE_SLIDER && LV_BUILD_EXAMPLES

/**
 * @title Slider image indicator
 * @brief Use bitmaps as the slider track and indicator — the slider clips each image to its part.
 *
 * `bg_image_src` on `selector="main"` textures the background track and on
 * `selector="indicator"` textures the filled portion. As the knob moves right the
 * indicator image is revealed left-to-right, showing cyan segments that transition
 * to yellow then red at the high end. Both images are 280×30, matching the slider
 * width.
 */
void lv_example_slider_img_indicator(void)
{
    LV_IMAGE_DECLARE(img_bar_bg);
    LV_IMAGE_DECLARE(img_bar_indicator);

    static lv_style_t style_slider_track;
    static lv_style_t style_slider_indicator;
    static lv_style_t style_slider_knob;

    static lv_subject_t subject_value2;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_slider_track);
        lv_style_set_bg_image_src(&style_slider_track, &img_bar_bg);
        lv_style_set_bg_opa(&style_slider_track, 0);
        lv_style_set_radius(&style_slider_track, 0);

        lv_style_init(&style_slider_indicator);
        lv_style_set_bg_image_src(&style_slider_indicator, &img_bar_indicator);
        lv_style_set_bg_opa(&style_slider_indicator, 0);
        lv_style_set_radius(&style_slider_indicator, 0);

        lv_style_init(&style_slider_knob);
        lv_style_set_bg_opa(&style_slider_knob, (255 * 100 / 100));
        lv_style_set_bg_color(&style_slider_knob, lv_color_hex(0x101820));
        lv_style_set_radius(&style_slider_knob, 4);
        lv_style_set_border_color(&style_slider_knob, lv_color_hex(0x00d4ff));
        lv_style_set_border_width(&style_slider_knob, 2);
        lv_style_set_pad_hor(&style_slider_knob, 0);
        lv_style_set_pad_ver(&style_slider_knob, 6);

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

    /* 💡 Drag the knob to reveal the segmented indicator image; cyan segments turn yellow then red near the maximum. */
    lv_obj_t * slider = lv_slider_create(screen);
    lv_obj_set_size(slider, 280, 30);
    lv_slider_set_min_value(slider, 0);
    lv_slider_set_max_value(slider, 20);
    lv_slider_bind_value(slider, &subject_value2);
    lv_obj_add_style(slider, &style_slider_track, LV_PART_MAIN);
    lv_obj_add_style(slider, &style_slider_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(slider, &style_slider_knob, LV_PART_KNOB);

    lv_obj_t * label = lv_label_create(screen);
    lv_label_bind_text(label, &subject_value2, "%d");
}
#endif
