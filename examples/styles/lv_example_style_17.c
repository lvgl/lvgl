#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS

/**
 * Using radial gradient as background with a repeating pattern
 */
void lv_example_style_17(void)
{
    int32_t width = lv_display_get_horizontal_resolution(NULL);
    int32_t height = lv_display_get_vertical_resolution(NULL);

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, 5);

    /*Make a radial gradient*/
    lv_style_set_bg_opa(&style, LV_OPA_COVER);

    /*First define a color gradient*/
    static lv_grad_dsc_t grad;
    grad.stops_count =
        2;                         /*two stops (to increase the number of stops you have to change LV_GRADIENT_MAX_STOPS in lv_conf.h)*/
    grad.stops[0].color = lv_color_hex(0x9B1842);   /*purple color*/
    grad.stops[0].opa = LV_OPA_COVER;               /*opaque*/
    grad.stops[0].frac =
        0;                         /*gradient stop at offset 0 sets the color at the border of the start circle*/
    grad.stops[1].color = lv_color_hex(0x000000);   /*black color*/
    grad.stops[1].opa = LV_OPA_COVER;               /*opaque*/
    grad.stops[1].frac =
        255;                       /*gradient stop at offset 255 sets the color at the border of the end circle*/

    /*Define radial gradient by the start and end limit circles. Here we use two limit circles with the start circle being within the end circle, but at an offset, and repeating the gradient outside the end circle (all coordinates are relative to the object)*/
    grad.dir = LV_GRAD_DIR_RADIAL;
    grad.params.radial.focal.x = lv_pct(50);                    /*start circle center x position*/
    grad.params.radial.focal.y = lv_pct(50);                    /*start circle center y position*/
    grad.params.radial.focal_extent.x = grad.params.radial.focal.x + 10;    /*start circle point x coordinate*/
    grad.params.radial.focal_extent.y = grad.params.radial.focal.y;    /*start circle point y coordinate*/
    grad.params.radial.end.x = grad.params.radial.focal.x - 20;             /*end circle center x position*/
    grad.params.radial.end.y = grad.params.radial.focal.y - 20;             /*end circle center y position*/
    grad.params.radial.end_extent.x = grad.params.radial.focal.x + 70;       /*end circle point x coordinate*/
    grad.params.radial.end_extent.y = grad.params.radial.focal.y;           /*end circle point y coordinate*/
    grad.extend = LV_GRAD_EXTEND_REPEAT;            /*repeat color pattern outside the end circle*/

    /*Set gradient as background*/
    lv_style_set_bg_grad(&style, &grad);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj, &style, 0);
    lv_obj_set_size(obj, width, height);
    lv_obj_center(obj);
}

#endif /*LV_USE_DRAW_SW_COMPLEX_GRADIENTS*/

#endif /*LV_BUILD_EXAMPLES*/
