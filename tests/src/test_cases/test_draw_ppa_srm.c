#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

/* The PPA SRM operation itself only runs on an ESP32-P4, but the source-block
 * geometry (lv_draw_ppa_srm_calc_block) is pure math and can be verified here
 * on the host. These tests cover the parts that are easy to get wrong:
 * scale mapping, clamping to the image, and the 1-pixel right/bottom gap that
 * the PPA's floor-rounding leaves behind. */

void setUp(void)
{
}

void tearDown(void)
{
}

/* 1:1, image fully inside the render tile: source block == whole image. */
void test_ppa_srm_identity(void)
{
    lv_area_t coords = {.x1 = 0, .y1 = 0, .x2 = 99, .y2 = 99};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};

    lv_draw_ppa_srm_block_t b = lv_draw_ppa_srm_calc_block(
                                    &coords, &buf, 200, 200, 100, 100,
                                    LV_SCALE_NONE, LV_SCALE_NONE, 0, 0);

    TEST_ASSERT_TRUE(b.draw);
    TEST_ASSERT_EQUAL_INT32(0, b.block_x);
    TEST_ASSERT_EQUAL_INT32(0, b.block_y);
    TEST_ASSERT_EQUAL_INT32(100, b.block_w);
    TEST_ASSERT_EQUAL_INT32(100, b.block_h);
    TEST_ASSERT_EQUAL_INT32(100, b.clip_w);
    TEST_ASSERT_EQUAL_INT32(100, b.clip_h);
    TEST_ASSERT_EQUAL_INT32(0, b.dest_area.x1);
    TEST_ASSERT_EQUAL_INT32(0, b.dest_area.y1);
    TEST_ASSERT_FALSE(b.gap_right);
    TEST_ASSERT_FALSE(b.gap_bottom);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, b.scale_x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, b.scale_y);
}

/* 2x upscale: a 100x100 image drawn over a 200x200 on-screen area reads the
 * full 100x100 source block. */
void test_ppa_srm_upscale_2x(void)
{
    lv_area_t coords = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 399, .y2 = 399};

    lv_draw_ppa_srm_block_t b = lv_draw_ppa_srm_calc_block(
                                    &coords, &buf, 400, 400, 100, 100,
                                    512, 512, 0, 0);

    TEST_ASSERT_TRUE(b.draw);
    TEST_ASSERT_EQUAL_INT32(0, b.block_x);
    TEST_ASSERT_EQUAL_INT32(0, b.block_y);
    TEST_ASSERT_EQUAL_INT32(100, b.block_w);
    TEST_ASSERT_EQUAL_INT32(100, b.block_h);
    TEST_ASSERT_EQUAL_INT32(200, b.clip_w);
    TEST_ASSERT_EQUAL_INT32(200, b.clip_h);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.0f, b.scale_x);
}

/* 0.5x downscale: a 100x100 image drawn over a 50x50 on-screen area still
 * reads the full 100x100 source block. */
void test_ppa_srm_downscale_half(void)
{
    lv_area_t coords = {.x1 = 0, .y1 = 0, .x2 = 49, .y2 = 49};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};

    lv_draw_ppa_srm_block_t b = lv_draw_ppa_srm_calc_block(
                                    &coords, &buf, 200, 200, 100, 100,
                                    128, 128, 0, 0);

    TEST_ASSERT_TRUE(b.draw);
    TEST_ASSERT_EQUAL_INT32(100, b.block_w);
    TEST_ASSERT_EQUAL_INT32(100, b.block_h);
    TEST_ASSERT_EQUAL_INT32(50, b.clip_w);
    TEST_ASSERT_EQUAL_INT32(50, b.clip_h);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, b.scale_x);
}

/* Nothing intersects the render tile: skip the operation entirely. */
void test_ppa_srm_offscreen(void)
{
    lv_area_t coords = {.x1 = 500, .y1 = 500, .x2 = 599, .y2 = 599};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};

    lv_draw_ppa_srm_block_t b = lv_draw_ppa_srm_calc_block(
                                    &coords, &buf, 200, 200, 100, 100,
                                    LV_SCALE_NONE, LV_SCALE_NONE, 0, 0);

    TEST_ASSERT_FALSE(b.draw);
}

/* Fractional scale (1.5x horizontally) makes ceil(clip/scale) overshoot
 * floor(avail/scale) at the buffer's right edge: the gap flag must fire and
 * the block width must be clamped. */
void test_ppa_srm_right_gap(void)
{
    lv_area_t coords = {.x1 = 0, .y1 = 0, .x2 = 99, .y2 = 99};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 99, .y2 = 99};

    lv_draw_ppa_srm_block_t b = lv_draw_ppa_srm_calc_block(
                                    &coords, &buf, 100, 100, 100, 100,
                                    384, LV_SCALE_NONE, 0, 0);

    TEST_ASSERT_TRUE(b.draw);
    TEST_ASSERT_TRUE(b.gap_right);
    TEST_ASSERT_FALSE(b.gap_bottom);
    TEST_ASSERT_EQUAL_INT32(66, b.block_w);   /* clamped from ceil(100/1.5)=67 */
    TEST_ASSERT_EQUAL_INT32(100, b.block_h);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.5f, b.scale_x);
}

/* A pivot + downscale that maps the visible tile before the source origin:
 * the negative-offset guard must reject the block. */
void test_ppa_srm_negative_block_rejected(void)
{
    lv_area_t coords = {.x1 = 0, .y1 = 0, .x2 = 49, .y2 = 49};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};

    lv_draw_ppa_srm_block_t b = lv_draw_ppa_srm_calc_block(
                                    &coords, &buf, 200, 200, 100, 100,
                                    128, 128, 100, 100);

    TEST_ASSERT_FALSE(b.draw);
}

#endif
