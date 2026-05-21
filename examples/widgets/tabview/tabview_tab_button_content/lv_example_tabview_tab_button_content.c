/**
 * @file lv_example_tabview_tab_button_content.c
 */

#include "../../../lv_examples.h"
#if LV_USE_TABVIEW && LV_BUILD_EXAMPLES

/**
 * @title Tab view tab-button content
 * @brief Attach a child widget to a specific tab button via `<lv_tabview-tab_button>`.
 *
 * `<lv_tabview-tab_button index="N">` selects the implicit tab button at the
 * given index so additional children can be parented to it after the tab has
 * been created. Both tab buttons here pick up a small `lv_switch` beside the
 * caption — handy for inline toggles or status indicators without having to
 * build a custom tab bar from scratch. The tab elements come first so the
 * buttons exist by the time the `_button` references attach to them.
 */
void lv_example_tabview_tab_button_content(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Tap the switch inside a tab button — it lives on the tab button itself, not on the tab content. */
    lv_obj_t * lv_tabview_0 = lv_tabview_create(screen);
    lv_obj_set_size(lv_tabview_0, lv_pct(100), lv_pct(100));
    lv_obj_t * lv_tabview_tab_0 = lv_tabview_add_tab(lv_tabview_0, "WiFi");
    lv_obj_t * lv_label_0 = lv_label_create(lv_tabview_tab_0);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_0, "WiFi tab content");

    lv_obj_t * lv_tabview_tab_1 = lv_tabview_add_tab(lv_tabview_0, "Bluetooth");
    lv_obj_t * lv_label_1 = lv_label_create(lv_tabview_tab_1);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "Bluetooth tab content");

    lv_obj_t * lv_tabview_tab_button_0 = lv_tabview_get_tab_button(lv_tabview_0, 0);
    lv_obj_t * lv_switch_0 = lv_switch_create(lv_tabview_tab_button_0);
    lv_obj_set_size(lv_switch_0, 40, 20);
    lv_obj_set_flag(lv_switch_0, LV_OBJ_FLAG_IGNORE_LAYOUT, true);
    lv_obj_set_align(lv_switch_0, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(lv_switch_0, -10);

    lv_obj_t * lv_tabview_tab_button_1 = lv_tabview_get_tab_button(lv_tabview_0, 1);
    lv_obj_t * lv_switch_1 = lv_switch_create(lv_tabview_tab_button_1);
    lv_obj_set_size(lv_switch_1, 40, 20);
    lv_obj_set_flag(lv_switch_1, LV_OBJ_FLAG_IGNORE_LAYOUT, true);
    lv_obj_set_align(lv_switch_1, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(lv_switch_1, -10);
}
#endif
