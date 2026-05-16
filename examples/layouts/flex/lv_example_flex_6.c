#include "../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * @title RTL flex wrap and scrolling
 * @brief Lay out twenty tiles in a right-to-left wrapping flex container.
 *
 * A 300x220 container has `LV_BASE_DIR_RTL` set on its main part, then
 * switches to `LV_FLEX_FLOW_ROW_WRAP`. Twenty 70 px tiles are placed in
 * ascending order; the RTL base direction flips the main axis so item 0 sits
 * at the top-right and rows fill leftward, and horizontal scrolling follows
 * the same reversed direction.
 */
void lv_example_flex_6(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_style_base_dir(cont, LV_BASE_DIR_RTL, 0);
    lv_obj_set_size(cont, 300, 220);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);

    uint32_t i;
    for(i = 0; i < 20; i++) {
        lv_obj_t * obj = lv_obj_create(cont);
        lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "%" LV_PRIu32, i);
        lv_obj_center(label);
    }
}
#endif
