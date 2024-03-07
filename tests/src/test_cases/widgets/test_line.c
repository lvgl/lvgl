#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * line = NULL;

static const uint16_t default_point_num = 0U;
static const int32_t initial_extra_draw_size = 5U;
static const int32_t final_extra_draw_size = 10U;

void setUp(void)
{
    active_screen = lv_screen_active();
    line = lv_line_create(active_screen);
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_line_should_have_valid_documented_default_values(void)
{
    lv_line_t * line_ptr = (lv_line_t *) line;
    TEST_ASSERT_EQUAL_UINT16(default_point_num, line_ptr->point_num);
    TEST_ASSERT_NULL(line_ptr->point_array);
    TEST_ASSERT_FALSE(lv_line_get_y_invert(line));
    TEST_ASSERT_FALSE(lv_obj_has_flag(line, LV_OBJ_FLAG_CLICKABLE));
    /* line doesn't have any points, so it's 0,0 in size */
    TEST_ASSERT_EQUAL_UINT16(0U, lv_obj_get_self_width(line));
    TEST_ASSERT_EQUAL_UINT16(0U, lv_obj_get_self_height(line));
}

void test_line_should_return_valid_y_invert(void)
{
    lv_line_set_y_invert(line, true);
    TEST_ASSERT_TRUE(lv_line_get_y_invert(line));
}

void test_line_size_should_be_updated_after_adding_points(void)
{
    static lv_point_precise_t points[] = { {5, 5} };
    uint16_t point_cnt = (uint16_t) sizeof(points) / sizeof(lv_point_precise_t);
    lv_line_set_points(line, points, point_cnt);

    int32_t calculated_width = 0;
    int32_t calculated_height = 0;

    /* Get the biggest coordinate on both axis */
    uint16_t point_idx = 0;
    for(point_idx = 0; point_idx < point_cnt; point_idx++) {
        calculated_width = (int32_t)LV_MAX(points[point_idx].x, calculated_width);
        calculated_height = (int32_t)LV_MAX(points[point_idx].y, calculated_height);
    }

    TEST_ASSERT_EQUAL_UINT16(calculated_width, lv_obj_get_self_width(line));
    TEST_ASSERT_EQUAL_UINT16(calculated_height, lv_obj_get_self_height(line));
}

static void line_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        /* Set the new line extra draw size */
        lv_event_set_ext_draw_size(e, initial_extra_draw_size);
    }
}

void test_line_should_update_extra_draw_size_based_on_style(void)
{
    /* Setup an event handler for line extra draw size event */
    lv_obj_add_event_cb(line, line_event_cb, LV_EVENT_ALL, NULL);
    /* Trigger the extra draw size event */
    lv_obj_refresh_ext_draw_size(line);

    TEST_ASSERT_EQUAL(initial_extra_draw_size, _lv_obj_get_ext_draw_size(line));

    /* Update line width style, the event handler should set the extra draw size
     * to the line width */
    lv_obj_set_style_line_width(line, final_extra_draw_size, LV_PART_MAIN);

    /* Trigger the extra draw size event */
    lv_obj_refresh_ext_draw_size(line);

    TEST_ASSERT_EQUAL(final_extra_draw_size, _lv_obj_get_ext_draw_size(line));
}

void test_line_basic_render(void)
{
    static lv_point_precise_t points[] = { {5, 5},
        {100, 5},    /*Horizontal*/
        {100, 100},  /*Vertical*/
        {120, 5},    /*Steep*/
        {200, 20},   /*Flat*/
    };
    uint16_t point_cnt = (uint16_t) sizeof(points) / sizeof(lv_point_precise_t);
    lv_line_set_points(line, points, point_cnt);
    lv_obj_set_pos(line, 10, 10);

    line = lv_line_create(active_screen);
    lv_line_set_points(line, points, point_cnt);
    lv_obj_set_pos(line, 400, 0);
    lv_obj_set_style_line_width(line, 5, LV_PART_MAIN);
    lv_obj_set_style_line_dash_gap(line, 3, LV_PART_MAIN);
    lv_obj_set_style_line_dash_width(line, 10, LV_PART_MAIN);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/line_1.png");
}

void test_line_dash_gap(void)
{
    static lv_point_precise_t line_points1[3] = { {50, 50}, {250, 50}, {250, 250} };
    static lv_point_precise_t line_points2[3] = { {50, 250}, {50, 50}, {250, 50} };

    lv_obj_t * line1;
    line1 = lv_line_create(lv_screen_active());
    lv_line_set_points(line1, line_points1, 3);
    lv_obj_set_style_line_width(line1, 1, LV_PART_MAIN);
    lv_obj_set_style_line_dash_width(line1, 1, LV_PART_MAIN);
    lv_obj_set_style_line_dash_gap(line1, 1, LV_PART_MAIN);

    lv_obj_align(line1, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t * line2;
    line2 = lv_line_create(lv_screen_active());
    lv_line_set_points(line2, line_points2, 3);
    lv_obj_set_style_line_width(line2, 2, LV_PART_MAIN);
    lv_obj_set_style_line_dash_width(line2, 2, LV_PART_MAIN);
    lv_obj_set_style_line_dash_gap(line2, 2, LV_PART_MAIN);

    lv_obj_center(line2);

    lv_obj_t * line3;
    line3 = lv_line_create(lv_screen_active());
    lv_line_set_points(line3, line_points2, 3);
    lv_obj_set_style_line_width(line3, 5, LV_PART_MAIN);
    lv_obj_set_style_line_dash_width(line3, 3, LV_PART_MAIN);
    lv_obj_set_style_line_dash_gap(line3, 1, LV_PART_MAIN);

    lv_obj_align(line3, LV_ALIGN_RIGHT_MID, 0, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/line_2.png");
}

#endif
