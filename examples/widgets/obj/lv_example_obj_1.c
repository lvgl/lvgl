#include "../../../lvgl.h"
#if LV_BUILD_EXAMPLES

void lv_example_obj_1(void)
{
    lv_obj_t * obj1;
    obj1 = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj1, 100, 50);
    lv_obj_align(obj1, NULL, LV_ALIGN_CENTER, -60, -30);

    static lv_style_t style_shadow;
    lv_style_init(&style_shadow);
    lv_style_set_shadow_width(&style_shadow, 10);
    lv_style_set_shadow_spread(&style_shadow, 5);
    lv_style_set_shadow_color(&style_shadow, lv_color_blue());

    lv_obj_t * obj3;
    obj3 = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_add_style(obj3, LV_PART_MAIN, LV_STATE_DEFAULT, &style_shadow);
    lv_obj_align(obj3, NULL, LV_ALIGN_CENTER, 60, 30);
}
#endif
