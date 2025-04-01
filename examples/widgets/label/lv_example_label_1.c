#include "../../lv_examples.h"
#if LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * Show line wrap, re-color, line align and text scrolling.
 */
void lv_example_label_1(void)
{
    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_MODE_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(label1, "Asyb.");
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    //    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);
    lv_obj_set_style_text_font(label1, &lv_font_montserrat_40, 0);

    lv_obj_t * label2 = lv_label_create(lv_screen_active());
    lv_label_set_text(label2, "It is a text. ");
    lv_obj_set_style_text_font(label2, &lv_font_montserrat_16, 0);
    //    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);
    lv_obj_set_x(label2, 200);
}

#endif
