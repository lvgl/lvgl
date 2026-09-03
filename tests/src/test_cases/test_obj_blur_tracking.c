#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

/* Tests for the cached per-widget blur status (obj->has_blur) and the global
 * count of widgets with blur (blur_obj_cnt in lv_global_t).
 *
 * The cache is recomputed at the same choke points as the layer-type cache
 * (lv_obj_refresh_style(), state changes) and released on widget deletion, so
 * the count returns to zero when the last blur user goes away and
 * lv_obj_invalidate_expand_blur() skips its tree walk again. The tests drive
 * the transition edges: local properties, style attach/detach/replace,
 * shared-style mutation, state-selective styles, per-widget style disabling
 * and subtree deletion. */

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

static uint32_t blur_cnt(void)
{
    return LV_GLOBAL_DEFAULT()->blur_obj_cnt;
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

/* True if any captured invalidated area overlaps the given area. */
static bool area_was_invalidated(const lv_area_t * area)
{
    for(uint32_t i = 0; i < captured_cnt; i++) {
        if(lv_area_is_on(&captured[i], area)) return true;
    }
    return false;
}

/* A normal object at (x, y) of the given size on the active screen. All styles
 * are removed so only the styles the test sets apply. */
static lv_obj_t * create_obj(int32_t x, int32_t y, int32_t w, int32_t h)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    return obj;
}

/* The baseline: nothing on a plain screen has blur. */
void test_count_zero_without_blur(void)
{
    lv_obj_t * obj = create_obj(10, 10, 50, 50);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_refr_now(NULL);

    TEST_ASSERT_FALSE(obj->has_blur);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());
}

/* The count follows a local blur property up and down, and deletion releases
 * it. */
void test_count_follows_local_blur_prop(void)
{
    lv_obj_t * obj = create_obj(10, 10, 50, 50);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());

    lv_obj_set_style_blur_radius(obj, 10, 0);
    TEST_ASSERT_TRUE(obj->has_blur);
    TEST_ASSERT_EQUAL_UINT32(1, blur_cnt());

    lv_obj_set_style_blur_radius(obj, 0, 0); /* radius 0 = no blur */
    TEST_ASSERT_FALSE(obj->has_blur);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());

    lv_obj_set_style_blur_radius(obj, 5, 0);
    TEST_ASSERT_EQUAL_UINT32(1, blur_cnt());

    lv_obj_delete(obj);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());
}

/* Drop shadow opacity also counts as blur. */
void test_count_follows_drop_shadow(void)
{
    lv_obj_t * obj = create_obj(10, 10, 50, 50);

    lv_obj_set_style_drop_shadow_opa(obj, LV_OPA_COVER, 0);
    TEST_ASSERT_EQUAL_UINT32(1, blur_cnt());

    lv_obj_set_style_drop_shadow_opa(obj, LV_OPA_TRANSP, 0);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());
}

/* Attaching and detaching a shared style with blur moves the count. */
void test_count_follows_style_attach_detach(void)
{
    lv_style_t style;
    lv_style_init(&style);
    lv_style_set_blur_radius(&style, 8);

    lv_obj_t * obj = create_obj(10, 10, 50, 50);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());

    lv_obj_add_style(obj, &style, 0);
    TEST_ASSERT_EQUAL_UINT32(1, blur_cnt());

    lv_obj_remove_style(obj, &style, 0);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());

    lv_obj_add_style(obj, &style, 0);
    TEST_ASSERT_EQUAL_UINT32(1, blur_cnt());

    lv_obj_remove_style_all(obj);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());

    lv_obj_delete(obj);
    lv_style_reset(&style);
}

/* A blur style bound to a state selector only counts while the widget is in
 * that state. Entering and leaving the state involves no style mutation or
 * (re)attachment, so this edge is covered only by the update_obj_state() hook. */
void test_count_follows_state_selective_blur(void)
{
    lv_style_t style;
    lv_style_init(&style);
    lv_style_set_blur_radius(&style, 8);

    lv_obj_t * obj = create_obj(10, 10, 50, 50);
    lv_obj_add_style(obj, &style, LV_STATE_PRESSED);

    TEST_ASSERT_FALSE(obj->has_blur); /* default state: the style doesn't apply */
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());

    lv_obj_add_state(obj, LV_STATE_PRESSED);
    TEST_ASSERT_TRUE(obj->has_blur);
    TEST_ASSERT_EQUAL_UINT32(1, blur_cnt());

    lv_obj_remove_state(obj, LV_STATE_PRESSED);
    TEST_ASSERT_FALSE(obj->has_blur);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());

    lv_obj_delete(obj);
    lv_style_reset(&style);
}

/* Disabling an attached blur style stops it from counting, re-enabling counts
 * it again. */
