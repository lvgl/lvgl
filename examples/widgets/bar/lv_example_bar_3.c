#include "../../../lvgl.h"
#if LV_USE_BAR && LV_BUILD_EXAMPLES

static void set_temp(void * bar, int32_t temp)
{
    lv_bar_set_value(bar, temp, LV_ANIM_ON);

    static char buf[10];    /*Only the pointer t saved so must be static*/
    lv_snprintf(buf, sizeof(buf), "%dÂ°C", temp);
//    lv_obj_set_style_content_text(bar, LV_PART_INDICATOR, LV_STATE_DEFAULT, buf);
}

/**
 * A temperature meter example
 */
void lv_example_bar_3(void)
{
    static lv_style_t style_indic;

    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_color_red());
    lv_style_set_bg_grad_color(&style_indic, lv_color_blue());
    lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_VER);
//    lv_style_set_content_align(&style_indic, LV_ALIGN_OUT_LEFT_TOP);
//    lv_style_set_content_ofs_x(&style_indic, -3);
//    lv_style_set_content_color(&style_indic, lv_color_grey());

    lv_obj_t * bar = lv_bar_create(lv_scr_act());
    lv_obj_add_style(bar, LV_PART_INDICATOR, LV_STATE_DEFAULT, &style_indic);
    lv_obj_set_size(bar, 20, 200);
    lv_obj_center(bar);
    lv_bar_set_range(bar, -20, 40);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_temp);
    lv_anim_set_time(&a, 3000);
    lv_anim_set_playback_time(&a, 3000);
    lv_anim_set_var(&a, bar);
    lv_anim_set_values(&a, -20, 40);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}


#endif
