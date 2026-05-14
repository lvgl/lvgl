/**
 * @file lv_example_dropdown_text.c
 */

#include "../../../../lvgl.h"

/**
 * @title Dropdown fixed text
 * @brief Pin the dropdown button label regardless of selection.
 *
 * Without a text attribute the dropdown shows the currently selected option (left). With
 * text="Menu" the button always displays that string while the option list remains
 * intact, which is useful for icon-style or fixed-label menus.
 */
void lv_example_dropdown_text_create(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Set text to make the button always show that fixed label, regardless of which option is picked. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Dropdown: fixed text");

    /* Without text the button reflects the selected option */
    lv_obj_t * lv_dropdown_0 = lv_dropdown_create(screen);
    lv_obj_set_align(lv_dropdown_0, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(lv_dropdown_0, 14, 46);
    lv_obj_set_width(lv_dropdown_0, 110);
    lv_dropdown_set_options(lv_dropdown_0, "Cut\nCopy\nPaste");

    /* text='Menu' overrides the displayed option */
    lv_obj_t * lv_dropdown_1 = lv_dropdown_create(screen);
    lv_obj_set_align(lv_dropdown_1, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_pos(lv_dropdown_1, -14, 46);
    lv_obj_set_width(lv_dropdown_1, 110);
    lv_dropdown_set_text(lv_dropdown_1, "Menu");
    lv_dropdown_set_options(lv_dropdown_1, "Cut\nCopy\nPaste");
}

