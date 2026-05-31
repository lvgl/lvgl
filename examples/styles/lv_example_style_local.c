/**
 * @file lv_example_style_local.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

#define ACCENT lv_color_hex(0x6366f1)

/**
 * @title Local style override
 * @brief Local properties — including state selectors — beat the shared style.
 *
 * All three buttons share `style_shared` (indigo, dark border, radius).
 * "Override" adds a plain local `style_bg_color`, so it is always pink.
 * "On press" adds a state-scoped local `style_bg_color-pressed`, so it
 * turns green only while held. Both keep the shared border, showing local
 * styles — and local *state* selectors — have the highest precedence.
 */
void lv_example_style_local(void)
{
    static lv_style_t style_shared;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_shared);
        lv_style_set_radius(&style_shared, 14);
        lv_style_set_bg_opa(&style_shared, (255 * 100 / 100));
        lv_style_set_bg_color(&style_shared, ACCENT);
        lv_style_set_border_width(&style_shared, 3);
        lv_style_set_border_color(&style_shared, lv_color_hex(0x312e81));
        lv_style_set_pad_all(&style_shared, 14);
        lv_style_set_text_color(&style_shared, lv_color_hex(0xffffff));

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 18, 0);

    /* 💡 "Override" is always pink; press "On press" — its `style_bg_color-pressed` wins only while held. Borders never change. */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_size(container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(container, 16, 0);
    lv_obj_set_style_bg_opa(container, 0, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_pad_all(container, 0, 0);
    lv_obj_t * button_1 = lv_button_create(container);
    lv_obj_set_width(button_1, 95);
    lv_obj_add_style(button_1, &style_shared, 0);
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Shared");

    /* Plain local override: fill always wins over the shared style */
    lv_obj_t * button_2 = lv_button_create(container);
    lv_obj_set_width(button_2, 95);
    lv_obj_set_style_bg_color(button_2, lv_color_hex(0xec4899), 0);
    lv_obj_add_style(button_2, &style_shared, 0);
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Override");

    /* Local state selector: overrides the fill only while pressed */
    lv_obj_t * button_3 = lv_button_create(container);
    lv_obj_set_width(button_3, 95);
    lv_obj_set_style_bg_color(button_3, lv_color_hex(0x10b981), LV_STATE_PRESSED);
    lv_obj_add_style(button_3, &style_shared, 0);
    lv_obj_t * label_3 = lv_label_create(button_3);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "On press");
}
#endif
