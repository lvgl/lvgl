#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "../../src/libs/lodepng/lodepng.h"

#include "unity/unity.h"

#include <stdio.h>

/* The display buffer is filled by hand, so these tests check only the
 * screenshot export and comparison, independently of the draw unit. */

#define BAND_H (LV_VER_RES / 4)

#define WRITTEN_PNG "screenshot_alpha/written.png"

typedef struct {
    uint8_t r, g, b, a;
} rgba_t;

/* Straight (non-premultiplied) colors chosen so that premultiplying and
 * un-premultiplying them is exact. */
static const rgba_t bands[4] = {
    {0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff, 0xff, 0x77},
    {0xff, 0x00, 0x00, 0x80},
    {0x00, 0x00, 0x00, 0x00},
};

void setUp(void)
{
}

void tearDown(void)
{
    lv_display_set_color_format(lv_display_get_default(), LV_COLOR_FORMAT_XRGB8888);
}

static lv_draw_buf_t * fill_display(lv_color_format_t cf, bool premultiply, uint8_t x_byte)
{
#if LV_USE_DRAW_NANOVG
    /* The NanoVG headless test display supports XRGB8888 and ARGB8888 only */
    if(cf == LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED) TEST_IGNORE_MESSAGE("NanoVG's test display has no premultiplied format");
#endif
    lv_display_t * disp = lv_display_get_default();
    lv_display_set_color_format(disp, cf);
    lv_draw_buf_t * buf = lv_display_get_buf_active(disp);

    for(int32_t y = 0; y < (int32_t)buf->header.h; y++) {
        const rgba_t c = bands[y / BAND_H];
        uint8_t a = cf == LV_COLOR_FORMAT_XRGB8888 ? x_byte : c.a;
        uint8_t r = premultiply ? LV_UDIV255(c.r * c.a) : c.r;
        uint8_t g = premultiply ? LV_UDIV255(c.g * c.a) : c.g;
        uint8_t b = premultiply ? LV_UDIV255(c.b * c.a) : c.b;
        uint8_t * row = buf->data + y * buf->header.stride;
        for(int32_t x = 0; x < (int32_t)buf->header.w; x++) {
            row[x * 4 + 0] = b;
            row[x * 4 + 1] = g;
            row[x * 4 + 2] = r;
            row[x * 4 + 3] = a;
        }
    }
    return buf;
}

static void assert_written_png_rgba(const rgba_t * expected)
{
    char path[256];
    lv_snprintf(path, sizeof(path), "%s%s", REF_IMGS_PATH, WRITTEN_PNG);
    remove(path);

    /* With no reference image, the comparison writes the screen to it */
    TEST_ASSERT_EQUAL(LV_TEST_SCREENSHOT_RESULT_PASSED, lv_test_screenshot_compare_core(WRITTEN_PNG));

    /* LVGL's lodepng decodes into an lv_draw_buf_t, with RGBA byte order */
    lv_draw_buf_t * png = NULL;
    unsigned w, h;
    TEST_ASSERT_EQUAL_UINT(0, lodepng_decode32_file((unsigned char **)&png, &w, &h, path));
    remove(path);

    /* Copy before asserting: a failed assert jumps out and would leak png */
    uint8_t pixels[4][4];
    for(int32_t i = 0; i < 4; i++) {
        lv_memcpy(pixels[i], png->data + (i * BAND_H + BAND_H / 2) * png->header.stride, 4);
    }
    lv_draw_buf_destroy(png);

    for(int32_t i = 0; i < 4; i++) {
        const uint8_t * px = pixels[i];
        char msg[64];
        lv_snprintf(msg, sizeof(msg), "band %d", (int)i);
        TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected[i].r, px[0], msg);
        TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected[i].g, px[1], msg);
        TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected[i].b, px[2], msg);
        TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected[i].a, px[3], msg);
    }
}

void test_screenshot_straight_argb8888_matches_reference(void)
{
    fill_display(LV_COLOR_FORMAT_ARGB8888, false, 0);
    TEST_ASSERT_EQUAL(LV_TEST_SCREENSHOT_RESULT_PASSED,
                      lv_test_screenshot_compare_core("screenshot_alpha/bands.png"));
}

void test_screenshot_premultiplied_format_matches_straight_reference(void)
{
    fill_display(LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED, true, 0);
    TEST_ASSERT_EQUAL(LV_TEST_SCREENSHOT_RESULT_PASSED,
                      lv_test_screenshot_compare_core("screenshot_alpha/bands.png"));
}

void test_screenshot_alpha_mismatch_is_detected(void)
{
    /* Same RGB as the reference, but the reference is fully opaque */
    fill_display(LV_COLOR_FORMAT_ARGB8888, false, 0);
    TEST_ASSERT_EQUAL(LV_TEST_SCREENSHOT_RESULT_FAILED,
                      lv_test_screenshot_compare_core("screenshot_alpha/bands_opaque.png"));
}

void test_screenshot_xrgb8888_ignores_x_byte(void)
{
    fill_display(LV_COLOR_FORMAT_XRGB8888, false, 0x00);
    TEST_ASSERT_EQUAL(LV_TEST_SCREENSHOT_RESULT_PASSED,
                      lv_test_screenshot_compare_core("screenshot_alpha/bands_opaque.png"));
}

#if LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE

void test_screenshot_written_png_from_premultiplied_format_is_straight(void)
{
    fill_display(LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED, true, 0);
    assert_written_png_rgba(bands);
}

void test_screenshot_written_png_from_xrgb8888_is_opaque(void)
{
    fill_display(LV_COLOR_FORMAT_XRGB8888, false, 0x00);
    rgba_t opaque[4];
    for(int32_t i = 0; i < 4; i++) {
        opaque[i] = bands[i];
        opaque[i].a = 0xff;
    }
    assert_written_png_rgba(opaque);
}

#endif

#endif
