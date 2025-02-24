#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

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

    lv_draw_buf_t * draw_buf = lv_draw_buf_create(500, 360, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    lv_draw_buf_clear(draw_buf, NULL);
    lv_canvas_set_draw_buf(canvas, draw_buf);

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

#ifndef NON_AMD64_BUILD
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/letter_0.png");
#endif

    canvas_destroy(canvas);
}

#endif
