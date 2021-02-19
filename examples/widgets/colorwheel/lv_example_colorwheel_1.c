#include "../../../lvgl.h"
#if LV_USE_COLORWHEEL && LV_BUILD_EXAMPLES

void lv_example_colorwheel_1(void)
{
    lv_obj_t * cw;

    cw = lv_colorwheel_create(lv_scr_act(), true);
    lv_obj_set_size(cw, 200, 200);
    lv_obj_align(cw, NULL, LV_ALIGN_CENTER, 0, 0);
}

#endif
