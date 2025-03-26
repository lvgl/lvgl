#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"

static uint32_t indev_reset_count = 0;

void setUp(void)
{
    /* Function run before every test */
    indev_reset_count = 0;
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static void event_cb(lv_event_t * e)
{
    lv_obj_t * scroll_obj = lv_event_get_target(e);
    lv_obj_t * act_obj = lv_obj_get_child(scroll_obj, 0);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SCROLL && act_obj) {
        lv_obj_delete(act_obj);
    }
    else if(code == LV_EVENT_INDEV_RESET) {
        indev_reset_count += 1;
    }
}

void test_indev_wait_release(void)
{
    lv_obj_t * scroll_obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(scroll_obj, 300, 300);
    lv_obj_align(scroll_obj, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_event_cb(scroll_obj, event_cb, LV_EVENT_ALL, NULL);

    lv_obj_t * act_obj = lv_obj_create(scroll_obj);
    lv_obj_set_size(act_obj, 400, 200);
    lv_obj_align(act_obj, LV_ALIGN_LEFT_MID, 0, 0);

    lv_test_mouse_move_to(200, 200);
    lv_test_mouse_press();
    lv_test_wait(50);

    lv_test_mouse_move_by(-20, 0);
    lv_test_mouse_press();
    lv_test_wait(50);

    lv_test_mouse_move_by(-20, 0);
    lv_test_mouse_press();
    lv_test_wait(50);

    lv_test_mouse_release();

    TEST_ASSERT_EQUAL_UINT32(1, indev_reset_count);
}

#endif
