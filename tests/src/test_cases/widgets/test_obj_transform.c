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

void test_obj_transform(void)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 100, 100);
    lv_obj_center(obj);

    lv_matrix_t matrix;

    lv_matrix_identity(&matrix);
    lv_obj_set_transform(obj, &matrix);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_transform_identity.png");

    lv_matrix_identity(&matrix);
    lv_matrix_translate(&matrix, 50, 100);
    lv_obj_set_transform(obj, &matrix);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_transform_translate.png");

    lv_matrix_identity(&matrix);
    lv_matrix_rotate(&matrix, 30);
    lv_obj_set_transform(obj, &matrix);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_transform_rotate.png");

    lv_matrix_identity(&matrix);
    lv_matrix_scale(&matrix, 0.8f, 1.6f);
    lv_obj_set_transform(obj, &matrix);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_transform_scale.png");

    lv_matrix_identity(&matrix);
    lv_matrix_skew(&matrix, 10.0f, 20.0f);
    lv_obj_set_transform(obj, &matrix);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_transform_skew.png");

    const lv_matrix_t * obj_transform = lv_obj_get_transform(obj);
    TEST_ASSERT_NOT_NULL(obj_transform);
    TEST_ASSERT_EQUAL(lv_memcmp(&matrix, obj_transform, sizeof(lv_matrix_t)), 0);

    lv_obj_reset_transform(obj);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/obj_transform_identity.png");
    TEST_ASSERT_NULL(lv_obj_get_transform(obj));

#else
    TEST_PASS();
#endif
}

#endif
