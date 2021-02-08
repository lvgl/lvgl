//#include "../../lv_examples.h"
//
///**
// * Demonstrate cell placement and span
// */
//void lv_example_grid_2(void)
//{
//    static lv_coord_t col_dsc[3] = {80, 80, 80};
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
//
//    /*Cell to 0;0 and align to to the start (left/top) horizontally and vertically too */
//    obj = lv_obj_create(cont, NULL);
//    lv_obj_set_size(obj, LV_SIZE_AUTO, LV_SIZE_AUTO);
//    lv_obj_set_grid_cell(obj, LV_GRID_CELL_START(0, 1), LV_GRID_CELL_START(0, 1));
//    label = lv_label_create(obj, NULL);
//    lv_label_set_text(label, "c0, r0");
//
//    /*Cell to 1;0 and align to to the start (left) horizontally and center vertically too */
//    obj = lv_obj_create(cont, NULL);
//    lv_obj_set_size(obj, LV_SIZE_AUTO, LV_SIZE_AUTO);
//    lv_obj_set_grid_cell(obj, LV_GRID_CELL_START(1, 1), LV_GRID_CELL_CENTER(0, 1));
//    label = lv_label_create(obj, NULL);
//    lv_label_set_text(label, "c1, r0");
//
//    /*Cell to 2;0 and align to to the start (left) horizontally and end (bottom) vertically too */
//    obj = lv_obj_create(cont, NULL);
//    lv_obj_set_size(obj, LV_SIZE_AUTO, LV_SIZE_AUTO);
//    lv_obj_set_grid_cell(obj, LV_GRID_CELL_START(2, 1), LV_GRID_CELL_END(0, 1));
//    label = lv_label_create(obj, NULL);
//    lv_label_set_text(label, "c2, r0");
//
//    /*Cell to 1;1 but 2 column wide (span = 2).Set width and height to stretched. */
//    obj = lv_obj_create(cont, NULL);
//    lv_obj_set_size(obj, LV_SIZE_AUTO, LV_SIZE_AUTO);
//    lv_obj_set_grid_cell(obj, LV_GRID_CELL_STRETCH(1, 2), LV_GRID_CELL_STRETCH(1, 1));
//    label = lv_label_create(obj, NULL);
//    lv_label_set_text(label, "c1-2, r1");
//
//    /*Cell to 0;1 but 2 rows tall (span = 2).Set width and height to stretched. */
//    obj = lv_obj_create(cont, NULL);
//    lv_obj_set_size(obj, LV_SIZE_AUTO, LV_SIZE_AUTO);
//    lv_obj_set_grid_cell(obj, LV_GRID_CELL_STRETCH(0, 1), LV_GRID_CELL_STRETCH(1, 2));
//    label = lv_label_create(obj, NULL);
//    lv_label_set_text(label, "c0\nr1-2");
//}
//
