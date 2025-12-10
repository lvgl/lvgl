#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"


#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_draw_drop_shadow(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_drop_shadow_opa(&style, 255);
    lv_style_set_drop_shadow_offset_x(&style, 5);
    lv_style_set_drop_shadow_offset_y(&style, 10);
    lv_style_set_drop_shadow_radius(&style, 16);
    lv_style_set_drop_shadow_color(&style, lv_color_hex(0x0000ff));

    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);

    lv_obj_set_style_pad_all(scr, 32, 0);
    lv_obj_set_style_pad_row(scr, 32, 0);
    lv_obj_set_style_pad_column(scr, 32, 0);

    lv_obj_t * arc = lv_arc_create(lv_screen_active());
    lv_arc_set_value(arc, 70);
    lv_obj_add_style(arc, &style, LV_PART_INDICATOR);
    lv_obj_center(arc);

    LV_IMAGE_DECLARE(img_benchmark_lvgl_logo_argb);
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &img_benchmark_lvgl_logo_argb);
    lv_obj_add_style(img, &style, LV_PART_MAIN);
    lv_obj_set_y(img, 300);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj, &style, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(obj, LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_outline_pad(obj, 10, LV_PART_MAIN);
    lv_obj_set_style_outline_width(obj, 2, LV_PART_MAIN);

    lv_obj_t * chart;
    chart = lv_chart_create(lv_screen_active());
    lv_obj_set_size(chart, 200, 150);
    lv_obj_add_style(chart, &style, LV_PART_ITEMS);
    lv_obj_set_style_line_width(chart, 4, LV_PART_ITEMS);

    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);

    lv_rand_set_seed(0x1234ABCD);
    uint32_t i;
    for(i = 0; i < 10; i++) {
        lv_chart_set_next_value(chart, ser1, (int32_t)lv_rand(10, 50));
    }

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_style(label, &style, LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
    lv_label_set_text(label, "Hello world");

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/draw_drop_shadow.png");
}

#endif
