#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#if LV_DRAW_SW_SUPPORT_I4

#include "src/draw/sw/blend/lv_draw_sw_blend_to_i4.h"

/*-----------------------------------------------------------------------
 * Test palette: 16 entries chosen so each entry is uniquely identifiable
 * in pixel-exact comparisons. Index N has R=N*16, G=N*16, B=N*16
 * — i.e. a strictly monotonic grayscale ramp identical to the default
 * but explicit so the tests don't depend on the blender's fallback.
 *---------------------------------------------------------------------*/
static const lv_color32_t s_test_palette[16] = {
    {.blue = 0x00, .green = 0x00, .red = 0x00, .alpha = 0xFF},
    {.blue = 0x10, .green = 0x10, .red = 0x10, .alpha = 0xFF},
    {.blue = 0x20, .green = 0x20, .red = 0x20, .alpha = 0xFF},
    {.blue = 0x30, .green = 0x30, .red = 0x30, .alpha = 0xFF},
    {.blue = 0x40, .green = 0x40, .red = 0x40, .alpha = 0xFF},
    {.blue = 0x50, .green = 0x50, .red = 0x50, .alpha = 0xFF},
    {.blue = 0x60, .green = 0x60, .red = 0x60, .alpha = 0xFF},
    {.blue = 0x70, .green = 0x70, .red = 0x70, .alpha = 0xFF},
    {.blue = 0x80, .green = 0x80, .red = 0x80, .alpha = 0xFF},
    {.blue = 0x90, .green = 0x90, .red = 0x90, .alpha = 0xFF},
    {.blue = 0xA0, .green = 0xA0, .red = 0xA0, .alpha = 0xFF},
    {.blue = 0xB0, .green = 0xB0, .red = 0xB0, .alpha = 0xFF},
    {.blue = 0xC0, .green = 0xC0, .red = 0xC0, .alpha = 0xFF},
    {.blue = 0xD0, .green = 0xD0, .red = 0xD0, .alpha = 0xFF},
    {.blue = 0xE0, .green = 0xE0, .red = 0xE0, .alpha = 0xFF},
    {.blue = 0xFF, .green = 0xFF, .red = 0xFF, .alpha = 0xFF},
};

/*-----------------------------------------------------------------------
 * For nibble-level tests we need the active display to expose the test
 * palette. setUp installs it; tearDown clears it. The blender pulls the
 * palette via lv_refr_get_disp_refreshing(); to make that path return
 * our display we wrap each test in lv_refr_set_disp_refreshing().
 *---------------------------------------------------------------------*/

void setUp(void)
{
    lv_display_t * disp = lv_display_get_default();
    lv_display_set_palette(disp, s_test_palette, 16);
    lv_refr_set_disp_refreshing(disp);
}

void tearDown(void)
{
    lv_display_t * disp = lv_display_get_default();
    lv_display_set_palette(disp, NULL, 0);
    lv_refr_set_disp_refreshing(NULL);
}

/* Pack two nibbles into one byte (high in upper half, low in lower half). */
static inline uint8_t nib_pair(uint8_t hi, uint8_t lo)
{
    return (uint8_t)(((hi & 0x0F) << 4) | (lo & 0x0F));
}

/*-----------------------------------------------------------------------
 * Solid fill of all 16 indices into a 16-pixel-wide row, with `dest_x` and
 * stride aligned to a byte boundary. Each pair of pixels packs into one
 * byte (high nibble = lower-x pixel).
 *---------------------------------------------------------------------*/
void test_draw_sw_blend_color_to_i4_solid_fill_all_indices(void)
{
    lv_color32_t pal[16];
    lv_memcpy(pal, s_test_palette, sizeof(pal));

    /* For each index, fill a 1-pixel-wide column at x=N and verify the
     * resulting nibble. Use an 8-pixel-wide / 1-row-high buffer so the
     * stride is 4 bytes. */
    for(uint8_t target = 0; target < 16; target++) {
        uint8_t buf[4] = {0};
        lv_draw_sw_blend_fill_dsc_t dsc = {0};
        dsc.dest_buf = buf;
        dsc.dest_w = 8;
        dsc.dest_h = 1;
        dsc.dest_stride = 4;
        dsc.opa = LV_OPA_COVER;
        dsc.color = (lv_color_t){
            .blue = pal[target].blue,
            .green = pal[target].green,
            .red = pal[target].red,
        };
        dsc.relative_area.x1 = 0;
        dsc.relative_area.y1 = 0;
        dsc.relative_area.x2 = 7;
        dsc.relative_area.y2 = 0;

        lv_draw_sw_blend_color_to_i4(&dsc);

        uint8_t expected = nib_pair(target, target);
        for(int i = 0; i < 4; i++) {
            TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected, buf[i],
                                           "solid fill produced wrong byte");
        }
    }
}

