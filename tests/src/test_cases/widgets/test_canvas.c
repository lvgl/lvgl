#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_obj_t * g_screen_active;

void setUp(void)
{
    g_screen_active = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(g_screen_active);
}

static void draw_event_cb(lv_event_t * e)
{
    int * draw_counter = lv_event_get_user_data(e);
    (*draw_counter)++;
}

static void canvas_draw_buf_reshape(lv_draw_buf_t * draw_buf)
{
#if LV_USE_DRAW_VG_LITE
    /* VG-Lite requires automatic stride calculation */
    lv_draw_buf_t * buf = lv_draw_buf_reshape(draw_buf,
                                              draw_buf->header.cf,
                                              draw_buf->header.w,
                                              draw_buf->header.h,
                                              LV_STRIDE_AUTO);
    TEST_ASSERT(buf == draw_buf);
#else
    LV_UNUSED(draw_buf);
#endif
}

void test_canvas_functions_invalidate(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);
    int draw_counter = 0;
    lv_obj_add_event_cb(canvas, draw_event_cb, LV_EVENT_DRAW_MAIN, &draw_counter);
    lv_refr_now(NULL);
    TEST_ASSERT(draw_counter == 0);

    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 100, 100, LV_COLOR_FORMAT_NATIVE);

    /* test uninitialized draw buffer, it should fail.*/
    lv_canvas_set_draw_buf(canvas, &draw_buf);
    TEST_ASSERT_NULL(lv_canvas_get_draw_buf(canvas));
    TEST_ASSERT_NULL(lv_canvas_get_image(canvas));
    TEST_ASSERT_NULL(lv_canvas_get_buf(canvas));

    LV_DRAW_BUF_INIT_STATIC(draw_buf);
    canvas_draw_buf_reshape(&draw_buf);

    lv_canvas_set_draw_buf(canvas, &draw_buf);
    TEST_ASSERT_EQUAL_PTR(lv_canvas_get_draw_buf(canvas), &draw_buf);
    TEST_ASSERT_EQUAL_PTR(lv_canvas_get_image(canvas), &draw_buf);
    TEST_ASSERT_EQUAL_PTR(lv_canvas_get_buf(canvas), draw_buf.unaligned_data);

    lv_refr_now(NULL);
    TEST_ASSERT(draw_counter == 1);

    lv_canvas_set_px(canvas, 0, 0, lv_color_black(), LV_OPA_COVER);
    lv_refr_now(NULL);
    TEST_ASSERT(draw_counter == 2);

    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);
    lv_refr_now(NULL);
    TEST_ASSERT(draw_counter == 3);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.p1.x = 10;
    line_dsc.p1.y = 10;
    line_dsc.p2.x = 20;
    line_dsc.p2.y = 20;
    line_dsc.width = 5;
    lv_draw_line(&layer, &line_dsc);
    lv_canvas_finish_layer(canvas, &layer);
    lv_refr_now(NULL);
    TEST_ASSERT(draw_counter == 4);

    lv_refr_now(NULL);
    TEST_ASSERT(draw_counter == 4);
}

