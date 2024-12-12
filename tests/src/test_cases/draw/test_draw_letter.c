#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "math.h"

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
    lv_obj_clean(lv_screen_active());
}

static lv_obj_t * canvas_create(void)
{
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_obj_set_size(canvas, 500, 360);
    lv_canvas_set_draw_buf(canvas, lv_draw_buf_create(500, 500, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO));

    return canvas;
}

static void canvas_destroy(lv_obj_t * canvas)
{
    lv_draw_buf_destroy(lv_canvas_get_draw_buf(canvas));
    lv_obj_delete(canvas);
}

void test_draw_sin_wave(void)
{
    const char * string = "lol~ I'm wavvvvvvving~";
    const uint32_t string_len = lv_strlen(string);

    LV_FONT_DECLARE(test_font_montserrat_ascii_4bpp);
    lv_obj_t * canvas = canvas_create();

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_letter_dsc_t letter_dsc;
    lv_draw_letter_dsc_init(&letter_dsc);
    letter_dsc.color = lv_color_hex(0xff0000);
    letter_dsc.font = &test_font_montserrat_ascii_4bpp;

    for(int16_t i = 0; i < 30; i++) {
        const int32_t x = i * 10 + 20;
        const int32_t y = (int32_t)(sin(i * M_PI / 10) * 40 + 150);
        letter_dsc.unicode = (uint32_t)string[i % string_len];
        lv_draw_letter(&layer, &letter_dsc, &(lv_point_t) {
            .x = x, .y = y
        });
    }

    for(int16_t i = 0; i < 30; i++) {
        const int32_t x = i * 15 + 20;
        const int32_t y = (int32_t)(sin(i * M_PI / 10) * 40 + 300);
        letter_dsc.unicode = (uint32_t)string[i % string_len];
        letter_dsc.rotation = (int32_t)(atan(cos(M_PI / 10 * i)) * 180 / M_PI) * 10;
        letter_dsc.color = lv_color_hsv_to_rgb(i * 10, 100, 100);
        lv_draw_letter(&layer, &letter_dsc, &(lv_point_t) {
            .x = x, .y = y
        });
    }

    lv_canvas_finish_layer(canvas, &layer);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/letter_0.png");

    canvas_destroy(canvas);
}

#endif
