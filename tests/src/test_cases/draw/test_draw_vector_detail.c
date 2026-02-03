#if LV_BUILD_TEST
#include "../lvgl.h"
#include "src/misc/cache/instance/lv_image_cache.h"
#include "unity/unity.h"

static lv_layer_t layer;
static lv_obj_t * canvas;
static lv_draw_buf_t * canvas_buf;

void setUp(void)
{
    canvas = lv_canvas_create(lv_screen_active());
    canvas_buf = lv_draw_buf_create(480, 480, LV_COLOR_FORMAT_ARGB8888, 0);
    TEST_ASSERT_NOT_NULL(canvas_buf);
    lv_canvas_set_draw_buf(canvas, canvas_buf);
    lv_canvas_fill_bg(canvas, lv_color_make(0xff, 0xff, 0xff), 255);
    lv_canvas_init_layer(canvas, &layer);
}

void tearDown(void)
{
    if(canvas_buf) {
        lv_image_cache_drop(canvas_buf);
        lv_draw_buf_destroy(canvas_buf);
        canvas_buf = NULL;
    }
    if(canvas) {
        lv_obj_delete(canvas);
        canvas = NULL;
    }
}

#define SNAPSHOT_NAME(n) (#n)

#ifndef NON_AMD64_BUILD
    #define EXT_NAME ".lp64.png"
#else
    #define EXT_NAME ".lp32.png"
#endif

static void draw_snapshot(const char * name)
{
    char fn_buf[64];
    lv_snprintf(fn_buf, sizeof(fn_buf), "draw/vector_draw_%s" EXT_NAME, name);
    TEST_ASSERT_EQUAL_SCREENSHOT(fn_buf);
}

static void draw_vector(lv_draw_vector_dsc_t * ctx)
{
    lv_image_cache_drop(canvas_buf);
    lv_canvas_set_draw_buf(canvas, canvas_buf);
    lv_canvas_fill_bg(canvas, lv_color_make(0xff, 0xff, 0xff), 255);
    lv_draw_vector(ctx);
    lv_canvas_finish_layer(canvas, &layer);
}

void test_draw_blend_modes_comparison(void)
{
    lv_draw_vector_dsc_t * ctx = lv_draw_vector_dsc_create(&layer);
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    /* Clear background */
    lv_area_t rect = {0, 0, 640, 480};
    lv_draw_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_draw_vector_dsc_clear_area(ctx, &rect);

    /* Draw 14 blend mode examples in 4x4 grid within {0,0,640,480} area */
    const int unit_width = 70;
    const int unit_height = 70;
    const int cols = 4;
    const int rows = 4;

    /* Calculate spacing between units */
    const int h_space = (640 - cols * unit_width * 3 / 2) / (cols + 1);
    const int v_space = (480 - rows * unit_height * 3 / 2) / (rows + 1);

    /* Blend modes for each unit */
    const lv_vector_blend_t blend_modes[] = {
        LV_VECTOR_BLEND_SRC_OVER,
        LV_VECTOR_BLEND_SRC_IN,
        LV_VECTOR_BLEND_DST_OVER,
        LV_VECTOR_BLEND_DST_IN,
        LV_VECTOR_BLEND_SCREEN,
        LV_VECTOR_BLEND_MULTIPLY,
        LV_VECTOR_BLEND_NONE,
        LV_VECTOR_BLEND_ADDITIVE,
        LV_VECTOR_BLEND_SUBTRACTIVE,
    };

    /* Draw blend mode examples */
    for(size_t i = 0; i < sizeof(blend_modes) / sizeof(blend_modes[0]); i++) {
        /* Calculate unit position */
        int col = i % cols;
        int row = i / rows;
        int x = h_space + col * (unit_width * 3 / 2 + h_space);
        int y = v_space + row * (unit_height * 3 / 2 + v_space);

        /* Draw blue rectangle (120x120 with 1/4 overlap) */
        lv_vector_path_clear(path);
        lv_area_t blue_rect = {
            x + unit_width / 2,
            y + unit_width / 2,
            x + unit_width * 3 / 2,
            y + unit_width * 3 / 2
        };
        lv_vector_path_append_rect(path, &blue_rect, 0, 0);
        lv_draw_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xff)); /* Blue */
        lv_draw_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
        lv_draw_vector_dsc_set_blend_mode(ctx, LV_VECTOR_BLEND_SRC_OVER);
        lv_draw_vector_dsc_add_path(ctx, path);

        /* Draw green rectangle (full size) */
        lv_vector_path_clear(path);
        lv_area_t green_rect = { x, y, x + unit_width, y + unit_height };
        lv_vector_path_append_rect(path, &green_rect, 0, 0);
        lv_draw_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0xff, 0x00)); /* Green */
        lv_draw_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
        lv_draw_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
        lv_draw_vector_dsc_set_blend_mode(ctx, LV_VECTOR_BLEND_SRC_OVER);
        lv_draw_vector_dsc_add_path(ctx, path);

        lv_vector_path_clear(path);
        lv_vector_path_append_rect(path, &blue_rect, 0, 0);
        lv_draw_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xff)); /* Blue */
        lv_draw_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
        lv_draw_vector_dsc_set_blend_mode(ctx, blend_modes[i]);
        lv_draw_vector_dsc_add_path(ctx, path);
    }

    draw_vector(ctx);
    draw_snapshot(SNAPSHOT_NAME(blend_modes_comparison));

    /* Cleanup */
    lv_vector_path_delete(path);
    lv_draw_vector_dsc_delete(ctx);
}

