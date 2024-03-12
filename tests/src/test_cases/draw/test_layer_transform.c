#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_no_residual_border_on_scale_down(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 100, 100);
    lv_obj_center(obj);
    lv_obj_set_style_border_color(obj, lv_color_hex3(0xf00), 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex3(0x0f0), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/layer_transform_1.png");

    int32_t pivot_x[] = {lv_pct(50), lv_pct(8),  lv_pct(0), lv_pct(106), lv_pct(-16)};
    int32_t pivot_y[] = {lv_pct(50), lv_pct(12), lv_pct(0), lv_pct(113), lv_pct(-27)};
    int32_t scale[] = {256 * 4,      412,        569,       288,         711};

    uint32_t i;
    for(i = 0; i < 5; i++) {
        lv_obj_set_style_transform_pivot_x(obj, pivot_x[i], 0);
        lv_obj_set_style_transform_pivot_y(obj, pivot_y[i], 0);
        lv_obj_set_style_transform_scale(obj, scale[i], 0);
        lv_refr_now(NULL);

        /*Should be the same as the original without any artifacts*/
        lv_obj_set_style_transform_scale(obj, 256, 0);
        TEST_ASSERT_EQUAL_SCREENSHOT("draw/layer_transform_1.png");
    }

}

void test_update_layer_type_on_state_change(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_transform_rotation(&style, 90);
    lv_style_set_transform_scale_x(&style, 100);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_center(obj);
    lv_obj_add_style(obj, &style, LV_STATE_CHECKED);
    lv_refr_now(NULL);

    lv_obj_add_state(obj, LV_STATE_CHECKED);
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/layer_transform_2.png");

}

#endif
