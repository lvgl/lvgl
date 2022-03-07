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

void test_custom_prop_ids(void)
{
    uint8_t fake_flag = 0;
    uint32_t initial_custom_props = lv_style_get_num_custom_props();
    uint32_t max_props_to_register = 64;
    for(uint32_t i = 0; i < max_props_to_register; i++) {
        lv_style_prop_t prop = lv_style_register_prop(fake_flag);
        /* Should have a higher index than the last built-in prop */
        TEST_ASSERT_GREATER_THAN(_LV_STYLE_LAST_BUILT_IN_PROP, prop);
        if(i == 0) {
            /* Should be equal to the first expected index of a custom prop */
            TEST_ASSERT_EQUAL(_LV_STYLE_NUM_BUILT_IN_PROPS + initial_custom_props, prop);
        }
        /*We should find our flags*/
        TEST_ASSERT_EQUAL(fake_flag, _lv_style_prop_lookup_flags(prop));
        if(fake_flag == 0xff)
            fake_flag = 0;
        else
            fake_flag++;
    }
    TEST_ASSERT_EQUAL(initial_custom_props + max_props_to_register, lv_style_get_num_custom_props());
    /*
     * Check that the resizing algorithm works correctly, given that 64 props
     * were registered + whatever's built-in. A failure here may just indicate
     * that LVGL registers more built-in properties now and this needs adjustment.
     */
    extern uint32_t _lv_style_custom_prop_flag_lookup_table_size;
    TEST_ASSERT_EQUAL(_lv_style_custom_prop_flag_lookup_table_size, 96);
}

#endif
