/**
 * @file lv_example_tabview_tab_bar_style.c
 */

#include "../../lv_examples.h"
#if LV_USE_TABVIEW && LV_BUILD_EXAMPLES

/**
 * @title Tab view styling spots
 * @brief Three independent style targets: tab view body, tab bar, and a single tab.
 *
 * The tab view body gets a dark `style_bg_color` (with `style_bg_opa="100%"` so
 * it actually paints). The `<lv_tabview-tab_bar>` element overrides the bar's
 * width, background, and text color. One `<lv_tabview-tab>` then sets its own
 * `bg_color` + `bg_opa` to override only that tab's content area; the others
 * stay transparent and let the tab view body show through. `active="1"` opens
 * the styled tab first so the per-tab override is immediately visible.
 */
void lv_example_tabview_tab_bar_style(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Switch tabs: tab B paints its own bg, A and C show the tab view body color. */
    lv_obj_t * tabview = lv_tabview_create(screen);
    lv_obj_set_size(tabview, lv_pct(100), lv_pct(100));
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
    lv_tabview_set_active(tabview, 1, false);
    lv_obj_set_style_bg_color(tabview, lv_color_hex(0x37474f), 0);
    lv_obj_set_style_bg_opa(tabview, (255 * 100 / 100), 0);
    lv_obj_t * lv_tabview_tab_bar_0 = lv_tabview_get_tab_bar(tabview);
    lv_obj_set_width(lv_tabview_tab_bar_0, 100);
    lv_obj_set_style_bg_color(lv_tabview_tab_bar_0, lv_color_hex(0x222831), 0);
    lv_obj_set_style_bg_opa(lv_tabview_tab_bar_0, (255 * 100 / 100), 0);
    lv_obj_set_style_text_color(lv_tabview_tab_bar_0, lv_color_hex(0xeeeeee), 0);
    lv_obj_t * lv_tabview_tab_0 = lv_tabview_add_tab(tabview, "A");
    lv_obj_t * label_1 = lv_label_create(lv_tabview_tab_0);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(label_1, lv_color_hex(0xeeeeee), 0);
    lv_label_set_text(label_1, "tab view bg = 0x37474f");

    lv_obj_t * lv_tabview_tab_1 = lv_tabview_add_tab(tabview, "B");
    lv_obj_set_style_bg_color(lv_tabview_tab_1, lv_color_hex(0xff7043), 0);
    lv_obj_set_style_bg_opa(lv_tabview_tab_1, (255 * 100 / 100), 0);
    lv_obj_t * label_2 = lv_label_create(lv_tabview_tab_1);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "this tab overrides bg = 0xff7043");

    lv_obj_t * lv_tabview_tab_2 = lv_tabview_add_tab(tabview, "C");
    lv_obj_t * label_3 = lv_label_create(lv_tabview_tab_2);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(label_3, lv_color_hex(0xeeeeee), 0);
    lv_label_set_text(label_3, "tab bar bg = 0x222831");
}
#endif
