//#include "../../lv_examples.h"
//
///**
// * Demonstrate the effect of margin on flex item
// */
//void lv_ex_flex_5(void)
//{
//    lv_obj_t * cont = lv_obj_create(lv_scr_act(), NULL);
//    lv_obj_set_size(cont, 300, 220);
//    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
//    lv_obj_set_flex_dir(cont, LV_FLEX_DIR_ROW_WRAP);
//
//    uint32_t i;
//    for(i = 0; i < 20; i++) {
//        lv_obj_t * obj = lv_obj_create(cont, NULL);
//        lv_obj_set_size(obj, 100, LV_SIZE_AUTO);
//        lv_obj_set_flex_item(obj, true);
//
//        /*Set margin on every side*/
//        if(i == 4) {
//            lv_obj_set_style_local_margin_all(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 20);
//        }
//
//        lv_obj_t * label = lv_label_create(obj, NULL);
//        lv_label_set_text_fmt(label, "Item:%d", i);
//    }
//}
//
