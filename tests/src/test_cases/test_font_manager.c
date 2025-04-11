#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#if LV_USE_FONT_MANAGER \
    && LV_FONT_MONTSERRAT_14 && LV_FONT_MONTSERRAT_32 \
    && LV_USE_FREETYPE \
    && LV_USE_TINY_TTF && LV_TINY_TTF_FILE_SUPPORT

/**
 * There are some differences between the rendering of FreeType in 64-bit and 32-bit,
 * so we compare them separately here.
 */
#ifndef NON_AMD64_BUILD
    #define EXT_NAME ".lp64.png"
#else
    #define EXT_NAME ".lp32.png"
#endif

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
    static lv_builtin_font_src_t builtin_font_src[3] = { 0 };
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
                                "NotoSansSC-Regular",
                                "./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                &lv_freetype_font_class);
    TEST_ASSERT_TRUE(add_src_result);

    /* Register TinyTTF font source */
    extern const uint8_t test_ubuntu_font[];
    extern size_t test_ubuntu_font_size;
    static lv_tiny_ttf_font_src_t tiny_ttf_font_data_src = { 0 };
    tiny_ttf_font_data_src.data = test_ubuntu_font;
    tiny_ttf_font_data_src.data_size = test_ubuntu_font_size;

    add_src_result = add_src_cb(g_font_manager,
                                "Ubuntu-Medium",
                                &tiny_ttf_font_data_src,
                                &lv_tiny_ttf_font_class);
    TEST_ASSERT_TRUE(add_src_result);

    static lv_tiny_ttf_font_src_t tiny_ttf_font_file_src = { 0 };
    tiny_ttf_font_file_src.path = "A:src/test_files/fonts/noto/NotoSansSC-Regular.ttf";

    add_src_result = add_src_cb(g_font_manager,
                                "NotoSansSC-Regular-2",
                                &tiny_ttf_font_file_src,
                                &lv_tiny_ttf_font_class);
    TEST_ASSERT_TRUE(add_src_result);

    /* Register binary font source */
    static const lv_binfont_font_src_t binfont_font_file_src = {
        .font_size = 20,
        .path = "A:src/test_assets/test_font_3.fnt",
        .buffer = NULL,
        .buffer_size = 0,
    };

    add_src_result = add_src_cb(g_font_manager,
                                "RobotoMono-Regular-file",
                                &binfont_font_file_src,
                                &lv_binfont_font_class);
    TEST_ASSERT_TRUE(add_src_result);

    extern uint8_t const test_font_3_buf[4892];
    static lv_binfont_font_src_t binfont_font_buffer_src = { 0 };
    binfont_font_buffer_src.font_size = 20;
    binfont_font_buffer_src.buffer = test_font_3_buf;
    binfont_font_buffer_src.buffer_size = sizeof(test_font_3_buf);

    add_src_result = add_src_cb(g_font_manager,
                                "RobotoMono-Regular-buffer",
                                &binfont_font_buffer_src,
                                &lv_binfont_font_class);
    TEST_ASSERT_TRUE(add_src_result);

    /* Create font from font manager */

    /* Try to create font with unknown name, should fail */
    lv_font_t * font_unknown = lv_font_manager_create_font(g_font_manager,
                                                           "UNKNOWN_FONT_NAME",
                                                           0,
                                                           10,
                                                           0,
                                                           LV_FONT_KERNING_NONE);
    TEST_ASSERT_NULL(font_unknown);

    lv_font_t * font_14 = lv_font_manager_create_font(g_font_manager,
                                                      "NotoSansSC-Regular,Ubuntu-Medium,Montserrat,UNKNOWN_FONT_NAME",
                                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                      14,
                                                      LV_FREETYPE_FONT_STYLE_NORMAL,
                                                      LV_FONT_KERNING_NONE);
    TEST_ASSERT_NOT_NULL(font_14);

    lv_font_t * font_32 = lv_font_manager_create_font(g_font_manager,
                                                      "Ubuntu-Medium,NotoSansSC-Regular,Montserrat",
                                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                      32,
                                                      LV_FREETYPE_FONT_STYLE_NORMAL,
                                                      LV_FONT_KERNING_NONE);
    TEST_ASSERT_NOT_NULL(font_32);

    lv_font_t * font_40 = lv_font_manager_create_font(g_font_manager,
                                                      "Ubuntu-Medium,NotoSansSC-Regular-2,Montserrat",
                                                      0,
                                                      40,
                                                      0,
                                                      LV_FONT_KERNING_NONE);
    TEST_ASSERT_NOT_NULL(font_40);

    lv_font_t * font_file_20 = lv_font_manager_create_font(g_font_manager,
                                                           "RobotoMono-Regular-file,NotoSansSC-Regular-2",
                                                           0,
                                                           20,
                                                           0,
                                                           LV_FONT_KERNING_NONE);
    TEST_ASSERT_NOT_NULL(font_file_20);

    lv_font_t * font_buffer_20 = lv_font_manager_create_font(g_font_manager,
                                                             "RobotoMono-Regular-buffer,NotoSansSC-Regular-2",
                                                             0,
                                                             20,
                                                             0,
                                                             LV_FONT_KERNING_NONE);
    TEST_ASSERT_NOT_NULL(font_file_20);

    /* Create label with the font */
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label,
                      "这是一段中文。\n"
                      "This is a English text.\n"
                      "Symbols: " LV_SYMBOL_OK LV_SYMBOL_CLOSE);
    lv_obj_center(label);

    lv_obj_set_style_text_font(label, font_14, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/font_manager_1" EXT_NAME);

    lv_obj_set_style_text_font(label, font_32, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/font_manager_2" EXT_NAME);

    lv_obj_set_style_text_font(label, font_40, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/font_manager_3" EXT_NAME);

    /* Freetype fonts have not been tested, so there is no need to distinguish and process images */
    lv_obj_set_style_text_font(label, font_file_20, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/font_manager_4.png");

    /* Rendered images should be the same */
    lv_obj_set_style_text_font(label, font_buffer_20, 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/font_manager_4.png");

    /* Should not be deleted successfully, because it is used by the label */
    bool delete_result = lv_font_manager_delete(g_font_manager);
    TEST_ASSERT_FALSE(delete_result);

    lv_obj_delete(label);
    lv_font_manager_delete_font(g_font_manager, font_14);
    lv_font_manager_delete_font(g_font_manager, font_32);
    lv_font_manager_delete_font(g_font_manager, font_40);
    lv_font_manager_delete_font(g_font_manager, font_file_20);
    lv_font_manager_delete_font(g_font_manager, font_buffer_20);

    /* Trying to delete a font that was not created by the font manager, it needs to handle this situation */
    lv_font_manager_delete_font(g_font_manager, (lv_font_t *)LV_FONT_DEFAULT);
    lv_font_manager_delete_font(g_font_manager, (lv_font_t *)&lv_font_montserrat_32);

    delete_result = lv_font_manager_delete(g_font_manager);
    TEST_ASSERT_TRUE(delete_result);
    g_font_manager = NULL;
}

void test_font_manager_src_normal(void)
{
    test_font_manager_src(lv_font_manager_add_src);
}

void test_font_manager_src_static(void)
{
    test_font_manager_src(lv_font_manager_add_src_static);
}

#else

void setUp(void)
{
}

void tearDown(void)
{
}

void test_font_manager_src_normal(void)
{
}

void test_font_manager_src_static(void)
{
}

#endif

#endif /* LV_BUILD_TEST */
