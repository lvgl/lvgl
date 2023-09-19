#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
    lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_scr_act(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);
}

void tearDown(void)
{
    /* Function run after every test */
}

static void draw_triangles(lv_layer_t * layer, lv_draw_triangle_dsc_t * dsc, uint32_t line)
{
    lv_point_t p[][3] = {
        {{0, 0},  {0, 50}, {50, 50}},    /* |\ */
        {{0, 0},  {0, 50}, {50, 0}},     /* |/ */
        {{0, 0},  {50, 0}, {50, 50}},    /* \| */
        {{0, 50}, {50, 0}, {50, 50}},    /* /| */

        {{0, 0},  {50, 0},  {25, 50}},   /* \/ */
        {{0, 50}, {50, 50}, {25, 0}},    /* /\ */
        {{0, 0},  {0, 50},  {50, 25}},   /* > */
        {{0, 25}, {50, 0},  {50, 50}},   /* < */

        /*Same triangle but different order*/
        {{0, 0},  {50, 20}, {35, 50}},

        /*Special case*/
        {{0, 0},  {50, 0},  {25, 1}},
        {{0, 0},  {0, 50},  {1, 25}},
        {{0, 0},  {50, 50}, {25, 26}},
    };


    uint32_t i;
    for(i = 0; i < sizeof(p) / sizeof(p[0]); i++) {
        dsc->p[0].x = p[i][0].x + i * 60 + 10;
        dsc->p[1].x = p[i][1].x + i * 60 + 10;
        dsc->p[2].x = p[i][2].x + i * 60 + 10;

        dsc->p[0].y = p[i][0].y + line * 55 + 5;
        dsc->p[1].y = p[i][1].y + line * 55 + 5;
        dsc->p[2].y = p[i][2].y + line * 55 + 5;

        lv_draw_triangle(layer, dsc);
    }

}

void test_draw_triangle(void)
{
    static uint8_t canvas_buf[760 * 440 * 4];


    lv_obj_t * canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, canvas_buf, 760, 440, LV_COLOR_FORMAT_ARGB8888);

    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_RED, 2), LV_OPA_50);

    lv_draw_triangle_dsc_t tri_dsc;
    lv_draw_triangle_dsc_init(&tri_dsc);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    tri_dsc.bg_color = lv_palette_main(LV_PALETTE_BLUE);
    draw_triangles(&layer, &tri_dsc, 0);

    tri_dsc.bg_opa = LV_OPA_50;
    draw_triangles(&layer, &tri_dsc, 1);

    tri_dsc.bg_opa = LV_OPA_COVER;
    tri_dsc.bg_grad.stops_count = 2;
    tri_dsc.bg_grad.dir = LV_GRAD_DIR_HOR;
    tri_dsc.bg_grad.stops[0].color = lv_palette_main(LV_PALETTE_BLUE);
    tri_dsc.bg_grad.stops[0].opa = LV_OPA_COVER;
    tri_dsc.bg_grad.stops[0].frac = 64;
    tri_dsc.bg_grad.stops[1].color = lv_palette_main(LV_PALETTE_GREEN);
    tri_dsc.bg_grad.stops[1].opa = LV_OPA_0;
    draw_triangles(&layer, &tri_dsc, 2);

    tri_dsc.bg_opa = LV_OPA_50;
    draw_triangles(&layer, &tri_dsc, 3);

    tri_dsc.bg_opa = LV_OPA_COVER;
    tri_dsc.bg_grad.dir = LV_GRAD_DIR_VER;
    draw_triangles(&layer, &tri_dsc, 4);

    tri_dsc.bg_opa = LV_OPA_50;
    draw_triangles(&layer, &tri_dsc, 5);

    lv_canvas_finish_layer(canvas, &layer);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/triangle.png");
}


#endif
