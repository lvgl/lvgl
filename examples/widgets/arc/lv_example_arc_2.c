#include "../../../lvgl.h" 
#if LV_USE_ARC

/**
 * An `lv_task` to call periodically to set the angles of the arc
 * @param t
 */
static void arc_loader(lv_timer_t * t)
{
    static int16_t a = 270;

    a+=5;

    lv_arc_set_end_angle(t->user_data, a);

    if(a >= 270 + 360) {
        lv_timer_del(t);
        return;
    }
}

/**
 * Create an arc which acts as a loader.
 */
void lv_example_arc_2(void)
{
  /*Create an Arc*/
  lv_obj_t * arc = lv_arc_create(lv_scr_act(), NULL);
  lv_arc_set_bg_angles(arc, 0, 360);
  lv_arc_set_angles(arc, 270, 270);
  lv_obj_align(arc, NULL, LV_ALIGN_CENTER, 0, 0);

  /* Create an `lv_task` to update the arc.
   * Store the `arc` in the user data*/
  lv_timer_create(arc_loader, 20, arc);
}

#endif
