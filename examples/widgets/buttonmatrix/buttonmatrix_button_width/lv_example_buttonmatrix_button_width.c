/**
 * @file lv_example_buttonmatrix_button_width.c
 */

#include "../../../lv_examples.h"
#if LV_USE_BUTTONMATRIX && LV_BUILD_EXAMPLES

/**
 * @title Button matrix button widths
 * @brief Set per-button relative widths through `ctrl_map`.
 *
 * Each token in `ctrl_map` corresponds to one button in `map` (row-break tokens are
 * skipped). Tokens like `width_1`..`width_15` give the button a relative width that
 * acts like CSS flex-grow within its row. Here the second row uses `width_1`,
 * `width_2`, `width_3`, splitting the row into a 1/6, 2/6, 3/6 ratio.
 *
 * The row break itself is written as `'&#10;'` — the XML numeric character entity
 * for a newline (U+000A) — because a raw `\n` inside an attribute would stay as
 * the literal backslash-n.
 */
void lv_example_buttonmatrix_button_width(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Change a `width_*` token (1..15) to see the matching button stretch relative to its row neighbors. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Button matrix: relative button widths");

    /* First row: equal widths. Second row: 1:2:3 split via width_1/2/3 tokens. */
    lv_obj_t * lv_buttonmatrix_0 = lv_buttonmatrix_create(screen);
    lv_obj_set_align(lv_buttonmatrix_0, LV_ALIGN_CENTER);
    lv_obj_set_y(lv_buttonmatrix_0, 20);
    lv_obj_set_size(lv_buttonmatrix_0, lv_pct(90), 120);
    static const char * lv_buttonmatrix_0_map_0[] = {"A", "B", "C", "\n", "x1", "x2", "x3", NULL};
    lv_buttonmatrix_set_map(lv_buttonmatrix_0, lv_buttonmatrix_0_map_0);
    static const lv_buttonmatrix_ctrl_t lv_buttonmatrix_0_ctrl_map_1[] = {LV_BUTTONMATRIX_CTRL_NONE, LV_BUTTONMATRIX_CTRL_NONE, LV_BUTTONMATRIX_CTRL_NONE, LV_BUTTONMATRIX_CTRL_WIDTH_1, LV_BUTTONMATRIX_CTRL_WIDTH_2, LV_BUTTONMATRIX_CTRL_WIDTH_3};
    lv_buttonmatrix_set_ctrl_map(lv_buttonmatrix_0, lv_buttonmatrix_0_ctrl_map_1);
}
#endif
