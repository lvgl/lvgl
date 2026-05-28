/**
 * @file lv_example_label_recolor.c
 */

#include "../../lv_examples.h"
#if LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * @title Label text recolor
 * @brief Recolor individual words via inline color tags.
 *
 * With recolor="true", segments wrapped as "#RRGGBB ... #" render in the given color. This
 * lets a single label mix multiple colors in one string without splitting it into separate
 * label widgets.
 */
void lv_example_label_recolor(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Recolor-enabled label using inline #RRGGBB color commands */
    lv_obj_t * label = lv_label_create(screen);
    lv_obj_set_width(label, lv_pct(90));
    lv_label_set_recolor(label, true);
    lv_label_set_text(label, "Write a #ff0000 red# word, a #00a000 green# word, and a #0000ff blue# word.");
}
#endif
