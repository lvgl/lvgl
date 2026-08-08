/**
 * Finding the object that covers the invalidated area
 */
#if LV_BUILD_TEST

#include "refr/lv_test_refr.h"

void setUp(void)
{
    refr_ctx_reset();
}

void tearDown(void)
{
    refr_disp_delete();
}

void test_refr_top_obj_invalid_arguments(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_area_t area = {0, 0, 9, 9};
    TEST_ASSERT_NULL(lv_refr_get_top_obj(NULL, refr_screen()));
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&area, NULL));
}

void test_refr_top_obj_finds_the_covering_object(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());

    /*Fully inside the object*/
    lv_area_t inside = {20, 20, 29, 29};
    TEST_ASSERT_EQUAL_PTR(obj, lv_refr_get_top_obj(&inside, refr_screen()));

    /*Only partially on the object, so the screen covers it*/
    lv_area_t crossing = {5, 5, 20, 20};
    TEST_ASSERT_EQUAL_PTR(refr_screen(), lv_refr_get_top_obj(&crossing, refr_screen()));

    /*Searching from the object itself finds the object*/
    TEST_ASSERT_EQUAL_PTR(obj, lv_refr_get_top_obj(&inside, obj));

    /*Outside of the given object*/
    lv_area_t outside = {55, 55, 60, 60};
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&outside, obj));
}

void test_refr_top_obj_skips_the_hidden_object(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());
    lv_obj_set_hidden(obj, true);

    lv_area_t inside = {20, 20, 29, 29};
    TEST_ASSERT_EQUAL_PTR(refr_screen(), lv_refr_get_top_obj(&inside, refr_screen()));
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&inside, obj));
}

void test_refr_top_obj_skips_the_transparent_object(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());
    lv_area_t inside = {20, 20, 29, 29};

    /*A not fully opaque object can not cover anything*/
    lv_obj_set_style_opa(obj, LV_OPA_50, 0);
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&inside, obj));

    /*Neither can an object that does not fill its area*/
    lv_obj_set_style_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_50, 0);
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&inside, obj));
}

void test_refr_top_obj_skips_the_layered_object(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());
    lv_area_t inside = {20, 20, 29, 29};

    /*Objects drawn on their own layer are blended afterwards, they can not cover*/
    lv_obj_set_style_opa_layered(obj, LV_OPA_50, 0);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_SIMPLE, lv_obj_get_layer_type(obj));
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&inside, obj));

    lv_obj_set_style_opa_layered(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_transform_rotation(obj, 300, 0);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_TRANSFORM, lv_obj_get_layer_type(obj));
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&inside, obj));
}

void test_refr_top_obj_skips_the_rounded_corner(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_RED);
    lv_obj_set_style_radius(obj, 15, 0);
    lv_obj_update_layout(refr_screen());

    /*The corner of the object is not covered by the rounded background*/
    lv_area_t corner = {10, 10, 14, 14};
    TEST_ASSERT_NULL(lv_refr_get_top_obj(&corner, obj));

    /*The middle still is*/
    lv_area_t middle = {25, 25, 34, 34};
    TEST_ASSERT_EQUAL_PTR(obj, lv_refr_get_top_obj(&middle, obj));
}

void test_refr_top_obj_prefers_the_topmost_child(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * bottom = refr_rect_create(refr_screen(), 0, 0, 60, 60, REFR_COLOR_RED);
    lv_obj_t * top = refr_rect_create(refr_screen(), 0, 0, 60, 60, REFR_COLOR_GREEN);
    lv_obj_update_layout(refr_screen());

    lv_area_t area = {10, 10, 19, 19};
    TEST_ASSERT_EQUAL_PTR(top, lv_refr_get_top_obj(&area, refr_screen()));

    /*With the topmost one hidden the one below is used*/
    lv_obj_set_hidden(top, true);
    TEST_ASSERT_EQUAL_PTR(bottom, lv_refr_get_top_obj(&area, refr_screen()));
}

#endif /*LV_BUILD_TEST*/
