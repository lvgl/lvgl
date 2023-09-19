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


static void draw_arcs_line(lv_layer_t * layer, lv_draw_arc_dsc_t * dsc, uint32_t line)
{
    dsc->center.x = 50;
    dsc->center.y = line * 80 + 50;
    dsc->radius = 30;
    dsc->start_angle = 0;
    dsc->end_angle = 30;
    lv_draw_arc(layer, dsc);

    dsc->center.x += 90;
    dsc->start_angle = 0;
    dsc->end_angle = 130;
    lv_draw_arc(layer, dsc);

    dsc->center.x += 90;
    dsc->start_angle = 0;
    dsc->end_angle = 230;
    lv_draw_arc(layer, dsc);

    dsc->center.x += 90;
    dsc->start_angle = 0;
    dsc->end_angle = 300;
    lv_draw_arc(layer, dsc);

    dsc->center.x += 90;
    dsc->start_angle = 150;
    dsc->end_angle = 300;
    lv_draw_arc(layer, dsc);

    dsc->center.x += 90;
    dsc->start_angle = 220;
    dsc->end_angle = 300;
    lv_draw_arc(layer, dsc);

    dsc->center.x += 90;
    dsc->start_angle = 310;
    dsc->end_angle = 60;
    lv_draw_arc(layer, dsc);

    dsc->center.x += 80;
    dsc->start_angle = 0;
    dsc->end_angle = 360;
    lv_draw_arc(layer, dsc);
}

static void draw_arcs(lv_draw_arc_dsc_t * arc_dsc, const char * fn)
{
    static uint8_t canvas_buf[760 * 440 * 4];

    lv_obj_t * canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, canvas_buf, 760, 440, LV_COLOR_FORMAT_ARGB8888);

    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_RED, 2), LV_OPA_50);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    arc_dsc->width = 1;
    arc_dsc->rounded = 0;
    arc_dsc->opa = LV_OPA_COVER;
    draw_arcs_line(&layer, arc_dsc, 0);

    arc_dsc->width = 10;
    draw_arcs_line(&layer, arc_dsc, 1);

    arc_dsc->width = 10;
    arc_dsc->rounded = 1;
    draw_arcs_line(&layer, arc_dsc, 2);

    arc_dsc->opa = LV_OPA_50;
    draw_arcs_line(&layer, arc_dsc, 3);

    arc_dsc->width = 50;
    arc_dsc->rounded = 0;
    arc_dsc->opa = LV_OPA_COVER;
    draw_arcs_line(&layer, arc_dsc, 4);

    lv_canvas_finish_layer(canvas, &layer);

    char fn_buf[64];
    lv_snprintf(fn_buf, sizeof(fn_buf), "draw/arc_%s.png", fn);
    TEST_ASSERT_EQUAL_SCREENSHOT(fn_buf);

    lv_obj_del(canvas);
}

void test_colored_arc(void)
{

    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_arc_dsc_init(&arc_dsc);

    arc_dsc.color = lv_palette_main(LV_PALETTE_BLUE);
    draw_arcs(&arc_dsc, "colored");
}

void test_image_arc(void)
{
    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_arc_dsc_init(&arc_dsc);

    LV_IMAGE_DECLARE(test_arc_bg);
    arc_dsc.img_src = &test_arc_bg;

    draw_arcs(&arc_dsc, "image");
}

#endif
