/**
 * Collecting and joining the invalidated areas
 */
#if LV_BUILD_TEST

#include "refr/lv_test_refr.h"

void setUp(void)
{
    refr_ctx_reset();
}

void tearDown(void)
{
    refr_disp_delete();
}

void test_refr_inv_area_without_display(void)
{
    lv_display_t * disp_def = lv_display_get_default();
    lv_area_t area = {0, 0, 9, 9};

    lv_display_set_default(NULL);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(NULL, &area));
    lv_display_set_default(disp_def);
}

void test_refr_inv_area_uses_default_display(void)
{
    lv_display_t * disp_def = lv_display_get_default();
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);

    lv_display_set_default(refr_ctx.disp);
    refr_ctx.disp->inv_p = 0;

    lv_area_t area = {10, 20, 29, 39};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(NULL, &area));
    lv_display_set_default(disp_def);

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&area, &refr_ctx.disp->inv_areas[0]));
}

void test_refr_inv_area_disabled(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_ctx.disp->inv_p = 0;

    lv_display_enable_invalidation(refr_ctx.disp, false);
    TEST_ASSERT_FALSE(lv_display_is_invalidation_enabled(refr_ctx.disp));

    lv_area_t area = {0, 0, 9, 9};
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(refr_ctx.disp, &area));
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.disp->inv_p);

    /*Nothing is rendered while invalidation is disabled*/
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);

    lv_display_enable_invalidation(refr_ctx.disp, true);
    TEST_ASSERT_TRUE(lv_display_is_invalidation_enabled(refr_ctx.disp));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &area));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
}

void test_refr_inv_area_null_area_clears_the_buffer(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);

    lv_area_t a1 = {0, 0, 9, 9};
    lv_area_t a2 = {50, 50, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.disp->inv_p);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, NULL));
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.disp->inv_p);

    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
}

void test_refr_inv_area_clipped_to_screen(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_ctx.disp->inv_p = 0;

    /*The areas that are completely out of the screen are dropped*/
    lv_area_t right = {100, 10, 120, 20};
    lv_area_t below = {10, 100, 20, 120};
    lv_area_t left = {-50, 10, -1, 20};
    lv_area_t above = {10, -50, 20, -1};

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(refr_ctx.disp, &right));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(refr_ctx.disp, &below));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(refr_ctx.disp, &left));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(refr_ctx.disp, &above));
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.disp->inv_p);

    /*The area that hangs over the edges is stored clipped*/
    lv_area_t area = { -20, -30, 120, 130};
    lv_area_t expected = {0, 0, 99, 99};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &area));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected, &refr_ctx.disp->inv_areas[0]));

    /*The caller's area is not modified*/
    lv_area_t untouched = { -20, -30, 120, 130};
    TEST_ASSERT_TRUE(lv_area_is_equal(&untouched, &area));
}

void test_refr_inv_area_i1_rounded_to_byte_boundary(void)
{
    refr_disp_create(64, 32, LV_COLOR_FORMAT_I1, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 32);
    refr_ctx.disp->inv_p = 0;

    lv_area_t area = {11, 5, 27, 9};
    lv_area_t expected = {8, 5, 31, 9};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &area));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected, &refr_ctx.disp->inv_areas[0]));

    /*Already aligned areas are kept*/
    refr_ctx.disp->inv_p = 0;
    lv_area_t aligned = {8, 0, 15, 1};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &aligned));
    TEST_ASSERT_TRUE(lv_area_is_equal(&aligned, &refr_ctx.disp->inv_areas[0]));
}

void test_refr_inv_area_full_mode_takes_the_whole_screen(void)
{
    refr_disp_create(100, 80, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_FULL, 1, 80);
    refr_ctx.disp->inv_p = 0;

    lv_area_t small = {10, 10, 19, 19};
    lv_area_t full = {0, 0, 99, 79};

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &small));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&full, &refr_ctx.disp->inv_areas[0]));

    /*More invalidations do not add more areas*/
    lv_area_t other = {50, 50, 59, 59};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &other));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&full, &refr_ctx.disp->inv_areas[0]));
}

