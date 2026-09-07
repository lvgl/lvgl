/**
 * Drawing the transformed displays and widgets
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

void test_refr_transformed_object_uses_a_layer(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 30, 30, 40, 40, REFR_COLOR_RED);
    lv_obj_set_style_transform_pivot_x(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_rotation(obj, 900, 0);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_TRANSFORM, lv_obj_get_layer_type(obj));

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, draw_cnt);
    /*A square rotated by 90 degrees around its center covers the same area*/
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 50));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(10, 10));
}

void test_refr_rotation_is_normalized(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 30, 30, 40, 40, REFR_COLOR_RED);
    lv_obj_set_style_transform_pivot_x(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);

    /*Angles out of the 0..3600 range are wrapped around*/
    lv_obj_set_style_transform_rotation(obj, 3600 + 900, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 50));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(10, 10));

    lv_obj_set_style_transform_rotation(obj, -900, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 50));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(10, 10));
}

void test_refr_scaled_and_skewed_object(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 40, 40, 20, 20, REFR_COLOR_RED);
    lv_obj_set_style_transform_pivot_x(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);

    /*Twice as large around its center*/
    lv_obj_set_style_transform_scale_x(obj, 2 * LV_SCALE_NONE, 0);
    lv_obj_set_style_transform_scale_y(obj, 2 * LV_SCALE_NONE, 0);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_TRANSFORM, lv_obj_get_layer_type(obj));

    lv_obj_invalidate(refr_screen());
    refr_frame();
    /*The object covers twice as much as before*/
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 50));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(35, 50));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 65));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(10, 10));

    /*A skewed object is rendered too. How it looks depends on the renderer,
     *only the fact that it is drawn is checked here.*/
    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);
    lv_obj_set_style_transform_scale_x(obj, LV_SCALE_NONE, 0);
    lv_obj_set_style_transform_scale_y(obj, LV_SCALE_NONE, 0);
    lv_obj_set_style_transform_skew_x(obj, 20, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(1, draw_cnt);
}

void test_refr_zero_scale_is_not_drawn(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 30, 30, 40, 40, REFR_COLOR_RED);
    lv_obj_set_style_transform_scale_x(obj, 0, 0);

    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(50, 50));
}

#if LV_DRAW_TRANSFORM_USE_MATRIX

/*The pixels of a rotated display are not where the logical coordinates say,
 *so the draw buffer is checked directly*/
static void check_rotated_rect(const lv_draw_buf_t * buf, const lv_area_t * logical_area, uint32_t color)
{
    lv_area_t physical = *logical_area;
    lv_display_rotate_area(refr_ctx.disp, &physical);

    ASSERT_PX_EQ(color, refr_buf_px(buf, physical.x1, physical.y1));
    ASSERT_PX_EQ(color, refr_buf_px(buf, physical.x2, physical.y2));
    ASSERT_PX_EQ(color, refr_buf_px(buf, (physical.x1 + physical.x2) / 2,
                                    (physical.y1 + physical.y2) / 2));
}

#endif /*LV_DRAW_TRANSFORM_USE_MATRIX*/

void test_refr_matrix_display_rotation(void)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    /*A square display so that every rotation uses the same buffer size*/
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_set_matrix_rotation(refr_ctx.disp, true);
    TEST_ASSERT_TRUE(lv_display_get_matrix_rotation(refr_ctx.disp));

    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_rect_create(refr_screen(), 0, 0, 20, 20, REFR_COLOR_RED);

    static const lv_display_rotation_t rotations[] = {
        LV_DISPLAY_ROTATION_0, LV_DISPLAY_ROTATION_90,
        LV_DISPLAY_ROTATION_180, LV_DISPLAY_ROTATION_270,
    };
    lv_area_t logical = {0, 0, 19, 19};

    size_t i;
    for(i = 0; i < sizeof(rotations) / sizeof(rotations[0]); i++) {
        lv_display_set_rotation(refr_ctx.disp, rotations[i]);
        lv_obj_invalidate(refr_screen());
        refr_frame();

        /*The buffer is always the size of the unrotated display*/
        TEST_ASSERT_EQUAL_INT32(lv_display_get_original_horizontal_resolution(refr_ctx.disp),
                                (int32_t)refr_ctx.disp->buf_act->header.w);
        check_rotated_rect(refr_ctx.disp->buf_act, &logical, REFR_COLOR_RED);
    }

    lv_display_set_rotation(refr_ctx.disp, LV_DISPLAY_ROTATION_0);
    lv_display_set_matrix_rotation(refr_ctx.disp, false);
