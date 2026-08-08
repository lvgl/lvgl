/**
 * Using the draw buffers: color formats, buffer count and stride
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

void test_refr_render_to_rgb565(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_RGB565, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
    refr_frame();

    ASSERT_PX_EQ(0xFFF80000, refr_screen_px(15, 15)); /*Red, quantized to 5 bits*/
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(50, 50));
}

void test_refr_render_to_rgb888(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_RGB888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_GREEN);
    refr_frame();

    ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(15, 15));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(50, 50));
}

void test_refr_transparent_screen_is_cleared(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_ARGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);

    /*Nothing covers the display, so the buffer has to be cleared before rendering*/
    lv_obj_t * scr = refr_screen();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(lv_display_get_layer_bottom(refr_ctx.disp), LV_OPA_TRANSP, 0);

    /*Fill the buffer with garbage to prove that it gets cleared*/
    lv_memset(refr_ctx.buf[0]->data, 0xAB, refr_ctx.buf[0]->data_size);

    refr_rect_create(scr, 10, 10, 20, 20, REFR_COLOR_RED);
    lv_obj_invalidate(scr);
    refr_frame();

    ASSERT_PX_EQ(0x00000000, refr_screen_px(50, 50));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(15, 15));
}

void test_refr_transparent_screen_clears_only_the_invalid_area(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_ARGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);

    lv_obj_t * scr = refr_screen();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(lv_display_get_layer_bottom(refr_ctx.disp), LV_OPA_TRANSP, 0);

    lv_obj_t * obj = refr_rect_create(scr, 0, 0, 20, 20, REFR_COLOR_RED);
    lv_obj_invalidate(scr);
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(10, 10));

    /*Invalidate an area away from the rectangle. The rectangle must survive in the buffer.*/
    lv_area_t far_area = {40, 40, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &far_area);
    refr_frame();

    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(refr_ctx.buf[0], 10, 10));
    ASSERT_PX_EQ(0x00000000, refr_buf_px(refr_ctx.buf[0], 50, 50));
    LV_UNUSED(obj);
}

void test_refr_indexed_format_is_not_tiled(void)
{
    /*Indexed formats can not be rendered in tiles, the palette is shared*/
    refr_disp_create(64, 32, LV_COLOR_FORMAT_I1, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 32);
    lv_display_set_tile_cnt(refr_ctx.disp, 4);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    lv_area_t full = {0, 0, 63, 31};
    TEST_ASSERT_TRUE(lv_area_is_equal(&full, &refr_ctx.flush_area[0]));
}

void test_refr_indexed_format_partial_mode(void)
{
    /*The palette is stored in the buffer, its space must not be used for rows*/
    const int32_t w = 848;
    const int32_t h = 64;
    refr_disp_create(w, h, LV_COLOR_FORMAT_I1, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 8);

    uint32_t stride = lv_draw_buf_width_to_stride((uint32_t)w, LV_COLOR_FORMAT_I1);
    uint32_t overhead = LV_COLOR_INDEXED_PALETTE_SIZE(LV_COLOR_FORMAT_I1) * sizeof(lv_color32_t);
    uint32_t data_size = refr_ctx.disp->buf_act->data_size;
    uint32_t max_row = refr_max_row_of(w, h);

    /*The rendered rows and the palette together have to fit into the buffer*/
    TEST_ASSERT_TRUE(max_row * stride + overhead <= data_size);
    TEST_ASSERT_TRUE((max_row + 1) * stride + overhead > data_size);
    TEST_ASSERT_TRUE(max_row < (uint32_t)h);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    lv_area_t screen_area = {0, 0, w - 1, h - 1};
    refr_check_flushed_chunks(&screen_area, max_row);
}

void test_refr_single_buffer_waits_before_rendering(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    TEST_ASSERT_FALSE(lv_display_is_double_buffered(refr_ctx.disp));
    lv_display_set_flush_wait_cb(refr_ctx.disp, refr_flush_wait_cb);
    refr_ctx.defer_flush_ready = true;
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*Rendering can only start once the previous flush is over*/
    lv_obj_invalidate(refr_screen());
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_wait_cnt);
    TEST_ASSERT_EQUAL_PTR(refr_ctx.buf[0], refr_ctx.disp->buf_act);
}

