#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "lv_test_helpers.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

static void draw_shapes(lv_layer_t * layer)
{
    lv_vector_dsc_t * ctx = lv_vector_dsc_create(layer);

    lv_area_t rect = {0, 0, 640, 480};
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    lv_area_t rect1 = {50, 50, 150, 150};
    lv_vector_dsc_identity(ctx);
    lv_vector_path_clear(path);
    lv_vector_path_append_rect(path, &rect1, 0, 0);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0x00, 0x00));
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_path_clear(path);
    lv_vector_path_append_rect(path, &rect1, 20, 20);
    lv_vector_dsc_translate(ctx, 150, 0);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x00, 0xff));
    lv_vector_dsc_add_path(ctx, path);

    lv_fpoint_t pc = {100, 100};
    lv_vector_path_clear(path);
    lv_vector_path_append_circle(path, &pc, 50, 50);
    lv_vector_dsc_translate(ctx, 150, 0);
    lv_vector_dsc_set_fill_color32(ctx, lv_color_to_32(lv_color_make(0x00, 0xff, 0x00), 0x80));
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_dsc_identity(ctx);
    lv_vector_dsc_translate(ctx, 0, 150);

    lv_gradient_stop_t stops[2];
    lv_memzero(stops, sizeof(stops));
    stops[0].color = lv_color_hex(0xffffff);
    stops[0].opa = LV_OPA_COVER;
    stops[0].frac = 0;
    stops[1].color = lv_color_hex(0x000000);
    stops[1].opa = LV_OPA_COVER;
    stops[1].frac = 255;

    lv_matrix_t mt;
    lv_matrix_identity(&mt);
    lv_vector_dsc_set_fill_transform(ctx, &mt);
    lv_vector_dsc_set_fill_radial_gradient(ctx, 100, 100, 50);
    lv_vector_dsc_set_fill_gradient_color_stops(ctx, stops, 2);
    lv_vector_dsc_set_fill_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_PAD);
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_dsc_identity(ctx);
    lv_vector_dsc_translate(ctx, 150, 150);
    lv_draw_image_dsc_t img_dsc;
    lv_draw_image_dsc_init(&img_dsc);
    LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);
    img_dsc.header = test_image_cogwheel_argb8888.header;
    img_dsc.src = &test_image_cogwheel_argb8888;
    lv_vector_dsc_set_fill_image(ctx, &img_dsc);
    lv_matrix_identity(&mt);
    lv_matrix_translate(&mt, 50, 50);
    //lv_matrix_rotate(&mt, 45);
    //lv_matrix_scale(&mt, 1.5f, 1.5f);
    lv_matrix_translate(&mt, -50, -50);
    lv_vector_dsc_set_fill_transform(ctx, &mt);
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_dsc_identity(ctx);
    lv_vector_dsc_translate(ctx, 300, 150);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xff, 0x00, 0x00));
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_path_clear(path);
    lv_vector_path_append_rect(path, &rect1, 20, 20);
    lv_area_t rect2 = {80, 80, 120, 120};
    lv_vector_path_append_rect(path, &rect2, 15, 15);
    lv_vector_dsc_translate(ctx, 50, 0);
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0x00, 0x80, 0xff));
    lv_vector_dsc_set_blend_mode(ctx, LV_VECTOR_BLEND_MULTIPLY);
    lv_vector_dsc_set_fill_rule(ctx, LV_VECTOR_FILL_EVENODD);
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_path_clear(path);
    lv_vector_dsc_identity(ctx);
    lv_vector_dsc_set_blend_mode(ctx, LV_VECTOR_BLEND_SRC_OVER);

    rect = (lv_area_t) {
        500, 50, 550, 100
    };
    lv_vector_dsc_set_fill_color(ctx, lv_color_lighten(lv_color_black(), 50));
    lv_vector_clear_area(ctx, &rect); // clear screen

    lv_fpoint_t p = {500, 100}; /* Center */
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0xff, 0xff));
    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_stroke_width(ctx, 2.0f);
    lv_vector_dsc_set_stroke_dash(ctx, NULL, 0);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_TRANSP);
    lv_vector_path_move_to(path, &p);
    lv_fpoint_t temp = {p.x + 50, p.y};
    lv_vector_path_line_to(path, &temp);
    lv_vector_path_append_arc(path, &p, 50, 0, -90, false);
    lv_vector_path_line_to(path, &p);
    lv_vector_path_close(path);

    lv_vector_dsc_add_path(ctx, path);

    /* Below code has same effect as above one but with solid fill */
    lv_vector_dsc_set_fill_color(ctx, lv_color_make(0xFF, 0x00, 0x00));
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_COVER);
    lv_vector_path_clear(path);
    lv_vector_path_append_arc(path, &p, 50, 45, 45, true);
    lv_vector_dsc_add_path(ctx, path); // draw a path

    lv_draw_vector(ctx);
    lv_vector_path_delete(path);
    lv_vector_dsc_delete(ctx);
}

