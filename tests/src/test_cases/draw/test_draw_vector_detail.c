#if LV_BUILD_TEST
#include "../lvgl.h"
#include "src/misc/cache/instance/lv_image_cache.h"
#include "unity/unity.h"
#include <float.h>

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

/*
 * Regression test: use-after-free in the vg_lite gradient cache.
 *
 * If grad_compare_cb() compares coordinates with an approximate float compare
 * (fabsf(a-b) < FLT_EPSILON), the comparator is not a strict weak ordering:
 * for x1 spaced by < FLT_EPSILON it falls through to y1, so three near-equal
 * x1 with inverted y1 order violate transitivity (a==b, b==c, but a!=c). That
 * corrupts the rb-tree ordering, a lookup can miss a node that is still linked,
 * and the eviction path then frees the entry while leaving the node linked ->
 * a later eviction touches freed memory.
 *
 * The coordinates below are a deterministic minimal trigger: x1 increases by
 * ~0.4*FLT_EPSILON per step (adjacent pairs compare equal, spaced-apart pairs
 * do not) while y1 zig-zags so near-equal neighbours invert. Drawing far more
 * than LV_VG_LITE_GRAD_CACHE_CNT (32) distinct gradients forces eviction
 * repeatedly. With exact comparison (and node-identity unlink) it passes.
 */
void test_draw_grad_cache_intransitive_uaf(void)
{
    /* Build the intransitive coordinate cluster:
     *  - x1 starts sub-unit (~0.0086) and increases by ~0.4*FLT_EPSILON per
     *    step, so adjacent x1 compare "equal" under an epsilon compare while
     *    x1 two steps apart do not;
     *  - y1 zig-zags with growing amplitude so near-equal x1 neighbours have
     *    inverted y1 order, which is what breaks transitivity.
     * Sub-unit x1 is essential: only there is 1 ULP smaller than FLT_EPSILON,
     * so an epsilon compare actually merges distinct values. */
    const int poison_cnt = 12;
    float poison[12][2];
    for(int i = 0; i < poison_cnt; i++) {
        poison[i][0] = 0.0086612f + (float)i * (0.4f * FLT_EPSILON);
        poison[i][1] = -11.7f + ((i & 1) ? 1e-4f : -1e-4f) * (float)(i + 1);
    }

#if LV_USE_DRAW_VG_LITE && LV_USE_VG_LITE_THORVG
    /* On real hardware the linear gradient EXT path is used (GRAD_TYPE_LINEAR_EXT),
     * which is the only type whose comparator compares x1/y1/x2/y2 via math_equal.
     * The ThorVG sim reports this feature as disabled by default, collapsing to
     * GRAD_TYPE_LINEAR (coords not compared). Force it on so the test exercises
     * the same comparison path that faulted on device. */
    vg_lite_uint32_t saved_lin_ext = vg_lite_query_feature(gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT);
    vg_lite_enable_feature(gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT, 1);
#endif

    lv_vector_dsc_t * ctx = lv_vector_dsc_create(&layer);
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    lv_gradient_stop_t stops[2];
    lv_memzero(stops, sizeof(stops));
    stops[0].color = lv_color_hex(0xff0000);
    stops[0].opa = LV_OPA_COVER;
    stops[0].frac = 0;
    stops[1].color = lv_color_hex(0x0000ff);
    stops[1].opa = LV_OPA_COVER;
    stops[1].frac = 255;

    lv_area_t rect = {10, 10, 60, 120};

    /*
     * Several rounds: each round draws the intransitive cluster plus enough
     * distinct "filler" gradients to overflow the 32-entry grad cache and force
     * eviction. Repeating re-inserts the poison keys so a dangling node created
     * in one round is selected as a victim in a later round.
     */
    for(int round = 0; round < 6; round++) {
        /* 1) the intransitive cluster */
        for(int i = 0; i < poison_cnt; i++) {
            lv_vector_path_clear(path);
            lv_vector_path_append_rect(path, &rect, 0, 0);
            lv_vector_dsc_set_fill_linear_gradient(ctx, poison[i][0], poison[i][1],
                                                   0.0f, -0.25f);
            lv_vector_dsc_set_fill_gradient_color_stops(ctx, stops, 2);
            lv_vector_dsc_set_fill_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_PAD);
            lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
            lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
            lv_vector_dsc_add_path(ctx, path);
            draw_vector(ctx);
        }

        /* 2) filler gradients to overflow the cache (> 32 distinct keys) */
        for(int j = 0; j < 40; j++) {
            lv_vector_path_clear(path);
            lv_vector_path_append_rect(path, &rect, 0, 0);
            lv_vector_dsc_set_fill_linear_gradient(ctx, (float)(j + 1), (float)(2 * j + 3),
                                                   (float)(j + 5), (float)(3 * j + 7));
            lv_vector_dsc_set_fill_gradient_color_stops(ctx, stops, 2);
            lv_vector_dsc_set_fill_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_PAD);
            lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
            lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_TRANSP);
            lv_vector_dsc_add_path(ctx, path);
            draw_vector(ctx);
        }
    }

    /* Reaching here without an abort/ASAN report means the cache no longer
     * leaves dangling nodes on comparator-induced find failures. */

    lv_vector_path_delete(path);
    lv_vector_dsc_delete(ctx);

#if LV_USE_DRAW_VG_LITE && LV_USE_VG_LITE_THORVG
    vg_lite_enable_feature(gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT, saved_lin_ext);
#endif
}

#endif
