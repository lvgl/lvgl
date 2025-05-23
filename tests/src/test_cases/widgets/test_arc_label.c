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

const char * ARC_LABEL_TEXT =
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

void test_arc_label_simple(void)
{
    if(!font) {
        LV_LOG_ERROR("freetype font create failed.");
        TEST_FAIL();
    }

    lv_obj_t * arc_label = lv_arc_label_create(active_screen);
    lv_obj_set_style_text_font(arc_label, font, LV_PART_MAIN);
    lv_obj_set_size(arc_label, 200, 200);
    lv_obj_set_style_text_letter_space(arc_label, 2, LV_PART_MAIN);
    lv_obj_set_style_text_color(arc_label, lv_color_hex(0x888888), LV_PART_MAIN);
    lv_arc_label_set_angle_start(arc_label, -180);
    lv_arc_label_set_text_static(arc_label, ARC_LABEL_TEXT);
    lv_arc_label_set_radius(arc_label, LV_PCT(100));
    lv_arc_label_set_recolor(arc_label, true);
    lv_arc_label_set_text_vertical_align(arc_label, LV_ARC_LABEL_TEXT_ALIGN_LEADING);
    lv_arc_label_set_dir(arc_label, LV_ARC_LABEL_DIR_CLOCKWISE);
    lv_arc_label_set_text_horizontal_align(arc_label, LV_ARC_LABEL_TEXT_ALIGN_CENTER);
    lv_obj_center(arc_label);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/arc_label_0" EXT_NAME);

    lv_obj_set_size(arc_label, 300, 300);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/arc_label_1" EXT_NAME);

    lv_arc_label_set_recolor(arc_label, false);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/arc_label_2" EXT_NAME);
}

#endif
