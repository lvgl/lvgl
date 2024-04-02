#include "../../lv_examples.h"
#if LV_USE_CANVAS && LV_BUILD_EXAMPLES

#define CANVAS_WIDTH  200
#define CANVAS_HEIGHT 200

/**
 * Draw an arc to the canvas
 */
void lv_example_canvas_5(void)
{
    /*Create a buffer for the canvas*/
    //    LV_DRAW_BUF_DEFINE(draw_buf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_DEFINE(draw_buf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_L8);

    /*Create a canvas and initialize its palette*/
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_draw_buf(canvas, &draw_buf);
    lv_canvas_fill_bg(canvas, lv_color_hex3(0x0cc), LV_OPA_COVER);
    lv_obj_center(canvas);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_arc_dsc_t dsc;
    lv_draw_arc_dsc_init(&dsc);
    dsc.color = lv_palette_main(LV_PALETTE_RED);
    dsc.width = 5;
    dsc.center.x = CANVAS_WIDTH / 2;
    dsc.center.y = CANVAS_HEIGHT / 2;
    dsc.width = CANVAS_WIDTH / 5;
    dsc.radius = CANVAS_WIDTH / 2 - 10;
    dsc.start_angle = 0;
    dsc.end_angle = 220;
    dsc.opa = LV_OPA_100;

    lv_draw_arc(&layer, &dsc);

    lv_canvas_finish_layer(canvas, &layer);

}
#endif
