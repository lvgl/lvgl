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
    static lv_style_t style_track;
    static lv_style_t style_progress;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_track);
        lv_style_set_arc_color(&style_track, lv_color_hex(0xe2e8f0));
        lv_style_set_arc_width(&style_track, 14);
        lv_style_set_arc_rounded(&style_track, true);

        lv_style_init(&style_progress);
        lv_style_set_arc_color(&style_progress, ACCENT);
        lv_style_set_arc_width(&style_progress, 14);
        lv_style_set_arc_rounded(&style_progress, true);

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
    lv_obj_add_style(arc, &style_track, LV_PART_MAIN);
    lv_obj_add_style(arc, &style_progress, LV_PART_INDICATOR);
    lv_obj_t * label = lv_label_create(arc);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "68%");
}
#endif
