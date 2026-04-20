#include "../../lv_examples.h"
#if LV_USE_BAR && LV_BUILD_EXAMPLES

/**
 * @title Ranged bar with tiled stripe pattern
 * @brief A 260 x 20 bar filled between 20 and 90 with a tiled skewed-stripe overlay.
 *
 * `LV_IMAGE_DECLARE(img_skew_strip)` provides the stripe asset, and an
 * `LV_PART_INDICATOR` style sets it as a tiled `bg_image_src` at 30% opacity.
 * `lv_bar_set_mode(bar, LV_BAR_MODE_RANGE)` unlocks a configurable start
 * value; `lv_bar_set_start_value` and `lv_bar_set_value` fill only the 20
 * to 90 portion of the track with the stripes on top of the theme color.
 */
void lv_example_bar_4(void)
{
    LV_IMAGE_DECLARE(img_skew_strip);
    static lv_style_t style_indic;

    lv_style_init(&style_indic);
    lv_style_set_bg_image_src(&style_indic, &img_skew_strip);
    lv_style_set_bg_image_tiled(&style_indic, true);
    lv_style_set_bg_image_opa(&style_indic, LV_OPA_30);

    lv_obj_t * bar = lv_bar_create(lv_screen_active());
    lv_obj_add_style(bar, &style_indic, LV_PART_INDICATOR);

    lv_obj_set_size(bar, 260, 20);
    lv_obj_center(bar);
    lv_bar_set_mode(bar, LV_BAR_MODE_RANGE);
    lv_bar_set_value(bar, 90, LV_ANIM_OFF);
    lv_bar_set_start_value(bar, 20, LV_ANIM_OFF);
}

#endif
