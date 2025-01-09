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

void test_xml_tabview_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * tabview_attrs[] = {
        "width", "300",
        "height", "250",
        "tab_bar_position", "bottom",
        "active", "1",
        NULL, NULL,
    };

    lv_obj_t * tabview = lv_xml_create(scr, "lv_tabview", tabview_attrs);
    lv_obj_center(tabview);

    const char * tabbar_attrs[] = {
        "style_pad_left", "100",
        "height", "100",
        NULL, NULL,
    };
    lv_obj_t * tabbar = lv_xml_create(tabview, "lv_tabview-tab_bar", tabbar_attrs);

    const char * tabbar_label_attrs[] = {
        "ignore_layout", "true",
        "text", "Hello\ntabview!",
        "x", "-90",
        "style_text_align", "right",
        "align", "left_mid",
        NULL, NULL,
    };
    lv_xml_create(tabbar, "lv_label", tabbar_label_attrs);


    const char * tab1_attrs[] = {
        "text", "Tab1",
        NULL, NULL,
    };
    lv_obj_t * tab1 = lv_xml_create(tabview, "lv_tabview-tab", tab1_attrs);

    lv_obj_t * label1 = lv_label_create(tab1);
    lv_label_set_text(label1, "This is the first tab");

    const char * tab2_attrs[] = {
        "text", "Tab2",
        NULL, NULL,
    };
    lv_obj_t * tab2 = lv_xml_create(tabview, "lv_tabview-tab", tab2_attrs);

    lv_obj_t * label2 = lv_label_create(tab2);
    lv_label_set_text(label2, "This is the second tab");


    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_tabview.png");
}

#endif
