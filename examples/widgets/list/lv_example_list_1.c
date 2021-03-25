#include "../../../lvgl.h"
#if LV_USE_LIST && LV_BUILD_EXAMPLES
static lv_obj_t * list1;

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked: %s", lv_list_get_btn_text(list1, obj));
    }
}
void lv_example_list_1(void)
{
    /*Create a list*/
    list1 = lv_list_create(lv_scr_act());
    lv_obj_set_size(list1, 180, 220);
    lv_obj_align(list1, NULL, LV_ALIGN_CENTER, 0, 0);


    uint32_t t = lv_tick_get();
    int i;
    for(i = 0; i < 3; i++) {
//        lv_btn_create(lv_scr_act());
        lv_list_add_btn(list1, LV_SYMBOL_FILE, "New", event_handler);
    }

    printf("%d\n", t);

    return;
    /*Add buttons to the list*/
    lv_list_add_text(list1, "File");
    lv_list_add_btn(list1, LV_SYMBOL_FILE, "New", event_handler);
    lv_list_add_btn(list1, LV_SYMBOL_DIRECTORY, "Open", event_handler);
    lv_list_add_btn(list1, LV_SYMBOL_SAVE, "Save", event_handler);
    lv_list_add_btn(list1, LV_SYMBOL_CLOSE, "Delete", event_handler);
    lv_list_add_btn(list1, LV_SYMBOL_EDIT, "Edit", event_handler);
    lv_list_add_text(list1, "Connectivity");
    lv_list_add_btn(list1, LV_SYMBOL_BLUETOOTH, "Bluetooth", event_handler);
    lv_list_add_btn(list1, LV_SYMBOL_GPS, "Navigation", event_handler);
    lv_list_add_btn(list1, LV_SYMBOL_USB, "USB", event_handler);
    lv_list_add_btn(list1, LV_SYMBOL_BATTERY_FULL, "Battery", event_handler);
    lv_list_add_text(list1, "Exit");
    lv_list_add_btn(list1, LV_SYMBOL_OK, "Apply", event_handler);
    lv_list_add_btn(list1, LV_SYMBOL_CLOSE, "Close", event_handler);
}

#endif
