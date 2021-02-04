#include "../../../lvgl.h"
#if LV_USE_BAR

static void set_temp(lv_obj_t * bar, int32_t temp)
{
    lv_bar_set_value(bar, temp, LV_ANIM_ON);

    static char buf[10];    /*Only the pointer t saved so must be static*/
    lv_snprintf(buf, sizeof(buf), "%d°C", temp);
    lv_obj_set_style_content_text(bar, LV_PART_INDICATOR, LV_STATE_DEFAULT, buf);
}

/**
 * A temperature meter example
 */
void lv_ex_bar_3(void)
{
    static lv_style_t style_indic;

    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, LV_COLOR_RED);
    lv_style_set_bg_grad_color(&style_indic, LV_COLOR_BLUE);
    lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_VER);
    lv_style_set_content_align(&style_indic, LV_ALIGN_OUT_LEFT_TOP);
    lv_style_set_content_ofs_x(&style_indic, -3);

    lv_obj_t * bar = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_add_style(bar, LV_PART_INDICATOR, LV_STATE_DEFAULT, &style_indic);
    lv_obj_set_size(bar, 20, 200);
    lv_obj_align(bar, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_bar_set_range(bar, -20, 40);
    set_temp(bar, 30);
}


#endif
