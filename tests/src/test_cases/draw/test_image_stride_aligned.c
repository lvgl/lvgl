#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    lv_obj_clean(lv_screen_active());

}

void tearDown(void)
{
    /* Function run after every test */
}

static void img_create(const char * name, const void * img_src, bool rotate, bool recolor)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, 120, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(cont, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
    lv_obj_set_style_pad_all(cont, 5, 0);

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * img = lv_image_create(cont);
    lv_image_set_src(img, img_src);
    lv_obj_set_style_image_recolor(img, lv_palette_main(LV_PALETTE_RED), 0);   /*For A8*/
    if(recolor) lv_obj_set_style_image_recolor_opa(img, LV_OPA_70, 0);
    if(rotate) lv_image_set_angle(img, 450);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, name);
}

static void bin_image_create(bool rotate, bool recolor)
{
    img_create("binI1", "A:test_images/stride_align64/UNCOMPRESSED/test_I1.bin", rotate, recolor);
    img_create("binI2", "A:test_images/stride_align64/UNCOMPRESSED/test_I2.bin", rotate, recolor);
    img_create("binI4", "A:test_images/stride_align64/UNCOMPRESSED/test_I4.bin", rotate, recolor);
    img_create("binI8", "A:test_images/stride_align64/UNCOMPRESSED/test_I8.bin", rotate, recolor);
    img_create("binA1", "A:test_images/stride_align64/UNCOMPRESSED/test_A1.bin", rotate, recolor);
    img_create("binA2", "A:test_images/stride_align64/UNCOMPRESSED/test_A2.bin", rotate, recolor);
    img_create("binA4", "A:test_images/stride_align64/UNCOMPRESSED/test_A4.bin", rotate, recolor);
    img_create("binA8", "A:test_images/stride_align64/UNCOMPRESSED/test_A8.bin", rotate, recolor);
    img_create("binRGB565A8", "A:test_images/stride_align64/UNCOMPRESSED/test_RGB565A8.bin", rotate, recolor);
    img_create("binRGB565", "A:test_images/stride_align64/UNCOMPRESSED/test_RGB565.bin", rotate, recolor);
    img_create("binRGB888", "A:test_images/stride_align64/UNCOMPRESSED/test_RGB888.bin", rotate, recolor);
    img_create("binXRGB8888", "A:test_images/stride_align64/UNCOMPRESSED/test_XRGB8888.bin", rotate, recolor);
    img_create("binARGB8888", "A:test_images/stride_align64/UNCOMPRESSED/test_ARGB8888.bin", rotate, recolor);
}

void test_image_aligned_decode(void)
{
    bin_image_create(false, false);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/image_stride_aligned_simple.png");
}

void test_image_aligned_decode_rotate(void)
{
    bin_image_create(true, false);
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/image_stride_aligned_rotated.png");
}

void test_image_aligned_decode_recolor(void)
{
    bin_image_create(false, true);
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/image_stride_aligned_recolor.png");
}

void test_image_aligned_decode_rotate_and_recolor(void)
{
    bin_image_create(true, true);
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/image_stride_aligned_rotate_and_recolor.png");
}

#endif
