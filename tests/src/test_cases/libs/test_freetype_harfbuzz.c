#if LV_BUILD_TEST
#include "lvgl.h"
#include "src/font/freetype/lv_freetype_private.h"
#include "src/font/freetype/lv_freetype_harfbuzz.h"

#include "unity/unity.h"

#if LV_USE_FREETYPE && LV_USE_HARFBUZZ

#ifndef NON_AMD64_BUILD
    #define EXT_NAME ".lp64.png"
#else
    #define EXT_NAME ".lp32.png"
#endif

#define DEVANAGARI_FONT_PATH "../examples/libs/harfbuzz/NotoSansDevanagari-Regular.subset.ttf"

static lv_font_t * font_devanagari;

void setUp(void)
{
    font_devanagari = lv_freetype_font_create(DEVANAGARI_FONT_PATH,
                                              LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                              32,
                                              LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NOT_NULL(font_devanagari);
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
    if(font_devanagari) {
        lv_freetype_font_delete(font_devanagari);
        font_devanagari = NULL;
    }
}

void test_freetype_font_is_harfbuzz(void)
{
    TEST_ASSERT_TRUE(lv_freetype_is_harfbuzz_font(font_devanagari));

    /*The per-font toggle disables and re-enables shaping*/
    lv_freetype_font_set_harfbuzz(font_devanagari, false);
    TEST_ASSERT_FALSE(lv_freetype_is_harfbuzz_font(font_devanagari));
    lv_freetype_font_set_harfbuzz(font_devanagari, true);
    TEST_ASSERT_TRUE(lv_freetype_is_harfbuzz_font(font_devanagari));

    /*Built-in bitmap fonts are never HarfBuzz fonts*/
    TEST_ASSERT_FALSE(lv_freetype_is_harfbuzz_font(&lv_font_montserrat_14));
}

void test_freetype_harfbuzz_shapes_conjuncts(void)
{
    /*"ksha": KA + VIRAMA + SSA (3 codepoints) fuses into fewer glyphs than
     *codepoints when shaping works. Without shaping each codepoint maps to
     *its own glyph.*/
    const char * ksha = "क्ष";
    lv_hb_shaped_text_t * shaped = lv_hb_shape_text(font_devanagari, ksha, strlen(ksha), LV_BASE_DIR_AUTO);
    TEST_ASSERT_NOT_NULL(shaped);
    TEST_ASSERT_LESS_THAN_UINT32(3, shaped->count);
    lv_hb_shaped_text_destroy(shaped);
}

void test_freetype_harfbuzz_cluster_mapping(void)
{
    /*"ki": KA + vowel sign I. The matra is displayed before the consonant
     *but both glyphs must keep the cluster of the syllable start so that
     *cursor positioning maps back to the right character.*/
    const char * ki = "कि";
    lv_hb_shaped_text_t * shaped = lv_hb_shape_text(font_devanagari, ki, strlen(ki), LV_BASE_DIR_AUTO);
    TEST_ASSERT_NOT_NULL(shaped);
    TEST_ASSERT_GREATER_THAN_UINT32(0, shaped->count);
    for(uint32_t i = 0; i < shaped->count; i++) {
        TEST_ASSERT_EQUAL_UINT32(0, shaped->glyphs[i].cluster);
    }
    lv_hb_shaped_text_destroy(shaped);
}

static lv_obj_t * add_label(lv_obj_t * parent, const char * text)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, font_devanagari, 0);
    lv_label_set_text(label, text);
    return label;
}

static void create_devanagari_labels(void)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, lv_pct(90), LV_SIZE_CONTENT);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(cont, 8, 0);

    add_label(cont, "नमस्ते दुनिया");           /*Hello World*/
    add_label(cont, "क्ष त्र ज्ञ श्र");          /*conjuncts*/
    add_label(cont, "कि की कु कू के कै को कौ");  /*matra reordering*/
    add_label(cont, "स्त्री विद्या राष्ट्र");     /*half-forms and reph*/
    add_label(cont, "हिन्दी");                  /*the string from issue #10370*/
    add_label(cont, "मराठी भाषा ळ");            /*Marathi, incl. retroflex LLA*/
}

void test_freetype_harfbuzz_render_devanagari(void)
{
    create_devanagari_labels();
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/freetype_harfbuzz_devanagari" EXT_NAME);
}

void test_freetype_harfbuzz_render_disabled(void)
{
    /*With shaping disabled the same text must still render (unshaped),
     *using the character-by-character path*/
    lv_freetype_font_set_harfbuzz(font_devanagari, false);
    create_devanagari_labels();
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/freetype_harfbuzz_disabled" EXT_NAME);
}

#else /*LV_USE_FREETYPE && LV_USE_HARFBUZZ*/

void setUp(void)
{
}

void tearDown(void)
{
}

void test_freetype_font_is_harfbuzz(void)
{
}

void test_freetype_harfbuzz_shapes_conjuncts(void)
{
}

void test_freetype_harfbuzz_cluster_mapping(void)
{
}

void test_freetype_harfbuzz_render_devanagari(void)
{
}

void test_freetype_harfbuzz_render_disabled(void)
{
}

#endif /*LV_USE_FREETYPE && LV_USE_HARFBUZZ*/

#endif /*LV_BUILD_TEST*/
