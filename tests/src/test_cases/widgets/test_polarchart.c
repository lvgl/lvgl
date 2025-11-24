#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * polarchart = NULL;

static lv_color_t red_color;

void setUp(void)
{
    active_screen = lv_screen_active();
    polarchart = lv_polarchart_create(active_screen);

    red_color = lv_palette_main(LV_PALETTE_RED);
}

void tearDown(void)
{
    /* Is there a way to destroy a polar chart without having to call remove_series for each of it series? */
    lv_obj_clean(active_screen);
}

/* NOTE: Default polar chart type is LV_POLARCHART_TYPE_LINE */
void test_polarchart_add_series(void)
{
    lv_polarchart_series_t * red_series;

    red_series = lv_polarchart_add_series(polarchart, red_color, LV_POLARCHART_AXIS_RADIAL);

    TEST_ASSERT_NOT_NULL_MESSAGE(red_series, "Red series not added to polar chart");
    TEST_ASSERT_NULL_MESSAGE(red_series->angle_points,
                             "'Angle' points in non scatter polar chart should not point to anything");

    lv_polarchart_remove_series(polarchart, red_series);
}

void test_polarchart_set_point_count_increments(void)
{
    lv_polarchart_series_t * red_series;
    red_series = lv_polarchart_add_series(polarchart, red_color, LV_POLARCHART_AXIS_RADIAL);

    uint16_t points_in_series = lv_polarchart_get_point_count(polarchart);
    uint16_t new_point_count = points_in_series * 2;
    lv_polarchart_set_point_count(polarchart, new_point_count);

    TEST_ASSERT_EQUAL_MESSAGE(new_point_count, lv_polarchart_get_point_count(polarchart),
                              "Actual points in polar chart are less than expected");
    lv_polarchart_remove_series(polarchart, red_series);
}

void test_polarchart_set_point_count_decrements(void)
{
    lv_polarchart_series_t * red_series;
    red_series = lv_polarchart_add_series(polarchart, red_color, LV_POLARCHART_AXIS_RADIAL);
    uint16_t points_in_series = lv_polarchart_get_point_count(polarchart);
    uint16_t new_point_count = points_in_series / 2;

    lv_polarchart_set_point_count(polarchart, new_point_count);

    TEST_ASSERT_EQUAL_MESSAGE(new_point_count, lv_polarchart_get_point_count(polarchart),
                              "Actual points in polar chart are more than expected");
    lv_polarchart_remove_series(polarchart, red_series);
}

void test_polarchart_set_point_count_as_same(void)
{
    lv_polarchart_series_t * red_series;
    red_series = lv_polarchart_add_series(polarchart, red_color, LV_POLARCHART_AXIS_RADIAL);
    uint16_t points_in_series = lv_polarchart_get_point_count(polarchart);
    uint16_t new_point_count = points_in_series;

    lv_polarchart_set_point_count(polarchart, new_point_count);

    TEST_ASSERT_EQUAL_MESSAGE(new_point_count, lv_polarchart_get_point_count(polarchart),
                              "Actual points is not equal to original point count");
    lv_polarchart_remove_series(polarchart, red_series);
}

void test_polarchart_set_new_point_count_as_zero(void)
{
    lv_polarchart_series_t * red_series;
    red_series = lv_polarchart_add_series(polarchart, red_color, LV_POLARCHART_AXIS_RADIAL);

    lv_polarchart_set_point_count(polarchart, 0u);

    TEST_ASSERT_EQUAL_MESSAGE(1u, lv_polarchart_get_point_count(polarchart),
                              "Actual points in polar chart are more than 1");
    lv_polarchart_remove_series(polarchart, red_series);
}

void test_polarchart_point_is_added_at_the_end_of_a_series(void)
{
    lv_polarchart_series_t * red_series;
    red_series = lv_polarchart_add_series(polarchart, red_color, LV_POLARCHART_AXIS_RADIAL);

    TEST_ASSERT_NOT_NULL_MESSAGE(red_series, "Red series not added to polar chart");
    TEST_ASSERT_NULL_MESSAGE(red_series->angle_points,
                             "'Angle' points in non scatter polar chart should not point to anything");

    lv_polarchart_remove_series(polarchart, red_series);
}

void test_polarchart_one_point_when_setting_point_count_to_zero(void)
{
    lv_polarchart_set_point_count(polarchart, 0u);
    TEST_ASSERT_EQUAL(1u, lv_polarchart_get_point_count(polarchart));
}

