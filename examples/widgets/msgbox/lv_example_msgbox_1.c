#include "../../../lvgl.h"
#if LV_USE_MSGBOX && LV_BUILD_EXAMPLES

static void event_cb(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Button %s clicked", lv_msgbox_get_active_btn_text(obj));
    }
}

void lv_example_msgbox_1(void)
{
    static const char * btns[] ={"Apply", "Close", ""};

    lv_obj_t * mbox1 = lv_msgbox_create("Hello", "This is a message box with two buttons.", btns, true);
    lv_obj_add_event_cb(mbox1, event_cb, NULL);
    lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
}

#endif
