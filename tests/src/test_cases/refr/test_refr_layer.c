/**
 * Drawing the screens and the layers
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

void test_refr_bottom_layer_is_drawn_when_nothing_covers(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);

    lv_obj_t * scr = refr_screen();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_TRANSP, 0);

    lv_obj_t * bottom = lv_display_get_layer_bottom(refr_ctx.disp);
    lv_obj_set_style_bg_opa(bottom, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(bottom, lv_color_hex(REFR_COLOR_BLUE), 0);

    lv_obj_invalidate(scr);
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_screen_px(30, 30));

    /*An opaque screen covers the bottom layer, so it is not drawn at all*/
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scr, lv_color_hex(REFR_COLOR_RED), 0);
    lv_obj_invalidate(scr);
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(30, 30));
}

void test_refr_top_and_sys_layers_are_always_drawn(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    /*The screen covers everything below, but the top and sys layers are above it*/
    refr_rect_create(refr_screen(), 0, 0, 64, 64, REFR_COLOR_RED);
    refr_rect_create(lv_display_get_layer_top(refr_ctx.disp), 0, 0, 20, 20, REFR_COLOR_GREEN);
    refr_rect_create(lv_display_get_layer_sys(refr_ctx.disp), 30, 0, 20, 20, REFR_COLOR_BLUE);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(10, 10));
    ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_screen_px(40, 10));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(10, 40));
}

void test_refr_previous_screen_drawn_below_the_active_one(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_t * disp_def = lv_display_get_default();
    lv_display_set_default(refr_ctx.disp);
    lv_obj_t * prev = lv_obj_create(NULL);
    lv_display_set_default(disp_def);

    lv_obj_remove_style_all(prev);
    lv_obj_set_style_bg_opa(prev, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(prev, lv_color_hex(REFR_COLOR_GREEN), 0);

    refr_screen_set_color(REFR_COLOR_RED);

    /*Both screens cover the display. The active one is drawn last, so it wins.*/
    refr_ctx.disp->prev_scr = prev;
    refr_ctx.disp->draw_prev_over_act = 0;
    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(30, 30));

    /*With draw_prev_over_act the previous screen is drawn on top*/
    refr_ctx.disp->draw_prev_over_act = 1;
    lv_obj_invalidate(refr_screen());
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(30, 30));

    refr_ctx.disp->prev_scr = NULL;
    refr_ctx.disp->draw_prev_over_act = 0;
    lv_obj_delete(prev);
}

void test_refr_screen_load_animation(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_t * disp_def = lv_display_get_default();
    lv_display_set_default(refr_ctx.disp);

    refr_screen_set_color(REFR_COLOR_RED);
    lv_obj_t * next = lv_obj_create(NULL);
    lv_obj_remove_style_all(next);
    lv_obj_set_style_bg_opa(next, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(next, lv_color_hex(REFR_COLOR_GREEN), 0);

    lv_screen_load_anim(next, LV_SCREEN_LOAD_ANIM_MOVE_LEFT, 100, 0, false);

    /*Half way through the animation both screens are on the display*/
    lv_tick_inc(50);
    lv_timer_handler();
    refr_frame();
    TEST_ASSERT_NOT_NULL(lv_display_get_screen_prev(refr_ctx.disp));

    bool red_seen = false;
    bool green_seen = false;
    int32_t x;
    for(x = 0; x < 64; x++) {
        uint32_t px = refr_screen_px(x, 30);
        if(px == REFR_COLOR_RED) red_seen = true;
        if(px == REFR_COLOR_GREEN) green_seen = true;
    }
    ASSERT_PX_TRUE(red_seen);
    ASSERT_PX_TRUE(green_seen);

    /*At the end only the new screen is left*/
    lv_tick_inc(100);
    lv_timer_handler();
    refr_frame();
    TEST_ASSERT_NULL(lv_display_get_screen_prev(refr_ctx.disp));
    ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(30, 30));

    lv_display_set_default(disp_def);
}

void test_refr_hidden_object_is_not_drawn(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);
    lv_obj_set_hidden(obj, true);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(15, 15));
}

void test_refr_fully_transparent_layer_is_not_drawn(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);

    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);
    lv_obj_set_style_opa_layered(obj, LV_OPA_TRANSP, 0);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(15, 15));
}

void test_refr_layered_opacity_is_blended(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_WHITE);
    lv_obj_set_style_opa_layered(obj, 128, 0);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    uint32_t px = refr_screen_px(15, 15);
    uint32_t r = (px >> 16) & 0xFF;
    /*White over black with 50% opacity*/
    ASSERT_PX_WITHIN(4, 128, r);
}

