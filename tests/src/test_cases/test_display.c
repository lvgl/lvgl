#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"

/*Bypassing resolution check*/
#define TEST_DISPLAY_ASSERT_EQUAL_SCREENSHOT(path) TEST_ASSERT_MESSAGE(lv_test_screenshot_compare(path), path);

#define TEST_DPX_CALC(dpi, n)   ((n) == 0 ? 0 :LV_MAX((( (dpi) * (n) + 80) / 160), 1)) /*+80 for rounding*/

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    lv_display_set_matrix_rotation(NULL, false);
    lv_obj_clean(lv_screen_active());
    lv_obj_clean(lv_layer_top());
    lv_obj_clean(lv_layer_sys());
    lv_obj_clean(lv_layer_bottom());
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

void test_get_invalidated_drawbuf_size_rgb888_partial(void)
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

void test_get_invalidated_drawbuf_size_rgb888_full(void)
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

void test_get_invalidated_drawbuf_size_i1_full(void)
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

void test_get_invalidated_drawbuf_size_i1_partial(void)
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

    if(disp->last_area && disp->last_part) {
        TEST_ASSERT_TRUE(lv_display_flush_is_last(disp));
    }

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

static void test_display_resolution_full_rotation(
    lv_display_t * disp,
    int32_t ori_hor_res, int32_t ori_ver_res,
    int32_t ori_physical_hor_res, int32_t ori_physical_ver_res,
    int32_t ori_offset_x, int32_t ori_offset_y)
{
    const lv_display_rotation_t rotations[] = {
        LV_DISPLAY_ROTATION_0,
        LV_DISPLAY_ROTATION_90,
        LV_DISPLAY_ROTATION_180,
        LV_DISPLAY_ROTATION_270,
        LV_DISPLAY_ROTATION_0,
    };

    for(size_t i = 0; i < sizeof(rotations) / sizeof(rotations[0]); i++) {
        lv_display_set_rotation(disp, rotations[i]);

        if(!disp && !lv_display_get_default()) {
            TEST_ASSERT_EQUAL(LV_DISPLAY_ROTATION_0, lv_display_get_rotation(disp));
        }
        else {
            TEST_ASSERT_EQUAL(rotations[i], lv_display_get_rotation(disp));
        }

        /* Original resolution should be the same */
        TEST_ASSERT_EQUAL_INT32(ori_hor_res, lv_display_get_original_horizontal_resolution(disp));
        TEST_ASSERT_EQUAL_INT32(ori_ver_res, lv_display_get_original_vertical_resolution(disp));

        /* verify resolution */
        switch(lv_display_get_rotation(disp)) {
            case LV_DISPLAY_ROTATION_0:
            case LV_DISPLAY_ROTATION_180:
                TEST_ASSERT_EQUAL_INT32(ori_physical_hor_res, lv_display_get_physical_horizontal_resolution(disp));
                TEST_ASSERT_EQUAL_INT32(ori_physical_ver_res, lv_display_get_physical_vertical_resolution(disp));
                TEST_ASSERT_EQUAL_INT32(ori_hor_res, lv_display_get_horizontal_resolution(disp));
                TEST_ASSERT_EQUAL_INT32(ori_ver_res, lv_display_get_vertical_resolution(disp));
                break;
            case LV_DISPLAY_ROTATION_90:
            case LV_DISPLAY_ROTATION_270:
                TEST_ASSERT_EQUAL_INT32(ori_physical_ver_res, lv_display_get_physical_horizontal_resolution(disp));
                TEST_ASSERT_EQUAL_INT32(ori_physical_hor_res, lv_display_get_physical_vertical_resolution(disp));
                TEST_ASSERT_EQUAL_INT32(ori_ver_res, lv_display_get_horizontal_resolution(disp));
                TEST_ASSERT_EQUAL_INT32(ori_hor_res, lv_display_get_vertical_resolution(disp));
                break;
            default:
                TEST_FAIL();
                break;
        }

        /* verify offset */
        switch(lv_display_get_rotation(disp)) {
            case LV_DISPLAY_ROTATION_0:
                TEST_ASSERT_EQUAL_INT32(ori_offset_x, lv_display_get_offset_x(disp));
                TEST_ASSERT_EQUAL_INT32(ori_offset_y, lv_display_get_offset_y(disp));
                break;
            case LV_DISPLAY_ROTATION_180:
                TEST_ASSERT_EQUAL_INT32(ori_physical_hor_res - ori_offset_x, lv_display_get_offset_x(disp));
                TEST_ASSERT_EQUAL_INT32(ori_physical_ver_res - ori_offset_y, lv_display_get_offset_y(disp));
                break;
            case LV_DISPLAY_ROTATION_90:
                TEST_ASSERT_EQUAL_INT32(ori_offset_y, lv_display_get_offset_x(disp));
                TEST_ASSERT_EQUAL_INT32(ori_offset_x, lv_display_get_offset_y(disp));
                break;
            case LV_DISPLAY_ROTATION_270:
                TEST_ASSERT_EQUAL_INT32(ori_physical_ver_res - ori_offset_y, lv_display_get_offset_x(disp));
                TEST_ASSERT_EQUAL_INT32(ori_physical_hor_res - ori_offset_x, lv_display_get_offset_y(disp));
                break;
            default:
                TEST_FAIL();
                break;
        }
    }
}

