#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_xml_subject_int(void)
{
    const char * xml = "<globals>"
                       "	<subjects>"
                       "		<int name=\"s1\" value=\"10\"/>"
                       "		<int name=\"s2\" value=\"20\" min_value=\"10\" max_value=\"30\"/>"
                       "	</subjects>"
                       "</globals>";

    lv_xml_register_component_from_data("globals", xml);

    lv_subject_t * s1 = lv_xml_get_subject(NULL, "s1");
    lv_subject_t * s2 = lv_xml_get_subject(NULL, "s2");

    TEST_ASSERT_EQUAL_INT32(10, lv_subject_get_int(s1));
    TEST_ASSERT_EQUAL_INT32(20, lv_subject_get_int(s2));

    lv_subject_set_int(s1, 35);
    lv_subject_set_int(s2, 35);
    TEST_ASSERT_EQUAL_INT32(35, lv_subject_get_int(s1));
    TEST_ASSERT_EQUAL_INT32(30, lv_subject_get_int(s2));

    lv_subject_set_int(s1, 5);
    lv_subject_set_int(s2, 5);
    TEST_ASSERT_EQUAL_INT32(5, lv_subject_get_int(s1));
    TEST_ASSERT_EQUAL_INT32(10, lv_subject_get_int(s2));
}

#endif
