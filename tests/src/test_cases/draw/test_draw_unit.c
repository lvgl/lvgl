#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#define UNITS_MAX 16

static lv_draw_unit_t * fake_unit;

static void fake_unit_create(void)
{
    fake_unit = lv_draw_create_unit(sizeof(lv_draw_unit_t));
    fake_unit->name = "FAKE";
}

static void fake_unit_delete(void)
{
    lv_draw_global_info_t * info = &LV_GLOBAL_DEFAULT()->draw_info;
    lv_draw_unit_t ** u = &info->unit_head;
    while(*u && *u != fake_unit) u = &(*u)->next;
    TEST_ASSERT_NOT_NULL(*u);
    *u = fake_unit->next;
    info->unit_cnt--;
    lv_free(fake_unit);
    fake_unit = NULL;
}

void setUp(void)
{
    fake_unit_create();
}

void tearDown(void)
{
    fake_unit_delete();
}

void test_draw_unit_get_next_starts_with_the_main_unit(void)
{
    TEST_ASSERT_EQUAL_PTR(fake_unit, lv_draw_unit_get_next(NULL));
}

void test_draw_unit_get_next_visits_every_unit_once(void)
{
    lv_draw_global_info_t * info = &LV_GLOBAL_DEFAULT()->draw_info;
    lv_draw_unit_t * seen[UNITS_MAX];
    uint32_t seen_cnt = 0;
    bool sw_seen = false;

    lv_draw_unit_t * u;
    for(u = lv_draw_unit_get_next(NULL); u; u = lv_draw_unit_get_next(u)) {
        TEST_ASSERT_LESS_THAN_UINT32(UNITS_MAX, seen_cnt);
        uint32_t i;
        for(i = 0; i < seen_cnt; i++) TEST_ASSERT_NOT_EQUAL(seen[i], u);
        seen[seen_cnt++] = u;
        if(u == lv_draw_sw_get_draw_unit()) sw_seen = true;
    }

    TEST_ASSERT_EQUAL_UINT32(info->unit_cnt, seen_cnt);
    TEST_ASSERT_TRUE(sw_seen);
}

void test_draw_unit_get_next_ends_with_null(void)
{
    lv_draw_unit_t * last = NULL;
    lv_draw_unit_t * u;
    for(u = lv_draw_unit_get_next(NULL); u; u = lv_draw_unit_get_next(u)) last = u;

    TEST_ASSERT_NOT_NULL(last);
    TEST_ASSERT_NULL(lv_draw_unit_get_next(last));
}

void test_draw_unit_get_next_no_longer_returns_a_removed_unit(void)
{
    fake_unit_delete();

    lv_draw_unit_t * u;
    for(u = lv_draw_unit_get_next(NULL); u; u = lv_draw_unit_get_next(u)) {
        TEST_ASSERT_NOT_EQUAL_INT32(0, lv_strcmp("FAKE", lv_draw_unit_get_name(u)));
    }

    /* tearDown removes it again */
    fake_unit_create();
}

void test_draw_unit_get_name(void)
{
    TEST_ASSERT_EQUAL_STRING("FAKE", lv_draw_unit_get_name(fake_unit));

    lv_draw_unit_t * sw = lv_draw_sw_get_draw_unit();
    TEST_ASSERT_NOT_NULL(lv_draw_unit_get_name(sw));
    TEST_ASSERT_EQUAL_STRING(sw->name, lv_draw_unit_get_name(sw));
}

#endif
