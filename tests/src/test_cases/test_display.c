#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

struct display_area_test_set {
    /* Parameters for setting up the display */
    uint32_t width;
    uint32_t height;
    lv_color_format_t color_format;
    lv_display_render_mode_t render_mode;
    /* Parameters for testing */
    uint32_t invalidated_width;
    uint32_t invalidated_height;
    uint32_t expected_buf0_size;
};

void test_get_drawbuf_size_double_buffered(void)
{
    static  LV_ATTRIBUTE_MEM_ALIGN uint8_t buf0[200 + LV_DRAW_BUF_ALIGN];
    static  LV_ATTRIBUTE_MEM_ALIGN uint8_t buf1[200 + LV_DRAW_BUF_ALIGN];

    lv_display_t * disp = lv_display_create(10, 20);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB888);

    lv_display_set_buffers(disp, lv_draw_buf_align(buf0, LV_COLOR_FORMAT_RGB888), lv_draw_buf_align(buf1,
                                                                                                    LV_COLOR_FORMAT_RGB888), 200, LV_DISPLAY_RENDER_MODE_PARTIAL);

    TEST_ASSERT_EQUAL(200, lv_display_get_draw_buf_size(disp));
}

void test_get_drawbuf_size_single_buffered(void)
{
    static  LV_ATTRIBUTE_MEM_ALIGN uint8_t buf0[200 + LV_DRAW_BUF_ALIGN];

    lv_display_t * disp = lv_display_create(10, 20);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB888);

    lv_display_set_buffers(disp, lv_draw_buf_align(buf0, LV_COLOR_FORMAT_RGB888), NULL, 200,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    TEST_ASSERT_EQUAL(200,  lv_display_get_draw_buf_size(disp));
}

static void exec_invalidated_drawbuf_size_test(const struct display_area_test_set * test_set)
{
    uint32_t buffer_size = lv_draw_buf_width_to_stride(test_set->width, test_set->color_format) * test_set->height;

    uint8_t * buf0 = lv_malloc(buffer_size + LV_DRAW_BUF_ALIGN);

    lv_display_t * disp = lv_display_create(test_set->width, test_set->height);
    lv_display_set_color_format(disp, test_set->color_format);

    lv_display_set_buffers(disp, lv_draw_buf_align(buf0, test_set->color_format), NULL, buffer_size,
                           test_set->render_mode);
    uint32_t invalidated_size = lv_display_get_invalidated_draw_buf_size(disp, test_set->invalidated_width,
                                                                         test_set->invalidated_height);

    TEST_ASSERT_EQUAL(test_set->expected_buf0_size, invalidated_size);
    lv_free(buf0);
    lv_display_delete(disp);
}

void test_get_invalidated_drawbuf_size_rgb888_partial()
{
    struct display_area_test_set test_set = {
        .width = 10,
        .height = 20,
        .color_format = LV_COLOR_FORMAT_RGB888,
        .render_mode = LV_DISPLAY_RENDER_MODE_PARTIAL,
        .invalidated_width = 5,
        .invalidated_height = 5,
        .expected_buf0_size = lv_draw_buf_width_to_stride(5, LV_COLOR_FORMAT_RGB888) * 5,
    };
    exec_invalidated_drawbuf_size_test(&test_set);
}

void test_get_invalidated_drawbuf_size_rgb888_full()
{
    struct display_area_test_set test_set = {
        .width = 10,
        .height = 20,
        .color_format = LV_COLOR_FORMAT_RGB888,
        .render_mode = LV_DISPLAY_RENDER_MODE_FULL,
        .invalidated_width = 10,
        .invalidated_height = 20,
        .expected_buf0_size = lv_draw_buf_width_to_stride(10, LV_COLOR_FORMAT_RGB888) * 20,
    };
    exec_invalidated_drawbuf_size_test(&test_set);
}

void test_get_invalidated_drawbuf_size_i1_full()
{
    struct display_area_test_set test_set =  {
        .width = 180,
        .height = 90,
        .color_format = LV_COLOR_FORMAT_I1,
        .render_mode = LV_DISPLAY_RENDER_MODE_FULL,
        .invalidated_width = 180,
        .invalidated_height = 90,
        .expected_buf0_size = lv_draw_buf_width_to_stride(180, LV_COLOR_FORMAT_I1) * 90,
    };
    exec_invalidated_drawbuf_size_test(&test_set);
}

void test_get_invalidated_drawbuf_size_i1_partial()
{
    struct display_area_test_set test_set = {
        .width = 180,
        .height = 90,
        .color_format = LV_COLOR_FORMAT_I1,
        .render_mode = LV_DISPLAY_RENDER_MODE_PARTIAL,
        .invalidated_width = 180,
        .invalidated_height = 10,
        .expected_buf0_size = lv_draw_buf_width_to_stride(180, LV_COLOR_FORMAT_I1) * 10,
    };
    exec_invalidated_drawbuf_size_test(&test_set);
}


#endif
