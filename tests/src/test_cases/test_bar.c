#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#include "lv_test_indev.h"

static lv_obj_t *active_screen = NULL;
static lv_obj_t *bar = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
    bar = lv_bar_create(active_screen);
}

void tearDown(void)
{
}

void test_bar_should_have_valid_default_attributes(void)
{
    TEST_ASSERT_EQUAL(0, lv_bar_get_min_value(bar));
    TEST_ASSERT_EQUAL(100, lv_bar_get_max_value(bar));
    TEST_ASSERT_EQUAL(LV_BAR_MODE_NORMAL, lv_bar_get_mode(bar));
}

/*
 * Bar has two parts, main and indicator, coordinates of the latter are
 * calculated based on:
 * - Bar size
 * - Bar (main part) padding
 * - Bar value
 * - Bar coordinates
 * - Bar base direction
 * See Boxing model in docs for reference.
 *
 * Bar properties assumed:
 * - mode: LV_BAR_MODE_NORMAL
 * - min value: 0
 * - max value: 100
 * - base direction: LTR
 */
void test_bar_should_update_indicator_right_coordinate_based_on_bar_value(void)
{
    lv_bar_t * bar_ptr = (lv_bar_t *) bar;

    static lv_style_t bar_style;

    const lv_coord_t style_padding = 5u;
    const lv_coord_t bar_width = 200u;
    const lv_coord_t bar_height = 20u;
    int32_t bar_value = 10u;

    lv_style_init(&bar_style);
    lv_style_set_pad_all(&bar_style, style_padding);

    /* Setup new style */
    lv_obj_remove_style_all(bar);
    lv_obj_add_style(bar, &bar_style, LV_PART_MAIN);

    /* Set properties */
    lv_obj_set_size(bar, bar_width, bar_height);
    lv_bar_set_value(bar, bar_value, LV_ANIM_OFF);

    /* FIXME: Remove wait */
    lv_test_indev_wait(50);

    int32_t actual_coord = lv_area_get_width(&bar_ptr->indic_area);
    
    /* Calculate bar indicator right coordinate, using rule of 3 */
    lv_coord_t bar_max_value = lv_bar_get_max_value(bar);
    lv_coord_t indicator_part_width = lv_obj_get_content_width(bar);
    lv_coord_t sides_padding = lv_obj_get_style_pad_left(bar, LV_PART_MAIN);
    sides_padding += lv_obj_get_style_pad_right(bar, LV_PART_MAIN);

    int32_t expected_coord = (bar_value * indicator_part_width) / bar_max_value;
    /* NOTE: Add 1 to calculation because the coordinates start at 0 */
    expected_coord += 1;

    TEST_ASSERT_EQUAL_INT32(expected_coord, actual_coord);
}

/*
 * Bar has two parts, main and indicator, coordinates of the latter are
 * calculated based on:
 * - Bar size
 * - Bar (main part) padding
 * - Bar value
 * - Bar coordinates
 * - Bar base direction
 * See Boxing model in docs for reference.
 *
 * Bar properties assumed:
 * - mode: LV_BAR_MODE_NORMAL
 * - min value: 0
 * - max value: 100
 */
void test_bar_rtl_should_update_indicator_left_coordinate_based_on_bar_value(void)
{
    lv_bar_t * bar_ptr = (lv_bar_t *) bar;

    static lv_style_t bar_style;

    const lv_coord_t style_padding = 5u;
    const lv_coord_t bar_width = 200u;
    const lv_coord_t bar_height = 20u;
    int32_t bar_value = 10u;

    lv_style_init(&bar_style);
    lv_style_set_pad_all(&bar_style, style_padding);

    /* Setup new style */
    lv_obj_remove_style_all(bar);
    lv_obj_add_style(bar, &bar_style, LV_PART_MAIN);

    /* Set properties */
    lv_obj_set_size(bar, bar_width, bar_height);
    lv_bar_set_value(bar, bar_value, LV_ANIM_OFF);
    lv_obj_set_style_base_dir(bar, LV_BASE_DIR_RTL, 0);

    /* FIXME: Remove wait */
    lv_test_indev_wait(50);

    int32_t actual_coord = bar_ptr->indic_area.x1;

    /* Calculate current indicator width */
    lv_coord_t bar_max_value = lv_bar_get_max_value(bar);
    lv_coord_t indicator_part_width = lv_obj_get_content_width(bar);
    lv_coord_t right_padding = lv_obj_get_style_pad_right(bar, LV_PART_MAIN);
    int32_t indicator_width = (bar_value * indicator_part_width) / bar_max_value;

    int32_t expected_coord = (bar_width - right_padding) - indicator_width;
    expected_coord -= 1;

    TEST_ASSERT_EQUAL_INT32(expected_coord, actual_coord);
}

