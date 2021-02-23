#include "../../lvgl.h"
#if LV_BUILD_EXAMPLES

/**
 * Using the Shadow style properties
 */
void lv_example_style_4(void)
{
    static lv_style_t style;
    lv_style_init(&style);

    /*Set a background color and a radius*/
    lv_style_set_radius(&style, 5);
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    lv_style_set_bg_color(&style, lv_color_grey_lighten_3());

    /*Add a shadow*/
    lv_style_set_shadow_width(&style, 8);
    lv_style_set_shadow_color(&style, lv_color_blue());
    lv_style_set_shadow_ofs_x(&style, 10);
    lv_style_set_shadow_ofs_y(&style, 20);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &style);
    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
}

#endif
