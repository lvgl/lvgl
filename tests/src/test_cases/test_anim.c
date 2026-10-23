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
    lv_anim_enable_vsync_mode(false);
}

static void start_cb(lv_anim_t * anim)
{
    (*(int *)lv_anim_get_user_data(anim))++;
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
void test_anim_start_cb_is_called(void)
{
    int32_t var;
    int start_cb_call_count = 0;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_user_data(&a, (void *)&start_cb_call_count);
    lv_anim_set_start_cb(&a, start_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_duration(&a, 100);
    lv_anim_set_repeat_count(&a, 2);
    lv_anim_start(&a);
    lv_test_wait(50);
    TEST_ASSERT_EQUAL(1, start_cb_call_count);
    lv_test_wait(100);
    TEST_ASSERT_EQUAL(2, start_cb_call_count);
    lv_test_wait(50);
    /*Delete the animation to avoid accessing it after return*/
    lv_anim_delete(&var, exec_cb);
}

void test_anim_vsync_mode(void)
{
    lv_anim_enable_vsync_mode(true);

    /* anim timer should be NULL when vsync mode is enabled */
    TEST_ASSERT_NULL(lv_anim_get_timer());

    int32_t var;

    /*Start an animation*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_values(&a, 0, 1000);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_duration(&a, 1000);
    lv_anim_start(&a);

    /*Use vsync events to notify anim updates*/
    lv_tick_inc(10);
    lv_display_send_vsync_event(lv_display_get_default(), NULL);
    TEST_ASSERT_EQUAL(9, var);

    lv_tick_inc(10);
    lv_display_send_vsync_event(lv_display_get_default(), NULL);
    TEST_ASSERT_EQUAL(19, var);

    lv_anim_enable_vsync_mode(false);
    TEST_ASSERT_NOT_NULL(lv_anim_get_timer());

    /* Should not update the animation with vsync events when vsync mode is disabled */
    lv_tick_inc(20);
    lv_display_send_vsync_event(lv_display_get_default(), NULL);
    TEST_ASSERT_EQUAL(19, var);

    /* Test normal timer mode */
    lv_test_wait(20);
    TEST_ASSERT_EQUAL(59, var);

    lv_anim_delete(&var, exec_cb);
}

/*#10767: regression tests for the `var` lookup index used by
 *`lv_anim_get()`/`lv_anim_delete()`/`remove_concurrent_anims()`.*/

static void exec_cb_alt(void * var, int32_t v)
{
    int32_t * var_i32 = var;
    *var_i32 = v;
}

void test_anim_index_mass_distinct_vars(void)
{
    enum { N = 128 };
    static int32_t vars[N];
    lv_anim_t a;

    for(int i = 0; i < N; i++) {
        lv_anim_init(&a);
        lv_anim_set_var(&a, &vars[i]);
        lv_anim_set_exec_cb(&a, exec_cb);
        lv_anim_set_values(&a, 0, 100);
        lv_anim_set_duration(&a, 1);
        lv_anim_start(&a);
    }

    /*Every animation must be found through the index*/
    for(int i = 0; i < N; i++) {
        TEST_ASSERT_NOT_NULL(lv_anim_get(&vars[i], exec_cb));
    }

    /*Deleting one var must not disturb the animations of other vars*/
    for(int i = 0; i < N; i++) {
        TEST_ASSERT_TRUE(lv_anim_delete(&vars[i], exec_cb));
        TEST_ASSERT_NULL(lv_anim_get(&vars[i], exec_cb));
        if(i + 1 < N) {
            TEST_ASSERT_NOT_NULL(lv_anim_get(&vars[i + 1], exec_cb));
        }
    }

    TEST_ASSERT_FALSE(lv_anim_delete(&vars[0], exec_cb));
}

void test_anim_index_same_var_different_cbs(void)
{
    int32_t var;
    lv_anim_t a;

    /*Two animations on the same var with different exec_cbs coexist*/
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_duration(&a, 1);
    lv_anim_start(&a);

    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_exec_cb(&a, exec_cb_alt);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_duration(&a, 1);
    lv_anim_start(&a);

    /*A second animation on another var must not be affected*/
    int32_t other;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &other);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_duration(&a, 1);
    lv_anim_start(&a);

    /*Delete one specific pair*/
    TEST_ASSERT_TRUE(lv_anim_delete(&var, exec_cb_alt));
    TEST_ASSERT_NULL(lv_anim_get(&var, exec_cb_alt));
    TEST_ASSERT_NOT_NULL(lv_anim_get(&var, exec_cb));

    /*Delete every animation of `var`, other vars untouched*/
    TEST_ASSERT_TRUE(lv_anim_delete(&var, NULL));
    TEST_ASSERT_NULL(lv_anim_get(&var, exec_cb));
    TEST_ASSERT_NOT_NULL(lv_anim_get(&other, exec_cb));

    /*Delete by exec_cb across vars*/
    TEST_ASSERT_TRUE(lv_anim_delete(NULL, exec_cb));
    TEST_ASSERT_NULL(lv_anim_get(&other, exec_cb));
}

static void deleted_cb_start_new(lv_anim_t * anim)
{
    /*Start a new animation for the same var while it is being deleted.
     *The new animation lands in the same `var` index bucket during unlink.*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, anim->var);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, 0, 1);
    lv_anim_set_duration(&a, 1);
    lv_anim_start(&a);
}

void test_anim_index_start_in_deleted_cb(void)
{
    int32_t var;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_duration(&a, 100);
    lv_anim_set_deleted_cb(&a, deleted_cb_start_new);
    lv_anim_start(&a);

    /*Delete: the deleted_cb starts a replacement animation, but the walk
     *restarts from the bucket head and removes it within the same
     *`lv_anim_delete()` call (unchanged, full-list behaviour)*/
    TEST_ASSERT_TRUE(lv_anim_delete(&var, exec_cb));
    TEST_ASSERT_NULL(lv_anim_get(&var, exec_cb));

    /*The index stays consistent afterwards: a new animation on the same
     *var must be found and removable*/
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_duration(&a, 100);
    lv_anim_start(&a);
    TEST_ASSERT_NOT_NULL(lv_anim_get(&var, exec_cb));
    TEST_ASSERT_TRUE(lv_anim_delete(&var, exec_cb));
    TEST_ASSERT_FALSE(lv_anim_delete(&var, exec_cb));
}

void test_anim_index_concurrent_removal_keeps_others(void)
{
    int32_t var_a, var_b;
    lv_anim_t a;

    /*Two concurrent pairs (same var + same exec_cb): the later start
     *must remove the earlier one of the same var only*/
    lv_anim_init(&a);
    lv_anim_set_var(&a, &var_a);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_duration(&a, 100);
    lv_anim_start(&a);

    lv_anim_init(&a);
    lv_anim_set_var(&a, &var_a);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, 100, 200);
    lv_anim_set_duration(&a, 100);
    lv_anim_start(&a);

    lv_anim_init(&a);
    lv_anim_set_var(&a, &var_b);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_duration(&a, 100);
    lv_anim_start(&a);

    TEST_ASSERT_EQUAL(100, var_a); /*early_apply applied the second animation*/
    TEST_ASSERT_NOT_NULL(lv_anim_get(&var_a, exec_cb));
    TEST_ASSERT_NOT_NULL(lv_anim_get(&var_b, exec_cb));
    TEST_ASSERT_EQUAL(0, var_b);

    TEST_ASSERT_TRUE(lv_anim_delete(&var_a, exec_cb));
    TEST_ASSERT_TRUE(lv_anim_delete(&var_b, exec_cb));
}

#endif
