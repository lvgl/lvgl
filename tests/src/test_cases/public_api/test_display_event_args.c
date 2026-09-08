#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static uint32_t call_cnt;

void setUp(void)
{
    call_cnt = 0;
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

/*Only LV_EVENT_INVALIDATE_AREA carries an area, every other code reports none*/
static void cb_invalidated_area_wrong_code(lv_event_t * e)
{
    call_cnt++;
    TEST_ASSERT_NULL(lv_event_get_invalidated_area(e));
}

void test_display_event_args_invalidated_area_of_wrong_event_code(void)
{
    lv_display_t * disp = lv_display_get_default();
    lv_display_add_event_cb(disp, cb_invalidated_area_wrong_code, LV_EVENT_REFR_START, NULL);

    lv_display_send_event(disp, LV_EVENT_REFR_START, NULL);
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);

    lv_display_remove_event_cb_with_user_data(disp, cb_invalidated_area_wrong_code, NULL);
}

#endif /*LV_BUILD_TEST*/
