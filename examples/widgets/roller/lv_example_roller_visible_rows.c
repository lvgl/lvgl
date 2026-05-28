/**
 * @file lv_example_roller_visible_rows.c
 */

#include "../../lv_examples.h"
#if LV_USE_ROLLER && LV_BUILD_EXAMPLES

/**
 * @title Roller visible row count
 * @brief Tune how many list items the roller shows at once.
 *
 * `visible_row_count` determines the height of the roller in *items*, not
 * pixels — the widget computes its own pixel height from the current font and
 * line spacing. A short roller (3 rows) feels compact for a yes/no pick;
 * a tall one (7 rows) shows more context but takes more screen real estate.
 */
void lv_example_roller_visible_rows(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(screen, 32, 0);

    /* 💡 Edit `visible_row_count` on either roller to compare its overall height. */
    lv_obj_t * roller_1 = lv_roller_create(screen);
    lv_obj_set_width(roller_1, 120);
    lv_roller_set_visible_row_count(roller_1, 3);
    lv_roller_set_options(roller_1, "Jan\nFeb\nMar\nApr\nMay\nJun\nJul", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(roller_1, 0, false);

    lv_obj_t * roller_2 = lv_roller_create(screen);
    lv_obj_set_width(roller_2, 120);
    lv_roller_set_visible_row_count(roller_2, 7);
    lv_roller_set_options(roller_2, "Jan\nFeb\nMar\nApr\nMay\nJun\nJul", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(roller_2, 3, false);
}
#endif
