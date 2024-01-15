#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_helpers.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

static void exec_cb(void * var, int32_t v)
{
    int32_t * var_i32 = var;
    *var_i32 = v;
}

static void custom_exec_cb(lv_anim_t * a, int32_t v)
{
    int32_t * var_i32 = a->var;
    *var_i32 = v;
}

void test_anim_delete(void)
{
    int32_t var;

    /*Start an animation*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_duration(&a, 100);
    lv_anim_start(&a);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(19, var);

    bool deleted;
    /*Wrong variable, nothing should happen*/
    deleted = lv_anim_delete(&a, exec_cb);
    TEST_ASSERT_FALSE(deleted);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(39, var);

    /*The value shouldn't change after delete*/
    deleted = lv_anim_delete(&var, exec_cb);
    TEST_ASSERT_TRUE(deleted);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(39, var);
}

void test_anim_delete_custom(void)
{
    int32_t var;

    /*Start an animation*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_custom_exec_cb(&a, custom_exec_cb);
    lv_anim_set_duration(&a, 100);
    lv_anim_start(&a);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(19, var);

    bool deleted;
    /*Wrong callback, nothing should happen*/
    deleted = lv_anim_delete(&var, exec_cb);
    TEST_ASSERT_FALSE(deleted);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(39, var);

    /*The value shouldn't change after delete*/
    deleted = lv_anim_delete(&var, NULL);
    TEST_ASSERT_TRUE(deleted);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(39, var);
}

#endif
