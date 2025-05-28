#include "../../lv_examples.h"
#if LV_USE_SCALE && LV_BUILD_EXAMPLES && LV_FONT_MONTSERRAT_12 && LV_FONT_MONTSERRAT_14 && LV_FONT_MONTSERRAT_16 && LV_FONT_MONTSERRAT_20

static void label_color_cb(lv_event_t * e)
{
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    if(!draw_task) return;

    lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
    if(!base_dsc || base_dsc->part != LV_PART_INDICATOR) return;

    lv_draw_label_dsc_t * label_dsc = lv_draw_task_get_label_dsc(draw_task);
    if(!label_dsc || !label_dsc->text) return;

    const char * txt = label_dsc->text;

    if(lv_strcmp(txt, "06") == 0 || lv_strcmp(txt, "12") == 0 ||
       lv_strcmp(txt, "18") == 0 || lv_strcmp(txt, "24") == 0) {
        label_dsc->color = lv_color_white();
    }
    else {
        label_dsc->color = lv_palette_darken(LV_PALETTE_GREY, 1);
    }
}

void lv_example_scale_11(void)
{
    lv_obj_t * bg = lv_obj_create(lv_screen_active());
    lv_obj_set_size(bg, 210, 210);
    lv_obj_center(bg);
    lv_obj_set_style_radius(bg, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(bg, lv_palette_darken(LV_PALETTE_GREY, 4), 0);
    lv_obj_set_style_bg_opa(bg, LV_OPA_COVER, 0);
    lv_obj_remove_flag(bg, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(bg, 0, LV_PART_MAIN);

    lv_obj_t * scale = lv_scale_create(bg);
    lv_obj_center(scale);
    lv_obj_set_size(scale, 150, 150);
    lv_obj_set_style_arc_width(scale, 5, LV_PART_MAIN);

    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_OUTER);
    lv_scale_set_range(scale, 0, 24);
    lv_scale_set_total_tick_count(scale, 25);
    lv_scale_set_major_tick_every(scale, 1);
    lv_scale_set_angle_range(scale, 360);
    lv_scale_set_rotation(scale, 105);
    lv_scale_set_label_show(scale, true);
    lv_obj_set_style_text_font(scale, &lv_font_montserrat_12, LV_PART_INDICATOR);
    lv_obj_set_style_pad_radial(scale, -6, LV_PART_INDICATOR);

    /*Rotate the labels of the ticks*/
    lv_obj_set_style_transform_rotation(scale, LV_SCALE_LABEL_ROTATE_MATCH_TICKS | LV_SCALE_LABEL_ROTATE_KEEP_UPRIGHT,
                                        LV_PART_INDICATOR);

    /* Style for major ticks */
    static lv_style_t style_ticks;
    lv_style_init(&style_ticks);
    lv_style_set_line_color(&style_ticks, lv_palette_darken(LV_PALETTE_GREY, 1));
    lv_style_set_line_width(&style_ticks, 2);
    lv_style_set_width(&style_ticks, 10);
    lv_obj_add_style(scale, &style_ticks, LV_PART_INDICATOR);

    /* Style for NIGHT — blue */
    static lv_style_t style_night;
    lv_style_init(&style_night);
    lv_style_set_arc_color(&style_night, lv_palette_main(LV_PALETTE_BLUE));

    /* Style for DAY — dark yellow */
    static lv_style_t style_day;
    lv_style_init(&style_day);
    lv_style_set_arc_color(&style_day, lv_palette_darken(LV_PALETTE_YELLOW, 3));

    /* NIGHT section */
    lv_scale_section_t * section_night1 = lv_scale_add_section(scale);
    lv_scale_set_section_range(scale, section_night1, 17, 5);
    lv_scale_set_section_style_main(scale, section_night1, &style_night);

    /* DAY section */
    lv_scale_section_t * section_day = lv_scale_add_section(scale);
    lv_scale_set_section_range(scale, section_day, 5, 17);
    lv_scale_set_section_style_main(scale, section_day, &style_day);


    static const char * hour_labels[] = {
        "01", "02", "03", "04", "05",
        "06", "07", "08", "09", "10",
        "11", "12", "13", "14", "15",
        "16", "17", "18", "19", "20",
        "21", "22", "23", "24",
        NULL
    };
    lv_scale_set_text_src(scale, hour_labels);

    lv_obj_add_flag(scale, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_obj_add_event_cb(scale, label_color_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);

    lv_obj_t * today = lv_label_create(bg);
    lv_label_set_text(today, "TODAY");
    lv_obj_set_style_text_font(today, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(today, lv_color_white(), 0);
    lv_obj_align(today, LV_ALIGN_TOP_MID, 0, 60);

    lv_obj_t * sunrise_lbl = lv_label_create(bg);
    lv_label_set_text(sunrise_lbl, "SUNRISE");
    lv_obj_set_style_text_font(sunrise_lbl, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(sunrise_lbl, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_align(sunrise_lbl, LV_ALIGN_LEFT_MID, 37, -10);

    lv_obj_t * sunrise_time = lv_label_create(bg);
    lv_label_set_text(sunrise_time, "6:43");
    lv_obj_set_style_text_font(sunrise_time, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(sunrise_time, lv_color_white(), 0);
    lv_obj_align_to(sunrise_time, sunrise_lbl, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);

    lv_obj_t * sunset_lbl = lv_label_create(bg);
    lv_label_set_text(sunset_lbl, "SUNSET");
    lv_obj_set_style_text_font(sunset_lbl, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(sunset_lbl, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_align(sunset_lbl, LV_ALIGN_RIGHT_MID, -37, -10);

    lv_obj_t * sunset_time = lv_label_create(bg);
    lv_label_set_text(sunset_time, "17:37");
    lv_obj_set_style_text_font(sunset_time, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(sunset_time, lv_color_white(), 0);
    lv_obj_align_to(sunset_time, sunset_lbl, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
}

#endif
