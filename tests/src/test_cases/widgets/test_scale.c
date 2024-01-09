#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

/* Function run before every test */
void setUp(void)
{
}

/* Function run after every test */
void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

/* A simple horizontal scale */
void test_scale_render_example_1(void)
{
    lv_obj_t * scale = lv_scale_create(lv_screen_active());
    lv_obj_set_size(scale, lv_pct(80), 100);
    lv_scale_set_mode(scale, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_obj_center(scale);

    lv_scale_set_label_show(scale, true);

    lv_scale_set_total_tick_count(scale, 31);
    lv_scale_set_major_tick_every(scale, 5);

    lv_obj_set_style_length(scale, 5, LV_PART_ITEMS);
    lv_obj_set_style_length(scale, 10, LV_PART_INDICATOR);
    lv_scale_set_range(scale, 10, 40);

    TEST_ASSERT_EQUAL_SCREENSHOT("scale_1.png");
}

/* An vertical scale with section and custom styling */
void test_scale_render_example_2(void)
{
    lv_obj_t * scale = lv_scale_create(lv_screen_active());
    lv_obj_set_size(scale, 60, 200);
    lv_scale_set_label_show(scale, true);
    lv_scale_set_mode(scale, LV_SCALE_MODE_VERTICAL_RIGHT);
    lv_obj_center(scale);

    lv_scale_set_total_tick_count(scale, 21);
    lv_scale_set_major_tick_every(scale, 5);

    lv_obj_set_style_length(scale, 5, LV_PART_ITEMS);
    lv_obj_set_style_length(scale, 10, LV_PART_INDICATOR);
    lv_scale_set_range(scale, 0, 100);

    static const char * custom_labels[] = {"0 °C", "25 °C", "50 °C", "75 °C", "100 °C", NULL};
    lv_scale_set_text_src(scale, custom_labels);

    static lv_style_t indicator_style;
    lv_style_init(&indicator_style);

    /* Label style properties */
    lv_style_set_text_font(&indicator_style, &lv_font_montserrat_14);
    lv_style_set_text_color(&indicator_style, lv_palette_darken(LV_PALETTE_BLUE, 3));

    /* Major tick properties */
    lv_style_set_line_color(&indicator_style, lv_palette_darken(LV_PALETTE_BLUE, 3));
    lv_style_set_width(&indicator_style, 10U);      /*Tick length*/
    lv_style_set_line_width(&indicator_style, 2U);  /*Tick width*/
    lv_obj_add_style(scale, &indicator_style, LV_PART_INDICATOR);

    static lv_style_t minor_ticks_style;
    lv_style_init(&minor_ticks_style);
    lv_style_set_line_color(&minor_ticks_style, lv_palette_lighten(LV_PALETTE_BLUE, 2));
    lv_style_set_width(&minor_ticks_style, 5U);         /*Tick length*/
    lv_style_set_line_width(&minor_ticks_style, 2U);    /*Tick width*/
    lv_obj_add_style(scale, &minor_ticks_style, LV_PART_ITEMS);

    static lv_style_t main_line_style;
    lv_style_init(&main_line_style);
    /* Main line properties */
    lv_style_set_line_color(&main_line_style, lv_palette_darken(LV_PALETTE_BLUE, 3));
    lv_style_set_line_width(&main_line_style, 2U); // Tick width
    lv_obj_add_style(scale, &main_line_style, LV_PART_MAIN);

    /* Add a section */
    static lv_style_t section_minor_tick_style;
    static lv_style_t section_label_style;
    static lv_style_t section_main_line_style;

    lv_style_init(&section_label_style);
    lv_style_init(&section_minor_tick_style);
    lv_style_init(&section_main_line_style);

    /* Label style properties */
    lv_style_set_text_font(&section_label_style, &lv_font_montserrat_14);
    lv_style_set_text_color(&section_label_style, lv_palette_darken(LV_PALETTE_RED, 3));

    lv_style_set_line_color(&section_label_style, lv_palette_darken(LV_PALETTE_RED, 3));
    lv_style_set_line_width(&section_label_style, 5U); /*Tick width*/

    lv_style_set_line_color(&section_minor_tick_style, lv_palette_lighten(LV_PALETTE_RED, 2));
    lv_style_set_line_width(&section_minor_tick_style, 4U); /*Tick width*/

    /* Main line properties */
    lv_style_set_line_color(&section_main_line_style, lv_palette_darken(LV_PALETTE_RED, 3));
    lv_style_set_line_width(&section_main_line_style, 4U); /*Tick width*/

    /* Configure section styles */
    lv_scale_section_t * section = lv_scale_add_section(scale);
    lv_scale_section_set_range(section, 75, 100);
    lv_scale_section_set_style(section, LV_PART_INDICATOR, &section_label_style);
    lv_scale_section_set_style(section, LV_PART_ITEMS, &section_minor_tick_style);
    lv_scale_section_set_style(section, LV_PART_MAIN, &section_main_line_style);

    lv_obj_set_style_bg_color(scale, lv_palette_main(LV_PALETTE_BLUE_GREY), 0);
    lv_obj_set_style_bg_opa(scale, LV_OPA_50, 0);
    lv_obj_set_style_pad_left(scale, 8, 0);
    lv_obj_set_style_radius(scale, 8, 0);
    lv_obj_set_style_pad_ver(scale, 20, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("scale_2.png");
}

/* A simple round scale */
void test_scale_render_example_3(void)
{
    lv_obj_t * scale = lv_scale_create(lv_screen_active());
    lv_obj_set_size(scale, 150, 150);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    lv_obj_set_style_bg_opa(scale, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scale, lv_palette_lighten(LV_PALETTE_RED, 5), 0);
    lv_obj_set_style_radius(scale, LV_RADIUS_CIRCLE, 0);
    lv_obj_center(scale);

    lv_scale_set_label_show(scale, true);

    lv_scale_set_total_tick_count(scale, 11);
    lv_scale_set_major_tick_every(scale, 5);

    lv_obj_set_style_length(scale, 5, LV_PART_ITEMS);
    lv_obj_set_style_length(scale, 10, LV_PART_INDICATOR);
    lv_scale_set_range(scale, 10, 40);

    TEST_ASSERT_EQUAL_SCREENSHOT("scale_3.png");
}

/* A round scale with section and custom styling */
void test_scale_render_example_4(void)
{
    lv_obj_t * scale = lv_scale_create(lv_screen_active());
    lv_obj_set_size(scale, 150, 150);
    lv_scale_set_label_show(scale, true);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_OUTER);
    lv_obj_center(scale);

    lv_scale_set_total_tick_count(scale, 21);
    lv_scale_set_major_tick_every(scale, 5);

    lv_obj_set_style_length(scale, 5, LV_PART_ITEMS);
    lv_obj_set_style_length(scale, 10, LV_PART_INDICATOR);
    lv_scale_set_range(scale, 0, 100);

    static const char * custom_labels[] = {"0 °C", "25 °C", "50 °C", "75 °C", "100 °C", NULL};
    lv_scale_set_text_src(scale, custom_labels);

    static lv_style_t indicator_style;
    lv_style_init(&indicator_style);

    /* Label style properties */
    lv_style_set_text_font(&indicator_style, &lv_font_montserrat_14);
    lv_style_set_text_color(&indicator_style, lv_palette_darken(LV_PALETTE_BLUE, 3));

    /* Major tick properties */
    lv_style_set_line_color(&indicator_style, lv_palette_darken(LV_PALETTE_BLUE, 3));
    lv_style_set_width(&indicator_style, 10U);      /*Tick length*/
    lv_style_set_line_width(&indicator_style, 2U);  /*Tick width*/
    lv_obj_add_style(scale, &indicator_style, LV_PART_INDICATOR);

    static lv_style_t minor_ticks_style;
    lv_style_init(&minor_ticks_style);
    lv_style_set_line_color(&minor_ticks_style, lv_palette_lighten(LV_PALETTE_BLUE, 2));
    lv_style_set_width(&minor_ticks_style, 5U);         /*Tick length*/
    lv_style_set_line_width(&minor_ticks_style, 2U);    /*Tick width*/
    lv_obj_add_style(scale, &minor_ticks_style, LV_PART_ITEMS);

    static lv_style_t main_line_style;
    lv_style_init(&main_line_style);
    /* Main line properties */
    lv_style_set_arc_color(&main_line_style, lv_palette_darken(LV_PALETTE_BLUE, 3));
    lv_style_set_arc_width(&main_line_style, 2U); /*Tick width*/
    lv_obj_add_style(scale, &main_line_style, LV_PART_MAIN);

    /* Add a section */
    static lv_style_t section_minor_tick_style;
    static lv_style_t section_label_style;
    static lv_style_t section_main_line_style;

    lv_style_init(&section_label_style);
    lv_style_init(&section_minor_tick_style);
    lv_style_init(&section_main_line_style);

    /* Label style properties */
    lv_style_set_text_font(&section_label_style, &lv_font_montserrat_14);
    lv_style_set_text_color(&section_label_style, lv_palette_darken(LV_PALETTE_RED, 3));

    lv_style_set_line_color(&section_label_style, lv_palette_darken(LV_PALETTE_RED, 3));
    lv_style_set_line_width(&section_label_style, 5U); /*Tick width*/

    lv_style_set_line_color(&section_minor_tick_style, lv_palette_lighten(LV_PALETTE_RED, 2));
    lv_style_set_line_width(&section_minor_tick_style, 4U); /*Tick width*/

    /* Main line properties */
    lv_style_set_arc_color(&section_main_line_style, lv_palette_darken(LV_PALETTE_RED, 3));
    lv_style_set_arc_width(&section_main_line_style, 4U); /*Tick width*/

    /* Configure section styles */
    lv_scale_section_t * section = lv_scale_add_section(scale);
    lv_scale_section_set_range(section, 75, 100);
    lv_scale_section_set_style(section, LV_PART_INDICATOR, &section_label_style);
    lv_scale_section_set_style(section, LV_PART_ITEMS, &section_minor_tick_style);
    lv_scale_section_set_style(section, LV_PART_MAIN, &section_main_line_style);

    TEST_ASSERT_EQUAL_SCREENSHOT("scale_4.png");
}

void test_scale_set_style(void)
{
    lv_obj_t * scale = lv_scale_create(lv_screen_active());

    static lv_style_t section_minor_tick_style;
    static lv_style_t section_label_style;
    static lv_style_t section_main_line_style;

    lv_style_init(&section_label_style);
    lv_style_init(&section_minor_tick_style);
    lv_style_init(&section_main_line_style);

    /* Configure section styles */
    lv_scale_section_t * section = lv_scale_add_section(scale);
    lv_scale_section_set_range(section, 75, 100);

    lv_scale_section_set_style(section, LV_PART_MAIN, &section_main_line_style);
    TEST_ASSERT_NOT_NULL(section->main_style);
    TEST_ASSERT_NULL(section->indicator_style);
    TEST_ASSERT_NULL(section->items_style);

    TEST_ASSERT_EQUAL(section->main_style, &section_main_line_style);

    lv_scale_section_set_style(section, LV_PART_INDICATOR, &section_label_style);
    TEST_ASSERT_NOT_NULL(section->main_style);
    TEST_ASSERT_NOT_NULL(section->indicator_style);
    TEST_ASSERT_NULL(section->items_style);

    TEST_ASSERT_EQUAL(section->main_style, &section_main_line_style);
    TEST_ASSERT_EQUAL(section->indicator_style, &section_label_style);

    lv_scale_section_set_style(section, LV_PART_ITEMS, &section_minor_tick_style);
    TEST_ASSERT_NOT_NULL(section->main_style);
    TEST_ASSERT_NOT_NULL(section->indicator_style);
    TEST_ASSERT_NOT_NULL(section->items_style);

    TEST_ASSERT_EQUAL(section->main_style, &section_main_line_style);
    TEST_ASSERT_EQUAL(section->indicator_style, &section_label_style);
    TEST_ASSERT_EQUAL(section->items_style, &section_minor_tick_style);

    /* Invalid part */
    lv_scale_section_set_style(section, LV_PART_CURSOR, &section_minor_tick_style);
    TEST_ASSERT_NOT_NULL(section->main_style);
    TEST_ASSERT_NOT_NULL(section->indicator_style);
    TEST_ASSERT_NOT_NULL(section->items_style);

    TEST_ASSERT_EQUAL(section->main_style, &section_main_line_style);
    TEST_ASSERT_EQUAL(section->indicator_style, &section_label_style);
    TEST_ASSERT_EQUAL(section->items_style, &section_minor_tick_style);

    /* NULL section */
    lv_scale_section_t * null_section = NULL;

    lv_scale_section_set_range(null_section, 75, 100);
    lv_scale_section_set_style(null_section, LV_PART_MAIN, &section_main_line_style);
}

/* The scale internally counts the number of custom labels until it finds the NULL sentinel */
void test_scale_custom_labels_count(void)
{
    lv_obj_t * scale = lv_scale_create(lv_screen_active());
    lv_scale_set_label_show(scale, true);

    static const char * custom_labels[] = {"0 °C", "25 °C", "50 °C", "75 °C", "100 °C", NULL};
    lv_scale_set_text_src(scale, custom_labels);

    lv_scale_t * scale_widget = (lv_scale_t *)scale;

    TEST_ASSERT_EQUAL(5U, scale_widget->custom_label_cnt);

    static const char * animal_labels[] = {"cat", "dog", NULL};
    lv_scale_set_text_src(scale, animal_labels);

    TEST_ASSERT_EQUAL(2U, scale_widget->custom_label_cnt);
}

void test_scale_mode(void)
{
    lv_obj_t * scale = lv_scale_create(lv_screen_active());

    lv_scale_mode_t mode = LV_SCALE_MODE_ROUND_INNER;
    lv_scale_set_mode(scale, mode);

    TEST_ASSERT_EQUAL(mode, lv_scale_get_mode(scale));
}

void test_scale_total_tick_count(void)
{
    lv_obj_t * scale = lv_scale_create(lv_screen_active());

    uint32_t total_tick_count = 42;
    lv_scale_set_total_tick_count(scale, total_tick_count);

    TEST_ASSERT_EQUAL(total_tick_count, lv_scale_get_total_tick_count(scale));
}

void test_scale_major_tick_every(void)
{
    lv_obj_t * scale = lv_scale_create(lv_screen_active());

    uint32_t major_tick_every = 6;
    lv_scale_set_major_tick_every(scale, major_tick_every);

    TEST_ASSERT_EQUAL(major_tick_every, lv_scale_get_major_tick_every(scale));
}

void test_scale_label_show(void)
{
    lv_obj_t * scale = lv_scale_create(lv_screen_active());

    bool label_show = true;
    lv_scale_set_label_show(scale, label_show);

    TEST_ASSERT_EQUAL(label_show, lv_scale_get_label_show(scale));

    label_show = false;
    lv_scale_set_label_show(scale, label_show);

    TEST_ASSERT_EQUAL(label_show, lv_scale_get_label_show(scale));
}

void test_scale_angle_range(void)
{
    lv_obj_t * scale = lv_scale_create(lv_screen_active());

    uint32_t angle_range = 42;
    lv_scale_set_angle_range(scale, angle_range);

    TEST_ASSERT_EQUAL(angle_range, lv_scale_get_angle_range(scale));
}

void test_scale_range(void)
{
    lv_obj_t * scale = lv_scale_create(lv_screen_active());

    int32_t min_range = 24;
    int32_t max_range = 42;
    lv_scale_set_range(scale, min_range, max_range);

    TEST_ASSERT_EQUAL(min_range, lv_scale_get_range_min_value(scale));
    TEST_ASSERT_EQUAL(max_range, lv_scale_get_range_max_value(scale));
}

#endif
