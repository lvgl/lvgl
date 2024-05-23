#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#include "lv_test_indev.h"

static lv_obj_t * g_active_screen = NULL;
static lv_obj_t * g_bar = NULL;

void setUp(void)
{
    g_active_screen = lv_screen_active();
    g_bar = lv_bar_create(g_active_screen);
}

void tearDown(void)
{
}

void test_bar_should_have_valid_default_attributes(void)
{
    TEST_ASSERT_EQUAL(0, lv_bar_get_min_value(g_bar));
    TEST_ASSERT_EQUAL(100, lv_bar_get_max_value(g_bar));
    TEST_ASSERT_EQUAL(LV_BAR_MODE_NORMAL, lv_bar_get_mode(g_bar));
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
    lv_bar_t * bar_ptr = (lv_bar_t *) g_bar;

    static lv_style_t bar_style;

    const int32_t style_padding = 5u;
    const int32_t bar_width = 200u;
    const int32_t bar_height = 20u;
    int32_t bar_value = 10u;

    lv_style_init(&bar_style);
    lv_style_set_pad_all(&bar_style, style_padding);

    /* Setup new style */
    lv_obj_remove_style_all(g_bar);
    lv_obj_add_style(g_bar, &bar_style, LV_PART_MAIN);

    /* Set properties */
    lv_obj_set_size(g_bar, bar_width, bar_height);
    lv_bar_set_value(g_bar, bar_value, LV_ANIM_OFF);

    /* FIXME: Remove wait */
    lv_test_indev_wait(50);

    int32_t actual_coord = lv_area_get_width(&bar_ptr->indic_area);

    /* Calculate bar indicator right coordinate, using rule of 3 */
    int32_t bar_max_value = lv_bar_get_max_value(g_bar);
    int32_t indicator_part_width = lv_obj_get_content_width(g_bar);

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
    lv_bar_t * bar_ptr = (lv_bar_t *) g_bar;

    static lv_style_t bar_style;

    const int32_t style_padding = 5u;
    const int32_t bar_width = 200u;
    const int32_t bar_height = 20u;
    int32_t bar_value = 10u;

    lv_style_init(&bar_style);
    lv_style_set_pad_all(&bar_style, style_padding);

    /* Setup new style */
    lv_obj_remove_style_all(g_bar);
    lv_obj_add_style(g_bar, &bar_style, LV_PART_MAIN);

    /* Set properties */
    lv_obj_set_size(g_bar, bar_width, bar_height);
    lv_bar_set_value(g_bar, bar_value, LV_ANIM_OFF);
    lv_obj_set_style_base_dir(g_bar, LV_BASE_DIR_RTL, 0);

    /* FIXME: Remove wait */
    lv_test_indev_wait(50);

    int32_t actual_coord = bar_ptr->indic_area.x1;

    /* Calculate current indicator width */
    int32_t bar_max_value = lv_bar_get_max_value(g_bar);
    int32_t indicator_part_width = lv_obj_get_content_width(g_bar);
    int32_t right_padding = lv_obj_get_style_pad_right(g_bar, LV_PART_MAIN);
    int32_t indicator_width = (bar_value * indicator_part_width) / bar_max_value;

    int32_t expected_coord = (bar_width - right_padding) - indicator_width;
    expected_coord -= 1;

    TEST_ASSERT_EQUAL_INT32(expected_coord, actual_coord);
}

