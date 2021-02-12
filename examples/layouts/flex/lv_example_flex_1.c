#include "../../../lvgl.h"

#if LV_USE_FLEX

/**
 * A simple row and a column layout with flexbox
 */
void lv_example_flex_1(void)
{
    static lv_flex_t flex_row;
    lv_flex_init(&flex_row);
    lv_flex_set_flow(&flex_row, LV_FLEX_FLOW_ROW);

    static lv_flex_t flex_col;
    lv_flex_init(&flex_col);
    lv_flex_set_flow(&flex_col, LV_FLEX_FLOW_COLUMN);

    /*Create a container with ROW flex direction*/
    lv_obj_t * cont_row = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont_row, 300, 75);
    lv_obj_align(cont_row, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
    lv_obj_set_layout(cont_row, &flex_row);

    /*Create a container with COLUMN flex direction*/
    lv_obj_t * cont_col = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont_col, 200, 150);
    lv_obj_align(cont_col, cont_row, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_layout(cont_col, &flex_col);

    uint32_t i;
    for(i = 0; i < 10; i++) {
        lv_obj_t * obj;
        lv_obj_t * label;

        /*Add items to the row*/
        obj= lv_obj_create(cont_row, NULL);
        lv_obj_set_size(obj, 100, LV_SIZE_PCT(100));

        label = lv_label_create(obj, NULL);
        lv_label_set_text_fmt(label, "Item: %d", i);
        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

        /*Add items to the column*/
        obj = lv_obj_create(cont_col, NULL);
        lv_obj_set_size(obj, LV_SIZE_PCT(100), LV_SIZE_CONTENT);

        label = lv_label_create(obj, NULL);
        lv_label_set_text_fmt(label, "Item: %d", i);
        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    }
}

#endif
