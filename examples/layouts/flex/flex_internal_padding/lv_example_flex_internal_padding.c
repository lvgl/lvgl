/**
 * @file lv_example_flex_internal_padding.c
 */

#include "../../../lv_examples.h"
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
        lv_style_set_pad_all(&style_tag, 8);
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
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_column(lv_obj_1, 10, 0);
    lv_obj_set_style_pad_row(lv_obj_1, 30, 0);
    lv_obj_set_size(lv_obj_1, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_t * lv_label_0 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_0, lv_color_hex(0x4a90d9), 0);
    lv_label_set_text(lv_label_0, "HTML");
    lv_obj_add_style(lv_label_0, &style_tag, 0);

    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_1, lv_color_hex(0xe67e22), 0);
    lv_label_set_text(lv_label_1, "CSS");
    lv_obj_add_style(lv_label_1, &style_tag, 0);

    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_2, lv_color_hex(0x27ae60), 0);
    lv_label_set_text(lv_label_2, "JavaScript");
    lv_obj_add_style(lv_label_2, &style_tag, 0);

    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_3, lv_color_hex(0x8e44ad), 0);
    lv_label_set_text(lv_label_3, "Python");
    lv_obj_add_style(lv_label_3, &style_tag, 0);

    lv_obj_t * lv_label_4 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_4, lv_color_hex(0xc0392b), 0);
    lv_label_set_text(lv_label_4, "C++");
    lv_obj_add_style(lv_label_4, &style_tag, 0);

    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_5, lv_color_hex(0x16a085), 0);
    lv_label_set_text(lv_label_5, "Rust");
    lv_obj_add_style(lv_label_5, &style_tag, 0);
}
#endif
