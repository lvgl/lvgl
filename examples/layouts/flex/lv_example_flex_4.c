//#include "../../lv_examples.h"
//
///**
// * Reverse the order of flex items
// */
//void lv_example_flex_4(void)
//{
//    lv_obj_t * cont = lv_obj_create(lv_scr_act(), NULL);
//    lv_obj_set_size(cont, 300, 220);
//    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
//    lv_obj_set_flex_dir(cont, LV_FLEX_DIR_COLUMN_WRAP_REVERSE);
//    lv_obj_set_flex_gap(cont, 10);
//
//    uint32_t i;
//    for(i = 0; i < 20; i++) {
//        lv_obj_t * obj = lv_obj_create(cont, NULL);
//        lv_obj_set_size(obj, 100, LV_SIZE_AUTO);
//        lv_obj_set_flex_item(obj, true);
//
//        lv_obj_t * label = lv_label_create(obj, NULL);
//        lv_label_set_text_fmt(label, "Item: %d", i);
//    }
//}
//
