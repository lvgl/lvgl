#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

/* Tests the `type` of style transitions: which style changes start a transition */

static const lv_style_prop_t trans_props[] = {LV_STYLE_WIDTH, LV_STYLE_PROP_INV};
static lv_style_transition_dsc_t trans_dsc;
static lv_style_t trans_style;
static lv_style_t style_a;

void setUp(void)
{
    lv_style_transition_dsc_init(&trans_dsc, trans_props, lv_anim_path_linear, 1000, 0, NULL);

    lv_style_init(&trans_style);
    lv_style_set_transition(&trans_style, &trans_dsc);

    lv_style_init(&style_a);
    lv_style_set_width(&style_a, 300);
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
    lv_style_reset(&trans_style);
    lv_style_reset(&style_a);
}

static lv_obj_t * create_obj(lv_style_transition_type_t type)
{
    lv_style_transition_dsc_set_type(&trans_dsc, type);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj, &trans_style, 0);
    lv_obj_set_width(obj, 100);

    /*Transitions are created only after the widget has been rendered*/
    lv_refr_now(NULL);
    return obj;
}

static int32_t get_width(lv_obj_t * obj)
{
    return lv_obj_get_style_width(obj, LV_PART_MAIN);
}

static void assert_in_progress(lv_obj_t * obj, int32_t from, int32_t to)
{
    int32_t w = get_width(obj);
    TEST_ASSERT_GREATER_THAN_INT32(LV_MIN(from, to), w);
    TEST_ASSERT_LESS_THAN_INT32(LV_MAX(from, to), w);
}

void test_default_type_is_state(void)
{
    lv_style_transition_dsc_t dsc;
    lv_style_transition_dsc_init(&dsc, trans_props, NULL, 100, 0, NULL);
    TEST_ASSERT_EQUAL_UINT8(LV_STYLE_TRANSITION_TYPE_STATE, dsc.type);
}

void test_local_change_is_instant_without_local_type(void)
{
    lv_obj_t * obj = create_obj(LV_STYLE_TRANSITION_TYPE_STATE);

    lv_obj_set_width(obj, 200);
    TEST_ASSERT_EQUAL_INT32(200, get_width(obj));
}

void test_local_change_animates_with_local_type(void)
{
    lv_obj_t * obj = create_obj(LV_STYLE_TRANSITION_TYPE_LOCAL);

    lv_obj_set_width(obj, 200);
    TEST_ASSERT_EQUAL_INT32(100, get_width(obj));

    lv_test_wait(500);
    assert_in_progress(obj, 100, 200);

    lv_test_wait(600);
    TEST_ASSERT_EQUAL_INT32(200, get_width(obj));
}

void test_local_change_is_instant_before_render(void)
{
    lv_style_transition_dsc_set_type(&trans_dsc, LV_STYLE_TRANSITION_TYPE_ALL);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_add_style(obj, &trans_style, 0);
    lv_obj_set_width(obj, 100);
    lv_obj_set_width(obj, 200);

    TEST_ASSERT_EQUAL_INT32(200, get_width(obj));
}

void test_local_change_retargets_running_transition(void)
{
    lv_obj_t * obj = create_obj(LV_STYLE_TRANSITION_TYPE_LOCAL);

    lv_obj_set_width(obj, 200);
    lv_test_wait(500);
    int32_t w_mid = get_width(obj);
    assert_in_progress(obj, 100, 200);

    /*Should continue from the current value without jumping*/
    lv_obj_set_width(obj, 0);
    TEST_ASSERT_EQUAL_INT32(w_mid, get_width(obj));

    lv_test_wait(500);
    assert_in_progress(obj, 0, w_mid);

    lv_test_wait(600);
    TEST_ASSERT_EQUAL_INT32(0, get_width(obj));
}

