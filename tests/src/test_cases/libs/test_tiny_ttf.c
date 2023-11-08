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
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_style(label, &style, 0);
    lv_label_set_text(label, "Hello world\n"
                      "I'm a font created with Tiny TTF\n"
                      "Accents: ÁÉÍÓÖŐÜŰ áéíóöőüű");
    lv_obj_center(label);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/tiny_ttf_1.png");

    lv_obj_del(label);
    lv_tiny_ttf_destroy(font);
#else
    TEST_PASS();
#endif
}

void test_tiny_ttf_kerning()
{
#if LV_USE_TINY_TTF
    extern const uint8_t kern_one_otf[];
    extern size_t kern_one_otf_size;
    lv_font_t * font_normal = lv_tiny_ttf_create_data(kern_one_otf, kern_one_otf_size, 80);
    lv_font_t * font_none = lv_tiny_ttf_create_data(kern_one_otf, kern_one_otf_size, 80);
    lv_font_set_kerning(font_none, LV_FONT_KERNING_NONE);

    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, lv_pct(90), lv_pct(90));
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * label_normal = lv_label_create(cont);
    lv_label_set_text(label_normal, "ıTuTuTı");
    lv_obj_set_style_text_font(label_normal, font_normal, LV_PART_MAIN);

    lv_obj_t * label_none = lv_label_create(cont);
    lv_label_set_text(label_none, "ıTuTuTı");
    lv_obj_set_style_text_font(label_none, font_none, LV_PART_MAIN);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/tiny_ttf_2.png");

    lv_obj_del(cont);
    lv_tiny_ttf_destroy(font_normal);
    lv_tiny_ttf_destroy(font_none);
#else
    TEST_PASS();
#endif
}

#endif
