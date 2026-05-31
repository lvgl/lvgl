/**
 * @file lv_example_dropdown_styling.c
 */

#include "../../lv_examples.h"
#if LV_USE_DROPDOWN && LV_BUILD_EXAMPLES

/**
 * @title Dropdown styling
 * @brief Style the dropdown button (MAIN) with rounded corners and a focused border.
 *
 * The dropdown's primary surface is MAIN (the button users tap). Local `style_*` props
 * give it a card-like rounded look with padding and a custom border. A `focused` state
 * style applied to MAIN brightens the border ring when the dropdown receives focus, so
 * the example also demonstrates a state-specific style on a non-button widget.
 */
void lv_example_dropdown_styling(void)
{
    static lv_style_t style_dropdown_main;
    static lv_style_t style_dropdown_focused;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_dropdown_main);
        lv_style_set_bg_opa(&style_dropdown_main, (255 * 100 / 100));
        lv_style_set_bg_color(&style_dropdown_main, lv_color_hex(0xffffff));
        lv_style_set_radius(&style_dropdown_main, 10);
        lv_style_set_border_color(&style_dropdown_main, lv_color_hex(0xd1d5db));
        lv_style_set_border_width(&style_dropdown_main, 2);
        lv_style_set_text_color(&style_dropdown_main, lv_color_hex(0x111827));
        lv_style_set_pad_left(&style_dropdown_main, 14);
        lv_style_set_pad_right(&style_dropdown_main, 14);
        lv_style_set_pad_top(&style_dropdown_main, 10);
        lv_style_set_pad_bottom(&style_dropdown_main, 10);

        lv_style_init(&style_dropdown_focused);
        lv_style_set_border_color(&style_dropdown_focused, lv_color_hex(0x6366f1));
        lv_style_set_outline_color(&style_dropdown_focused, lv_color_hex(0x6366f1));
        lv_style_set_outline_width(&style_dropdown_focused, 3);
        lv_style_set_outline_pad(&style_dropdown_focused, 2);
        lv_style_set_outline_opa(&style_dropdown_focused, 120);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Tap or focus the dropdown to see the focused-state border swap in. */
    lv_obj_t * dropdown = lv_dropdown_create(screen);
    lv_obj_set_width(dropdown, 200);
    lv_dropdown_set_options(dropdown, "Apple\nBanana\nCherry\nDate");
    lv_obj_add_style(dropdown, &style_dropdown_main, LV_PART_MAIN);
    lv_obj_add_style(dropdown, &style_dropdown_focused, LV_PART_MAIN | LV_STATE_FOCUSED);
}
#endif
