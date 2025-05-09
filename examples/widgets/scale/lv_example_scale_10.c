#include "../../lv_examples.h"
#if LV_USE_SCALE && LV_BUILD_EXAMPLES

static int32_t hr_value = 98;
static int8_t hr_step = 1;
static lv_obj_t * needle_line = NULL;
static lv_obj_t * hr_value_label = NULL;
static lv_obj_t * bpm_label = NULL;
static lv_obj_t * scale = NULL;

lv_color_t get_hr_zone_color(int32_t hr)
{
    if(hr < 117) return lv_palette_main(LV_PALETTE_GREY);      // Zone 1
    else if(hr < 135) return lv_palette_main(LV_PALETTE_BLUE); // Zone 2
    else if(hr < 158) return lv_palette_main(LV_PALETTE_GREEN);// Zone 3
    else if(hr < 176) return lv_palette_main(LV_PALETTE_ORANGE);// Zone 4
    else return lv_palette_main(LV_PALETTE_RED);               // Zone 5
}

static void hr_anim_timer_cb(lv_timer_t * timer)
{
    hr_value += hr_step;

    if(hr_value >= 195) {
        hr_value = 195;
        hr_step = -1;
    }
    else if(hr_value <= 98) {
        hr_value = 98;
        hr_step = 1;
    }

    /* Update needle */
    lv_scale_set_line_needle_value(scale, needle_line, -8, hr_value);

    /* Update HR text */
    lv_label_set_text_fmt(hr_value_label, "%d", hr_value);

    /* Update text color based on zone */
    lv_color_t zone_color = get_hr_zone_color(hr_value);
    lv_obj_set_style_text_color(hr_value_label, zone_color, 0);
    lv_obj_set_style_text_color(bpm_label, zone_color, 0);
}

void lv_example_scale_10(void)
{
    scale = lv_scale_create(lv_screen_active());
    lv_obj_center(scale);
    lv_obj_set_size(scale, 200, 200);

    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_range(scale, 98, 195);
    lv_scale_set_total_tick_count(scale, 15);
    lv_scale_set_major_tick_every(scale, 3);
    lv_scale_set_angle_range(scale, 280);
    lv_scale_set_rotation(scale, 130);
    lv_scale_set_label_show(scale, false);

    lv_obj_set_style_length(scale, 6, LV_PART_ITEMS);
    lv_obj_set_style_length(scale, 10, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(scale, 0, LV_PART_MAIN);

    // Helper macro to create section styles
#define INIT_SECTION_STYLE(name, color)                                  \
    static lv_style_t name##_items, name##_main, name##_indicator;       \
    /*Minor ticks style*/                                                \
    lv_style_init(&name##_items);                                        \
    lv_style_set_line_color(&name##_items, color);                       \
    lv_style_set_line_width(&name##_items, 0); \
    /*Major ticks style*/                                                \
    lv_style_init(&name##_indicator);                                    \
    lv_style_set_line_color(&name##_indicator, color);                   \
    lv_style_set_line_width(&name##_indicator, 0); \
    /*Scale arc style*/                                                  \
    lv_style_init(&name##_main);                                         \
    lv_style_set_arc_color(&name##_main, color);                         \
    lv_style_set_arc_width(&name##_main, 20);

    // Helper macro to create and add section
#define ADD_SECTION(from, to, name)                                          \
    lv_scale_section_t * name##_sec = lv_scale_add_section(scale);          \
    lv_scale_set_section_range(scale, name##_sec, from, to);                \
    lv_scale_set_section_style_items(scale, name##_sec, &name##_items);     \
    lv_scale_set_section_style_indicator(scale, name##_sec, &name##_indicator); \
    lv_scale_set_section_style_main(scale, name##_sec, &name##_main);

    // Zone 1: (Grey)
    INIT_SECTION_STYLE(zone1, lv_palette_main(LV_PALETTE_GREY));
    ADD_SECTION(98, 117, zone1);

    // Zone 2: (Blue)
    INIT_SECTION_STYLE(zone2, lv_palette_main(LV_PALETTE_BLUE));
    ADD_SECTION(117, 135, zone2);

    // Zone 3: (Green)
    INIT_SECTION_STYLE(zone3, lv_palette_main(LV_PALETTE_GREEN));
    ADD_SECTION(135, 158, zone3);

    // Zone 4: (Orange)
    INIT_SECTION_STYLE(zone4, lv_palette_main(LV_PALETTE_ORANGE));
    ADD_SECTION(158, 176, zone4);

    // Zone 5: (Red)
    INIT_SECTION_STYLE(zone5, lv_palette_main(LV_PALETTE_RED));
    ADD_SECTION(176, 195, zone5);

    needle_line = lv_line_create(scale);

    // Optional styling
    lv_obj_set_style_line_color(needle_line, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_line_width(needle_line, 12, LV_PART_MAIN);
    lv_obj_set_style_length(needle_line, 20, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(needle_line, false, LV_PART_MAIN);
    lv_obj_set_style_pad_right(needle_line, 50, LV_PART_MAIN);

    int32_t current_hr = 145;

    lv_scale_set_line_needle_value(scale, needle_line, 0, current_hr);

    lv_obj_t * circle = lv_obj_create(lv_scr_act());
    lv_obj_set_size(circle, 130, 130);
    lv_obj_center(circle);

    lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);

    lv_obj_set_style_bg_color(circle, lv_obj_get_style_bg_color(lv_scr_act(), LV_PART_MAIN), 0);
    lv_obj_set_style_bg_opa(circle, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(circle, 0, LV_PART_MAIN);

    lv_obj_t * hr_container = lv_obj_create(circle);
    lv_obj_center(hr_container);
    lv_obj_set_size(hr_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(hr_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(hr_container, 0, 0);
    lv_obj_set_layout(hr_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(hr_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(hr_container, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(hr_container, 0, 0);
    lv_obj_set_flex_align(hr_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    hr_value_label = lv_label_create(hr_container);
    lv_label_set_text_fmt(hr_value_label, "%d", current_hr);
    lv_obj_set_style_text_font(hr_value_label, &lv_font_montserrat_40, 0);
    lv_obj_set_style_text_align(hr_value_label, LV_TEXT_ALIGN_CENTER, 0);

    bpm_label = lv_label_create(hr_container);
    lv_label_set_text(bpm_label, "bpm");
    lv_obj_set_style_text_font(bpm_label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_align(bpm_label, LV_TEXT_ALIGN_CENTER, 0);

    lv_color_t zone_color = get_hr_zone_color(current_hr);
    lv_obj_set_style_text_color(hr_value_label, zone_color, 0);
    lv_obj_set_style_text_color(bpm_label, zone_color, 0);

    lv_timer_create(hr_anim_timer_cb, 80, NULL);
}

#endif
