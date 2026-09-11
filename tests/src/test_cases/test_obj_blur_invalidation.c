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

/* Large enough for every test's capture window. */
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

/* Start capturing invalidated areas. The callback is only attached while
 * capturing, so the captured list holds exactly the areas the action under test
 * produced, not the areas from creating and first rendering the objects. */
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

/* True if any captured invalidated area overlaps the object's coordinates. */
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

/* In LV_DISPLAY_RENDER_MODE_FULL any change redraws the whole screen and
 * lv_inv_area() sends no LV_EVENT_INVALIDATE_AREA, so there is no per-area
 * expansion to observe. */
static void skip_in_full_render_mode(void)
{
    if(lv_display_get_render_mode(disp) == LV_DISPLAY_RENDER_MODE_FULL) {
        TEST_IGNORE_MESSAGE("No per-area invalidation in LV_DISPLAY_RENDER_MODE_FULL");
    }
}

/* Invalidating an object behind a blur object must also invalidate the blur
 * object's full extent because its background changed. The changed object is
 * small and in one corner. The assertion checks the opposite corner of the
 * blur object, which only the full expansion can invalidate. */
void test_change_behind_blur_invalidates_the_blur_object(void)
{
    skip_in_full_render_mode();
    lv_obj_t * blur = create_blur_obj(100, 100, 80, 80);  /* ~100..180 */
    lv_obj_t * behind = create_obj(105, 105, 10, 10);     /* top-left corner, inside blur */
    lv_refr_now(NULL);

    /* A patch in the blur object's far (bottom-right) corner, away from the
     * changed object so its own invalidation cannot cover it. */
    lv_area_t far_corner = {blur->coords.x2 - 10, blur->coords.y2 - 10, blur->coords.x2, blur->coords.y2};
    TEST_ASSERT_FALSE(lv_area_is_on(&behind->coords, &far_corner));

    invalidate_and_refresh(behind);

    TEST_ASSERT_TRUE(area_was_invalidated(&far_corner));
}

/* Invalidating an object that does not overlap the blur object must not
 * invalidate the blur object. */
void test_change_away_from_blur_leaves_it_untouched(void)
{
    skip_in_full_render_mode();
    lv_obj_t * blur = create_blur_obj(10, 10, 40, 40); /* top-left corner */
    lv_obj_t * behind = create_obj(300, 250, 20, 20);  /* far away, bottom-right */
    lv_refr_now(NULL);

    invalidate_and_refresh(behind);

    TEST_ASSERT_FALSE(obj_area_was_invalidated(blur));
}

/* Transitive chain: blur object b1 sits over the changed object and blur object
 * b2 overlaps b1's right edge but not the changed object. Expanding b1
 * invalidates b1's area, and the fixed-point loop must then see b2 overlapping
 * that area and invalidate b2 too. A single pass would miss b2.
 *
 * The assertion checks b2's far-right region, beyond b1 and its blur-expanded
 * extent, so only b2's own expansion can invalidate it. */
