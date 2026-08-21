#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

/* Tests for deferred blur-invalidation expansion.
 *
 * A blur object samples the pixels behind it, so when anything behind it
 * changes it must be redrawn in full. These tests drive the real path: a
 * normal object behind a blur object is invalidated, a refresh is run (which
 * runs the once-per-frame expansion), and the areas reported through
 * LV_EVENT_INVALIDATE_AREA are inspected to confirm the blur object's area was
 * (or was not) invalidated. */

static lv_display_t * disp;

/* Comfortably larger than the most any test invalidates in one capture window. */
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
    if(captured_cnt >= MAX_CAPTURED) return; /* keep within bounds; no test gets close */
    lv_area_t * area = lv_event_get_param(e);
    captured[captured_cnt++] = *area;
}

/* Start capturing invalidated areas. The callback is only attached while
 * capturing, so the captured list holds exactly the areas the action under test
 * produced -- not the areas from creating and first-rendering the objects. */
static void start_capture(void)
{
    captured_cnt = 0;
    lv_display_add_event_cb(disp, invalidate_area_cb, LV_EVENT_INVALIDATE_AREA, NULL);
}

static void stop_capture(void)
{
    lv_display_remove_event_cb_with_user_data(disp, invalidate_area_cb, NULL);
}

/* Invalidate an object and run a refresh (which runs the blur expansion),
 * capturing the invalidated areas it produces. */
static void invalidate_and_refresh(lv_obj_t * obj)
{
    start_capture();
    lv_obj_invalidate(obj);
    lv_refr_now(NULL);
    stop_capture();
}

/* True if any captured invalidated area overlaps the given area. */
static bool area_was_invalidated(const lv_area_t * area)
{
    for(uint32_t i = 0; i < captured_cnt; i++) {
        if(lv_area_is_on(&captured[i], area)) return true;
    }
    return false;
}

/* True if any captured invalidated area overlaps the object's coordinates --
 * i.e. the object's area was (at least partly) invalidated. */
static bool obj_area_was_invalidated(const lv_obj_t * obj)
{
    return area_was_invalidated(&obj->coords);
}

/* A normal object at (x, y) of the given size on the active screen. */
static lv_obj_t * create_obj(int32_t x, int32_t y, int32_t w, int32_t h)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    return obj;
}

/* Like create_obj() but with a blur set, so it samples the pixels behind it. */
static lv_obj_t * create_blur_obj(int32_t x, int32_t y, int32_t w, int32_t h)
{
    lv_obj_t * obj = create_obj(x, y, w, h);
    lv_obj_set_style_blur_radius(obj, 10, 0);
    return obj;
}

/* Invalidating an object behind a blur object must, after a refresh, also
 * invalidate the blur object's *full* extent -- it has to be redrawn because its
 * background changed. The changed object is small and in one corner; the
 * assertion checks the opposite corner of the blur object, which only the
 * full-extent expansion (not the changed object's own area) can invalidate. */
void test_change_behind_blur_invalidates_the_blur_object(void)
{
    lv_obj_t * blur = create_blur_obj(100, 100, 80, 80);  /* ~100..180 */
    lv_obj_t * behind = create_obj(105, 105, 10, 10);     /* top-left corner, inside blur */
    lv_refr_now(NULL);

    /* A patch in the blur object's far (bottom-right) corner, away from the
     * changed object -- not covered by the changed object's own invalidation. */
    lv_area_t far_corner = {blur->coords.x2 - 10, blur->coords.y2 - 10, blur->coords.x2, blur->coords.y2};
    TEST_ASSERT_FALSE(lv_area_is_on(&behind->coords, &far_corner));

    invalidate_and_refresh(behind);

    TEST_ASSERT_TRUE(area_was_invalidated(&far_corner));
}

/* Invalidating an object that does not overlap the blur object must not
 * invalidate the blur object. */
void test_change_away_from_blur_leaves_it_untouched(void)
{
    lv_obj_t * blur = create_blur_obj(10, 10, 40, 40); /* top-left corner */
    lv_obj_t * behind = create_obj(300, 250, 20, 20);  /* far away, bottom-right */
    lv_refr_now(NULL);

    invalidate_and_refresh(behind);

    TEST_ASSERT_FALSE(obj_area_was_invalidated(blur));
}

