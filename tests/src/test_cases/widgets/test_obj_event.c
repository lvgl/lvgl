#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
static uint32_t cb_called_count;
static void * cb_item;

static void my_delete_cb(void * item)
{
    cb_called_count ++;
    cb_item = item;
}

void setUp(void)
{
    cb_called_count = 0;
    cb_item = NULL;
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

void test_add_delete_cb_is_called_on_delete(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    int item = 42;

    lv_delete_dsc_t * dsc = lv_obj_add_delete_cb(obj, my_delete_cb, &item);
    TEST_ASSERT_NOT_NULL(dsc);
    TEST_ASSERT_EQUAL(0, cb_called_count);

    lv_obj_delete(obj);

    TEST_ASSERT_EQUAL(1, cb_called_count);
    TEST_ASSERT_EQUAL_PTR(&item, cb_item);
}

void test_add_delete_cb_multiple_cbs_all_called(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    int item = 0;

    lv_obj_add_delete_cb(obj, my_delete_cb, &item);
    lv_obj_add_delete_cb(obj, my_delete_cb, &item);
    lv_obj_add_delete_cb(obj, my_delete_cb, &item);

    lv_obj_delete(obj);

    TEST_ASSERT_EQUAL(3, cb_called_count);
    TEST_ASSERT_EQUAL_PTR(&item, cb_item);
}
void test_add_delete_cb_can_be_removed(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    int item = 42;

    lv_delete_dsc_t * dsc = lv_obj_add_delete_cb(obj, my_delete_cb, &item);
    TEST_ASSERT_NOT_NULL(dsc);
    TEST_ASSERT_EQUAL(0, cb_called_count);
    lv_obj_remove_delete_cb(dsc);

    lv_obj_delete(obj);

    TEST_ASSERT_EQUAL(0, cb_called_count);
    TEST_ASSERT_NULL(cb_item);
}

#endif
