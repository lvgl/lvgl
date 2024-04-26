#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include <stdio.h>

static const char * color_formats[] = {
    "I1", /*7 I1*/
    "I2", /*8 I2*/
    "I4", /*9 I4*/
    "I8", /*10 I8*/
    "A1", /*11 A1*/
    "A2", /*12 A2*/
    "A4", /*13 A4*/
    "A8", /*14 A8*/
    "RGB565A8", /*20 RGB565A8*/
    "RGB565", /*18 RGB565*/
    "RGB888", /*15 RGB888*/
    "XRGB8888", /*17 XRGB8888*/
    "ARGB8888", /*16 ARGB8888*/
};

static const char * compressions[] = {
    "UNCOMPRESSED",
#if LV_BIN_DECODER_RAM_LOAD == 1
    "RLE",
    "LZ4"
#endif
};

static const char * modes[] = {
    "simple",
    "recolor",
    "rotate",
    "rotate_recolor",
};

static const int stride_align[] = {
    1,
    64,
};

LV_IMAGE_DECLARE(test_I1_NONE_align1);
LV_IMAGE_DECLARE(test_I2_NONE_align1);
LV_IMAGE_DECLARE(test_I4_NONE_align1);
LV_IMAGE_DECLARE(test_I8_NONE_align1);
LV_IMAGE_DECLARE(test_A1_NONE_align1);
LV_IMAGE_DECLARE(test_A2_NONE_align1);
LV_IMAGE_DECLARE(test_A4_NONE_align1);
LV_IMAGE_DECLARE(test_A8_NONE_align1);
LV_IMAGE_DECLARE(test_RGB565A8_NONE_align1);
LV_IMAGE_DECLARE(test_RGB565_NONE_align1);
LV_IMAGE_DECLARE(test_RGB888_NONE_align1);
LV_IMAGE_DECLARE(test_XRGB8888_NONE_align1);
LV_IMAGE_DECLARE(test_ARGB8888_NONE_align1);
LV_IMAGE_DECLARE(test_I1_RLE_align1);
LV_IMAGE_DECLARE(test_I2_RLE_align1);
LV_IMAGE_DECLARE(test_I4_RLE_align1);
LV_IMAGE_DECLARE(test_I8_RLE_align1);
LV_IMAGE_DECLARE(test_A1_RLE_align1);
LV_IMAGE_DECLARE(test_A2_RLE_align1);
LV_IMAGE_DECLARE(test_A4_RLE_align1);
LV_IMAGE_DECLARE(test_A8_RLE_align1);
LV_IMAGE_DECLARE(test_RGB565A8_RLE_align1);
LV_IMAGE_DECLARE(test_RGB565_RLE_align1);
LV_IMAGE_DECLARE(test_RGB888_RLE_align1);
LV_IMAGE_DECLARE(test_XRGB8888_RLE_align1);
LV_IMAGE_DECLARE(test_ARGB8888_RLE_align1);

LV_IMAGE_DECLARE(test_I1_LZ4_align1);
LV_IMAGE_DECLARE(test_I2_LZ4_align1);
LV_IMAGE_DECLARE(test_I4_LZ4_align1);
LV_IMAGE_DECLARE(test_I8_LZ4_align1);
LV_IMAGE_DECLARE(test_A1_LZ4_align1);
LV_IMAGE_DECLARE(test_A2_LZ4_align1);
LV_IMAGE_DECLARE(test_A4_LZ4_align1);
LV_IMAGE_DECLARE(test_A8_LZ4_align1);
LV_IMAGE_DECLARE(test_RGB565A8_LZ4_align1);
LV_IMAGE_DECLARE(test_RGB565_LZ4_align1);
LV_IMAGE_DECLARE(test_RGB888_LZ4_align1);
LV_IMAGE_DECLARE(test_XRGB8888_LZ4_align1);
LV_IMAGE_DECLARE(test_ARGB8888_LZ4_align1);

