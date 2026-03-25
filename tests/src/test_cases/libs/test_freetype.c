#if LV_BUILD_TEST
#include "lvgl.h"
#include "src/libs/freetype/lv_freetype_private.h"

#include "unity/unity.h"

#if LV_USE_FREETYPE

#ifndef NON_AMD64_BUILD
    #define EXT_NAME ".lp64.png"
#else
    #define EXT_NAME ".lp32.png"
#endif

#define OPTION_GENERATE_OUTLINE_DATA 0

/*
 * Generated vector ops string can use https://w-mai.github.io/vegravis/
 * to visualize the outline data.
 **/
#define OPTION_GENERATE_VECTOR_OPS_STRING 0

static const char * UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_CN =
    "鉴于对人类家庭所有成员的固有尊严及其平等的和不移的权利的承认，乃是世界自由、正义与和平的基础...";
static const char * UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_EN =
    "Whereas recognition of the inherent dignity and of the equal and inalienable rights of all members of the human family is the foundation of freedom, justice and peace in the world...";
static const char * UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_JP =
    "人間の家族のすべての構成員の固有の尊厳と平等で譲渡不能な権利とを承認することは、自由と正義と平和の基礎である...";

#if OPTION_GENERATE_VECTOR_OPS_STRING
static void vegravis_generate_vector_ops_string(lv_freetype_outline_event_param_t * param, char * buf,
                                                uint32_t buf_len);
