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


static void small_canvas_render(const char * name_sub, lv_color_format_t cf, uint32_t idx)
{
    static LV_ATTRIBUTE_MEM_ALIGN uint8_t small_canvas_buf[4][LV_TEST_WIDTH_TO_STRIDE(180, 4) * 180 + LV_DRAW_BUF_ALIGN];

    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_buffer(canvas, small_canvas_buf[idx], 180, 180, cf);
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

    lv_area_t label1_coords = {10, 30, 170, 150};
    lv_draw_label(&layer, &label_dsc, &label1_coords);

    /*Blur the middle of the canvas*/
    lv_draw_blur_dsc_t blur_dsc;
    lv_draw_blur_dsc_init(&blur_dsc);
    blur_dsc.corner_radius = 10;
    blur_dsc.blur_radius = 8;

    lv_area_t fill_coords = {40, 40, 140, 140};
    lv_draw_blur(&layer, &blur_dsc, &fill_coords);

    /*Draw a semi-transparent rectangle on the blurred area*/
    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.color = lv_palette_lighten(LV_PALETTE_BLUE, 1);
    fill_dsc.radius = 10;
    fill_dsc.opa = LV_OPA_30;

    lv_draw_fill(&layer, &fill_dsc, &fill_coords);

    /*Add label on the blurred area*/
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_black();
    label_dsc.font = &lv_font_montserrat_14;
    label_dsc.align = LV_TEXT_ALIGN_CENTER;
    label_dsc.text = name_sub;

    lv_area_t label2_coords = {50, 50, 130, 130};
    lv_draw_label(&layer, &label_dsc, &label2_coords);

    lv_canvas_finish_layer(canvas, &layer);
}

void test_blur(void)
{
    small_canvas_render("rgb565", LV_COLOR_FORMAT_RGB565, 0);
    small_canvas_render("rgb888", LV_COLOR_FORMAT_RGB888, 1);
    small_canvas_render("xrgb8888", LV_COLOR_FORMAT_XRGB8888, 2);
    small_canvas_render("argb8888", LV_COLOR_FORMAT_ARGB8888, 3);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/draw_blur.png");

}

#endif
