/**
 * @file lv_example_style_size_position.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

#define ACCENT lv_color_hex(0x6366f1)

/**
 * @title Size and padding
 * @brief Set a Widget's width and padding; its height follows the content.
 *
 * Both cards share one style but set a different `width` (140 vs 240) and
 * `style_pad_all` (12 vs 28). `height="content"` lets each grow to exactly
 * fit its label plus that padding, so the same style yields a tight chip
 * and a roomy panel — size comes only from these properties.
 */
void lv_example_style_size_position(void)
{
    static lv_style_t style_card;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_card);
        lv_style_set_radius(&style_card, 16);
        lv_style_set_bg_opa(&style_card, (255 * 100 / 100));
        lv_style_set_bg_color(&style_card, lv_color_hex(0xffffff));
        lv_style_set_border_width(&style_card, 0);
        lv_style_set_shadow_color(&style_card, ACCENT);
        lv_style_set_shadow_width(&style_card, 22);
        lv_style_set_shadow_offset_y(&style_card, 6);
        lv_style_set_shadow_opa(&style_card, 55);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();

    /* 💡 Change a card's `width` or `style_pad_all`; `height="content"` keeps it exactly as tall as its text. */
    lv_obj_t * container_1 = lv_obj_create(screen);
    lv_obj_set_size(container_1, 140, LV_SIZE_CONTENT);
    lv_obj_set_align(container_1, LV_ALIGN_CENTER);
    lv_obj_set_y(container_1, -40);
    lv_obj_set_style_pad_all(container_1, 12, 0);
    lv_obj_add_style(container_1, &style_card, 0);
    lv_obj_t * label_1 = lv_label_create(container_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "140 wide");

    lv_obj_t * container_2 = lv_obj_create(screen);
    lv_obj_set_size(container_2, 260, LV_SIZE_CONTENT);
    lv_obj_set_align(container_2, LV_ALIGN_CENTER);
    lv_obj_set_y(container_2, 40);
    lv_obj_set_style_pad_all(container_2, 28, 0);
    lv_obj_add_style(container_2, &style_card, 0);
    lv_obj_t * label_2 = lv_label_create(container_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "260 wide, roomy padding");
}
#endif
