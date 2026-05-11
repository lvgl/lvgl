#include "../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * @title Reversed column flex order
 * @brief Stack six items bottom-up inside a centered container.
 *
 * A 300x220 container uses `LV_FLEX_FLOW_COLUMN_REVERSE`, so items added later
 * in the loop appear higher on screen. Six 100x50 children numbered 0 through
 * 5 are added in ascending order but rendered in reverse, with "Item: 0"
 * sitting at the bottom of the container.
 */
void lv_example_flex_4(void)
{

    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, 300, 220);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN_REVERSE);

    uint32_t i;
    for(i = 0; i < 6; i++) {
        lv_obj_t * obj = lv_obj_create(cont);
        lv_obj_set_size(obj, 100, 50);

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "Item: %" LV_PRIu32, i);
        lv_obj_center(label);
    }
}

#endif
