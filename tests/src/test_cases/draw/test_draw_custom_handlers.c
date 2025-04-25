#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_register_custom_handlers(void)
{
    lv_draw_sw_custom_blend_handler_t handler = {
        .dest_cf = LV_COLOR_FORMAT_I2,
        .handler = NULL,
    };

    TEST_ASSERT_TRUE(lv_draw_sw_register_blend_handler(&handler));
}

void test_overwrite_custom_handler(void)
{
    lv_draw_sw_custom_blend_handler_t handler = {
        .dest_cf = LV_COLOR_FORMAT_I2,
        .handler = (lv_draw_sw_blend_handler_t)0xDEADBEEF,
    };

    TEST_ASSERT_TRUE(lv_draw_sw_register_blend_handler(&handler));

    lv_draw_sw_custom_blend_handler_t handler2 = {
        .dest_cf = LV_COLOR_FORMAT_I2,
        .handler = (lv_draw_sw_blend_handler_t)0xCAFEBABE,
    };

    TEST_ASSERT_TRUE(lv_draw_sw_register_blend_handler(&handler2));

    lv_draw_sw_blend_handler_t handler3 = lv_draw_sw_get_blend_handler(LV_COLOR_FORMAT_I2);
    TEST_ASSERT_NOT_NULL(handler3);
    TEST_ASSERT_EQUAL_PTR(handler3, (lv_draw_sw_blend_handler_t)0xCAFEBABE);
}

void test_unregister_custom_handler(void)
{
    lv_draw_sw_custom_blend_handler_t handler = {
        .dest_cf = LV_COLOR_FORMAT_I2,
        .handler = NULL,
    };

    TEST_ASSERT_TRUE(lv_draw_sw_register_blend_handler(&handler));

    TEST_ASSERT_TRUE(lv_draw_sw_unregister_blend_handler(LV_COLOR_FORMAT_I2));

    lv_draw_sw_blend_handler_t handler2 = lv_draw_sw_get_blend_handler(LV_COLOR_FORMAT_I2);
    TEST_ASSERT_NULL(handler2);
}

#endif
