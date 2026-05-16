#include "../../lv_examples.h"
#if LV_USE_SCALE && LV_BUILD_EXAMPLES

/**
 * @title Tilted major ticks on horizontal scale
 * @brief Horizontal scale whose major ticks rotate 45 degrees with a translate offset.
 *
 * A 200 by 100 horizontal-bottom scale carries 31 ticks (major every
 * fifth) over the range 10 to 40. The indicator part receives a
 * transform rotation of 450 (45.0 degrees), a 30 px tick length, and a
 * 5 px x-translate, leaning each major tick and its label away from
 * vertical. Minor ticks keep a 5 px length and straight orientation.
 */
void lv_example_scale_9(void)
{
    lv_obj_t * scale = lv_scale_create(lv_screen_active());
    lv_obj_set_size(scale, 200, 100);
    lv_scale_set_mode(scale, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_obj_center(scale);

    lv_scale_set_label_show(scale, true);
    lv_obj_set_style_transform_rotation(scale, 450, LV_PART_INDICATOR);
    lv_obj_set_style_length(scale, 30, LV_PART_INDICATOR);
    lv_obj_set_style_translate_x(scale, 5, LV_PART_INDICATOR);

    lv_scale_set_total_tick_count(scale, 31);
    lv_scale_set_major_tick_every(scale, 5);

    lv_obj_set_style_length(scale, 5, LV_PART_ITEMS);
    lv_obj_set_style_length(scale, 10, LV_PART_INDICATOR);
    lv_scale_set_range(scale, 10, 40);
}

#endif
