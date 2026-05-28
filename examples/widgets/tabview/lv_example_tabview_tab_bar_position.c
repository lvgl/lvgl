/**
 * @file lv_example_tabview_tab_bar_position.c
 */

#include "../../lv_examples.h"
#if LV_USE_TABVIEW && LV_BUILD_EXAMPLES

/**
 * @title Tab view tab bar position
 * @brief Place the tab bar on any of the four sides.
 *
 * Four mini tab views lay out a 2×2 grid covering every `tab_bar_position` value:
 * `top`, `bottom`, `left`, `right`. Each carries a distinct `style_bg_color` so
 * the four widgets are easy to tell apart at a glance; only `tab_bar_position`
 * changes the layout itself.
 */
void lv_example_tabview_tab_bar_position(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_all(screen, 8, 0);

    /* Bar on the top edge */
    lv_obj_t * tabview_1 = lv_tabview_create(screen);
    lv_obj_set_size(tabview_1, lv_pct(45), 100);
    lv_tabview_set_tab_bar_position(tabview_1, LV_DIR_TOP);
    lv_obj_set_style_bg_color(tabview_1, lv_color_hex(0xffcc80), 0);
    lv_obj_set_style_bg_opa(tabview_1, (255 * 100 / 100), 0);
    lv_obj_t * lv_tabview_tab_0 = lv_tabview_add_tab(tabview_1, "A");
    lv_obj_t * label_1 = lv_label_create(lv_tabview_tab_0);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "top");

    lv_tabview_add_tab(tabview_1, "B");

    lv_obj_t * tabview_2 = lv_tabview_create(screen);
    lv_obj_set_size(tabview_2, lv_pct(45), 100);
    lv_tabview_set_tab_bar_position(tabview_2, LV_DIR_BOTTOM);
    lv_obj_set_style_bg_color(tabview_2, lv_color_hex(0x90caf9), 0);
    lv_obj_set_style_bg_opa(tabview_2, (255 * 100 / 100), 0);
    lv_obj_t * lv_tabview_tab_2 = lv_tabview_add_tab(tabview_2, "A");
    lv_obj_t * label_2 = lv_label_create(lv_tabview_tab_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "bottom");

    lv_tabview_add_tab(tabview_2, "B");

    /* Bar on the bottom edge */
    lv_obj_t * tabview_3 = lv_tabview_create(screen);
    lv_obj_set_size(tabview_3, lv_pct(45), 100);
    lv_tabview_set_tab_bar_position(tabview_3, LV_DIR_LEFT);
    lv_obj_set_style_bg_color(tabview_3, lv_color_hex(0xa5d6a7), 0);
    lv_obj_set_style_bg_opa(tabview_3, (255 * 100 / 100), 0);
    lv_obj_t * lv_tabview_tab_4 = lv_tabview_add_tab(tabview_3, "A");
    lv_obj_t * label_3 = lv_label_create(lv_tabview_tab_4);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "left");

    lv_tabview_add_tab(tabview_3, "B");

    lv_obj_t * tabview_4 = lv_tabview_create(screen);
    lv_obj_set_size(tabview_4, lv_pct(45), 100);
    lv_tabview_set_tab_bar_position(tabview_4, LV_DIR_RIGHT);
    lv_obj_set_style_bg_color(tabview_4, lv_color_hex(0xce93d8), 0);
    lv_obj_set_style_bg_opa(tabview_4, (255 * 100 / 100), 0);
    lv_obj_t * lv_tabview_tab_6 = lv_tabview_add_tab(tabview_4, "A");
    lv_obj_t * label_4 = lv_label_create(lv_tabview_tab_6);
    lv_obj_set_align(label_4, LV_ALIGN_CENTER);
    lv_label_set_text(label_4, "right");

    lv_tabview_add_tab(tabview_4, "B");
}
#endif
