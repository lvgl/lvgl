#include "../../lv_examples.h"
#if LV_USE_BUTTONMATRIX && LV_BUILD_EXAMPLES

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target_obj(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_buttonmatrix_get_selected_button(obj);
        const char * txt = lv_buttonmatrix_get_button_text(obj, id);
        LV_UNUSED(txt);
        LV_LOG_USER("%s was pressed\n", txt);
    }
}

static const char * btnm_map[] = {"1", "2", "3", "4", "5", "\n",
                                  "6", "7", "8", "9", "0", "\n",
                                  "Action1", "Action2", ""
                                 };

void lv_example_buttonmatrix_1(void)
{
    lv_obj_t * btnm1 = lv_buttonmatrix_create(lv_screen_active());
    lv_buttonmatrix_set_map(btnm1, btnm_map);
    lv_buttonmatrix_set_button_width(btnm1, 10, 2);        /*Make "Action1" twice as wide as "Action2"*/
    lv_buttonmatrix_set_button_ctrl(btnm1, 10, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(btnm1, 11, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_obj_align(btnm1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btnm1, event_handler, LV_EVENT_ALL, NULL);
}

#endif
