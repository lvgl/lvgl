#include "../../lvgl.h"
#if LV_BUILD_EXAMPLES && LV_USE_ARC

/**
 * Using the Arc style properties
 */
void lv_example_style_11(void)
{
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_arc_color(&style, LV_COLOR_RED);
    lv_style_set_arc_width(&style, 4);


    /*Create an object with the new style*/
    lv_obj_t * obj = lv_arc_create(lv_scr_act(), NULL);
    lv_obj_add_style(obj, LV_PART_MAIN, LV_STATE_DEFAULT, &style);
    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
}
#endif

