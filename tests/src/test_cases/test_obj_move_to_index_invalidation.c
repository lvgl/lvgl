#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

/* Tests that lv_obj_move_to_index() invalidates what the moved object draws,
 * not its whole parent.
 *
 * A reorder only changes the stacking order, so only pixels drawn by the moved
 * object can look different. Invalidating the whole parent makes a reorder in a
 * screen-sized parent redraw the whole screen. */

static lv_display_t * disp;

#define MAX_CAPTURED 64
static lv_area_t captured[MAX_CAPTURED];
static uint32_t captured_cnt;

void setUp(void)
{
    disp = lv_display_get_default();
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

/* Record every area the display is asked to invalidate while capturing. */
static void invalidate_area_cb(lv_event_t * e)
{
    TEST_ASSERT_LESS_THAN_UINT32(MAX_CAPTURED, captured_cnt);
    lv_area_t * area = lv_event_get_param(e);
    captured[captured_cnt++] = *area;
}

static void start_capture(void)
{
    /*Draw everything pending so only the reorder is captured*/
    lv_refr_now(disp);
    captured_cnt = 0;
    lv_display_add_event_cb(disp, invalidate_area_cb, LV_EVENT_INVALIDATE_AREA, NULL);
}

static void stop_capture(void)
{
    lv_display_remove_event_cb_with_user_data(disp, invalidate_area_cb, NULL);
}

/* True if any captured invalidated area covers the whole given area. */
static bool area_was_invalidated(const lv_area_t * area)
{
    for(uint32_t i = 0; i < captured_cnt; i++) {
        if(lv_area_is_in(area, &captured[i], 0)) return true;
    }
    return false;
}

/* True if any captured invalidated area touches the given area. */
static bool area_was_touched(const lv_area_t * area)
{
    lv_area_t common;
    for(uint32_t i = 0; i < captured_cnt; i++) {
        if(lv_area_intersect(&common, area, &captured[i])) return true;
    }
    return false;
}

/* In LV_DISPLAY_RENDER_MODE_FULL any change redraws the whole screen and
 * lv_inv_area() sends no LV_EVENT_INVALIDATE_AREA, so there is no per-area
 * invalidation to observe. */
static void skip_in_full_render_mode(void)
{
    if(lv_display_get_render_mode(disp) == LV_DISPLAY_RENDER_MODE_FULL) {
        TEST_IGNORE_MESSAGE("No per-area invalidation in LV_DISPLAY_RENDER_MODE_FULL");
    }
}

static lv_obj_t * create_box(lv_obj_t * parent, int32_t x, int32_t y, int32_t w, int32_t h)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    return obj;
}

void test_move_to_index_invalidates_only_the_moved_object(void)
{
    skip_in_full_render_mode();

    lv_obj_t * parent = create_box(lv_screen_active(), 0, 0, 400, 400);
    lv_obj_t * a = create_box(parent, 10, 10, 50, 50);
    create_box(parent, 30, 30, 50, 50);
    lv_obj_t * far = create_box(parent, 300, 300, 50, 50);
    lv_obj_update_layout(parent);

    start_capture();
    lv_obj_move_to_index(a, -1);
    stop_capture();

    lv_area_t a_area;
    lv_obj_get_coords(a, &a_area);
    TEST_ASSERT_TRUE(area_was_invalidated(&a_area));

    lv_area_t far_area;
    lv_obj_get_coords(far, &far_area);
    TEST_ASSERT_FALSE(area_was_touched(&far_area));
}

void test_move_to_index_covers_the_extra_draw_area(void)
{
    skip_in_full_render_mode();

    lv_obj_t * parent = create_box(lv_screen_active(), 0, 0, 400, 400);
    lv_obj_t * a = create_box(parent, 50, 50, 50, 50);
    lv_obj_set_style_outline_width(a, 10, 0);
    lv_obj_set_style_outline_opa(a, LV_OPA_COVER, 0);
    create_box(parent, 70, 70, 50, 50);
    lv_obj_update_layout(parent);
    TEST_ASSERT_GREATER_OR_EQUAL_INT32(10, lv_obj_get_ext_draw_size(a));

    start_capture();
    lv_obj_move_to_index(a, -1);
    stop_capture();

    lv_area_t drawn_area;
    lv_obj_get_coords(a, &drawn_area);
    lv_area_increase(&drawn_area, lv_obj_get_ext_draw_size(a), lv_obj_get_ext_draw_size(a));
    TEST_ASSERT_TRUE(area_was_invalidated(&drawn_area));
}

void test_move_to_index_of_hidden_object_invalidates_nothing(void)
{
    skip_in_full_render_mode();

    lv_obj_t * parent = create_box(lv_screen_active(), 0, 0, 400, 400);
    lv_obj_t * hidden = create_box(parent, 10, 10, 50, 50);
    lv_obj_set_hidden(hidden, true);
    create_box(parent, 20, 20, 50, 50);
    lv_obj_update_layout(parent);

    start_capture();
    lv_obj_move_to_index(hidden, -1);
    stop_capture();

    TEST_ASSERT_EQUAL_UINT32(0, captured_cnt);
}

#endif
