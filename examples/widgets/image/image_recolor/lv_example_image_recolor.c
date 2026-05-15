/**
 * @file lv_example_image_recolor.c
 */

#include "../../../../lvgl.h"

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
void lv_example_image_recolor_create(void)
{
    LV_IMAGE_DECLARE(img_render_lvgl_logo_argb8888);

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
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Drag the slider; the cogwheel's tint fades in and out as `subject_opa` (0..255) moves. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Image: recolor");

    lv_obj_t * lv_image_0 = lv_image_create(screen);
    lv_image_set_src(lv_image_0, &img_render_lvgl_logo_argb8888);
    lv_obj_set_style_image_recolor(lv_image_0, lv_color_hex(0xef4444), LV_PART_MAIN);
    lv_obj_bind_style_prop(lv_image_0, LV_STYLE_IMAGE_RECOLOR_OPA, LV_PART_MAIN, &subject_opa);

    lv_obj_t * lv_slider_0 = lv_slider_create(screen);
    lv_obj_set_width(lv_slider_0, 240);
    lv_slider_set_min_value(lv_slider_0, 0);
    lv_slider_set_max_value(lv_slider_0, 255);
    lv_slider_bind_value(lv_slider_0, &subject_opa);

    lv_obj_t * lv_label_1 = lv_label_create(screen);
    lv_label_bind_text(lv_label_1, &subject_opa, "recolor_opa = %d / 255");
}

