#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

/* The PPA rotation only runs on an ESP32-P4, but the source-block geometry
 * (lv_draw_ppa_rot_calc_block) is pure math and can be verified here on the
 * host. These tests cover the parts that are easy to get wrong: clipping the
 * rotated image to the render tile, the axis swap for the 90/270 steps, the
 * 180 offset inversion, the off-screen guard, and clamping the destination to
 * the buffer so the PPA can never write out of bounds. */

void setUp(void)
{
}

void tearDown(void)
{
}

/* 180, image fully inside the tile: source block == whole image, no flip of size. */
void test_ppa_rot_180_full(void)
{
    lv_area_t coords = {.x1 = 0, .y1 = 0, .x2 = 99, .y2 = 99};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};

    lv_draw_ppa_rot_block_t b = lv_draw_ppa_rot_calc_block(
                                    &coords, &buf, 200, 200, 100, 100, 1800);

    TEST_ASSERT_TRUE(b.draw);
    TEST_ASSERT_EQUAL_INT32(0, b.dest_area.x1);
    TEST_ASSERT_EQUAL_INT32(0, b.dest_area.y1);
    TEST_ASSERT_EQUAL_INT32(0, b.block_x);
    TEST_ASSERT_EQUAL_INT32(0, b.block_y);
    TEST_ASSERT_EQUAL_INT32(100, b.block_w);
    TEST_ASSERT_EQUAL_INT32(100, b.block_h);
}

/* 90 of a 100x60 image: on-screen bbox is 60x100 (axes swapped). Full tile maps
 * to the full source block, with source dimensions un-swapped (100x60). */
void test_ppa_rot_90_full(void)
{
    lv_area_t coords = {.x1 = 0, .y1 = 0, .x2 = 59, .y2 = 99};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};

    lv_draw_ppa_rot_block_t b = lv_draw_ppa_rot_calc_block(
                                    &coords, &buf, 200, 200, 100, 60, 900);

    TEST_ASSERT_TRUE(b.draw);
    TEST_ASSERT_EQUAL_INT32(0, b.block_x);
    TEST_ASSERT_EQUAL_INT32(0, b.block_y);
    TEST_ASSERT_EQUAL_INT32(100, b.block_w);
    TEST_ASSERT_EQUAL_INT32(60, b.block_h);
}

/* 270 of the same 100x60 image, full tile: also maps to the full source block. */
void test_ppa_rot_270_full(void)
{
    lv_area_t coords = {.x1 = 0, .y1 = 0, .x2 = 59, .y2 = 99};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};

    lv_draw_ppa_rot_block_t b = lv_draw_ppa_rot_calc_block(
                                    &coords, &buf, 200, 200, 100, 60, 2700);

    TEST_ASSERT_TRUE(b.draw);
    TEST_ASSERT_EQUAL_INT32(0, b.block_x);
    TEST_ASSERT_EQUAL_INT32(0, b.block_y);
    TEST_ASSERT_EQUAL_INT32(100, b.block_w);
    TEST_ASSERT_EQUAL_INT32(60, b.block_h);
}

/* 90, partial tile (right half of the on-screen image). The visible output
 * column range maps back to a clipped source row range; the source axes stay
 * swapped. This is exactly what the old full-image path got wrong. */
void test_ppa_rot_90_partial_tile(void)
{
    lv_area_t coords = {.x1 = 0, .y1 = 0, .x2 = 59, .y2 = 99};
    lv_area_t buf = {.x1 = 30, .y1 = 0, .x2 = 59, .y2 = 99}; /* right half, 30x100 tile */

    lv_draw_ppa_rot_block_t b = lv_draw_ppa_rot_calc_block(
                                    &coords, &buf, 30, 100, 100, 60, 900);

    TEST_ASSERT_TRUE(b.draw);
    /* destination is buffer-local, so it starts at the tile origin */
    TEST_ASSERT_EQUAL_INT32(0, b.dest_area.x1);
    TEST_ASSERT_EQUAL_INT32(0, b.dest_area.y1);
    /* output x[30,60) -> source y[0,30); full image height in x */
    TEST_ASSERT_EQUAL_INT32(0, b.block_x);
    TEST_ASSERT_EQUAL_INT32(0, b.block_y);
    TEST_ASSERT_EQUAL_INT32(100, b.block_w);
    TEST_ASSERT_EQUAL_INT32(30, b.block_h);
}

/* 180, bottom-right tile maps to the top-left of the source (both axes flip). */
void test_ppa_rot_180_partial_tile(void)
{
    lv_area_t coords = {.x1 = 0, .y1 = 0, .x2 = 99, .y2 = 99};
    lv_area_t buf = {.x1 = 50, .y1 = 50, .x2 = 99, .y2 = 99}; /* bottom-right, 50x50 tile */

    lv_draw_ppa_rot_block_t b = lv_draw_ppa_rot_calc_block(
                                    &coords, &buf, 50, 50, 100, 100, 1800);

    TEST_ASSERT_TRUE(b.draw);
    TEST_ASSERT_EQUAL_INT32(0, b.dest_area.x1);
    TEST_ASSERT_EQUAL_INT32(0, b.dest_area.y1);
    TEST_ASSERT_EQUAL_INT32(0, b.block_x);
    TEST_ASSERT_EQUAL_INT32(0, b.block_y);
    TEST_ASSERT_EQUAL_INT32(50, b.block_w);
    TEST_ASSERT_EQUAL_INT32(50, b.block_h);
}

/* Nothing intersects the render tile: skip the operation entirely (the old path
 * would have decoded and submitted a PPA op writing at a bogus offset). */
void test_ppa_rot_offscreen(void)
{
    lv_area_t coords = {.x1 = 500, .y1 = 500, .x2 = 559, .y2 = 599};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 199, .y2 = 199};

    lv_draw_ppa_rot_block_t b = lv_draw_ppa_rot_calc_block(
                                    &coords, &buf, 200, 200, 100, 60, 900);

    TEST_ASSERT_FALSE(b.draw);
}

/* Destination clamp: a buffer narrower/shorter than the visible window must be
 * clamped so the PPA cannot write past the buffer. */
void test_ppa_rot_dest_clamped_to_buffer(void)
{
    lv_area_t coords = {.x1 = 0, .y1 = 0, .x2 = 99, .y2 = 99};
    lv_area_t buf = {.x1 = 0, .y1 = 0, .x2 = 99, .y2 = 99};

    /* buffer is only 80x80 even though the visible window is 100x100 */
    lv_draw_ppa_rot_block_t b = lv_draw_ppa_rot_calc_block(
                                    &coords, &buf, 80, 80, 100, 100, 1800);

    TEST_ASSERT_TRUE(b.draw);
    /* source block (and therefore the PPA output) is clamped to 80x80 */
    TEST_ASSERT_EQUAL_INT32(80, b.block_w);
    TEST_ASSERT_EQUAL_INT32(80, b.block_h);
    TEST_ASSERT_TRUE(b.dest_area.x1 + b.block_w <= 80);
    TEST_ASSERT_TRUE(b.dest_area.y1 + b.block_h <= 80);
}

#endif
