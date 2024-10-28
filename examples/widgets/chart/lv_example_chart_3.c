#include "../../lv_examples.h"
#if LV_USE_CHART && LV_BUILD_EXAMPLES

static void event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * chart = lv_event_get_target(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_invalidate(chart);
    }
    if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        int32_t * s = lv_event_get_param(e);
        *s = LV_MAX(*s, 20);
    }
    else if(code == LV_EVENT_DRAW_POST_END) {
        int32_t id = lv_chart_get_pressed_point(chart);
        if(id == LV_CHART_POINT_NONE) return;

        LV_LOG_USER("Selected point %d", (int)id);

        lv_chart_series_t * ser = lv_chart_get_series_next(chart, NULL);
        while(ser) {
            lv_point_t p;
            lv_chart_get_point_pos_by_id(chart, ser, id, &p);

            int32_t * y_array = lv_chart_get_y_array(chart, ser);
            int32_t value = y_array[id];

            /*Draw a rectangle above the clicked point*/
            lv_layer_t * layer = lv_event_get_layer(e);
            lv_draw_rect_dsc_t draw_rect_dsc;
            lv_draw_rect_dsc_init(&draw_rect_dsc);
            draw_rect_dsc.bg_color = lv_color_black();
            draw_rect_dsc.bg_opa = LV_OPA_50;
            draw_rect_dsc.radius = 3;

            lv_area_t chart_obj_coords;
            lv_obj_get_coords(chart, &chart_obj_coords);
            lv_area_t rect_area;
            rect_area.x1 = chart_obj_coords.x1 + p.x - 20;
            rect_area.x2 = chart_obj_coords.x1 + p.x + 20;
            rect_area.y1 = chart_obj_coords.y1 + p.y - 30;
            rect_area.y2 = chart_obj_coords.y1 + p.y - 10;
            lv_draw_rect(layer, &draw_rect_dsc, &rect_area);

            /*Draw the value as label to the center of the rectangle*/
            char buf[16];
            lv_snprintf(buf, sizeof(buf), LV_SYMBOL_DUMMY"$%d", value);

            lv_draw_label_dsc_t draw_label_dsc;
            lv_draw_label_dsc_init(&draw_label_dsc);
            draw_label_dsc.color = lv_color_white();
            draw_label_dsc.text = buf;
            draw_label_dsc.text_local = 1;
            draw_label_dsc.align = LV_TEXT_ALIGN_CENTER;
            lv_area_t label_area = rect_area;
            lv_area_set_height(&label_area, lv_font_get_line_height(draw_label_dsc.font));
            lv_area_align(&rect_area, &label_area, LV_ALIGN_CENTER, 0, 0);
            lv_draw_label(layer, &draw_label_dsc, &label_area);

            ser = lv_chart_get_series_next(chart, ser);
        }
    }
    else if(code == LV_EVENT_RELEASED) {
        lv_obj_invalidate(chart);
    }
}

/**
 * Show the value of the pressed points
 */
void lv_example_chart_3(void)
{
    /*Create a chart*/
    lv_obj_t * chart;
    chart = lv_chart_create(lv_screen_active());
    lv_obj_set_size(chart, 200, 150);
    lv_obj_center(chart);

    lv_obj_add_event_cb(chart, event_cb, LV_EVENT_ALL, NULL);
    lv_obj_refresh_ext_draw_size(chart);

    /*Zoom in a little in X*/
    //    lv_chart_set_scale_x(chart, 800);

    /*Add two data series*/
    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    uint32_t i;
    for(i = 0; i < 10; i++) {
        lv_chart_set_next_value(chart, ser1, lv_rand(60, 90));
        lv_chart_set_next_value(chart, ser2, lv_rand(10, 40));
    }
}

#endif