LV_IMAGE_DECLARE(test_I1_NONE_align64);
LV_IMAGE_DECLARE(test_I2_NONE_align64);
LV_IMAGE_DECLARE(test_I4_NONE_align64);
LV_IMAGE_DECLARE(test_I8_NONE_align64);
LV_IMAGE_DECLARE(test_A1_NONE_align64);
LV_IMAGE_DECLARE(test_A2_NONE_align64);
LV_IMAGE_DECLARE(test_A4_NONE_align64);
LV_IMAGE_DECLARE(test_A8_NONE_align64);
LV_IMAGE_DECLARE(test_RGB565A8_NONE_align64);
LV_IMAGE_DECLARE(test_RGB565_NONE_align64);
LV_IMAGE_DECLARE(test_RGB888_NONE_align64);
LV_IMAGE_DECLARE(test_XRGB8888_NONE_align64);
LV_IMAGE_DECLARE(test_ARGB8888_NONE_align64);

LV_IMAGE_DECLARE(test_I1_RLE_align64);
LV_IMAGE_DECLARE(test_I2_RLE_align64);
LV_IMAGE_DECLARE(test_I4_RLE_align64);
LV_IMAGE_DECLARE(test_I8_RLE_align64);
LV_IMAGE_DECLARE(test_A1_RLE_align64);
LV_IMAGE_DECLARE(test_A2_RLE_align64);
LV_IMAGE_DECLARE(test_A4_RLE_align64);
LV_IMAGE_DECLARE(test_A8_RLE_align64);
LV_IMAGE_DECLARE(test_RGB565A8_RLE_align64);
LV_IMAGE_DECLARE(test_RGB565_RLE_align64);
LV_IMAGE_DECLARE(test_RGB888_RLE_align64);
LV_IMAGE_DECLARE(test_XRGB8888_RLE_align64);
LV_IMAGE_DECLARE(test_ARGB8888_RLE_align64);

LV_IMAGE_DECLARE(test_I1_LZ4_align64);
LV_IMAGE_DECLARE(test_I2_LZ4_align64);
LV_IMAGE_DECLARE(test_I4_LZ4_align64);
LV_IMAGE_DECLARE(test_I8_LZ4_align64);
LV_IMAGE_DECLARE(test_A1_LZ4_align64);
LV_IMAGE_DECLARE(test_A2_LZ4_align64);
LV_IMAGE_DECLARE(test_A4_LZ4_align64);
LV_IMAGE_DECLARE(test_A8_LZ4_align64);
LV_IMAGE_DECLARE(test_RGB565A8_LZ4_align64);
LV_IMAGE_DECLARE(test_RGB565_LZ4_align64);
LV_IMAGE_DECLARE(test_RGB888_LZ4_align64);
LV_IMAGE_DECLARE(test_XRGB8888_LZ4_align64);
LV_IMAGE_DECLARE(test_ARGB8888_LZ4_align64);

