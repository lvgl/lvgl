#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"


void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
    lv_anim_delete_all();
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
void test_anim_pause(void)
{
    int32_t var;

    /*Start an animation*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_duration(&a, 100);
    lv_anim_t * animation = lv_anim_start(&a);

    lv_test_wait(40);
    TEST_ASSERT_EQUAL(39, var);

    lv_anim_pause(animation);

    lv_test_wait(40);
    TEST_ASSERT_EQUAL(39, var);

    lv_anim_resume(animation);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(59, var);

    lv_test_wait(41);
    TEST_ASSERT_EQUAL(100, var);
}

void test_anim_pause_for(void)
{
    int32_t var;

    /*Start an animation*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_duration(&a, 100);
    lv_anim_t * animation = lv_anim_start(&a);

    lv_anim_pause_for(animation, 20);
    lv_test_wait(40);

    TEST_ASSERT_EQUAL(19, var);

    lv_anim_pause_for(animation, 20);

    lv_test_wait(40);
    lv_test_wait(40);

    TEST_ASSERT_EQUAL(79, var);

    /*Delete the animation to avoid accessing it after return*/
    lv_anim_delete(&var, exec_cb);
}

void test_anim_pause_for_resume(void)
{
    int32_t var;

    /*Start an animation*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_duration(&a, 100);
    lv_anim_t * animation = lv_anim_start(&a);

    lv_anim_pause_for(animation, 40);

    lv_test_wait(20);
    lv_anim_resume(animation);

    lv_test_wait(20);
    TEST_ASSERT_EQUAL(19, var);
}

static void event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    int * var = lv_event_get_user_data(e);
    lv_anim_delete(obj, NULL);
    *var += 1;
}

void test_scroll_anim_delete(void)
{
    int var = 0;
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_event_cb(obj, event_cb, LV_EVENT_SCROLL_END, &var);
    lv_obj_scroll_by(obj, 0, 100, LV_ANIM_ON);
    lv_test_wait(20);
    lv_obj_scroll_by(obj, 0, 100, LV_ANIM_ON);

    TEST_ASSERT_EQUAL(1, var);
}

#endif