void test_bar_normal(void)
{
    int32_t w = 300;
    int32_t h = 40;
    int32_t h_gap = 20;
    int32_t w_gap = 20;
    int32_t y = h_gap;
    int32_t x = w_gap;
    lv_color_t bg_color = lv_color_black();
    lv_color_t indic_color = lv_color_hex(0x0000FF);

    lv_obj_t * screen = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, 800, 480);
    lv_obj_center(screen);
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_100, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    lv_obj_t * test_bar = lv_bar_create(screen);
    lv_obj_set_style_radius(test_bar, 100, 0);
    lv_obj_set_style_radius(test_bar, 100, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(test_bar, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_bar, bg_color, LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_bar, indic_color, LV_PART_INDICATOR);
    lv_bar_set_range(test_bar, 0, 100);
    lv_bar_set_value(test_bar, 30, LV_ANIM_OFF);
    lv_obj_set_size(test_bar, w, h);
    lv_obj_align(test_bar, LV_ALIGN_TOP_LEFT, x, y);
    y += h + h_gap;

    test_bar = lv_bar_create(screen);
    lv_obj_set_style_radius(test_bar, 100, 0);
    lv_obj_set_style_radius(test_bar, 100, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(test_bar, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_bar, bg_color, LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_bar, indic_color, LV_PART_INDICATOR);
    lv_bar_set_range(test_bar, 0, 100);
    lv_bar_set_value(test_bar, 30, LV_ANIM_OFF);
    lv_obj_set_size(test_bar, w, h);
    lv_obj_align(test_bar, LV_ALIGN_TOP_LEFT, x, y);
    lv_obj_set_style_base_dir(test_bar, LV_BASE_DIR_RTL, 0);
    y += h + h_gap;

    x = 150;
    test_bar = lv_bar_create(screen);
    lv_obj_set_style_radius(test_bar, 100, 0);
    lv_obj_set_style_radius(test_bar, 100, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(test_bar, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_bar, bg_color, LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_bar, indic_color, LV_PART_INDICATOR);
    lv_bar_set_range(test_bar, 0, 100);
    lv_bar_set_value(test_bar, 30, LV_ANIM_OFF);
    lv_obj_set_size(test_bar, h, w);
    lv_obj_align(test_bar, LV_ALIGN_TOP_LEFT, x, y);
    x += h + w_gap;

    test_bar = lv_bar_create(screen);
    lv_obj_set_style_radius(test_bar, 100, 0);
    lv_obj_set_style_radius(test_bar, 100, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(test_bar, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_bar, bg_color, LV_PART_MAIN);
    lv_obj_set_style_bg_color(test_bar, indic_color, LV_PART_INDICATOR);
    lv_bar_set_range(test_bar, 100, 0);
    lv_bar_set_value(test_bar, 30, LV_ANIM_OFF);
    lv_obj_set_size(test_bar, h, w);
    lv_obj_align(test_bar, LV_ALIGN_TOP_LEFT, x, y);
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/bar_1.png");
}

void test_bar_indicator_area_should_get_smaller_when_padding_is_increased(void)
{
    lv_bar_t * bar_ptr = (lv_bar_t *) g_bar;

    const int32_t style_padding = 10u;
    static lv_style_t bar_style;

    int32_t new_height = 0u;
    int32_t new_width = 0u;
    int32_t original_height = 0u;
    int32_t original_width = 0u;

    lv_bar_set_value(g_bar, 50, LV_ANIM_OFF);
    lv_test_indev_wait(50);

    original_width = lv_area_get_width(&bar_ptr->indic_area);
    original_height = lv_area_get_height(&bar_ptr->indic_area);

    /* Setup new padding */
    lv_style_init(&bar_style);
    lv_style_set_pad_all(&bar_style, style_padding);
    lv_obj_set_size(g_bar, 100, 50);

    /* Apply new style  */
    lv_obj_remove_style_all(g_bar);
    lv_obj_add_style(g_bar, &bar_style, LV_PART_MAIN);

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

    lv_bar_set_value(g_bar, 90, LV_ANIM_OFF);
    lv_bar_set_start_value(g_bar, new_start_value, LV_ANIM_OFF);

    /* Start value shouldn't be updated when not in RANGE mode */
    TEST_ASSERT_EQUAL_INT32(0u, lv_bar_get_start_value(g_bar));

    /* Set bar in RANGE mode so we can edit the start value */
    lv_bar_set_mode(g_bar, LV_BAR_MODE_RANGE);
    lv_bar_set_start_value(g_bar, new_start_value, LV_ANIM_OFF);

    TEST_ASSERT_EQUAL_INT32(new_start_value, lv_bar_get_start_value(g_bar));
}

void test_bar_start_value_should_be_smaller_than_current_value_in_range_mode(void)
{
    /* Set bar in RANGE mode so we can edit the start value */
    lv_bar_set_mode(g_bar, LV_BAR_MODE_RANGE);
    lv_bar_set_value(g_bar, 50, LV_ANIM_OFF);
    lv_bar_set_start_value(g_bar, 100u, LV_ANIM_OFF);

    TEST_ASSERT_EQUAL_INT32(lv_bar_get_value(g_bar), lv_bar_get_start_value(g_bar));
}

void test_bar_current_value_should_be_truncated_to_max_value_when_exceeds_it(void)
{
    int32_t max_value = lv_bar_get_max_value(g_bar);
    int32_t new_value = max_value + 1u;

    lv_bar_set_value(g_bar, new_value, LV_ANIM_OFF);
    TEST_ASSERT_EQUAL_INT32(max_value, lv_bar_get_value(g_bar));
}

void test_bar_current_value_should_be_truncated_to_min_value_when_it_is_below_it(void)
{
    int32_t min_value = lv_bar_get_min_value(g_bar);
    int32_t new_value = min_value - 1u;

    lv_bar_set_value(g_bar, new_value, LV_ANIM_OFF);
    TEST_ASSERT_EQUAL_INT32(min_value, lv_bar_get_value(g_bar));
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
    lv_bar_t * bar_ptr = (lv_bar_t *) g_bar;

    /* Setup bar properties */
    lv_obj_set_size(g_bar, 100, 50);
    lv_bar_set_mode(g_bar, LV_BAR_MODE_SYMMETRICAL);
    lv_bar_set_range(g_bar, -100, 100);

    /* Set bar value to 1, so it gets drawn at the middle of the bar */
    lv_bar_set_value(g_bar, 1, LV_ANIM_OFF);
    lv_test_indev_wait(50);

    int32_t original_pos = bar_ptr->indic_area.x1;

    /* Set bar to a more negative value */
    lv_bar_set_value(g_bar, -50, LV_ANIM_OFF);
    lv_test_indev_wait(50);

    int32_t final_pos = bar_ptr->indic_area.x1;

    TEST_ASSERT_LESS_THAN(original_pos, final_pos);
}

static lv_obj_t * styled_bar_create(bool ver, int32_t start_value, int32_t end_value, lv_grad_dir_t grad_dir,
                                    int32_t bg_radius, int32_t indic_radius, int32_t bg_pad)
{
    lv_obj_t * bar = lv_bar_create(lv_screen_active());
    if(ver) lv_obj_set_size(bar, 20, 100);
    else lv_obj_set_size(bar, 100, 20);
    lv_bar_set_range(bar, 0, 100);
    lv_bar_set_mode(bar, LV_BAR_MODE_RANGE);
    lv_bar_set_value(bar, end_value, LV_ANIM_OFF);
    lv_bar_set_start_value(bar, start_value, LV_ANIM_OFF);

    lv_obj_set_style_bg_opa(bar, 255, LV_PART_MAIN);
    lv_obj_set_style_bg_color(bar, lv_color_hex3(0x0ff), LV_PART_MAIN);
    lv_obj_set_style_radius(bar, bg_radius, LV_PART_MAIN);
    lv_obj_set_style_pad_all(bar, bg_pad, LV_PART_MAIN);

    lv_obj_set_style_bg_opa(bar, LV_OPA_70, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(bar, lv_color_hex3(0xf0f), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(bar, lv_color_hex3(0x8f8), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(bar, grad_dir, LV_PART_INDICATOR);

    lv_obj_set_style_border_width(bar, 2, LV_PART_INDICATOR);
    lv_obj_set_style_border_color(bar, lv_color_hex3(0x0f0), LV_PART_INDICATOR);
    lv_obj_set_style_border_opa(bar, LV_OPA_70, LV_PART_INDICATOR);
    lv_obj_set_style_outline_width(bar, 2, LV_PART_INDICATOR);
    lv_obj_set_style_outline_pad(bar, 4, LV_PART_INDICATOR);
    lv_obj_set_style_outline_color(bar, lv_color_hex3(0xff0), LV_PART_INDICATOR);
    lv_obj_set_style_outline_opa(bar, LV_OPA_70, LV_PART_INDICATOR);
    lv_obj_set_style_shadow_width(bar, 20, LV_PART_INDICATOR);
    lv_obj_set_style_shadow_spread(bar, 5, LV_PART_INDICATOR);
    lv_obj_set_style_shadow_color(bar, lv_color_hex3(0xf00), LV_PART_INDICATOR);
    lv_obj_set_style_shadow_opa(bar, LV_OPA_70, LV_PART_INDICATOR);
    lv_obj_set_style_radius(bar, indic_radius, LV_PART_INDICATOR);

    return bar;
}

static void render_test_screen_create(bool ver, lv_grad_dir_t grad_dir, const char * ref_img_path)
{
    lv_obj_t * active_screen = lv_screen_active();
    lv_obj_clean(active_screen);

    lv_obj_set_flex_flow(active_screen, ver ? LV_FLEX_FLOW_ROW_WRAP : LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_set_flex_align(active_screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_obj_set_style_pad_row(active_screen, 17, 0);
    lv_obj_set_style_pad_column(active_screen, 12, 0);
    lv_obj_set_style_pad_all(active_screen, 15, 0);

    int32_t bar_values[6][2] = {{0, 5}, {0, 50}, {0, 95}, {5, 100}, {95, 100}, {20, 90}};
    int32_t bg_radius[2] = {0, 15};
    int32_t indic_radius[2] = {0, 15};
    int32_t bg_pad[3] = {-5, 0, 5};

    uint32_t pad;
    for(pad = 0; pad < 3; pad++) {
        uint32_t indic_r;
        for(indic_r = 0; indic_r < 2; indic_r++) {
            uint32_t bg_r;
            for(bg_r = 0; bg_r < 2; bg_r++) {
                uint32_t v;
                for(v = 0; v < 6; v++) {
                    styled_bar_create(ver, bar_values[v][0], bar_values[v][1], grad_dir, bg_radius[bg_r], indic_radius[indic_r],
                                      bg_pad[pad]);
                }
            }
        }
    }

    TEST_ASSERT_EQUAL_SCREENSHOT(ref_img_path);
}

void test_bar_render_corner(void)
{
    render_test_screen_create(false, LV_GRAD_DIR_NONE, "widgets/bar_corner_1.png");
    render_test_screen_create(false, LV_GRAD_DIR_HOR, "widgets/bar_corner_2.png");
    render_test_screen_create(false, LV_GRAD_DIR_VER, "widgets/bar_corner_3.png");
    render_test_screen_create(true, LV_GRAD_DIR_NONE, "widgets/bar_corner_4.png");
    render_test_screen_create(true, LV_GRAD_DIR_HOR, "widgets/bar_corner_5.png");
    render_test_screen_create(true, LV_GRAD_DIR_VER, "widgets/bar_corner_6.png");
}

#endif