void test_display_resolution(void)
{
    lv_display_t * disp_def = lv_display_get_default();
    TEST_ASSERT_NOT_NULL(disp_def);

    lv_display_t * disp = lv_display_create(480, 320);
    TEST_ASSERT_NOT_NULL(disp);
    lv_display_set_default(disp);
    lv_display_set_flush_cb(disp, never_called);
    test_display_resolution_full_rotation(disp, 480, 320, 480, 320, 0, 0);

    lv_display_set_resolution(disp, 320, 480);
    lv_display_set_physical_resolution(disp, 240, 320);
    lv_display_set_offset(disp, 10, 20);
    test_display_resolution_full_rotation(disp, 320, 480, 240, 320, 10, 20);

    /* Set same resolution again */
    lv_display_set_resolution(disp, 320, 480);
    test_display_resolution_full_rotation(disp, 320, 480, 240, 320, 10, 20);

    /* Test default display */
    lv_display_set_resolution(NULL, 32, 48);
    lv_display_set_physical_resolution(NULL, 24, 32);
    lv_display_set_offset(NULL, 1, 2);
    test_display_resolution_full_rotation(disp, 32, 48, 24, 32, 1, 2);

    /* Test NULL default display, should not affect the display */
    lv_display_set_default(NULL);
    lv_display_set_resolution(NULL, 2, 3);
    lv_display_set_physical_resolution(NULL, 4, 5);
    lv_display_set_offset(NULL, 6, 7);
    test_display_resolution_full_rotation(disp, 32, 48, 24, 32, 1, 2);
    test_display_resolution_full_rotation(NULL, 0, 0, 0, 0, 0, 0);

    /* Restore default display */
    lv_display_set_default(disp_def);
    lv_display_delete(disp);
}

