#include "../../../lvgl.h"
#if LV_USE_GRID && LV_BUILD_EXAMPLES

/**
 * Demonstrate track placement
 */
void lv_example_grid_4(void)
{
    static lv_coord_t col_dsc[3] = {60, 60, 60};
    static lv_coord_t row_dsc[3] = {40, 40, 40};

    static lv_grid_t grid;
    lv_grid_init(&grid);
    lv_grid_set_template(&grid, col_dsc, 3, row_dsc, 3);

    /*Add space between the columns and move the rows to the bottom (end)*/
    lv_grid_set_place(&grid, LV_GRID_SPACE_BETWEEN, LV_GRID_END);

    /*Create a container with grid*/
    lv_obj_t * cont = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont, 300, 220);
    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_layout(cont, &grid);

    lv_obj_t * label;
    lv_obj_t * obj;
    uint32_t i;
    for(i = 0; i < 9; i++) {
        uint8_t col = i % 3;
        uint8_t row = i / 3;

        obj = lv_obj_create(cont, NULL);
        /*Stretch the cell horizontally and vertically too
         *Set span to 1 to make the cell 1 column/row sized*/
        lv_obj_set_grid_cell(obj, LV_GRID_STRETCH, col, 1,
                                  LV_GRID_STRETCH, row, 1);

        label = lv_label_create(obj, NULL);
        lv_label_set_text_fmt(label, "%d,%d", col, row);
        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    }
}

#endif