void test_bar_indicator_area_should_get_smaller_when_padding_is_increased(void)
{
    lv_bar_t * bar_ptr = (lv_bar_t *) bar;

    const lv_coord_t style_padding = 10u;
    static lv_style_t bar_style;

    int32_t new_height = 0u;
    int32_t new_width = 0u;
    int32_t original_height = 0u;
    int32_t original_width = 0u;

    lv_bar_set_value(bar, 50, LV_ANIM_OFF);
    lv_test_indev_wait(50);

    original_width = lv_area_get_width(&bar_ptr->indic_area);
    original_height = lv_area_get_height(&bar_ptr->indic_area);

    /* Setup new padding */
    lv_style_init(&bar_style);
    lv_style_set_pad_all(&bar_style, style_padding);
    lv_obj_set_size(bar, 100, 50);

    /* Apply new style  */
    lv_obj_remove_style_all(bar);
    lv_obj_add_style(bar, &bar_style, LV_PART_MAIN);

    /* Notify LVGL of style change */
    lv_obj_report_style_change(&bar_style);
    lv_test_indev_wait(50);

    new_height = lv_area_get_height(&bar_ptr->indic_area);
    new_width = lv_area_get_width(&bar_ptr->indic_area);

    TEST_ASSERT_LESS_THAN_INT32(original_height, new_height);
    TEST_ASSERT_LESS_THAN_INT32(original_width, new_width);
}

void test_bar_start_value_should_only_change_when_in_range_mode(void)
{
    int32_t new_start_value = 20u;

    lv_bar_set_value(bar, 90, LV_ANIM_OFF);
    lv_bar_set_start_value(bar, new_start_value, LV_ANIM_OFF);

    /* Start value shouldn't be updated when not in RANGE mode */
    TEST_ASSERT_EQUAL_INT32(0u, lv_bar_get_start_value(bar));

    /* Set bar in RANGE mode so we can edit the start value */
    lv_bar_set_mode(bar, LV_BAR_MODE_RANGE);
    lv_bar_set_start_value(bar, new_start_value, LV_ANIM_OFF);

    TEST_ASSERT_EQUAL_INT32(new_start_value, lv_bar_get_start_value(bar));
}

void test_bar_start_value_should_be_smaller_than_current_value_in_range_mode(void)
{
    /* Set bar in RANGE mode so we can edit the start value */
    lv_bar_set_mode(bar, LV_BAR_MODE_RANGE);
    lv_bar_set_value(bar, 50, LV_ANIM_OFF);
    lv_bar_set_start_value(bar, 100u, LV_ANIM_OFF);

    TEST_ASSERT_EQUAL_INT32(lv_bar_get_value(bar), lv_bar_get_start_value(bar));
}

void test_bar_current_value_should_be_truncated_to_max_value_when_exceeds_it(void)
{
    int32_t max_value = lv_bar_get_max_value(bar);
    int32_t new_value = max_value + 1u;

    lv_bar_set_value(bar, new_value, LV_ANIM_OFF);
    TEST_ASSERT_EQUAL_INT32(max_value, lv_bar_get_value(bar));
}

void test_bar_current_value_should_be_truncated_to_min_value_when_it_is_below_it(void)
{
    int32_t min_value = lv_bar_get_min_value(bar);
    int32_t new_value = min_value - 1u;

    lv_bar_set_value(bar, new_value, LV_ANIM_OFF);
    TEST_ASSERT_EQUAL_INT32(min_value, lv_bar_get_value(bar));
}

/** When in symmetrical mode, the bar indicator has to be drawn towards the min
 * range value. Requires a negative min range value and a positive max range
 * value.
 *
 * Bar properties assumed:
 * - base direction: LTR
 */
void test_bar_indicator_should_be_drawn_towards_the_min_range_side_after_setting_a_more_negative_value(void)
{
    lv_bar_t * bar_ptr = (lv_bar_t *) bar;

    /* Setup bar properties */
    lv_obj_set_size(bar, 100, 50);
    lv_bar_set_mode(bar, LV_BAR_MODE_SYMMETRICAL);
    lv_bar_set_range(bar, -100, 100);

    /* Set bar value to 1, so it gets drawn at the middle of the bar */
    lv_bar_set_value(bar, 1, LV_ANIM_OFF);
    lv_test_indev_wait(50);

    lv_coord_t original_pos = bar_ptr->indic_area.x1;

    /* Set bar to a more negative value */
    lv_bar_set_value(bar, -50, LV_ANIM_OFF);
    lv_test_indev_wait(50);

    lv_coord_t final_pos = bar_ptr->indic_area.x1;

    TEST_ASSERT_LESS_THAN(original_pos, final_pos);
}

#endif
