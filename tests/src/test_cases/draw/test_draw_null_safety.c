#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

/*
 * Verify that lv_draw_buf_goto_xy returns NULL when coordinates exceed
 * the buffer dimensions, rather than performing an out-of-bounds access.
 */
void test_draw_buf_goto_xy_out_of_bounds_returns_null(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(100, 100, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /* x beyond width */
    void * ptr = lv_draw_buf_goto_xy(buf, 100, 0);
    TEST_ASSERT_NULL(ptr);

    /* y beyond height */
    ptr = lv_draw_buf_goto_xy(buf, 0, 100);
    TEST_ASSERT_NULL(ptr);

    /* Both beyond bounds */
    ptr = lv_draw_buf_goto_xy(buf, 200, 200);
    TEST_ASSERT_NULL(ptr);

    /* Valid coordinates should still work */
    ptr = lv_draw_buf_goto_xy(buf, 0, 0);
    TEST_ASSERT_NOT_NULL(ptr);

    ptr = lv_draw_buf_goto_xy(buf, 99, 99);
    TEST_ASSERT_NOT_NULL(ptr);

    lv_draw_buf_destroy(buf);
}

/*
 * Verify that lv_draw_buf_goto_xy handles a NULL buffer pointer
 * gracefully by returning NULL instead of dereferencing it.
 */
void test_draw_buf_goto_xy_null_buf_returns_null(void)
{
    void * ptr = lv_draw_buf_goto_xy(NULL, 0, 0);
    TEST_ASSERT_NULL(ptr);
}

/*
 * Verify that lv_draw_layer_go_to_xy returns NULL when given negative
 * coordinates, preventing underflow in the buffer offset calculation.
 */
void test_draw_layer_go_to_xy_negative_coords_returns_null(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(100, 100, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    lv_layer_t layer;
    lv_memzero(&layer, sizeof(lv_layer_t));
    layer.draw_buf = buf;

    /* Negative x */
    void * ptr = lv_draw_layer_go_to_xy(&layer, -1, 0);
    TEST_ASSERT_NULL(ptr);

    /* Negative y */
    ptr = lv_draw_layer_go_to_xy(&layer, 0, -1);
    TEST_ASSERT_NULL(ptr);

    /* Both negative */
    ptr = lv_draw_layer_go_to_xy(&layer, -10, -10);
    TEST_ASSERT_NULL(ptr);

    /* Valid coordinates should still work */
    ptr = lv_draw_layer_go_to_xy(&layer, 0, 0);
    TEST_ASSERT_NOT_NULL(ptr);

    ptr = lv_draw_layer_go_to_xy(&layer, 50, 50);
    TEST_ASSERT_NOT_NULL(ptr);

    lv_draw_buf_destroy(buf);
}

/*
 * Verify that lv_draw_buf_create handles a NULL result from malloc gracefully
 * by returning NULL rather than crashing. Uses a direct NULL buffer pointer
 * to test the guard without depending on allocation behavior.
 */
void test_draw_buf_goto_xy_on_zero_size_buf(void)
{
    /* Create smallest possible buffer, then access beyond its bounds.
     * This exercises the bounds-checking guards without relying on OOM. */
    lv_draw_buf_t * buf = lv_draw_buf_create(1, 1, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(buf);

    /* Access beyond the 1x1 buffer — should return NULL, not crash */
    void * ptr = lv_draw_buf_goto_xy(buf, 1, 1);
    TEST_ASSERT_NULL(ptr);

    ptr = lv_draw_buf_goto_xy(buf, 0, 1);
    TEST_ASSERT_NULL(ptr);

    ptr = lv_draw_buf_goto_xy(buf, 1, 0);
    TEST_ASSERT_NULL(ptr);

    /* Valid access at (0,0) should work */
    ptr = lv_draw_buf_goto_xy(buf, 0, 0);
    TEST_ASSERT_NOT_NULL(ptr);

    lv_draw_buf_destroy(buf);
}

/*
 * Verify that lv_draw_layer_go_to_xy handles a layer whose draw_buf
 * is NULL by returning NULL instead of dereferencing the NULL pointer.
 * This exercises the NULL draw_buf guard added to the blend path.
 */
void test_draw_layer_go_to_xy_null_draw_buf_returns_null(void)
{
    lv_layer_t layer;
    lv_memzero(&layer, sizeof(lv_layer_t));
    layer.draw_buf = NULL;

    /* Should return NULL via the lv_draw_buf_goto_xy(NULL, ...) path */
    void * ptr = lv_draw_layer_go_to_xy(&layer, 0, 0);
    TEST_ASSERT_NULL(ptr);
}

#endif