void test_remove_local_prop_animates(void)
{
    lv_obj_t * obj = create_obj(LV_STYLE_TRANSITION_TYPE_LOCAL);
    lv_obj_add_style(obj, &style_a, 0); /*Not animated: no STYLE type*/
    TEST_ASSERT_EQUAL_INT32(100, get_width(obj)); /*The local style wins*/

    lv_obj_remove_local_style_prop(obj, LV_STYLE_WIDTH, 0);
    TEST_ASSERT_EQUAL_INT32(100, get_width(obj));

    lv_test_wait(500);
    assert_in_progress(obj, 100, 300);

    lv_test_wait(600);
    TEST_ASSERT_EQUAL_INT32(300, get_width(obj));
}

void test_add_and_remove_style_animate_with_style_type(void)
{
    lv_obj_t * obj = create_obj(LV_STYLE_TRANSITION_TYPE_STYLE);
    lv_obj_remove_local_style_prop(obj, LV_STYLE_WIDTH, 0); /*Instant: no LOCAL type*/
    int32_t w_default = get_width(obj);
    TEST_ASSERT_NOT_EQUAL_INT32(300, w_default);

    lv_obj_add_style(obj, &style_a, 0);
    TEST_ASSERT_EQUAL_INT32(w_default, get_width(obj));
    lv_test_wait(500);
    assert_in_progress(obj, w_default, 300);
    lv_test_wait(600);
    TEST_ASSERT_EQUAL_INT32(300, get_width(obj));

    lv_obj_remove_style(obj, &style_a, 0);
    TEST_ASSERT_EQUAL_INT32(300, get_width(obj));
    lv_test_wait(500);
    assert_in_progress(obj, w_default, 300);
    lv_test_wait(600);
    TEST_ASSERT_EQUAL_INT32(w_default, get_width(obj));
}

void test_add_style_is_instant_without_style_type(void)
{
    lv_obj_t * obj = create_obj(LV_STYLE_TRANSITION_TYPE_STATE | LV_STYLE_TRANSITION_TYPE_LOCAL);
    lv_obj_remove_local_style_prop(obj, LV_STYLE_WIDTH, 0);
    lv_test_wait(1100);

    lv_obj_add_style(obj, &style_a, 0);
    TEST_ASSERT_EQUAL_INT32(300, get_width(obj));
}

void test_style_enable_animates_with_style_type(void)
{
    lv_obj_t * obj = create_obj(LV_STYLE_TRANSITION_TYPE_STYLE);
    lv_obj_remove_local_style_prop(obj, LV_STYLE_WIDTH, 0);
    lv_obj_set_style_width(obj, 100, LV_STATE_CHECKED);
    lv_obj_add_state(obj, LV_STATE_CHECKED); /*Instant: no STATE type*/
    TEST_ASSERT_EQUAL_INT32(100, get_width(obj));

    lv_obj_add_style(obj, &style_a, LV_STATE_CHECKED);
    lv_test_wait(1100);
    TEST_ASSERT_EQUAL_INT32(100, get_width(obj)); /*The local style wins*/

    lv_obj_remove_local_style_prop(obj, LV_STYLE_WIDTH, LV_STATE_CHECKED);
    TEST_ASSERT_EQUAL_INT32(300, get_width(obj));

    lv_obj_set_style_enabled(obj, &style_a, LV_STATE_CHECKED, false);
    TEST_ASSERT_EQUAL_INT32(300, get_width(obj));
    lv_test_wait(500);
    int32_t w_default = lv_obj_get_style_width(obj, LV_PART_MAIN);
    TEST_ASSERT_NOT_EQUAL_INT32(300, w_default);
    lv_test_wait(600);
    w_default = get_width(obj);
    TEST_ASSERT_NOT_EQUAL_INT32(300, w_default);

    lv_obj_set_style_enabled(obj, &style_a, LV_STATE_CHECKED, true);
    TEST_ASSERT_EQUAL_INT32(w_default, get_width(obj));
    lv_test_wait(500);
    assert_in_progress(obj, w_default, 300);
    lv_test_wait(600);
    TEST_ASSERT_EQUAL_INT32(300, get_width(obj));
}

