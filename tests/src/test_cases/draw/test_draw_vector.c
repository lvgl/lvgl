#if LV_BUILD_TEST
#include "../lvgl.h"
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

static void draw_lines(lv_layer_t* layer)
{
    lv_vector_dsc_t * ctx = lv_vector_dsc_create(layer);

    lv_area_t rect = {0, 0, 640, 480};
    lv_vector_dsc_set_fill_color(ctx, lv_color_white());
    lv_vector_clear(ctx, &rect);

    lv_vector_path_t * path = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_MEDIUM);

    lv_vector_dsc_set_stroke_opa(ctx, LV_OPA_COVER);
    lv_vector_dsc_set_fill_opa(ctx, LV_OPA_0);

    lv_fpoint_t pts1[] = {{50, 50}, {400, 50}};
    lv_vector_path_clear(path);
    lv_vector_path_move_to(path, &pts1[0]);
    lv_vector_path_line_to(path, &pts1[1]);
    lv_vector_add(ctx, path);

    lv_fpoint_t pts2[] = {{50, 80}, {400, 80}};
    lv_vector_path_clear(path);
    lv_vector_path_move_to(path, &pts2[0]);
    lv_vector_path_line_to(path, &pts2[1]);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0xff, 0x00, 0x00));
    lv_vector_dsc_set_stroke_width(ctx, 3.0f);
    lv_vector_add(ctx, path);

    lv_fpoint_t pts3[] = {{50, 120}, {400, 120}};
    lv_vector_path_clear(path);
    lv_vector_path_move_to(path, &pts3[0]);
    lv_vector_path_line_to(path, &pts3[1]);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0xff, 0x00));
    lv_vector_dsc_set_stroke_width(ctx, 5.0f);
    float dashs[] = {10, 15, 20, 12};
    lv_vector_dsc_set_stroke_dash(ctx, dashs, 4);
    lv_vector_add(ctx, path);

    lv_fpoint_t pts4[] = {{100, 150}, {50, 250}, {150, 250}};
    lv_vector_path_clear(path);
    lv_vector_path_move_to(path, &pts4[0]);
    lv_vector_path_line_to(path, &pts4[1]);
    lv_vector_path_line_to(path, &pts4[2]);
    lv_vector_dsc_set_stroke_color(ctx, lv_color_make(0x00, 0x00, 0x00));
    lv_vector_dsc_set_stroke_width(ctx, 10.0f);
    lv_vector_dsc_set_stroke_dash(ctx, NULL, 0);
    lv_vector_dsc_set_stroke_join(ctx, LV_VECTOR_STROKE_JOIN_MITER);
    lv_vector_add(ctx, path);

    lv_vector_dsc_translate(ctx, 150, 0);
    lv_vector_dsc_set_stroke_join(ctx, LV_VECTOR_STROKE_JOIN_BEVEL);
    lv_vector_add(ctx, path);

    lv_vector_dsc_translate(ctx, 150, 0);
    lv_vector_dsc_set_stroke_join(ctx, LV_VECTOR_STROKE_JOIN_ROUND);
    lv_vector_add(ctx, path);

    lv_fpoint_t pts5[] = {{50, 300}, {150, 300}};
    lv_vector_dsc_identity(ctx);
    lv_vector_path_clear(path);
    lv_vector_path_move_to(path, &pts5[0]);
    lv_vector_path_line_to(path, &pts5[1]);
    lv_vector_dsc_set_stroke_cap(ctx, LV_VECTOR_STROKE_CAP_BUTT);
    lv_vector_add(ctx, path);

    lv_vector_dsc_translate(ctx, 0, 40);
    lv_vector_dsc_set_stroke_cap(ctx, LV_VECTOR_STROKE_CAP_SQUARE);
    lv_vector_add(ctx, path);

    lv_vector_dsc_translate(ctx, 0, 40);
    lv_vector_dsc_set_stroke_cap(ctx, LV_VECTOR_STROKE_CAP_ROUND);
    lv_vector_add(ctx, path);

    lv_vector_draw(ctx);
    lv_vector_path_destroy(path);
    lv_vector_dsc_destroy(ctx);
}

static void canvas_draw(const char* name, void (*draw_cb)(lv_layer_t*))
{
    static uint8_t canvas_buf[CANVAS_WIDTH_TO_STRIDE(640, 4) * 480 + LV_DRAW_BUF_ALIGN];
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_buffer(canvas, canvas_buf, 640, 480, LV_COLOR_FORMAT_ARGB8888);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    draw_cb(&layer);

    lv_canvas_finish_layer(canvas, &layer);

    char fn_buf[64];
    lv_snprintf(fn_buf, sizeof(fn_buf), "draw/vector_%s.png", name);
    TEST_ASSERT_EQUAL_SCREENSHOT(fn_buf);
}

void test_draw_lines(void)
{
    canvas_draw("draw_lines", draw_lines); 
}
#endif
