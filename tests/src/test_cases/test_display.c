#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"

/*Bypassing resolution check*/
#define TEST_DISPLAY_ASSERT_EQUAL_SCREENSHOT(path) TEST_ASSERT_MESSAGE(lv_test_screenshot_compare(path), path);

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    lv_display_set_matrix_rotation(NULL, false);
    lv_obj_clean(lv_screen_active());
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
    lv_display_delete(disp);
}

void test_get_drawbuf_size_single_buffered(void)
{
    static  LV_ATTRIBUTE_MEM_ALIGN uint8_t buf0[200 + LV_DRAW_BUF_ALIGN];

    lv_display_t * disp = lv_display_create(10, 20);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB888);

    lv_display_set_buffers(disp, lv_draw_buf_align(buf0, LV_COLOR_FORMAT_RGB888), NULL, 200,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    TEST_ASSERT_EQUAL(200,  lv_display_get_draw_buf_size(disp));
    lv_display_delete(disp);
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

#if LV_DRAW_TRANSFORM_USE_MATRIX
static void test_matrix_transform_area(lv_display_t * display)
{
    lv_area_t ori_area = { 0 };
    lv_area_set_width(&ori_area, 100);
    lv_area_set_height(&ori_area, 100);

    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);

    const int32_t hor_res = lv_display_get_original_horizontal_resolution(display);
    const int32_t ver_res = lv_display_get_original_vertical_resolution(display);

    switch(lv_display_get_rotation(display)) {
        case LV_DISPLAY_ROTATION_0:
            break;
        case LV_DISPLAY_ROTATION_90:
            lv_matrix_rotate(&matrix, 270);
            lv_matrix_translate(&matrix, -ver_res, 0);
            break;
        case LV_DISPLAY_ROTATION_180:
            lv_matrix_rotate(&matrix, 180);
            lv_matrix_translate(&matrix, -hor_res, -ver_res);
            break;
        case LV_DISPLAY_ROTATION_270:
            lv_matrix_rotate(&matrix, 90);
            lv_matrix_translate(&matrix, 0, -hor_res);
            break;
    }

    lv_area_t disp_area = ori_area;
    lv_display_rotate_area(display, &disp_area);

    lv_area_t matrix_area = lv_matrix_transform_area(&matrix, &ori_area);

    TEST_ASSERT_TRUE(lv_area_is_equal(&disp_area, &matrix_area));
}
#endif

void test_display_matrix_rotation(void)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 300, 200);
    lv_obj_set_pos(obj, 30, 20);
    lv_obj_t * label = lv_label_create(obj);

    lv_display_t * disp = lv_obj_get_display(obj);
    lv_display_set_matrix_rotation(disp, true);
    TEST_ASSERT_TRUE(lv_display_get_matrix_rotation(disp));

    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);
    test_matrix_transform_area(disp);
    lv_label_set_text(label, "Rotation: 0 degrees");
    TEST_DISPLAY_ASSERT_EQUAL_SCREENSHOT("display_matrix_rotation_0.png");

    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);
    test_matrix_transform_area(disp);
    lv_label_set_text(label, "Rotation: 90 degrees");
    TEST_DISPLAY_ASSERT_EQUAL_SCREENSHOT("display_matrix_rotation_90.png");

    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_180);
    test_matrix_transform_area(disp);
    lv_label_set_text(label, "Rotation: 180 degrees");
    TEST_DISPLAY_ASSERT_EQUAL_SCREENSHOT("display_matrix_rotation_180.png");

    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);
    test_matrix_transform_area(disp);
    lv_label_set_text(label, "Rotation: 270 degrees");
    TEST_DISPLAY_ASSERT_EQUAL_SCREENSHOT("display_matrix_rotation_270.png");

    lv_display_set_matrix_rotation(disp, false);
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);
    test_matrix_transform_area(disp);
    lv_label_set_text(label, "Rotation: 0 degrees");
    TEST_DISPLAY_ASSERT_EQUAL_SCREENSHOT("display_matrix_rotation_0.png");
#else
    TEST_PASS();
#endif
}

static void dummy_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    LV_UNUSED(area);
    LV_UNUSED(color_p);
    lv_display_flush_ready(disp);
}

void test_display_triple_buffer(void)
{
    lv_display_t * disp = lv_display_create(480, 320);
    lv_display_set_flush_cb(disp, dummy_flush_cb);
    lv_draw_buf_t * buf1 = lv_draw_buf_create(480, 320, LV_COLOR_FORMAT_NATIVE, 0);
    lv_draw_buf_t * buf2 = lv_draw_buf_create(480, 320, LV_COLOR_FORMAT_NATIVE, 0);
    lv_draw_buf_t * buf3 = lv_draw_buf_create(480, 320, LV_COLOR_FORMAT_NATIVE, 0);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_draw_buffers(disp, buf1, buf2);
    lv_display_set_3rd_draw_buffer(disp, buf3);

    lv_obj_invalidate(lv_display_get_screen_active(disp));
    lv_display_refr_timer(lv_display_get_refr_timer(disp));
    TEST_ASSERT_EQUAL(lv_display_get_buf_active(disp), buf2);

    lv_obj_invalidate(lv_display_get_screen_active(disp));
    lv_display_refr_timer(lv_display_get_refr_timer(disp));
    TEST_ASSERT_EQUAL(lv_display_get_buf_active(disp), buf3);

    lv_obj_invalidate(lv_display_get_screen_active(disp));
    lv_display_refr_timer(lv_display_get_refr_timer(disp));
    TEST_ASSERT_EQUAL(lv_display_get_buf_active(disp), buf1);

    lv_display_delete(disp);
    lv_draw_buf_destroy(buf1);
    lv_draw_buf_destroy(buf2);
    lv_draw_buf_destroy(buf3);
}

static void refr_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    int * called = lv_event_get_user_data(e);
    (*called)++;
    /* We should not receive the LV_EVENT_REFR_READY event as the display was deleted*/
    TEST_ASSERT_EQUAL(code, LV_EVENT_REFR_START);
    lv_display_delete(lv_event_get_current_target(e));
}

static void never_called(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    LV_UNUSED(disp);
    LV_UNUSED(area);
    LV_UNUSED(color_p);
    TEST_FAIL();
}

void test_display_deleted_during_event(void)
{
    lv_display_t * disp = lv_display_create(480, 320);
    lv_display_set_flush_cb(disp, never_called);
    lv_draw_buf_t * buf1 = lv_draw_buf_create(480, 320, LV_COLOR_FORMAT_NATIVE, 0);
    lv_display_set_draw_buffers(disp, buf1, NULL);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_DIRECT);
    int called = 0;
    lv_display_add_event_cb(disp, refr_event_handler, LV_EVENT_REFR_START, &called);
    lv_display_add_event_cb(disp, refr_event_handler, LV_EVENT_REFR_READY, &called);
    lv_refr_now(disp);
    TEST_ASSERT_EQUAL(called, 1);
    lv_draw_buf_destroy(buf1);
}

#endif
