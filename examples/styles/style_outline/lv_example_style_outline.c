/**
 * @file lv_example_style_outline.c
 */

#include "../../../lvgl.h"

#define ACCENT lv_color_hex(0x6366f1)

/**
 * @title Outline
 * @brief Add a focus-style ring that sits outside the card and ignores layout.
 *
 * An outline is painted outside the box, so unlike a border it never
 * resizes the Widget. `outline_color` + `outline_width="3"` draw the ring
 * and `outline_pad="6"` is the gap between the card edge and the ring —
 * the look of a modern keyboard-focus halo.
 */
void lv_example_style_outline_create(void)
{
    static lv_style_t style_focus;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_focus);
        lv_style_set_radius(&style_focus, 14);
        lv_style_set_bg_opa(&style_focus, (255 * 100 / 100));
        lv_style_set_bg_color(&style_focus, lv_color_hex(0xffffff));
        lv_style_set_border_width(&style_focus, 1);
        lv_style_set_border_color(&style_focus, lv_color_hex(0xe2e8f0));
        lv_style_set_outline_color(&style_focus, ACCENT);
        lv_style_set_outline_width(&style_focus, 3);
        lv_style_set_outline_pad(&style_focus, 6);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Raise `outline_pad` to push the ring further out; the card and layout never move. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Style: outline");

    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_size(lv_obj_1, 200, 120);
    lv_obj_add_style(lv_obj_1, &style_focus, 0);
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "Focused");
}

