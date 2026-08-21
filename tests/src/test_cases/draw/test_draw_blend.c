#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"


#include "unity/unity.h"

static uint16_t make_rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
    return (uint16_t)(((red & 0xf8U) << 8) | ((green & 0xfcU) << 3) | (blue >> 3));
}

static uint16_t add_argb8888_to_rgb565(uint16_t dst, lv_color32_t src)
{
    if(src.alpha == 0) return dst;

    uint16_t red = (uint16_t)((dst >> 11) + (src.red >> 3));
    uint16_t green = (uint16_t)(((dst >> 5) & 0x3fU) + (src.green >> 2));
    uint16_t blue = (uint16_t)((dst & 0x1fU) + (src.blue >> 3));
    if(red > 31) red = 31;
    if(green > 63) green = 63;
    if(blue > 31) blue = 31;

    uint16_t res = (uint16_t)((red << 11) | (green << 5) | blue);
    return lv_color_16_16_mix(res, dst, src.alpha);
}

void setUp(void)
{
    /* Function run before every test */
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

}

void tearDown(void)
{
    /* Function run after every test */
}

static void canvas_basic_render(uint8_t * canvas_buf, lv_color_format_t render_cf, const char * name_main,
                                const char * name_sub)
{
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_buffer(canvas, canvas_buf, 180, 180, render_cf);

    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 2), LV_OPA_COVER);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_area_t area;
    char txt[64];
    lv_snprintf(txt, sizeof(txt), "%s_to_%s", name_main, name_sub);
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.text = txt;

    area.x1 = 3;
    area.x2 = 180;
    area.y1 = 5;
    area.y2 = 25;
    lv_draw_label(&layer, &label_dsc, &area);

    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.radius = 10;
    rect_dsc.bg_color = lv_palette_main(LV_PALETTE_RED);
    rect_dsc.bg_opa = LV_OPA_COVER;
    rect_dsc.outline_color = lv_palette_main(LV_PALETTE_INDIGO);
    rect_dsc.outline_width = 10;
    rect_dsc.outline_pad = -5;
    rect_dsc.outline_opa = LV_OPA_60;
    rect_dsc.shadow_color = lv_palette_main(LV_PALETTE_ORANGE);
    rect_dsc.shadow_width = 10;
    rect_dsc.shadow_offset_x = 5;
    rect_dsc.shadow_offset_y = 10;

    area.x1 = 10;
    area.x2 = 170;
    area.y1 = 30;
    area.y2 = 60;

    lv_draw_rect(&layer, &rect_dsc, &area);

    area.y1 = 80;
    area.y2 = 110;

    rect_dsc.bg_grad.dir = LV_GRAD_DIR_HOR;
    rect_dsc.bg_grad.stops_count = 2;
    rect_dsc.bg_grad.stops[0].color = lv_palette_main(LV_PALETTE_RED);
    rect_dsc.bg_grad.stops[0].opa = LV_OPA_COVER;
    rect_dsc.bg_grad.stops[0].frac = 0;
    rect_dsc.bg_grad.stops[1].color = lv_palette_main(LV_PALETTE_GREEN);
    rect_dsc.bg_grad.stops[1].opa = LV_OPA_COVER;
    rect_dsc.bg_grad.stops[1].frac = 255;

    lv_draw_rect(&layer, &rect_dsc, &area);

    area.y1 = 130;
    area.y2 = 160;
    rect_dsc.bg_grad.stops[0].opa = LV_OPA_30;

    lv_draw_rect(&layer, &rect_dsc, &area);

    lv_canvas_finish_layer(canvas, &layer);

    lv_obj_delete(canvas);
}

