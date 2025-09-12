#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#define TEST_COLORKEY_RECOLOR 1
#define TEST_COLORKEY_TRANSFORM 2

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

void test_image_colorkey(void)
{
    static lv_image_colorkey_t colorkey;
    colorkey.low = lv_color_hex(0x000000);
    colorkey.high = lv_color_hex(0x55ff66);

    LV_IMAGE_DECLARE(test_RGB565A8_NONE_align64);
    LV_IMAGE_DECLARE(test_RGB565_NONE_align64);
    LV_IMAGE_DECLARE(test_RGB888_NONE_align64);
    LV_IMAGE_DECLARE(test_XRGB8888_NONE_align64);
    LV_IMAGE_DECLARE(test_ARGB8888_NONE_align64);

    const lv_image_dsc_t * img_dscs[] = {
        &test_RGB565A8_NONE_align64,
        &test_RGB565_NONE_align64,
        &test_RGB888_NONE_align64,
        &test_XRGB8888_NONE_align64,
        &test_ARGB8888_NONE_align64,
    };
    const char * names[] = {
        "RGB565A8",
        "RGB565",
        "RGB888",
        "XRGB8888",
        "ARGB8888",
    };
    int count = sizeof(img_dscs) / sizeof(img_dscs[0]);
    int item = 3; // colorkey, colorkey + recolor, colorkey + transform

    for(int i = 0; i < count * item; i++) {
        lv_obj_t * cont =  lv_obj_create(lv_screen_active());
        lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_style_bg_color(cont, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
        lv_obj_set_style_bg_opa(cont, 255, 0);
        lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

        lv_obj_t * img = lv_image_create(cont);
        lv_image_set_src(img, img_dscs[i / item]);
        if(i % item == TEST_COLORKEY_RECOLOR) {
            lv_obj_set_style_image_recolor(img, lv_palette_main(LV_PALETTE_RED), 0);
            lv_obj_set_style_image_recolor_opa(img, LV_OPA_50, 0);
        }
        if(i % item == TEST_COLORKEY_TRANSFORM) {
            lv_image_set_scale(img, 250);
            lv_image_set_rotation(img, 300);
        }
        lv_obj_set_style_image_colorkey(img, &colorkey, 0);
        lv_obj_set_style_opa(img, 255, 0);

        lv_obj_t * label = lv_label_create(cont);
        lv_label_set_text(label, names[i / item]);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/image_colorkey.png");
}

#endif
