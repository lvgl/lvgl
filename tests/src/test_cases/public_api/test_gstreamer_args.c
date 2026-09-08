#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#if LV_USE_GSTREAMER

void setUp(void)
{
}

void tearDown(void)
{
}

void test_gstreamer_stream_state_of_invalid_event(void)
{
    TEST_ASSERT_EQUAL(LV_GSTREAMER_STREAM_STATE_INVALID, lv_gstreamer_get_stream_state(NULL));
}

#else /*LV_USE_GSTREAMER*/

void setUp(void) { }
void tearDown(void) { }
void test_gstreamer_stream_state_of_invalid_event(void) { }

#endif /*LV_USE_GSTREAMER*/
#endif /*LV_BUILD_TEST*/
