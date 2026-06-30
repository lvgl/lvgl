/**
 * @file lv_example_span_styling.c
 */

#include "../../lv_examples.h"
#if LV_USE_SPAN && LV_BUILD_EXAMPLES

/**
 * @title Spangroup styled text
 * @brief Mix per-span color and decoration in one wrapping rich-text block.
 *
 * Five spans share one group but each carries its own named style: a colored
 * title, plain body, a red accent, an underlined run, and a struck-through
 * run. The group has a fixed width with `LV_SIZE_CONTENT` height so the text
 * wraps, `indent` pushes the first line in, and `overflow="ellipsis"` with
 * `max_lines` shows how the tail is truncated.
 */
void lv_example_span_styling(void)
{
    static lv_style_t style_span_title;
    static lv_style_t style_span_body;
    static lv_style_t style_span_accent;
    static lv_style_t style_span_underline;
    static lv_style_t style_span_strike;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_span_title);
        lv_style_set_text_color(&style_span_title, lv_color_hex(0x1d4ed8));
        lv_style_set_text_decor(&style_span_title, LV_TEXT_DECOR_UNDERLINE);

        lv_style_init(&style_span_body);
        lv_style_set_text_color(&style_span_body, lv_color_hex(0x111827));

        lv_style_init(&style_span_accent);
        lv_style_set_text_color(&style_span_accent, lv_color_hex(0xdc2626));

        lv_style_init(&style_span_underline);
        lv_style_set_text_color(&style_span_underline, lv_color_hex(0x059669));
        lv_style_set_text_decor(&style_span_underline, LV_TEXT_DECOR_UNDERLINE);

        lv_style_init(&style_span_strike);
        lv_style_set_text_color(&style_span_strike, lv_color_hex(0x6b7280));
        lv_style_set_text_decor(&style_span_strike, LV_TEXT_DECOR_STRIKETHROUGH);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Each span keeps its own style; the group only controls wrap, indent, and overflow. */
    lv_obj_t * spangroup = lv_spangroup_create(screen);
    lv_obj_set_size(spangroup, 260, LV_SIZE_CONTENT);
    lv_spangroup_set_indent(spangroup, 20);
    lv_spangroup_set_overflow(spangroup, LV_SPAN_OVERFLOW_ELLIPSIS);
    lv_spangroup_set_max_lines(spangroup, 4);
    lv_obj_set_style_border_color(spangroup, lv_color_hex(0xf59e0b), 0);
    lv_obj_set_style_border_width(spangroup, 1, 0);
    lv_obj_set_style_pad_all(spangroup, 6, 0);
    lv_span_t * lv_spangroup_span_0 = lv_spangroup_add_span(spangroup);
    lv_spangroup_set_span_text(spangroup, lv_spangroup_span_0, "LVGL ");
    lv_spangroup_set_span_style(spangroup, lv_spangroup_span_0, &style_span_title);
    lv_span_t * lv_spangroup_span_1 = lv_spangroup_add_span(spangroup);
    lv_spangroup_set_span_text(spangroup, lv_spangroup_span_1, "renders rich text ");
    lv_spangroup_set_span_style(spangroup, lv_spangroup_span_1, &style_span_body);
    lv_span_t * lv_spangroup_span_2 = lv_spangroup_add_span(spangroup);
    lv_spangroup_set_span_text(spangroup, lv_spangroup_span_2, "with per-span color, ");
    lv_spangroup_set_span_style(spangroup, lv_spangroup_span_2, &style_span_accent);
    lv_span_t * lv_spangroup_span_3 = lv_spangroup_add_span(spangroup);
    lv_spangroup_set_span_text(spangroup, lv_spangroup_span_3, "underline, ");
    lv_spangroup_set_span_style(spangroup, lv_spangroup_span_3, &style_span_underline);
    lv_span_t * lv_spangroup_span_4 = lv_spangroup_add_span(spangroup);
    lv_spangroup_set_span_text(spangroup, lv_spangroup_span_4, "and strikethrough decoration.");
    lv_spangroup_set_span_style(spangroup, lv_spangroup_span_4, &style_span_strike);
}
#endif
