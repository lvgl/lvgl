/**
 * @file lv_example_arc_img_indicator.c
 */

#include "../../lv_examples.h"
#if LV_USE_ARC && LV_BUILD_EXAMPLES

/**
 * @title Arc image indicator
 * @brief Use bitmaps as the arc background and indicator — the arc shape is cut from each image.
 *
 * `arc_image_src` replaces the solid `arc_color` with pixels sampled from a bitmap,
 * centred on the arc's centre point. Here the MAIN part uses a dark segmented-ring
 * bitmap so the background track shows individual tiles, and the INDICATOR part uses
 * a glowing-cyan version of the same ring so the filled portion lights up. Both images
 * are 224×224 and match the 180×180 widget with `arc_width="23"`.
 */
void lv_example_arc_img_indicator(void)
{
    LV_IMAGE_DECLARE(img_arc_bg);
    LV_IMAGE_DECLARE(img_arc_indicator);

    static lv_style_t style_arc_bg;
    static lv_style_t style_arc_indicator;
    static lv_style_t style_arc_knob;

    static lv_subject_t subject_value2;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_arc_bg);
        lv_style_set_arc_image_src(&style_arc_bg, &img_arc_bg);
        lv_style_set_arc_width(&style_arc_bg, 40);
        lv_style_set_arc_rounded(&style_arc_bg, false);

        lv_style_init(&style_arc_indicator);
        lv_style_set_arc_image_src(&style_arc_indicator, &img_arc_indicator);
        lv_style_set_arc_width(&style_arc_indicator, 40);
        lv_style_set_arc_rounded(&style_arc_indicator, false);

        lv_style_init(&style_arc_knob);
        lv_style_set_opa(&style_arc_knob, 0);

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
    lv_obj_set_style_pad_row(screen, 0, 0);

    /* 💡 Change value to sweep the glowing cyan indicator across the dark segmented background. */
    lv_obj_t * arc = lv_arc_create(screen);
    lv_obj_set_size(arc, 200, 200);
    lv_arc_set_min_value(arc, 0);
    lv_arc_set_max_value(arc, 22);
    lv_arc_set_bg_start_angle(arc, 138);
    lv_arc_set_bg_end_angle(arc, 41);
    lv_arc_bind_value(arc, &subject_value2);
    lv_obj_add_style(arc, &style_arc_bg, LV_PART_MAIN);
    lv_obj_add_style(arc, &style_arc_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(arc, &style_arc_knob, LV_PART_KNOB);
    lv_obj_t * label = lv_label_create(arc);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_bind_text(label, &subject_value2, NULL);
}
#endif
