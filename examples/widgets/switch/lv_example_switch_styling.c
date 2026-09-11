/**
 * @file lv_example_switch_styling.c
 */

#include "../../lv_examples.h"
#if LV_USE_SWITCH && LV_BUILD_EXAMPLES

/**
 * @title Switch styling
 * @brief Style the track (MAIN), the filled portion (INDICATOR), and the knob.
 *
 * A switch has three drawable parts: `MAIN` (the rounded track), `INDICATOR`
 * (the filled side when the switch is on), and `KNOB` (the circle that slides).
 * The on-state look is attached with `selector="indicator|checked"` so the
 * indicator only paints when the switch is checked.
 *
 * Padding plays a dual role here:
 *
 * * `pad_*` on MAIN shrinks the INDICATOR inwards (the indicator fills the
 * content area of MAIN, which padding makes smaller). Use this to leave a
 * visible track around the coloured fill — `pad_all="6"` gives a 6 px
 * ring of track that stays visible behind the indicator.
 * * `pad_*` on KNOB grows the knob *outside* the track. `pad_all="0"` makes
 * the knob exactly as tall as MAIN; positive padding makes it overhang.
 * `pad_all="2"` here lets the knob sit slightly larger than the track,
 * which (combined with a shadow) gives the typical floating-knob look.
 */
void lv_example_switch_styling(void)
{
    static lv_style_t style_switch_main;
    static lv_style_t style_switch_indicator_checked;
    static lv_style_t style_switch_knob;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_switch_main);
        lv_style_set_bg_color(&style_switch_main, lv_color_hex(0xc4d8cb));
        lv_style_set_bg_opa(&style_switch_main, (255 * 100 / 100));
        lv_style_set_radius(&style_switch_main, 999);
        lv_style_set_pad_all(&style_switch_main, 6);
        lv_style_set_border_width(&style_switch_main, 0);

        lv_style_init(&style_switch_indicator_checked);
        lv_style_set_bg_color(&style_switch_indicator_checked, lv_color_hex(0x22c55e));
        lv_style_set_bg_opa(&style_switch_indicator_checked, (255 * 100 / 100));

        lv_style_init(&style_switch_knob);
        lv_style_set_bg_color(&style_switch_knob, lv_color_hex(0xffffff));
        lv_style_set_bg_opa(&style_switch_knob, (255 * 100 / 100));
        lv_style_set_pad_all(&style_switch_knob, 2);
        lv_style_set_border_color(&style_switch_knob, lv_color_hex(0xd1d5db));
        lv_style_set_border_width(&style_switch_knob, 1);
        lv_style_set_shadow_color(&style_switch_knob, lv_color_hex(0x000000));
        lv_style_set_shadow_opa(&style_switch_knob, (255 * 40 / 100));
        lv_style_set_shadow_width(&style_switch_knob, 16);
        lv_style_set_shadow_offset_y(&style_switch_knob, 2);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Bump `pad_all` on `style_switch_main` to widen the track around the indicator; bump it on `style_switch_knob` to grow the knob past the track height. */
    lv_obj_t * sw_1 = lv_switch_create(screen);
    lv_obj_set_size(sw_1, 60, 30);
    lv_obj_add_style(sw_1, &style_switch_main, LV_PART_MAIN);
    lv_obj_add_style(sw_1, &style_switch_indicator_checked, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_style(sw_1, &style_switch_knob, LV_PART_KNOB);

    lv_obj_t * sw_2 = lv_switch_create(screen);
    lv_obj_set_size(sw_2, 60, 30);
    lv_obj_set_state(sw_2, LV_STATE_CHECKED, true);
    lv_obj_add_style(sw_2, &style_switch_main, LV_PART_MAIN);
    lv_obj_add_style(sw_2, &style_switch_indicator_checked, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_style(sw_2, &style_switch_knob, LV_PART_KNOB);
}
#endif
