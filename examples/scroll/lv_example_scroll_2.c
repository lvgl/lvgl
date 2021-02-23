#include "../../lvgl.h"
#if LV_BUILD_EXAMPLES

static void sw_event_cb(lv_obj_t * sw, lv_event_t e)
{
    if(e == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t * list = lv_event_get_user_data();

        if(lv_obj_has_state(sw, LV_STATE_CHECKED)) lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLL_ONE);
        else lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLL_ONE);
    }
}

/**
 * Show an example to scroll snap
 */
void lv_example_scroll_2(void)
{
    lv_obj_t * list = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(list, 280, 150);
    lv_obj_set_scroll_snap_align_x(list, LV_SCROLL_SNAP_ALIGN_CENTER);
    lv_obj_set_layout(list, &lv_flex_queue);

    uint32_t i;
    for(i = 0; i < 10; i++) {
        lv_obj_t * btn = lv_btn_create(list, NULL);
        lv_obj_set_size(btn, 150, 100);

        lv_obj_t * label = lv_label_create(btn, NULL);
        if(i == 3) {
            lv_label_set_text_fmt(label, "Panel %d\nno snap", i);
            lv_obj_clear_flag(btn, LV_OBJ_FLAG_SNAPABLE);
        } else {
            lv_label_set_text_fmt(label, "Panel %d", i);
        }

        lv_obj_center(label);
    }
    lv_obj_update_snap(list, LV_ANIM_ON);


    /*Switch between "One scroll" and "Normal scroll" mode*/
    lv_obj_t * sw = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_align(sw, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 10);
    lv_obj_add_event_cb(sw, sw_event_cb, list);
    lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label, "One scroll");
    lv_obj_align(label, sw, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

#endif
