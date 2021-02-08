#include "../../../lvgl.h"
#include <stdio.h>
#if LV_USE_DROPDOWN


/**
 * Create a menu from a drop-down list and show some drop-down list features and styling
 */
void lv_example_dropdown_3(void)
{
    /*Create a drop down list*/
    lv_obj_t * dd = lv_dropdown_create(lv_scr_act(), NULL);
    lv_obj_align(dd, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 10);
    lv_dropdown_set_options(dd, "New\n"
                                "Open\n"
                                "Edit\n"
                                "Close\n"
                                "Preferences\n"
                                "Exit");

    /*Set a fixed text to display on the button of the drop-down list*/
    lv_dropdown_set_text(dd, "Menu");

    /*Use a custom image as down icon*/
    LV_IMG_DECLARE(img_caret_down)
    lv_dropdown_set_symbol(dd, &img_caret_down);

    /* Remove the style of the selected part on the list.
     * In a menu we don't need to show the last clicked item*/
    lv_obj_t * list = lv_dropdown_get_list(dd);
    lv_obj_remove_style(list, LV_PART_SELECTED, LV_STATE_DEFAULT, NULL);
}

#endif
