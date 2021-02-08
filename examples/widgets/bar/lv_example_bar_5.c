#include "../../../lvgl.h"
#if LV_USE_BAR

/**
 * Bar with LTR and RTL base direction
 */
void lv_example_bar_5(void)
{
    static lv_style_t style_bg;
    lv_style_init(&style_bg);
    lv_style_set_content_ofs_y(&style_bg, -3);
    lv_style_set_content_align(&style_bg, LV_ALIGN_OUT_TOP_MID);

    lv_obj_t * bar_ltr = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_size(bar_ltr, 200, 20);
    lv_bar_set_value(bar_ltr, 70, LV_ANIM_OFF);
    lv_obj_align(bar_ltr, NULL, LV_ALIGN_CENTER, 0, -30);
    lv_obj_add_style(bar_ltr, LV_PART_MAIN, LV_STATE_DEFAULT, &style_bg);
    lv_obj_set_style_content_text(bar_ltr, LV_PART_MAIN, LV_STATE_DEFAULT, "Left to Right base direction");

    lv_obj_t * bar_rtl = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_base_dir(bar_rtl, LV_BIDI_DIR_RTL);
    lv_obj_set_size(bar_rtl, 200, 20);
    lv_bar_set_value(bar_rtl, 70, LV_ANIM_OFF);
    lv_obj_align(bar_rtl, NULL, LV_ALIGN_CENTER, 0, 30);
    lv_obj_add_style(bar_rtl, LV_PART_MAIN, LV_STATE_DEFAULT, &style_bg);
    lv_obj_set_style_content_text(bar_rtl, LV_PART_MAIN, LV_STATE_DEFAULT, "Right to Left base direction");
}

#endif
