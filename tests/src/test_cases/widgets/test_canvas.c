#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "lv_test_init.h"

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

void test_canvas_buffer_operations(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);

    /* Test uninitialized buffer */
    static LV_ATTRIBUTE_MEM_ALIGN uint8_t buf[LV_DRAW_BUF_SIZE(100, 50, LV_COLOR_FORMAT_RGB565)];
    lv_canvas_set_buffer(canvas, buf, 100, 50, LV_COLOR_FORMAT_RGB565);
    TEST_ASSERT_NOT_NULL(lv_canvas_get_draw_buf(canvas));
    TEST_ASSERT_NOT_NULL(lv_canvas_get_image(canvas));
    TEST_ASSERT_NOT_NULL(lv_canvas_get_buf(canvas));

    /* Test lv_canvas_fill_bg to ensure no out-of-bounds access */
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);

    /* Test draw buffer with handlers */
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 100, 50, LV_COLOR_FORMAT_RGB565);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);
    canvas_draw_buf_reshape(&draw_buf);

    lv_canvas_set_draw_buf(canvas, &draw_buf);
    TEST_ASSERT_EQUAL_PTR(&draw_buf, lv_canvas_get_draw_buf(canvas));
}

void test_canvas_layer_operations(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);
    int draw_counter = 0;
    lv_obj_add_event_cb(canvas, draw_event_cb, LV_EVENT_DRAW_MAIN, &draw_counter);

    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 100, 100, LV_COLOR_FORMAT_RGB565);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);
    canvas_draw_buf_reshape(&draw_buf);
    lv_canvas_set_draw_buf(canvas, &draw_buf);

    /* Test layer initialization */
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    TEST_ASSERT_EQUAL_PTR(&draw_buf, layer.draw_buf);
    TEST_ASSERT_EQUAL(LV_COLOR_FORMAT_RGB565, layer.color_format);

    /* Test drawing on layer */
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_black();
    lv_area_t rect_area = {10, 10, 50, 50};
    lv_draw_rect(&layer, &rect_dsc, &rect_area);

    /* Test layer finalization */
    lv_canvas_finish_layer(canvas, &layer);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT(1, draw_counter);

    /* Test for secondary redrawing */
    lv_canvas_finish_layer(canvas, &layer);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_INT(1, draw_counter);
}

void test_canvas_image_operations(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);

    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 10, 10, LV_COLOR_FORMAT_RGB565);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);
    canvas_draw_buf_reshape(&draw_buf);
    lv_canvas_set_draw_buf(canvas, &draw_buf);

    /* Test get_image */
    lv_image_dsc_t * img = lv_canvas_get_image(canvas);
    TEST_ASSERT_NOT_NULL(img);
    TEST_ASSERT_EQUAL_UINT32(10, img->header.w);
    TEST_ASSERT_EQUAL_UINT32(10, img->header.h);

    /* Test get_buf */
    const void * buf = lv_canvas_get_buf(canvas);
    TEST_ASSERT_NOT_NULL(buf);
    TEST_ASSERT_EQUAL_PTR(draw_buf.unaligned_data, buf);
}

static void verify_canvas_px(lv_obj_t * canvas, lv_color_t expected_color, lv_opa_t expected_opa)
{
    for(int32_t y = 0; y < 10; y++) {
        for(int32_t x = 0; x < 10; x++) {
            lv_color32_t px = lv_canvas_get_px(canvas, x, y);
            TEST_ASSERT_EQUAL_UINT8(expected_color.red, px.red);
            TEST_ASSERT_EQUAL_UINT8(expected_color.green, px.green);
            TEST_ASSERT_EQUAL_UINT8(expected_color.blue, px.blue);
            TEST_ASSERT_EQUAL_UINT8(expected_opa, px.alpha);
        }
    }
}

static lv_color_t canvas_convert_c16(lv_color_t color)
{
    /* Use the same color conversion algorithm as canvas to keep the results consistent */
    uint16_t px = lv_color_to_u16(color);
    lv_color16_t * c16 = (lv_color16_t *) &px;
    lv_color_t ret;
    ret.red = (c16->red * 2106) >> 8;  /*To make it rounded*/
    ret.green = (c16->green * 1037) >> 8;
    ret.blue = (c16->blue * 2106) >> 8;
    return ret;
}

