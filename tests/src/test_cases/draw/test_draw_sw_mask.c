#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"
#include "lvgl_private.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_radius_mask_overflow(void)
{
    int width = 1280;
    int height = 1280;
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, width * 2, height * 2);
    lv_obj_set_style_radius(obj, height, LV_PART_MAIN);

    lv_timer_handler();
}

/* With Clang's function sanitizer enabled, these tests also check that the
 * initializer installs a callback compatible with lv_draw_sw_mask_xcb_t. */
static void check_mask_callback(void * param)
{
    lv_opa_t pixels[8];
    lv_memset(pixels, LV_OPA_COVER, sizeof(pixels));
    void * masks[] = {param, NULL};
    lv_draw_sw_mask_res_t result = lv_draw_sw_mask_apply(masks, pixels, 0, 2, 8);
    TEST_ASSERT_TRUE(result == LV_DRAW_SW_MASK_RES_FULL_COVER ||
                     result == LV_DRAW_SW_MASK_RES_CHANGED ||
                     result == LV_DRAW_SW_MASK_RES_TRANSP);
    lv_draw_sw_mask_free_param(param);
}

void test_line_mask_callback_type(void)
{
    lv_draw_sw_mask_line_param_t mask;
    lv_draw_sw_mask_line_points_init(&mask, 4, 0, 4, 7, LV_DRAW_SW_MASK_LINE_SIDE_LEFT);
    check_mask_callback(&mask);
}

void test_angle_mask_callback_type(void)
{
    lv_draw_sw_mask_angle_param_t mask;
    lv_draw_sw_mask_angle_init(&mask, 4, 4, 0, 90);
    check_mask_callback(&mask);
}

void test_radius_mask_callback_type(void)
{
    lv_draw_sw_mask_radius_param_t mask;
    const lv_area_t area = {0, 0, 7, 7};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_draw_sw_mask_radius_init(&mask, &area, 2, false));
    check_mask_callback(&mask);
}

void test_fade_mask_callback_type(void)
{
    lv_draw_sw_mask_fade_param_t mask;
    const lv_area_t area = {0, 0, 7, 7};
    lv_draw_sw_mask_fade_init(&mask, &area, LV_OPA_TRANSP, 0, LV_OPA_COVER, 7);
    check_mask_callback(&mask);
}

void test_map_mask_callback_type(void)
{
    lv_draw_sw_mask_map_param_t mask;
    const lv_area_t area = {0, 0, 7, 7};
    const lv_opa_t map[64] = {0};
    lv_draw_sw_mask_map_init(&mask, &area, map);
    check_mask_callback(&mask);
}

#endif /* LV_BUILD_TEST */