void test_refr_simple_layer_is_split_into_chunks(void)
{
    /*A simple layer is rendered in chunks that fit into LV_DRAW_LAYER_SIMPLE_BUF_SIZE*/
    const int32_t w = 200;
    const int32_t h = 100;
    refr_disp_create(w + 20, h + 20, LV_COLOR_FORMAT_RGB565, LV_DISPLAY_RENDER_MODE_DIRECT, 1, h + 20);
    refr_screen_set_color(REFR_COLOR_BLACK);

    lv_obj_t * obj = refr_rect_create(refr_screen(), 0, 0, w, h, REFR_COLOR_WHITE);
    lv_obj_set_style_opa_layered(obj, 128, 0);
    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*The object is opaque, so the chunks are rendered without an alpha channel*/
    uint32_t px_size = lv_color_format_get_size(LV_COLOR_FORMAT_RGB565);
    uint32_t max_rgb_rows = LV_DRAW_LAYER_SIMPLE_BUF_SIZE / (uint32_t)w / px_size;
    uint32_t expected_rgb = ((uint32_t)h + max_rgb_rows - 1) / max_rgb_rows;
    TEST_ASSERT_EQUAL_UINT32(expected_rgb, draw_cnt);

    /*Every chunk is blended the same way*/
    uint32_t reference = refr_screen_px(w / 2, 0);
    ASSERT_PX_WITHIN(12, 128, (reference >> 16) & 0xFF);
    int32_t y;
    for(y = 0; y < h; y += 7) {
        ASSERT_PX_EQ(reference, refr_screen_px(w / 2, y));
    }

    /*With a rounded corner the layer needs an alpha channel, so the chunks are smaller*/
    draw_cnt = 0;
    lv_obj_set_style_radius(obj, 20, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    uint32_t max_argb_rows = LV_DRAW_LAYER_SIMPLE_BUF_SIZE / (uint32_t)w / sizeof(lv_color32_t);
    uint32_t expected_argb = ((uint32_t)h + max_argb_rows - 1) / max_argb_rows;
    TEST_ASSERT_EQUAL_UINT32(expected_argb, draw_cnt);
    TEST_ASSERT_TRUE(expected_argb >= expected_rgb);
}

void test_refr_bitmap_mask_forces_an_alpha_layer(void)
{
    /*An object with a bitmap mask always needs an alpha channel, even if it is
     *opaque. So its layer is split into the smaller, ARGB sized chunks.*/
    const int32_t w = 200;
    const int32_t h = 100;
    refr_disp_create(w + 20, h + 20, LV_COLOR_FORMAT_RGB565, LV_DISPLAY_RENDER_MODE_DIRECT, 1, h + 20);
    refr_screen_set_color(REFR_COLOR_BLACK);

    lv_obj_t * obj = refr_rect_create(refr_screen(), 0, 0, w, h, REFR_COLOR_WHITE);
    /*Keep the object on a layer of its own in every configuration*/
    lv_obj_set_style_opa_layered(obj, 128, 0);
    uint32_t draw_cnt = 0;
    lv_obj_add_event_cb(obj, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &draw_cnt);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    uint32_t px_size = lv_color_format_get_size(LV_COLOR_FORMAT_RGB565);
    uint32_t max_rgb_rows = LV_DRAW_LAYER_SIMPLE_BUF_SIZE / (uint32_t)w / px_size;
    TEST_ASSERT_EQUAL_UINT32(((uint32_t)h + max_rgb_rows - 1) / max_rgb_rows, draw_cnt);

    LV_IMAGE_DECLARE(test_image_cogwheel_a8);
    lv_obj_set_style_bitmap_mask_src(obj, &test_image_cogwheel_a8, 0);
    TEST_ASSERT_EQUAL(LV_LAYER_TYPE_SIMPLE, lv_obj_get_layer_type(obj));

    draw_cnt = 0;
    lv_obj_invalidate(refr_screen());
    refr_frame();

    uint32_t max_argb_rows = LV_DRAW_LAYER_SIMPLE_BUF_SIZE / (uint32_t)w / sizeof(lv_color32_t);
    TEST_ASSERT_EQUAL_UINT32(((uint32_t)h + max_argb_rows - 1) / max_argb_rows, draw_cnt);
}

void test_refr_children_of_a_transparent_parent_are_skipped(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * parent = refr_rect_create(refr_screen(), 10, 10, 60, 60, REFR_COLOR_BLUE);
    lv_obj_t * child = refr_rect_create(parent, 0, 0, 20, 20, REFR_COLOR_RED);

    uint32_t child_draw_cnt = 0;
    lv_obj_add_event_cb(child, refr_draw_main_counter_cb, LV_EVENT_DRAW_MAIN, &child_draw_cnt);

    /*A completely transparent parent makes the whole subtree invisible*/
    lv_obj_set_style_opa(parent, LV_OPA_TRANSP, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, child_draw_cnt);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(15, 15));
}

#endif /*LV_BUILD_TEST*/