static void draw_lines(lv_layer_t * layer)
{
    lv_vector_dsc_t * ctx = lv_vector_dsc_create(layer);

    lv_area_t rect = {0, 0, 640, 480};
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear_area(ctx, &rect);

    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_0);

    lv_fpoint_t pts1[] = {{50, 50}, {400, 50}};
    lv_vector_path_clear(path);
    lv_vector_path_move_to(path, &pts1[0]);
    lv_vector_path_line_to(path, &pts1[1]);
    lv_vector_dsc_add_path(ctx, path);

    lv_fpoint_t pts2[] = {{50, 80}, {400, 80}};
    lv_vector_path_clear(path);
    lv_vector_path_move_to(path, &pts2[0]);
    lv_vector_path_line_to(path, &pts2[1]);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0xff, 0x00, 0x00));
    lv_vector_dsc_set_stroke_width(ctx, 3.0f);
    lv_vector_dsc_add_path(ctx, path);

    lv_fpoint_t pts3[] = {{50, 120}, {400, 120}};
    lv_vector_path_clear(path);
    lv_vector_path_move_to(path, &pts3[0]);
    lv_vector_path_line_to(path, &pts3[1]);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0xff, 0x00));
    lv_vector_dsc_set_stroke_width(ctx, 5.0f);
    float dashes[] = {10, 15, 20, 12};
    lv_vector_dsc_set_stroke_dash(ctx, dashes, 4);
    lv_vector_dsc_add_path(ctx, path);

    lv_fpoint_t pts4[] = {{100, 150}, {50, 250}, {150, 250}};
    lv_vector_path_clear(path);
    lv_vector_path_move_to(path, &pts4[0]);
    lv_vector_path_line_to(path, &pts4[1]);
    lv_vector_path_line_to(path, &pts4[2]);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0x00, 0x00));
    lv_vector_dsc_set_stroke_width(ctx, 10.0f);
    lv_vector_dsc_set_stroke_dash(ctx, NULL, 0);
    lv_vector_dsc_set_stroke_join(ctx, LV_VECTOR_STROKE_JOIN_MITER);
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_dsc_translate(ctx, 150, 0);
    lv_vector_dsc_set_stroke_join(ctx, LV_VECTOR_STROKE_JOIN_BEVEL);
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_dsc_translate(ctx, 150, 0);
    lv_vector_dsc_set_stroke_join(ctx, LV_VECTOR_STROKE_JOIN_ROUND);
    lv_vector_dsc_add_path(ctx, path);

    lv_fpoint_t pts5[] = {{50, 300}, {150, 300}};
    lv_vector_dsc_identity(ctx);
    lv_vector_path_clear(path);
    lv_vector_path_move_to(path, &pts5[0]);
    lv_vector_path_line_to(path, &pts5[1]);
    lv_vector_dsc_set_stroke_cap(ctx, LV_VECTOR_STROKE_CAP_BUTT);
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_dsc_translate(ctx, 0, 40);
    lv_vector_dsc_set_stroke_cap(ctx, LV_VECTOR_STROKE_CAP_SQUARE);
    lv_vector_dsc_add_path(ctx, path);

    lv_vector_dsc_translate(ctx, 0, 40);
    lv_vector_dsc_set_stroke_cap(ctx, LV_VECTOR_STROKE_CAP_ROUND);
    lv_vector_dsc_add_path(ctx, path);

    lv_area_t rect1 = {250, 300, 350, 400};
    lv_vector_dsc_identity(ctx);
    lv_vector_path_clear(path);
    lv_vector_path_append_rect(path, &rect1, 0, 0);

    lv_gradient_stop_t stops[2];
    lv_memzero(stops, sizeof(stops));
    stops[0].color = lv_color_hex(0xff0000);
    stops[0].opa = LV_OPA_COVER;
    stops[0].frac = 0;
    stops[1].color = lv_color_hex(0x00ff00);
    stops[1].opa = LV_OPA_COVER;
    stops[1].frac = 255;

    lv_matrix_t mt;
    lv_matrix_identity(&mt);
    lv_matrix_rotate(&mt, 5);
    lv_matrix_translate(&mt, 20, 20);
    lv_vector_dsc_set_stroke_transform(ctx, &mt);
    lv_vector_dsc_set_stroke_join(ctx, LV_VECTOR_STROKE_JOIN_MITER);
    lv_vector_dsc_set_stroke_linear_gradient(ctx, 250, 300, 350, 300);
    lv_vector_dsc_set_stroke_gradient_color_stops(ctx, stops, 2);
    lv_vector_dsc_set_stroke_gradient_spread(ctx, LV_VECTOR_GRADIENT_SPREAD_REFLECT);
    lv_vector_dsc_add_path(ctx, path); // draw a path

    lv_draw_vector(ctx);
    lv_vector_path_delete(path);
    lv_vector_dsc_delete(ctx);
}

