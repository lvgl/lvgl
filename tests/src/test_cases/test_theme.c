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

static void test_widgets(const char * text, const char * img_name)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 300, 300);
    lv_obj_center(obj);

    lv_obj_t * arc = lv_arc_create(obj);
    lv_obj_set_size(arc, 200, 200);
    lv_obj_center(arc);

    lv_obj_t * label = lv_label_create(obj);
    lv_label_set_text(label, text);
    lv_obj_center(label);

    TEST_ASSERT_EQUAL_SCREENSHOT(img_name);

    lv_obj_clean(lv_screen_active());
}

void test_theme_default(void)
{
    TEST_ASSERT_TRUE(lv_theme_default_is_inited());
    TEST_ASSERT_NOT_NULL(lv_theme_default_get());

    lv_theme_t * theme = lv_theme_default_init(NULL,
                                               lv_palette_main(LV_PALETTE_BLUE),
                                               lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    TEST_ASSERT_EQUAL_PTR(theme, lv_theme_default_get());
    lv_display_set_theme(NULL, theme);
    test_widgets("Theme: Default-Light", "theme_default_light.png");

    theme = lv_theme_default_init(NULL,
                                  lv_palette_main(LV_PALETTE_BLUE),
                                  lv_palette_main(LV_PALETTE_RED),
                                  true, LV_FONT_DEFAULT);
    TEST_ASSERT_EQUAL_PTR(theme, lv_theme_default_get());
    lv_display_set_theme(NULL, theme);
    test_widgets("Theme: Default-Dark", "theme_default_dark.png");

    lv_display_set_theme(NULL, NULL);

    lv_theme_default_deinit();
    TEST_ASSERT_FALSE(lv_theme_default_is_inited());
    TEST_ASSERT_NULL(lv_theme_default_get());
}

void test_theme_mono(void)
{
    lv_theme_t * theme = lv_theme_mono_init(NULL, false, LV_FONT_DEFAULT);
    TEST_ASSERT_EQUAL_PTR(theme, lv_theme_mono_get());
    lv_display_set_theme(NULL, theme);
    test_widgets("Theme: Mono-Light", "theme_mono_light.png");

    theme = lv_theme_mono_init(NULL, true, LV_FONT_DEFAULT);
    TEST_ASSERT_EQUAL_PTR(theme, lv_theme_mono_get());
    lv_display_set_theme(NULL, theme);
    test_widgets("Theme: Mono-Dark", "theme_mono_dark.png");

    lv_display_set_theme(NULL, NULL);

    lv_theme_mono_deinit();
    TEST_ASSERT_FALSE(lv_theme_mono_is_inited());
    TEST_ASSERT_NULL(lv_theme_mono_get());
}

void test_theme_simple(void)
{
    lv_theme_t * theme = lv_theme_simple_init(NULL);
    TEST_ASSERT_EQUAL_PTR(theme, lv_theme_simple_get());
    lv_display_set_theme(NULL, theme);
    test_widgets("Theme: Simple", "theme_simple.png");

    lv_display_set_theme(NULL, NULL);

    lv_theme_simple_deinit();
    TEST_ASSERT_FALSE(lv_theme_simple_is_inited());
    TEST_ASSERT_NULL(lv_theme_simple_get());
}

#endif