void test_refr_double_buffer_partial_swaps_on_every_flush(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 2, 16);
    TEST_ASSERT_TRUE(lv_display_is_double_buffered(refr_ctx.disp));
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    const lv_draw_buf_t * first = refr_ctx.disp->buf_act;
    const lv_draw_buf_t * second = first == refr_ctx.buf[0] ? refr_ctx.buf[1] : refr_ctx.buf[0];

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*The buffers alternate between the chunks*/
    TEST_ASSERT_TRUE(refr_ctx.flush_cnt > 1);
    uint32_t i;
    for(i = 0; i < refr_ctx.flush_cnt; i++) {
        TEST_ASSERT_EQUAL_PTR(i % 2 == 0 ? first : second, refr_ctx.flush_buf[i]);
    }
}

void test_refr_double_buffer_direct_swaps_only_after_the_last_area(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    lv_draw_buf_t * buf_before = refr_ctx.disp->buf_act;
    lv_area_t a1 = {0, 0, 9, 9};
    lv_area_t a2 = {50, 50, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.flush_cnt);
    /*Both areas of a frame go to the same buffer*/
    TEST_ASSERT_EQUAL_PTR(buf_before, refr_ctx.flush_buf[0]);
    TEST_ASSERT_EQUAL_PTR(buf_before, refr_ctx.flush_buf[1]);
    /*The swap happens after the frame is complete*/
    TEST_ASSERT_NOT_EQUAL(buf_before, refr_ctx.disp->buf_act);
}

void test_refr_triple_buffer_rotates_the_buffers(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 3, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    /*The buffers are used in a round robin fashion*/
    uint32_t start;
    for(start = 0; start < 3; start++) {
        if(refr_ctx.disp->buf_act == refr_ctx.buf[start]) break;
    }
    TEST_ASSERT_TRUE(start < 3);

    uint32_t i;
    for(i = 1; i <= 4; i++) {
        lv_obj_invalidate(refr_screen());
        refr_frame();
        TEST_ASSERT_EQUAL_PTR(refr_ctx.buf[(start + i) % 3], refr_ctx.disp->buf_act);
    }
}

void test_refr_full_mode_double_buffer(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_FULL, 2, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    const lv_draw_buf_t * first = refr_ctx.disp->buf_act;
    const lv_draw_buf_t * second = first == refr_ctx.buf[0] ? refr_ctx.buf[1] : refr_ctx.buf[0];

    /*Full mode always renders everything, so both buffers get a complete image*/
    refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_EQUAL_PTR(first, refr_ctx.flush_buf[0]);

    lv_obj_invalidate(refr_screen());
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.flush_cnt);
    TEST_ASSERT_EQUAL_PTR(second, refr_ctx.flush_buf[1]);

    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(refr_ctx.buf[0], 15, 15));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(refr_ctx.buf[1], 15, 15));
}

void test_refr_buffers_with_explicit_stride(void)
{
    /*A driver may hand over a frame buffer with padding at the end of the rows*/
    const int32_t w = 60;
    const int32_t h = 40;
    const uint32_t stride = 64 * 4;
    lv_memzero(&refr_ctx, sizeof(refr_ctx));
    refr_ctx.cf = LV_COLOR_FORMAT_XRGB8888;
    refr_ctx.hor_res = w;
    refr_ctx.ver_res = h;

    lv_display_t * disp = lv_display_create(w, h);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_XRGB8888);
    /*The draw buffer is padded to LV_DRAW_BUF_ALIGN*/
    uint32_t buf_size = LV_ROUND_UP(stride * (uint32_t)h, LV_DRAW_BUF_ALIGN);
    uint8_t * raw = lv_malloc_zeroed(buf_size + LV_DRAW_BUF_ALIGN);
    TEST_ASSERT_NOT_NULL(raw);
    uint8_t * aligned = lv_draw_buf_align(raw, LV_COLOR_FORMAT_XRGB8888);
    lv_display_set_buffers_with_stride(disp, aligned, NULL, buf_size, stride, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(disp, refr_flush_cb);
    refr_hide_sysmon(disp);
    refr_ctx.disp = disp;
    TEST_ASSERT_EQUAL_UINT32(0, disp->stride_is_auto);

    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
    refr_frame();

    /*The stride is kept when the layer's draw buffer is reshaped*/
    TEST_ASSERT_EQUAL_UINT32(stride, disp->buf_act->header.stride);
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_px_read(aligned, stride, LV_COLOR_FORMAT_XRGB8888, 15, 15));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_px_read(aligned, stride, LV_COLOR_FORMAT_XRGB8888, 50, 30));

    lv_display_delete(disp);
    refr_ctx.disp = NULL;
    lv_free(raw);
}

