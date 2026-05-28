/**
 * @file lv_example_arc_pie_chart.c
 */

#include "../../lv_examples.h"
#if LV_USE_ARC && LV_BUILD_EXAMPLES

/**
 * @title Arc as pie-chart slices
 * @brief Stack five arcs at the same centre to render a coloured pie chart.
 *
 * Each slice is a separate `lv_arc` aligned at the same centre.
 * `bg_start_angle` and `bg_end_angle` carve out the wedge; the slice is
 * painted by `LV_PART_MAIN` (the bg track) — there's no need to set a value
 * just to make an indicator cover the bg, instead we hide the indicator
 * entirely with `arc_opa="0"` on the `indicator` selector. The MAIN style
 * sets `arc_width` to half the radius so the band reaches the centre, and
 * `arc_rounded="false"` flattens the end caps so adjacent slices share
 * their seam pixels exactly (rounded caps overshoot and leave gaps).
 *
 * The three shared parts (thick flat-ended main, invisible indicator,
 * invisible knob) are factored into named styles. Because position and size
 * are style properties too, the `style_pie_main` style also pins
 * `width`/`height`/`align` — so each `<lv_arc>` only carries its slice
 * angles and its colour.
 */
void lv_example_arc_pie_chart(void)
{
    static lv_style_t style_pie_main;
    static lv_style_t style_pie_indicator;
    static lv_style_t style_pie_knob;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_pie_main);
        lv_style_set_width(&style_pie_main, 200);
        lv_style_set_height(&style_pie_main, 200);
        lv_style_set_align(&style_pie_main, LV_ALIGN_CENTER);
        lv_style_set_arc_width(&style_pie_main, 100);
        lv_style_set_arc_rounded(&style_pie_main, false);

        lv_style_init(&style_pie_indicator);
        lv_style_set_arc_opa(&style_pie_indicator, 0);

        lv_style_init(&style_pie_knob);
        lv_style_set_bg_opa(&style_pie_knob, 0);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();

    /* 5 slices with uneven sweeps so the colours look like a real data breakdown.
       Angles measured clockwise from 3 o'clock; the seam between E and A is at 12 o'clock (270°). */
    lv_obj_t * arc_1 = lv_arc_create(screen);
    lv_obj_set_flag(arc_1, LV_OBJ_FLAG_CLICKABLE, false);
    lv_arc_set_bg_start_angle(arc_1, 270);
    lv_arc_set_bg_end_angle(arc_1, 335);
    lv_obj_set_style_arc_color(arc_1, lv_color_hex(0xef4444), LV_PART_MAIN);
    lv_obj_add_style(arc_1, &style_pie_main, LV_PART_MAIN);
    lv_obj_add_style(arc_1, &style_pie_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(arc_1, &style_pie_knob, LV_PART_KNOB);

    lv_obj_t * arc_2 = lv_arc_create(screen);
    lv_obj_set_flag(arc_2, LV_OBJ_FLAG_CLICKABLE, false);
    lv_arc_set_bg_start_angle(arc_2, 335);
    lv_arc_set_bg_end_angle(arc_2, 38);
    lv_obj_set_style_arc_color(arc_2, lv_color_hex(0xf59e0b), LV_PART_MAIN);
    lv_obj_add_style(arc_2, &style_pie_main, LV_PART_MAIN);
    lv_obj_add_style(arc_2, &style_pie_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(arc_2, &style_pie_knob, LV_PART_KNOB);

    lv_obj_t * arc_3 = lv_arc_create(screen);
    lv_obj_set_flag(arc_3, LV_OBJ_FLAG_CLICKABLE, false);
    lv_arc_set_bg_start_angle(arc_3, 38);
    lv_arc_set_bg_end_angle(arc_3, 142);
    lv_obj_set_style_arc_color(arc_3, lv_color_hex(0x22c55e), LV_PART_MAIN);
    lv_obj_add_style(arc_3, &style_pie_main, LV_PART_MAIN);
    lv_obj_add_style(arc_3, &style_pie_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(arc_3, &style_pie_knob, LV_PART_KNOB);

    lv_obj_t * arc_4 = lv_arc_create(screen);
    lv_obj_set_flag(arc_4, LV_OBJ_FLAG_CLICKABLE, false);
    lv_arc_set_bg_start_angle(arc_4, 142);
    lv_arc_set_bg_end_angle(arc_4, 205);
    lv_obj_set_style_arc_color(arc_4, lv_color_hex(0x3b82f6), LV_PART_MAIN);
    lv_obj_add_style(arc_4, &style_pie_main, LV_PART_MAIN);
    lv_obj_add_style(arc_4, &style_pie_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(arc_4, &style_pie_knob, LV_PART_KNOB);

    lv_obj_t * arc_5 = lv_arc_create(screen);
    lv_obj_set_flag(arc_5, LV_OBJ_FLAG_CLICKABLE, false);
    lv_arc_set_bg_start_angle(arc_5, 205);
    lv_arc_set_bg_end_angle(arc_5, 270);
    lv_obj_set_style_arc_color(arc_5, lv_color_hex(0xa855f7), LV_PART_MAIN);
    lv_obj_add_style(arc_5, &style_pie_main, LV_PART_MAIN);
    lv_obj_add_style(arc_5, &style_pie_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(arc_5, &style_pie_knob, LV_PART_KNOB);
}
#endif
