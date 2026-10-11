#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"
#include <stdlib.h>

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

void test_delete_cb_null(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    TEST_ASSERT_NULL(lv_obj_add_delete_cb(NULL, free, NULL));
    TEST_ASSERT_NULL(lv_obj_add_delete_cb(obj, NULL, NULL));

    /*NULL is okay in remove functions*/
    lv_obj_remove_delete_cb(NULL);
    TEST_PASS();
}
#endif /*LV_BUILD_TEST*/
