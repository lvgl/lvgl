#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

static void create_image_item(lv_obj_t * parent, const void * src, const char * text)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, 300, 200);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * img = lv_image_create(cont);
    lv_image_set_src(img, src);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, text);
}

static void create_webp_images(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_clean(screen);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    /* Lossy WebP file */
    create_image_item(screen, "A:src/test_assets/test_img_lvgl_logo_lossy.webp", "Lossy WebP");

    /* Lossless WebP file */
    create_image_item(screen, "A:src/test_assets/test_img_lvgl_logo_lossless.webp", "Lossless WebP");

    /* Transparent WebP file */
    create_image_item(screen, "A:src/test_assets/test_img_lvgl_logo_transparent.webp", "Transparent WebP");

    /* Corrupted WebP file : "WEBP" -> "WEDP" */
    create_image_item(screen, "A:src/test_assets/test_img_lvgl_logo_corrupted.webp", "Corrupted WebP");
}

void test_libwebp_1(void)
{
    create_webp_images();

    /* Verify initial rendering */
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/webp_1.png");

    /* Test memory stability */
    size_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 50; i++) {
        create_webp_images();

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }

    /* Verify rendering after multiple refreshes */
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/webp_1.png");

    /* Check for memory leaks */
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 128);
}

#endif
