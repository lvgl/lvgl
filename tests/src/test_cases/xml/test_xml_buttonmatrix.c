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

void test_xml_buttonmatrix_with_attrs(void)
{

    const char * attrs_1[] = {
        "map", "'1' '2' '3' '\n' '4\\'s value' '\n' '5' '6' '7' '8' '9' '\n' 'Escape \\ 10'",
        "ctrl_map", "checked|width_3 none disabled none checkable none none none none checked",
        "width", "300",
        "height", "200",
        NULL, NULL,
    };

    lv_xml_create(lv_screen_active(), "lv_buttonmatrix", attrs_1);


    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_buttonmatrix.png");
}

#endif
