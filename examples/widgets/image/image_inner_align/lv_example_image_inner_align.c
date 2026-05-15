/**
 * @file lv_example_image_inner_align.c
 */

#include "../../../../lvgl.h"

/**
 * @title Image inner align
 * @brief When the widget is larger than the image, `inner_align` chooses where it sits.
 *
 * Each image widget here has a fixed 140x100 size while `img_cogwheel` is
 * smaller, so the source occupies a fraction of the widget. `inner_align`
 * picks how the image is positioned inside the widget box — `top_left`,
 * `center`, `bottom_right` for fixed placement; `contain`, `cover`, `stretch`,
 * `tile` for fit modes. A coloured background makes the unused area visible.
 */
void lv_example_image_inner_align_create(void)
{
    LV_IMAGE_DECLARE(img_render_lvgl_logo_argb8888);

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(screen, 16, 0);

    /* 💡 Switch `inner_align` to `contain`, `cover`, or `stretch` to compare placement vs scaling modes. */
    lv_obj_t * lv_image_0 = lv_image_create(screen);
    lv_image_set_src(lv_image_0, &img_render_lvgl_logo_argb8888);
    lv_obj_set_size(lv_image_0, 140, 100);
    lv_image_set_inner_align(lv_image_0, LV_IMAGE_ALIGN_TOP_LEFT);
    lv_obj_set_style_bg_color(lv_image_0, lv_color_hex(0xe5e7eb), 0);
    lv_obj_set_style_bg_opa(lv_image_0, (255 * 100 / 100), 0);

    lv_obj_t * lv_image_1 = lv_image_create(screen);
    lv_image_set_src(lv_image_1, &img_render_lvgl_logo_argb8888);
    lv_obj_set_size(lv_image_1, 140, 100);
    lv_image_set_inner_align(lv_image_1, LV_IMAGE_ALIGN_CENTER);
    lv_obj_set_style_bg_color(lv_image_1, lv_color_hex(0xe5e7eb), 0);
    lv_obj_set_style_bg_opa(lv_image_1, (255 * 100 / 100), 0);

    lv_obj_t * lv_image_2 = lv_image_create(screen);
    lv_image_set_src(lv_image_2, &img_render_lvgl_logo_argb8888);
    lv_obj_set_size(lv_image_2, 140, 100);
    lv_image_set_inner_align(lv_image_2, LV_IMAGE_ALIGN_BOTTOM_RIGHT);
    lv_obj_set_style_bg_color(lv_image_2, lv_color_hex(0xe5e7eb), 0);
    lv_obj_set_style_bg_opa(lv_image_2, (255 * 100 / 100), 0);
}