void test_display_dpi_tile_cnt_antialiasing(void)
{
    lv_display_t * disp_def = lv_display_get_default();
    TEST_ASSERT_NOT_NULL(disp_def);

    lv_display_t * disp = lv_display_create(480, 320);
    TEST_ASSERT_NOT_NULL(disp);
    lv_display_set_default(disp);
    lv_display_set_flush_cb(disp, never_called);

    /* Verify default values */
    TEST_ASSERT_EQUAL_INT32(LV_DPI_DEF, lv_display_get_dpi(disp));
#if defined(LV_DRAW_SW_DRAW_UNIT_CNT) && (LV_DRAW_SW_DRAW_UNIT_CNT != 0)
    TEST_ASSERT_EQUAL_INT32(LV_DRAW_SW_DRAW_UNIT_CNT, lv_display_get_tile_cnt(disp));
#else
    TEST_ASSERT_EQUAL_INT32(1, lv_display_get_tile_cnt(disp));
#endif
    TEST_ASSERT_EQUAL(LV_COLOR_DEPTH > 8 ? true : false, lv_display_get_antialiasing(disp));

    lv_display_set_dpi(disp, 80);
    TEST_ASSERT_EQUAL_INT32(80, lv_display_get_dpi(disp));
    TEST_ASSERT_EQUAL_INT32(TEST_DPX_CALC(80, 0), lv_dpx(0));
    TEST_ASSERT_EQUAL_INT32(TEST_DPX_CALC(80, 10), lv_dpx(10));
    TEST_ASSERT_EQUAL_INT32(TEST_DPX_CALC(80, 100), lv_display_dpx(disp, 100));

    lv_display_set_antialiasing(disp, false);
    TEST_ASSERT_FALSE(lv_display_get_antialiasing(disp));
    lv_display_set_antialiasing(disp, true);
    TEST_ASSERT_TRUE(lv_display_get_antialiasing(disp));
    lv_display_set_tile_cnt(disp, 10);
    TEST_ASSERT_EQUAL_INT32(10, lv_display_get_tile_cnt(disp));

    /* Test default display, should same affect the display */
    lv_display_set_dpi(NULL, 160);
    TEST_ASSERT_EQUAL_INT32(160, lv_display_get_dpi(NULL));
    lv_display_set_antialiasing(NULL, false);
    TEST_ASSERT_FALSE(lv_display_get_antialiasing(NULL));
    lv_display_set_antialiasing(NULL, true);
    TEST_ASSERT_TRUE(lv_display_get_antialiasing(NULL));
    lv_display_set_tile_cnt(NULL, 20);
    TEST_ASSERT_EQUAL_INT32(20, lv_display_get_tile_cnt(NULL));

    /* Test NULL default display, should not affect the display */
    lv_display_set_default(NULL);
    lv_display_set_dpi(NULL, 200);
    TEST_ASSERT_EQUAL_INT32(LV_DPI_DEF, lv_display_get_dpi(NULL));
    lv_display_set_antialiasing(NULL, false);
    TEST_ASSERT_FALSE(lv_display_get_antialiasing(NULL));
    lv_display_set_antialiasing(NULL, true);
    TEST_ASSERT_FALSE(lv_display_get_antialiasing(NULL));
    lv_display_set_tile_cnt(NULL, 20);
    TEST_ASSERT_EQUAL_INT32(0, lv_display_get_tile_cnt(NULL));

    /* Restore default display */
    lv_display_set_default(disp_def);
    lv_display_delete(disp);
}

void test_display_delete_refr_timer(void)
{
    lv_display_t * disp = lv_display_create(480, 320);
    TEST_ASSERT_NOT_NULL(disp);
    lv_display_set_flush_cb(disp, never_called);

    TEST_ASSERT_NOT_NULL(lv_display_get_refr_timer(disp));
    lv_display_delete_refr_timer(disp);
    TEST_ASSERT_NULL(lv_display_get_refr_timer(disp));

    lv_display_delete(disp);
}

