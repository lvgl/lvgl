/**
 * @file lv_example_checkbox_styling.c
 */

#include "../../lv_examples.h"
#if LV_USE_CHECKBOX && LV_BUILD_EXAMPLES

/**
 * @title Checkbox styling
 * @brief Style the tick box (INDICATOR) and its checked state separately.
 *
 * Checkbox exposes MAIN (label text area) and INDICATOR (tick box). The named styles
 * target INDICATOR in both default and `checked` states so the tick box has a soft,
 * rounded look unchecked and switches to an accent color with a tinted halo when
 * checked. MAIN is styled with local `style_*` props to recolor the label area.
 */
void lv_example_checkbox_styling(void)
{
    static lv_style_t style_cb_indicator;
    static lv_style_t style_cb_indicator_checked;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_cb_indicator);
        lv_style_set_bg_opa(&style_cb_indicator, (255 * 100 / 100));
        lv_style_set_bg_color(&style_cb_indicator, lv_color_hex(0xf9fafb));
        lv_style_set_radius(&style_cb_indicator, 6);
        lv_style_set_border_color(&style_cb_indicator, lv_color_hex(0x9ca3af));
        lv_style_set_border_width(&style_cb_indicator, 2);
        lv_style_set_pad_all(&style_cb_indicator, 4);

        lv_style_init(&style_cb_indicator_checked);
        lv_style_set_bg_color(&style_cb_indicator_checked, lv_color_hex(0x16a34a));
        lv_style_set_border_color(&style_cb_indicator_checked, lv_color_hex(0x14532d));
        lv_style_set_outline_color(&style_cb_indicator_checked, lv_color_hex(0x16a34a));
        lv_style_set_outline_width(&style_cb_indicator_checked, 4);
        lv_style_set_outline_pad(&style_cb_indicator_checked, 2);
        lv_style_set_outline_opa(&style_cb_indicator_checked, 80);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 14, 0);

    /* Unchecked: only the default indicator style applies */
    lv_obj_t * checkbox_1 = lv_checkbox_create(screen);
    lv_checkbox_set_text(checkbox_1, "Default look");
    lv_obj_set_style_text_color(checkbox_1, lv_color_hex(0x111827), 0);
    lv_obj_set_style_pad_column(checkbox_1, 10, 0);
    lv_obj_add_style(checkbox_1, &style_cb_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(checkbox_1, &style_cb_indicator_checked, LV_PART_INDICATOR | LV_STATE_CHECKED);

    /* Pre-checked so the checked-state style is visible at load time */
    lv_obj_t * checkbox_2 = lv_checkbox_create(screen);
    lv_checkbox_set_text(checkbox_2, "Pre-checked");
    lv_obj_set_state(checkbox_2, LV_STATE_CHECKED, true);
    lv_obj_set_style_text_color(checkbox_2, lv_color_hex(0x111827), 0);
    lv_obj_set_style_pad_column(checkbox_2, 10, 0);
    lv_obj_add_style(checkbox_2, &style_cb_indicator, LV_PART_INDICATOR);
    lv_obj_add_style(checkbox_2, &style_cb_indicator_checked, LV_PART_INDICATOR | LV_STATE_CHECKED);
}
#endif