void test_draw_copy_path(void)
{
    lv_draw_vector_dsc_t * ctx = lv_draw_vector_dsc_create(&layer);
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    lv_fpoint_t pts[] = { { 50, 50 }, { 200, 200 }, { 50, 200 } };
    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_line_to(path, &pts[1]);
    lv_vector_path_line_to(path, &pts[2]);
    lv_vector_path_close(path);

    lv_vector_path_t * path2 = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_vector_path_copy(path2, path);

    lv_draw_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xFF));
    lv_draw_vector_dsc_add_path(ctx, path);

    lv_draw_vector_dsc_set_fill_color(ctx, lv_color_make(0xFF, 0x00, 0x00));
    lv_draw_vector_dsc_add_path(ctx, path2);

    lv_vector_path_delete(path2);

    lv_draw_vector_dsc_set_stroke_opa(ctx, LV_OPA_0);
    lv_draw_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);

    draw_vector(ctx);
    draw_snapshot(SNAPSHOT_NAME(copy_path));

    /* Cleanup */
    lv_vector_path_delete(path);
    lv_draw_vector_dsc_delete(ctx);
}

void test_draw_rect_path(void)
{
    lv_draw_vector_dsc_t * ctx = lv_draw_vector_dsc_create(&layer);
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    lv_area_t rect = { 50, 50, 100, 200 };
    lv_vector_path_append_rect(path, &rect, 0, 0);
    lv_draw_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x1e, 0x8F));
    lv_draw_vector_dsc_add_path(ctx, path);

    lv_draw_vector_dsc_set_stroke_opa(ctx, LV_OPA_0);
    lv_draw_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);

    draw_vector(ctx);
    draw_snapshot(SNAPSHOT_NAME(rect_path));

    /* Cleanup */
    lv_vector_path_delete(path);
    lv_draw_vector_dsc_delete(ctx);
}

void test_draw_append_path(void)
{
    lv_draw_vector_dsc_t * ctx = lv_draw_vector_dsc_create(&layer);
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    lv_fpoint_t pts[] = { { 50, 50 }, { 200, 200 }, { 50, 200 }, { 200, 50 } };
    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_line_to(path, &pts[1]);
    lv_vector_path_line_to(path, &pts[2]);
    lv_vector_path_close(path);

    lv_vector_path_t * path2 = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_line_to(path, &pts[3]);
    lv_vector_path_close(path2);

    lv_vector_path_append_path(path, path2);

    lv_draw_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_draw_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_draw_vector_dsc_set_stroke_width(ctx, 5.0f);

    lv_draw_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xFF));
    lv_draw_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0xFF, 0x00));

    lv_draw_vector_dsc_add_path(ctx, path);

    draw_vector(ctx);
    draw_snapshot(SNAPSHOT_NAME(append_path));

    /* Cleanup */
    lv_vector_path_delete(path);
    lv_vector_path_delete(path2);
    lv_draw_vector_dsc_delete(ctx);
}

void test_draw_arc_path(void)
{
    lv_draw_vector_dsc_t * ctx = lv_draw_vector_dsc_create(&layer);
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    lv_fpoint_t p1 = { 200, 200 }; /* Center */
    lv_draw_vector_dsc_set_stroke_color(ctx, lv_color_make(0xFF, 0x0, 0x0));
    lv_draw_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_draw_vector_dsc_set_fill_opa(ctx, LV_OPA_0);
    lv_draw_vector_dsc_set_stroke_width(ctx, 5.0f);
    lv_vector_path_append_arc(path, &p1, 100, -90, 90, false);
    lv_draw_vector_dsc_add_path(ctx, path);

    lv_fpoint_t p2 = { 250, 250 };
    lv_vector_path_append_circle(path, &p2, 100, 60);
    lv_draw_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xFF));
    lv_draw_vector_dsc_add_path(ctx, path);

    lv_draw_vector_dsc_set_stroke_opa(ctx, LV_OPA_0);
    lv_draw_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);

    draw_vector(ctx);
    draw_snapshot(SNAPSHOT_NAME(arc_path));

    /* Cleanup */
    lv_vector_path_delete(path);
    lv_draw_vector_dsc_delete(ctx);
}

#endif
