/**
 * @file lv_example_flex_ignore_layout.c
 */

#include "../../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * @title Flex ignore layout
 * @brief Opt one item out of the flex flow.
 *
 * With ignore_layout="true", Flex skips the item entirely: it keeps its manual x/y
 * position and does not occupy a slot. The remaining items lay out as if it were not
 * there, which is useful for overlays, floating badges, or absolutely positioned helpers.
 */
void lv_example_flex_ignore_layout(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);

    /* 💡 Toggle ignore_layout on the red item: Flex skips it and it floats by its own align. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "IGNORE_LAYOUT\nOpt out of flex positioning");

    /* Demo row mixing normal and ignore_layout items.
       The red item uses ignore_layout="true", so Flex skips it;
       it keeps its manual x/y position and the other items are laid out normally. */
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(lv_obj_1, lv_pct(100), 150);
    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_1);
    lv_obj_set_height(lv_button_0, LV_SIZE_CONTENT);
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_0);
    lv_label_set_text(lv_label_1, "Item A");
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);

    /* This item is invisible to flex. It centers itself by its own
       align and does not occupy a flex slot. */
    lv_obj_t * lv_button_1 = lv_button_create(lv_obj_1);
    lv_obj_set_align(lv_button_1, LV_ALIGN_CENTER);
    lv_obj_set_flag(lv_button_1, LV_OBJ_FLAG_IGNORE_LAYOUT, true);
    lv_obj_set_style_bg_color(lv_button_1, lv_color_hex(0xe74c3c), 0);
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_1);
    lv_label_set_text(lv_label_2, "I'm Free!");
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);

    lv_obj_t * lv_button_2 = lv_button_create(lv_obj_1);
    lv_obj_t * lv_label_3 = lv_label_create(lv_button_2);
    lv_label_set_text(lv_label_3, "Item B");
    lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);

    lv_obj_t * lv_button_3 = lv_button_create(lv_obj_1);
    lv_obj_set_height(lv_button_3, LV_SIZE_CONTENT);
    lv_obj_t * lv_label_4 = lv_label_create(lv_button_3);
    lv_label_set_text(lv_label_4, "Item C");
    lv_obj_set_align(lv_label_4, LV_ALIGN_CENTER);
}
#endif
