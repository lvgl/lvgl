#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void test_gradient_vertical_misalignment(void)
{
    lv_obj_t * obj = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x00ff00), 0);

    lv_obj_set_size(obj, 300, 100);

    lv_refr_now(NULL);
    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xffff00), 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x00ffff), 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_height);
    lv_anim_set_time(&a, 3000);
    lv_anim_set_playback_time(&a, 3000);
    lv_anim_set_repeat_count(&a, 100);
    lv_anim_set_values(&a, 0, 300);
    lv_anim_start(&a);
}

#endif
