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

static void count_event_cb(lv_event_t * e)
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
    lv_obj_add_event_cb(obj, count_event_cb,   LV_EVENT_FOCUSED,   &focused_count);
    lv_obj_add_event_cb(obj, count_event_cb, LV_EVENT_DEFOCUSED, &defocused_count);

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
    lv_obj_add_event_cb(obj_a, count_event_cb, LV_EVENT_DEFOCUSED, &defocused_a);
    lv_obj_add_event_cb(obj_b, count_event_cb,   LV_EVENT_FOCUSED,   &focused_b);

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
    lv_obj_add_event_cb(obj_a, count_event_cb, LV_EVENT_FOCUSED, &focused_a);

    lv_group_focus_obj(obj_a);

    uint32_t final_focused_a = focused_a;

    lv_obj_delete(obj_a);
    lv_obj_delete(obj_b);
    lv_group_delete(group);

    TEST_ASSERT_EQUAL_UINT32(1U, final_focused_a);
}

void test_group_remove_focused_obj_sends_defocused_when_others_unfocusable(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_group_t * group = lv_group_create();

    lv_obj_t * obj_hidden  = lv_obj_create(screen);
    lv_obj_t * obj_disabled = lv_obj_create(screen);
    lv_obj_t * obj_focused = lv_obj_create(screen);

    lv_group_add_obj(group, obj_hidden);
    lv_group_add_obj(group, obj_disabled);
    lv_group_add_obj(group, obj_focused);

    lv_obj_add_flag(obj_hidden, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_state(obj_disabled, LV_STATE_DISABLED);

    lv_group_focus_obj(obj_focused);
    TEST_ASSERT_EQUAL_PTR(lv_group_get_focused(group), obj_focused);

    uint32_t defocused_count = 0;
    lv_obj_add_event_cb(obj_focused, count_event_cb, LV_EVENT_DEFOCUSED, &defocused_count);

    lv_group_remove_obj(obj_focused);

    uint32_t final_count = defocused_count;

    lv_obj_delete(obj_hidden);
    lv_obj_delete(obj_disabled);
    lv_obj_delete(obj_focused);
    lv_group_delete(group);

    TEST_ASSERT_EQUAL_UINT32(1U, final_count);
}

void test_group_remove_only_focused_obj_sends_defocused(void)
{
    lv_group_t * group = lv_group_create();
    lv_obj_t * obj = lv_obj_create(lv_screen_active());

    lv_group_add_obj(group, obj);

    uint32_t defocused_count = 0;
    lv_obj_add_event_cb(obj, count_event_cb, LV_EVENT_DEFOCUSED, &defocused_count);

    lv_group_remove_obj(obj);

    uint32_t final_defocused = defocused_count;
    lv_obj_t * focused_after = lv_group_get_focused(group);

    lv_obj_delete(obj);
    lv_group_delete(group);

    TEST_ASSERT_EQUAL_UINT32(1U, final_defocused);
    TEST_ASSERT_NULL(focused_after);
}

void test_group_remove_focused_obj_focuses_previous(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_group_t * group = lv_group_create();

    lv_obj_t * obj_prev    = lv_obj_create(screen);
    lv_obj_t * obj_focused = lv_obj_create(screen);

    lv_group_add_obj(group, obj_prev);
    lv_group_add_obj(group, obj_focused);
    lv_group_focus_obj(obj_focused);

    uint32_t defocused_count = 0;
    uint32_t prev_focused_count = 0;
    lv_obj_add_event_cb(obj_focused, count_event_cb, LV_EVENT_DEFOCUSED, &defocused_count);
    lv_obj_add_event_cb(obj_prev,    count_event_cb,   LV_EVENT_FOCUSED,   &prev_focused_count);

    lv_group_remove_obj(obj_focused);

    uint32_t final_defocused    = defocused_count;
    uint32_t final_prev_focused = prev_focused_count;
    lv_obj_t * focused_after    = lv_group_get_focused(group);

    lv_obj_delete(obj_prev);
    lv_obj_delete(obj_focused);
    lv_group_delete(group);

    TEST_ASSERT_EQUAL_UINT32(1U, final_defocused);
    TEST_ASSERT_EQUAL_UINT32(1U, final_prev_focused);
    TEST_ASSERT_EQUAL_PTR(obj_prev, focused_after);
}

void test_group_remove_focused_first_obj_focuses_next(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_group_t * group = lv_group_create();

    lv_obj_t * obj_focused = lv_obj_create(screen);
    lv_obj_t * obj_next    = lv_obj_create(screen);

    lv_group_add_obj(group, obj_focused); /* auto-focused as the first object */
    lv_group_add_obj(group, obj_next);

    uint32_t defocused_count = 0;
    uint32_t next_focused_count = 0;
    lv_obj_add_event_cb(obj_focused, count_event_cb, LV_EVENT_DEFOCUSED, &defocused_count);
    lv_obj_add_event_cb(obj_next,    count_event_cb,   LV_EVENT_FOCUSED,   &next_focused_count);

    lv_group_remove_obj(obj_focused);

    uint32_t final_defocused    = defocused_count;
    uint32_t final_next_focused = next_focused_count;
    lv_obj_t * focused_after    = lv_group_get_focused(group);

    lv_obj_delete(obj_focused);
    lv_obj_delete(obj_next);
    lv_group_delete(group);

    TEST_ASSERT_EQUAL_UINT32(1U, final_defocused);
    TEST_ASSERT_EQUAL_UINT32(1U, final_next_focused);
    TEST_ASSERT_EQUAL_PTR(obj_next, focused_after);
}

void test_group_remove_non_focused_obj_no_focus_events(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_group_t * group = lv_group_create();

    lv_obj_t * obj_focused = lv_obj_create(screen);
    lv_obj_t * obj_other   = lv_obj_create(screen);

    lv_group_add_obj(group, obj_focused); /* auto-focused as the first object */
    lv_group_add_obj(group, obj_other);

    uint32_t defocused_count = 0;
    lv_obj_add_event_cb(obj_focused, count_event_cb, LV_EVENT_DEFOCUSED, &defocused_count);

    lv_group_remove_obj(obj_other);

    uint32_t final_defocused = defocused_count;
    lv_obj_t * focused_after = lv_group_get_focused(group);

    lv_obj_delete(obj_focused);
    lv_obj_delete(obj_other);
    lv_group_delete(group);

    TEST_ASSERT_EQUAL_UINT32(0U, final_defocused);
    TEST_ASSERT_EQUAL_PTR(obj_focused, focused_after);
}


#endif
