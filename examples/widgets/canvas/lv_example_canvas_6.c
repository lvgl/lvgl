#include "../../lv_examples.h"
#if LV_USE_CANVAS && LV_BUILD_EXAMPLES

#if 1

#define CANVAS_WIDTH  200
#define CANVAS_HEIGHT  (9*40+10)
#define DEMO_IMAGE_RGB img_star_grayscale_rgb
#define DEMO_IMAGE_ARGB img_star_grayscale_argb
#define DEMO_IMAGE_L8 img_star_grayscale_l8
#define IMAGE_OPA       LV_OPA_10
#define RECOLOR_OPA     LV_OPA_0
#define RECOLOR_COLOR   0xff0

/**
 * Draw an image to the canvas
 */
void lv_example_canvas_6(void)
{
    /*Create a buffer for the canvas*/
    //LV_DRAW_BUF_DEFINE(draw_buf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_AL88);
    LV_DRAW_BUF_DEFINE(draw_buf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_ARGB8888);

    /*Create a canvas and initialize its palette*/
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_draw_buf(canvas, &draw_buf);
    lv_canvas_fill_bg(canvas, lv_color_hex3(0xccc), LV_OPA_COVER);
    lv_obj_center(canvas);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    LV_IMAGE_DECLARE(DEMO_IMAGE_ARGB);
    LV_IMAGE_DECLARE(DEMO_IMAGE_RGB);
    LV_IMAGE_DECLARE(DEMO_IMAGE_L8);

    lv_draw_image_dsc_t dsc;

    lv_draw_image_dsc_init(&dsc);
    dsc.opa = IMAGE_OPA;
    dsc.recolor = lv_color_hex3(RECOLOR_COLOR);
    dsc.rotation = 300;
    dsc.pivot.x = 0;
    dsc.pivot.y = 0;

    lv_area_t coords1 = { 20, 10, 20 + DEMO_IMAGE_RGB.header.w - 1, 10 + DEMO_IMAGE_RGB.header.h - 1 };
    lv_area_t coords2 = { 70, 10, 70 + DEMO_IMAGE_RGB.header.w - 1, 10 + DEMO_IMAGE_RGB.header.h - 1 };
    lv_area_t coords3 = { 120, 10, 120 + DEMO_IMAGE_RGB.header.w - 1, 10 + DEMO_IMAGE_RGB.header.h - 1 };

    for(int i = 0; i <= 256; i += 32) {

        dsc.recolor_opa = RECOLOR_OPA;
        dsc.opa = LV_MIN(i, 255);

        dsc.src = &DEMO_IMAGE_ARGB;
        lv_draw_image(&layer, &dsc, &coords1);

        dsc.src = &DEMO_IMAGE_RGB;
        lv_draw_image(&layer, &dsc, &coords2);

        dsc.src = &DEMO_IMAGE_L8;
        lv_draw_image(&layer, &dsc, &coords3);

        coords1.y1 += 40;
        coords1.y2 += 40;
        coords2.y1 += 40;
        coords2.y2 += 40;
        coords3.y1 += 40;
        coords3.y2 += 40;
    }

    lv_canvas_finish_layer(canvas, &layer);
}

#else

#define CANVAS_WIDTH  150
#define CANVAS_HEIGHT  150
#define DEMO_IMAGE_RGB img_cogwheel_argb //img_lenna_rgb565
//#define DEMO_IMAGE_L8 img_lenna_l8
#define IMAGE_OPA       LV_OPA_50
#define RECOLOR_OPA     LV_OPA_0
#define RECOLOR_COLOR   0xff0

/**
 * Draw an image to the canvas
 */
void lv_example_canvas_6(void)
{
    /*Create a buffer for the canvas*/
    LV_DRAW_BUF_DEFINE(draw_buf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_RGB888);

    /*Create a canvas and initialize its palette*/
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_draw_buf(canvas, &draw_buf);
    lv_canvas_fill_bg(canvas, lv_color_hex3(0xccc), LV_OPA_COVER);
    lv_obj_center(canvas);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    LV_IMAGE_DECLARE(DEMO_IMAGE_RGB);
    //    LV_IMAGE_DECLARE(DEMO_IMAGE_L8);

    lv_draw_image_dsc_t dsc;

    lv_draw_image_dsc_init(&dsc);
    dsc.opa = IMAGE_OPA;
    dsc.recolor = lv_color_hex3(RECOLOR_COLOR);
    dsc.recolor_opa = RECOLOR_OPA;
    lv_area_t coords1 = { 10, 10, 10 + DEMO_IMAGE_RGB.header.w - 1, 10 + DEMO_IMAGE_RGB.header.h - 1 };
    //    lv_area_t coords2 = { 60, 10, 60 + DEMO_IMAGE_RGB.header.w - 1, 10 + DEMO_IMAGE_RGB.header.h - 1 };

    dsc.src = &DEMO_IMAGE_RGB;
    lv_draw_image(&layer, &dsc, &coords1);

    //    dsc.src = &DEMO_IMAGE_L8;
    //    lv_draw_image(&layer, &dsc, &coords2);

    lv_canvas_finish_layer(canvas, &layer);
}

#endif

#endif