void test_refr_raw_buffers_use_automatic_stride(void)
{
    const int32_t w = 60;
    const int32_t h = 40;
    uint32_t stride = lv_draw_buf_width_to_stride(w, LV_COLOR_FORMAT_XRGB8888);
    lv_memzero(&refr_ctx, sizeof(refr_ctx));
    refr_ctx.cf = LV_COLOR_FORMAT_XRGB8888;
    refr_ctx.hor_res = w;
    refr_ctx.ver_res = h;

    lv_display_t * disp = lv_display_create(w, h);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_XRGB8888);
    uint32_t buf_size = LV_ROUND_UP(stride * (uint32_t)h, LV_DRAW_BUF_ALIGN);
    uint8_t * raw = lv_malloc_zeroed(buf_size + LV_DRAW_BUF_ALIGN);
    TEST_ASSERT_NOT_NULL(raw);
    uint8_t * aligned = lv_draw_buf_align(raw, LV_COLOR_FORMAT_XRGB8888);
    lv_display_set_buffers(disp, aligned, NULL, buf_size, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(disp, refr_flush_cb);
    refr_hide_sysmon(disp);
    refr_ctx.disp = disp;
    TEST_ASSERT_EQUAL_UINT32(1, disp->stride_is_auto);

    refr_screen_set_color(REFR_COLOR_BLUE);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(stride, disp->buf_act->header.stride);
    ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_px_read(aligned, stride, LV_COLOR_FORMAT_XRGB8888, 30, 20));

    lv_display_delete(disp);
    refr_ctx.disp = NULL;
    lv_free(raw);
}

void test_refr_tiled_rendering(void)
{
    refr_disp_create(80, 80, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 80);
    lv_display_set_tile_cnt(refr_ctx.disp, 4);
    TEST_ASSERT_EQUAL_UINT32(4, lv_display_get_tile_cnt(refr_ctx.disp));

    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_rect_create(refr_screen(), 0, 0, 80, 80, REFR_COLOR_GREEN);
    /*A rectangle crossing every tile boundary*/
    refr_rect_create(refr_screen(), 30, 0, 20, 80, REFR_COLOR_RED);
    /*An object on a layer of its own, so that the tiles have nested layers*/
    lv_obj_t * layered = refr_rect_create(refr_screen(), 0, 0, 20, 20, REFR_COLOR_BLUE);
    lv_obj_set_style_opa_layered(layered, 128, 0);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*Tiling is invisible from the outside: one flush with the complete image*/
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    lv_area_t full = {0, 0, 79, 79};
    TEST_ASSERT_TRUE(lv_area_is_equal(&full, &refr_ctx.flush_area[0]));

    int32_t y;
    for(y = 25; y < 80; y += 5) {
        ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(10, y));
        ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(40, y));
        ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(70, y));
    }
}

void test_refr_tiled_rendering_partial_mode(void)
{
    refr_disp_create(80, 80, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 20);
    lv_display_set_tile_cnt(refr_ctx.disp, 2);

    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_rect_create(refr_screen(), 0, 0, 80, 80, REFR_COLOR_BLUE);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(4, refr_ctx.flush_cnt);
    int32_t y;
    for(y = 0; y < 80; y += 5) {
        ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_screen_px(40, y));
    }
}

#endif /*LV_BUILD_TEST*/
