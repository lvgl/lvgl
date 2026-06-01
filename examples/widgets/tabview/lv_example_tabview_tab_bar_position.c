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

    /* Bar on the right edge */
    lv_obj_t * tabview_1 = lv_tabview_create(screen);
    lv_tabview_set_tab_bar_position(tabview_1, LV_DIR_RIGHT);
    lv_obj_set_style_bg_color(tabview_1, lv_color_hex(0xce93d8), 0);
    lv_obj_set_style_bg_opa(tabview_1, (255 * 100 / 100), 0);
    lv_obj_t * lv_tabview_tab_0 = lv_tabview_add_tab(tabview_1, "A");
    lv_obj_t * label_1 = lv_label_create(lv_tabview_tab_0);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Right tab bar");

    lv_obj_t * lv_tabview_tab_1 = lv_tabview_add_tab(tabview_1, "B");
    lv_obj_t * label_2 = lv_label_create(lv_tabview_tab_1);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Same here");
}
#endif
