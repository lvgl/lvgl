#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
    lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_scr_act(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    lv_obj_clean(lv_scr_act());

}

void tearDown(void)
{
    /* Function run after every test */
}


static void img_create(const char * name, const void * img_src, bool rotate, bool recolor)
{
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
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


void test_image_built_in_decode(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_i4);
    LV_IMAGE_DECLARE(test_image_cogwheel_a8);
    LV_IMAGE_DECLARE(test_image_cogwheel_rgb565);
    LV_IMAGE_DECLARE(test_image_cogwheel_rgb565a8);
    LV_IMAGE_DECLARE(test_image_cogwheel_xrgb8888);
    LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);

    img_create("I4", &test_image_cogwheel_i4, false, false);
    img_create("A8", &test_image_cogwheel_a8, false, false);
    img_create("RGB565", &test_image_cogwheel_rgb565, false, false);
    img_create("RGB565A8", &test_image_cogwheel_rgb565a8, false, false);
    img_create("XRGB8888", &test_image_cogwheel_xrgb8888, false, false);
    img_create("ARGB8888", &test_image_cogwheel_argb8888, false, false);

    img_create("binI1", "A:src/test_files/binimages/cogwheel.I1.bin", false, false);
    img_create("binI2", "A:src/test_files/binimages/cogwheel.I2.bin", false, false);
    img_create("binI4", "A:src/test_files/binimages/cogwheel.I4.bin", false, false);
    img_create("binI8", "A:src/test_files/binimages/cogwheel.I8.bin", false, false);
    img_create("binA8", "A:src/test_files/binimages/cogwheel.A8.bin", false, false);
    img_create("binRGB565A8", "A:src/test_files/binimages/cogwheel.RGB565A8.bin", false, false);
    img_create("binRGB565", "A:src/test_files/binimages/cogwheel.RGB565.bin", false, false);
    img_create("binRGB888", "A:src/test_files/binimages/cogwheel.RGB888.bin", false, false);
    img_create("binXRGB8888", "A:src/test_files/binimages/cogwheel.XRGB8888.bin", false, false);
    img_create("binARGB8888", "A:src/test_files/binimages/cogwheel.ARGB8888.bin", false, false);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/image_format_simple.png");
}

void test_image_built_in_decode_rotate(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_i4);
    LV_IMAGE_DECLARE(test_image_cogwheel_a8);
    LV_IMAGE_DECLARE(test_image_cogwheel_rgb565);
    LV_IMAGE_DECLARE(test_image_cogwheel_rgb565a8);
    LV_IMAGE_DECLARE(test_image_cogwheel_xrgb8888);
    LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);

    img_create("I4", &test_image_cogwheel_i4, true, false);
    img_create("A8", &test_image_cogwheel_a8, true, false);
    img_create("RGB565", &test_image_cogwheel_rgb565, true, false);
    img_create("RGB565A8", &test_image_cogwheel_rgb565a8, true, false);
    img_create("XRGB8888", &test_image_cogwheel_xrgb8888, true, false);
    img_create("ARGB8888", &test_image_cogwheel_argb8888, true, false);

    /*Only A8 is read to ram thus can rotate for now*/
    img_create("binA8", "A:src/test_files/binimages/cogwheel.A8.bin", true, false);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/image_format_rotated.png");
}

void test_image_built_in_decode_recolor(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_i4);
    LV_IMAGE_DECLARE(test_image_cogwheel_a8);
    LV_IMAGE_DECLARE(test_image_cogwheel_rgb565);
    LV_IMAGE_DECLARE(test_image_cogwheel_rgb565a8);
    LV_IMAGE_DECLARE(test_image_cogwheel_xrgb8888);
    LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);

    img_create("I4", &test_image_cogwheel_i4, false, true);
    img_create("A8", &test_image_cogwheel_a8, false, true);
    img_create("RGB565", &test_image_cogwheel_rgb565, false, true);
    img_create("RGB565A8", &test_image_cogwheel_rgb565a8, false, true);
    img_create("XRGB8888", &test_image_cogwheel_xrgb8888, false, true);
    img_create("ARGB8888", &test_image_cogwheel_argb8888, false, true);

    img_create("binI1", "A:src/test_files/binimages/cogwheel.I1.bin", false, true);
    img_create("binI2", "A:src/test_files/binimages/cogwheel.I2.bin", false, true);
    img_create("binI4", "A:src/test_files/binimages/cogwheel.I4.bin", false, true);
    img_create("binI8", "A:src/test_files/binimages/cogwheel.I8.bin", false, true);
    img_create("binA8", "A:src/test_files/binimages/cogwheel.A8.bin", false, true);
    img_create("binRGB565A8", "A:src/test_files/binimages/cogwheel.RGB565A8.bin", false, true);
    img_create("binRGB565", "A:src/test_files/binimages/cogwheel.RGB565.bin", false, true);
    img_create("binRGB888", "A:src/test_files/binimages/cogwheel.RGB888.bin", false, true);
    img_create("binXRGB8888", "A:src/test_files/binimages/cogwheel.XRGB8888.bin", false, true);
    img_create("binARGB8888", "A:src/test_files/binimages/cogwheel.ARGB8888.bin", false, true);


    TEST_ASSERT_EQUAL_SCREENSHOT("draw/image_format_recolor.png");
}

void test_image_built_in_decode_rotate_and_recolor(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_i4);
    LV_IMAGE_DECLARE(test_image_cogwheel_a8);
    LV_IMAGE_DECLARE(test_image_cogwheel_rgb565);
    LV_IMAGE_DECLARE(test_image_cogwheel_rgb565a8);
    LV_IMAGE_DECLARE(test_image_cogwheel_xrgb8888);
    LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);

    img_create("I4", &test_image_cogwheel_i4, true, true);
    img_create("A8", &test_image_cogwheel_a8, true, true);
    img_create("RGB565", &test_image_cogwheel_rgb565, true, true);
    img_create("RGB565A8", &test_image_cogwheel_rgb565a8, true, true);
    img_create("XRGB8888", &test_image_cogwheel_xrgb8888, true, true);
    img_create("ARGB8888", &test_image_cogwheel_argb8888, true, true);

    /*Only A8 is read to ram thus can rotate for now*/
    img_create("binA8", "A:src/test_files/binimages/cogwheel.A8.bin", true, true);


    TEST_ASSERT_EQUAL_SCREENSHOT("draw/image_format_rotate_and_recolor.png");
}




#endif
