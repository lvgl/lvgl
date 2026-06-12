/**
 * @file lv_example_slider_styling.c
 */

#include "../../lv_examples.h"
#if LV_USE_SLIDER && LV_BUILD_EXAMPLES

#define TEXT_COLOR lv_color_hex(0x013992)

/**
 * @title Slider styling
 * @brief Style MAIN, INDICATOR, and KNOB — including a pressed-state knob highlight.
 *
 * Slider has three parts: MAIN (track), INDICATOR (filled portion), and KNOB. The
 * example targets each with a named style. The knob also gets a second style with
 * `selector="pressed|knob"` so it picks up an outline halo and a brighter fill only
 * while being dragged, demonstrating combined part+state selectors.
 */
void lv_example_slider_styling(void)
{
    static lv_style_t style_slider_track;
    static lv_style_t style_slider_indicator;
    static lv_style_t style_slider_knob;
    static lv_style_t style_slider_knob_pressed;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_slider_track);
        lv_style_set_bg_opa(&style_slider_track, (255 * 100 / 100));
        lv_style_set_bg_color(&style_slider_track, lv_color_hex(0x1f2937));
        lv_style_set_radius(&style_slider_track, 12);
        lv_style_set_border_color(&style_slider_track, lv_color_hex(0x0f172a));
        lv_style_set_border_width(&style_slider_track, 1);

        lv_style_init(&style_slider_indicator);
        lv_style_set_bg_opa(&style_slider_indicator, (255 * 100 / 100));
        lv_style_set_bg_color(&style_slider_indicator, lv_color_hex(0x8b5cf6));
        lv_style_set_bg_grad_color(&style_slider_indicator, lv_color_hex(0xec4899));
        lv_style_set_bg_grad_dir(&style_slider_indicator, LV_GRAD_DIR_HOR);
        lv_style_set_radius(&style_slider_indicator, 12);
        lv_style_set_shadow_color(&style_slider_indicator, lv_color_hex(0x6d28d9));
        lv_style_set_shadow_width(&style_slider_indicator, 12);
        lv_style_set_shadow_spread(&style_slider_indicator, -1);
        lv_style_set_shadow_opa(&style_slider_indicator, 150);

        lv_style_init(&style_slider_knob);
        lv_style_set_bg_opa(&style_slider_knob, (255 * 100 / 100));
        lv_style_set_bg_color(&style_slider_knob, lv_color_hex(0xffffff));
        lv_style_set_radius(&style_slider_knob, 100);
        lv_style_set_border_color(&style_slider_knob, lv_color_hex(0x8b5cf6));
        lv_style_set_border_width(&style_slider_knob, 3);
        lv_style_set_pad_all(&style_slider_knob, 6);
        lv_style_set_shadow_color(&style_slider_knob, lv_color_hex(0x000000));
        lv_style_set_shadow_width(&style_slider_knob, 8);
        lv_style_set_shadow_offset_y(&style_slider_knob, 2);
        lv_style_set_shadow_opa(&style_slider_knob, 80);

        lv_style_init(&style_slider_knob_pressed);
        lv_style_set_bg_color(&style_slider_knob_pressed, lv_color_hex(0xfff7ed));
        lv_style_set_outline_color(&style_slider_knob_pressed, lv_color_hex(0xec4899));
        lv_style_set_outline_width(&style_slider_knob_pressed, 6);
        lv_style_set_outline_pad(&style_slider_knob_pressed, 4);
        lv_style_set_outline_opa(&style_slider_knob_pressed, 120);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 20, 0);

    /* 💡 Drag the knob slowly to see the knob-pressed style kick in. */
    lv_obj_t * slider = lv_slider_create(screen);
    lv_obj_set_size(slider, lv_pct(90), 22);
    lv_slider_set_min_value(slider, 0);
    lv_slider_set_max_value(slider, 100);
    lv_slider_set_value(slider, 55, false);
    lv_obj_add_style(slider, &style_slider_track, LV_PART_MAIN);
    lv_obj_add_style(slider, &style_slider_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(slider, &style_slider_knob, LV_PART_KNOB);
    lv_obj_add_style(slider, &style_slider_knob_pressed, LV_PART_KNOB | LV_STATE_PRESSED);
}
#endif
