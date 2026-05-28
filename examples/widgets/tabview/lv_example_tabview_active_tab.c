/**
 * @file lv_example_tabview_active_tab.c
 */

#include "../../lv_examples.h"
#if LV_USE_TABVIEW && LV_BUILD_EXAMPLES

/**
 * @title Tab view active tab on creation
 * @brief Open a specific tab on first display via the `active` prop.
 *
 * `active` takes a 0-based tab index. Setting `active="2"` makes the third tab
 * ("C") open when the screen first appears, so the user lands directly on a
 * specific tab without having to tap or swipe.
 */
void lv_example_tabview_active_tab(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 active is 0-based — change the value to pick which tab opens first. */
    lv_obj_t * tabview = lv_tabview_create(screen);
    lv_obj_set_size(tabview, lv_pct(100), lv_pct(100));
    lv_obj_t * lv_tabview_tab_0 = lv_tabview_add_tab(tabview, "A");
    lv_obj_t * label_1 = lv_label_create(lv_tabview_tab_0);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "A");

    lv_obj_t * lv_tabview_tab_1 = lv_tabview_add_tab(tabview, "B");
    lv_obj_t * label_2 = lv_label_create(lv_tabview_tab_1);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "B");

    lv_obj_t * lv_tabview_tab_2 = lv_tabview_add_tab(tabview, "C");
    lv_obj_t * label_3 = lv_label_create(lv_tabview_tab_2);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "C — opened first");

    lv_obj_t * lv_tabview_tab_3 = lv_tabview_add_tab(tabview, "D");
    lv_obj_t * label_4 = lv_label_create(lv_tabview_tab_3);
    lv_obj_set_align(label_4, LV_ALIGN_CENTER);
    lv_label_set_text(label_4, "D");
}
#endif
