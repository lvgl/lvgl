#include "../../../lvgl.h"
#if LV_USE_BTN && LV_BUILD_EXAMPLES

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked\n");
    }
    else if(event == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled\n");
    }
}

void lv_example_btn_1(void)
{
    lv_obj_t * label;

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_add_event_cb(btn1, event_handler, NULL);
    lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -40);

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Button");

    lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_add_event_cb(btn2, event_handler, NULL);
    lv_obj_align(btn2, NULL, LV_ALIGN_CENTER, 0, 40);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "Toggle");
}
#endif