void test_canvas_fill_background_formats(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);

    /* Test ARGB8888 format */
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf_argb, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(draw_buf_argb);
    canvas_draw_buf_reshape(&draw_buf_argb);
    lv_canvas_set_draw_buf(canvas, &draw_buf_argb);

    lv_color_t fill_color_argb = lv_color_hex(0xABCDEF);
    lv_canvas_fill_bg(canvas, fill_color_argb, 0x80);
    verify_canvas_px(canvas, fill_color_argb, 0x80);

    /* Test RGB565 format */
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf_rgb565, 10, 10, LV_COLOR_FORMAT_RGB565);
    LV_DRAW_BUF_INIT_STATIC(draw_buf_rgb565);
    canvas_draw_buf_reshape(&draw_buf_rgb565);
    lv_canvas_set_draw_buf(canvas, &draw_buf_rgb565);

    lv_color_t fill_color_rgb565 = lv_color_hex(0x123456);
    lv_canvas_fill_bg(canvas, fill_color_rgb565, LV_OPA_COVER);
    verify_canvas_px(canvas, canvas_convert_c16(fill_color_rgb565), LV_OPA_COVER);

    /* Test A8 format */
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf_a8, 10, 10, LV_COLOR_FORMAT_A8);
    LV_DRAW_BUF_INIT_STATIC(draw_buf_a8);
    canvas_draw_buf_reshape(&draw_buf_a8);
    lv_canvas_set_draw_buf(canvas, &draw_buf_a8);

    lv_canvas_fill_bg(canvas, lv_color_black(), 0x80);
    verify_canvas_px(canvas, lv_obj_get_style_image_recolor(canvas, LV_PART_MAIN), 0x80);

    /* Test RGB888 format */
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf_rgb888, 10, 10, LV_COLOR_FORMAT_RGB888);
    LV_DRAW_BUF_INIT_STATIC(draw_buf_rgb888);
    canvas_draw_buf_reshape(&draw_buf_rgb888);
    lv_canvas_set_draw_buf(canvas, &draw_buf_rgb888);

    lv_color_t fill_color_rgb888 = lv_color_hex(0x123456);
    lv_canvas_fill_bg(canvas, fill_color_rgb888, LV_OPA_COVER);
    verify_canvas_px(canvas, fill_color_rgb888, LV_OPA_COVER);

    /* Test unsupported format (default case) */
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf_unsupported, 10, 10, LV_COLOR_FORMAT_ARGB8565);
    LV_DRAW_BUF_INIT_STATIC(draw_buf_unsupported);
    canvas_draw_buf_reshape(&draw_buf_unsupported);
    lv_canvas_set_draw_buf(canvas, &draw_buf_unsupported);
    verify_canvas_px(canvas, lv_color_black(), 0);
}

void test_canvas_layer_complex_drawing(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);
    int draw_counter = 0;
    lv_obj_add_event_cb(canvas, draw_event_cb, LV_EVENT_DRAW_MAIN, &draw_counter);

    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 100, 100, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);
    canvas_draw_buf_reshape(&draw_buf);
    lv_canvas_set_draw_buf(canvas, &draw_buf);

    /* Initialize layer */
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    /* Draw multiple shapes */
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_hex(0xFF0000);
    lv_area_t rect_area = {10, 10, 50, 50};
    lv_draw_rect(&layer, &rect_dsc, &rect_area);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_white();
    lv_point_t label_pos = {20, 20};
    lv_area_t label_area = {label_pos.x, label_pos.y, label_pos.x + 100, label_pos.y + 50};
    /* label_dsc already declared above, just initialize and use */
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.text = "Test";
    lv_draw_label(&layer, &label_dsc, &label_area);

    /* Finish layer and verify */
    lv_canvas_finish_layer(canvas, &layer);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/canvas_layer_complex.png");
    TEST_ASSERT_EQUAL_INT(1, draw_counter);

    /* Verify pixel content */
    lv_color32_t px = lv_canvas_get_px(canvas, 30, 30);
    TEST_ASSERT_EQUAL_UINT8(0xFF, px.red);
    TEST_ASSERT_EQUAL_UINT8(0x00, px.green);
    TEST_ASSERT_EQUAL_UINT8(0x00, px.blue);
}

void test_canvas_pixel_operations(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);

    /* Test RGB565 format */
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf_rgb565, 10, 10, LV_COLOR_FORMAT_RGB565);
    LV_DRAW_BUF_INIT_STATIC(draw_buf_rgb565);
    canvas_draw_buf_reshape(&draw_buf_rgb565);
    lv_canvas_set_draw_buf(canvas, &draw_buf_rgb565);

    /* Test basic color conversion */
    lv_canvas_set_px(canvas, 0, 0, lv_color_hex(0x0000FF), 0);
    lv_color32_t px_blue = lv_canvas_get_px(canvas, 0, 0);
    TEST_ASSERT_EQUAL_UINT8(0xFF, px_blue.blue);
    TEST_ASSERT_EQUAL_UINT8(0x00, px_blue.green);
    TEST_ASSERT_EQUAL_UINT8(0x00, px_blue.red);
    TEST_ASSERT_EQUAL_UINT8(0xFF, px_blue.alpha);

    /* Test ARGB8888 format */
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf_argb, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(draw_buf_argb);
    canvas_draw_buf_reshape(&draw_buf_argb);
    lv_canvas_set_draw_buf(canvas, &draw_buf_argb);

    lv_color_t test_color = lv_color_hex(0x1234);

    lv_canvas_set_px(canvas, 3, 3, test_color, 0x80);
    lv_color32_t px = lv_canvas_get_px(canvas, 3, 3);
    TEST_ASSERT_EQUAL_UINT8(test_color.red, px.red);
    TEST_ASSERT_EQUAL_UINT8(test_color.green, px.green);
    TEST_ASSERT_EQUAL_UINT8(test_color.blue, px.blue);
    TEST_ASSERT_EQUAL_UINT8(0x80, px.alpha);
}

