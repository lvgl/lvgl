//#include "../../lv_examples.h"
//
///**
// * RTL base direction changes order of the items.
// */
//void lv_ex_flex_6(void)
//{
//    lv_obj_t * cont = lv_obj_create(lv_scr_act(), NULL);
//    lv_obj_set_base_dir(cont, LV_BIDI_DIR_RTL);
//    lv_obj_set_size(cont, 300, 220);
//    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
//    lv_obj_set_flex_dir(cont, LV_FLEX_DIR_COLUMN_WRAP);
//
//    uint32_t i;
//    for(i = 0; i < 20; i++) {
//        lv_obj_t * obj = lv_obj_create(cont, NULL);
//        lv_obj_set_size(obj, 80, LV_SIZE_AUTO);
//        lv_obj_set_flex_item(obj, true);
//
//        lv_obj_t * label = lv_label_create(obj, NULL);
//        lv_label_set_text_fmt(label, "%d", i);
//        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
//    }
//}
//
