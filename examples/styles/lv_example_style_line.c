#include "../lv_examples.h"
#if LV_USE_LINE && LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * @title Line stroke
 *
 * `line_color` and `line_width` set the stroke and `line_rounded` rounds
 * the end caps and vertices so the bends look smooth instead of mitred.
 * The point array is fixed; only the stroke style changes the look. Line
 * points are a C `lv_point_precise_t` array, which is why this is C.
 */
void lv_example_style_line(void)
{
    static lv_style_t style_stroke;
    lv_style_init(&style_stroke);
    lv_style_set_line_color(&style_stroke, lv_color_hex(0x9e9e9e));
    lv_style_set_line_width(&style_stroke, 6);
    lv_style_set_line_rounded(&style_stroke, true);

    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(scr, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(scr, 16, 0);

    /* 💡 Raise `line_width` or toggle `line_rounded`; the stroke thickens and the bends round off instead of mitring. */
    lv_obj_t * title = lv_label_create(scr);
    lv_obj_set_width(title, lv_pct(100));
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(title, "Style: line stroke");

    static lv_point_precise_t pts[] = {{10, 30}, {60, 90}, {130, 20}, {200, 70}};
    lv_obj_t * line = lv_line_create(scr);
    lv_line_set_points(line, pts, sizeof(pts) / sizeof(pts[0]));
    lv_obj_add_style(line, &style_stroke, 0);
}

#endif
