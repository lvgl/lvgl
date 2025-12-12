
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

void test_xml_imagebutton_with_attrs(void)
{


    LV_IMAGE_DECLARE(imagebutton_left);
    LV_IMAGE_DECLARE(imagebutton_right);
    LV_IMAGE_DECLARE(imagebutton_mid);

    lv_xml_register_image(NULL, "img_left", &imagebutton_left);
    lv_xml_register_image(NULL, "img_right", &imagebutton_right);
    lv_xml_register_image(NULL, "img_mid", &imagebutton_mid);

    lv_obj_t * scr = lv_screen_active();

    const char * imagebutton_attrs[] = {
        "state", "checked_disabled",
        "align", "center",
        "width", "200",
        NULL, NULL,
    };

    lv_obj_t * imagebutton = lv_xml_create(scr, "lv_imagebutton", imagebutton_attrs);
    lv_obj_center(imagebutton);

    const char * imagebutton_src_left_attrs[] = {
        "state", "checked_disabled",
        "src", "img_left",
        NULL, NULL,
    };
    lv_xml_create(imagebutton, "lv_imagebutton-src_left", imagebutton_src_left_attrs);

    const char * imagebutton_src_right_attrs[] = {
        "state", "checked_disabled",
        "src", "img_right",
        NULL, NULL,
    };
    lv_xml_create(imagebutton, "lv_imagebutton-src_right", imagebutton_src_right_attrs);

    const char * imagebutton_src_mid_attrs[] = {
        "state", "checked_disabled",
        "src", "img_mid",
        NULL, NULL,
    };
    lv_xml_create(imagebutton, "lv_imagebutton-src_mid", imagebutton_src_mid_attrs);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/lv_imagebutton.png");
}

#endif