#endif

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static void test_freetype_with_render_mode(lv_freetype_font_render_mode_t render_mode, const char * screenshot_name)
{
    /*Create a font*/
    lv_font_t * font_italic = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                      render_mode,
                                                      24,
                                                      LV_FREETYPE_FONT_STYLE_ITALIC);
    TEST_ASSERT_NOT_NULL(font_italic);
    lv_font_t * font_normal = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                      render_mode,
                                                      24,
                                                      LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NOT_NULL(font_normal);
    lv_font_t * font_normal_small = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                            render_mode,
                                                            12,
                                                            LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NOT_NULL(font_normal_small);

    /* Emoji is only supported in bitmap mode */
    lv_font_t * font_emoji = lv_freetype_font_create("../examples/libs/freetype/NotoColorEmoji-32.subset.ttf",
                                                     LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                     12,
                                                     LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NOT_NULL(font_emoji);

    lv_font_t * font_path_error = lv_freetype_font_create("ERROR_PATH", render_mode, 24,
                                                          LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NULL(font_path_error);

    font_path_error = lv_freetype_font_create("", render_mode, 24, LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NULL(font_path_error);

    font_path_error = lv_freetype_font_create(NULL, render_mode, 24, LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NULL(font_path_error);

    lv_font_t * font_size_error = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                          render_mode,
                                                          0,
                                                          LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NULL(font_size_error);

    /*Create style with the new font*/
    static lv_style_t style_italic;
    lv_style_init(&style_italic);
    lv_style_set_text_font(&style_italic, font_italic);
    lv_style_set_text_align(&style_italic, LV_TEXT_ALIGN_CENTER);

    static lv_style_t style_normal_with_outline;
    lv_style_init(&style_normal_with_outline);
    lv_style_set_text_font(&style_normal_with_outline, font_normal);
    lv_style_set_text_align(&style_normal_with_outline, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_outline_stroke_color(&style_normal_with_outline, lv_color_hex(0xff0000));
    lv_style_set_text_outline_stroke_width(&style_normal_with_outline, 4);
    lv_style_set_text_outline_stroke_opa(&style_normal_with_outline, LV_OPA_50);

    static lv_style_t style_normal_small;
    lv_style_init(&style_normal_small);
    lv_style_set_text_font(&style_normal_small, font_normal_small);

    static lv_style_t style_normal_emoji;
    lv_style_init(&style_normal_emoji);
    lv_style_set_text_font(&style_normal_emoji, font_emoji);
    lv_style_set_text_align(&style_normal_emoji, LV_TEXT_ALIGN_CENTER);

    /*Create a label with the new style*/
    lv_obj_t * label0 = lv_label_create(lv_screen_active());
    lv_obj_add_style(label0, &style_italic, 0);
    lv_obj_set_width(label0, lv_obj_get_width(lv_screen_active()) - 20);
    lv_label_set_text(label0, UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_CN);
    lv_obj_align(label0,  LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_obj_add_style(label1, &style_normal_with_outline, 0);
    lv_obj_set_width(label1, lv_obj_get_width(lv_screen_active()) - 20);
    lv_label_set_text(label1, UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_EN);
    lv_obj_align_to(label1, label0, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_t * label2 = lv_label_create(lv_screen_active());
    lv_obj_add_style(label2, &style_normal_small, 0);
    lv_obj_set_width(label2, lv_obj_get_width(lv_screen_active()) - 20);
    lv_label_set_text(label2, UNIVERSAL_DECLARATION_OF_HUMAN_RIGHTS_JP);
    lv_obj_align_to(label2, label1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    /* test emoji rendering
     * emoji font does not contain normal characters, use fallback to render them */
    font_emoji->fallback = font_normal;

    lv_obj_t * label_emoji = lv_label_create(lv_screen_active());
    lv_obj_add_style(label_emoji, &style_normal_emoji, 0);
    lv_obj_set_width(label_emoji, lv_obj_get_width(lv_screen_active()) - 20);
    lv_label_set_text(label_emoji, "FreeType Emoji test: 😀");
    lv_obj_align_to(label_emoji, label2, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    TEST_ASSERT_EQUAL_SCREENSHOT(screenshot_name);

    lv_obj_clean(lv_screen_active());
    lv_style_reset(&style_italic);
    lv_style_reset(&style_normal_with_outline);
    lv_style_reset(&style_normal_small);
    lv_style_reset(&style_normal_emoji);
    lv_freetype_font_delete(font_italic);
    lv_freetype_font_delete(font_normal);
    lv_freetype_font_delete(font_normal_small);
    lv_freetype_font_delete(font_emoji);
}

void test_freetype_render_bitmap(void)
{
    test_freetype_with_render_mode(LV_FREETYPE_FONT_RENDER_MODE_BITMAP, "libs/freetype_render_bitmap" EXT_NAME);
}

void test_freetype_render_outline(void)
{
    test_freetype_with_render_mode(LV_FREETYPE_FONT_RENDER_MODE_OUTLINE, "libs/freetype_render_outline" EXT_NAME);
}

#if OPTION_GENERATE_VECTOR_OPS_STRING
static void vegravis_generate_vector_ops_string(lv_freetype_outline_event_param_t * param, char * buf, uint32_t buf_len)
{
    float x, y, p1x, p1y, p2x, p2y;

    x = LV_FREETYPE_F26DOT6_TO_FLOAT(param->to.x);
    y = LV_FREETYPE_F26DOT6_TO_FLOAT(param->to.y);
    p1x = LV_FREETYPE_F26DOT6_TO_FLOAT(param->control1.x);
    p1y = LV_FREETYPE_F26DOT6_TO_FLOAT(param->control1.y);
    p2x = LV_FREETYPE_F26DOT6_TO_FLOAT(param->control2.x);
    p2y = LV_FREETYPE_F26DOT6_TO_FLOAT(param->control2.y);

    switch(param->type) {
        case LV_FREETYPE_OUTLINE_MOVE_TO:
            lv_snprintf(buf, buf_len, "MOVE, %f, %f, ", x, x);
            break;
        case LV_FREETYPE_OUTLINE_LINE_TO:
            lv_snprintf(buf, buf_len, "LINE, %f, %f, ", x, y);
            break;
        case LV_FREETYPE_OUTLINE_CONIC_TO:
            lv_snprintf(buf, buf_len, "QUAD, %f, %f, %f, %f, ", p1x, p1y, x, y);
            break;
        case LV_FREETYPE_OUTLINE_CUBIC_TO:
            lv_snprintf(buf, buf_len, "CUBI, %f, %f, %f, %f, %f, %f, ", p1x, p1y, p2x, p2y, x, y);
            break;
        case LV_FREETYPE_OUTLINE_END:
            lv_snprintf(buf, buf_len, "END, ");
            break;
        default:
            break;
    }
}
#endif

/**
 * Test kerning functionality with scalable FreeType fonts.
 * This test covers the FT_IS_SCALABLE and FT_Set_Pixel_Sizes code path
 * in freetype_get_glyph_dsc_cb when getting kerning information.
 */
void test_freetype_kerning(void)
{
    /* Create a font with kerning enabled using font_info */
    lv_font_info_t font_info;
    lv_freetype_init_font_info(&font_info);
    font_info.name = "./src/test_files/fonts/noto/NotoSansSC-Regular.ttf";
    font_info.size = 32;
    font_info.render_mode = LV_FREETYPE_FONT_RENDER_MODE_BITMAP;
    font_info.style = LV_FREETYPE_FONT_STYLE_NORMAL;
    font_info.kerning = LV_FONT_KERNING_NORMAL;

    lv_font_t * font_kerning = lv_freetype_font_create_with_info(&font_info);
    TEST_ASSERT_NOT_NULL(font_kerning);

    /* Create a font with kerning disabled */
    font_info.kerning = LV_FONT_KERNING_NONE;
    lv_font_t * font_no_kerning = lv_freetype_font_create_with_info(&font_info);
    TEST_ASSERT_NOT_NULL(font_no_kerning);

    /* Test glyph width with kerning - exercises the FT_Set_Pixel_Sizes code path */
    uint16_t width_kerning = lv_font_get_glyph_width(font_kerning, 'A', 'V');
    uint16_t width_no_kerning = lv_font_get_glyph_width(font_no_kerning, 'A', 'V');

    /* Both should return valid widths */
    TEST_ASSERT_GREATER_THAN(0, width_kerning);
    TEST_ASSERT_GREATER_THAN(0, width_no_kerning);

    /* Clean up */
    lv_freetype_font_delete(font_kerning);
    lv_freetype_font_delete(font_no_kerning);
}

/**
 * Test kerning with multiple font sizes to ensure FT_Set_Pixel_Sizes
 * is called correctly for scalable fonts when kerning is requested.
 */
void test_freetype_kerning_scalable_sizes(void)
{
    /* Test with multiple sizes to ensure pixel size is set correctly */
    const uint32_t sizes[] = {16, 24, 32, 48, 64};

    for(int i = 0; i < 5; i++) {
        lv_font_info_t font_info;
        lv_freetype_init_font_info(&font_info);
        font_info.name = "./src/test_files/fonts/noto/NotoSansSC-Regular.ttf";
        font_info.size = sizes[i];
        font_info.render_mode = LV_FREETYPE_FONT_RENDER_MODE_BITMAP;
        font_info.style = LV_FREETYPE_FONT_STYLE_NORMAL;
        font_info.kerning = LV_FONT_KERNING_NORMAL;

        lv_font_t * font = lv_freetype_font_create_with_info(&font_info);
        TEST_ASSERT_NOT_NULL(font);

        /* Test getting glyph width which triggers kerning lookup */
        /* This exercises the FT_IS_SCALABLE and FT_Set_Pixel_Sizes path */
        uint16_t width_V = lv_font_get_glyph_width(font, 'V', 'A');
        uint16_t width_A = lv_font_get_glyph_width(font, 'A', 'V');
        uint16_t width_T = lv_font_get_glyph_width(font, 'T', 'o');

        /* Width should be non-zero for valid glyphs */
        TEST_ASSERT_GREATER_THAN(0, width_V);
        TEST_ASSERT_GREATER_THAN(0, width_A);
        TEST_ASSERT_GREATER_THAN(0, width_T);

        lv_freetype_font_delete(font);
    }
}

/**
 * Test that font with kerning enabled handles gracefully even if
 * the font has limited kerning data.
 */
void test_freetype_no_kerning_info(void)
{
    lv_font_info_t font_info;
    lv_freetype_init_font_info(&font_info);
    font_info.name = "./src/test_files/fonts/noto/NotoSansSC-Regular.ttf";
    font_info.size = 24;
    font_info.render_mode = LV_FREETYPE_FONT_RENDER_MODE_BITMAP;
    font_info.style = LV_FREETYPE_FONT_STYLE_NORMAL;
    font_info.kerning = LV_FONT_KERNING_NORMAL;

    lv_font_t * font = lv_freetype_font_create_with_info(&font_info);
    TEST_ASSERT_NOT_NULL(font);

    /* Test glyph width with various character pairs to exercise kerning lookup */
    uint16_t width1 = lv_font_get_glyph_width(font, 'A', 'V');
    uint16_t width2 = lv_font_get_glyph_width(font, 'T', 'o');
    uint16_t width3 = lv_font_get_glyph_width(font, 'W', 'A');

    /* All widths should be valid (greater than 0) */
    TEST_ASSERT_GREATER_THAN(0, width1);
    TEST_ASSERT_GREATER_THAN(0, width2);
    TEST_ASSERT_GREATER_THAN(0, width3);

    /* Clean up */
    lv_freetype_font_delete(font);
}

#else

void setUp(void)
{
}

void tearDown(void)
{
}

void test_freetype_render_bitmap(void)
{
}

void test_freetype_render_outline(void)
{
}

void test_freetype_kerning(void)
{
}

void test_freetype_kerning_scalable_sizes(void)
{
}

void test_freetype_no_kerning_info(void)
{
}

#endif /*LV_USE_FREETYPE*/

#endif
