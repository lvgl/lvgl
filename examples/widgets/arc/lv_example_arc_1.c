#include "../../../lvgl.h"

#if LV_USE_ARC && LV_BUILD_EXAMPLES

void lv_example_arc_1(void)
{
  /*Create an Arc*/
  lv_obj_t * arc = lv_arc_create(lv_scr_act());
  lv_arc_set_end_angle(arc, 200);
  lv_obj_set_size(arc, 150, 150);
  lv_obj_center(arc);
}

#endif
