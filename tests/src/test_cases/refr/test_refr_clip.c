/**
 * Propagating the clip area while the widgets are drawn
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

void test_refr_object_outside_the_clip_area_is_not_drawn(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
    lv_obj_t * layered = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_GREEN);
    lv_obj_set_style_opa_layered(layered, LV_OPA_50, 0);

    uint32_t draw_cnt = 0;
    uint32_t layered_draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);
    lv_obj_add_event_cb(layered, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &layered_draw_cnt);
    lv_obj_update_layout(refr_screen());

    /*Refresh an area far from the objects*/
    lv_area_t area = {40, 40, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, draw_cnt);
    TEST_ASSERT_EQUAL_UINT32(0, layered_draw_cnt);
}

void test_refr_transformed_object_outside_the_clip_area(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
    lv_obj_set_style_transform_pivot_x(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_rotation(obj, 450, 0);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_TRANSFORM, lv_obj_get_layer_type(obj));

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);
    lv_obj_update_layout(refr_screen());

    /*The transformed area of the object is not in the refreshed area*/
    lv_area_t area = {60, 60, 79, 79};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, draw_cnt);
}

void test_refr_transformed_object_corner_of_the_bounding_box(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 30, 30, 40, 40, REFR_COLOR_RED);
    lv_obj_set_style_transform_pivot_x(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);
    lv_obj_set_style_transform_rotation(obj, 450, 0);

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);
    lv_obj_update_layout(refr_screen());

    /*The corner of the bounding box of a rotated square is empty. It overlaps the
     *transformed area, but no part of the object is rendered there.*/
    lv_area_t corner = {22, 22, 24, 24};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &corner);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(23, 23));
}

void test_refr_children_are_clipped_to_the_parent(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * parent = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_BLUE);
    refr_rect_create(parent, 20, 20, 60, 60, REFR_COLOR_RED);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*The child is visible inside the parent*/
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(40, 40));
    /*and clipped outside of it*/
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(60, 60));
}

static void ext_draw_size_cb(lv_event_t * e)
{
    lv_event_set_ext_draw_size(e, 40);
}

void test_refr_overflow_visible_extends_the_clip_area(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * parent = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_BLUE);
    lv_obj_add_event_cb(parent, ext_draw_size_cb, LV_EVENT_REFR_EXT_DRAW_SIZE, NULL);
    lv_obj_refresh_ext_draw_size(parent);
    refr_rect_create(parent, 20, 20, 60, 60, REFR_COLOR_RED);

    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(60, 60));

    /*With overflow visible the children may draw on the extended draw area*/
    lv_obj_set_overflow_visible(parent, true);
    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(60, 60));
}

void test_refr_clip_corner_splits_the_children(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * parent = refr_rect_create(refr_screen(), 10, 10, 60, 60, REFR_COLOR_BLUE);
    lv_obj_set_style_radius(parent, 20, 0);
    lv_obj_set_style_clip_corner(parent, true, 0);

    lv_obj_t * child = refr_rect_create(parent, 0, 0, 60, 60, REFR_COLOR_RED);
    uint32_t child_draw_cnt = 0;
    lv_obj_add_event_cb(child, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &child_draw_cnt);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*The children are rendered three times: the top, the middle and the bottom part*/
    TEST_ASSERT_EQUAL_UINT32(3, child_draw_cnt);
    /*The corner of the child is cut off, the middle is not*/
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(11, 11));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(40, 40));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(40, 12));

    /*Without a radius there is nothing to clip*/
    child_draw_cnt = 0;
    lv_obj_set_style_radius(parent, 0, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(1, child_draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(11, 11));
}

void test_refr_post_draw_events_are_sent(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * parent = refr_rect_create(refr_screen(), 10, 10, 40, 40, REFR_COLOR_BLUE);
    lv_obj_t * child = refr_rect_create(parent, 0, 0, 10, 10, REFR_COLOR_RED);

    uint32_t parent_post = 0;
    uint32_t child_post = 0;
    lv_obj_add_event_cb(parent, refr_draw_main_counter_cb, LV_EVENT_DRAW_POST, &parent_post);
    lv_obj_add_event_cb(child, refr_draw_main_counter_cb, LV_EVENT_DRAW_POST, &child_post);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*Both the object with children and the leaf object get the post draw events*/
    TEST_ASSERT_EQUAL_UINT32(1, parent_post);
    TEST_ASSERT_EQUAL_UINT32(1, child_post);
}

#endif /*LV_BUILD_TEST*/
