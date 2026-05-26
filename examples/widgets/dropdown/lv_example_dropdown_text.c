/**
 * @file lv_example_dropdown_text.c
 */

#include "../../lv_examples.h"
#if LV_USE_DROPDOWN && LV_BUILD_EXAMPLES

/**
 * @title Dropdown fixed text
 * @brief Pin the dropdown button label regardless of selection.
 *
 * Without a text attribute the dropdown shows the currently selected option (left). With
 * text="Menu" the button always displays that string while the option list remains
 * intact, which is useful for icon-style or fixed-label menus.
 */
void lv_example_dropdown_text(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* Without text the button reflects the selected option */
    lv_obj_t * dropdown_1 = lv_dropdown_create(screen);
    lv_obj_set_align(dropdown_1, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(dropdown_1, 14);
    lv_obj_set_width(dropdown_1, 110);
    lv_dropdown_set_options(dropdown_1, "Cut\nCopy\nPaste");

    /* text='Menu' overrides the displayed option */
    lv_obj_t * dropdown_2 = lv_dropdown_create(screen);
    lv_obj_set_align(dropdown_2, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(dropdown_2, -14);
    lv_obj_set_width(dropdown_2, 110);
    lv_dropdown_set_text(dropdown_2, "Menu");
    lv_dropdown_set_options(dropdown_2, "Cut\nCopy\nPaste");
}
#endif
