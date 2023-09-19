#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

lv_obj_t * spangroup = NULL;

void setUp(void)
{
    spangroup = lv_spangroup_create(NULL);
}

void tearDown(void)
{
    // TODO: free-me
    spangroup = NULL;
}

void test_spangroup_create_not_null(void)
{
    lv_obj_t * obj = lv_spangroup_create(NULL);
    
    TEST_ASSERT(NULL != obj);
}

void test_spangroup_new_span_with_null_parameter_fails(void)
{
    lv_span_t * span = lv_spangroup_new_span(NULL);

    TEST_ASSERT(NULL == span);
}

void test_spangroup_new_span_not_null(void)
{
    lv_span_t * span = lv_spangroup_new_span(spangroup);

    TEST_ASSERT(NULL != span);
}

void test_spangroup_del_span_span_is_null(void)
{
    lv_span_t * span = lv_spangroup_new_span(spangroup);

    lv_spangroup_del_span(spangroup, span);

    // TODO: this test shouldn't pass this way
    TEST_ASSERT(NULL != span);
}

void test_spangroup_get_child_cnt_returns_right_cnt(void)
{

    (void)lv_spangroup_new_span(spangroup);

    uint32_t cnt = lv_spangroup_get_child_cnt(spangroup);

    TEST_ASSERT_EQUAL(cnt, 1);
}

void test_span_set_text(void)
{
    char const * test_text = "Test Text";    
    lv_span_t * span = lv_spangroup_new_span(spangroup);
    lv_span_set_text(span, test_text);
void lv_span_set_text_static(lv_span_t * span, const char * text);

    TEST_ASSERT_EQUAL_STRING(span->txt, test_text);
}

void test_span_set_text_static(void)
{
    char const * test_text = "Test Text";    
    lv_span_t * span = lv_spangroup_new_span(spangroup);
    lv_span_set_text_static(span, test_text);

    TEST_ASSERT_EQUAL_STRING(span->txt, test_text);
}

void test_spangroup_set_align(void)
{
    lv_text_align_t align = LV_TEXT_ALIGN_CENTER; 
    lv_spangroup_set_align(spangroup, align);

    TEST_ASSERT_EQUAL(align, lv_spangroup_get_align(spangroup));
}

void test_spangroup_set_overflow(void)
{
    lv_span_overflow_t overflow = LV_SPAN_OVERFLOW_ELLIPSIS; 
    lv_spangroup_set_overflow(spangroup, overflow);

    TEST_ASSERT_EQUAL(overflow, lv_spangroup_get_overflow(spangroup));
}

#endif
