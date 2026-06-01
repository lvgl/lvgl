/**
 * @file lv_example_flex_ignore_layout.c
 */

#include "../../lv_examples.h"
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
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* Demo row mixing normal and ignore_layout items.
       The red item uses ignore_layout="true", so Flex skips it;
       it keeps its manual x/y position and the other items are laid out normally. */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(container, lv_pct(90), 150);
    lv_obj_t * button_1 = lv_button_create(container);
    lv_obj_set_height(button_1, LV_SIZE_CONTENT);
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_label_set_text(label_1, "Item A");
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);

    /* This item is invisible to flex. It centers itself by its own
       align and does not occupy a flex slot. */
    lv_obj_t * button_2 = lv_button_create(container);
    lv_obj_set_align(button_2, LV_ALIGN_CENTER);
    lv_obj_set_flag(button_2, LV_OBJ_FLAG_IGNORE_LAYOUT, true);
    lv_obj_set_style_bg_color(button_2, lv_color_hex(0xe74c3c), 0);
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_label_set_text(label_2, "I'm Free!");
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);

    lv_obj_t * button_3 = lv_button_create(container);
    lv_obj_t * label_3 = lv_label_create(button_3);
    lv_label_set_text(label_3, "Item B");
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);

    lv_obj_t * button_4 = lv_button_create(container);
    lv_obj_set_height(button_4, LV_SIZE_CONTENT);
    lv_obj_t * label_4 = lv_label_create(button_4);
    lv_label_set_text(label_4, "Item C");
    lv_obj_set_align(label_4, LV_ALIGN_CENTER);
}
#endif
