#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_LINE

/**
 * Using the drop shadow style properties
 */
void lv_example_style_10(void)
{
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_drop_shadow_color(&style, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_drop_shadow_radius(&style, 16);
    lv_style_set_drop_shadow_opa(&style, 255);
    lv_style_set_drop_shadow_offset_x(&style, 5);
    lv_style_set_drop_shadow_offset_y(&style, 10);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_arc_create(lv_screen_active());
    lv_obj_add_style(obj, &style, LV_PART_INDICATOR);
    lv_obj_center(obj);
}

#endif
