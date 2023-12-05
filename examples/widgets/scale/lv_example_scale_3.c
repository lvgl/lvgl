#include "../../lv_examples.h"
#if LV_USE_SCALE && LV_BUILD_EXAMPLES

lv_obj_t * scale;
lv_obj_t * needle_line;

static void set_needle_value(void * obj, int32_t v)
{
    lv_scale_set_line_needle_value(obj, needle_line, 60, v);
}

/**
 * A simple round scale
 */
void lv_example_scale_3(void)
{
    scale = lv_scale_create(lv_screen_active());

    lv_obj_set_size(scale, 150, 150);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    lv_obj_set_style_bg_opa(scale, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scale, lv_palette_lighten(LV_PALETTE_RED, 5), 0);
    lv_obj_set_style_radius(scale, LV_RADIUS_CIRCLE, 0);
    lv_obj_center(scale);

    lv_scale_set_label_show(scale, true);

    lv_scale_set_total_tick_count(scale, 31);
    lv_scale_set_major_tick_every(scale, 5);

    lv_scale_set_major_tick_length(scale, 10);
    lv_scale_set_minor_tick_length(scale, 5);
    lv_scale_set_range(scale, 10, 40);

    lv_scale_set_round_props(scale, 270, 135);

    needle_line = lv_line_create(scale);

    lv_obj_set_style_line_width(needle_line, 6, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(needle_line, true, LV_PART_MAIN);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, scale);
    lv_anim_set_exec_cb(&a, set_needle_value);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_values(&a, 10, 40);
    lv_anim_start(&a);
}

#endif
