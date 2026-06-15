#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_group_count(void)
{
    lv_group_t * group_1 = lv_group_create();
    lv_group_t * group_2 = lv_group_create();

    TEST_ASSERT_EQUAL_UINT32(lv_group_get_count(), 2U);

    lv_group_delete(group_2);
    TEST_ASSERT_EQUAL_UINT32(lv_group_get_count(), 1U);

    lv_group_delete(group_1);
    TEST_ASSERT_EQUAL_UINT32(lv_group_get_count(), 0U);
}

void test_group_by_index(void)
{
    lv_group_t * group_1 = lv_group_create();
    lv_group_t * group_2 = lv_group_create();

    TEST_ASSERT_EQUAL_PTR(lv_group_by_index(2), NULL);
    TEST_ASSERT_EQUAL_PTR(lv_group_by_index(0), group_1);
    TEST_ASSERT_EQUAL_PTR(lv_group_by_index(1), group_2);

    lv_group_delete(group_1);
    lv_group_delete(group_2);
}

void test_group_obj_by_index(void)
{
    lv_group_t * group = lv_group_create();

    TEST_ASSERT_EQUAL_PTR(lv_group_get_obj_by_index(group, 0), NULL);

    lv_obj_t * obj_0 = lv_obj_create(NULL);
    lv_obj_t * obj_1 = lv_obj_create(NULL);
    lv_group_add_obj(group, obj_0);
    lv_group_add_obj(group, obj_1);

    TEST_ASSERT_EQUAL_PTR(lv_group_get_obj_by_index(group, 0), obj_0);
    TEST_ASSERT_EQUAL_PTR(lv_group_get_obj_by_index(group, 1), obj_1);

    lv_group_remove_obj(obj_0);
    TEST_ASSERT_EQUAL_PTR(lv_group_get_obj_by_index(group, 0), obj_1);
    TEST_ASSERT_EQUAL_PTR(lv_group_get_obj_by_index(group, 1), NULL);
}

static void count_defocused_cb(lv_event_t * e)
{
    uint32_t * counter = lv_event_get_user_data(e);
    (*counter)++;
}

static void count_focused_cb(lv_event_t * e)
{
    uint32_t * counter = lv_event_get_user_data(e);
    (*counter)++;
}

/* Focusing an already-focused object must not fire FOCUSED or DEFOCUSED again. */
void test_group_focus_already_focused_obj_sends_no_events(void)
{
    lv_group_t * group = lv_group_create();
    lv_obj_t * obj = lv_obj_create(lv_screen_active());

    lv_group_add_obj(group, obj); /* obj is auto-focused as the only object */

    uint32_t focused_count   = 0;
    uint32_t defocused_count = 0;
    lv_obj_add_event_cb(obj, count_focused_cb,   LV_EVENT_FOCUSED,   &focused_count);
    lv_obj_add_event_cb(obj, count_defocused_cb, LV_EVENT_DEFOCUSED, &defocused_count);

    lv_group_focus_obj(obj);

    uint32_t final_focused   = focused_count;
    uint32_t final_defocused = defocused_count;

    lv_obj_delete(obj);
    lv_group_delete(group);

    TEST_ASSERT_EQUAL_UINT32(0U, final_focused);
    TEST_ASSERT_EQUAL_UINT32(0U, final_defocused);
}

/* Focusing a different object must send DEFOCUSED on the old and FOCUSED on the new. */
void test_group_focus_different_obj_sends_correct_events(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_group_t * group = lv_group_create();

    lv_obj_t * obj_a = lv_obj_create(screen);
    lv_obj_t * obj_b = lv_obj_create(screen);

    lv_group_add_obj(group, obj_a); /* auto-focused */
    lv_group_add_obj(group, obj_b);

    uint32_t defocused_a = 0;
    uint32_t focused_b   = 0;
    lv_obj_add_event_cb(obj_a, count_defocused_cb, LV_EVENT_DEFOCUSED, &defocused_a);
    lv_obj_add_event_cb(obj_b, count_focused_cb,   LV_EVENT_FOCUSED,   &focused_b);

    lv_group_focus_obj(obj_b);

    uint32_t final_defocused_a = defocused_a;
    uint32_t final_focused_b   = focused_b;

    lv_obj_delete(obj_a);
    lv_obj_delete(obj_b);
    lv_group_delete(group);

    TEST_ASSERT_EQUAL_UINT32(1U, final_defocused_a);
    TEST_ASSERT_EQUAL_UINT32(1U, final_focused_b);
}

/* Re-focusing a previously unfocused object must send exactly one FOCUSED event. */
void test_group_focus_obj_that_was_previously_unfocused(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_group_t * group = lv_group_create();

    lv_obj_t * obj_a = lv_obj_create(screen);
    lv_obj_t * obj_b = lv_obj_create(screen);

    lv_group_add_obj(group, obj_a); /* auto-focused */
    lv_group_add_obj(group, obj_b);
    lv_group_focus_obj(obj_b); /* obj_a defocused, obj_b focused */

    uint32_t focused_a = 0;
    lv_obj_add_event_cb(obj_a, count_focused_cb, LV_EVENT_FOCUSED, &focused_a);

    lv_group_focus_obj(obj_a);

    uint32_t final_focused_a = focused_a;

    lv_obj_delete(obj_a);
    lv_obj_delete(obj_b);
    lv_group_delete(group);

    TEST_ASSERT_EQUAL_UINT32(1U, final_focused_a);
}

#endif