/*-----------------------------------------------------------------------
 * Odd-x start: x1=1 (start in the lower nibble of byte 0). The upper
 * nibble of byte 0 must be left untouched.
 *---------------------------------------------------------------------*/
void test_draw_sw_blend_color_to_i4_odd_dest_x(void)
{
    /* Pre-fill with a sentinel pattern. */
    uint8_t buf[3] = {0xAB, 0xCD, 0xEF};

    lv_draw_sw_blend_fill_dsc_t dsc = {0};
    dsc.dest_buf = buf;        /*Already at byte 0 — caller adjusted for odd x.*/
    dsc.dest_w = 5;            /*5 pixels: lower nibble of [0], both of [1], upper nibble of [2].*/
    dsc.dest_h = 1;
    dsc.dest_stride = 3;
    dsc.opa = LV_OPA_COVER;
    dsc.color = (lv_color_t){.blue = 0xFF, .green = 0xFF, .red = 0xFF};
    dsc.relative_area.x1 = 1;  /*Odd start.*/
    dsc.relative_area.y1 = 0;
    dsc.relative_area.x2 = 5;
    dsc.relative_area.y2 = 0;

    lv_draw_sw_blend_color_to_i4(&dsc);

    /* Expected:
     *   byte[0] upper nibble = 0xA (untouched), lower = 0xF (white-ish, idx 15)
     *   byte[1] = 0xFF
     *   byte[2] upper nibble = 0xF, lower = 0xF (untouched)
     */
    TEST_ASSERT_EQUAL_HEX8(0xAF, buf[0]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, buf[1]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, buf[2]);
}

/*-----------------------------------------------------------------------
 * Odd width with even start: 7 pixels written into 4 bytes; the lower
 * nibble of the last byte must be untouched.
 *---------------------------------------------------------------------*/
void test_draw_sw_blend_color_to_i4_odd_width(void)
{
    uint8_t buf[4] = {0xAA, 0xBB, 0xCC, 0xDD};

    lv_draw_sw_blend_fill_dsc_t dsc = {0};
    dsc.dest_buf = buf;
    dsc.dest_w = 7;
    dsc.dest_h = 1;
    dsc.dest_stride = 4;
    dsc.opa = LV_OPA_COVER;
    dsc.color = (lv_color_t){.blue = 0x00, .green = 0x00, .red = 0x00};
    dsc.relative_area.x1 = 0;
    dsc.relative_area.y1 = 0;
    dsc.relative_area.x2 = 6;
    dsc.relative_area.y2 = 0;

    lv_draw_sw_blend_color_to_i4(&dsc);

    /* idx 0 = black → bytes [0..2] = 0x00, byte[3] upper = 0, lower = 0xD untouched */
    TEST_ASSERT_EQUAL_HEX8(0x00, buf[0]);
    TEST_ASSERT_EQUAL_HEX8(0x00, buf[1]);
    TEST_ASSERT_EQUAL_HEX8(0x00, buf[2]);
    TEST_ASSERT_EQUAL_HEX8(0x0D, buf[3]);
}

/*-----------------------------------------------------------------------
 * Odd start AND odd width — both ends partial.
 * Range x∈[1..5], 5 pixels. Buffer pre-filled to verify untouched halves.
 *---------------------------------------------------------------------*/
void test_draw_sw_blend_color_to_i4_odd_both(void)
{
    uint8_t buf[3] = {0x12, 0x34, 0x56};

    lv_draw_sw_blend_fill_dsc_t dsc = {0};
    dsc.dest_buf = buf;
    dsc.dest_w = 5;
    dsc.dest_h = 1;
    dsc.dest_stride = 3;
    dsc.opa = LV_OPA_COVER;
    /* index 8 = mid-gray (R=G=B=0x80) */
    dsc.color = (lv_color_t){.blue = 0x80, .green = 0x80, .red = 0x80};
    dsc.relative_area.x1 = 1;
    dsc.relative_area.y1 = 0;
    dsc.relative_area.x2 = 5;
    dsc.relative_area.y2 = 0;

    lv_draw_sw_blend_color_to_i4(&dsc);

    TEST_ASSERT_EQUAL_HEX8(0x18, buf[0]); /*upper: 0x1 untouched, lower: 0x8 written*/
    TEST_ASSERT_EQUAL_HEX8(0x88, buf[1]);
    TEST_ASSERT_EQUAL_HEX8(0x86, buf[2]); /*upper: 0x8 written, lower: 0x6 untouched*/
}

