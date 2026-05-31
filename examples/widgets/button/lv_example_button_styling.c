/**
 * @file lv_example_button_styling.c
 */

#include "../../lv_examples.h"
#if LV_USE_BUTTON && LV_BUILD_EXAMPLES

/**
 * @title Button styling
 * @brief State-aware colors, rounded corners, shadow, and a pressed outline.
 *
 * A single named style sets the resting look (rounded corners, soft shadow, a base
 * color). Separate styles attached with `selector="pressed"` and `selector="checked"`
 * swap the background color, lift the shadow, and add an outline so the rendered
 * button changes appearance when the user interacts with it.
 */
void lv_example_button_styling(void)
{
    static lv_style_t style_button;
    static lv_style_t style_button_pressed;
    static lv_style_t style_button_checked;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_button);
        lv_style_set_bg_opa(&style_button, (255 * 100 / 100));
        lv_style_set_bg_color(&style_button, lv_color_hex(0x3b82f6));
        lv_style_set_radius(&style_button, 12);
        lv_style_set_shadow_color(&style_button, lv_color_hex(0x1e3a8a));
        lv_style_set_shadow_width(&style_button, 14);
        lv_style_set_shadow_offset_y(&style_button, 4);
        lv_style_set_shadow_opa(&style_button, 120);
        lv_style_set_text_color(&style_button, lv_color_hex(0xffffff));
        lv_style_set_pad_all(&style_button, 10);

        lv_style_init(&style_button_pressed);
        lv_style_set_bg_color(&style_button_pressed, lv_color_hex(0x1d4ed8));
        lv_style_set_shadow_offset_y(&style_button_pressed, 1);
        lv_style_set_shadow_width(&style_button_pressed, 6);
        lv_style_set_outline_color(&style_button_pressed, lv_color_hex(0x60a5fa));
        lv_style_set_outline_width(&style_button_pressed, 3);
        lv_style_set_outline_pad(&style_button_pressed, 2);
        lv_style_set_outline_opa(&style_button_pressed, (255 * 100 / 100));

        lv_style_init(&style_button_checked);
        lv_style_set_bg_color(&style_button_checked, lv_color_hex(0x16a34a));
        lv_style_set_shadow_color(&style_button_checked, lv_color_hex(0x14532d));
        lv_style_set_shadow_width(&style_button_checked, 20);
        lv_style_set_shadow_opa(&style_button_checked, 160);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 20, 0);

    /* Default button (resting style only) */
    lv_obj_t * button_1 = lv_button_create(screen);
    lv_obj_set_size(button_1, 180, 48);
    lv_obj_add_style(button_1, &style_button, 0);
    lv_obj_add_style(button_1, &style_button_pressed, LV_STATE_PRESSED);
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Press me");

    /* Checkable button: swaps to the checked style on toggle */
    lv_obj_t * button_2 = lv_button_create(screen);
    lv_obj_set_size(button_2, 180, 48);
    lv_obj_set_flag(button_2, LV_OBJ_FLAG_CHECKABLE, true);
    lv_obj_add_style(button_2, &style_button, 0);
    lv_obj_add_style(button_2, &style_button_pressed, LV_STATE_PRESSED);
    lv_obj_add_style(button_2, &style_button_checked, LV_STATE_CHECKED);
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Toggle me");

    /* Local style_* overrides: pure-CSS-style attribute styling, no named block needed */
    lv_obj_t * button_3 = lv_button_create(screen);
    lv_obj_set_size(button_3, 180, 48);
    lv_obj_set_style_bg_color(button_3, lv_color_hex(0xf59e0b), 0);
    lv_obj_set_style_bg_opa(button_3, (255 * 100 / 100), 0);
    lv_obj_set_style_radius(button_3, 24, 0);
    lv_obj_set_style_border_color(button_3, lv_color_hex(0x78350f), 0);
    lv_obj_set_style_border_width(button_3, 2, 0);
    lv_obj_set_style_text_color(button_3, lv_color_hex(0x451a03), 0);
    lv_obj_set_style_pad_all(button_3, 10, 0);
    lv_obj_t * label_3 = lv_label_create(button_3);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "Local props");
}
#endif