void test_canvas_fill_and_set_px(void)
{
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_obj_center(canvas);

    LV_DRAW_BUF_DEFINE_STATIC(buf_i1, 10, 10, LV_COLOR_FORMAT_I1);
    LV_DRAW_BUF_INIT_STATIC(buf_i1);
    canvas_draw_buf_reshape(&buf_i1);
    lv_canvas_set_draw_buf(canvas, &buf_i1);
    lv_canvas_set_palette(canvas, 0, lv_color32_make(0x00, 0xff, 0x00, 0xff));
    lv_canvas_set_palette(canvas, 1, lv_color32_make(0x00, 0x00, 0xff, 0xff));
    lv_canvas_fill_bg(canvas, lv_color_hex(0), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(1), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_i2, 10, 10, LV_COLOR_FORMAT_I2);
    LV_DRAW_BUF_INIT_STATIC(buf_i2);
    canvas_draw_buf_reshape(&buf_i2);
    lv_canvas_set_draw_buf(canvas, &buf_i2);
    lv_canvas_set_palette(canvas, 0, lv_color32_make(0x00, 0xff, 0x00, 0xff));
    lv_canvas_set_palette(canvas, 3, lv_color32_make(0x00, 0x00, 0xff, 0xff));
    lv_canvas_fill_bg(canvas, lv_color_hex(0), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(3), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_i4, 10, 10, LV_COLOR_FORMAT_I4);
    LV_DRAW_BUF_INIT_STATIC(buf_i4);
    canvas_draw_buf_reshape(&buf_i4);
    lv_canvas_set_draw_buf(canvas, &buf_i4);
    lv_canvas_set_palette(canvas, 0, lv_color32_make(0x00, 0xff, 0x00, 0xff));
    lv_canvas_set_palette(canvas, 15, lv_color32_make(0x00, 0x00, 0xff, 0xff));
    lv_canvas_fill_bg(canvas, lv_color_hex(0), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(15), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_i8, 10, 10, LV_COLOR_FORMAT_I8);
    LV_DRAW_BUF_INIT_STATIC(buf_i8);
    canvas_draw_buf_reshape(&buf_i8);
    lv_canvas_set_draw_buf(canvas, &buf_i8);
    lv_canvas_set_palette(canvas, 0, lv_color32_make(0x00, 0xff, 0x00, 0xff));
    lv_canvas_set_palette(canvas, 255, lv_color32_make(0x00, 0x00, 0xff, 0xff));
    lv_canvas_fill_bg(canvas, lv_color_hex(0), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(255), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_rgb888, 10, 10, LV_COLOR_FORMAT_RGB888);
    LV_DRAW_BUF_INIT_STATIC(buf_rgb888);
    canvas_draw_buf_reshape(&buf_rgb888);
    lv_canvas_set_draw_buf(canvas, &buf_rgb888);
    lv_canvas_fill_bg(canvas, lv_color_hex(0x00ff00), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(0x0000ff), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_rgb565, 10, 10, LV_COLOR_FORMAT_RGB565);
    LV_DRAW_BUF_INIT_STATIC(buf_rgb565);
    canvas_draw_buf_reshape(&buf_rgb565);
    lv_canvas_set_draw_buf(canvas, &buf_rgb565);
    lv_canvas_fill_bg(canvas, lv_color_hex(0x00ff00), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(0x0000ff), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_xrgb8888, 10, 10, LV_COLOR_FORMAT_XRGB8888);
    LV_DRAW_BUF_INIT_STATIC(buf_xrgb8888);
    canvas_draw_buf_reshape(&buf_xrgb8888);
    lv_canvas_set_draw_buf(canvas, &buf_xrgb8888);
    lv_canvas_fill_bg(canvas, lv_color_hex(0x00ff00), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(0x0000ff), 0);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");

    LV_DRAW_BUF_DEFINE_STATIC(buf_argb8888, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(buf_argb8888);
    canvas_draw_buf_reshape(&buf_argb8888);
    lv_canvas_set_draw_buf(canvas, &buf_argb8888);
    lv_canvas_fill_bg(canvas, lv_color_hex(0x00ff00), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 1, 7, lv_color_hex(0x0000ff), LV_OPA_COVER);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_1.png");
}

void test_canvas_triangles(void)
{
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_obj_center(canvas);

    LV_DRAW_BUF_DEFINE_STATIC(buf, 500, 500, LV_COLOR_FORMAT_RGB888);
    LV_DRAW_BUF_INIT_STATIC(buf);
    canvas_draw_buf_reshape(&buf);
    lv_canvas_set_draw_buf(canvas, &buf);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_canvas_fill_bg(canvas, lv_color_hex(0xAFAFAF), LV_OPA_COVER);

    {
        /* 1. Right-angled with vertical side on left (bottom-left corner at right angle) */
        lv_draw_triangle_dsc_t tri_dsc;
        lv_draw_triangle_dsc_init(&tri_dsc);
        tri_dsc.color = lv_color_hex(0xff0000);
        tri_dsc.p[0].x = 50;
        tri_dsc.p[0].y = 50;
        tri_dsc.p[1].x = 50;
        tri_dsc.p[1].y = 100;
        tri_dsc.p[2].x = 100;
        tri_dsc.p[2].y = 100;
        lv_draw_triangle(&layer, &tri_dsc);
    }

    {
        /* 2. Right-angled with vertical side on right (bottom-right corner at right angle) */
        lv_draw_triangle_dsc_t tri_dsc;
        lv_draw_triangle_dsc_init(&tri_dsc);
        tri_dsc.color = lv_color_hex(0x00ff00);
        tri_dsc.p[0].x = 150;
        tri_dsc.p[0].y = 50;
        tri_dsc.p[1].x = 200;
        tri_dsc.p[1].y = 50;
        tri_dsc.p[2].x = 200;
        tri_dsc.p[2].y = 100;
        lv_draw_triangle(&layer, &tri_dsc);
    }

    {
        /* 3. Pointing up (no vertical sides) */
        lv_draw_triangle_dsc_t tri_dsc;
        lv_draw_triangle_dsc_init(&tri_dsc);
        tri_dsc.color = lv_color_hex(0x0000ff);
        tri_dsc.p[0].x = 250;
        tri_dsc.p[0].y = 50;
        tri_dsc.p[1].x = 230;
        tri_dsc.p[1].y = 100;
        tri_dsc.p[2].x = 270;
        tri_dsc.p[2].y = 100;
        lv_draw_triangle(&layer, &tri_dsc);
    }

    {
        /* 4. Pointing down (no vertical sides) */
        lv_draw_triangle_dsc_t tri_dsc;
        lv_draw_triangle_dsc_init(&tri_dsc);
        tri_dsc.color = lv_color_hex(0xffff00);
        tri_dsc.p[0].x = 350;
        tri_dsc.p[0].y = 50;
        tri_dsc.p[1].x = 390;
        tri_dsc.p[1].y = 50;
        tri_dsc.p[2].x = 370;
        tri_dsc.p[2].y = 100;
        lv_draw_triangle(&layer, &tri_dsc);
    }

    {
        /* 5. Pointing left (no vertical sides) */
        lv_draw_triangle_dsc_t tri_dsc;
        lv_draw_triangle_dsc_init(&tri_dsc);
        tri_dsc.color = lv_color_hex(0xff00ff);
        tri_dsc.p[0].x = 50;
        tri_dsc.p[0].y = 150;
        tri_dsc.p[1].x = 100;
        tri_dsc.p[1].y = 130;
        tri_dsc.p[2].x = 100;
        tri_dsc.p[2].y = 170;
        lv_draw_triangle(&layer, &tri_dsc);
    }

    {
        /* 6. Pointing right (no vertical sides) */
        lv_draw_triangle_dsc_t tri_dsc;
        lv_draw_triangle_dsc_init(&tri_dsc);
        tri_dsc.color = lv_color_hex(0x00ffff);
        tri_dsc.p[0].x = 150;
        tri_dsc.p[0].y = 130;
        tri_dsc.p[1].x = 150;
        tri_dsc.p[1].y = 170;
        tri_dsc.p[2].x = 200;
        tri_dsc.p[2].y = 150;
        lv_draw_triangle(&layer, &tri_dsc);
    }

    {
        /* 7. Obtuse triangle (one angle > 90°) */
        lv_draw_triangle_dsc_t tri_dsc;
        lv_draw_triangle_dsc_init(&tri_dsc);
        tri_dsc.color = lv_color_hex(0x800080);
        tri_dsc.p[0].x = 250;
        tri_dsc.p[0].y = 150;
        tri_dsc.p[1].x = 300;
        tri_dsc.p[1].y = 140;
        tri_dsc.p[2].x = 280;
        tri_dsc.p[2].y = 180;
        lv_draw_triangle(&layer, &tri_dsc);
    }

    {
        /* 8. Acute equilateral-ish triangle */
        lv_draw_triangle_dsc_t tri_dsc;
        lv_draw_triangle_dsc_init(&tri_dsc);
        tri_dsc.color = lv_color_hex(0xffa500);
        tri_dsc.p[0].x = 350;
        tri_dsc.p[0].y = 150;
        tri_dsc.p[1].x = 330;
        tri_dsc.p[1].y = 180;
        tri_dsc.p[2].x = 370;
        tri_dsc.p[2].y = 180;
        lv_draw_triangle(&layer, &tri_dsc);
    }

    {
        /* 9. Very flat wide triangle (horizontal base) */
        lv_draw_triangle_dsc_t tri_dsc;
        lv_draw_triangle_dsc_init(&tri_dsc);
        tri_dsc.color = lv_color_hex(0x008000);
        tri_dsc.p[0].x = 100;
        tri_dsc.p[0].y = 250;
        tri_dsc.p[1].x = 50;
        tri_dsc.p[1].y = 280;
        tri_dsc.p[2].x = 150;
        tri_dsc.p[2].y = 280;
        lv_draw_triangle(&layer, &tri_dsc);
    }

    {
        /* 10. Tall thin triangle */
        lv_draw_triangle_dsc_t tri_dsc;
        lv_draw_triangle_dsc_init(&tri_dsc);
        tri_dsc.color = lv_color_hex(0x000080);
        tri_dsc.p[0].x = 250;
        tri_dsc.p[0].y = 230;
        tri_dsc.p[1].x = 240;
        tri_dsc.p[1].y = 290;
        tri_dsc.p[2].x = 260;
        tri_dsc.p[2].y = 290;
        lv_draw_triangle(&layer, &tri_dsc);
    }

    {
        /* 11. Y value of last point is the biggest one */
        lv_draw_triangle_dsc_t tri_dsc;
        lv_draw_triangle_dsc_init(&tri_dsc);
        tri_dsc.color = lv_color_hex(0x800000);

        tri_dsc.p[0].x = 350;
        tri_dsc.p[0].y = 230;

        tri_dsc.p[1].x = 350;
        tri_dsc.p[1].y = 280;
        tri_dsc.p[2].x = 390;
        tri_dsc.p[2].y = 290;
        lv_draw_triangle(&layer, &tri_dsc);
    }
    {
        /* 12. Y value of last point is the smallest one */
        lv_draw_triangle_dsc_t tri_dsc;
        lv_draw_triangle_dsc_init(&tri_dsc);
        tri_dsc.color = lv_color_hex(0x808000);

        tri_dsc.p[0].x = 100;
        tri_dsc.p[0].y = 350;

        tri_dsc.p[1].x = 100;
        tri_dsc.p[1].y = 400;
        tri_dsc.p[2].x = 150;
        tri_dsc.p[2].y = 320;
        lv_draw_triangle(&layer, &tri_dsc);
    }

    lv_canvas_finish_layer(canvas, &layer);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_2.png");
}

#endif
