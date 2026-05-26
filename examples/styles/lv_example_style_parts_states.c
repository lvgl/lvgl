/**
 * @file lv_example_style_parts_states.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Parts and states
 * @brief Style one part normally and again for a state with `selector`.
 *
 * `selector="indicator"` gives the slider's filled part a violet→pink
 * gradient. `selector="pressed|indicator"` adds a glow that applies only
 * while the indicator is pressed, so the same part carries one base look
 * plus a state-specific overlay — the core of LVGL's part/state model.
 */
void lv_example_style_parts_states(void)
{
    static lv_style_t style_indicator;
    static lv_style_t style_indicator_pressed;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_indicator);
        lv_style_set_bg_opa(&style_indicator, (255 * 100 / 100));
        lv_style_set_bg_color(&style_indicator, lv_color_hex(0x8b5cf6));
        lv_style_set_bg_grad_color(&style_indicator, lv_color_hex(0xec4899));
        lv_style_set_bg_grad_dir(&style_indicator, LV_GRAD_DIR_HOR);
        lv_style_set_radius(&style_indicator, 10);

        lv_style_init(&style_indicator_pressed);
        lv_style_set_shadow_color(&style_indicator_pressed, lv_color_hex(0xec4899));
        lv_style_set_shadow_width(&style_indicator_pressed, 18);
        lv_style_set_shadow_spread(&style_indicator_pressed, 2);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 20, 0);

    /* 💡 Press and hold the slider: the indicator keeps its gradient but gains the pressed glow. */
    lv_obj_t * slider = lv_slider_create(screen);
    lv_obj_set_size(slider, 220, 16);
    lv_slider_set_min_value(slider, 0);
    lv_slider_set_max_value(slider, 100);
    lv_slider_set_value(slider, 65, false);
    lv_obj_add_style(slider, &style_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(slider, &style_indicator_pressed, LV_PART_INDICATOR | LV_STATE_PRESSED);
}
#endif
