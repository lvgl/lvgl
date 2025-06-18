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

void test_xml_scale_with_attrs(void)
{
    lv_obj_t * scr = lv_screen_active();

    const char * scale_attrs[] = {
        "width", "120",
        "height", "120",
        "mode", "round_outer",
        "min_value", "0",
        "max_value", "150",
        "total_tick_count", "16",
        "major_tick_every", "3",
        "style_length:indicator", "10",
        "style_transform_rotation:indicator", "300",
        NULL, NULL,
    };

    lv_obj_t * scale = lv_xml_create(scr, "lv_scale", scale_attrs);
    lv_obj_center(scale);

    const char * section_attrs[] = {
        "min_value", "10",
        "max_value", "80",
        NULL, NULL,
    };

    lv_scale_section_t * section = lv_xml_create(scale, "lv_scale-section", section_attrs);

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_color(&style, lv_color_hex(0xff0000));
    lv_style_set_line_color(&style, lv_color_hex(0x00ff00));
    lv_style_set_line_width(&style, 4);
    lv_scale_set_section_style_indicator(scale, section, &style);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_scale.png");
}

#endif
