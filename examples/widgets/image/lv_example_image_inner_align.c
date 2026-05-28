/**
 * @file lv_example_image_inner_align.c
 */

#include "../../lv_examples.h"
#if LV_USE_IMAGE && LV_BUILD_EXAMPLES

/**
 * @title Image inner align
 * @brief When the widget is larger than the image, `inner_align` chooses where it sits.
 *
 * Each image widget here has a fixed 140x100 size while `img_example_lvgl_logo` is
 * smaller, so the source occupies a fraction of the widget. `inner_align`
 * picks how the image is positioned inside the widget box — `top_left`,
 * `center`, `bottom_right` for fixed placement; `contain`, `cover`, `stretch`,
 * `tile` for fit modes. A coloured background makes the unused area visible.
 */
void lv_example_image_inner_align(void)
{
    LV_IMAGE_DECLARE(img_example_lvgl_logo);

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(screen, 16, 0);

    /* 💡 Switch `inner_align` to `contain`, `cover`, or `stretch` to compare placement vs scaling modes. */
    lv_obj_t * image_1 = lv_image_create(screen);
    lv_image_set_src(image_1, &img_example_lvgl_logo);
    lv_obj_set_size(image_1, 80, 80);
    lv_image_set_inner_align(image_1, LV_IMAGE_ALIGN_TOP_LEFT);
    lv_obj_set_style_bg_color(image_1, lv_color_hex(0xe5e7eb), 0);
    lv_obj_set_style_bg_opa(image_1, (255 * 100 / 100), 0);

    lv_obj_t * image_2 = lv_image_create(screen);
    lv_image_set_src(image_2, &img_example_lvgl_logo);
    lv_obj_set_size(image_2, 80, 80);
    lv_image_set_inner_align(image_2, LV_IMAGE_ALIGN_CENTER);
    lv_obj_set_style_bg_color(image_2, lv_color_hex(0xe5e7eb), 0);
    lv_obj_set_style_bg_opa(image_2, (255 * 100 / 100), 0);

    lv_obj_t * image_3 = lv_image_create(screen);
    lv_image_set_src(image_3, &img_example_lvgl_logo);
    lv_obj_set_size(image_3, 80, 80);
    lv_image_set_inner_align(image_3, LV_IMAGE_ALIGN_BOTTOM_RIGHT);
    lv_obj_set_style_bg_color(image_3, lv_color_hex(0xe5e7eb), 0);
    lv_obj_set_style_bg_opa(image_3, (255 * 100 / 100), 0);
}
#endif
