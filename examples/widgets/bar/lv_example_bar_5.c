#include "../../lv_examples.h"
#if LV_USE_BAR && LV_BUILD_EXAMPLES

/**
 * Bar with LTR and RTL base direction
 */
void lv_example_bar_5(void)
{
    lv_obj_t * bar_ltr = lv_bar_create(lv_scr_act());
    lv_obj_set_size(bar_ltr, 200, 20);
    lv_bar_set_value(bar_ltr, 70, LV_ANIM_OFF);
    lv_obj_align(bar_ltr, LV_ALIGN_CENTER, 0, -30);
//    lv_obj_add_style(bar_ltr, &style_bg);
//    lv_obj_set_style_content_text(bar_ltr, "Left to Right base direction");

    lv_obj_t * bar_rtl = lv_bar_create(lv_scr_act());
    lv_obj_set_base_dir(bar_rtl, LV_BIDI_DIR_RTL);
    lv_obj_set_size(bar_rtl, 200, 20);
    lv_bar_set_value(bar_rtl, 70, LV_ANIM_OFF);
    lv_obj_align(bar_rtl, LV_ALIGN_CENTER, 0, 30);
//    lv_obj_add_style(bar_rtl, &style_bg);
//    lv_obj_set_style_content_text(bar_rtl, "Right to Left base direction");
}

#endif