void canvas_blend_test(lv_obj_t  * canvas_large, lv_draw_image_dsc_t * img_dsc, const char * name_main,
                       const char * name_sub, lv_color_format_t small_render_cf, uint32_t idx)
{
    lv_draw_buf_t * img = (lv_draw_buf_t *)img_dsc->src;
    img->header.cf = small_render_cf;
    img->header.stride = lv_draw_buf_width_to_stride(180, small_render_cf);
    canvas_basic_render((uint8_t *)img->data, small_render_cf, name_main, name_sub);

    lv_area_t area;
    area.x1 = (idx % 2) * 380 + 10;
    area.x2 = area.x1 + 179;
    area.y1 = (idx / 2) * 190 + 10;
    area.y2 = area.y1 + 179;

    lv_layer_t layer;

    img_dsc->opa = LV_OPA_COVER;
    lv_canvas_init_layer(canvas_large, &layer);
    lv_draw_image(&layer, img_dsc, &area);
    lv_canvas_finish_layer(canvas_large, &layer);

    lv_area_move(&area, 190, 0);

    img_dsc->opa = LV_OPA_50;
    lv_canvas_init_layer(canvas_large, &layer);
    lv_draw_image(&layer, img_dsc, &area);
    lv_canvas_finish_layer(canvas_large, &layer);

    lv_image_cache_drop(img);
}

static void canvas_draw(const char * name, lv_color_format_t large_render_cf)
{
    lv_obj_clean(lv_screen_active());

    static LV_ATTRIBUTE_MEM_ALIGN uint8_t canvas_buf[LV_TEST_WIDTH_TO_STRIDE(180, 4) * 180 + LV_DRAW_BUF_ALIGN];

    static uint8_t canvas2_buf[LV_TEST_WIDTH_TO_STRIDE(768, 4) * 390 + LV_DRAW_BUF_ALIGN];
    lv_obj_t * canvas2 = lv_canvas_create(lv_screen_active());
    lv_canvas_set_buffer(canvas2, lv_draw_buf_align(canvas2_buf, large_render_cf), 768, 390, large_render_cf);
    lv_canvas_fill_bg(canvas2, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 2), LV_OPA_COVER);

    lv_draw_buf_t img;
    lv_draw_buf_init(&img, 180, 180, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO, canvas_buf, sizeof(canvas_buf));

    lv_draw_image_dsc_t img_dsc;
    lv_draw_image_dsc_init(&img_dsc);
    img_dsc.src = &img;

    canvas_blend_test(canvas2, &img_dsc, "rgb565", name, LV_COLOR_FORMAT_RGB565, 0);
    canvas_blend_test(canvas2, &img_dsc, "rgb888", name, LV_COLOR_FORMAT_RGB888, 1);
    canvas_blend_test(canvas2, &img_dsc, "xrgb8888", name, LV_COLOR_FORMAT_XRGB8888, 2);
    canvas_blend_test(canvas2, &img_dsc, "argb8888", name, LV_COLOR_FORMAT_ARGB8888, 3);

    char fn_buf[64];
    lv_snprintf(fn_buf, sizeof(fn_buf), "draw/blend_to_%s.png", name);
    TEST_ASSERT_EQUAL_SCREENSHOT(fn_buf);
}

void test_xrgb8888(void)
{
    canvas_draw("xrgb8888", LV_COLOR_FORMAT_XRGB8888);
}

void test_argb8888(void)
{
    canvas_draw("argb8888", LV_COLOR_FORMAT_ARGB8888);
}

void test_rgb888(void)
{
    canvas_draw("rgb888", LV_COLOR_FORMAT_RGB888);
}

void test_rgb565(void)
{
    canvas_draw("rgb565", LV_COLOR_FORMAT_RGB565);
}

