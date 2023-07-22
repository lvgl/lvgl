#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_ime_creation(void)
{
    lv_obj_t * pinyin_ime = lv_ime_pinyin_create(lv_scr_act());
    lv_ime_pinyin_set_dict(pinyin_ime, lv_ime_pinyin_def_dict);
    TEST_ASSERT_NOT_NULL(pinyin_ime);
}

#endif