/*-----------------------------------------------------------------------
 * Image blend from RGB565 source with the test palette. The grayscale
 * palette + monotonic ramp means each RGB565 luminance maps to a known
 * index. Verifies the full quantize path on the inner loop.
 *---------------------------------------------------------------------*/
void test_draw_sw_blend_image_to_i4_from_rgb565(void)
{
    /* Source: 4 RGB565 pixels with luminance close to palette entries
     * 0, 4, 8, 15. */
    static const lv_color16_t src_px[4] = {
        {.red = 0x00, .green = 0x00, .blue = 0x00},  /*black ~ idx 0*/
        {.red = 0x08, .green = 0x10, .blue = 0x08},  /*~0x40 gray ~ idx 4*/
        {.red = 0x10, .green = 0x20, .blue = 0x10},  /*~0x80 gray ~ idx 8*/
        {.red = 0x1F, .green = 0x3F, .blue = 0x1F},  /*white ~ idx 15*/
    };

    uint8_t dest[2] = {0, 0};
    lv_draw_sw_blend_image_dsc_t dsc = {0};
    dsc.dest_buf = dest;
    dsc.dest_w = 4;
    dsc.dest_h = 1;
    dsc.dest_stride = 2;
    dsc.src_buf = src_px;
    dsc.src_stride = sizeof(src_px);
    dsc.src_color_format = LV_COLOR_FORMAT_RGB565;
    dsc.opa = LV_OPA_COVER;
    dsc.blend_mode = LV_BLEND_MODE_NORMAL;
    dsc.relative_area.x1 = 0;
    dsc.relative_area.y1 = 0;
    dsc.relative_area.x2 = 3;
    dsc.relative_area.y2 = 0;
    dsc.src_area.x1 = 0;
    dsc.src_area.y1 = 0;
    dsc.src_area.x2 = 3;
    dsc.src_area.y2 = 0;

    lv_draw_sw_blend_image_to_i4(&dsc);

    /* Index 0 / 4 in byte[0], 8 / 15 in byte[1]. */
    TEST_ASSERT_EQUAL_HEX8(nib_pair(0x0, 0x4), dest[0]);
    TEST_ASSERT_EQUAL_HEX8(nib_pair(0x8, 0xF), dest[1]);
}

/*-----------------------------------------------------------------------
 * Non-normal blend modes round-trip through the palette: dst index →
 * palette RGB → blend → quantize. Verify ADDITIVE (dst+src clamped) and
 * MULTIPLY (dst*src/255) produce the expected indices.
 *---------------------------------------------------------------------*/
void test_draw_sw_blend_image_to_i4_additive(void)
{
    /* Pre-fill destination with index 4 in both nibbles of byte[0]. */
    uint8_t dest[1] = {nib_pair(0x4, 0x4)};

    /* Source: ARGB8888, R=G=B=0x40, alpha=0xFF.
     * Additive: dst(0x40) + src(0x40) = 0x80 → idx 8. */
    static const lv_color32_t src[2] = {
        {.blue = 0x40, .green = 0x40, .red = 0x40, .alpha = 0xFF},
        {.blue = 0x40, .green = 0x40, .red = 0x40, .alpha = 0xFF},
    };

    lv_draw_sw_blend_image_dsc_t dsc = {0};
    dsc.dest_buf = dest;
    dsc.dest_w = 2;
    dsc.dest_h = 1;
    dsc.dest_stride = 1;
    dsc.src_buf = src;
    dsc.src_stride = sizeof(src);
    dsc.src_color_format = LV_COLOR_FORMAT_ARGB8888;
    dsc.opa = LV_OPA_COVER;
    dsc.blend_mode = LV_BLEND_MODE_ADDITIVE;
    dsc.relative_area.x1 = 0;
    dsc.relative_area.y1 = 0;
    dsc.relative_area.x2 = 1;
    dsc.relative_area.y2 = 0;
    dsc.src_area.x1 = 0;
    dsc.src_area.y1 = 0;
    dsc.src_area.x2 = 1;
    dsc.src_area.y2 = 0;

    lv_draw_sw_blend_image_to_i4(&dsc);

    TEST_ASSERT_EQUAL_HEX8(nib_pair(0x8, 0x8), dest[0]);
}