static void polarchart_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_DRAW_TASK_ADDED) {
        lv_draw_task_t * draw_task = lv_event_get_param(e);
        lv_draw_dsc_base_t * base_dsc = draw_task->draw_dsc;

        lv_polarchart_type_t polarchart_type = lv_polarchart_get_type(obj);
        if(polarchart_type == LV_POLARCHART_TYPE_LINE && base_dsc->part != LV_PART_INDICATOR)  return;

        const lv_polarchart_series_t * ser = NULL;
        for(uint32_t i = 0; i < base_dsc->id1 + 1; i++) {
            ser = lv_polarchart_get_series_next(obj, ser);
        }

        char buf[8];
        lv_snprintf(buf, sizeof(buf), "%"LV_PRIu32, ser->radial_points[base_dsc->id2]);

        lv_point_t text_size;
        lv_text_attributes_t attributes = {0};

        attributes.letter_space = 0;
        attributes.line_space = 0;
        attributes.max_width = LV_COORD_MAX;
        attributes.text_flags = LV_TEXT_FLAG_NONE;

        lv_text_get_size_attributes(&text_size, buf, LV_FONT_DEFAULT, &attributes);

        lv_area_t txt_area;
        txt_area.x1 = draw_task->area.x1 + lv_area_get_width(&draw_task->area) / 2 - text_size.x / 2;
        txt_area.x2 = txt_area.x1 + text_size.x;
        txt_area.y2 = draw_task->area.y1 - LV_DPX(15);
        txt_area.y1 = txt_area.y2 - text_size.y;

        lv_area_t bg_area;
        bg_area.x1 = txt_area.x1 - LV_DPX(8);
        bg_area.x2 = txt_area.x2 + LV_DPX(8);
        bg_area.y1 = txt_area.y1 - LV_DPX(8);
        bg_area.y2 = txt_area.y2 + LV_DPX(8);

        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.bg_color = ser->color;
        rect_dsc.radius = LV_DPX(5);
        lv_draw_rect(base_dsc->layer, &rect_dsc, &bg_area);

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lv_color_white();
        label_dsc.font = LV_FONT_DEFAULT;
        label_dsc.text = buf;
        label_dsc.text_local = true;
        lv_draw_label(base_dsc->layer, &label_dsc, &txt_area);
    }
}

void test_draw_task_hooking(void)
{
    lv_obj_t * polarchart_wrapper = lv_obj_create(lv_screen_active());
    lv_obj_set_size(polarchart_wrapper, 600, 300);
    lv_obj_center(polarchart_wrapper);

    lv_obj_set_parent(polarchart, polarchart_wrapper);
    lv_obj_set_size(polarchart, lv_pct(200), lv_pct(100));
    lv_obj_add_flag(polarchart, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_polarchart_set_div_line_count(polarchart, 0, 21);
    lv_polarchart_set_point_count(polarchart, 21);
    lv_obj_add_event_cb(polarchart, polarchart_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);

    lv_polarchart_series_t * polarchart_set1 = lv_polarchart_add_series(polarchart, lv_palette_main(LV_PALETTE_YELLOW),
                                                                        0);
    lv_polarchart_series_t * polarchart_set2 = lv_polarchart_add_series(polarchart, lv_palette_main(LV_PALETTE_GREEN),
                                                                        0);
    lv_polarchart_series_t * polarchart_set3 = lv_polarchart_add_series(polarchart, lv_palette_main(LV_PALETTE_RED), 0);
    lv_polarchart_set_axis_range(polarchart, LV_POLARCHART_AXIS_RADIAL, 0, 700);

    int32_t points[21] = {0, 31, 59, 81, 95, 100, 95, 81, 59, 31, 0, -31, -59, -81, -95, -100, -95, -81, -59, -31, 0};

    for(uint32_t i = 0; i < 21; i++) {
        lv_polarchart_set_next_value(polarchart, polarchart_set1, points[i] + 100);
        lv_polarchart_set_next_value(polarchart, polarchart_set2, points[i] + 300);
        lv_polarchart_set_next_value(polarchart, polarchart_set3, points[i] + 500);
    }

    lv_obj_scroll_to_x(polarchart_wrapper, 300, LV_ANIM_OFF);

    lv_polarchart_set_type(polarchart, LV_POLARCHART_TYPE_LINE);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/polarchart_line_draw_hook.png");
}

void test_polarchart_scatter(void)
{
    lv_obj_center(polarchart);
    lv_obj_set_size(polarchart, LV_PCT(100), LV_PCT(100));

    lv_polarchart_set_type(polarchart, LV_POLARCHART_TYPE_SCATTER);

    lv_polarchart_set_axis_range(polarchart, LV_POLARCHART_AXIS_ANGLE, 50, 100);
    lv_polarchart_set_axis_range(polarchart, LV_POLARCHART_AXIS_RADIAL, 10, 20);

    lv_polarchart_set_point_count(polarchart, 3);
    lv_polarchart_series_t * ser = lv_polarchart_add_series(polarchart, red_color, LV_POLARCHART_AXIS_RADIAL);
    //lv_polarchart_set_next_value2(polarchart, ser, 50, 10);
    //lv_polarchart_set_next_value2(polarchart, ser, 75, 12);
    //lv_polarchart_set_next_value2(polarchart, ser, 100, 20);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/polarchart_scatter.png");
}

#endif
