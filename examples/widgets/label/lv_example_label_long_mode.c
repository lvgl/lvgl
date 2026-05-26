/**
 * @file lv_example_label_long_mode.c
 */

#include "../../lv_examples.h"
#if LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * @title Label long modes
 * @brief Show every long_mode behavior side by side.
 *
 * Five labels share the same width but use different long_mode values: wrap, dots, scroll,
 * scroll_circular, and clip. Each label is colored differently and contains text that does
 * not fit on a single line, so the mode-specific behavior is immediately visible.
 */
void lv_example_label_long_mode(void)
{
    static lv_style_t style_label_bg;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_label_bg);
        lv_style_set_bg_opa(&style_label_bg, (255 * 100 / 100));
        lv_style_set_radius(&style_label_bg, 6);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Wrap mode example */
    lv_obj_t * label_1 = lv_label_create(screen);
    lv_obj_set_width(label_1, 170);
    lv_label_set_long_mode(label_1, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_bg_color(label_1, lv_color_hex(0xd6eaf8), 0);
    lv_label_set_text(label_1, "Wrap keeps all text visible by splitting long lines into multiple rows.");
    lv_obj_add_style(label_1, &style_label_bg, 0);

    /* Dots mode example */
    lv_obj_t * label_2 = lv_label_create(screen);
    lv_obj_set_size(label_2, 170, 32);
    lv_label_set_long_mode(label_2, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_set_style_bg_color(label_2, lv_color_hex(0xd5f5e3), 0);
    lv_label_set_text(label_2, "Dots shortens overflowing text and replaces the tail with '...'");
    lv_obj_add_style(label_2, &style_label_bg, 0);

    /* Scroll mode example */
    lv_obj_t * label_3 = lv_label_create(screen);
    lv_obj_set_width(label_3, 170);
    lv_label_set_long_mode(label_3, LV_LABEL_LONG_MODE_SCROLL);
    lv_obj_set_style_bg_color(label_3, lv_color_hex(0xfdebd0), 0);
    lv_label_set_text(label_3, "Scroll moves overflowing text back and forth inside the label area.");
    lv_obj_add_style(label_3, &style_label_bg, 0);

    /* Circular scroll mode example */
    lv_obj_t * label_4 = lv_label_create(screen);
    lv_obj_set_width(label_4, 170);
    lv_label_set_long_mode(label_4, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_obj_set_style_bg_color(label_4, lv_color_hex(0xfadbd8), 0);
    lv_label_set_text(label_4, "Scroll circular continuously loops overflowing text forever.");
    lv_obj_add_style(label_4, &style_label_bg, 0);

    /* Clip mode example */
    lv_obj_t * label_5 = lv_label_create(screen);
    lv_obj_set_width(label_5, 170);
    lv_label_set_long_mode(label_5, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_set_style_bg_color(label_5, lv_color_hex(0xebdef0), 0);
    lv_label_set_text(label_5, "Clip simply cuts any part that falls outside the label width.");
    lv_obj_add_style(label_5, &style_label_bg, 0);
}
#endif
