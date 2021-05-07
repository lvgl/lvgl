#include "../../lv_examples.h"

#if LV_USE_ARC && LV_BUILD_EXAMPLES

void lv_example_arc_1(void)
{
  /*Create an Arc*/
  lv_obj_t * arc = lv_arc_create(lv_scr_act());
  lv_obj_set_size(arc, 150, 150);
  lv_arc_set_rotation(arc, 135);
  lv_arc_set_bg_angles(arc, 0, 270);
  lv_arc_set_value(arc, 40);
  lv_obj_center(arc);
}

#endif
