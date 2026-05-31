/**
 * @file lv_example_label_styling.c
 */

#include "../../lv_examples.h"
#if LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * @title Label styling
 * @brief Decorate labels with background, padding, border, outline, and shadow.
 *
 * A label is just text, but it inherits the full background/border/outline/shadow stack
 * from the base widget. The named "badge" style turns a label into a pill via radius +
 * padding + bg color; the second label demonstrates the same effect built purely from
 * local `style_*` attributes for a one-off look.
 */
void lv_example_label_styling(void)
{
    static lv_style_t style_badge;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_badge);
        lv_style_set_bg_opa(&style_badge, (255 * 100 / 100));
        lv_style_set_bg_color(&style_badge, lv_color_hex(0x6366f1));
        lv_style_set_radius(&style_badge, 100);
        lv_style_set_pad_left(&style_badge, 14);
        lv_style_set_pad_right(&style_badge, 14);
        lv_style_set_pad_top(&style_badge, 4);
        lv_style_set_pad_bottom(&style_badge, 4);
        lv_style_set_text_color(&style_badge, lv_color_hex(0xffffff));

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Named-style pill badge */
    lv_obj_t * label_1 = lv_label_create(screen);
    lv_label_set_text(label_1, "NEW");
    lv_obj_add_style(label_1, &style_badge, 0);

    /* Local-style call-out: border + outline halo + shadow + padding */
    lv_obj_t * label_2 = lv_label_create(screen);
    lv_label_set_text(label_2, "Heads up");
    lv_obj_set_style_bg_opa(label_2, (255 * 100 / 100), 0);
    lv_obj_set_style_bg_color(label_2, lv_color_hex(0xfef3c7), 0);
    lv_obj_set_style_text_color(label_2, lv_color_hex(0x92400e), 0);
    lv_obj_set_style_border_color(label_2, lv_color_hex(0xf59e0b), 0);
    lv_obj_set_style_border_width(label_2, 2, 0);
    lv_obj_set_style_radius(label_2, 8, 0);
    lv_obj_set_style_pad_all(label_2, 10, 0);
    lv_obj_set_style_outline_color(label_2, lv_color_hex(0xf59e0b), 0);
    lv_obj_set_style_outline_width(label_2, 3, 0);
    lv_obj_set_style_outline_pad(label_2, 3, 0);
    lv_obj_set_style_outline_opa(label_2, 80, 0);
    lv_obj_set_style_shadow_color(label_2, lv_color_hex(0x78350f), 0);
    lv_obj_set_style_shadow_width(label_2, 14, 0);
    lv_obj_set_style_shadow_offset_y(label_2, 3, 0);
    lv_obj_set_style_shadow_opa(label_2, 100, 0);
}
#endif
