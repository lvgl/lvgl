/**
 * @file lv_example_flex_internal_padding.c
 */

#include "../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * @title Flex internal padding
 * @brief Set horizontal and vertical gaps between flex items.
 *
 * style_pad_column controls the gap between items on a row, and style_pad_row controls the
 * gap between wrapped rows. The container uses row_wrap so both gaps are visible at once
 * — the chips spread out horizontally and rows are pushed apart vertically.
 */
void lv_example_flex_internal_padding(void)
{
    static lv_style_t style_tag;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_tag);
        lv_style_set_bg_opa(&style_tag, (255 * 100 / 100));
        lv_style_set_pad_ver(&style_tag, 12);
        lv_style_set_pad_hor(&style_tag, 18);
        lv_style_set_radius(&style_tag, 4);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 10, 0);

    /* Demo container with row/column gaps */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_column(container, 10, 0);
    lv_obj_set_style_pad_row(container, 30, 0);
    lv_obj_set_size(container, lv_pct(90), LV_SIZE_CONTENT);
    lv_obj_t * label_1 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_1, lv_color_hex(0x5e9ee3), 0);
    lv_label_set_text(label_1, "HTML");
    lv_obj_add_style(label_1, &style_tag, 0);

    lv_obj_t * label_2 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_2, lv_color_hex(0xe67e22), 0);
    lv_label_set_text(label_2, "CSS");
    lv_obj_add_style(label_2, &style_tag, 0);

    lv_obj_t * label_3 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_3, lv_color_hex(0x27ae60), 0);
    lv_label_set_text(label_3, "JavaScript");
    lv_obj_add_style(label_3, &style_tag, 0);

    lv_obj_t * label_4 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_4, lv_color_hex(0x8e44ad), 0);
    lv_label_set_text(label_4, "Python");
    lv_obj_add_style(label_4, &style_tag, 0);

    lv_obj_t * label_5 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_5, lv_color_hex(0xc0392b), 0);
    lv_label_set_text(label_5, "C++");
    lv_obj_add_style(label_5, &style_tag, 0);

    lv_obj_t * label_6 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_6, lv_color_hex(0x16a085), 0);
    lv_label_set_text(label_6, "Rust");
    lv_obj_add_style(label_6, &style_tag, 0);

    lv_obj_t * label_7 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_7, lv_color_hex(0xd8da19), 0);
    lv_label_set_text(label_7, "Java");
    lv_obj_add_style(label_7, &style_tag, 0);

    lv_obj_t * label_8 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_8, lv_color_hex(0x18d99e), 0);
    lv_label_set_text(label_8, "PHP");
    lv_obj_add_style(label_8, &style_tag, 0);

    lv_obj_t * label_9 = lv_label_create(container);
    lv_obj_set_style_bg_color(label_9, lv_color_hex(0xeb4c9e), 0);
    lv_label_set_text(label_9, "Zig");
    lv_obj_add_style(label_9, &style_tag, 0);
}
#endif
