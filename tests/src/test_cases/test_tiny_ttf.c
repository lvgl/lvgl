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
}

void test_tiny_ttf_rendering_test(void)
{
#if LV_USE_TINY_TTF
    /*Create a font*/
    extern const uint8_t ubuntu_font[];
    extern size_t ubuntu_font_size;
    lv_font_t * font = lv_tiny_ttf_create_data(ubuntu_font, ubuntu_font_size, 30);

    /*Create style with the new font*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    lv_style_set_bg_color(&style, lv_color_hex(0xffaaaa));

    /*Create a label with the new style*/
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_obj_add_style(label, &style, 0);
    lv_label_set_text(label, "Hello world\n"
                      "I'm a font created with Tiny TTF\n"
                      "Accents: ÁÉÍÓÖŐÜŰ áéíóöőüű");
    lv_obj_center(label);

    TEST_ASSERT_EQUAL_SCREENSHOT("tiny_ttf_1.png");

    lv_obj_del(label);
    lv_tiny_ttf_destroy(font);
#else
    TEST_PASS();
#endif
}

#endif
