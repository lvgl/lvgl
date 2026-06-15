/**
 * @file lv_example_style_arc.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

#define ACCENT lv_color_hex(0x6366f1)

/**
 * @title Arc stroke
 * @brief Style the arc's ring and indicator into a modern progress dial.
 *
 * `arc_width` and `arc_rounded="true"` give both parts a thick, round-capped
 * stroke. On `selector="main"` `arc_color` is a light neutral track; on
 * `selector="indicator"` it is the accent, so the same three properties
 * produce a clean circular-progress look.
 */
void lv_example_style_arc(void)
{
    static lv_style_t style_bg;
    static lv_style_t style_indicator;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_bg);
        lv_style_set_arc_color(&style_bg, ACCENT);
        lv_style_set_arc_width(&style_bg, 14);
        lv_style_set_arc_opa(&style_bg, (255 * 20 / 100));
        lv_style_set_arc_rounded(&style_bg, true);

        lv_style_init(&style_indicator);
        lv_style_set_arc_color(&style_indicator, ACCENT);
        lv_style_set_arc_width(&style_indicator, 14);
        lv_style_set_arc_rounded(&style_indicator, true);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 12, 0);

    /* 💡 Change `arc_width` or either `arc_color`; track and progress restyle independently. */
    lv_obj_t * arc = lv_arc_create(screen);
    lv_obj_set_size(arc, 160, 160);
    lv_arc_set_min_value(arc, 0);
    lv_arc_set_max_value(arc, 100);
    lv_arc_set_value(arc, 68);
    lv_obj_add_style(arc, &style_bg, LV_PART_MAIN);
    lv_obj_add_style(arc, &style_indicator, LV_PART_INDICATOR);
}
#endif
