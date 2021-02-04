#include "../../../lvgl.h"
#include <stdio.h>
#if LV_USE_DROPDOWN


/**
 * Create a drop down, up, left and right menus
 */
void lv_ex_dropdown_2(void)
{
    static const char * opts = "Apple\n"
                               "Banana\n"
                               "Orange\n"
                               "Melon\n"
                               "Grape\n"
                               "Raspberry";

    lv_obj_t * dd;
    dd = lv_dropdown_create(lv_scr_act(), NULL);
    lv_dropdown_set_options_static(dd, opts);
    lv_obj_align(dd, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

    dd = lv_dropdown_create(lv_scr_act(), NULL);
    lv_dropdown_set_options_static(dd, opts);
    lv_dropdown_set_dir(dd, LV_DIR_BOTTOM);
    lv_dropdown_set_symbol(dd, LV_SYMBOL_UP);
    lv_obj_align(dd, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);

    dd = lv_dropdown_create(lv_scr_act(), NULL);
    lv_dropdown_set_options_static(dd, opts);
    lv_dropdown_set_dir(dd, LV_DIR_RIGHT);
    lv_dropdown_set_symbol(dd, LV_SYMBOL_RIGHT);
    lv_obj_align(dd, NULL, LV_ALIGN_IN_LEFT_MID, 10, 0);

    dd = lv_dropdown_create(lv_scr_act(), NULL);
    lv_dropdown_set_options_static(dd, opts);
    lv_dropdown_set_dir(dd, LV_DIR_LEFT);
    lv_dropdown_set_symbol(dd, LV_SYMBOL_LEFT);
    lv_obj_align(dd, NULL, LV_ALIGN_IN_RIGHT_MID, -10, 0);
}

#endif
