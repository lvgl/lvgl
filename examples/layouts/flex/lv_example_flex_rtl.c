/**
 * @file lv_example_flex_rtl.c
 */

#include "../../lv_examples.h"
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
    lv_obj_t * label_1 = lv_label_create(screen);
    lv_label_set_text(label_1, "LTR (default):");

    lv_obj_t * container_1 = lv_obj_create(screen);
    lv_obj_set_flex_flow(container_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(container_1, lv_pct(90), LV_SIZE_CONTENT);
    lv_obj_t * label_2 = lv_label_create(container_1);
    lv_obj_set_style_bg_color(label_2, lv_color_hex(0x3498db), 0);
    lv_label_set_text(label_2, "First");
    lv_obj_add_style(label_2, &style_label, 0);

    lv_obj_t * label_3 = lv_label_create(container_1);
    lv_obj_set_style_bg_color(label_3, lv_color_hex(0x2ecc71), 0);
    lv_label_set_text(label_3, "Second");
    lv_obj_add_style(label_3, &style_label, 0);

    lv_obj_t * label_4 = lv_label_create(container_1);
    lv_obj_set_style_bg_color(label_4, lv_color_hex(0xe74c3c), 0);
    lv_label_set_text(label_4, "Third");
    lv_obj_add_style(label_4, &style_label, 0);

    /* Right-to-left row with the same children */
    lv_obj_t * label_5 = lv_label_create(screen);
    lv_label_set_text(label_5, "RTL:");

    lv_obj_t * container_2 = lv_obj_create(screen);
    lv_obj_set_flex_flow(container_2, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_base_dir(container_2, LV_BASE_DIR_RTL, 0);
    lv_obj_set_size(container_2, lv_pct(90), LV_SIZE_CONTENT);
    lv_obj_t * label_6 = lv_label_create(container_2);
    lv_obj_set_style_bg_color(label_6, lv_color_hex(0x3498db), 0);
    lv_label_set_text(label_6, "First");
    lv_obj_add_style(label_6, &style_label, 0);

    lv_obj_t * label_7 = lv_label_create(container_2);
    lv_obj_set_style_bg_color(label_7, lv_color_hex(0x2ecc71), 0);
    lv_label_set_text(label_7, "Second");
    lv_obj_add_style(label_7, &style_label, 0);

    lv_obj_t * label_8 = lv_label_create(container_2);
    lv_obj_set_style_bg_color(label_8, lv_color_hex(0xe74c3c), 0);
    lv_label_set_text(label_8, "Third");
    lv_obj_add_style(label_8, &style_label, 0);
}
#endif
