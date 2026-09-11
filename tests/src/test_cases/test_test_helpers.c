#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static size_t timer_click_event_count = 0;
static size_t timer_click_timer_cb_count = 0;

static void timer_click_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    timer_click_event_count++;
}

static void timer_click_at_cb(lv_timer_t * t)
{
    lv_timer_delete(t);
    timer_click_timer_cb_count++;
    lv_test_mouse_click_at(70, 70);
}

static void create_timer_click_screen_case(lv_obj_t ** source_screen, lv_obj_t ** target_screen)
{
    *source_screen = lv_obj_create(NULL);
    *target_screen = lv_obj_create(NULL);

    lv_obj_t * btn = lv_button_create(*source_screen);
    lv_obj_set_pos(btn, 20, 20);
    lv_obj_set_size(btn, 100, 100);
    lv_obj_add_event_cb(btn, timer_click_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_screen_load_event(btn, LV_EVENT_CLICKED, *target_screen, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);

    lv_screen_load(*source_screen);
    lv_timer_create(timer_click_at_cb, 1, NULL);
}

void test_mouse_click_at_from_timer_loads_screen(void)
{
    timer_click_event_count = 0;
    timer_click_timer_cb_count = 0;

    lv_obj_t * source_screen = NULL;
    lv_obj_t * target_screen = NULL;
    create_timer_click_screen_case(&source_screen, &target_screen);

    lv_test_wait(200);

    TEST_ASSERT_EQUAL_UINT32(1, timer_click_timer_cb_count);
    TEST_ASSERT_EQUAL_UINT32(1, timer_click_event_count);
    TEST_ASSERT_EQUAL(target_screen, lv_screen_active());
}

void test_mouse_click_at_from_timer_loads_screen_fast_forward(void)
{
    timer_click_event_count = 0;
    timer_click_timer_cb_count = 0;

    lv_obj_t * source_screen = NULL;
    lv_obj_t * target_screen = NULL;
    create_timer_click_screen_case(&source_screen, &target_screen);

    lv_test_fast_forward(200);

    TEST_ASSERT_EQUAL_UINT32(1, timer_click_timer_cb_count);
    TEST_ASSERT_EQUAL_UINT32(1, timer_click_event_count);
    TEST_ASSERT_EQUAL(target_screen, lv_screen_active());
}

#endif

