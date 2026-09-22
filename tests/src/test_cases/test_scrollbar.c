#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

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

static lv_obj_t * scrollbar_test_child(int32_t x, int32_t y)
{
    lv_obj_t * child = lv_obj_create(lv_screen_active());
    lv_obj_set_scrollable(child, false);
    lv_obj_set_size(child, 200, 200);
    lv_obj_set_pos(child, x, y);
    return child;
}

void test_scrollbar_is_redrawn_when_a_child_moves_between_outside_positions(void)
{
    lv_obj_t * child = scrollbar_test_child(-10, -10);
    lv_refr_now(NULL);

    /*Out of the parent before and after the move, so the scrollbar area has to be
     *invalidated even though the child never was inside*/
    lv_obj_set_pos(child, 700, 400);
    lv_refr_now(NULL);

    /*compare_core() so the screen is not invalidated as a whole before the comparison,
     *that would repaint the scrollbars and hide the bug*/
    TEST_ASSERT_MESSAGE(lv_test_screenshot_compare_core("scrollbar_child_moved_outside.png"),
                        "scrollbar_child_moved_outside.png");
}

void test_scrollbar_is_redrawn_when_a_child_moves_into_the_parent(void)
{
    /*Hangs off the bottom edge, so the scrollbars stay visible for the whole test*/
    scrollbar_test_child(500, 400);

    lv_obj_t * mover = scrollbar_test_child(200, 1400);
    lv_refr_now(NULL);

    /*The scrollable area shrinks, so the scrollbar gets longer and has to be redrawn*/
    lv_obj_set_pos(mover, 200, 100);
    lv_refr_now(NULL);

    TEST_ASSERT_MESSAGE(lv_test_screenshot_compare_core("scrollbar_child_moved_inside.png"),
                        "scrollbar_child_moved_inside.png");
}

void test_scrollbar_vertical(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW);

    const int32_t inner_sizes[] = {100, 500, 1000, 5000, 100000, 500000, 1000000, 4000000};
    for(size_t i = 0; i < sizeof(inner_sizes) / sizeof(inner_sizes[0]); i++) {
        lv_obj_t * wrapper = lv_obj_create(lv_screen_active());
        lv_obj_set_flex_grow(wrapper, 1);
        lv_obj_set_height(wrapper, LV_PCT(100));

        lv_obj_t * inner = lv_obj_create(wrapper);
        lv_obj_set_size(inner, LV_PCT(100), inner_sizes[i]);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("scrollbar_vertical.png");
}

void test_scrollbar_horizontal(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);

    const int32_t inner_sizes[] = {100, 500, 1000, 5000, 100000, 500000, 1000000, 4000000};
    for(size_t i = 0; i < sizeof(inner_sizes) / sizeof(inner_sizes[0]); i++) {
        lv_obj_t * wrapper = lv_obj_create(lv_screen_active());
        lv_obj_set_flex_grow(wrapper, 1);
        lv_obj_set_width(wrapper, LV_PCT(100));

        lv_obj_t * inner = lv_obj_create(wrapper);
        lv_obj_set_size(inner, inner_sizes[i], LV_PCT(100));
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("scrollbar_horizontal.png");
}

#endif
