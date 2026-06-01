/**
 * @file lv_example_spinner_basic.c
 */

#include "../../lv_examples.h"
#if LV_USE_SPINNER && LV_BUILD_EXAMPLES

/**
 * @title Spinner basics
 * @brief A loading indicator that rotates a configurable arc segment.
 *
 * A spinner is a self-animating widget: nothing has to drive its value from
 * outside. `anim_duration` is the number of milliseconds it takes to complete
 * one full rotation (lower = faster). `arc_sweep` is the angular length of
 * the moving segment in degrees — common values are 270° (a long C shape) or
 * 90° (a short dash). Width and height set the diameter; the spinner sizes
 * the arc to fill the widget.
 */
void lv_example_spinner_basic(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* Long arc, medium speed */
    lv_obj_t * spinner_1 = lv_spinner_create(screen);
    lv_obj_set_size(spinner_1, 80, 80);
    lv_spinner_set_anim_duration(spinner_1, 1500);
    lv_spinner_set_arc_sweep(spinner_1, 270);

    /* Short arc, faster */
    lv_obj_t * spinner_2 = lv_spinner_create(screen);
    lv_obj_set_size(spinner_2, 80, 80);
    lv_spinner_set_anim_duration(spinner_2, 600);
    lv_spinner_set_arc_sweep(spinner_2, 180);
}
#endif
