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

void test_xml_spangroup_with_attrs(void)
{

    lv_obj_t * scr = lv_screen_active();

    const char * spangroup_attrs[] = {
        "width", "300",
        "indent", "40",
        "max_lines", "3",
        "overflow", "ellipsis",
        "style_text_align", "center",
        "style_bg_opa", "20%",
        "style_bg_color", "#ffff00",
        NULL, NULL,
    };

    lv_obj_t * spangroup = lv_xml_create(scr, "lv_spangroup", spangroup_attrs);
    lv_obj_center(spangroup);

    const char * span1_attrs[] = {
        "text", "This is a red text as a span!",
        NULL, NULL,
    };

    lv_span_t * span1 = lv_xml_create(spangroup, "lv_spangroup-span", span1_attrs);

    static lv_style_t style1;
    lv_style_init(&style1);
    lv_style_set_text_color(&style1, lv_color_hex(0xff0000));
    lv_style_set_text_letter_space(&style1, 5);

    lv_spangroup_set_span_style(spangroup, span1, &style1);

    const char * span2_attrs[] = {
        "text", "And this a large blue!",
        NULL, NULL,
    };

    lv_span_t * span2 = lv_xml_create(spangroup, "lv_spangroup-span", span2_attrs);

    static lv_style_t style2;
    lv_style_init(&style2);
    lv_style_set_text_color(&style2, lv_color_hex(0x0000ff));
    lv_style_set_text_font(&style2, &lv_font_montserrat_48);

    lv_spangroup_set_span_style(spangroup, span2, &style2);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_spangroup.png");
}

#endif
