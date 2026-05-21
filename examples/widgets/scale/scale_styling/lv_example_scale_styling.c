/**
 * @file lv_example_scale_styling.c
 */

#include "../../../lv_examples.h"
#if LV_USE_SCALE && LV_BUILD_EXAMPLES

/**
 * @title Scale styling
 * @brief Style the main rail, minor ticks, and major ticks (with labels) independently.
 *
 * A scale has three drawable parts: `MAIN` (the rail line/arc), `ITEMS`
 * (every minor tick) and `INDICATOR` (every major tick *plus* its label).
 * Each is targeted by `selector="main"`, `selector="items"`, and
 * `selector="indicator"`. Line-family properties (`line_color`/`line_width`/
 * `line_opa`) drive ticks and rail; text properties on `INDICATOR` style the
 * numeric labels. Keep the indicator line thicker than items to make
 * major/minor visually distinguishable.
 *
 * The major-tick labels can also be rotated by the indicator's
 * `transform_rotation` style — `style_transform_rotation-indicator="300"`
 * (units of 0.1°, so 300 = 30°) tilts every label by 30°. Use this when
 * dense labels would otherwise overlap.
 */
void lv_example_scale_styling(void)
{
    static lv_style_t style_scale_main;
    static lv_style_t style_scale_items;
    static lv_style_t style_scale_indicator;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_scale_main);
        lv_style_set_line_color(&style_scale_main, lv_color_hex(0x6b7280));
        lv_style_set_line_width(&style_scale_main, 2);

        lv_style_init(&style_scale_items);
        lv_style_set_line_color(&style_scale_items, lv_color_hex(0x9ca3af));
        lv_style_set_line_width(&style_scale_items, 1);

        lv_style_init(&style_scale_indicator);
        lv_style_set_line_color(&style_scale_indicator, lv_color_hex(0x6366f1));
        lv_style_set_line_width(&style_scale_indicator, 3);
        lv_style_set_text_color(&style_scale_indicator, lv_color_hex(0x6366f1));

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Recolour the three named styles to see exactly which part each selector targets; bump `transform_rotation-indicator` to tilt labels further. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Scale: styling parts");

    lv_obj_t * lv_scale_0 = lv_scale_create(screen);
    lv_obj_set_size(lv_scale_0, 280, 80);
    lv_scale_set_mode(lv_scale_0, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_total_tick_count(lv_scale_0, 21);
    lv_scale_set_major_tick_every(lv_scale_0, 4);
    lv_scale_set_label_show(lv_scale_0, true);
    lv_scale_set_min_value(lv_scale_0, 0);
    lv_scale_set_max_value(lv_scale_0, 100);
    lv_obj_set_style_transform_rotation(lv_scale_0, 300, LV_PART_INDICATOR);
    lv_obj_add_style(lv_scale_0, &style_scale_main, LV_PART_MAIN);
    lv_obj_add_style(lv_scale_0, &style_scale_items, LV_PART_ITEMS);
    lv_obj_add_style(lv_scale_0, &style_scale_indicator, LV_PART_INDICATOR);
}
#endif
