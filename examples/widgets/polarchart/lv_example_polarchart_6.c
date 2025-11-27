#include "../../lv_examples.h"
#if LV_USE_POLARCHART && LV_BUILD_EXAMPLES

static lv_obj_t * polarchart;
static lv_polarchart_series_t * ser;
static lv_polarchart_cursor_t * cursor;

static void value_changed_event_cb(lv_event_t * e)
{
    uint32_t last_id;
    lv_obj_t * obj = lv_event_get_target_obj(e);

    last_id = lv_polarchart_get_pressed_point(obj);
    if(last_id != LV_POLARCHART_POINT_NONE) {
        lv_polarchart_set_cursor_point(obj, cursor, NULL, last_id);
    }
}

/**
 * Show cursor on the clicked point
 */
void lv_example_polarchart_6(void)
{
    polarchart = lv_polarchart_create(lv_screen_active());
    lv_obj_set_size(polarchart, 200, 150);
    lv_obj_align(polarchart, LV_ALIGN_CENTER, 0, -10);

    //    lv_polarchart_set_axis_tick(polarchart, LV_POLARCHART_AXIS_RADIAL, 10, 5, 6, 5, true, 40);
    //    lv_polarchart_set_axis_tick(polarchart, LV_POLARCHART_AXIS_ANGLE, 10, 5, 10, 1, true, 30);

    lv_obj_add_event_cb(polarchart, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_refresh_ext_draw_size(polarchart);

    cursor = lv_polarchart_add_cursor(polarchart, lv_palette_main(LV_PALETTE_BLUE),
                                      (lv_dir_t)(LV_DIR_LEFT | LV_DIR_BOTTOM));

    ser = lv_polarchart_add_series(polarchart, lv_palette_main(LV_PALETTE_RED), LV_POLARCHART_AXIS_RADIAL);
    uint32_t i;
    for(i = 0; i < 10; i++) {
        lv_polarchart_set_next_value(polarchart, ser, (int32_t)lv_rand(10, 90));
    }

    //    lv_polarchart_set_scale_angle(polarchart, 360);

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Click on a point");
    lv_obj_align_to(label, polarchart, LV_ALIGN_OUT_TOP_MID, 0, -5);
}

#endif
