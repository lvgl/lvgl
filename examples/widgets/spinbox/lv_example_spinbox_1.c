#include "../../../lvgl.h"
#if LV_USE_SPINBOX && LV_BUILD_EXAMPLES

static lv_obj_t * spinbox;


static void lv_spinbox_increment_event_cb(lv_obj_t * btn, lv_event_t e)
{
    LV_UNUSED(btn);
    if(e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_increment(spinbox);
    }
}

static void lv_spinbox_decrement_event_cb(lv_obj_t * btn, lv_event_t e)
{
    LV_UNUSED(btn);
    if(e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_decrement(spinbox);
    }
}


void lv_example_spinbox_1(void)
{
    spinbox = lv_spinbox_create(lv_scr_act());
    lv_spinbox_set_range(spinbox, -1000, 25000);
    lv_spinbox_set_digit_format(spinbox, 5, 2);
    lv_spinbox_step_prev(spinbox);
    lv_obj_set_width(spinbox, 100);
    lv_obj_align(spinbox, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_coord_t h = lv_obj_get_height(spinbox);

    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(btn, h, h);
    lv_obj_align(btn, spinbox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_content_text(btn, LV_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_PLUS);
    lv_obj_add_event_cb(btn, lv_spinbox_increment_event_cb, NULL);

    btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(btn, h, h);
    lv_obj_align(btn, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_content_text(btn, LV_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_MINUS);
    lv_obj_add_event_cb(btn, lv_spinbox_decrement_event_cb, NULL);
}

#endif
