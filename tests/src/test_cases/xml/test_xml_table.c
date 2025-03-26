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

void test_xml_table_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * table_attrs[] = {
        "width", "content",
        "style_border_side:items", "full",
        NULL, NULL,
    };

    lv_obj_t * table = lv_xml_create(scr, "lv_table", table_attrs);
    lv_obj_center(table);

    const char * column_1_attrs[] = {
        "column", "0",
        "width", "80",
        NULL, NULL,
    };
    lv_xml_create(table, "lv_table-column", column_1_attrs);

    const char * cell_1_2_attrs[] = {
        "row", "1",
        "column", "2",
        "value", "A",
        NULL, NULL,
    };
    lv_xml_create(table, "lv_table-cell", cell_1_2_attrs);


    const char * cell_2_0_attrs[] = {
        "row", "2",
        "column", "0",
        "value", "hello this a long text which should be cropped",
        "ctrl", "text_crop|merge_right",
        NULL, NULL,
    };

    lv_xml_create(table, "lv_table-cell", cell_2_0_attrs);

    const char * cell_3_0_attrs[] = {
        "row", "3",
        "column", "0",
        "value", "wrap this text",
        NULL, NULL,
    };

    lv_xml_create(table, "lv_table-cell", cell_3_0_attrs);


    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_table.png");
}

#endif