void test_refr_inv_area_contained_area_is_not_stored(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_ctx.disp->inv_p = 0;

    lv_area_t big = {10, 10, 59, 59};
    lv_area_t inside = {20, 20, 29, 29};
    lv_area_t same = {10, 10, 59, 59};
    lv_area_t overlapping = {50, 50, 79, 79};

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &big));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &inside));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &same));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);

    /*Only partially covered areas are stored*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &overlapping));
    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.disp->inv_p);
}

void test_refr_inv_area_buffer_overflow_falls_back_to_full_screen(void)
{
    refr_disp_create(400, 400, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 400);
    refr_ctx.disp->inv_p = 0;

    /*Fill the invalid area buffer with distinct, non-overlapping areas*/
    uint32_t i;
    for(i = 0; i < LV_INV_BUF_SIZE; i++) {
        lv_area_t a;
        a.x1 = (int32_t)(i % 20) * 20;
        a.y1 = (int32_t)(i / 20) * 20;
        a.x2 = a.x1 + 9;
        a.y2 = a.y1 + 9;
        TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &a));
    }
    TEST_ASSERT_EQUAL_UINT32(LV_INV_BUF_SIZE, refr_ctx.disp->inv_p);

    /*One more area does not fit, so the whole screen is invalidated instead*/
    lv_area_t extra = {300, 300, 309, 309};
    lv_area_t full = {0, 0, 399, 399};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &extra));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&full, &refr_ctx.disp->inv_areas[0]));
}

void test_refr_inv_area_event_can_modify_the_area(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_set_row_rounding(4);
    lv_display_add_event_cb(refr_ctx.disp, refr_row_rounding_cb, LV_EVENT_INVALIDATE_AREA, NULL);
    refr_ctx.disp->inv_p = 0;

    lv_area_t area = {10, 8, 19, 12};      /*5 rows*/
    lv_area_t expected = {10, 8, 19, 15};  /*rounded up to 8 rows*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_inv_area(refr_ctx.disp, &area));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.disp->inv_p);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected, &refr_ctx.disp->inv_areas[0]));
}

static void delete_disp_event_cb(lv_event_t * e)
{
    lv_display_delete(lv_event_get_current_target(e));
    refr_ctx.disp = NULL;
}

void test_refr_inv_area_display_deleted_in_event(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    lv_display_add_event_cb(refr_ctx.disp, delete_disp_event_cb, LV_EVENT_INVALIDATE_AREA, NULL);

    lv_area_t area = {10, 10, 19, 19};
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_inv_area(refr_ctx.disp, &area));
    TEST_ASSERT_NULL(refr_ctx.disp);
}

void test_refr_join_overlapping_areas(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*Two overlapping areas are cheaper to redraw together*/
    lv_area_t a1 = {10, 10, 49, 49};
    lv_area_t a2 = {20, 20, 59, 59};
    lv_area_t joined = {10, 10, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&joined, &refr_ctx.flush_area[0]));
}

void test_refr_dont_join_distant_areas(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*Joining these would cover much more than the two areas together*/
    lv_area_t a1 = {0, 0, 9, 9};
    lv_area_t a2 = {90, 90, 99, 99};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&a1, &refr_ctx.flush_area[0]));
    TEST_ASSERT_TRUE(lv_area_is_equal(&a2, &refr_ctx.flush_area[1]));
    /*Only the very last part of the very last area is flagged*/
    TEST_ASSERT_FALSE(refr_ctx.flush_is_last[0]);
    TEST_ASSERT_TRUE(refr_ctx.flush_is_last[1]);
}

void test_refr_join_contained_area(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*The small area is stored first, so it is not filtered out by lv_inv_area()*/
    lv_area_t small = {20, 20, 29, 29};
    lv_area_t big = {10, 10, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &small);
    lv_inv_area(refr_ctx.disp, &big);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&big, &refr_ctx.flush_area[0]));
}

void test_refr_invalid_areas_are_cleared_after_refreshing(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);

    lv_area_t a1 = {0, 0, 9, 9};
    lv_area_t a2 = {40, 40, 49, 49};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.disp->inv_p);
    uint32_t i;
    for(i = 0; i < LV_INV_BUF_SIZE; i++) {
        TEST_ASSERT_EQUAL_UINT8(0, refr_ctx.disp->inv_area_joined[i]);
        TEST_ASSERT_EQUAL_INT32(0, refr_ctx.disp->inv_areas[i].x1);
        TEST_ASSERT_EQUAL_INT32(0, refr_ctx.disp->inv_areas[i].y2);
    }
}

#endif /*LV_BUILD_TEST*/
