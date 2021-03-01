#include "../../lvgl.h"
#if LV_BUILD_EXAMPLES && LV_USE_LIST

static uint32_t btn_cnt = 1;

static void float_btn_event_cb(lv_obj_t * float_btn, lv_event_t e)
{
    if(e == LV_EVENT_CLICKED) {
        lv_obj_t * list = lv_event_get_user_data();
        char buf[32];
        lv_snprintf(buf, sizeof(buf), "Track %d", btn_cnt);
        lv_obj_t * list_btn = lv_list_add_btn(list, LV_SYMBOL_AUDIO, buf, NULL);
        btn_cnt++;
        lv_obj_scroll_to_view(list_btn, LV_ANIM_ON);
        lv_obj_move_foreground(float_btn);
    }
}

/**
 * Create a list a with a floating button
 */
void lv_example_scroll_3(void)
{
    lv_obj_t * list = lv_list_create(lv_scr_act());
    lv_obj_set_size(list, 280, 220);
    lv_obj_center(list);

    for(btn_cnt = 1; btn_cnt <= 2; btn_cnt++) {
        char buf[32];
        lv_snprintf(buf, sizeof(buf), "Track %d", btn_cnt);
        lv_list_add_btn(list, LV_SYMBOL_AUDIO, buf, NULL);
    }

    lv_obj_t * float_btn = lv_btn_create(list, NULL);
    lv_obj_set_size(float_btn, 50, 50);
    lv_obj_add_flag(float_btn, LV_OBJ_FLAG_FLOATING);
    lv_obj_align(float_btn, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, -lv_obj_get_style_pad_right(list, LV_PART_MAIN));
    lv_obj_add_event_cb(float_btn, float_btn_event_cb, list);
    lv_obj_set_style_radius(float_btn, LV_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
    lv_obj_set_style_content_text(float_btn, LV_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_PLUS);
    lv_obj_set_style_content_font(float_btn, LV_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_large(float_btn));
}

#endif
