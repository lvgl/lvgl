/**
 * @file lv_example_buttonmatrix_recolor.c
 */

#include "../../lv_examples.h"
#if LV_USE_BUTTONMATRIX && LV_BUILD_EXAMPLES

/**
 * @title Button matrix per-button text recolor
 * @brief Color parts of a button label inline with `#RRGGBB ... #` tags.
 *
 * With the `recolor` flag set in `ctrl_map`, the same `#RRGGBB ... #` syntax used
 * by `lv_label` recoloring becomes active in that button's `map` text. The three
 * buttons share a layout but each colors a different word, so a single buttonmatrix
 * can mix severity colors, badges, or status markers without per-button styles.
 */
void lv_example_buttonmatrix_recolor(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* Three buttons, each highlighting a different word with recolor */
    lv_obj_t * buttonmatrix = lv_buttonmatrix_create(screen);
    lv_obj_set_align(buttonmatrix, LV_ALIGN_CENTER);
    lv_obj_set_size(buttonmatrix, lv_pct(90), 60);
    static const char * buttonmatrix_map_0[] = {"#ff0000 Stop#", "#e08800 Warn#", "#00a000 Go#", NULL};
    lv_buttonmatrix_set_map(buttonmatrix, buttonmatrix_map_0);
    static const lv_buttonmatrix_ctrl_t buttonmatrix_ctrl_map_1[] = {LV_BUTTONMATRIX_CTRL_RECOLOR, LV_BUTTONMATRIX_CTRL_RECOLOR, LV_BUTTONMATRIX_CTRL_RECOLOR};
    lv_buttonmatrix_set_ctrl_map(buttonmatrix, buttonmatrix_ctrl_map_1);
}
#endif
