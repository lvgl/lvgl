#include "../../../lvgl.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * Arrange items in rows with wrap and place the items to get even space around them.
 */
void lv_example_flex_2(void)
{
    static lv_flex_t flex_row_wrap;
    lv_flex_init(&flex_row_wrap);
    lv_flex_set_flow(&flex_row_wrap, LV_FLEX_FLOW_ROW_WRAP);
    lv_flex_set_place(&flex_row_wrap, LV_FLEX_PLACE_SPACE_EVENLY, LV_FLEX_PLACE_START, LV_FLEX_PLACE_START);

    lv_obj_t * cont = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(cont, 300, 220);
    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_layout(cont, &flex_row_wrap);

    uint32_t i;
    for(i = 0; i < 8; i++) {
        lv_obj_t * obj = lv_obj_create(cont, NULL);
        lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);

        lv_obj_t * label = lv_label_create(obj, NULL);
        lv_label_set_text_fmt(label, "%d", i);
    }
}

#endif
