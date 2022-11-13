#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * chart = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
    chart = lv_chart_create(active_screen);
}

void tearDown(void)
{

}

#endif
