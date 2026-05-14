/**
 * @file lv_example_label_recolor.c
 */

#include "../../../../lvgl.h"

/**
 * @title Label text recolor
 * @brief Recolor individual words via inline color tags.
 *
 * With recolor="true", segments wrapped as "#RRGGBB ... #" render in the given color. This
 * lets a single label mix multiple colors in one string without splitting it into separate
 * label widgets.
 */
void lv_example_label_recolor_create(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Adjust inline #RRGGBB segments to recolor different words and ranges in one label. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Label: text recolor");

    /* Recolor-enabled label using inline #RRGGBB color commands */
    lv_obj_t * lv_label_1 = lv_label_create(screen);
    lv_obj_set_y(lv_label_1, 30);
    lv_label_set_recolor(lv_label_1, true);
    lv_label_set_text(lv_label_1, "Write a #ff0000 red# word, a #00a000 green# word, and a #0000ff blue# word.");
}

