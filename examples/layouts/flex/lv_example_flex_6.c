#include "../../../lvgl.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * RTL base direction changes order of the items.
 * Also demonstrate how horizontal scrolling works with RTL.
 */
void lv_example_flex_6(void)
{
    lv_obj_t * cont = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_base_dir(cont, LV_BIDI_DIR_RTL);
    lv_obj_set_size(cont, 300, 220);
    lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_layout(cont, &lv_flex_column_center);

    uint32_t i;
    for(i = 0; i < 20; i++) {
        lv_obj_t * obj = lv_obj_create(cont, NULL);
        lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);

        lv_obj_t * label = lv_label_create(obj, NULL);
        lv_label_set_text_fmt(label, "%d", i);
        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    }
}
#endif
