#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

/* The PPA SRM operation itself only runs on an ESP32-P4, but the source-block
 * geometry (lv_draw_ppa_srm_calc_block) is pure math and can be verified here
 * on the host. These tests cover the parts that are easy to get wrong:
 * scale mapping, clamping to the image, and the 1-pixel right/bottom gap that
 * the PPA's floor-rounding leaves behind.
 *
 * calc_block takes the transformed on-screen bounding box (t->_real_area): its
 * top-left is the virtual source origin and its size is img * scale. */

void setUp(void)
{
}

void tearDown(void)
{
}

/* 1:1, image fully inside the render tile: source block == whole image. */
void test_ppa_srm_identity(void)
{
    lv_area_t real_area = {.x1 = 0, .y1 = 0, .x2 = 99, .y2 = 99};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};

    lv_draw_ppa_srm_block_t b = lv_draw_ppa_srm_calc_block(
                                    &real_area, &buf, 200, 200, 100, 100,
                                    LV_SCALE_NONE, LV_SCALE_NONE);

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

/* 2x upscale: a 100x100 image whose transformed bbox is 200x200 reads the full
 * 100x100 source block. */
void test_ppa_srm_upscale_2x(void)
{
    lv_area_t real_area = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 399, .y2 = 399};

    lv_draw_ppa_srm_block_t b = lv_draw_ppa_srm_calc_block(
                                    &real_area, &buf, 400, 400, 100, 100,
                                    512, 512);

    TEST_ASSERT_TRUE(b.draw);
    TEST_ASSERT_EQUAL_INT32(0, b.block_x);
    TEST_ASSERT_EQUAL_INT32(0, b.block_y);
    TEST_ASSERT_EQUAL_INT32(100, b.block_w);
    TEST_ASSERT_EQUAL_INT32(100, b.block_h);
    TEST_ASSERT_EQUAL_INT32(200, b.clip_w);
    TEST_ASSERT_EQUAL_INT32(200, b.clip_h);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.0f, b.scale_x);
}

/* 0.5x downscale, image anchored at the origin: transformed bbox is 50x50 and
 * still reads the full 100x100 source block. */
void test_ppa_srm_downscale_half(void)
{
    lv_area_t real_area = {.x1 = 0, .y1 = 0, .x2 = 49, .y2 = 49};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};

    lv_draw_ppa_srm_block_t b = lv_draw_ppa_srm_calc_block(
                                    &real_area, &buf, 200, 200, 100, 100,
                                    128, 128);

    TEST_ASSERT_TRUE(b.draw);
    TEST_ASSERT_EQUAL_INT32(100, b.block_w);
    TEST_ASSERT_EQUAL_INT32(100, b.block_h);
    TEST_ASSERT_EQUAL_INT32(50, b.clip_w);
    TEST_ASSERT_EQUAL_INT32(50, b.clip_h);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, b.scale_x);
}

/* 0.5x downscale of a CENTERED image (pivot at the image center): the scaled
 * 50x50 box sits offset from the origin. Because calc_block clips against the
 * transformed box, the source block starts at (0,0) and the op is emitted.
 * The previous coords-based math mapped this to a negative source offset and
 * dropped the draw -- this is the regression this whole change fixes. */
void test_ppa_srm_downscale_centered(void)
{
    /* 100x100 image at 0.5x -> 50x50 transformed box, centered at (25,25). */
    lv_area_t real_area = {.x1 = 25, .y1 = 25, .x2 = 74, .y2 = 74};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};

    lv_draw_ppa_srm_block_t b = lv_draw_ppa_srm_calc_block(
                                    &real_area, &buf, 200, 200, 100, 100,
                                    128, 128);

    TEST_ASSERT_TRUE(b.draw);
    TEST_ASSERT_EQUAL_INT32(0, b.block_x);
    TEST_ASSERT_EQUAL_INT32(0, b.block_y);
    TEST_ASSERT_EQUAL_INT32(100, b.block_w);
    TEST_ASSERT_EQUAL_INT32(100, b.block_h);
    TEST_ASSERT_EQUAL_INT32(50, b.clip_w);
    TEST_ASSERT_EQUAL_INT32(50, b.clip_h);
    /* placed at the box origin, not the un-scaled image origin */
    TEST_ASSERT_EQUAL_INT32(25, b.dest_area.x1);
    TEST_ASSERT_EQUAL_INT32(25, b.dest_area.y1);
}

/* Nothing intersects the render tile: skip the operation entirely. */
void test_ppa_srm_offscreen(void)
{
    lv_area_t real_area = {.x1 = 500, .y1 = 500, .x2 = 599, .y2 = 599};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};

    lv_draw_ppa_srm_block_t b = lv_draw_ppa_srm_calc_block(
                                    &real_area, &buf, 200, 200, 100, 100,
                                    LV_SCALE_NONE, LV_SCALE_NONE);

    TEST_ASSERT_FALSE(b.draw);
}

/* Fractional scale (1.5x horizontally) makes ceil(clip/scale) overshoot
 * floor(avail/scale) at the buffer's right edge: the gap flag must fire and
 * the block width must be clamped. */
void test_ppa_srm_right_gap(void)
{
    lv_area_t real_area = {.x1 = 0, .y1 = 0, .x2 = 99, .y2 = 99};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 99, .y2 = 99};

    lv_draw_ppa_srm_block_t b = lv_draw_ppa_srm_calc_block(
                                    &real_area, &buf, 100, 100, 100, 100,
                                    384, LV_SCALE_NONE);

    TEST_ASSERT_TRUE(b.draw);
    TEST_ASSERT_TRUE(b.gap_right);
    TEST_ASSERT_FALSE(b.gap_bottom);
    TEST_ASSERT_EQUAL_INT32(66, b.block_w);   /* clamped from ceil(100/1.5)=67 */
    TEST_ASSERT_EQUAL_INT32(100, b.block_h);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.5f, b.scale_x);
}

#endif
