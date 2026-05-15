/**
 * @file lv_example_button_basic.c
 */

#include "../../../../lvgl.h"

/**
 * @title Button basics
 * @brief Two buttons showing default content-sized sizing and an explicit width.
 *
 * The first button uses default sizing so it wraps tightly around its label. The second
 * button sets an explicit width and height, illustrating that a button is just a
 * clickable base widget with a label child.
 */
void lv_example_button_basic_create(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Resize one button or change its label to see how content-sizing reacts. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Button: basics");

    /* Default size driven by label content */
    lv_obj_t * lv_button_0 = lv_button_create(screen);
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "Click me");

    /* Explicit width and height */
    lv_obj_t * lv_button_1 = lv_button_create(screen);
    lv_obj_set_size(lv_button_1, 160, 48);
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "Wide button");
}

