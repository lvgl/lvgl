//#include "../../../lvgl.h"
//#if LV_USE_MSGBOX
//
//static void mbox_event_cb(lv_obj_t *obj, lv_event_t evt);
//static void btn_event_cb(lv_obj_t *btn, lv_event_t evt);
//static void opa_anim(void * bg, lv_anim_value_t v);
//
//static lv_obj_t *mbox, *info;
//static lv_style_t style_modal;
//
//static const char welcome_info[] = "Welcome to the modal message box demo!\n"
//        "Press the button to display a message box.";
//
//static const char in_msg_info[] = "Notice that you cannot touch "
//        "the button again while the message box is open.";
//
//void lv_ex_msgbox_2(void)
//{
//    lv_style_init(&style_modal);
//    lv_style_set_bg_color(&style_modal, LV_STATE_DEFAULT, LV_COLOR_BLACK);
//
//    /* Create a button, then set its position and event callback */
//    lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
//    lv_obj_set_size(btn, 200, 60);
//    lv_obj_set_event_cb(btn, btn_event_cb);
//    lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 20);
//
//    /* Create a label on the button */
//    lv_obj_t *label = lv_label_create(btn, NULL);
//    lv_label_set_text(label, "Display a message box!");
//
//    /* Create an informative label on the screen */
//    info = lv_label_create(lv_scr_act(), NULL);
//    lv_label_set_text(info, welcome_info);
//    lv_label_set_long_mode(info, LV_LABEL_LONG_BREAK); /* Make sure text will wrap */
//    lv_obj_set_width(info, LV_HOR_RES - 10);
//    lv_obj_align(info, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 5, -5);
//
//}
//
//static void mbox_event_cb(lv_obj_t *obj, lv_event_t evt)
//{
//    if(evt == LV_EVENT_DELETE && obj == mbox) {
//        /* Delete the parent modal background */
//        lv_obj_del_async(lv_obj_get_parent(mbox));
//        mbox = NULL; /* happens before object is actually deleted! */
//        lv_label_set_text(info, welcome_info);
//    } else if(evt == LV_EVENT_VALUE_CHANGED) {
//        /* A button was clicked */
//        lv_msgbox_start_auto_close(mbox, 0);
//    }
//}
//
//static void btn_event_cb(lv_obj_t *btn, lv_event_t evt)
//{
//    if(evt == LV_EVENT_CLICKED) {
//        /* Create a full-screen background */
//
//        /* Create a base object for the modal background */
//        lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
//        lv_obj_reset_style_list(obj, LV_OBJ_PART_MAIN);
//        lv_obj_add_style(obj, LV_OBJ_PART_MAIN, &style_modal);
//        lv_obj_set_pos(obj, 0, 0);
//        lv_obj_set_size(obj, LV_HOR_RES, LV_VER_RES);
//
//        static const char * btns2[] = {"Ok", "Cancel", ""};
//
//        /* Create the message box as a child of the modal background */
//        mbox = lv_msgbox_create(obj, NULL);
//        lv_msgbox_add_btns(mbox, btns2);
//        lv_msgbox_set_text(mbox, "Hello world!");
//        lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
//        lv_obj_set_event_cb(mbox, mbox_event_cb);
//
//        /* Fade the message box in with an animation */
//        lv_anim_t a;
//        lv_anim_init(&a);
//        lv_anim_set_var(&a, obj);
//        lv_anim_set_time(&a, 500);
//        lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_50);
//        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)opa_anim);
//        lv_anim_start(&a);
//
//        lv_label_set_text(info, in_msg_info);
//        lv_obj_align(info, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 5, -5);
//    }
//}
//
//static void opa_anim(void * bg, lv_anim_value_t v)
//{
//    lv_obj_set_style_local_bg_opa(bg, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, v);
//}
//
//#endif
