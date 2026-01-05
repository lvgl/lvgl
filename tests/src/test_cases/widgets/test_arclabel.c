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
    "I'm on an #FA7C45 ARC#! Centered with #12c2E9 C##8B68E8 O##c471ed L##B654E5 O##C84AB2 R##DB417A F##f64659 U##ff8888 L# 特性!";
static const char * ARCLABEL_TEXT_ALIGNMENT =
    "ALIGNMENT #12c2E9 C##8B68E8 O##c471ed L##B654E5 O##C84AB2 R##DB417A F##f64659 U##ff8888 L#";

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

    lv_obj_t * arclabels[3];

    for(int32_t i = 0; i < (int32_t)(sizeof(arclabels) / sizeof(arclabels[0])); i++) {
        arclabels[i] = lv_arclabel_create(active_screen);
        lv_obj_t * arclabel = arclabels[i];
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
    }

    lv_obj_set_size(arclabels[1], 250, 250);
    lv_obj_set_style_text_letter_space(arclabels[1], 10, LV_PART_MAIN);

    lv_obj_set_size(arclabels[2], 300, 300);
    lv_arclabel_set_recolor(arclabels[2], false);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/arclabel_simple" EXT_NAME);
}

void test_arclabel_alignment(void)
{
    if(!font) {
        LV_LOG_ERROR("freetype font create failed.");
        TEST_FAIL();
    }

    lv_arclabel_text_align_t h_aligns[] = {
        LV_ARCLABEL_TEXT_ALIGN_CENTER,
        LV_ARCLABEL_TEXT_ALIGN_LEADING,
        LV_ARCLABEL_TEXT_ALIGN_TRAILING,
    };

    lv_arclabel_text_align_t v_aligns[] = {
        LV_ARCLABEL_TEXT_ALIGN_CENTER,
        LV_ARCLABEL_TEXT_ALIGN_LEADING,
        LV_ARCLABEL_TEXT_ALIGN_TRAILING,
    };

    lv_arclabel_dir_t dirs[] = {
        LV_ARCLABEL_DIR_CLOCKWISE,
        LV_ARCLABEL_DIR_COUNTER_CLOCKWISE,
    };

    const int32_t a_w = 150;
    const int32_t a_h = 150;

    const int32_t c_ofs_x = (800 - a_w * 2) / 2;
    const int32_t c_ofs_y = (480 - a_h * 2) / 2;

    for(int32_t i = 0; i < (int32_t)(sizeof(h_aligns) / sizeof(h_aligns[0])); i++) {
        for(int32_t j = 0; j < (int32_t)(sizeof(v_aligns) / sizeof(v_aligns[0])); j++) {
            for(int32_t k = 0; k < (int32_t)(sizeof(dirs) / sizeof(dirs[0])); k++) {
                if(j == 0 && k == 0) {
                    continue;
                }

                lv_obj_t * arclabel = lv_arclabel_create(active_screen);
                lv_obj_set_style_text_font(arclabel, font, LV_PART_MAIN);
                lv_obj_set_size(arclabel, a_w, a_h);
                lv_obj_set_style_text_letter_space(arclabel, 2, LV_PART_MAIN);
                lv_obj_set_style_text_color(arclabel, lv_color_hex(0x888888), LV_PART_MAIN);
                lv_arclabel_set_angle_start(arclabel, -180);
                lv_arclabel_set_text_static(arclabel, ARCLABEL_TEXT_ALIGNMENT);
                lv_arclabel_set_radius(arclabel, LV_PCT(60));
                lv_arclabel_set_recolor(arclabel, true);
                lv_arclabel_set_text_vertical_align(arclabel, v_aligns[j]);
                lv_arclabel_set_dir(arclabel, dirs[k]);
                lv_arclabel_set_text_horizontal_align(arclabel, h_aligns[i]);
                lv_obj_set_pos(arclabel, c_ofs_x + i * a_w - a_w / 2, c_ofs_y + j * a_h - a_h / 2);
            }
        }
    }
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/arclabel_alignment" EXT_NAME);
}

void test_arclabel_overflow(void)
{
    if(!font) {
        LV_LOG_ERROR("freetype font create failed.");
        TEST_FAIL();
    }

    lv_arclabel_overflow_t overflows[] = {
        LV_ARCLABEL_OVERFLOW_VISIBLE,
        LV_ARCLABEL_OVERFLOW_ELLIPSIS,
        LV_ARCLABEL_OVERFLOW_CLIP,
    };

    lv_obj_t * arclabels[3];
    const int32_t start_size = 130;
    const int32_t inc_size = 50;

    for(int32_t i = 0; i < (int32_t)(sizeof(overflows) / sizeof(overflows[0])); i++) {
        arclabels[i] = lv_arclabel_create(active_screen);
        lv_obj_t * arclabel = arclabels[i];
        lv_obj_set_style_text_font(arclabel, font, LV_PART_MAIN);
        lv_obj_set_size(arclabel, start_size + inc_size * (i + 1), start_size + inc_size * (i + 1));
        lv_obj_set_style_text_letter_space(arclabel, 2, LV_PART_MAIN);
        lv_obj_set_style_text_color(arclabel, lv_color_hex(0x888888), LV_PART_MAIN);
        lv_arclabel_set_angle_start(arclabel, -180);
        lv_arclabel_set_angle_size(arclabel, 180);
        lv_arclabel_set_text_static(arclabel, ARCLABEL_TEXT);
        lv_arclabel_set_radius(arclabel, LV_PCT(100));
        lv_arclabel_set_recolor(arclabel, true);
        lv_arclabel_set_text_vertical_align(arclabel, LV_ARCLABEL_TEXT_ALIGN_LEADING);
        lv_arclabel_set_dir(arclabel, LV_ARCLABEL_DIR_CLOCKWISE);
        lv_arclabel_set_overflow(arclabel, overflows[i]);
        lv_obj_center(arclabel);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/arclabel_overflow" EXT_NAME);
}

#endif
