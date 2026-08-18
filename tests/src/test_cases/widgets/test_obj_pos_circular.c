#if LV_BUILD_TEST
#include "../../lvgl.h"

#include "unity/unity.h"

#include <string.h>

/**
 * A size that depends on itself makes `lv_obj_update_layout()` repeat forever. These tests cover
 * both halves of the answer to that: sizes that used to oscillate now settle, and a layout that
 * genuinely cannot settle is abandoned with a warning instead of hanging.
 */

static bool layout_gave_up;

static void log_cb(lv_log_level_t level, const char * buf)
{
    if(level >= LV_LOG_LEVEL_WARN && strstr(buf, "didn't settle")) layout_gave_up = true;
}

void setUp(void)
{
    layout_gave_up = false;
    lv_log_register_print_cb(log_cb);
}

void tearDown(void)
{
    lv_log_register_print_cb(NULL);
    lv_obj_clean(lv_screen_active());
}

void test_one_line_textarea_in_content_parent_settles(void)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_set_size(parent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_t * ta = lv_textarea_create(parent);
    lv_textarea_set_one_line(ta, true);
    lv_textarea_set_text(ta, "item value");
    lv_obj_set_size(ta, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_update_layout(lv_screen_active());

    TEST_ASSERT_FALSE(layout_gave_up);
    lv_obj_t * label = lv_obj_get_child(ta, 0);
    TEST_ASSERT_GREATER_THAN_INT32(0, lv_obj_get_width(label));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(label), lv_obj_get_content_width(ta));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(ta), lv_obj_get_content_width(parent));
}

/* more generic approach to test above*/
void test_content_size_chain_with_pct_min_width_settles(void)
{
    lv_obj_t * grandparent = lv_obj_create(lv_screen_active());
    lv_obj_set_size(grandparent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_t * parent = lv_obj_create(grandparent);
    lv_obj_set_size(parent, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text(label, "item value");
    lv_obj_set_width(label, LV_SIZE_CONTENT);
    lv_obj_set_style_min_width(label, LV_PCT(100), LV_PART_MAIN);

    lv_obj_update_layout(lv_screen_active());

    TEST_ASSERT_FALSE(layout_gave_up);
    /*The label is sized to its text and the parents wrap it instead of collapsing*/
    TEST_ASSERT_GREATER_THAN_INT32(0, lv_obj_get_width(label));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(label), lv_obj_get_content_width(parent));
    TEST_ASSERT_EQUAL_INT32(lv_obj_get_width(parent), lv_obj_get_content_width(grandparent));
}

static void toggle_width_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_set_width(obj, lv_obj_get_width(obj) == 100 ? 200 : 100);
}

/**
 * A size dependency LVGL cannot resolve must end in a warning, and not in an infinite loop.
 * If this test times out instead of failing, the pass limit in `lv_obj_update_layout()` is gone.
 */
void test_unresolvable_layout_gives_up_with_a_warning(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 100, 100);
    lv_obj_add_event_cb(obj, toggle_width_cb, LV_EVENT_SIZE_CHANGED, NULL);
    lv_obj_set_width(obj, 200);

    lv_obj_update_layout(lv_screen_active());

    TEST_ASSERT_TRUE(layout_gave_up);
}


#endif
