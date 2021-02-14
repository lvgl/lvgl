#include "../../../lvgl.h"
#if LV_USE_KEYBOARD && LV_BUILD_EXAMPLES

static void ta_event_cb(lv_obj_t * ta, lv_event_t e)
{
    lv_obj_t * kb = lv_event_get_user_data();
    if(e == LV_EVENT_FOCUSED) {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }

    if(e == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

void lv_keyboard_example_1(void)
{
    /*Create a keyboard to use it with an of the text areas*/
    lv_obj_t *kb = lv_keyboard_create(lv_scr_act());

    /*Create a text area. The keyboard will write here*/
    lv_obj_t * ta;
    ta = lv_textarea_create(lv_scr_act(), NULL);
    lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);
    lv_obj_add_event_cb(ta, ta_event_cb, kb);
    lv_textarea_set_placeholder_text(ta, "Hello");

    ta = lv_textarea_create(lv_scr_act(), NULL);
    lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 10);
    lv_obj_add_event_cb(ta, ta_event_cb, kb);

    lv_keyboard_set_textarea(kb, ta);
}
#endif
