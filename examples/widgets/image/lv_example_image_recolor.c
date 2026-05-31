/**
 * @file lv_example_image_recolor.c
 */

#include "../../lv_examples.h"
#if LV_USE_IMAGE && LV_BUILD_EXAMPLES

/**
 * @title Image recolor
 * @brief Mix a tint into an image; a slider drives the recolor intensity live via `bind_style_prop`.
 *
 * The image's MAIN part exposes `image_recolor` (the colour to mix in) and
 * `image_recolor_opa` (the intensity, 0..255). Here `image_recolor` is set
 * statically and `image_recolor_opa` is bound to `subject_opa` through
 * `<bind_style_prop>`. Dragging the slider — which is also bound to
 * `subject_opa` — rewrites the subject and the binding pushes the new
 * intensity into the style, so the image retints in real time without any
 * event callback.
 */
void lv_example_image_recolor(void)
{
    LV_IMAGE_DECLARE(img_example_lvgl_logo);

    static lv_subject_t subject_opa;

    static bool inited = false;

    if(!inited) {
        lv_subject_init_int(&subject_opa, 128);
        lv_subject_set_min_value_int(&subject_opa, 0);
        lv_subject_set_max_value_int(&subject_opa, 255);
        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Drag the slider; the logo's tint fades in and out as `subject_opa` (0..255) moves. */
    lv_obj_t * image = lv_image_create(screen);
    lv_image_set_src(image, &img_example_lvgl_logo);
    lv_obj_set_style_image_recolor(image, lv_color_hex(0xef4444), LV_PART_MAIN);
    lv_obj_bind_style_prop(image, LV_STYLE_IMAGE_RECOLOR_OPA, LV_PART_MAIN, &subject_opa);

    lv_obj_t * slider = lv_slider_create(screen);
    lv_obj_set_width(slider, 240);
    lv_slider_set_min_value(slider, 0);
    lv_slider_set_max_value(slider, 255);
    lv_slider_bind_value(slider, &subject_opa);

    lv_obj_t * label = lv_label_create(screen);
    lv_label_bind_text(label, &subject_opa, "recolor_opa = %d / 255");
}
#endif
