#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES


#define USE_RADIAL_GRADIENT

/**
 * Using complex gradient (radial)
 */
void lv_example_style_16(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, 5);

    /*Make a radial gradient*/
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    static lv_grad_dsc_t grad;
#ifdef USE_RADIAL_GRADIENT
    grad.dir = LV_GRAD_DIR_RADIAL;
#else
    grad.dir = LV_GRAD_DIR_HOR;
#endif
    grad.stops_count = 2;
    grad.stops[0].color = lv_color_hex(0x189B42);
    grad.stops[0].opa = LV_OPA_COVER;
    grad.stops[0].frac = 0;
    grad.stops[1].color = lv_color_hex(0x184D9B);
    grad.stops[1].opa = LV_OPA_COVER;
    grad.stops[1].frac = 255;
    /*Radial gradient parameters: start and end circle*/
#if 0	/* concentric circles */
    grad.r.start.x = 400;
    grad.r.start.y = 600;
    grad.r.start_radius = 0;
    grad.r.end.x = 400;
    grad.r.end.y = 600;
    grad.r.end_radius = 150;
    grad.extend = LV_GRAD_EXTEND_PAD;
#elif 0	/* degenerate case: start circle touches the end circle from inside */
    grad.r.start.x = 800 - 20;
    grad.r.start.y = 600;
    grad.r.start_radius = 20;
    grad.r.end.x = 800 - 150;
    grad.r.end.y = 600;
    grad.r.end_radius = 150;
    grad.extend = LV_GRAD_EXTEND_REPEAT;
//    grad.extend = LV_GRAD_EXTEND_REFLECT;
#elif 0	/* start and end circles are the same size */
    grad.r.start.x = 200;
    grad.r.start.y = 600;
    grad.r.start_radius = 150;
    grad.r.end.x = 600;
    grad.r.end.y = 600;
    grad.r.end_radius = 150;
    grad.extend = LV_GRAD_EXTEND_REPEAT;
#elif 0	/* generic case */
    grad.r.start.x = 450;
    grad.r.start.y = 650;
    grad.r.start_radius = 0;
    grad.r.end.x = 400;
    grad.r.end.y = 600;
    grad.r.end_radius = 150;
//    grad.extend = LV_GRAD_EXTEND_PAD;
    grad.extend = LV_GRAD_EXTEND_REPEAT;
//    grad.extend = LV_GRAD_EXTEND_REFLECT;
#elif 1	/* concentric, center in the top left corner */
    grad.r.start.x = 0;
    grad.r.start.y = 0;
    grad.r.start_radius = 0;
    grad.r.end.x = 0;
    grad.r.end.y = 0;
    grad.r.end_radius = 1500;
    grad.extend = LV_GRAD_EXTEND_PAD;
#endif

    lv_style_set_bg_grad(&style, &grad);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj, &style, 0);
	lv_obj_set_size(obj, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
    lv_obj_center(obj);
}

#endif
