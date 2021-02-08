//#include "../../lv_examples.h"
//
///**
// * A simple row and a column layout with flexbox
// */
//void lv_example_flex_1(void)
//{
//    /*Create a container with ROW flex direction*/
//    lv_obj_t * cont_row = lv_obj_create(lv_scr_act(), NULL);
//    lv_obj_set_size(cont_row, 300, 75);
//    lv_obj_align(cont_row, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
//    lv_obj_set_flex_dir(cont_row, LV_FLEX_DIR_ROW);
//    lv_obj_set_flex_gap(cont_row, 10);
//
//    /*Create a container with COLUMN flex direction*/
//    lv_obj_t * cont_col = lv_obj_create(lv_scr_act(), NULL);
//    lv_obj_set_size(cont_col, 200, 150);
//    lv_obj_align(cont_col, cont_row, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
//    lv_obj_set_flex_dir(cont_col, LV_FLEX_DIR_COLUMN);
//    lv_obj_set_flex_gap(cont_col, 10);
//
//    uint32_t i;
//    for(i = 0; i < 10; i++) {
//        /*Add items to the row*/
//        lv_obj_t * obj1 = lv_obj_create(cont_row, NULL);
//        lv_obj_set_size(obj1, 100, LV_COORD_PCT(100));
//        lv_obj_set_flex_item(obj1, true);
//
//        lv_obj_t * label1 = lv_label_create(obj1, NULL);
//        lv_label_set_text_fmt(label1, "Item: %d", i);
//        lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
//
//        /*Add items to the column*/
//        lv_obj_t * obj2 = lv_obj_create(cont_col, NULL);
//        lv_obj_set_size(obj2, LV_COORD_PCT(100), LV_SIZE_AUTO);
//        lv_obj_set_flex_item(obj2, true);
//
//        lv_obj_t * label3 = lv_label_create(obj2, NULL);
//        lv_label_set_text_fmt(label3, "Item: %d", i);
//        lv_obj_align(label3, NULL, LV_ALIGN_CENTER, 0, 0);
//    }
//}
//
