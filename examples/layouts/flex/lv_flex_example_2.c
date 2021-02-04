//#include "../../lv_examples.h"
//
///**
// * Arrange items in column with wrap and place the row to get even space around them.
// */
//void lv_ex_flex_2(void)
//{
//    lv_obj_t * cont = lv_obj_create(lv_scr_act(), NULL);
//    lv_obj_set_size(cont, 300, 220);
//    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
//    lv_obj_set_flex_dir(cont, LV_FLEX_DIR_COLUMN_WRAP);
//    lv_obj_set_flex_place(cont, LV_FLEX_PLACE_START, LV_FLEX_PLACE_START);
//
//    uint32_t i;
//    for(i = 0; i < 3; i++) {
//        lv_obj_t * obj = lv_obj_create(cont, NULL);
//        lv_obj_set_flex_item_place(obj, LV_FLEX_PLACE_STRETCH);
//        lv_obj_set_size(obj, 70, LV_SIZE_AUTO);
//
//        lv_obj_t * label = lv_label_create(obj, NULL);
//        lv_label_set_text_fmt(label, "%d", i);
//    }
//}
//
