#include "../../lv_examples.h"
#if LV_USE_LABEL && LV_BUILD_EXAMPLES && LV_FONT_DEJAVU_16_PERSIAN_HEBREW && LV_FONT_SIMSUN_16_CJK && LV_USE_BIDI

/**
 * Show mixed LTR, RTL and Chiease label
 */
void lv_example_label_3(void)
{
    lv_obj_t * ltr_label = lv_label_create(lv_scr_act());
    lv_label_set_text(ltr_label, "An embedded system is a computer system - a combination of a computer processor, computer memory, and input/output peripheral devices...");
    lv_obj_set_style_text_font(ltr_label, &lv_font_montserrat_16, 0);
    lv_obj_set_width(ltr_label, 300);
    lv_obj_align(ltr_label, LV_ALIGN_TOP_LEFT, 10, 10);

    lv_obj_t * rtl_label = lv_label_create(lv_scr_act());
    lv_label_set_text(rtl_label, "RTL:\nמערכת משובצת מחשב (באנגלית: Embedded system) היא מערכת או מכשיר, בו משולב מחשב (או, ליתר דיוק, מעבד) המבצע רק פונקציות ספציפיות המיועדות לתפעול המערכת או המכשיר.");
    lv_obj_set_style_base_dir(rtl_label, LV_BASE_DIR_RTL, 0);
    lv_obj_set_style_text_font(rtl_label, &lv_font_dejavu_16_persian_hebrew, 0);
    lv_obj_set_width(rtl_label, 300);
    lv_obj_align(rtl_label, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t * cz_label = lv_label_create(lv_scr_act());
    lv_label_set_text(cz_label, "嵌入式系统（Embedded System），是一种嵌入机械或电气系统内部、具有专一功能和实时计算性能的计算机系统。");
    lv_obj_set_style_text_font(cz_label, &lv_font_simsun_16_cjk, 0);
    lv_obj_set_width(cz_label, 300);
    lv_obj_align(cz_label, LV_ALIGN_BOTTOM_LEFT, 10, -50);
}

#endif
