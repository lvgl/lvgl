#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

/* Tests that a scrollbar-only redraw-only state change does not refresh the child subtree. */

static uint32_t child_style_changed_count;

static void count_style_changed_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_STYLE_CHANGED) child_style_changed_count++;
}

void setUp(void)
{
    child_style_changed_count = 0;
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

/* Build a 2-level subtree with a `LV_EVENT_STYLE_CHANGED` counter on every
 * descendant (not on `parent`, which gets its own event on a state change).
 * Render it once: state changes only refresh already-rendered objects, so
 * without a render the toggles below would be skipped and the tests would pass
 * for the wrong reason. */
static lv_obj_t * build_rendered_subtree(void)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_size(parent, 200, 200);

    for(uint32_t i = 0; i < 3; i++) {
        lv_obj_t * child = lv_obj_create(parent);
        lv_obj_add_event_cb(child, count_style_changed_cb, LV_EVENT_STYLE_CHANGED, NULL);
        for(uint32_t j = 0; j < 2; j++) {
            lv_obj_t * grandchild = lv_obj_create(child);
            lv_obj_add_event_cb(grandchild, count_style_changed_cb, LV_EVENT_STYLE_CHANGED, NULL);
        }
    }

    lv_refr_now(NULL);
    return parent;
}

/* A redraw-only, scrollbar-only state change must not reach any descendant. */
void test_scrollbar_redraw_state_does_not_cascade_to_children(void)
{
    lv_obj_t * parent = build_rendered_subtree();

    lv_style_t scrollbar_scrolled;
    lv_style_init(&scrollbar_scrolled);
    lv_style_set_bg_opa(&scrollbar_scrolled, LV_OPA_COVER); /* redraw-only property */
    lv_obj_add_style(parent, &scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);

    /* Reset after the subtree setup (which emits its own STYLE_CHANGED events)
     * so the count reflects only the state change below. */
    child_style_changed_count = 0;
    lv_obj_add_state(parent, LV_STATE_SCROLLED);
    lv_obj_remove_state(parent, LV_STATE_SCROLLED);

    TEST_ASSERT_EQUAL(0, child_style_changed_count);

    lv_obj_remove_style(parent, &scrollbar_scrolled, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
    lv_style_reset(&scrollbar_scrolled);
}

/* A main-part state change must still cascade into the subtree. */
void test_main_part_state_still_cascades_to_children(void)
{
    lv_obj_t * parent = build_rendered_subtree();

    lv_style_t main_user_1;
    lv_style_init(&main_user_1);
    lv_style_set_pad_all(&main_user_1, 20);
    lv_obj_add_style(parent, &main_user_1, LV_PART_MAIN | LV_STATE_USER_1);

    child_style_changed_count = 0;
    lv_obj_add_state(parent, LV_STATE_USER_1);

    TEST_ASSERT_GREATER_THAN(0, child_style_changed_count);

    lv_obj_remove_style(parent, &main_user_1, LV_PART_MAIN | LV_STATE_USER_1);
    lv_style_reset(&main_user_1);
}

/* A layout-affecting scrollbar change must not take the redraw fast path: it
 * classifies as a layout difference and still cascades via the full refresh. */
void test_scrollbar_layout_state_still_cascades_to_children(void)
{
    lv_obj_t * parent = build_rendered_subtree();

    lv_style_t scrollbar_layout;
    lv_style_init(&scrollbar_layout);
    lv_style_set_width(&scrollbar_layout, 30); /* layout-affecting property */
    lv_obj_add_style(parent, &scrollbar_layout, LV_PART_SCROLLBAR | LV_STATE_USER_1);

    child_style_changed_count = 0;
    lv_obj_add_state(parent, LV_STATE_USER_1);

    TEST_ASSERT_GREATER_THAN(0, child_style_changed_count);

    lv_obj_remove_style(parent, &scrollbar_layout, LV_PART_SCROLLBAR | LV_STATE_USER_1);
    lv_style_reset(&scrollbar_layout);
}

/* A redraw-only change spanning more than one part is reported as LV_PART_ANY,
 * so it must not take the scrollbar fast path and still cascades. */
void test_multi_part_redraw_state_cascades_to_children(void)
{
    lv_obj_t * parent = build_rendered_subtree();

    lv_style_t main_redraw;
    lv_style_init(&main_redraw);
    lv_style_set_bg_color(&main_redraw, lv_color_hex(0x123456));
    lv_obj_add_style(parent, &main_redraw, LV_PART_MAIN | LV_STATE_USER_1);

    lv_style_t scrollbar_redraw;
    lv_style_init(&scrollbar_redraw);
    lv_style_set_bg_opa(&scrollbar_redraw, LV_OPA_COVER);
    lv_obj_add_style(parent, &scrollbar_redraw, LV_PART_SCROLLBAR | LV_STATE_USER_1);

    child_style_changed_count = 0;
    lv_obj_add_state(parent, LV_STATE_USER_1);

    TEST_ASSERT_GREATER_THAN(0, child_style_changed_count);

    lv_obj_remove_style(parent, &main_redraw, LV_PART_MAIN | LV_STATE_USER_1);
    lv_obj_remove_style(parent, &scrollbar_redraw, LV_PART_SCROLLBAR | LV_STATE_USER_1);
    lv_style_reset(&main_redraw);
    lv_style_reset(&scrollbar_redraw);
}

#endif
