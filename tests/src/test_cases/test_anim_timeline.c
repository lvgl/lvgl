#if LV_BUILD_TEST

#include "../lvgl.h"

#include "unity/unity.h"

static lv_anim_timeline_t * anim_timeline;

void setUp(void)
{
    /* Function run before every test */
    anim_timeline = NULL;
}

void tearDown(void)
{
    /* Function run after every test */
    if(anim_timeline) lv_anim_timeline_delete(anim_timeline);
    lv_obj_clean(lv_screen_active());

}

void test_anim_timeline_progress_1(void)
{
    lv_obj_t * obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj, 100, 100);
    lv_obj_set_pos(obj, 30, 40);

    lv_anim_t a1;
    lv_anim_init(&a1);
    lv_anim_set_exec_cb(&a1, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&a1, obj);
    lv_anim_set_values(&a1, 0, 700);
    lv_anim_set_time(&a1, 1000);

    anim_timeline = lv_anim_timeline_create();
    TEST_ASSERT_NOT_NULL(anim_timeline);

    lv_anim_timeline_add(anim_timeline, 0, &a1);

    lv_anim_timeline_set_progress(anim_timeline, 0);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(0, lv_obj_get_x(obj));

    lv_anim_timeline_set_progress(anim_timeline, 65535);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(700, lv_obj_get_x(obj));

    lv_anim_timeline_set_progress(anim_timeline, 10000);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(105, lv_obj_get_x(obj));

    lv_anim_timeline_set_progress(anim_timeline, 20000);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(213, lv_obj_get_x(obj));

    lv_anim_timeline_set_progress(anim_timeline, 30000);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(319, lv_obj_get_x(obj));

    lv_anim_timeline_set_progress(anim_timeline, 40000);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(426, lv_obj_get_x(obj));

    lv_anim_timeline_set_progress(anim_timeline, 50000);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(533, lv_obj_get_x(obj));

    lv_anim_timeline_set_progress(anim_timeline, 60000);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(639, lv_obj_get_x(obj));

    lv_anim_timeline_set_progress(anim_timeline, 32768);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(350, lv_obj_get_x(obj));

    lv_anim_timeline_set_progress(anim_timeline, 65535);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(700, lv_obj_get_x(obj));

}

void test_anim_timeline_progress_2(void)
{
    lv_obj_t * obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj, 100, 100);
    lv_anim_t a1;
    lv_anim_init(&a1);
    lv_anim_set_exec_cb(&a1, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_var(&a1, obj);
    lv_anim_set_values(&a1, 0, 700);
    lv_anim_set_time(&a1, 1000);

    lv_anim_t a2;
    lv_anim_init(&a2);
    lv_anim_set_exec_cb(&a2, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_var(&a2, obj);
    lv_anim_set_values(&a2, 0, 300);
    lv_anim_set_time(&a2, 1000);

    /*
     *   |------X------|
     *          |------Y------|
     *   0     500    1000   1500 ms
     *   0     21845  43690  65535 progress
     */

    anim_timeline = lv_anim_timeline_create();
    lv_anim_timeline_add(anim_timeline, 0, &a1);
    lv_anim_timeline_add(anim_timeline, 500, &a2);

    lv_anim_timeline_set_progress(anim_timeline, 0);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(0, lv_obj_get_x(obj));

    lv_anim_timeline_set_progress(anim_timeline, 65535);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(700, lv_obj_get_x(obj));
    TEST_ASSERT_EQUAL(300, lv_obj_get_y(obj));

    lv_anim_timeline_set_progress(anim_timeline, 10000);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(159, lv_obj_get_x(obj));
    TEST_ASSERT_EQUAL(0, lv_obj_get_y(obj));

    lv_anim_timeline_set_progress(anim_timeline, 21845);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(350, lv_obj_get_x(obj));
    TEST_ASSERT_EQUAL(0, lv_obj_get_y(obj));

    lv_anim_timeline_set_progress(anim_timeline, 32768);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(525, lv_obj_get_x(obj));
    TEST_ASSERT_EQUAL(75, lv_obj_get_y(obj));

    lv_anim_timeline_set_progress(anim_timeline, 43690);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(700, lv_obj_get_x(obj));
    TEST_ASSERT_EQUAL(150, lv_obj_get_y(obj));

    lv_anim_timeline_set_progress(anim_timeline, 65535);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL(700, lv_obj_get_x(obj));
    TEST_ASSERT_EQUAL(300, lv_obj_get_y(obj));
}

#endif
