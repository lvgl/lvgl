#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#if __WORDSIZE == 64
    #define TEST_FREETYPE_ASSERT_EQUAL_SCREENSHOT(NAME) TEST_ASSERT_EQUAL_SCREENSHOT("libs/freetype_" NAME ".lp64.png")
#elif __WORDSIZE == 32
    #define TEST_FREETYPE_ASSERT_EQUAL_SCREENSHOT(NAME) TEST_ASSERT_EQUAL_SCREENSHOT("libs/freetype_" NAME ".lp32.png")
#endif

static const char * UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_CN =
    "鉴于对人类家庭所有成员的固有尊严及其平等的和不移的权利的承认，乃是世界自由、正义与和平的基础...";
static const char * UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_EN =
    "Whereas recognition of the inherent dignity and of the equal and inalienable rights of all members of the human family is the foundation of freedom, justice and peace in the world...";
static const char * UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_JP =
    "人間の家族のすべての構成員の固有の尊厳と平等で譲渡不能な権利とを承認することは、自由と正義と平和の基礎である...";

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

void test_freetype_rendering_test(void)
{
#if LV_USE_FREETYPE
    /*Create a font*/
    lv_font_t * font_italic = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                      24,
                                                      LV_FREETYPE_FONT_STYLE_ITALIC);
    lv_font_t * font_normal = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                      24,
                                                      LV_FREETYPE_FONT_STYLE_NORMAL);
    lv_font_t * font_normal_small = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                            LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                            12,
                                                            LV_FREETYPE_FONT_STYLE_NORMAL);

    if(!font_italic || !font_normal || !font_normal_small) {
        LV_LOG_ERROR("freetype font create failed.");
        TEST_FAIL();
    }

    /*Create style with the new font*/
    static lv_style_t style_italic;
    lv_style_init(&style_italic);
    lv_style_set_text_font(&style_italic, font_italic);
    lv_style_set_text_align(&style_italic, LV_TEXT_ALIGN_CENTER);

    static lv_style_t style_normal;
    lv_style_init(&style_normal);
    lv_style_set_text_font(&style_normal, font_normal);
    lv_style_set_text_align(&style_normal, LV_TEXT_ALIGN_CENTER);

    static lv_style_t style_normal_small;
    lv_style_init(&style_normal_small);
    lv_style_set_text_font(&style_normal_small, font_normal_small);

    /*Create a label with the new style*/
    lv_obj_t * label0 = lv_label_create(lv_screen_active());
    lv_obj_add_style(label0, &style_italic, 0);
    lv_obj_set_width(label0, lv_obj_get_width(lv_screen_active()) - 20);
    lv_label_set_text(label0, UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_CN);
    lv_obj_align(label0,  LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_obj_add_style(label1, &style_normal, 0);
    lv_obj_set_width(label1, lv_obj_get_width(lv_screen_active()) - 20);
    lv_label_set_text(label1, UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_EN);
    lv_obj_align_to(label1, label0, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_t * label2 = lv_label_create(lv_screen_active());
    lv_obj_add_style(label2, &style_normal_small, 0);
    lv_obj_set_width(label2, lv_obj_get_width(lv_screen_active()) - 20);
    lv_label_set_text(label2, UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_JP);
    lv_obj_align_to(label2, label1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    TEST_FREETYPE_ASSERT_EQUAL_SCREENSHOT("1");

#else
    TEST_PASS();
#endif
}

#endif
