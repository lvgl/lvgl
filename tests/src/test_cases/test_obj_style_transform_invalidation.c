#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

/* Tests that changing a style property which affects the drawn area invalidates
 * the *old* area too.
 *
 * A transformed widget is drawn outside its coordinates, and the area to
 * invalidate is computed from the current style values. So if the new value is
 * written first and the widget is invalidated only afterwards, the area the
 * widget used to occupy is never redrawn and the old pixels stay on the screen.
 * Every path that can change such a property therefore has to invalidate before
 * the change.
 *
 * The tests shrink the drawn area (so the new area can't cover the old one) and
 * check through LV_EVENT_INVALIDATE_AREA that the part of the old area which
 * the widget no longer covers was invalidated. */

static lv_display_t * disp;

#define MAX_CAPTURED 64
static lv_area_t captured[MAX_CAPTURED];
static uint32_t captured_cnt;

/* Styles used by the tests. They outlive the widget in every case. */
static lv_style_t style_big;
static lv_style_t style_small;
static lv_style_t style_plain;
static lv_style_transition_dsc_t trans_dsc;

void setUp(void)
{
    disp = lv_display_get_default();

    lv_style_init(&style_big);
    lv_style_set_transform_scale_y(&style_big, 4 * 256);

    lv_style_init(&style_small);
    lv_style_set_transform_scale_y(&style_small, 256);

    /*No property that affects the drawn area*/
    lv_style_init(&style_plain);
    lv_style_set_bg_color(&style_plain, lv_color_hex(0x00ff00));
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
    lv_style_reset(&style_big);
    lv_style_reset(&style_small);
    lv_style_reset(&style_plain);
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

/* In LV_DISPLAY_RENDER_MODE_FULL any change redraws the whole screen and
 * lv_inv_area() sends no LV_EVENT_INVALIDATE_AREA, so there is nothing to
 * observe. */
static void skip_in_full_render_mode(void)
{
    if(lv_display_get_render_mode(disp) == LV_DISPLAY_RENDER_MODE_FULL) {
        TEST_IGNORE_MESSAGE("No per-area invalidation in LV_DISPLAY_RENDER_MODE_FULL");
    }
}

/* Where the widget is really drawn right now. */
static lv_area_t drawn_area(lv_obj_t * obj)
{
    lv_area_t a = obj->coords;
    lv_obj_get_transformed_area(obj, &a, LV_OBJ_POINT_TRANSFORM_FLAG_RECURSIVE);
    return a;
}

/* A styleless widget in the middle of the screen, scaled up 4x vertically so
 * there is a large area above and below it that a later shrink has to give
 * back. */
static lv_obj_t * create_scaled_obj(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, 100, 100);
    lv_obj_set_size(obj, 60, 40);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_add_style(obj, &style_big, 0);
    lv_refr_now(NULL);
    return obj;
}

/* Assert that the strip the widget dropped when it shrank from `before` to its
 * current area was invalidated. The default pivot is the top left corner, so a
 * smaller scale_y releases a strip below the new area. Only an invalidation done
 * before the change can cover it. */
static void assert_released_strip_was_invalidated(lv_obj_t * obj, const lv_area_t * before)
{
    lv_area_t after = drawn_area(obj);

    /*The widget really shrank, otherwise the new area alone could cover the old*/
    TEST_ASSERT_LESS_THAN_INT32(before->y2 - 4, after.y2);

    lv_area_t strip = {before->x1, after.y2 + 2, before->x2, before->y2};
    TEST_ASSERT_TRUE(area_was_invalidated(&strip));
}

/* Baseline: a local style property. */
void test_local_style_prop_invalidates_the_old_area(void)
{
    skip_in_full_render_mode();
    lv_obj_t * obj = create_scaled_obj();
    lv_area_t before = drawn_area(obj);

    start_capture();
    lv_obj_set_style_transform_scale_y(obj, 256, 0);
    stop_capture();

    assert_released_strip_was_invalidated(obj, &before);
}

/* Adding a style that overrides the transform. */
void test_add_style_invalidates_the_old_area(void)
{
    skip_in_full_render_mode();
    lv_obj_t * obj = create_scaled_obj();
    lv_area_t before = drawn_area(obj);

    start_capture();
    lv_obj_add_style(obj, &style_small, 0);
    stop_capture();

    assert_released_strip_was_invalidated(obj, &before);
}

