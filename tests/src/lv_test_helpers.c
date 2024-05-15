#if LV_BUILD_TEST

#include "lv_test_helpers.h"

void lv_test_wait(uint32_t ms)
{
    lv_tick_inc(ms);
    lv_timer_handler();
    lv_refr_now(NULL);
}

#endif
