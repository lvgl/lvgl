#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

lv_obj_t * m_obj = NULL;

void setUp(void)
{
    m_obj = lv_spangroup_create(NULL);
}

void tearDown(void)
{
    // TODO: free-me
    m_obj = NULL;
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
    lv_span_t * span = lv_spangroup_new_span(m_obj);

    TEST_ASSERT(NULL != span);
}

void test_spangroup_del_span_span_is_null(void)
{
    lv_span_t * span = lv_spangroup_new_span(m_obj);

    lv_spangroup_del_span(m_obj, span);

    TEST_ASSERT(NULL == span);
}

#endif
