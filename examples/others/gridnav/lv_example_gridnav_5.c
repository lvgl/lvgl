#include "../../lv_examples.h"
#if LV_USE_GRIDNAV && LV_USE_FLEX && LV_BUILD_EXAMPLES

static const char * opts_0_to_9 = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9";
static const char * opts_0_to_5 = "0\n1\n2\n3\n4\n5";
static const char * opts_time_units = "s\nm\nh";

/**
 * Simple navigation on a list widget
 */
void lv_example_gridnav_5(void)
{
    /*It's assumed that the default group is set and
     *there is a keyboard indev*/

    lv_group_t * group = lv_group_get_default();
    lv_obj_t * corner;

    corner = lv_obj_create(lv_screen_active());
    lv_obj_set_flex_flow(corner, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(corner, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_size(corner, lv_pct(100), lv_pct(50));
    lv_obj_align(corner, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_gridnav_add(corner, LV_GRIDNAV_CTRL_HORIZONTAL_MOVE_ONLY);
    lv_group_add_obj(group, corner);
    const char * opts[] = {opts_0_to_5, opts_0_to_9, opts_time_units};
    for(uint8_t i = 0; i < 3; i++) {
        lv_obj_t * roller = lv_roller_create(corner);
        lv_roller_set_options(roller, opts[i], LV_ROLLER_MODE_INFINITE);
        lv_obj_set_size(roller, lv_pct(30), lv_pct(100));
        lv_group_remove_obj(roller);
    }

    corner = lv_obj_create(lv_screen_active());
    lv_obj_set_flex_flow(corner, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(corner, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_size(corner, lv_pct(100), lv_pct(50));
    lv_obj_align(corner, LV_ALIGN_TOP_MID, 0, 0);
    lv_gridnav_add(corner, LV_GRIDNAV_CTRL_VERTICAL_MOVE_ONLY);
    lv_group_add_obj(group, corner);
    for(uint8_t i = 0; i < 3; i++) {
        lv_obj_t * slider = lv_slider_create(corner);
        lv_slider_set_range(slider, 0, 9);
        lv_group_remove_obj(slider);
        lv_obj_set_width(slider, lv_pct(85));
    }
}

#endif
