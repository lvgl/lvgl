#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

static lv_obj_t * g_screen;
static lv_group_t * g_group;
static struct {
    bool press_happened;
    uint32_t key;
    lv_obj_t * obj;
} g_key_data;

void setUp(void)
{
    g_screen = lv_screen_active();
    g_group = lv_group_create();
    g_key_data.press_happened = false;
}

void tearDown(void)
{
    lv_obj_clean(g_screen);
    lv_group_delete(g_group); /* also removes all indevs set to the group */
}

static void key_event_cb(lv_event_t * e)
{
    TEST_ASSERT_FALSE(g_key_data.press_happened);
    g_key_data.press_happened = true;
    g_key_data.key = *(uint32_t *)lv_event_get_param(e);
    g_key_data.obj = lv_event_get_target_obj(e);
}

static void gridnav_one_axis_move_only(uint32_t key_grid_axis_next,
                                       uint32_t key_grid_axis_prev,
                                       uint32_t key_obj_axis_next,
                                       uint32_t key_obj_axis_prev,
                                       lv_gridnav_ctrl_t gridnav_ctrl,
                                       lv_flex_flow_t flex_flow)
{
    lv_indev_set_group(lv_test_keypad_indev, g_group);

    lv_obj_t * cont = lv_obj_create(g_screen);
    lv_obj_set_flex_flow(cont, flex_flow);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_center(cont);
    lv_gridnav_add(cont, gridnav_ctrl);
    lv_group_add_obj(g_group, cont);
    lv_obj_t * objs[3];
    for(uint32_t i = 0; i < 3; i++) {
        lv_obj_t * obj = lv_obj_create(cont);
        lv_obj_create(obj); /* the obj needs a child to be focusable by gridnav */
        lv_group_remove_obj(obj);
        lv_obj_add_event_cb(obj, key_event_cb, LV_EVENT_KEY, NULL);
        objs[i] = obj;
    }

    TEST_ASSERT(lv_obj_get_state(objs[0]) & LV_STATE_FOCUSED);

    /* gridnav direction key moves the focus */
    lv_test_key_hit(key_grid_axis_next);
    TEST_ASSERT(lv_obj_get_state(objs[1]) & LV_STATE_FOCUSED);
    TEST_ASSERT_FALSE(g_key_data.press_happened);

    /* non gridnav direction key does not move the focus. */
    /* the key is sent to the object instead */
    lv_test_key_hit(key_obj_axis_next);
    TEST_ASSERT(lv_obj_get_state(objs[1]) & LV_STATE_FOCUSED);
    TEST_ASSERT_TRUE(g_key_data.press_happened);
    TEST_ASSERT(g_key_data.key == key_obj_axis_next);
    TEST_ASSERT(g_key_data.obj == objs[1]);
    g_key_data.press_happened = false;

    lv_test_key_hit(key_obj_axis_prev);
    TEST_ASSERT(lv_obj_get_state(objs[1]) & LV_STATE_FOCUSED);
    TEST_ASSERT_TRUE(g_key_data.press_happened);
    TEST_ASSERT(g_key_data.key == key_obj_axis_prev);
    TEST_ASSERT(g_key_data.obj == objs[1]);
    g_key_data.press_happened = false;

    /* go back */
    lv_test_key_hit(key_grid_axis_prev);
    TEST_ASSERT(lv_obj_get_state(objs[0]) & LV_STATE_FOCUSED);
    TEST_ASSERT_FALSE(g_key_data.press_happened);
    /* at the beginning, can't move further back */
    lv_test_key_hit(key_grid_axis_prev);
    TEST_ASSERT(lv_obj_get_state(objs[0]) & LV_STATE_FOCUSED);
    TEST_ASSERT_FALSE(g_key_data.press_happened);
}

void test_gridnav_vertical_move_only(void)
{
    gridnav_one_axis_move_only(LV_KEY_DOWN,
                               LV_KEY_UP,
                               LV_KEY_RIGHT,
                               LV_KEY_LEFT,
                               LV_GRIDNAV_CTRL_VERTICAL_MOVE_ONLY,
                               LV_FLEX_FLOW_COLUMN);
}

void test_gridnav_horizontal_move_only(void)
{
    gridnav_one_axis_move_only(LV_KEY_RIGHT,
                               LV_KEY_LEFT,
                               LV_KEY_DOWN,
                               LV_KEY_UP,
                               LV_GRIDNAV_CTRL_HORIZONTAL_MOVE_ONLY,
                               LV_FLEX_FLOW_ROW);
}

#endif