void test_canvas_fill_background(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);

    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);
    canvas_draw_buf_reshape(&draw_buf);
    lv_canvas_set_draw_buf(canvas, &draw_buf);

    lv_color_t fill_color = lv_color_hex(0xABCD);
    lv_canvas_fill_bg(canvas, fill_color, LV_OPA_COVER);

    for(int32_t y = 0; y < 10; y++) {
        for(int32_t x = 0; x < 10; x++) {
            lv_color32_t px = lv_canvas_get_px(canvas, x, y);
            TEST_ASSERT_EQUAL_UINT8(fill_color.red, px.red);
            TEST_ASSERT_EQUAL_UINT8(fill_color.green, px.green);
            TEST_ASSERT_EQUAL_UINT8(fill_color.blue, px.blue);
        }
    }
}

void test_canvas_copy_buffer(void)
{
    lv_obj_t * src_canvas = lv_canvas_create(g_screen_active);
    lv_obj_t * dst_canvas = lv_canvas_create(g_screen_active);

    LV_DRAW_BUF_DEFINE_STATIC(src_buf, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_DEFINE_STATIC(dst_buf, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(src_buf);
    LV_DRAW_BUF_INIT_STATIC(dst_buf);
    canvas_draw_buf_reshape(&src_buf);
    canvas_draw_buf_reshape(&dst_buf);

    lv_canvas_set_draw_buf(src_canvas, &src_buf);
    lv_canvas_set_draw_buf(dst_canvas, &dst_buf);

    /* Fill source with pattern */
    for(int32_t y = 0; y < 10; y++) {
        for(int32_t x = 0; x < 10; x++) {
            lv_color_t c = lv_color_make(x * 25, y * 25, (x + y) * 12);
            lv_canvas_set_px(src_canvas, x, y, c, LV_OPA_COVER);
        }
    }

    /* Copy area */
    lv_area_t dst_area = {2, 2, 7, 7};
    lv_area_t src_area = {0, 0, 5, 5};
    lv_canvas_copy_buf(dst_canvas, &dst_area, &src_buf, &src_area);

    /* Verify copied pixels */
    for(int32_t y = 0; y < 6; y++) {
        for(int32_t x = 0; x < 6; x++) {
            lv_color32_t src_px = lv_canvas_get_px(src_canvas, x, y);
            lv_color32_t dst_px = lv_canvas_get_px(dst_canvas, x + 2, y + 2);
            TEST_ASSERT_EQUAL_UINT8(src_px.red, dst_px.red);
            TEST_ASSERT_EQUAL_UINT8(src_px.green, dst_px.green);
            TEST_ASSERT_EQUAL_UINT8(src_px.blue, dst_px.blue);
            TEST_ASSERT_EQUAL_UINT8(src_px.alpha, dst_px.alpha);
        }
    }
}

void test_canvas_palette_operations(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);

    /* Test with supported color formats */
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);
    canvas_draw_buf_reshape(&draw_buf);
    lv_canvas_set_draw_buf(canvas, &draw_buf);

    /* Test directly setting and getting pixel colors */
    lv_color32_t test_color = {.red = 0x12, .green = 0x34, .blue = 0x56, .alpha = 0xFF};
    lv_canvas_set_px(canvas, 0, 0, lv_color_hex(0x123456), LV_OPA_COVER);
    lv_color32_t px = lv_canvas_get_px(canvas, 0, 0);
    TEST_ASSERT_EQUAL_UINT8(test_color.red, px.red);
    TEST_ASSERT_EQUAL_UINT8(test_color.green, px.green);
    TEST_ASSERT_EQUAL_UINT8(test_color.blue, px.blue);
    TEST_ASSERT_EQUAL_UINT8(test_color.alpha, px.alpha);
}

