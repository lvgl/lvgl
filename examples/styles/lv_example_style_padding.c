/**
 * @file lv_example_style_padding.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

#define ACCENT lv_color_hex(0x6366f1)

/**
 * @title Padding
 * @brief Inset a Widget's content per side with the directional pad_* properties.
 *
 * Padding shrinks the content area. The left card pads only top and left
 * (`style_pad_top`/`style_pad_left` = 26, the opposite sides 8); the right
 * card pads every side by 18. An accent block fills each card at
 * 100% × 100%, so the offset makes asymmetric vs uniform inset obvious.
 */
void lv_example_style_padding(void)
{
    static lv_style_t style_card;
    static lv_style_t style_block;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_card);
        lv_style_set_radius(&style_card, 16);
        lv_style_set_bg_opa(&style_card, (255 * 100 / 100));
        lv_style_set_bg_color(&style_card, lv_color_hex(0xffffff));
        lv_style_set_border_width(&style_card, 1);
        lv_style_set_border_color(&style_card, lv_color_hex(0xe2e8f0));

        lv_style_init(&style_block);
        lv_style_set_radius(&style_block, 10);
        lv_style_set_bg_opa(&style_block, (255 * 100 / 100));
        lv_style_set_bg_color(&style_block, ACCENT);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 18, 0);

    /* 💡 Edit each card's `style_pad_*`; the accent block tracks the shrinking content area. */
    lv_obj_t * container_1 = lv_obj_create(screen);
    lv_obj_set_size(container_1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(container_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(container_1, 22, 0);
    lv_obj_set_style_bg_opa(container_1, 0, 0);
    lv_obj_set_style_border_width(container_1, 0, 0);
    lv_obj_set_style_pad_all(container_1, 0, 0);
    lv_obj_t * container_2 = lv_obj_create(container_1);
    lv_obj_set_size(container_2, 120, 120);
    lv_obj_set_style_pad_top(container_2, 26, 0);
    lv_obj_set_style_pad_left(container_2, 26, 0);
    lv_obj_set_style_pad_right(container_2, 8, 0);
    lv_obj_set_style_pad_bottom(container_2, 8, 0);
    lv_obj_add_style(container_2, &style_card, 0);
    lv_obj_t * container_3 = lv_obj_create(container_2);
    lv_obj_set_size(container_3, lv_pct(100), lv_pct(100));
    lv_obj_add_style(container_3, &style_block, 0);

    lv_obj_t * container_4 = lv_obj_create(container_1);
    lv_obj_set_size(container_4, 120, 120);
    lv_obj_set_style_pad_all(container_4, 18, 0);
    lv_obj_add_style(container_4, &style_card, 0);
    lv_obj_t * container_5 = lv_obj_create(container_4);
    lv_obj_set_size(container_5, lv_pct(100), lv_pct(100));
    lv_obj_add_style(container_5, &style_block, 0);
}
#endif
