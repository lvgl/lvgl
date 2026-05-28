/**
 * @file lv_example_scroll_scrollbar_style.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

#define ACCENT lv_color_hex(0x9429ff)

/**
 * @title Styling the scrollbar
 * @brief Restyle the SCROLLBAR part into a thick, rounded, coloured bar.
 *
 * The scrollbar is the `scrollbar` part of any scrollable Widget. A named
 * style attached with `selector="scrollbar"` overrides its `width`
 * (thickness), `radius`, fill colour/opacity and `pad_right` (gap from
 * the edge). `scrollbar_mode="on"` keeps it visible so the styling is
 * always seen.
 */
void lv_example_scroll_scrollbar_style(void)
{
    static lv_style_t style_scrollbar;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_scrollbar);
        lv_style_set_width(&style_scrollbar, 10);
        lv_style_set_radius(&style_scrollbar, 5);
        lv_style_set_bg_color(&style_scrollbar, ACCENT);
        lv_style_set_bg_opa(&style_scrollbar, (255 * 100 / 100));
        lv_style_set_pad_right(&style_scrollbar, 4);
        lv_style_set_pad_top(&style_scrollbar, 4);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 12, 0);

    /* 💡 Drag the panel — the wide blue rounded bar is the styled `scrollbar` part. */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_size(container, 240, 160);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(container, 8, 0);
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_ON);
    lv_obj_add_style(container, &style_scrollbar, LV_PART_SCROLLBAR);
    lv_obj_t * button_1 = lv_button_create(container);
    lv_obj_set_width(button_1, lv_pct(100));
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Row 1");

    lv_obj_t * button_2 = lv_button_create(container);
    lv_obj_set_width(button_2, lv_pct(100));
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Row 2");

    lv_obj_t * button_3 = lv_button_create(container);
    lv_obj_set_width(button_3, lv_pct(100));
    lv_obj_t * label_3 = lv_label_create(button_3);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "Row 3");

    lv_obj_t * button_4 = lv_button_create(container);
    lv_obj_set_width(button_4, lv_pct(100));
    lv_obj_t * label_4 = lv_label_create(button_4);
    lv_obj_set_align(label_4, LV_ALIGN_CENTER);
    lv_label_set_text(label_4, "Row 4");

    lv_obj_t * button_5 = lv_button_create(container);
    lv_obj_set_width(button_5, lv_pct(100));
    lv_obj_t * label_5 = lv_label_create(button_5);
    lv_obj_set_align(label_5, LV_ALIGN_CENTER);
    lv_label_set_text(label_5, "Row 5");

    lv_obj_t * button_6 = lv_button_create(container);
    lv_obj_set_width(button_6, lv_pct(100));
    lv_obj_t * label_6 = lv_label_create(button_6);
    lv_obj_set_align(label_6, LV_ALIGN_CENTER);
    lv_label_set_text(label_6, "Row 6");
}
#endif