void test_draw_sw_blend_image_to_i4_multiply(void)
{
    /* Pre-fill destination with index 8 (R=G=B=0x80) in both nibbles. */
    uint8_t dest[1] = {nib_pair(0x8, 0x8)};

    /* Source: R=G=B=0x80. Multiply: (0x80 * 0x80) >> 8 = 0x40 → idx 4. */
    static const lv_color32_t src[2] = {
        {.blue = 0x80, .green = 0x80, .red = 0x80, .alpha = 0xFF},
        {.blue = 0x80, .green = 0x80, .red = 0x80, .alpha = 0xFF},
    };

    lv_draw_sw_blend_image_dsc_t dsc = {0};
    dsc.dest_buf = dest;
    dsc.dest_w = 2;
    dsc.dest_h = 1;
    dsc.dest_stride = 1;
    dsc.src_buf = src;
    dsc.src_stride = sizeof(src);
    dsc.src_color_format = LV_COLOR_FORMAT_ARGB8888;
    dsc.opa = LV_OPA_COVER;
    dsc.blend_mode = LV_BLEND_MODE_MULTIPLY;
    dsc.relative_area.x1 = 0;
    dsc.relative_area.y1 = 0;
    dsc.relative_area.x2 = 1;
    dsc.relative_area.y2 = 0;
    dsc.src_area.x1 = 0;
    dsc.src_area.y1 = 0;
    dsc.src_area.x2 = 1;
    dsc.src_area.y2 = 0;

    lv_draw_sw_blend_image_to_i4(&dsc);

    TEST_ASSERT_EQUAL_HEX8(nib_pair(0x4, 0x4), dest[0]);
}

/*-----------------------------------------------------------------------
 * Source ARGB8888 with non-cover alpha mixes into the underlying I4 dst
 * via palette quantization. Pre-fill dst index 0 (black, RGB 0x00) and
 * blend src white (RGB 0xFF) at alpha 128 → expected midgray ~ idx 7..8.
 *---------------------------------------------------------------------*/
void test_draw_sw_blend_image_to_i4_argb_with_alpha(void)
{
    uint8_t dest[1] = {nib_pair(0x0, 0x0)};

    static const lv_color32_t src[2] = {
        {.blue = 0xFF, .green = 0xFF, .red = 0xFF, .alpha = 0x80},
        {.blue = 0xFF, .green = 0xFF, .red = 0xFF, .alpha = 0x80},
    };

    lv_draw_sw_blend_image_dsc_t dsc = {0};
    dsc.dest_buf = dest;
    dsc.dest_w = 2;
    dsc.dest_h = 1;
    dsc.dest_stride = 1;
    dsc.src_buf = src;
    dsc.src_stride = sizeof(src);
    dsc.src_color_format = LV_COLOR_FORMAT_ARGB8888;
    dsc.opa = LV_OPA_COVER;
    dsc.blend_mode = LV_BLEND_MODE_NORMAL;
    dsc.relative_area.x1 = 0;
    dsc.relative_area.y1 = 0;
    dsc.relative_area.x2 = 1;
    dsc.relative_area.y2 = 0;
    dsc.src_area.x1 = 0;
    dsc.src_area.y1 = 0;
    dsc.src_area.x2 = 1;
    dsc.src_area.y2 = 0;

    lv_draw_sw_blend_image_to_i4(&dsc);

    /* (0 * (255-128) + 255 * 128) / 256 = 0x7F → quantized to idx 7. */
    uint8_t lo = dest[0] & 0x0F;
    uint8_t hi = dest[0] >> 4;
    TEST_ASSERT_TRUE_MESSAGE(hi == 0x7 || hi == 0x8, "high nibble must be ~midgray");
    TEST_ASSERT_TRUE_MESSAGE(lo == 0x7 || lo == 0x8, "low nibble must be ~midgray");
    TEST_ASSERT_EQUAL_MESSAGE(hi, lo, "both nibbles must agree");
}

#endif /*LV_DRAW_SW_SUPPORT_I4*/

#endif
