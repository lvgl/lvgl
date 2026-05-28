/**
 * @file lv_example_style_image.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

#define ACCENT lv_color_hex(0x2196f3)

/**
 * @title Image recolor and rotation
 * @brief Tint and rotate an image purely through style properties.
 *
 * `image_recolor` with `image_recolor_opa="50%"` mixes a blue tint into the
 * bitmap without touching the source, and `transform_rotation="300"` turns
 * the widget 30° (the unit is 0.1°). The `radius`, `bg_color`, and `border_*`
 * on the same style transform together with the image.
 */
void lv_example_style_image(void)
{
    LV_IMAGE_DECLARE(img_example_lvgl_logo);

    static lv_style_t style_img;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_img);
        lv_style_set_radius(&style_img, 5);
        lv_style_set_bg_opa(&style_img, (255 * 100 / 100));
        lv_style_set_bg_color(&style_img, lv_color_hex(0xf5f5f5));
        lv_style_set_border_width(&style_img, 2);
        lv_style_set_border_color(&style_img, ACCENT);
        lv_style_set_image_recolor(&style_img, ACCENT);
        lv_style_set_image_recolor_opa(&style_img, (255 * 50 / 100));
        lv_style_set_transform_rotation(&style_img, 300);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 The bitmap is tinted and rotated by style alone; lower `image_recolor_opa` to fade the tint or change `transform_rotation`. */
    lv_obj_t * image = lv_image_create(screen);
    lv_image_set_src(image, &img_example_lvgl_logo);
    lv_obj_add_style(image, &style_img, 0);
}
#endif
