#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include <unistd.h>

static void obj_set_height_helper(void * obj, int32_t height)
{
    lv_obj_set_height((lv_obj_t *)obj, (lv_coord_t)height);
}

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
    lv_anim_set_exec_cb(&a, obj_set_height_helper);
    lv_anim_set_time(&a, 3000);
    lv_anim_set_playback_time(&a, 3000);
    lv_anim_set_repeat_count(&a, 100);
    lv_anim_set_values(&a, 0, 300);
    lv_anim_start(&a);

    uint32_t i;
    for(i = 0; i < 1000; i++) {
        lv_timer_handler();
        lv_tick_inc(100);
        usleep(1000);
    }
}

#endif
