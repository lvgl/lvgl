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

#define CANVAS_WIDTH 180
#define CANVAS_HEIGHT 100


static void small_canvas_render(const char * name_sub, lv_color_format_t cf, void * canvas_buf, int32_t blur_radius,
                                uint32_t corner_radius)
{
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_buffer(canvas,    canvas_buf, CANVAS_WIDTH, CANVAS_HEIGHT, cf);
    lv_canvas_fill_bg(canvas, lv_color_hex3(0xccc), LV_OPA_COVER);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    /*A label in the background*/
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_palette_main(LV_PALETTE_RED);
    label_dsc.font = &lv_font_montserrat_14;
    label_dsc.decor = LV_TEXT_DECOR_UNDERLINE;
    label_dsc.align = LV_TEXT_ALIGN_CENTER;
    label_dsc.text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
                     "Curabitur sed velit sed neque tristique sagittis vel et sapien.";

    lv_area_t label1_coords = {10, 10, CANVAS_WIDTH - 10, CANVAS_HEIGHT - 10};
    lv_draw_label(&layer, &label_dsc, &label1_coords);

    /*Blur the middle of the canvas*/
    lv_draw_blur_dsc_t blur_dsc;
    lv_draw_blur_dsc_init(&blur_dsc);
    blur_dsc.corner_radius = corner_radius;
    blur_dsc.blur_radius = blur_radius;

    lv_area_t fill_coords = {40, 20, CANVAS_WIDTH - 40, CANVAS_HEIGHT - 20};
    lv_draw_blur(&layer, &blur_dsc, &fill_coords);

    /*Draw a semi-transparent rectangle on the blurred area*/
    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.color = lv_palette_lighten(LV_PALETTE_BLUE, 1);
    fill_dsc.radius = corner_radius;
    fill_dsc.opa = LV_OPA_30;

    lv_draw_fill(&layer, &fill_dsc, &fill_coords);

    /*Add label on the blurred area*/
    char buf[128];
    lv_snprintf(buf, sizeof(buf), "%s\nblur:%d", name_sub, blur_radius);

    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_black();
    label_dsc.font = &lv_font_montserrat_14;
    label_dsc.align = LV_TEXT_ALIGN_CENTER;
    label_dsc.text = buf;

    lv_area_t label2_coords = {50, 30, CANVAS_WIDTH - 50, CANVAS_HEIGHT - 30};
    lv_draw_label(&layer, &label_dsc, &label2_coords);

    lv_canvas_finish_layer(canvas, &layer);
}

void test_blur(void)
{
    static LV_ATTRIBUTE_MEM_ALIGN uint8_t canvas_buf[16][LV_TEST_WIDTH_TO_STRIDE(CANVAS_WIDTH,
                                                                                 4) * CANVAS_HEIGHT + LV_DRAW_BUF_ALIGN];

    uint32_t corner_radius_options[4] = {0, 4, 16, 32};
    uint32_t r;

    for(r = 0; r < 4; r++) {
        lv_obj_clean(lv_screen_active());

        uint32_t radius_current = corner_radius_options[r];
        small_canvas_render("rgb565", LV_COLOR_FORMAT_RGB565, canvas_buf[0], 6, radius_current);
        small_canvas_render("rgb888", LV_COLOR_FORMAT_RGB888, canvas_buf[1], 6, radius_current);
        small_canvas_render("xrgb8888", LV_COLOR_FORMAT_XRGB8888, canvas_buf[2], 6, radius_current);
        small_canvas_render("argb8888", LV_COLOR_FORMAT_ARGB8888, canvas_buf[3], 6, radius_current);

        small_canvas_render("rgb565", LV_COLOR_FORMAT_RGB565, canvas_buf[4], 12, radius_current);
        small_canvas_render("rgb888", LV_COLOR_FORMAT_RGB888, canvas_buf[5], 12, radius_current);
        small_canvas_render("xrgb8888", LV_COLOR_FORMAT_XRGB8888, canvas_buf[6], 12, radius_current);
        small_canvas_render("argb8888", LV_COLOR_FORMAT_ARGB8888, canvas_buf[7], 12, radius_current);

        small_canvas_render("rgb565", LV_COLOR_FORMAT_RGB565, canvas_buf[8], 24, radius_current);
        small_canvas_render("rgb888", LV_COLOR_FORMAT_RGB888, canvas_buf[9], 24, radius_current);
        small_canvas_render("xrgb8888", LV_COLOR_FORMAT_XRGB8888, canvas_buf[10], 24, radius_current);
        small_canvas_render("argb8888", LV_COLOR_FORMAT_ARGB8888, canvas_buf[11], 24, radius_current);

        small_canvas_render("rgb565", LV_COLOR_FORMAT_RGB565, canvas_buf[12], 60, radius_current);
        small_canvas_render("rgb888", LV_COLOR_FORMAT_RGB888, canvas_buf[13], 60, radius_current);
        small_canvas_render("xrgb8888", LV_COLOR_FORMAT_XRGB8888, canvas_buf[14], 60, radius_current);
        small_canvas_render("argb8888", LV_COLOR_FORMAT_ARGB8888, canvas_buf[15], 60, radius_current);

        char buf[128];
        lv_snprintf(buf, sizeof(buf), "draw/draw_blur_corner_%u.png", radius_current);
        TEST_ASSERT_EQUAL_SCREENSHOT(buf);
    }
}

#endif
