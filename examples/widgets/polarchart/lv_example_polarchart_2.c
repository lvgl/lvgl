#include "../../lv_examples.h"
#if LV_USE_POLARCHART && LV_BUILD_EXAMPLES

/**
 * Use lv_scale to add ticks to a scrollable polarchart
 */
void lv_example_polarchart_2(void)
{
    /*Create a container*/
    lv_obj_t * main_cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(main_cont, 200, 150);
    lv_obj_center(main_cont);

    /*Create a transparent wrapper for the polarchart and the scale.
     *Set a large width, to make it scrollable on the main container*/
    lv_obj_t * wrapper = lv_obj_create(main_cont);
    lv_obj_remove_style_all(wrapper);
    lv_obj_set_size(wrapper, lv_pct(300), lv_pct(100));
    lv_obj_set_flex_flow(wrapper, LV_FLEX_FLOW_COLUMN);

    /*Create a polarchart on the wrapper
     *Set it's width to 100% to fill the large wrapper*/
    lv_obj_t * polarchart = lv_polarchart_create(wrapper);
    lv_obj_set_width(polarchart, lv_pct(100));
    lv_obj_set_flex_grow(polarchart, 1);
    lv_polarchart_set_type(polarchart, LV_CHART_TYPE_BAR);
    lv_polarchart_set_axis_range(polarchart, LV_CHART_AXIS_RADIAL, 0, 100);
    lv_polarchart_set_point_count(polarchart, 12);
    lv_obj_set_style_radius(polarchart, 0, 0);

    /*Create a scale also with 100% width*/
    lv_obj_t * scale_bottom = lv_scale_create(wrapper);
    lv_scale_set_mode(scale_bottom, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_obj_set_size(scale_bottom, lv_pct(100), 25);
    lv_scale_set_total_tick_count(scale_bottom, 12);
    lv_scale_set_major_tick_every(scale_bottom, 1);
    lv_obj_set_style_pad_hor(scale_bottom, lv_polarchart_get_first_point_center_offset(polarchart), 0);

    static const char * month[] = {"Jan", "Febr", "March", "Apr", "May", "Jun", "July", "Aug", "Sept", "Oct", "Nov", "Dec", NULL};
    lv_scale_set_text_src(scale_bottom, month);

    /*Add two data series*/
    lv_polarchart_series_t * ser1 = lv_polarchart_add_series(polarchart, lv_palette_lighten(LV_PALETTE_GREEN, 2),
                                                             LV_CHART_AXIS_RADIAL);
    lv_polarchart_series_t * ser2 = lv_polarchart_add_series(polarchart, lv_palette_darken(LV_PALETTE_GREEN, 2),
                                                             LV_CHART_AXIS_RADIAL);

    /*Set the next points on 'ser1'*/
    uint32_t i;
    for(i = 0; i < 12; i++) {
        lv_polarchart_set_next_value(polarchart, ser1, (int32_t)lv_rand(10, 60));
        lv_polarchart_set_next_value(polarchart, ser2, (int32_t)lv_rand(50, 90));
    }
    lv_polarchart_refresh(polarchart); /*Required after direct set*/
}

#endif
