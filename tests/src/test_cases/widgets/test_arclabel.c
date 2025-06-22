#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#if LV_USE_VG_LITE_THORVG
    #define FREETYPE_FONT_TYPE LV_FREETYPE_FONT_RENDER_MODE_OUTLINE
#else
    #define FREETYPE_FONT_TYPE LV_FREETYPE_FONT_RENDER_MODE_BITMAP
#endif

#ifndef NON_AMD64_BUILD
    #define EXT_NAME ".lp64.png"
#else
    #define EXT_NAME ".lp32.png"
#endif

static const char * ARCLABEL_TEXT =
    "I'm on an #FA7C45 ARC#! Centered with #12c2E9 C##8B68E8 O##c471ed L##B654E5 O##C84AB2 R##DB417A F##f64659 U##ff8888 L# 特性! \n";

static lv_font_t * font;
static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
    font = lv_freetype_font_create("src/test_files/fonts/noto/NotoSansSC-Regular.ttf", FREETYPE_FONT_TYPE, 18,
                                   LV_FREETYPE_FONT_STYLE_NORMAL);
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
    lv_obj_set_style_layout(active_screen, LV_LAYOUT_NONE, 0);

    if(font) {
        lv_freetype_font_delete(font);
    }
}

void test_arclabel_simple(void)
{
    if(!font) {
        LV_LOG_ERROR("freetype font create failed.");
        TEST_FAIL();
    }

    lv_obj_t * arclabel = lv_arclabel_create(active_screen);
    lv_obj_set_style_text_font(arclabel, font, LV_PART_MAIN);
    lv_obj_set_size(arclabel, 200, 200);
    lv_obj_set_style_text_letter_space(arclabel, 2, LV_PART_MAIN);
    lv_obj_set_style_text_color(arclabel, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_arclabel_set_angle_start(arclabel, -180);
    lv_arclabel_set_text_static(arclabel, ARCLABEL_TEXT);
    lv_arclabel_set_radius(arclabel, LV_PCT(100));
    lv_arclabel_set_recolor(arclabel, true);
    lv_arclabel_set_text_vertical_align(arclabel, LV_ARCLABEL_TEXT_ALIGN_LEADING);
    lv_arclabel_set_dir(arclabel, LV_ARCLABEL_DIR_CLOCKWISE);
    lv_arclabel_set_text_horizontal_align(arclabel, LV_ARCLABEL_TEXT_ALIGN_CENTER);
    lv_obj_center(arclabel);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/arclabel_0" EXT_NAME);

    lv_obj_set_size(arclabel, 300, 300);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/arclabel_1" EXT_NAME);

    lv_arclabel_set_recolor(arclabel, false);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/arclabel_2" EXT_NAME);
}

#endif
