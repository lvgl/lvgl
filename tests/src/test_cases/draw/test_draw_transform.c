#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_scr_act(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

}

void tearDown(void)
{
    /* Function run after every test */
}


static void canvas_basic_render(lv_color_format_t render_cf, uint8_t * canvas_buf)
{
    lv_obj_t * canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, canvas_buf, 100, 100, render_cf);
    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_RED, 2), LV_OPA_TRANSP);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_palette_main(LV_PALETTE_LIGHT_BLUE);
    rect_dsc.radius = 10;

    lv_area_t rect_area = {10, 10, 90, 90};

    lv_draw_rect(&layer, &rect_dsc, &rect_area);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.align = LV_TEXT_ALIGN_CENTER;
    label_dsc.text = "Hello world";

    lv_area_t label_area = {10, 30, 90, 70};

    lv_draw_label(&layer, &label_dsc, &label_area);

    lv_canvas_finish_layer(canvas, &layer);

    lv_obj_del(canvas);
}

static void draw_images(lv_layer_t * layer, lv_draw_image_dsc_t * dsc)
{
    /* Angle */
    lv_area_t area = { 10, 30, 109, 129};
    dsc->pivot.x = 50;
    dsc->pivot.y = 50;

    lv_draw_image(layer, dsc, &area);

    lv_area_move(&area, 110, 0);
    dsc->angle = 900;
    lv_draw_image(layer, dsc, &area);

    lv_area_move(&area, 110, 0);
    dsc->angle = 1800;
    lv_draw_image(layer, dsc, &area);

    lv_area_move(&area, 110, 0);
    dsc->angle = -900;
    lv_draw_image(layer, dsc, &area);

    lv_area_move(&area, 150, 0);
    dsc->angle = 300;
    lv_draw_image(layer, dsc, &area);

    dsc->pivot.x = 0;
    dsc->pivot.y = 0;
    dsc->angle = 300;
    lv_area_move(&area, 150, 0);
    lv_draw_image(layer, dsc, &area);

    /* Zoom */
    area.x1 = 10;
    area.y1 = 150;
    area.x2 = 109;
    area.y2 = 249;
    dsc->pivot.x = 50;
    dsc->pivot.y = 50;
    dsc->angle = 0;
    dsc->zoom = 128;
    lv_draw_image(layer, dsc, &area);

    lv_area_move(&area, 110, 0);
    dsc->zoom = 300;
    lv_draw_image(layer, dsc, &area);

    lv_area_move(&area, 110, 0);
    dsc->pivot.x = 0;
    dsc->pivot.y = 0;
    dsc->zoom = 128;
    lv_draw_image(layer, dsc, &area);

    lv_area_move(&area, 110, 0);
    dsc->zoom = 300;
    lv_draw_image(layer, dsc, &area);

    /* Zoom + Angle*/
    dsc->angle = 650;
    dsc->zoom = 200;
    lv_area_move(&area, 200, 0);
    lv_draw_image(layer, dsc, &area);

    /*Edge color bleeding test*/
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_palette_lighten(LV_PALETTE_RED, 2);
    lv_area_t fill_area = {10, 290, 750, 360};
    lv_draw_rect(layer, &rect_dsc, &fill_area);
    lv_area_move(&fill_area, 0, 70);
    rect_dsc.bg_color = lv_palette_main(LV_PALETTE_LIGHT_BLUE);
    lv_draw_rect(layer, &rect_dsc, &fill_area);

    area.x1 = 10;
    area.y1 = 310;
    area.x2 = 109;
    area.y2 = 409;
    dsc->pivot.x = 50;
    dsc->pivot.y = 50;
    dsc->angle = 0;
    dsc->zoom = 256;
    lv_draw_image(layer, dsc, &area);

    lv_area_move(&area, 180, 0);
    dsc->angle = 300;
    dsc->zoom = 200;
    lv_draw_image(layer, dsc, &area);

    lv_area_move(&area, 150, 0);
    dsc->angle = 1400;
    dsc->zoom = 150;
    lv_draw_image(layer, dsc, &area);

    lv_area_move(&area, 120, 0);
    dsc->angle = 2000;
    dsc->zoom = 100;
    lv_draw_image(layer, dsc, &area);
}

void create_test_screen(lv_color_format_t render_cf, const char * name)
{
    static uint8_t canvas_buf_large[760 * 440 * 4];

    lv_obj_t * canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, canvas_buf_large, 760, 440, LV_COLOR_FORMAT_ARGB8888);

    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 2), LV_OPA_50);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    static uint8_t canvas_buf_small[100 * 100 * 4];

    lv_image_dsc_t img;
    img.data = canvas_buf_small;
    img.header.cf = render_cf;
    img.header.w = 100;
    img.header.h = 100;
    img.header.stride = 100 * lv_color_format_get_size(render_cf);
    img.header.always_zero = 0;

    lv_draw_image_dsc_t img_dsc;
    lv_draw_image_dsc_init(&img_dsc);
    img_dsc.src = &img;

    canvas_basic_render(render_cf, canvas_buf_small);
    draw_images(&layer, &img_dsc);

    lv_canvas_finish_layer(canvas, &layer);

    char buf[64];
    lv_snprintf(buf, sizeof(buf), "/draw/transform_%s.png", name);
    TEST_ASSERT_EQUAL_SCREENSHOT(buf);

    lv_obj_del(canvas);
}

void test_draw_transform_rgb565(void)
{
    create_test_screen(LV_COLOR_FORMAT_RGB565, "rgb565");
}

void test_draw_transform_rgb8888(void)
{
    create_test_screen(LV_COLOR_FORMAT_RGB888, "rgb888");
}

void test_draw_transform_xrgb8888(void)
{
    create_test_screen(LV_COLOR_FORMAT_XRGB8888, "xrgb8888");
}

void test_draw_transform_argb8888(void)
{
    create_test_screen(LV_COLOR_FORMAT_ARGB8888, "argb8888");
}

#endif
