/**
 * @file lv_example_roller_options.c
 */

#include "../../../lv_examples.h"
#if LV_USE_ROLLER && LV_BUILD_EXAMPLES

/**
 * @title Roller options and mode
 * @brief Define the option list and choose between normal and infinite scrolling.
 *
 * `options` is a newline-separated list of items, written with `&#10;` between
 * entries because raw `\n` inside an attribute survives as the literal two
 * characters. `options-mode` controls scroll behaviour: `normal` stops at the
 * first/last entry; `infinite` wraps around so the user can keep scrolling in
 * either direction. `selected` picks the initially focused index.
 */
void lv_example_roller_options(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(screen, 32, 0);

    /* Normal: bounded scrolling */
    lv_obj_t * lv_roller_0 = lv_roller_create(screen);
    lv_obj_set_width(lv_roller_0, 120);
    lv_roller_set_visible_row_count(lv_roller_0, 3);
    lv_roller_set_options(lv_roller_0, "Mon\nTue\nWed\nThu\nFri\nSat\nSun", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(lv_roller_0, 0, false);

    /* Infinite: wraps endlessly */
    lv_obj_t * lv_roller_1 = lv_roller_create(screen);
    lv_obj_set_width(lv_roller_1, 120);
    lv_roller_set_visible_row_count(lv_roller_1, 3);
    lv_roller_set_options(lv_roller_1, "Mon\nTue\nWed\nThu\nFri\nSat\nSun", LV_ROLLER_MODE_INFINITE);
    lv_roller_set_selected(lv_roller_1, 2, false);
}
#endif
