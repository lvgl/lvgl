//#include "../../lv_examples.h"
//
///**
// * Demonstrate margin in grid
// */
//void lv_example_grid_5(void)
//{
//
//    /*60x60 cells*/
//    static lv_coord_t col_dsc[3] = {100, 60, 60};
//    static lv_coord_t row_dsc[3] = {60, 60, 60};
//
//    static lv_grid_t grid;
//    lv_grid_init(&grid);
//    lv_grid_set_template(&grid, col_dsc, 3, row_dsc, 3);
//
//    /*Create a container with grid*/
//    lv_obj_t * cont = lv_obj_create(lv_scr_act(), NULL);
//    lv_obj_set_size(cont, 300, 220);
//    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
//    lv_obj_set_grid(cont, &grid);
//
//    lv_obj_t * label;
//    lv_obj_t * obj;
//    uint32_t i;
//    for(i = 0; i < 9; i++) {
//        uint8_t col = i % 3;
//        uint8_t row = i / 3;
//
//        obj = lv_obj_create(cont, NULL);
//        lv_obj_set_grid_cell(obj, LV_GRID_CELL_START(col, 1), LV_GRID_CELL_START(row, 1));
//        lv_obj_set_size(obj, 55, 55);
//        if(i == 1) {
//            lv_obj_set_style_local_margin_all(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 5);
//        }
//        label = lv_label_create(obj, NULL);
//        lv_label_set_text_fmt(label, "%d,%d", col, row);
//        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
//    }
//}
//
