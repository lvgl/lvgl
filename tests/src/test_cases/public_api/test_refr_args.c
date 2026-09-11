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

void test_refr_obj_redraw_invalid_arguments(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_layer_t layer;
    lv_obj_redraw(NULL, refr_screen());
    lv_obj_redraw(&layer, NULL);
    lv_obj_refr(NULL, refr_screen());
    lv_obj_refr(&layer, NULL);
}

#endif /*LV_BUILD_TEST*/
