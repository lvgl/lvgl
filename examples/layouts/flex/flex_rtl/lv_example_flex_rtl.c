/**
 * @file lv_example_flex_rtl.c
 */

#include "../../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * @title Flex RTL
 * @brief Reverse the main axis direction with style_base_dir.
 *
 * The same three children appear in two identical row containers. Setting style_base_dir
 * to rtl on the second container reverses item ordering on the main axis without changing
 * the order of children in XML — handy for right-to-left languages or mirrored layouts.
 */
void lv_example_flex_rtl(void)
{
    static lv_style_t style_label;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_label);
        lv_style_set_bg_opa(&style_label, (255 * 100 / 100));
        lv_style_set_text_color(&style_label, lv_color_hex(0xffffff));
        lv_style_set_pad_all(&style_label, 8);

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 10, 0);

    /* Default left-to-right row */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_label_set_text(lv_label_0, "LTR (default):");

    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(lv_obj_1, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_t * lv_label_1 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_1, lv_color_hex(0x3498db), 0);
    lv_label_set_text(lv_label_1, "First");
    lv_obj_add_style(lv_label_1, &style_label, 0);

    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_2, lv_color_hex(0x2ecc71), 0);
    lv_label_set_text(lv_label_2, "Second");
    lv_obj_add_style(lv_label_2, &style_label, 0);

    lv_obj_t * lv_label_3 = lv_label_create(lv_obj_1);
    lv_obj_set_style_bg_color(lv_label_3, lv_color_hex(0xe74c3c), 0);
    lv_label_set_text(lv_label_3, "Third");
    lv_obj_add_style(lv_label_3, &style_label, 0);

    /* Right-to-left row with the same children */
    lv_obj_t * lv_label_4 = lv_label_create(screen);
    lv_label_set_text(lv_label_4, "RTL:");

    lv_obj_t * lv_obj_2 = lv_obj_create(screen);
    lv_obj_set_flex_flow(lv_obj_2, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_base_dir(lv_obj_2, LV_BASE_DIR_RTL, 0);
    lv_obj_set_size(lv_obj_2, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_t * lv_label_5 = lv_label_create(lv_obj_2);
    lv_obj_set_style_bg_color(lv_label_5, lv_color_hex(0x3498db), 0);
    lv_label_set_text(lv_label_5, "First");
    lv_obj_add_style(lv_label_5, &style_label, 0);

    lv_obj_t * lv_label_6 = lv_label_create(lv_obj_2);
    lv_obj_set_style_bg_color(lv_label_6, lv_color_hex(0x2ecc71), 0);
    lv_label_set_text(lv_label_6, "Second");
    lv_obj_add_style(lv_label_6, &style_label, 0);

    lv_obj_t * lv_label_7 = lv_label_create(lv_obj_2);
    lv_obj_set_style_bg_color(lv_label_7, lv_color_hex(0xe74c3c), 0);
    lv_label_set_text(lv_label_7, "Third");
    lv_obj_add_style(lv_label_7, &style_label, 0);
}
#endif
