#include "../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * @title Row wrap with even spacing
 * @brief A 300x220 container lays out eight checkable tiles in wrapping rows with even spacing.
 *
 * A reusable `lv_style_t` sets `LV_LAYOUT_FLEX`, `LV_FLEX_FLOW_ROW_WRAP`, and
 * `LV_FLEX_ALIGN_SPACE_EVENLY` on the main axis. The style is applied to a
 * centered container holding eight 70 px-wide tiles with `LV_SIZE_CONTENT`
 * height. Each tile carries `LV_OBJ_FLAG_CHECKABLE` so it toggles between
 * default and `LV_STATE_CHECKED` when clicked.
 */
void lv_example_flex_2(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_flex_flow(&style, LV_FLEX_FLOW_ROW_WRAP);
    lv_style_set_flex_main_place(&style, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_style_set_layout(&style, LV_LAYOUT_FLEX);

    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, 300, 220);
    lv_obj_center(cont);
    lv_obj_add_style(cont, &style, 0);

    uint32_t i;
    for(i = 0; i < 8; i++) {
        lv_obj_t * obj = lv_obj_create(cont);
        lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "%" LV_PRIu32, i);
        lv_obj_center(label);
    }
}

#endif