void test_blur_over_blur_is_invalidated_transitively(void)
{
    skip_in_full_render_mode();
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

/* A blur object that is a descendant of the invalidated object must be expanded
 * too. The child's full extent must be redrawn even when the change overlaps
 * only a corner of it. */
void test_blur_child_of_invalidated_parent_is_expanded(void)
{
    skip_in_full_render_mode();
    lv_obj_t * parent = create_obj(100, 100, 200, 200);   /* ~100..299 */
    lv_obj_t * blur_child = lv_obj_create(parent);
    lv_obj_remove_style_all(blur_child);
    lv_obj_set_pos(blur_child, 20, 20);                   /* abs ~120..219 */
    lv_obj_set_size(blur_child, 100, 100);
    lv_obj_set_style_blur_radius(blur_child, 10, 0);
    lv_refr_now(NULL);

    /* A small patch of the parent overlapping only the child's top-left corner. */
    lv_area_t patch = {blur_child->coords.x1 - 5, blur_child->coords.y1 - 5,
                       blur_child->coords.x1 + 5, blur_child->coords.y1 + 5
                      };

    /* Far corner of the child, well away from the patch. Only a full expansion
     * of the child itself can invalidate it. */
    lv_area_t far_corner = {blur_child->coords.x2 - 10, blur_child->coords.y2 - 10,
                            blur_child->coords.x2, blur_child->coords.y2
                           };
    TEST_ASSERT_FALSE(lv_area_is_on(&patch, &far_corner));

    start_capture();
    lv_obj_invalidate_area(parent, &patch);
    lv_refr_now(NULL);
    stop_capture();

    TEST_ASSERT_TRUE(area_was_invalidated(&far_corner));
}

/* has_blur() is also true for an object with a drop shadow, so a change behind a
 * drop-shadow object must invalidate it in full as well. Same far-corner check
 * as the blur case, but the object carries a drop shadow instead of a blur. */
void test_change_behind_drop_shadow_invalidates_the_object(void)
{
    skip_in_full_render_mode();
    lv_obj_t * shadow = create_obj(100, 100, 80, 80); /* ~100..180 */
    lv_obj_set_style_drop_shadow_opa(shadow, LV_OPA_COVER, 0);
    lv_obj_t * behind = create_obj(105, 105, 10, 10); /* top-left corner, inside shadow */
    lv_refr_now(NULL);

    lv_area_t far_corner = {shadow->coords.x2 - 10, shadow->coords.y2 - 10, shadow->coords.x2, shadow->coords.y2};
    TEST_ASSERT_FALSE(lv_area_is_on(&behind->coords, &far_corner));

    invalidate_and_refresh(behind);

    TEST_ASSERT_TRUE(area_was_invalidated(&far_corner));
}

/* The expansion handles a frame with many invalidations at once, so the inner
 * loop must scan more than one invalidated area. Two disjoint objects in
 * opposite corners of a blur object are invalidated in the same frame (with
 * these coordinates they stay separate rather than being joined), and the blur
 * object's center, covered by neither, must still be invalidated. Only the
 * full expansion can do that. */
void test_multiple_disjoint_changes_behind_blur(void)
{
    skip_in_full_render_mode();
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
    /* Both invalidations were recorded as separate areas, so the expansion
     * runs with more than one area to scan. */
    TEST_ASSERT_GREATER_THAN_UINT32(1, captured_cnt);
    lv_refr_now(NULL);
    stop_capture();

    TEST_ASSERT_TRUE(area_was_invalidated(&center));
}

/* A transformed blur object renders at its transformed area, not at its
 * untransformed coordinates. A change behind the transformed area must
 * invalidate all of it. */
void test_change_behind_rotated_blur_invalidates_transformed_extent(void)
{
    skip_in_full_render_mode();
    lv_obj_t * blur = create_blur_obj(300, 200, 120, 120);
    lv_obj_set_style_transform_pivot_x(blur, 0, 0);
    lv_obj_set_style_transform_pivot_y(blur, 0, 0);
    lv_obj_set_style_transform_rotation(blur, 900, 0); /* 90 degrees */
    lv_refr_now(NULL);

    /* Where the blur object actually renders. */
    lv_area_t tr;
    tr = blur->coords;
    lv_obj_get_transformed_area(blur, &tr, LV_OBJ_POINT_TRANSFORM_FLAG_RECURSIVE);

    /* The rotation must move the transformed area almost fully off the
     * untransformed coordinates, otherwise this test could pass without any
     * transform handling at all. */
    lv_area_t overlap;
    bool on = lv_area_intersect(&overlap, &tr, &blur->coords);
    TEST_ASSERT_TRUE(!on || lv_area_get_width(&overlap) <= 2 || lv_area_get_height(&overlap) <= 2);

    /* A change behind the transformed area, near its top-left corner. */
    lv_obj_t * behind = create_obj(tr.x1 + 16, tr.y1 + 16, 10, 10);
    lv_refr_now(NULL);

    /* Probe the center of the transformed area. It is away from the changed
     * object, the area's edges and the untransformed coordinates, so only a
     * full expansion of the transformed area can invalidate it. */
    int32_t cx = (tr.x1 + tr.x2) / 2;
    int32_t cy = (tr.y1 + tr.y2) / 2;
    lv_area_t probe = {cx - 5, cy - 5, cx + 5, cy + 5};
    TEST_ASSERT_FALSE(lv_area_is_on(&behind->coords, &probe));
    TEST_ASSERT_FALSE(lv_area_is_on(&blur->coords, &probe));

    invalidate_and_refresh(behind);

    TEST_ASSERT_TRUE(area_was_invalidated(&probe));
}

#endif