#else
    TEST_IGNORE_MESSAGE("LV_DRAW_TRANSFORM_USE_MATRIX not enabled");
#endif
}

void test_refr_matrix_sync_areas_are_rotated(void)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    lv_display_set_matrix_rotation(refr_ctx.disp, true);
    lv_display_set_rotation(refr_ctx.disp, LV_DISPLAY_ROTATION_90);

    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*Render a rectangle into one buffer only*/
    const lv_draw_buf_t * rendered_buf = refr_ctx.disp->buf_act;
    refr_rect_create(refr_screen(), 0, 0, 20, 20, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());
    lv_area_t logical = {0, 0, 19, 19};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &logical);
    refr_frame();
    check_rotated_rect(rendered_buf, &logical, REFR_COLOR_RED);

    /*Refresh an unrelated area. The rectangle has to be copied to the other
     *buffer at its rotated position.*/
    const lv_draw_buf_t * next_buf = refr_ctx.disp->buf_act;
    TEST_ASSERT_NOT_EQUAL(rendered_buf, next_buf);
    lv_area_t other = {40, 40, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &other);
    refr_frame();
    check_rotated_rect(next_buf, &logical, REFR_COLOR_RED);

    lv_display_set_rotation(refr_ctx.disp, LV_DISPLAY_ROTATION_0);
    lv_display_set_matrix_rotation(refr_ctx.disp, false);
#else
    TEST_IGNORE_MESSAGE("LV_DRAW_TRANSFORM_USE_MATRIX not enabled");
#endif
}

void test_refr_obj_matrix_is_applied_without_a_layer(void)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);

    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);
    lv_matrix_translate(&matrix, 40, 30);
    lv_obj_set_transform(obj, &matrix);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_TRANSFORM, lv_obj_get_layer_type(obj));

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*A fully opaque transformed object is drawn directly with a matrix*/
    TEST_ASSERT_EQUAL_UINT32(1, draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(60, 50));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(15, 15));

    lv_obj_set_transform(obj, NULL);
#else
    TEST_IGNORE_MESSAGE("LV_DRAW_TRANSFORM_USE_MATRIX not enabled");
#endif
}

void test_refr_obj_matrix_not_invertible(void)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);

    /*A matrix that collapses everything to a line can not be inverted*/
    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);
    lv_matrix_scale(&matrix, 1.0f, 0.0f);
    lv_obj_set_transform(obj, &matrix);

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(15, 15));

    lv_obj_set_transform(obj, NULL);
#else
    TEST_IGNORE_MESSAGE("LV_DRAW_TRANSFORM_USE_MATRIX not enabled");
#endif
}

void test_refr_rotated_obj_clipped_by_the_invalid_area(void)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 30, 30, 40, 40, REFR_COLOR_RED);
    lv_obj_set_style_transform_pivot_x(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_rotation(obj, 900, 0);

    /*The whole object is inside the clip area, so the matrix can be used*/
    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 50));

    /*Now only a part of the object is refreshed, so it is rendered to a layer*/
    lv_area_t part = {40, 40, 59, 59};
    refr_log_reset();
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &part);
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(50, 50));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(10, 10));
#else
    TEST_IGNORE_MESSAGE("LV_DRAW_TRANSFORM_USE_MATRIX not enabled");
#endif
}

#endif /*LV_BUILD_TEST*/
