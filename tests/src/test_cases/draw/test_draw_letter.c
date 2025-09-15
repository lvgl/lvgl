#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#ifndef NON_AMD64_BUILD
    #define TEST_ASSERT_EQUAL_LETTER_SCREENSHOT(path) TEST_ASSERT_EQUAL_SCREENSHOT(path)
#else
    #define TEST_ASSERT_EQUAL_LETTER_SCREENSHOT(path) LV_UNUSED(path)
#endif

void setUp(void)
{
    /* Function run before every test */
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static void on_canvas_delete(lv_event_t * e)
{
    lv_obj_t * canvas = lv_event_get_current_target(e);
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(canvas);
    TEST_ASSERT_NOT_NULL(draw_buf);
    lv_draw_buf_destroy(draw_buf);
}

static lv_obj_t * canvas_create(uint32_t w, uint32_t h)
{
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_obj_set_size(canvas, w, h);

    lv_draw_buf_t * draw_buf = lv_draw_buf_create(w, h, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    lv_draw_buf_clear(draw_buf, NULL);
    lv_canvas_set_draw_buf(canvas, draw_buf);

    lv_obj_add_event_cb(canvas, on_canvas_delete, LV_EVENT_DELETE, NULL);
    return canvas;
}

void test_draw_sin_wave(void)
{
    const char * string = "lol~ I'm wavvvvvvving~";
    const uint32_t string_len = lv_strlen(string);

    LV_FONT_DECLARE(test_font_montserrat_ascii_4bpp);
    lv_obj_t * canvas = canvas_create(500, 360);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_letter_dsc_t letter_dsc;
    lv_draw_letter_dsc_init(&letter_dsc);
    letter_dsc.color = lv_color_hex(0xff0000);
    letter_dsc.font = &test_font_montserrat_ascii_4bpp;

    {
#define CURVE1_X(t) (t * 2 + 20)
#define CURVE1_Y(t) (lv_trigo_sin(t) * 40 / 32767 + 80)
        int32_t pre_x = CURVE1_X(-1);
        int32_t pre_y = CURVE1_Y(-1);

        for(int16_t i = 0; i < 30; i++) {
            const int32_t angle = i * 10;
            const int32_t x = CURVE1_X(angle);
            const int32_t y = CURVE1_Y(angle);
            letter_dsc.unicode = (uint32_t)string[i % string_len];
            letter_dsc.rotation = lv_atan2(y - pre_y, x - pre_x);
            letter_dsc.rotation = (letter_dsc.rotation > 180 ? letter_dsc.rotation - 360 : letter_dsc.rotation) * 5;
            lv_draw_letter(&layer, &letter_dsc, &(lv_point_t) {
                .x = x, .y = y
            });
            pre_x = x;
            pre_y = y;
        }
    }

    {
#define CURVE2_X(t) (t * 3 + 20)
#define CURVE2_Y(t) (lv_trigo_sin((t) * 4) * 40 / 32767 + 230)

        int32_t pre_x = CURVE2_X(-1);
        int32_t pre_y = CURVE2_Y(-1);
        for(int16_t i = 0; i < 30; i++) {
            const int32_t angle = i * 5;
            const int32_t x = CURVE2_X(angle);
            const int32_t y = CURVE2_Y(angle);

            letter_dsc.unicode = (uint32_t)string[i % string_len];
            letter_dsc.rotation = lv_atan2(y - pre_y, x - pre_x) * 10;
            letter_dsc.color = lv_color_hsv_to_rgb(i * 10, 100, 100);
            lv_draw_letter(&layer, &letter_dsc, &(lv_point_t) {
                .x = x, .y = y
            });

            pre_x = x;
            pre_y = y;
        }
    }

    lv_canvas_finish_layer(canvas, &layer);

    TEST_ASSERT_EQUAL_LETTER_SCREENSHOT("draw/letter_0.png");

    lv_obj_delete(canvas);
}

static void draw_letter_with_rotation(lv_obj_t * canvas, lv_layer_t * layer, uint32_t unicode, int32_t rotation,
                                      int32_t x, int32_t y)
{
    lv_draw_letter_dsc_t letter_dsc;
    lv_draw_letter_dsc_init(&letter_dsc);
    letter_dsc.color = lv_color_hex(0xff0000);
    letter_dsc.font = lv_obj_get_style_text_font(canvas, 0);
    letter_dsc.unicode = unicode;
    letter_dsc.rotation = rotation;
    lv_draw_letter(layer, &letter_dsc, &(lv_point_t) {
        .x = x, .y = y
    });
}

static void test_draw_letter(lv_freetype_font_render_mode_t render_mode, uint32_t unicode, int32_t rotation,
                             const char * ref_img_path)
{
    lv_obj_t * canvas = canvas_create(240, 240);

    lv_font_t * font_normal = lv_freetype_font_create("./src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                                      render_mode,
                                                      80,
                                                      LV_FREETYPE_FONT_STYLE_NORMAL);
    TEST_ASSERT_NOT_NULL(font_normal);
    lv_obj_set_style_text_font(canvas, font_normal, 0);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_COVER);

    /* drawing letter with clipping */
    lv_area_t clip_area;
    lv_area_set(&clip_area, 40, 40, 200 - 1, 200 - 1);
    layer._clip_area = clip_area;

    const int32_t offset_x = 40;
    const int32_t offset_y = 70;

    for(int i = 0; i < 9; i++) {
        draw_letter_with_rotation(canvas, &layer, unicode, rotation, (i % 3) * 80 + offset_x, (i / 3) * 80 + offset_y);
    }

    lv_draw_border_dsc_t draw_border_dsc;
    lv_draw_border_dsc_init(&draw_border_dsc);
    draw_border_dsc.width = 1;
    draw_border_dsc.color = lv_color_black();
    lv_draw_border(&layer, &draw_border_dsc, &clip_area);

    lv_canvas_finish_layer(canvas, &layer);

    TEST_ASSERT_EQUAL_LETTER_SCREENSHOT(ref_img_path);

    lv_obj_delete(canvas);
    lv_freetype_font_delete(font_normal);
}

void test_draw_letter_bitmap(void)
{
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 'A', 0, "draw/letter_bitmap_A_rotated_0.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 'A', 450, "draw/letter_bitmap_A_rotated_45.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 'A', 900, "draw/letter_bitmap_A_rotated_90.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 'A', 1800, "draw/letter_bitmap_A_rotated_180.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 'g', 0, "draw/letter_bitmap_g_rotated_0.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 'g', 450, "draw/letter_bitmap_g_rotated_45.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 'g', 900, "draw/letter_bitmap_g_rotated_90.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 'g', 1800, "draw/letter_bitmap_g_rotated_180.png");
}

void test_draw_letter_outline(void)
{
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_OUTLINE, 'A', 0, "draw/letter_outline_A_rotated_0.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_OUTLINE, 'A', 450, "draw/letter_outline_A_rotated_45.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_OUTLINE, 'A', 900, "draw/letter_outline_A_rotated_90.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_OUTLINE, 'A', 1800, "draw/letter_outline_A_rotated_180.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_OUTLINE, 'g', 0, "draw/letter_outline_g_rotated_0.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_OUTLINE, 'g', 450, "draw/letter_outline_g_rotated_45.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_OUTLINE, 'g', 900, "draw/letter_outline_g_rotated_90.png");
    test_draw_letter(LV_FREETYPE_FONT_RENDER_MODE_OUTLINE, 'g', 1800, "draw/letter_outline_g_rotated_180.png");
}

#endif