void test_display_buffers_with_stride(void)
{
    lv_display_t * disp = lv_display_create(32, 64);
    TEST_ASSERT_NOT_NULL(disp);

    lv_draw_buf_t * buf1 = lv_draw_buf_create(32, 64, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(buf1);
    lv_draw_buf_t * buf2 = lv_draw_buf_dup(buf1);
    TEST_ASSERT_NOT_NULL(buf2);

    lv_display_set_buffers_with_stride(disp, buf1->data, buf2->data, buf1->data_size, LV_STRIDE_AUTO,
                                       LV_DISPLAY_RENDER_MODE_PARTIAL);
    TEST_ASSERT_EQUAL_PTR(buf1->data, disp->buf_1->data);
    TEST_ASSERT_EQUAL_PTR(buf2->data, disp->buf_2->data);

    lv_display_set_buffers_with_stride(disp, buf1->data, buf2->data, buf1->data_size, 32 * sizeof(uint32_t),
                                       LV_DISPLAY_RENDER_MODE_PARTIAL);
    TEST_ASSERT_EQUAL_PTR(buf1->data, disp->buf_1->data);
    TEST_ASSERT_EQUAL_PTR(buf2->data, disp->buf_2->data);

    lv_display_set_buffers_with_stride(disp, buf1->data, buf2->data, buf1->data_size, 32 * sizeof(uint32_t),
                                       LV_DISPLAY_RENDER_MODE_DIRECT);
    TEST_ASSERT_EQUAL_PTR(buf1->data, disp->buf_1->data);
    TEST_ASSERT_EQUAL_PTR(buf2->data, disp->buf_2->data);

    lv_draw_buf_destroy(buf1);
    lv_draw_buf_destroy(buf2);
    lv_display_delete(disp);
}

void test_display_layers(void)
{
    lv_obj_t * parents[] = {
        lv_layer_bottom(), /* Bottom layer should be covered by screen active */
        lv_screen_active(),
        lv_layer_top(),
        lv_layer_sys(),
    };

    static const lv_palette_t paletts[] = {
        LV_PALETTE_RED,
        LV_PALETTE_GREEN,
        LV_PALETTE_BLUE,
        LV_PALETTE_YELLOW,
    };

    static const char * layer_names[] = {
        "Layer Bottom",
        "Screen Active",
        "Layer Top",
        "Layer Sys",
    };

    for(size_t i = 0; i < sizeof(parents) / sizeof(parents[0]); i++) {
        lv_obj_t * obj = lv_obj_create(parents[i]);
        lv_obj_align(obj, LV_ALIGN_CENTER, 20 * i, 20 * i);
        lv_obj_set_style_opa(obj, LV_OPA_50, LV_PART_MAIN);
        lv_obj_set_style_bg_color(obj, lv_palette_main(paletts[i]), LV_PART_MAIN);

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "%s", layer_names[i]);
        lv_obj_center(label);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("display_layers.png");

    lv_display_t * disp_def = lv_display_get_default();
    TEST_ASSERT_NOT_NULL(disp_def);

    TEST_ASSERT_NOT_NULL(lv_display_get_screen_active(NULL));
    TEST_ASSERT_NULL(lv_display_get_screen_prev(NULL));
    TEST_ASSERT_NOT_NULL(lv_display_get_layer_top(NULL));
    TEST_ASSERT_NOT_NULL(lv_display_get_layer_sys(NULL));
    TEST_ASSERT_NOT_NULL(lv_display_get_layer_bottom(NULL));

    lv_display_set_default(NULL);
    TEST_ASSERT_NULL(lv_display_get_screen_active(NULL));
    TEST_ASSERT_NULL(lv_display_get_screen_prev(NULL));
    TEST_ASSERT_NULL(lv_display_get_layer_top(NULL));
    TEST_ASSERT_NULL(lv_display_get_layer_sys(NULL));
    TEST_ASSERT_NULL(lv_display_get_layer_bottom(NULL));

    lv_display_set_default(disp_def);
}

void test_display_active_time(void)
{
    lv_display_t * disp_def = lv_display_get_default();

    lv_display_trigger_activity(disp_def);
    lv_tick_inc(1000);
    TEST_ASSERT_EQUAL_UINT32(1000, lv_display_get_inactive_time(disp_def));

    lv_display_trigger_activity(NULL);
    lv_tick_inc(1000);
    TEST_ASSERT_EQUAL_UINT32(1000, lv_display_get_inactive_time(NULL));

    /* Test NULL default display, should not affect the display */
    lv_display_set_default(NULL);
    lv_display_trigger_activity(NULL);
    lv_tick_inc(1000);
    TEST_ASSERT_EQUAL_UINT32(2000, lv_display_get_inactive_time(NULL));

    lv_display_set_default(disp_def);
}

void test_display_rotate_point(void)
{
    lv_display_t * disp = lv_display_create(480, 320);
    TEST_ASSERT_NOT_NULL(disp);
    lv_display_set_flush_cb(disp, never_called);

    lv_point_t point;

    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);
    point.x = 10;
    point.y = 20;
    lv_display_rotate_point(disp, &point);
    TEST_ASSERT_EQUAL_INT32(10, point.x);
    TEST_ASSERT_EQUAL_INT32(20, point.y);

    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);
    point.x = 10;
    point.y = 20;
    lv_display_rotate_point(disp, &point);
    TEST_ASSERT_EQUAL_INT32(320 - 20 - 1, point.x);
    TEST_ASSERT_EQUAL_INT32(10, point.y);

    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_180);
    point.x = 10;
    point.y = 20;
    lv_display_rotate_point(disp, &point);
    TEST_ASSERT_EQUAL_INT32(480 - 10 - 1, point.x);
    TEST_ASSERT_EQUAL_INT32(320 - 20 - 1, point.y);

    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);
    point.x = 10;
    point.y = 20;
    lv_display_rotate_point(disp, &point);
    TEST_ASSERT_EQUAL_INT32(20, point.x);
    TEST_ASSERT_EQUAL_INT32(480 - 10 - 1, point.y);

    lv_display_delete(disp);
}

#endif