void test_replace_style_animates_with_style_type(void)
{
    lv_style_t style_b;
    lv_style_init(&style_b);
    lv_style_set_width(&style_b, 100);

    lv_obj_t * obj = create_obj(LV_STYLE_TRANSITION_TYPE_STYLE);
    lv_obj_remove_local_style_prop(obj, LV_STYLE_WIDTH, 0);
    lv_obj_add_style(obj, &style_b, 0);
    lv_test_wait(1100);
    TEST_ASSERT_EQUAL_INT32(100, get_width(obj));

    lv_obj_replace_style(obj, &style_b, &style_a, 0);
    TEST_ASSERT_EQUAL_INT32(100, get_width(obj));
    lv_test_wait(500);
    assert_in_progress(obj, 100, 300);
    lv_test_wait(600);
    TEST_ASSERT_EQUAL_INT32(300, get_width(obj));

    lv_obj_clean(lv_screen_active());
    lv_style_reset(&style_b);
}

void test_state_change_is_instant_without_state_type(void)
{
    lv_obj_t * obj = create_obj(LV_STYLE_TRANSITION_TYPE_LOCAL);
    lv_obj_set_style_width(obj, 200, LV_STATE_CHECKED);

    lv_obj_add_state(obj, LV_STATE_CHECKED);
    TEST_ASSERT_EQUAL_INT32(200, get_width(obj));
}

void test_state_change_animates_with_state_type(void)
{
    lv_obj_t * obj = create_obj(LV_STYLE_TRANSITION_TYPE_STATE);
    lv_obj_set_style_width(obj, 200, LV_STATE_CHECKED);

    lv_obj_add_state(obj, LV_STATE_CHECKED);
    lv_test_wait(500);
    assert_in_progress(obj, 100, 200);
    lv_test_wait(600);
    TEST_ASSERT_EQUAL_INT32(200, get_width(obj));
}

void test_bind_style_prop_animates_with_local_type(void)
{
    lv_subject_t * subject = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_int(subject, 100);

    lv_obj_t * obj = create_obj(LV_STYLE_TRANSITION_TYPE_LOCAL);
    lv_obj_bind_style_prop(obj, LV_STYLE_WIDTH, 0, subject);
    lv_test_wait(1100);
    TEST_ASSERT_EQUAL_INT32(100, get_width(obj));

    lv_subject_set_int(subject, 200);
    TEST_ASSERT_EQUAL_INT32(100, get_width(obj));
    lv_test_wait(500);
    assert_in_progress(obj, 100, 200);
    lv_test_wait(600);
    TEST_ASSERT_EQUAL_INT32(200, get_width(obj));

    lv_obj_clean(lv_screen_active());
    lv_subject_delete(subject);
}
void test_pointer_prop_is_not_interpolated(void)
{
    static const lv_style_prop_t ptr_props[] = {LV_STYLE_GRID_COLUMN_DSC_ARRAY, LV_STYLE_PROP_INV};
    static const int32_t col_a[] = {10, LV_GRID_TEMPLATE_LAST};
    static const int32_t col_b[] = {20, 30, LV_GRID_TEMPLATE_LAST};

    lv_style_transition_dsc_t dsc;
    lv_style_transition_dsc_init(&dsc, ptr_props, lv_anim_path_linear, 1000, 0, NULL);
    lv_style_transition_dsc_set_type(&dsc, LV_STYLE_TRANSITION_TYPE_LOCAL);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_style_transition(obj, &dsc, 0);
    lv_obj_set_style_grid_column_dsc_array(obj, col_a, 0);
    lv_refr_now(NULL);

    lv_obj_set_style_grid_column_dsc_array(obj, col_b, 0);
    lv_test_wait(500);
    TEST_ASSERT_EQUAL_PTR(col_a, lv_obj_get_style_grid_column_dsc_array(obj, 0));

    lv_test_wait(600);
    TEST_ASSERT_EQUAL_PTR(col_b, lv_obj_get_style_grid_column_dsc_array(obj, 0));

    lv_obj_delete(obj);
}

#endif