static void canvas_draw(const char * name, void (*draw_cb)(lv_layer_t *))
{
    LV_UNUSED(name);
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(640, 480, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    lv_draw_buf_clear(draw_buf, NULL);
    lv_canvas_set_draw_buf(canvas, draw_buf);
    lv_canvas_fill_bg(canvas, lv_color_make(0xff, 0xff, 0xff), 255);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    draw_cb(&layer);

    lv_canvas_finish_layer(canvas, &layer);

#ifndef NON_AMD64_BUILD
    char fn_buf[64];
    lv_snprintf(fn_buf, sizeof(fn_buf), "draw/vector_%s.lp64.png", name);
    TEST_ASSERT_EQUAL_SCREENSHOT(fn_buf);
#else
    char fn_buf[64];
    lv_snprintf(fn_buf, sizeof(fn_buf), "draw/vector_%s.lp32.png", name);
    TEST_ASSERT_EQUAL_SCREENSHOT(fn_buf);
#endif

    lv_image_cache_drop(draw_buf);
    lv_draw_buf_destroy(draw_buf);
    lv_obj_delete(canvas);
}

void test_transform(void)
{
    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);
    lv_matrix_translate(&matrix, 100, 100);

    lv_fpoint_t p = {10, 10};
    lv_matrix_transform_point(&matrix, &p);

    TEST_ASSERT_EQUAL_FLOAT(110.0f, p.x);
    TEST_ASSERT_EQUAL_FLOAT(110.0f, p.y);

    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);
    lv_vector_path_move_to(path, &p);

    lv_fpoint_t p2 = {20, 20};
    lv_vector_path_line_to(path, &p2);
    lv_matrix_transform_path(&matrix, path);

    lv_fpoint_t * pt = lv_array_at(&path->points, 0);

    TEST_ASSERT_EQUAL_FLOAT(210.0f, pt[0].x);
    TEST_ASSERT_EQUAL_FLOAT(210.0f, pt[0].y);
    TEST_ASSERT_EQUAL_FLOAT(120.0f, pt[1].x);
    TEST_ASSERT_EQUAL_FLOAT(120.0f, pt[1].y);

    lv_vector_path_delete(path);
}

void test_draw_lines(void)
{
    canvas_draw("draw_lines", draw_lines);
}

void test_draw_shapes(void)
{
    canvas_draw("draw_shapes", draw_shapes);
}

static void event_cb(lv_event_t * e)
{
    lv_layer_t * layer = lv_event_get_layer(e);
    lv_obj_t * obj = lv_event_get_current_target_obj(e);

    lv_vector_dsc_t * dsc = lv_vector_dsc_create(layer);
    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    lv_fpoint_t pts[] = {{10, 10}, {130, 130}, {10, 130}};
    lv_vector_path_move_to(path, &pts[0]);
    lv_vector_path_line_to(path, &pts[1]);
    lv_vector_path_line_to(path, &pts[2]);
    lv_vector_path_close(path);

    lv_vector_dsc_translate(dsc, obj->coords.x1, obj->coords.y1);
    lv_vector_dsc_set_fill_color(dsc, lv_color_make(0x00, 0x80, 0xff));
    lv_vector_dsc_add_path(dsc, path);

    lv_draw_vector(dsc);
    lv_vector_path_delete(path);
    lv_vector_dsc_delete(dsc);
}

void test_draw_during_rendering(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_center(obj);
    lv_obj_add_event_cb(obj, event_cb, LV_EVENT_DRAW_MAIN, NULL);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/vector_draw_during_rendering.png");
}

#endif
