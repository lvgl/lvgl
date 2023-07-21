#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

static void radial_lines_create(lv_style_t * style, uint32_t line_index)
{
    uint32_t length = 90;
    lv_point_t pivot = {100, 100};
    static lv_point_t p[24][2];

    uint32_t i;
    for(i = 0; i < 24; i ++) {
        lv_obj_t * line = lv_line_create(lv_scr_act());
        if(style) lv_obj_add_style(line, style, 0);
        p[i][0].x = 140;
        p[i][0].y = 100;
        p[i][1].x = 100 + length;
        p[i][1].y = 100;

        lv_point_transform(&p[i][0], i * 150, 256, &pivot);
        lv_point_transform(&p[i][1], i * 150, 256, &pivot);

        lv_line_set_points(line, p[i], 2);
        lv_obj_set_pos(line, line_index % 4 * 200, line_index / 4 * 200);
    }
}

void test_lines_radial(void)
{

    static lv_style_t style_thin;
    lv_style_init(&style_thin);
    lv_style_set_line_color(&style_thin, lv_palette_main(LV_PALETTE_RED));
    radial_lines_create(&style_thin, 0);

    static lv_style_t style_thin_round;
    lv_style_init(&style_thin_round);
    lv_style_set_line_color(&style_thin_round, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_line_rounded(&style_thin_round, true);
    radial_lines_create(&style_thin_round, 1);

    static lv_style_t style_thin_opa;
    lv_style_init(&style_thin_opa);
    lv_style_set_line_color(&style_thin_opa, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_line_opa(&style_thin_opa, LV_OPA_50);
    radial_lines_create(&style_thin_opa, 2);

    static lv_style_t style_thin_opa_rounded;
    lv_style_init(&style_thin_opa_rounded);
    lv_style_set_line_color(&style_thin_opa_rounded, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_line_opa(&style_thin_opa_rounded, LV_OPA_50);
    radial_lines_create(&style_thin_opa_rounded, 3);


    static lv_style_t style_tick;
    lv_style_init(&style_tick);
    lv_style_set_line_color(&style_tick, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_line_width(&style_tick, 8);
    radial_lines_create(&style_tick, 4);

    static lv_style_t style_tick_round;
    lv_style_init(&style_tick_round);
    lv_style_set_line_color(&style_tick_round, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_line_rounded(&style_tick_round, true);
    lv_style_set_line_width(&style_tick_round, 8);
    radial_lines_create(&style_tick_round, 5);

    static lv_style_t style_tick_opa;
    lv_style_init(&style_tick_opa);
    lv_style_set_line_color(&style_tick_opa, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_line_opa(&style_tick_opa, LV_OPA_50);
    lv_style_set_line_width(&style_tick_opa, 8);
    radial_lines_create(&style_tick_opa, 6);

    static lv_style_t style_tick_opa_rounded;
    lv_style_init(&style_tick_opa_rounded);
    lv_style_set_line_color(&style_tick_opa_rounded, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_line_opa(&style_tick_opa_rounded, LV_OPA_50);
    lv_style_set_line_rounded(&style_tick_opa_rounded, true);
    lv_style_set_line_width(&style_tick_opa_rounded, 8);
    radial_lines_create(&style_tick_opa_rounded, 7);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/line_radial.png");
}

#endif
