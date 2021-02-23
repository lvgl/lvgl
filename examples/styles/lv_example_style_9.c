#include "../../lvgl.h"
#if LV_BUILD_EXAMPLES && LV_USE_IMG

/**
 * Using the Image style properties
 */
void lv_example_style_9(void)
{
    static lv_style_t style;
    lv_style_init(&style);

    /*Set a background color and a radius*/
    lv_style_set_radius(&style, 5);
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    lv_style_set_bg_color(&style, lv_color_grey_lighten_3());
    lv_style_set_border_width(&style, 2);
    lv_style_set_border_color(&style, lv_color_blue());

//    lv_style_set_pad_all(&style, 10);

    lv_style_set_img_recolor(&style, lv_color_blue());
    lv_style_set_img_recolor_opa(&style, LV_OPA_50);
    lv_style_set_transform_angle(&style, 300);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_img_create(lv_scr_act(), NULL);
    lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &style);

    LV_IMG_DECLARE(img_cogwheel_argb);
    lv_img_set_src(obj, &img_cogwheel_argb);
    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);

//    lv_obj_set_width(obj, 200);
}

#endif
