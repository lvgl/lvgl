#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#if LV_USE_FONT_MANAGER \
    && LV_FONT_MONTSERRAT_14 && LV_FONT_MONTSERRAT_32 \
    && LV_USE_FREETYPE \
    && LV_USE_TINY_TTF && LV_TINY_TTF_FILE_SUPPORT

static lv_font_manager_t * g_font_manager = NULL;

typedef bool (*add_src_cb_t)(lv_font_manager_t * manager,
                             const char * name,
                             const void * src,
                             const lv_font_class_t * class_p);

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static void test_font_manager_src(add_src_cb_t add_src_cb)
{
    g_font_manager = lv_font_manager_create(2);
    TEST_ASSERT_NOT_NULL(g_font_manager);

    /* Register built-in font sources */
    lv_builtin_font_src_t builtin_font_src[3] = { 0 };
    builtin_font_src[0].font_p = &lv_font_montserrat_14;
    builtin_font_src[0].size = 14;
    builtin_font_src[1].font_p = &lv_font_montserrat_32;
    builtin_font_src[1].size = 32;

    /* IMPORTANT! Marking the end of the array */
    builtin_font_src[2].font_p = NULL;
    builtin_font_src[2].size = 0;

    bool add_src_result = add_src_cb(g_font_manager,
                                     "Montserrat",
                                     builtin_font_src,
                                     &lv_builtin_font_class);
    TEST_ASSERT_TRUE(add_src_result);

    /* Try to add the same source again, should fail */
    add_src_result = add_src_cb(g_font_manager,
                                "Montserrat",
                                builtin_font_src,
                                &lv_builtin_font_class);
    TEST_ASSERT_FALSE(add_src_result);

    /* Register FreeType font source */
    add_src_result = add_src_cb(g_font_manager,
                                "NotoColorEmoji",
                                PATH_PREFIX "lvgl/examples/libs/freetype/NotoColorEmoji-32.subset.ttf",
                                &lv_freetype_font_class);
    TEST_ASSERT_TRUE(add_src_result);


    /* Register TinyTTF font source */
    static const lv_tiny_ttf_font_src_t tiny_ttf_font_src = {
        .path = "A:lvgl/examples/libs/tiny_ttf/Ubuntu-Medium.ttf",
        .data = NULL,
        .data_size = 0,
        .cache_size = 0,
    };

    add_src_result = add_src_cb(g_font_manager,
                                "Ubuntu-Medium",
                                &tiny_ttf_font_src,
                                &lv_tiny_ttf_font_class);
    TEST_ASSERT_TRUE(add_src_result);

    /* Create font from font manager */
    lv_font_t * font = lv_font_manager_create_font(g_font_manager,
                                                   "Ubuntu-Medium,NotoColorEmoji,Montserrat,UNKNOWN_FONT",
                                                   LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                   32,
                                                   LV_FREETYPE_FONT_STYLE_NORMAL);

    TEST_ASSERT_NOT_NULL(font);

    /* Create label with the font */
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_font(label, font, 0);
    lv_label_set_text(label, "Hello Font Manager! 😀 " LV_SYMBOL_OK);
    lv_obj_center(label);

#ifndef NON_AMD64_BUILD
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/font_manager_1.lp32.png")
#else
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/font_manager_1.lp64.png")
#endif

    /* Should not be deleted successfully, because it is used by the label */
    bool delete_result = lv_font_manager_delete(g_font_manager);
    TEST_ASSERT_FALSE(delete_result);

    lv_obj_delete(label);
    lv_font_manager_delete_font(g_font_manager, font);

    delete_result = lv_font_manager_delete(g_font_manager);
    TEST_ASSERT_TRUE(delete_result);
    g_font_manager = NULL;
}

void test_font_manager_src_static(void)
{
    test_font_manager_src(lv_font_manager_add_src_static);
}

void test_font_manager_src_not_static(void)
{
    test_font_manager_src(lv_font_manager_add_src);
}

#else

void setUp(void)
{
}

void tearDown(void)
{
}

void test_font_manager_src_static(void)
{
}

void test_font_manager_src_not_static(void)
{
}

#endif

#endif /* LV_BUILD_TEST */
