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
 * Inditor width is determined based on both:
 * - Bar size
 * - Bar (main part) padding
 * - Bar value
 * See Boxing model in docs for reference.
 */
void test_bar_indicator_width_should_track_bar_value(void)
{
    lv_bar_t * bar_ptr = (lv_bar_t *) bar;

    static lv_style_t bar_style;

    const lv_coord_t style_padding = 5u;
    const lv_coord_t bar_width = 200u;
    const lv_coord_t bar_height = 20u;

    int32_t bar_value = 10u;
    int32_t actual_width = 0u;
    int32_t expected_width = 0u;

    /* Setup new padding */
    lv_style_init(&bar_style);

    lv_style_set_pad_all(&bar_style, style_padding);

    lv_obj_remove_style_all(bar);
    lv_obj_add_style(bar, &bar_style, LV_PART_MAIN);
    lv_obj_set_size(bar, bar_width, bar_height);

    /* Set a new value */
    lv_bar_set_value(bar, bar_value, LV_ANIM_OFF);
    lv_test_indev_wait(50);

    /* Calculate expected indicator width based on bar properties */
    lv_coord_t sides_padding = 0;
    lv_coord_t bar_max_value = 0;

    bar_max_value = lv_bar_get_max_value(bar);
    sides_padding = lv_obj_get_style_pad_left(bar, LV_PART_MAIN);
    sides_padding += lv_obj_get_style_pad_right(bar, LV_PART_MAIN);

    expected_width = bar_value * (bar_width - sides_padding) / bar_max_value;

    actual_width = lv_area_get_width(&bar_ptr->indic_area);
    TEST_ASSERT_EQUAL_INT32(expected_width, actual_width);
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

#endif
