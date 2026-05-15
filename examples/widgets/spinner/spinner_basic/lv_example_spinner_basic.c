/**
 * @file lv_example_spinner_basic.c
 */

#include "../../../../lvgl.h"

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
void lv_example_spinner_basic_create(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(screen, 32, 0);

    /* Long arc, medium speed */
    lv_obj_t * lv_spinner_0 = lv_spinner_create(screen);
    lv_obj_set_size(lv_spinner_0, 80, 80);
    lv_spinner_set_anim_duration(lv_spinner_0, 1500);
    lv_spinner_set_arc_sweep(lv_spinner_0, 270);

    /* Short arc, faster */
    lv_obj_t * lv_spinner_1 = lv_spinner_create(screen);
    lv_obj_set_size(lv_spinner_1, 80, 80);
    lv_spinner_set_anim_duration(lv_spinner_1, 800);
    lv_spinner_set_arc_sweep(lv_spinner_1, 90);
}