void test_count_follows_style_disable(void)
{
    lv_style_t style;
    lv_style_init(&style);
    lv_style_set_blur_radius(&style, 8);

    lv_obj_t * obj = create_obj(10, 10, 50, 50);
    lv_obj_add_style(obj, &style, 0);
    TEST_ASSERT_EQUAL_UINT32(1, blur_cnt());

    lv_obj_style_set_disabled(obj, &style, 0, true);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());

    lv_obj_style_set_disabled(obj, &style, 0, false);
    TEST_ASSERT_EQUAL_UINT32(1, blur_cnt());

    lv_obj_delete(obj);
    lv_style_reset(&style);
}

/* Mutating a shared style takes effect through lv_obj_report_style_change(),
 * which refreshes the style's users and updates their cached blur status. */
void test_count_follows_shared_style_mutation(void)
{
    lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_opa(&style, LV_OPA_COVER);

    lv_obj_t * obj = create_obj(10, 10, 50, 50);
    lv_obj_add_style(obj, &style, 0);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());

    lv_style_set_blur_radius(&style, 8);
    lv_obj_report_style_change(&style);
    TEST_ASSERT_EQUAL_UINT32(1, blur_cnt());

    lv_style_remove_prop(&style, LV_STYLE_BLUR_RADIUS);
    lv_obj_report_style_change(&style);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());

    lv_obj_delete(obj);
    lv_style_reset(&style);
}

/* lv_obj_replace_style() swaps style pointers without add/remove, and must
 * still move the count both ways. The blur style is const to also cover
 * styles that never pass through lv_style_set_prop(). */
const lv_style_const_prop_t blur_const_props[] = {
    LV_STYLE_CONST_BLUR_RADIUS(12),
    LV_STYLE_CONST_PROPS_END
};
LV_STYLE_CONST_INIT(blur_const_style, blur_const_props);

void test_count_follows_replace_style(void)
{
    lv_style_t plain;
    lv_style_init(&plain);
    lv_style_set_bg_opa(&plain, LV_OPA_COVER);

    lv_obj_t * obj = create_obj(10, 10, 50, 50);
    lv_obj_add_style(obj, &plain, 0);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());

    TEST_ASSERT_TRUE(lv_obj_replace_style(obj, &plain, &blur_const_style, 0));
    TEST_ASSERT_EQUAL_UINT32(1, blur_cnt());

    TEST_ASSERT_TRUE(lv_obj_replace_style(obj, &blur_const_style, &plain, 0));
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());

    lv_obj_delete(obj);
    lv_style_reset(&plain);
}

/* Deleting a subtree releases the count of every blur widget inside it. */
void test_subtree_delete_releases_count(void)
{
    lv_obj_t * parent = create_obj(10, 10, 200, 200);

    lv_obj_t * child1 = lv_obj_create(parent);
    lv_obj_remove_style_all(child1);
    lv_obj_set_style_blur_radius(child1, 10, 0);

    lv_obj_t * child2 = lv_obj_create(parent);
    lv_obj_remove_style_all(child2);
    lv_obj_set_style_drop_shadow_opa(child2, LV_OPA_COVER, 0);

    lv_obj_t * grandchild = lv_obj_create(child1);
    lv_obj_remove_style_all(grandchild);
    lv_obj_set_style_blur_radius(grandchild, 4, 0);

    TEST_ASSERT_EQUAL_UINT32(3, blur_cnt());

    lv_obj_delete(parent);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());
}

/* End to end: a change behind a blur object expands to its full extent. After
 * the blur is removed the same change no longer does, because the zero count
 * skips the expansion walk. */
void test_expansion_stops_when_blur_removed(void)
{
    /* In LV_DISPLAY_RENDER_MODE_FULL there are no per-area invalidations to observe */
    if(lv_display_get_render_mode(disp) == LV_DISPLAY_RENDER_MODE_FULL) {
        TEST_IGNORE_MESSAGE("No per-area invalidation in LV_DISPLAY_RENDER_MODE_FULL");
    }
    lv_obj_t * blur = create_obj(100, 100, 80, 80);            /* ~100..180 */
    lv_obj_set_style_blur_radius(blur, 10, 0);
    lv_obj_t * behind = create_obj(105, 105, 10, 10);          /* top-left corner, inside blur */
    lv_refr_now(NULL);

    /* A patch in the blur object's far (bottom-right) corner, away from the
     * changed object -- reachable only through the full-extent expansion. */
    lv_area_t far_corner = {blur->coords.x2 - 10, blur->coords.y2 - 10, blur->coords.x2, blur->coords.y2};
    TEST_ASSERT_FALSE(lv_area_is_on(&behind->coords, &far_corner));

    /* With blur: the change behind expands to the blur object's far corner. */
    start_capture();
    lv_obj_invalidate(behind);
    lv_refr_now(NULL);
    stop_capture();
    TEST_ASSERT_TRUE(area_was_invalidated(&far_corner));

    /* Remove the blur and let the resulting invalidation drain. */
    lv_obj_set_style_blur_radius(blur, 0, 0);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_UINT32(0, blur_cnt());

    /* Without blur: the same change stays local; the far corner is untouched. */
    start_capture();
    lv_obj_invalidate(behind);
    lv_refr_now(NULL);
    stop_capture();
    TEST_ASSERT_FALSE(area_was_invalidated(&far_corner));
}

#endif
