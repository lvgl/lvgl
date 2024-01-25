#if LV_BUILD_TEST
#include "../lvgl.h"
#include "lv_test_helpers.h"

#include "unity/unity.h"

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

    static uint8_t canvas_buf[CANVAS_WIDTH_TO_STRIDE(180, 4) * 180 + LV_DRAW_BUF_ALIGN];

    static uint8_t canvas2_buf[CANVAS_WIDTH_TO_STRIDE(768, 4) * 390 + LV_DRAW_BUF_ALIGN];
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

#endif
