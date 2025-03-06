#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_xml_view2_from_xml(void)
{
    lv_xml_register_font(NULL, "lv_montserrat_30", &lv_font_montserrat_30);
    lv_xml_component_register_from_file("A:src/test_assets/xml/view2.xml");

    lv_xml_create(lv_screen_active(), "view2", NULL);

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/view2.png");
}

#define LIGHT_BLUE lv_color_hex(0xbbbbff)
#define DARK_BLUE lv_color_hex(0x000080)

void test_xml_view2_from_c(void)
{
    static lv_style_t style_red;
    static lv_style_t style_blue;
    static lv_style_t style_scale_main;
    static lv_style_t style_scale_indic;
    static lv_style_t style_scale_section_indic;
    static bool style_inited = false;
    if(!style_inited) {

        lv_style_init(&style_red);
        lv_style_set_text_color(&style_red, lv_color_hex(0xff0000));
        lv_style_set_text_opa(&style_red, 150);

        lv_style_init(&style_blue);
        lv_style_set_text_color(&style_blue, DARK_BLUE);
        lv_style_set_text_font(&style_blue, &lv_font_montserrat_30);

        lv_style_init(&style_scale_main);
        lv_style_set_bg_opa(&style_scale_main, 127); /*128 is alculated from bg_opa=50%*/
        lv_style_set_pad_all(&style_scale_main, 32);
        lv_style_set_radius(&style_scale_main, 8);

        lv_style_init(&style_scale_indic);
        lv_style_set_length(&style_scale_indic, 20);

        lv_style_init(&style_scale_section_indic);
        lv_style_set_text_color(&style_scale_section_indic, lv_color_hex(0x00ff00));
        lv_style_set_line_width(&style_scale_section_indic, 6);

        style_inited = true;
    }

    lv_obj_t * lv_obj_1 = lv_obj_create(lv_screen_active());
    lv_obj_set_width(lv_obj_1, 700);
    lv_obj_set_height(lv_obj_1, 470);
    lv_obj_set_style_bg_color(lv_obj_1, LIGHT_BLUE, 0);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_COLUMN_WRAP);

    lv_obj_t * lv_buttonmatrix_1 = lv_buttonmatrix_create(lv_obj_1);

    static const char * lv_buttonmatrix_1_map[] = {"1", "2", "\n", "3", NULL};
    lv_buttonmatrix_set_map(lv_buttonmatrix_1, lv_buttonmatrix_1_map);

    static const lv_buttonmatrix_ctrl_t lv_buttonmatrix_1_ctrl_map[] = {LV_BUTTONMATRIX_CTRL_CHECKED | LV_BUTTONMATRIX_CTRL_WIDTH_3, LV_BUTTONMATRIX_CTRL_NONE, LV_BUTTONMATRIX_CTRL_DISABLED};
    lv_buttonmatrix_set_ctrl_map(lv_buttonmatrix_1, lv_buttonmatrix_1_ctrl_map);

    lv_obj_set_width(lv_buttonmatrix_1, 200);

    lv_obj_t * lv_spangroup_1 = lv_spangroup_create(lv_obj_1);
    lv_span_t * lv_spangroup_1_span_1 = lv_spangroup_add_span(lv_spangroup_1);
    lv_spangroup_set_span_text(lv_spangroup_1, lv_spangroup_1_span_1, "hello");
    lv_spangroup_set_span_style(lv_spangroup_1, lv_spangroup_1_span_1, &style_red);

    lv_span_t * lv_spangroup_1_span_2 = lv_spangroup_add_span(lv_spangroup_1);
    lv_spangroup_set_span_text(lv_spangroup_1, lv_spangroup_1_span_2, "world");
    lv_spangroup_set_span_style(lv_spangroup_1, lv_spangroup_1_span_2, &style_blue);

    lv_obj_t * lv_scale_1 = lv_scale_create(lv_obj_1);
    lv_scale_set_total_tick_count(lv_scale_1, 31);
    lv_scale_set_label_show(lv_scale_1, true);
    lv_scale_set_major_tick_every(lv_scale_1, 6);
    lv_scale_set_range(lv_scale_1, 10, 110);
    lv_obj_set_width(lv_scale_1, 300);
    lv_obj_add_style(lv_scale_1, &style_scale_main, 0);
    lv_obj_add_style(lv_scale_1, &style_scale_indic, LV_PART_INDICATOR);

    lv_scale_section_t * lv_scale_1_section_1 = lv_scale_add_section(lv_scale_1);
    lv_scale_set_section_range(lv_scale_1, lv_scale_1_section_1, 30, 60);
    lv_scale_set_section_style_indicator(lv_scale_1, lv_scale_1_section_1, &style_scale_section_indic);

    lv_obj_t * lv_table_1  = lv_table_create(lv_obj_1);
    lv_table_set_column_count(lv_table_1, 4);
    lv_obj_set_style_border_side(lv_table_1, LV_BORDER_SIDE_FULL, LV_PART_ITEMS);
    lv_table_set_column_width(lv_table_1, 1, 30);
    lv_table_set_column_width(lv_table_1, 3, 20);
    lv_table_set_cell_value(lv_table_1, 2, 1, "hello hello hello hello");
    lv_table_set_cell_ctrl(lv_table_1, 2, 1, LV_TABLE_CELL_CTRL_TEXT_CROP | LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    lv_table_set_cell_value(lv_table_1, 3, 0, "world");

    TEST_ASSERT_EQUAL_SCREENSHOT("xml/view2.png");
}

#endif
