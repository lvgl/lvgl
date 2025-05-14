#include "../../lv_examples.h"
#if LV_USE_SCALE && LV_BUILD_EXAMPLES

void lv_example_scale_12(void)
{
    static const char * years[] = {"", "2020", "2021", "2022", "2023", "2024", NULL};
    int percentages[] = {80, 56, 60, 30, 48};
    const int num_bars = sizeof(percentages) / sizeof(percentages[0]);

    lv_obj_t * scr = lv_screen_active();

    /* Create X axis (horizontal scale) */
    lv_obj_t * x_axis = lv_scale_create(scr);
    lv_obj_set_size(x_axis, 200, 50);
    lv_obj_align(x_axis, LV_ALIGN_BOTTOM_LEFT, 60, -5);
    lv_scale_set_mode(x_axis, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_range(x_axis, 1, num_bars);
    lv_scale_set_total_tick_count(x_axis, num_bars + 1);
    lv_scale_set_major_tick_every(x_axis, 1);
    lv_scale_set_label_show(x_axis, true);
    lv_scale_set_text_src(x_axis, years);

    lv_obj_update_layout(x_axis);
    lv_coord_t x_axis_y = lv_obj_get_y(x_axis);

    /* Create Y axis (vertical scale) */
    lv_obj_t * y_axis = lv_scale_create(scr);
    lv_obj_set_size(y_axis, 50, 170);
    lv_scale_set_mode(y_axis, LV_SCALE_MODE_VERTICAL_LEFT);
    lv_scale_set_range(y_axis, 0, 100);
    lv_scale_set_total_tick_count(y_axis, 6);
    lv_scale_set_major_tick_every(y_axis, 1);
    lv_scale_set_label_show(y_axis, true);

    lv_obj_update_layout(y_axis);
    lv_coord_t y_axis_height = lv_obj_get_height(y_axis);
    lv_obj_set_pos(y_axis, 10, x_axis_y - y_axis_height);

    /* Draw bars */
    int bar_width = 20;

    for(int i = 0; i < num_bars; i++) {
        lv_obj_t * bar = lv_obj_create(scr);
        lv_obj_set_style_radius(bar, 0, 0);
        lv_obj_set_style_bg_color(bar, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_set_style_border_width(bar, 0, 0);

        lv_area_t y_coords;
        lv_obj_get_coords(y_axis, &y_coords);
        lv_area_t x_coords;
        lv_obj_get_coords(x_axis, &x_coords);

        int y_axis_height = lv_obj_get_height(y_axis);
        int max_percentage = 100;
        int bar_height = (percentages[i] * y_axis_height) / max_percentage;

        lv_obj_set_size(bar, bar_width, bar_height);

        int tick_count = num_bars + 1;
        int axis_w = lv_obj_get_width(x_axis);
        int tick_spacing = axis_w / (tick_count - 1);
        int tick_x = x_coords.x1 + tick_spacing * (i + 1);

        lv_obj_align(bar, LV_ALIGN_BOTTOM_LEFT, tick_x - bar_width / 2, -55);

        lv_obj_t * label = lv_label_create(scr);
        char buf[8];
        lv_snprintf(buf, sizeof(buf), "%d%%", percentages[i]);
        lv_label_set_text(label, buf);
        lv_obj_align_to(label, bar, LV_ALIGN_OUT_TOP_MID, 0, -2);
    }
}

#endif