/* Transitive chain: blur object b1 sits over the changed object; blur object b2
 * overlaps b1's right edge but not the changed object. Expanding b1 makes its
 * area dirty, and the fixed-point loop must then see b2 overlapping b1's
 * now-dirty area and invalidate b2 too -- a single pass would miss b2.
 *
 * The assertion checks b2's far-right region, which lies beyond b1 (and beyond
 * b1's blur-expanded extent). b1's own invalidation cannot reach there, so that
 * region is invalidated only if b2 was expanded in its own right -- which is
 * exactly the transitive behavior under test. */
void test_blur_over_blur_is_invalidated_transitively(void)
{
    lv_obj_t * b1 = create_blur_obj(100, 100, 80, 80);  /* coords ~100..179 */
    lv_obj_t * b2 = create_blur_obj(175, 100, 80, 80);  /* coords ~175..254, sliver-overlaps b1 */
    lv_obj_t * behind = create_obj(105, 105, 10, 10);   /* inside b1 only, not b2 */
    lv_refr_now(NULL);

    /* The changed object is inside b1 but not b2, so b2 can only be reached
     * transitively through b1. */
    TEST_ASSERT_TRUE(lv_area_is_on(&behind->coords, &b1->coords));
    TEST_ASSERT_FALSE(lv_area_is_on(&behind->coords, &b2->coords));

    /* A patch at b2's far-right edge, clear of b1 and its blur-expanded extent,
     * so only b2's own expansion can invalidate it. */
    lv_area_t b2_far = {b2->coords.x2 - 10, b2->coords.y1, b2->coords.x2, b2->coords.y1 + 10};
    TEST_ASSERT_TRUE(b2_far.x1 > b1->coords.x2 + 32); /* clear of b1 + any blur expansion */

    invalidate_and_refresh(behind);

    TEST_ASSERT_TRUE(obj_area_was_invalidated(b1)); /* b1: direct overlap with the change */
    TEST_ASSERT_TRUE(area_was_invalidated(&b2_far)); /* b2: only reachable via transitive expansion */
}

/* has_blur() is also true for an object with a drop shadow, so a change behind a
 * drop-shadow object must invalidate it in full as well. Same far-corner check
 * as the blur case, but the object carries a drop shadow instead of a blur. */
void test_change_behind_drop_shadow_invalidates_the_object(void)
{
    lv_obj_t * shadow = create_obj(100, 100, 80, 80); /* ~100..180 */
    lv_obj_set_style_drop_shadow_opa(shadow, LV_OPA_COVER, 0);
    lv_obj_t * behind = create_obj(105, 105, 10, 10); /* top-left corner, inside shadow */
    lv_refr_now(NULL);

    lv_area_t far_corner = {shadow->coords.x2 - 10, shadow->coords.y2 - 10, shadow->coords.x2, shadow->coords.y2};
    TEST_ASSERT_FALSE(lv_area_is_on(&behind->coords, &far_corner));

    invalidate_and_refresh(behind);

    TEST_ASSERT_TRUE(area_was_invalidated(&far_corner));
}

/* The expansion's purpose is to handle a frame with many invalidations at once,
 * so the inner loop must scan more than one dirty area. Two disjoint objects in
 * opposite corners of a blur object are invalidated in the same frame (with
 * these coordinates they stay separate rather than being joined), and the blur
 * object's center, covered by neither, must still be invalidated -- which only
 * the full-extent expansion can do. */
void test_multiple_disjoint_changes_behind_blur(void)
{
    lv_obj_t * blur = create_blur_obj(100, 100, 120, 120); /* ~100..220 */
    lv_obj_t * behind_tl = create_obj(105, 105, 10, 10);   /* top-left */
    lv_obj_t * behind_br = create_obj(205, 205, 10, 10);   /* bottom-right */
    lv_refr_now(NULL);

    /* Center patch, covered by neither changed object. */
    int32_t cx = (blur->coords.x1 + blur->coords.x2) / 2;
    int32_t cy = (blur->coords.y1 + blur->coords.y2) / 2;
    lv_area_t center = {cx - 5, cy - 5, cx + 5, cy + 5};
    TEST_ASSERT_FALSE(lv_area_is_on(&behind_tl->coords, &center));
    TEST_ASSERT_FALSE(lv_area_is_on(&behind_br->coords, &center));

    start_capture();
    lv_obj_invalidate(behind_tl);
    lv_obj_invalidate(behind_br);
    /* Both invalidations were recorded as separate dirty areas, so the expansion
     * runs with more than one area to scan (its inner loop). */
    TEST_ASSERT_GREATER_THAN_UINT32(1, captured_cnt);
    lv_refr_now(NULL);
    stop_capture();

    TEST_ASSERT_TRUE(area_was_invalidated(&center));
}

#endif
