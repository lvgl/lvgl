#include "../../../lvgl.h"
#include <stdio.h>
#if LV_USE_MSGBOX

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        printf("Button: %s\n", lv_msgbox_get_active_btn_text(obj));
    }
}

void lv_ex_msgbox_1(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, LV_STATE_DEFAULT, 30);

    static const char * btns[] ={"Apply", "Close", ""};

    lv_obj_t * mbox1 = lv_msgbox_create("Hello", "This is a message box with two buttons.", btns, true);
//    lv_obj_set_width(mbox1, 300);
    lv_obj_set_event_cb(mbox1, event_handler);
    lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
}

#endif