void test_argb8888_additive_blend_to_rgb565(void)
{
    enum {
        TEST_W = 4,
        TEST_H = 2,
        TEST_STRIDE_PX = 6,
    };

    uint16_t dest[TEST_H][TEST_STRIDE_PX] = {
        {
            make_rgb565(0x10, 0x20, 0x30),
            make_rgb565(0x21, 0x32, 0x43),
            make_rgb565(0x40, 0x50, 0x60),
            make_rgb565(0xf0, 0xe0, 0xd0),
            0xa55a,
            0x5aa5,
        },
        {
            make_rgb565(0x01, 0x23, 0x45),
            make_rgb565(0x67, 0x89, 0xab),
            make_rgb565(0xcd, 0xef, 0x12),
            make_rgb565(0x34, 0x56, 0x78),
            0x33cc,
            0xcc33,
        },
    };
    const uint16_t original[TEST_H][TEST_STRIDE_PX] = {
        {
            dest[0][0],
            dest[0][1],
            dest[0][2],
            dest[0][3],
            dest[0][4],
            dest[0][5],
        },
        {
            dest[1][0],
            dest[1][1],
            dest[1][2],
            dest[1][3],
            dest[1][4],
            dest[1][5],
        },
    };
    lv_color32_t src[TEST_H][TEST_STRIDE_PX] = {
        {
            { .blue = 0x40, .green = 0x20, .red = 0x10, .alpha = 0x00 },
            { .blue = 0x00, .green = 0x00, .red = 0x00, .alpha = 0xff },
            { .blue = 0x30, .green = 0x60, .red = 0x90, .alpha = 0x80 },
            { .blue = 0xff, .green = 0xff, .red = 0xff, .alpha = 0xff },
            { .blue = 0x7e, .green = 0x7e, .red = 0x7e, .alpha = 0x7e },
            { .blue = 0x7e, .green = 0x7e, .red = 0x7e, .alpha = 0x7e },
        },
        {
            { .blue = 0x18, .green = 0x28, .red = 0x38, .alpha = 0xff },
            { .blue = 0x48, .green = 0x58, .red = 0x68, .alpha = 0x40 },
            { .blue = 0x00, .green = 0x10, .red = 0x20, .alpha = 0x00 },
            { .blue = 0x90, .green = 0xa0, .red = 0xb0, .alpha = 0xff },
            { .blue = 0x7e, .green = 0x7e, .red = 0x7e, .alpha = 0x7e },
            { .blue = 0x7e, .green = 0x7e, .red = 0x7e, .alpha = 0x7e },
        },
    };

    lv_draw_sw_blend_image_dsc_t dsc;
    lv_memzero(&dsc, sizeof(dsc));
    dsc.dest_buf = dest;
    dsc.dest_w = TEST_W;
    dsc.dest_h = TEST_H;
    dsc.dest_stride = sizeof(dest[0]);
    dsc.src_buf = src;
    dsc.src_stride = sizeof(src[0]);
    dsc.src_color_format = LV_COLOR_FORMAT_ARGB8888;
    dsc.opa = LV_OPA_COVER;
    dsc.blend_mode = LV_BLEND_MODE_ADDITIVE;

    lv_draw_sw_blend_image_to_rgb565(&dsc);

    TEST_ASSERT_EQUAL_HEX16(add_argb8888_to_rgb565(original[0][0], src[0][0]), dest[0][0]);
    TEST_ASSERT_EQUAL_HEX16(add_argb8888_to_rgb565(original[0][1], src[0][1]), dest[0][1]);
    TEST_ASSERT_EQUAL_HEX16(add_argb8888_to_rgb565(original[0][2], src[0][2]), dest[0][2]);
    TEST_ASSERT_EQUAL_HEX16(add_argb8888_to_rgb565(original[0][3], src[0][3]), dest[0][3]);
    TEST_ASSERT_EQUAL_HEX16(add_argb8888_to_rgb565(original[1][0], src[1][0]), dest[1][0]);
    TEST_ASSERT_EQUAL_HEX16(add_argb8888_to_rgb565(original[1][1], src[1][1]), dest[1][1]);
    TEST_ASSERT_EQUAL_HEX16(add_argb8888_to_rgb565(original[1][2], src[1][2]), dest[1][2]);
    TEST_ASSERT_EQUAL_HEX16(add_argb8888_to_rgb565(original[1][3], src[1][3]), dest[1][3]);

    TEST_ASSERT_EQUAL_HEX16(original[0][4], dest[0][4]);
    TEST_ASSERT_EQUAL_HEX16(original[0][5], dest[0][5]);
    TEST_ASSERT_EQUAL_HEX16(original[1][4], dest[1][4]);
    TEST_ASSERT_EQUAL_HEX16(original[1][5], dest[1][5]);
}

#endif
