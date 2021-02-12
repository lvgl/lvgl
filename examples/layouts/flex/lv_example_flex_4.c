#include "../../../lvgl.h"

#if LV_USE_FLEX

/**
 * Reverse the order of flex items
 */
void lv_example_flex_4(void)
{

    static lv_flex_t flex_col_rev;
    lv_flex_init(&flex_col_rev);
    lv_flex_set_flow(&flex_col_rev, LV_FLEX_FLOW_COLUMN_WRAP_REVERSE);

    lv_obj_t * cont = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont, 300, 220);
    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_layout(cont, &flex_col_rev);

    uint32_t i;
    for(i = 0; i < 6; i++) {
        lv_obj_t * obj = lv_obj_create(cont, NULL);
        lv_obj_set_size(obj, 100, 30);

        lv_obj_t * label = lv_label_create(obj, NULL);
        lv_label_set_text_fmt(label, "Item: %d", i);
    }
}

#endif
