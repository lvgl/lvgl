#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include "lv_test_helpers.h"

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

static void create_images(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_clean(screen);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    /* PNG array */
    LV_IMAGE_DECLARE(test_img_lvgl_logo_png);
    create_image_item(screen, &test_img_lvgl_logo_png, "Array");

    /* 32 bit PNG file */
    create_image_item(screen, "A:src/test_assets/test_img_lvgl_logo.png", "File (32 bit)");

    /* No extension PNG file */
    create_image_item(screen, "A:src/test_assets/test_img_lvgl_logo_png_no_ext", "File (32 bit) No Extension");

    /* 8 bit palette PNG file */
    create_image_item(screen, "A:src/test_assets/test_img_lvgl_logo_8bit_palette.png", "File (8 bit palette)");
}

void test_libpng_1(void)
{
    /* Temporarily remove lodepng decoder */
    lv_lodepng_deinit();

    create_images();

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/png_1.png");

    size_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        create_images();

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/png_1.png");

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 32);

    /* Re-add lodepng decoder */
    lv_lodepng_init();
}

#endif
