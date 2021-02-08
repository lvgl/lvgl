//#include "../../lv_examples.h"
//
///**
// * Arrange items in a row and demonstrate flex grow.
// */
//void lv_example_flex_3(void)
//{
//    lv_obj_t * cont = lv_obj_create(lv_scr_act(), NULL);
//    lv_obj_set_size(cont, 300, 220);
//    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
//    lv_obj_set_flex_dir(cont, LV_FLEX_DIR_ROW);
//
//    lv_obj_t * obj;
//    obj = lv_obj_create(cont, NULL);
//    lv_obj_set_size(obj, 20, 20);               /*Fix size*/
//    lv_obj_set_flex_item(obj, true);
//
//    obj = lv_obj_create(cont, NULL);
//    lv_obj_set_size(obj, LV_FLEX_GROW(1), 30);  /*1 portion from the free space*/
//    lv_obj_set_flex_item(obj, true);
//
//    obj = lv_obj_create(cont, NULL);
//    lv_obj_set_size(obj, LV_FLEX_GROW(2), 40);  /*2 portion from the free space*/
//    lv_obj_set_flex_item(obj, true);
//
//    obj = lv_obj_create(cont, NULL);
//    lv_obj_set_size(obj, 20, 20);               /*Fix size. It is flushed to the right by the "grow" items*/
//    lv_obj_set_flex_item(obj, true);
//}
//
