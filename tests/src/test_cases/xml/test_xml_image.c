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
    LV_IMAGE_DECLARE(test_img_lvgl_logo_png);
    lv_xml_register_image("logo", &test_img_lvgl_logo_png);
    lv_obj_t * scr = lv_screen_active();

    const char * image1_attrs[] = {
        "src", "logo",
        "rotation", "300",
        "scale_x", "300",
        "scale_y", "500",
        "x", "50",
        "y", "50",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_image", image1_attrs);

    const char * image2_attrs[] = {
        "src", "logo",
        "inner_align", "bottom_right",
        "style_bg_color", "0xff0000",
        "style_bg_opa", "100",
        "x", "200",
        "y", "10",
        "width", "150",
        "height", "100",
        NULL, NULL,
    };

    lv_xml_create(scr, "lv_image", image2_attrs);


    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_image.png");
}

#endif
