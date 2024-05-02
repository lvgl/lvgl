#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES


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
    grad.dir = LV_GRAD_DIR_RADIAL;
    grad.stops_count = 2;
    grad.stops[0].color = lv_color_hex(0x189B42);
    grad.stops[0].opa = LV_OPA_COVER;
    grad.stops[0].frac = 0;
    grad.stops[1].color = lv_color_hex(0x184D9B);
    grad.stops[1].opa = LV_OPA_COVER;
    grad.stops[1].frac = 255;
    grad.extend = LV_GRAD_EXTEND_PAD,//LV_GRAD_EXTEND_REFLECT //LV_GRAD_EXTEND_PAD, //LV_GRAD_EXTEND_REPEAT,
    /*Radial gradient parameters: start and end circle*/
    grad.r.start.x = 100;
    grad.r.start.y = 0;
    grad.r.start_radius = 0;
    grad.r.end.x = 100;
    grad.r.end.y = 0;
    grad.r.end_radius = 150;

    lv_style_set_bg_grad(&style, &grad);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj, &style, 0);
    lv_obj_center(obj);
}

#endif