static const void * c_array_images[sizeof(stride_align)][sizeof(compressions)][sizeof(color_formats)] = {
    {
        {
            &test_I1_NONE_align1,
            &test_I2_NONE_align1,
            &test_I4_NONE_align1,
            &test_I8_NONE_align1,
            &test_A1_NONE_align1,
            &test_A2_NONE_align1,
            &test_A4_NONE_align1,
            &test_A8_NONE_align1,
            &test_RGB565A8_NONE_align1,
            &test_RGB565_NONE_align1,
            &test_RGB888_NONE_align1,
            &test_XRGB8888_NONE_align1,
            &test_ARGB8888_NONE_align1,
        },
        {
            &test_I1_RLE_align1,
            &test_I2_RLE_align1,
            &test_I4_RLE_align1,
            &test_I8_RLE_align1,
            &test_A1_RLE_align1,
            &test_A2_RLE_align1,
            &test_A4_RLE_align1,
            &test_A8_RLE_align1,
            &test_RGB565A8_RLE_align1,
            &test_RGB565_RLE_align1,
            &test_RGB888_RLE_align1,
            &test_XRGB8888_RLE_align1,
            &test_ARGB8888_RLE_align1,
        },
        {
            &test_I1_LZ4_align1,
            &test_I2_LZ4_align1,
            &test_I4_LZ4_align1,
            &test_I8_LZ4_align1,
            &test_A1_LZ4_align1,
            &test_A2_LZ4_align1,
            &test_A4_LZ4_align1,
            &test_A8_LZ4_align1,
            &test_RGB565A8_LZ4_align1,
            &test_RGB565_LZ4_align1,
            &test_RGB888_LZ4_align1,
            &test_XRGB8888_LZ4_align1,
            &test_ARGB8888_LZ4_align1,
        }
    },
    {
        {
            &test_I1_NONE_align64,
            &test_I2_NONE_align64,
            &test_I4_NONE_align64,
            &test_I8_NONE_align64,
            &test_A1_NONE_align64,
            &test_A2_NONE_align64,
            &test_A4_NONE_align64,
            &test_A8_NONE_align64,
            &test_RGB565A8_NONE_align64,
            &test_RGB565_NONE_align64,
            &test_RGB888_NONE_align64,
            &test_XRGB8888_NONE_align64,
            &test_ARGB8888_NONE_align64,
        },
        {
            &test_I1_RLE_align64,
            &test_I2_RLE_align64,
            &test_I4_RLE_align64,
            &test_I8_RLE_align64,
            &test_A1_RLE_align64,
            &test_A2_RLE_align64,
            &test_A4_RLE_align64,
            &test_A8_RLE_align64,
            &test_RGB565A8_RLE_align64,
            &test_RGB565_RLE_align64,
            &test_RGB888_RLE_align64,
            &test_XRGB8888_RLE_align64,
            &test_ARGB8888_RLE_align64,
        },
        {
            &test_I1_LZ4_align64,
            &test_I2_LZ4_align64,
            &test_I4_LZ4_align64,
            &test_I8_LZ4_align64,
            &test_A1_LZ4_align64,
            &test_A2_LZ4_align64,
            &test_A4_LZ4_align64,
            &test_A8_LZ4_align64,
            &test_RGB565A8_LZ4_align64,
            &test_RGB565_LZ4_align64,
            &test_RGB888_LZ4_align64,
            &test_XRGB8888_LZ4_align64,
            &test_ARGB8888_LZ4_align64,
        }
    }
};

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
    if(rotate) lv_image_set_rotation(img, 450);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, name);
}

static void bin_image_create(bool rotate, bool recolor, int align, int compress)
{
    char name[32];
    char path[256];
    int stride = stride_align[align];
    for(unsigned i = 0; i < sizeof(color_formats) / sizeof(color_formats[0]); i++) {
        lv_snprintf(name, sizeof(name), "bin%s", color_formats[i]);
        lv_snprintf(path, sizeof(path), "A:test_images/stride_align%d/%s/test_%s.bin", stride, compressions[compress],
                    color_formats[i]);
        img_create(name, path, rotate, recolor);
    }
}

static void c_array_image_create(bool rotate, bool recolor, int align, int compress)
{
    char name[32];
    for(unsigned i = 0; i < sizeof(color_formats) / sizeof(color_formats[0]); i++) {
        lv_snprintf(name, sizeof(name), "%s%s", compressions[compress], color_formats[i]);
        const void * src = c_array_images[align][compress][i];
        img_create(name, src, rotate, recolor);
    }
}

void test_image_formats(void)
{
    for(unsigned align = 0; align <= 1; align++) {
        int stride = stride_align[align];
        for(unsigned mode = 0; mode <= 3; mode++) {
            bool rotate = mode & 0x02;
            bool recolor = mode & 0x01;
#if LV_BIN_DECODER_RAM_LOAD == 0
            if(rotate) continue;  /* Transform relies on LV_BIN_DECODER_RAM_LOAD to be enabled */
#endif
            /*Loop compressions array and do test.*/
            for(unsigned i = 0; i < sizeof(compressions) / sizeof(compressions[0]); i++) {
                char reference[256];
                bin_image_create(rotate, recolor, align, i);
                lv_snprintf(reference, sizeof(reference), "draw/bin_image_stride%d_%s_%s.png", stride, compressions[i], modes[mode]);
                TEST_ASSERT_EQUAL_SCREENSHOT(reference);
                lv_obj_clean(lv_screen_active());

                c_array_image_create(rotate, recolor, align, i);
                lv_snprintf(reference, sizeof(reference), "draw/c_array_image_stride%d_%s_%s.png", stride, compressions[i],
                            modes[mode]);
                TEST_ASSERT_EQUAL_SCREENSHOT(reference);
                lv_obj_clean(lv_screen_active());
            }
        }
    }
}

#endif
