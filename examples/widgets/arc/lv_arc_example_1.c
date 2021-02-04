#include "../../../lvgl.h"

#if LV_USE_ARC

void lv_ex_arc_1(void) 
{
  /*Create an Arc*/
  lv_obj_t * arc = lv_arc_create(lv_scr_act(), NULL);
  lv_arc_set_end_angle(arc, 200);
  lv_obj_set_size(arc, 150, 150);
  lv_obj_align(arc, NULL, LV_ALIGN_CENTER, 0, 0);
}

#endif
