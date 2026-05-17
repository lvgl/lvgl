/**
 * @file lv_example_tabview_active_tab.c
 */

#include "../../../../lvgl.h"

/**
 * @title Tab view active tab on creation
 * @brief Open a specific tab on first display via the `active` prop.
 *
 * `active` takes a 0-based tab index. Setting `active="2"` makes the third tab
 * ("C") open when the screen first appears, so the user lands directly on a
 * specific tab without having to tap or swipe.
 */
void lv_example_tabview_active_tab_create(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 active is 0-based — change the value to pick which tab opens first. */
    lv_obj_t * lv_tabview_0 = lv_tabview_create(screen);
    lv_obj_set_size(lv_tabview_0, lv_pct(100), lv_pct(100));
    lv_tabview_set_active(lv_tabview_0, 2, LV_ANIM_OFF);
    lv_obj_t * lv_tabview_tab_0 = lv_tabview_add_tab(lv_tabview_0, "A");
    lv_obj_t * lv_label_0 = lv_label_create(lv_tabview_tab_0);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_0, "A");

    lv_obj_t * lv_tabview_tab_1 = lv_tabview_add_tab(lv_tabview_0, "B");
    lv_obj_t * lv_label_1 = lv_label_create(lv_tabview_tab_1);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "B");

    lv_obj_t * lv_tabview_tab_2 = lv_tabview_add_tab(lv_tabview_0, "C");
    lv_obj_t * lv_label_2 = lv_label_create(lv_tabview_tab_2);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "C — opened first");

    lv_obj_t * lv_tabview_tab_3 = lv_tabview_add_tab(lv_tabview_0, "D");
    lv_obj_t * lv_label_3 = lv_label_create(lv_tabview_tab_3);
    lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_3, "D");
}

