#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

void test_rotate90_RGB565(void)
{
    uint16_t srcArray[3 * 2] = {
        0x1110, 0x2220, 0x3330,
        0x4440, 0x5550, 0x6660
    };
    uint16_t dstArray[2 * 3] = {0};

    uint16_t expectedArray[2 * 3] = {
        0x4440, 0x1110,
        0x5550, 0x2220,
        0x6660, 0x3330
    };

    lv_draw_sw_rotate(srcArray, dstArray,
                      3, 2,
                      3 * sizeof(uint16_t),
                      2 * sizeof(uint16_t),
                      LV_DISPLAY_ROTATION_90,
                      LV_COLOR_FORMAT_RGB565);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedArray, dstArray, sizeof(dstArray));
}

void test_rotate180_RGB565(void)
{
    uint16_t srcArray[3 * 2] = {
        0x1110, 0x2220, 0x3330,
        0x4440, 0x5550, 0x6660
    };
    uint16_t dstArray[3 * 2] = {0};
    uint16_t expectedArray[3 * 2] = {
        0x6660, 0x5550, 0x4440,
        0x3330, 0x2220, 0x1110,
    };
    lv_draw_sw_rotate(srcArray, dstArray,
                      3, 2,
                      3 * sizeof(uint16_t),
                      3 * sizeof(uint16_t),
                      LV_DISPLAY_ROTATION_180,
                      LV_COLOR_FORMAT_RGB565);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedArray, dstArray, sizeof(dstArray));
}

void test_rotate270_RGB565(void)
{
    uint16_t srcArray[3 * 2] = {
        0x1110, 0x2220, 0x3330,
        0x4440, 0x5550, 0x6660
    };

    uint16_t dstArray[2 * 3] = {0};
    uint16_t expectedArray[2 * 3] = {
        0x3330, 0x6660,
        0x2220, 0x5550,
        0x1110, 0x4440,
    };

    lv_draw_sw_rotate(srcArray, dstArray,
                      3, 2,
                      3 * sizeof(uint16_t),
                      2 * sizeof(uint16_t),
                      LV_DISPLAY_ROTATION_270,
                      LV_COLOR_FORMAT_RGB565);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedArray, dstArray, sizeof(dstArray));
}

void test_rotate90_RGB888(void)
{
    uint8_t srcArray[3 * 2 * 3] = {
        0x11, 0x1A, 0x1B,   0x22, 0x2A, 0x2B,   0x33, 0x3A, 0x3B,
        0x44, 0x4A, 0x4B,   0x55, 0x5A, 0x5B,   0x66, 0x6A, 0x6B
    };
    uint8_t dstArray[2 * 3 * 3] = {0};
    uint8_t expectedArray[2 * 3 * 3] = {
        0x33, 0x3A, 0x3B,   0x66, 0x6A, 0x6B,
        0x22, 0x2A, 0x2B,   0x55, 0x5A, 0x5B,
        0x11, 0x1A, 0x1B,   0x44, 0x4A, 0x4B,
    };

    lv_draw_sw_rotate(srcArray, dstArray,
                      3, 2,
                      3 * 3,
                      2 * 3,
                      LV_DISPLAY_ROTATION_90,
                      LV_COLOR_FORMAT_RGB888);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedArray, dstArray, sizeof(dstArray));
}

void test_rotate180_RGB888(void)
{
    uint8_t srcArray[3 * 2 * 3] = {
        0x11, 0x1A, 0x1B,    0x22, 0x2A, 0x2B,    0x33, 0x3A, 0x3B,
        0x44, 0x4A, 0x4B,    0x55, 0x5A, 0x5B,    0x66, 0x6A, 0x6B
    };
    uint8_t dstArray[3 * 2 * 3] = {0};
    uint8_t expectedArray[3 * 2 * 3] = {
        0x66, 0x6A, 0x6B,    0x55, 0x5A, 0x5B,    0x44, 0x4A, 0x4B,
        0x33, 0x3A, 0x3B,    0x22, 0x2A, 0x2B,    0x11, 0x1A, 0x1B
    };

    lv_draw_sw_rotate(srcArray, dstArray,
                      2, 3,
                      2 * 3,
                      2 * 3,
                      LV_DISPLAY_ROTATION_180,
                      LV_COLOR_FORMAT_RGB888);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedArray, dstArray, sizeof(dstArray));
}

