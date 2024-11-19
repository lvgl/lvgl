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

void test_group_count(void)
{
    lv_group_t * group_1 = lv_group_create();
    lv_group_t * group_2 = lv_group_create();

    TEST_ASSERT_EQUAL_UINT32(lv_group_get_count(), 2U);

    lv_group_delete(group_2);
    TEST_ASSERT_EQUAL_UINT32(lv_group_get_count(), 1U);

    lv_group_delete(group_1);
    TEST_ASSERT_EQUAL_UINT32(lv_group_get_count(), 0U);
}

void test_group_by_index(void)
{
    lv_group_t * group_1 = lv_group_create();
    lv_group_t * group_2 = lv_group_create();

    TEST_ASSERT_EQUAL_PTR(lv_group_by_index(2), NULL);
    TEST_ASSERT_EQUAL_PTR(lv_group_by_index(0), group_1);
    TEST_ASSERT_EQUAL_PTR(lv_group_by_index(1), group_2);

    lv_group_delete(group_1);
    lv_group_delete(group_2);
}

void test_group_obj_by_index(void)
{
    lv_group_t * group = lv_group_create();

    TEST_ASSERT_EQUAL_PTR(lv_group_get_obj_by_index(group, 0), NULL);

    lv_obj_t * obj_0 = lv_obj_create(NULL);
    lv_obj_t * obj_1 = lv_obj_create(NULL);
    lv_group_add_obj(group, obj_0);
    lv_group_add_obj(group, obj_1);

    TEST_ASSERT_EQUAL_PTR(lv_group_get_obj_by_index(group, 0), obj_0);
    TEST_ASSERT_EQUAL_PTR(lv_group_get_obj_by_index(group, 1), obj_1);

    lv_group_remove_obj(obj_0);
    TEST_ASSERT_EQUAL_PTR(lv_group_get_obj_by_index(group, 0), obj_1);
    TEST_ASSERT_EQUAL_PTR(lv_group_get_obj_by_index(group, 1), NULL);
}

#endif
