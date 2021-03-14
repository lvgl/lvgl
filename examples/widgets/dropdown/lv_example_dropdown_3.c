#include "../../../lvgl.h"
#if LV_USE_DROPDOWN && LV_BUILD_EXAMPLES

static void event_cb(lv_obj_t * dropdown, lv_event_t e)
{
    if(e == LV_EVENT_VALUE_CHANGED) {
        char buf[64];
        lv_dropdown_get_selected_str(dropdown, buf, sizeof(buf));
        LV_LOG_USER("'%s' is selected", buf);
    }
}

/**
 * Create a menu from a drop-down list and show some drop-down list features and styling
 */
void lv_example_dropdown_3(void)
{
    /*Create a drop down list*/
    lv_obj_t * dropdown = lv_dropdown_create(lv_scr_act(), NULL);
    lv_obj_align(dropdown, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);
    lv_dropdown_set_options(dropdown, "New project\n"
                                      "New file\n"
                                      "Open project\n"
                                      "Recent projects\n"
                                      "Preferences\n"
                                      "Exit");

    /*Set a fixed text to display on the button of the drop-down list*/
    lv_dropdown_set_text(dropdown, "Menu");

    /*Use a custom image as down icon and flip it when the list is opened*/
    LV_IMG_DECLARE(img_caret_down)
    lv_dropdown_set_symbol(dropdown, &img_caret_down);
    lv_obj_set_style_transform_angle(dropdown, LV_PART_MAIN, LV_STATE_CHECKED, 1800);

    /*In a menu we don't need to show the last clicked item*/
    lv_dropdown_set_selected_highlight(dropdown, false);

    lv_obj_add_event_cb(dropdown, event_cb, NULL);
}

#endif
