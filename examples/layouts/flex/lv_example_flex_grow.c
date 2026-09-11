/**
 * @file lv_example_flex_grow.c
 */

#include "../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * @title Flex grow
 * @brief Distribute leftover row space by weight.
 *
 * Three buttons share a row: the first has a fixed width and the other two use flex_grow.
 * The free space left in the row is split in proportion to the grow weights, so the
 * grow=2 item receives twice as much extra width as the grow=1 item.
 */
void lv_example_flex_grow(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* Demo row for grow behavior */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(container, lv_pct(90), LV_SIZE_CONTENT);
    /* Fixed-width item on the left */
    lv_obj_t * button_1 = lv_button_create(container);
    lv_obj_set_size(button_1, 50, LV_SIZE_CONTENT);
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_label_set_text(label_1, "Fix");
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);

    /* Grows with weight 1 */
    lv_obj_t * button_2 = lv_button_create(container);
    lv_obj_set_height(button_2, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(button_2, 1);
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_label_set_text(label_2, "Grow x1");
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);

    /* Grows with weight 2 — receives twice as much free space */
    lv_obj_t * button_3 = lv_button_create(container);
    lv_obj_set_height(button_3, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(button_3, 2);
    lv_obj_t * label_3 = lv_label_create(button_3);
    lv_label_set_text(label_3, "Grow x2");
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
}
#endif
