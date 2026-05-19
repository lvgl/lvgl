/**
 * @file lv_example_tabview_basic.c
 */

#include "../../../lv_examples.h"
#if LV_USE_TABVIEW && LV_BUILD_EXAMPLES

/**
 * @title Tab view basic structure
 * @brief Three tabs hosting plain labels, demonstrating the minimal markup.
 *
 * An `lv_tabview` carries three `lv_tabview-tab` children. The `text` arg sets
 * each tab's button caption and the children of a tab become its content. No
 * `tab_bar_position` or `active` is set, so the bar sits on the top edge and
 * the first tab opens.
 */
void lv_example_tabview_basic(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Tap a tab button or swipe horizontally to switch tabs. */
    lv_obj_t * lv_tabview_0 = lv_tabview_create(screen);
    lv_obj_set_size(lv_tabview_0, lv_pct(100), lv_pct(100));
    lv_obj_t * lv_tabview_tab_0 = lv_tabview_add_tab(lv_tabview_0, "Tab 1");
    lv_obj_t * lv_label_0 = lv_label_create(lv_tabview_tab_0);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_0, "First tab");

    lv_obj_t * lv_tabview_tab_1 = lv_tabview_add_tab(lv_tabview_0, "Tab 2");
    lv_obj_t * lv_label_1 = lv_label_create(lv_tabview_tab_1);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "Second tab");

    lv_obj_t * lv_tabview_tab_2 = lv_tabview_add_tab(lv_tabview_0, "Tab 3");
    lv_obj_t * lv_label_2 = lv_label_create(lv_tabview_tab_2);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "Third tab");
}
#endif
