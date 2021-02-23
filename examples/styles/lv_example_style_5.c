#include "../../lvgl.h"
#if LV_BUILD_EXAMPLES

/**
 * Using the content style properties
 */
void lv_example_style_6(void)
{
    static lv_style_t style;
    lv_style_init(&style);

    /*Set a background color and a radius*/
    lv_style_set_radius(&style, 5);
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    lv_style_set_bg_color(&style, lv_color_grey_lighten_3());

    /*Add a value text properties*/
    lv_style_set_content_color(&style, lv_color_blue());
    lv_style_set_content_align(&style, LV_ALIGN_IN_BOTTOM_RIGHT);
    lv_style_set_content_ofs_x(&style, -5);
    lv_style_set_content_ofs_y(&style, -5);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &style);
    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Add a value text to the local style. This way every object can have different text*/
    lv_obj_set_style_content_text(obj, LV_PART_MAIN, LV_STATE_DEFAULT, "Text");
}

#endif
