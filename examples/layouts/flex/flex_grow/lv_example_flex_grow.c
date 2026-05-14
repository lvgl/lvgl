/**
 * @file lv_example_flex_grow.c
 */

#include "../../../../lvgl.h"

/**
 * @title Flex grow
 * @brief Distribute leftover row space by weight.
 *
 * Three buttons share a row: the first has a fixed width and the other two use flex_grow.
 * The free space left in the row is split in proportion to the grow weights, so the
 * grow=2 item receives twice as much extra width as the grow=1 item.
 */
void lv_example_flex_grow_create(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);

    /* 💡 Adjust the flex_grow values (e.g. 1, 3, 5) to see how free space is distributed. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Flex grow: distribute remaining space proportionally");

    /* Demo row for grow behavior */
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(lv_obj_1, lv_pct(100), LV_SIZE_CONTENT);
    /* Fixed-width item on the left */
    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_1);
    lv_obj_set_size(lv_button_0, 50, LV_SIZE_CONTENT);
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_0);
    lv_label_set_text(lv_label_1, "Fix");
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);

    /* Grows with weight 1 */
    lv_obj_t * lv_button_1 = lv_button_create(lv_obj_1);
    lv_obj_set_height(lv_button_1, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(lv_button_1, 1);
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_1);
    lv_label_set_text(lv_label_2, "Grow x1");
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);

    /* Grows with weight 2 — receives twice as much free space */
    lv_obj_t * lv_button_2 = lv_button_create(lv_obj_1);
    lv_obj_set_height(lv_button_2, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(lv_button_2, 2);
    lv_obj_t * lv_label_3 = lv_label_create(lv_button_2);
    lv_label_set_text(lv_label_3, "Grow x2");
    lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);
}

