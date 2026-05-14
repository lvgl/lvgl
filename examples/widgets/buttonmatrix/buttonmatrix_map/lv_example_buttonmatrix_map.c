/**
 * @file lv_example_buttonmatrix_map.c
 */

#include "../../../../lvgl.h"

/**
 * @title Button matrix map
 * @brief Lay out buttons in rows using the `map` attribute.
 *
 * The `map` attribute is a space-separated list of single-quoted button labels. A
 * quoted `'\n'` token starts a new row, so the 3x3 keypad shown here is written as
 * nine labels split by two row breaks. Within a row the matrix shares the available
 * width equally between buttons by default.
 */
void lv_example_buttonmatrix_map_create(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Move or remove `'\n'` tokens inside `map` to change how buttons are grouped into rows. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Button matrix: map and row breaks");

    /* 3x3 numeric keypad: rows split by '\n' inside the map */
    lv_obj_t * lv_buttonmatrix_0 = lv_buttonmatrix_create(screen);
    lv_obj_set_align(lv_buttonmatrix_0, LV_ALIGN_CENTER);
    lv_obj_set_y(lv_buttonmatrix_0, 20);
    lv_obj_set_size(lv_buttonmatrix_0, 220, 180);
    static const char * lv_buttonmatrix_0_map_0[] = {"1", "2", "3", "\\n", "4", "5", "6", "\\n", "7", "8", "9", NULL};
    lv_buttonmatrix_set_map(lv_buttonmatrix_0, lv_buttonmatrix_0_map_0);
}

