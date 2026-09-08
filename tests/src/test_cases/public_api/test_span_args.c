#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_obj_t * spangroup = NULL;

void setUp(void)
{
    spangroup = lv_spangroup_create(NULL);
}

void tearDown(void)
{
    lv_obj_delete(spangroup);
    spangroup = NULL;
    lv_obj_clean(lv_screen_active());
}

void test_spangroup_new_span_with_null_parameter_returns_null_object(void)
{
    lv_span_t * span = lv_spangroup_add_span(NULL);

    TEST_ASSERT(NULL == span);
    TEST_ASSERT_EQUAL_INT(0, lv_spangroup_get_span_count(spangroup));
}

void test_span_set_text_with_bad_parameter_no_action_performed(void)
{
    const char * test_text = "Test Text";
    lv_span_t * span = lv_spangroup_add_span(spangroup);

    lv_span_set_text(span, test_text);
    lv_span_set_text(span, NULL);

    TEST_ASSERT_EQUAL_STRING(span->txt, test_text);
}

void test_span_set_text_static_with_bad_parameter_no_action_performed(void)
{
    const char * test_text = "Test Text";
    lv_span_t * span = lv_spangroup_add_span(spangroup);

    lv_span_set_text_static(span, test_text);
    lv_span_set_text_static(span, NULL);

    TEST_ASSERT_EQUAL_STRING(span->txt, test_text);
}

#endif /*LV_BUILD_TEST*/
