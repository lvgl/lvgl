/**
 * @file lv_example_spinbox_format.c
 */

#include "../../lv_examples.h"
#if LV_USE_SPINBOX && LV_BUILD_EXAMPLES

/**
 * @title Spinbox digit count and decimal point
 * @brief Control how the number is displayed: how many digits, and where the dot goes.
 *
 * `digit_count` is the *total* number of digits drawn (the spinbox left-pads
 * with zeros when the value is shorter). `dec_point_pos` inserts a decimal
 * point that many positions from the right — `0` means no decimal point, `2`
 * puts it two digits from the right. The stored value is the same integer in
 * both spinboxes (`123`); only the rendering differs.
 */
void lv_example_spinbox_format(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 3 digits, no decimal point → "123" */
    lv_obj_t * spinbox_1 = lv_spinbox_create(screen);
    lv_obj_set_width(spinbox_1, 160);
    lv_spinbox_set_digit_count(spinbox_1, 3);
    lv_spinbox_set_value(spinbox_1, 123);
    lv_spinbox_set_dec_point_pos(spinbox_1, 0);
    lv_spinbox_set_step(spinbox_1, 1);

    /* 5 digits with decimal 2 from the right → "001.23" (same value, padded + decimalised) */
    lv_obj_t * spinbox_2 = lv_spinbox_create(screen);
    lv_obj_set_width(spinbox_2, 160);
    lv_spinbox_set_digit_count(spinbox_2, 5);
    lv_spinbox_set_value(spinbox_2, 123);
    lv_spinbox_set_dec_point_pos(spinbox_2, 2);
    lv_spinbox_set_step(spinbox_2, 1);
}
#endif
