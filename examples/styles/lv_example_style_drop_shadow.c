#include "../lv_examples.h"
#if LV_USE_ARC && LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * @title Drop shadow
 *
 * Unlike `shadow_*` (always a rectangle), `drop_shadow_*` blurs the actual
 * shape of the part. Added to `LV_PART_INDICATOR`, the active arc gets a
 * red shadow with `drop_shadow_radius = 16` and an offset of (5, 10) that
 * follows the curve. `drop_shadow_*` has no XML attribute yet, so this is C.
 */
void lv_example_style_drop_shadow(void)
{
    static lv_style_t style_drop;
    lv_style_init(&style_drop);
    lv_style_set_drop_shadow_color(&style_drop, lv_color_hex(0xf44336));
    lv_style_set_drop_shadow_radius(&style_drop, 16);
    lv_style_set_drop_shadow_opa(&style_drop, 255);
    lv_style_set_drop_shadow_offset_x(&style_drop, 5);
    lv_style_set_drop_shadow_offset_y(&style_drop, 10);

    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(scr, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(scr, 16, 0);

    /* 💡 The shadow follows the arc's curve, not a box; change `drop_shadow_offset_x`/`drop_shadow_offset_y` to slide it. */
    lv_obj_t * title = lv_label_create(scr);
    lv_obj_set_width(title, lv_pct(100));
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(title, "Style: drop shadow");

    lv_obj_t * arc = lv_arc_create(scr);
    lv_obj_set_size(arc, 150, 150);
    lv_arc_set_value(arc, 60);
    lv_obj_add_style(arc, &style_drop, LV_PART_INDICATOR);
}

#endif