void test_canvas_copy_buffer_partial(void)
{
    lv_obj_t * src_canvas = lv_canvas_create(g_screen_active);
    lv_obj_t * dst_canvas = lv_canvas_create(g_screen_active);

    LV_DRAW_BUF_DEFINE_STATIC(src_buf, 20, 20, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_DEFINE_STATIC(dst_buf, 20, 20, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(src_buf);
    LV_DRAW_BUF_INIT_STATIC(dst_buf);
    canvas_draw_buf_reshape(&src_buf);
    canvas_draw_buf_reshape(&dst_buf);

    lv_canvas_set_draw_buf(src_canvas, &src_buf);
    lv_canvas_set_draw_buf(dst_canvas, &dst_buf);

    /* Fill source with pattern */
    for(int32_t y = 0; y < 20; y++) {
        for(int32_t x = 0; x < 20; x++) {
            lv_color_t c = lv_color_make(x * 12, y * 12, (x + y) * 6);
            lv_canvas_set_px(src_canvas, x, y, c, LV_OPA_COVER);
        }
    }

    /* Copy partial area with offset */
    lv_area_t src_area = {5, 5, 15, 15};
    lv_area_t dst_area = {0, 0, 10, 10};
    lv_canvas_copy_buf(src_canvas, &src_area, &dst_buf, &dst_area);

    /* Verify copied pixels */
    for(int32_t y = 0; y < 11; y++) {
        for(int32_t x = 0; x < 11; x++) {
            lv_color32_t src_px = lv_canvas_get_px(src_canvas, x + 5, y + 5);
            lv_color32_t dst_px = lv_canvas_get_px(dst_canvas, x, y);
            TEST_ASSERT_TRUE(lv_color32_eq(src_px, dst_px));
        }
    }
}

void test_canvas_empty_draw_buf(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);

    TEST_ASSERT_NULL(lv_canvas_get_draw_buf(canvas));
    TEST_ASSERT_NULL(lv_canvas_get_image(canvas));
    TEST_ASSERT_NULL(lv_canvas_get_buf(canvas));
    lv_layer_t layer;

    LV_DRAW_BUF_DEFINE_STATIC(src_buf, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(src_buf);
    canvas_draw_buf_reshape(&src_buf);

    lv_canvas_copy_buf(canvas, NULL, &src_buf, NULL);
    lv_canvas_fill_bg(canvas, lv_color_hex(0xFFFFFF), LV_OPA_COVER);
    lv_canvas_init_layer(canvas, &layer);
    lv_canvas_set_px(canvas, 0, 0, lv_color_hex(0x000000), LV_OPA_COVER);

    lv_color32_t src_px = lv_color_to_32(lv_color_hex(0x000000), LV_OPA_0);
    lv_color32_t dst_px = lv_canvas_get_px(canvas, 0, 0);
    TEST_ASSERT_TRUE(lv_color32_eq(src_px, dst_px));

    lv_canvas_set_palette(canvas, 0, src_px);
    lv_canvas_finish_layer(canvas, &layer);
}

void test_canvas_out_of_area(void)
{
    lv_obj_t * canvas = lv_canvas_create(g_screen_active);

    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 10, 10, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);
    canvas_draw_buf_reshape(&draw_buf);
    lv_canvas_set_draw_buf(canvas, &draw_buf);

    lv_color_t test_color = lv_color_hex(0x1234);

    lv_canvas_set_px(canvas, -1, -1, test_color, LV_OPA_0);
    lv_color32_t px = lv_canvas_get_px(canvas, -1, -1);
    TEST_ASSERT_EQUAL_UINT8(0x00, px.red);
    TEST_ASSERT_EQUAL_UINT8(0x00, px.green);
    TEST_ASSERT_EQUAL_UINT8(0x00, px.blue);
    TEST_ASSERT_EQUAL_UINT8(0x00, px.alpha);
}

void test_line_bigger_than_display_resolution(void)
{
    int32_t hor_res = lv_display_get_horizontal_resolution(lv_display_get_default());
    int32_t ver_res = lv_display_get_vertical_resolution(lv_display_get_default());
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, LV_TEST_DISPLAY_HOR_RES + 1, LV_TEST_DISPLAY_VER_RES + 1, LV_COLOR_FORMAT_NATIVE);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);
    draw_buf.header.stride = LV_STRIDE_AUTO;

    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_draw_buf(canvas, &draw_buf);
    lv_canvas_fill_bg(canvas, lv_color_hex3(0xccc), LV_OPA_COVER);
    lv_obj_center(canvas);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_line_dsc_t dsc;
    lv_draw_line_dsc_init(&dsc);
    dsc.color = lv_palette_main(LV_PALETTE_RED);
    dsc.width = 4;
    dsc.round_end = 1;
    dsc.round_start = 1;
    dsc.p1.x = 0;
    dsc.p1.y = 0;
    dsc.p2.x = hor_res + 1;
    dsc.p2.y = ver_res + 1;
    lv_draw_line(&layer, &dsc);
    lv_canvas_finish_layer(canvas, &layer);

    /* Test passes if no crash occurs when drawing a line with endpoint
     * at (hor_res+1, ver_res+1) on a buffer of size (hor_res+1)x(ver_res+1)*/
}

#endif
