/**
 * @file lv_example_style_margin.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

#define ACCENT lv_color_hex(0x6366f1)

/**
 * @title Margin
 * @brief Reserve space around one item on top of the layout's own gaps.
 *
 * The row sets `style_pad_column="8"` between chips. The middle chip adds
 * `style_margin_left="24"` and `style_margin_right="24"`, so it sits
 * visibly further from its neighbors than the 8 px flex gap — margin
 * reserves space around a Widget without changing the Widget's size.
 */
void lv_example_style_margin(void)
{
    static lv_style_t style_chip;
    static lv_style_t style_chip_accent;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_chip);
        lv_style_set_radius(&style_chip, 10);
        lv_style_set_bg_opa(&style_chip, (255 * 100 / 100));
        lv_style_set_bg_color(&style_chip, lv_color_hex(0xe2e8f0));
        lv_style_set_pad_all(&style_chip, 14);

        lv_style_init(&style_chip_accent);
        lv_style_set_radius(&style_chip_accent, 10);
        lv_style_set_bg_opa(&style_chip_accent, (255 * 100 / 100));
        lv_style_set_bg_color(&style_chip_accent, ACCENT);
        lv_style_set_pad_all(&style_chip_accent, 14);
        lv_style_set_text_color(&style_chip_accent, lv_color_hex(0xffffff));

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* 💡 Change the middle chip's `style_margin_left`/`style_margin_right`; the side chips stay put, the gap grows. */
    lv_obj_t * label_1 = lv_label_create(screen);
    lv_label_set_text(label_1, "A");
    lv_obj_add_style(label_1, &style_chip, 0);

    lv_obj_t * label_2 = lv_label_create(screen);
    lv_label_set_text(label_2, "B");
    lv_obj_set_style_margin_left(label_2, 48, 0);
    lv_obj_set_style_margin_right(label_2, 24, 0);
    lv_obj_add_style(label_2, &style_chip_accent, 0);

    lv_obj_t * label_3 = lv_label_create(screen);
    lv_label_set_text(label_3, "C");
    lv_obj_add_style(label_3, &style_chip, 0);
}
#endif