void test_rotate270_RGB888(void)
{
    uint8_t srcArray[3 * 2 * 3] = {
        0x11, 0x1A, 0x1B,    0x22, 0x2A, 0x2B,    0x33, 0x3A, 0x3B,
        0x44, 0x4A, 0x4B,    0x55, 0x5A, 0x5B,    0x66, 0x6A, 0x6B
    };
    uint8_t dstArray[2 * 3 * 3] = {0};
    uint8_t expectedArray[2 * 3 * 3] = {
        0x44, 0x4A, 0x4B,     0x11, 0x1A, 0x1B,
        0x55, 0x5A, 0x5B,     0x22, 0x2A, 0x2B,
        0x66, 0x6A, 0x6B,     0x33, 0x3A, 0x3B,
    };

    lv_draw_sw_rotate(srcArray, dstArray,
                      3, 2,
                      3 * 3,
                      2 * 3,
                      LV_DISPLAY_ROTATION_270,
                      LV_COLOR_FORMAT_RGB888);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedArray, dstArray, sizeof(dstArray));
}

void test_rotate90_ARGB8888(void)
{
    uint32_t srcArray[3 * 2] = {
        0x111A1B1C, 0x222A2B2C, 0x333A3B3C,
        0x444A4B4C, 0x555A5B5C, 0x666A6B6C

    };
    uint32_t dstArray[2 * 3] = {0};
    uint32_t expectedArray[2 * 3] = {
        0x444A4B4C, 0x111A1B1C,
        0x555A5B5C, 0x222A2B2C,
        0x666A6B6C, 0x333A3B3C
    };

    lv_draw_sw_rotate(srcArray, dstArray,
                      3, 2,
                      3 * sizeof(uint32_t),
                      2 * sizeof(uint32_t),
                      LV_DISPLAY_ROTATION_90,
                      LV_COLOR_FORMAT_ARGB8888);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedArray, dstArray, sizeof(dstArray));
}

void test_rotate180_ARGB8888(void)
{
    uint32_t srcArray[3 * 2] = {
        0xFF0000FF, 0xFF00FF00, // Row 1: Red, Green
        0xFFFF0000, 0xFFFFFFFF, // Row 2: Blue, White
        0xFF00FFFF, 0xFFFFFF00  // Row 3: Cyan, Yellow
    };
    uint32_t dstArray[3 * 2] = {0};
    uint32_t expectedArray[3 * 2] = {
        0xFFFFFF00, 0xFF00FFFF, // Rotated Row 1
        0xFFFFFFFF, 0xFFFF0000, // Rotated Row 2
        0xFF00FF00, 0xFF0000FF  // Rotated Row 3
    };

    lv_draw_sw_rotate(srcArray, dstArray,
                      2, 3,
                      2 * sizeof(uint32_t),
                      2 * sizeof(uint32_t),
                      LV_DISPLAY_ROTATION_180,
                      LV_COLOR_FORMAT_ARGB8888);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedArray, dstArray, sizeof(dstArray));
}

void test_rotate270_ARGB8888(void)
{
    uint32_t srcArray[3 * 2] = {
        0x111A1B1C, 0x222A2B2C, 0x333A3B3C,
        0x444A4B4C, 0x555A5B5C, 0x666A6B6C
    };
    uint32_t dstArray[2 * 3] = {0};
    uint32_t expectedArray[2 * 3] = {
        0x333A3B3C, 0x666A6B6C,
        0x222A2B2C, 0x555A5B5C,
        0x111A1B1C, 0x444A4B4C,
    };

    lv_draw_sw_rotate(srcArray, dstArray,
                      3, 2,
                      3 * sizeof(uint32_t),
                      2 * sizeof(uint32_t),
                      LV_DISPLAY_ROTATION_270,
                      LV_COLOR_FORMAT_ARGB8888);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedArray, dstArray, sizeof(dstArray));
}

#endif
