/**
 * @file lv_example_bar_styling.c
 */

#include "../../lv_examples.h"
#if LV_USE_BAR && LV_BUILD_EXAMPLES

/**
 * @title Bar styling
 * @brief Style the MAIN track and the INDICATOR independently, with a gradient fill.
 *
 * Bar exposes two parts: MAIN (the background track) and INDICATOR (the filled portion).
 * Each is styled separately via `selector="main"` and `selector="indicator"`. The
 * indicator style adds a horizontal gradient and a darker inner shadow so the fill
 * reads as a glossy progress strip on top of the recessed track.
 */
void lv_example_bar_styling(void)
{
    static lv_style_t style_bar_track;
    static lv_style_t style_bar_indicator;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_bar_track);
        lv_style_set_bg_opa(&style_bar_track, (255 * 100 / 100));
        lv_style_set_bg_color(&style_bar_track, lv_color_hex(0x1f2937));
        lv_style_set_radius(&style_bar_track, 12);
        lv_style_set_border_color(&style_bar_track, lv_color_hex(0x0f172a));
        lv_style_set_border_width(&style_bar_track, 1);

        lv_style_init(&style_bar_indicator);
        lv_style_set_bg_opa(&style_bar_indicator, (255 * 100 / 100));
        lv_style_set_bg_color(&style_bar_indicator, lv_color_hex(0x10b981));
        lv_style_set_bg_grad_color(&style_bar_indicator, lv_color_hex(0x06b6d4));
        lv_style_set_bg_grad_dir(&style_bar_indicator, LV_GRAD_DIR_HOR);
        lv_style_set_radius(&style_bar_indicator, 12);
        lv_style_set_shadow_color(&style_bar_indicator, lv_color_hex(0x064e3b));
        lv_style_set_shadow_width(&style_bar_indicator, 8);
        lv_style_set_shadow_spread(&style_bar_indicator, -2);
        lv_style_set_shadow_opa(&style_bar_indicator, 180);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 20, 0);

    /* Tall pill-shaped bar with named styles for both parts */
    lv_obj_t * bar_1 = lv_bar_create(screen);
    lv_obj_set_size(bar_1, lv_pct(90), 22);
    lv_bar_set_min_value(bar_1, 0);
    lv_bar_set_max_value(bar_1, 100);
    lv_bar_set_value(bar_1, 65, false);
    lv_obj_add_style(bar_1, &style_bar_track, LV_PART_MAIN);
    lv_obj_add_style(bar_1, &style_bar_indicator, LV_PART_INDICATOR);

    /* Same widget, local style_* overrides for a flat alternative look */
    lv_obj_t * bar_2 = lv_bar_create(screen);
    lv_obj_set_size(bar_2, lv_pct(90), 10);
    lv_bar_set_min_value(bar_2, 0);
    lv_bar_set_max_value(bar_2, 100);
    lv_bar_set_value(bar_2, 35, false);
    lv_obj_set_style_bg_color(bar_2, lv_color_hex(0xe5e7eb), 0);
    lv_obj_set_style_bg_opa(bar_2, (255 * 100 / 100), 0);
    lv_obj_set_style_radius(bar_2, 2, 0);
    lv_obj_set_style_border_color(bar_2, lv_color_hex(0xd1d5db), 0);
    lv_obj_set_style_border_width(bar_2, 1, 0);
}
#endif
