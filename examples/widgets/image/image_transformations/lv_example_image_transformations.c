/**
 * @file lv_example_image_transformations.c
 */

#include "../../../../lvgl.h"

/**
 * @title Image transformations
 * @brief Rotate and scale a single image source by setting widget attributes.
 *
 * `rotation` is in *tenths of a degree*, so `450` means 45°. `scale_x`/
 * `scale_y` use 100 = original (so `160` is 1.6×). `pivot_x`/`pivot_y` move
 * the rotation/scale anchor; defaults centre on the image. All three copies
 * share the same registered `img_example_lvgl_logo` source — the transforms happen
 * at render time, no extra image variants needed.
 */
void lv_example_image_transformations_create(void)
{
    LV_IMAGE_DECLARE(img_example_lvgl_logo);

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(screen, 16, 0);

    /* 💡 Edit `rotation`, the `scale_*` pair, or `pivot_*` to compare against the unmodified copy on the left. */
    lv_obj_t * lv_image_0 = lv_image_create(screen);
    lv_image_set_src(lv_image_0, &img_example_lvgl_logo);

    lv_obj_t * lv_image_1 = lv_image_create(screen);
    lv_image_set_src(lv_image_1, &img_example_lvgl_logo);
    lv_image_set_rotation(lv_image_1, 450);

    lv_obj_t * lv_image_2 = lv_image_create(screen);
    lv_image_set_src(lv_image_2, &img_example_lvgl_logo);
    lv_image_set_scale_x(lv_image_2, 160);
    lv_image_set_scale_y(lv_image_2, 160);
}