/* Removing the style that provides the transform. */
void test_remove_style_invalidates_the_old_area(void)
{
    skip_in_full_render_mode();
    lv_obj_t * obj = create_scaled_obj();
    lv_area_t before = drawn_area(obj);

    start_capture();
    lv_obj_remove_style(obj, &style_big, 0);
    stop_capture();

    assert_released_strip_was_invalidated(obj, &before);
}

/* lv_obj_remove_style_all() passes no style, so the properties it drops can't be
 * checked for flags and it has to invalidate unconditionally. */
void test_remove_style_all_invalidates_the_old_area(void)
{
    skip_in_full_render_mode();
    lv_obj_t * obj = create_scaled_obj();
    lv_area_t before = drawn_area(obj);

    start_capture();
    lv_obj_remove_style_all(obj);
    stop_capture();

    assert_released_strip_was_invalidated(obj, &before);
}

/* Replacing the style that provides the transform with a smaller one. */
void test_replace_style_invalidates_the_old_area(void)
{
    skip_in_full_render_mode();
    lv_obj_t * obj = create_scaled_obj();
    lv_area_t before = drawn_area(obj);

    start_capture();
    TEST_ASSERT_TRUE(lv_obj_replace_style(obj, &style_big, &style_small, 0));
    stop_capture();

    assert_released_strip_was_invalidated(obj, &before);
}

/* Disabling the style that provides the transform. */
void test_disable_style_invalidates_the_old_area(void)
{
    skip_in_full_render_mode();
    lv_obj_t * obj = create_scaled_obj();
    lv_area_t before = drawn_area(obj);

    start_capture();
    lv_obj_set_style_enabled(obj, &style_big, 0, false);
    stop_capture();

    assert_released_strip_was_invalidated(obj, &before);
}

/* Start a scale_y transition from 4x down to 1x and stop half way, so a
 * transition is running and the widget is drawn larger than its coordinates. */
static lv_obj_t * create_obj_with_running_transition(void)
{
    static const lv_style_prop_t props[] = {LV_STYLE_TRANSFORM_SCALE_Y, LV_STYLE_PROP_INV};

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, 100, 100);
    lv_obj_set_size(obj, 60, 40);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);

    /*4x while pressed, 1x otherwise*/
    lv_obj_add_style(obj, &style_big, LV_STATE_PRESSED);

    lv_style_transition_dsc_init(&trans_dsc, props, lv_anim_path_linear, 400, 0, NULL);
    lv_obj_set_style_transition(obj, &trans_dsc, 0);

    /*A transition is only started for a widget that was already rendered*/
    lv_refr_now(NULL);

    lv_obj_add_state(obj, LV_STATE_PRESSED);
    lv_test_wait(400);
    TEST_ASSERT_EQUAL_INT32(4 * 256, lv_obj_get_style_transform_scale_y(obj, LV_PART_MAIN));

    /*Transition back to 1x, stopped half way*/
    lv_obj_remove_state(obj, LV_STATE_PRESSED);
    lv_test_wait(200);
    int32_t scale = lv_obj_get_style_transform_scale_y(obj, LV_PART_MAIN);
    TEST_ASSERT_GREATER_THAN_INT32(256 + 32, scale);
    TEST_ASSERT_LESS_THAN_INT32(4 * 256, scale);

    return obj;
}

/* Every step of a transform transition shrinks the drawn area, so each step has
 * to invalidate the area the widget had before the step. */
void test_transition_step_invalidates_the_old_area(void)
{
    skip_in_full_render_mode();
    lv_obj_t * obj = create_obj_with_running_transition();
    lv_area_t before = drawn_area(obj);

    start_capture();
    lv_tick_inc(100);
    lv_timer_handler();
    stop_capture();

    assert_released_strip_was_invalidated(obj, &before);
}

/* Killing a running transform transition changes the drawn area right away.
 * The style being added carries no property that affects the drawn area, so
 * only the transition removal itself can invalidate the old area. */
void test_killing_a_transform_transition_invalidates_the_old_area(void)
{
    skip_in_full_render_mode();
    lv_obj_t * obj = create_obj_with_running_transition();
    lv_area_t before = drawn_area(obj);

    start_capture();
    lv_obj_add_style(obj, &style_plain, 0);
    stop_capture();

    assert_released_strip_was_invalidated(obj, &before);
}

#endif
