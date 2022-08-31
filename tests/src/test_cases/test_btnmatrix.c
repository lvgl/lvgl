#if LV_BUILD_TEST
#include "../lvgl.h"
#include "lv_test_indev.h"
#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * btnm;
static bool event_triggered = false;
lv_event_code_t exp_evt_code;

void setUp(void)
{
    active_screen = lv_scr_act();
    btnm = lv_btnmatrix_create(active_screen);
    TEST_ASSERT_NOT_NULL(btnm);
    event_triggered = false;
    exp_evt_code = 0;
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_btn_matrix_creation(void)
{
    const char ** map;

    /* Verify the default map. */
    map = lv_btnmatrix_get_map(btnm);
    TEST_ASSERT_EQUAL_STRING(map[0], "Btn1");
    TEST_ASSERT_EQUAL_STRING(map[1], "Btn2");
    TEST_ASSERT_EQUAL_STRING(map[2], "Btn3");
    TEST_ASSERT_EQUAL_STRING(map[3], "\n");
    TEST_ASSERT_EQUAL_STRING(map[4], "Btn4");
    TEST_ASSERT_EQUAL_STRING(map[5], "Btn5");
}

void test_btn_matrix_set_map_works(void)
{
    const char ** ret_map;
    static const char * exp_map[] = {"A", "B", "\n", "C", "D", ""};

    lv_btnmatrix_set_map(btnm, exp_map);

    /* Verify if the map was set correctly. */
    ret_map = lv_btnmatrix_get_map(btnm);
    TEST_ASSERT_EQUAL_STRING(exp_map[0], ret_map[0]);
    TEST_ASSERT_EQUAL_STRING(exp_map[1], ret_map[1]);
    TEST_ASSERT_EQUAL_STRING(exp_map[2], ret_map[2]);
    TEST_ASSERT_EQUAL_STRING(exp_map[3], ret_map[3]);
    TEST_ASSERT_EQUAL_STRING(exp_map[4], ret_map[4]);
    TEST_ASSERT_EQUAL_STRING(exp_map[5], ret_map[5]);
}

void test_btn_matrix_set_ctrl_map_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);

    lv_btnmatrix_ctrl_t ctrl_map[4];
    ctrl_map[0] = 1 | LV_BTNMATRIX_CTRL_DISABLED;
    ctrl_map[1] = 1 | LV_BTNMATRIX_CTRL_CHECKABLE | LV_BTNMATRIX_CTRL_CHECKED;
    ctrl_map[2] = 1 | LV_BTNMATRIX_CTRL_HIDDEN;
    ctrl_map[3] = 1 | LV_BTNMATRIX_CTRL_CHECKABLE;
    lv_btnmatrix_set_ctrl_map(btnm, ctrl_map);

    /* Verify if the ctrl map was set correctly. */
    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_DISABLED));
    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 1,
                                               LV_BTNMATRIX_CTRL_CHECKABLE | LV_BTNMATRIX_CTRL_CHECKED));
    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 2, LV_BTNMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 3, LV_BTNMATRIX_CTRL_CHECKABLE));

    /* Also checking randomly that no other flags are set. */
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_CHECKABLE));
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_DISABLED));
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 3, LV_BTNMATRIX_CTRL_CHECKED));
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 4, LV_BTNMATRIX_CTRL_HIDDEN));
}

void test_btn_matrix_set_btn_ctrl_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);

    /* Set btn control map using individual APIs. */
    lv_btnmatrix_set_btn_ctrl(btnm, 0, 1 | LV_BTNMATRIX_CTRL_DISABLED);
    lv_btnmatrix_set_btn_ctrl(btnm, 1, 1 | LV_BTNMATRIX_CTRL_CHECKABLE | LV_BTNMATRIX_CTRL_CHECKED);
    lv_btnmatrix_set_btn_ctrl(btnm, 2, 1 | LV_BTNMATRIX_CTRL_HIDDEN);
    lv_btnmatrix_set_btn_ctrl(btnm, 3, 1 | LV_BTNMATRIX_CTRL_CHECKABLE);

    /* Verify if the ctrl map was set correctly. */
    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_DISABLED));
    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 1,
                                               LV_BTNMATRIX_CTRL_CHECKABLE | LV_BTNMATRIX_CTRL_CHECKED));
    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 2, LV_BTNMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 3, LV_BTNMATRIX_CTRL_CHECKABLE));

    /* Also checking randomly that no other flags are set. */
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_CHECKABLE));
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_DISABLED));
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 3, LV_BTNMATRIX_CTRL_CHECKED));
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 4, LV_BTNMATRIX_CTRL_HIDDEN));
}

void test_btn_matrix_clear_btn_ctrl_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);

    /* Set btn control map using individual APIs. */
    lv_btnmatrix_set_btn_ctrl(btnm, 0, 1 | LV_BTNMATRIX_CTRL_DISABLED);
    lv_btnmatrix_set_btn_ctrl(btnm, 1, 1 | LV_BTNMATRIX_CTRL_CHECKABLE | LV_BTNMATRIX_CTRL_CHECKED);
    lv_btnmatrix_set_btn_ctrl(btnm, 2, 1 | LV_BTNMATRIX_CTRL_HIDDEN);
    lv_btnmatrix_set_btn_ctrl(btnm, 3, 1 | LV_BTNMATRIX_CTRL_CHECKABLE);

    lv_btnmatrix_clear_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_DISABLED);
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_DISABLED));
    lv_btnmatrix_clear_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_CHECKED);
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_CHECKED));
    lv_btnmatrix_clear_btn_ctrl(btnm, 2, LV_BTNMATRIX_CTRL_HIDDEN);
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 2, LV_BTNMATRIX_CTRL_HIDDEN));
    lv_btnmatrix_clear_btn_ctrl(btnm, 3, LV_BTNMATRIX_CTRL_CHECKABLE);
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 3, LV_BTNMATRIX_CTRL_CHECKABLE));
}

void test_btn_matrix_set_selected_btn_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);

    lv_btnmatrix_set_selected_btn(btnm, 2);
    TEST_ASSERT_EQUAL_UINT16(2, lv_btnmatrix_get_selected_btn(btnm));

    lv_btnmatrix_set_selected_btn(btnm, 0);
    TEST_ASSERT_EQUAL_UINT16(0, lv_btnmatrix_get_selected_btn(btnm));

    lv_btnmatrix_set_selected_btn(btnm, 3);
    TEST_ASSERT_EQUAL_UINT16(3, lv_btnmatrix_get_selected_btn(btnm));

    lv_btnmatrix_set_selected_btn(btnm, 1);
    TEST_ASSERT_EQUAL_UINT16(1, lv_btnmatrix_get_selected_btn(btnm));
}

void test_btn_matrix_set_btn_ctrl_all_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);

    lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_HIDDEN);

    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 2, LV_BTNMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 3, LV_BTNMATRIX_CTRL_HIDDEN));
}

void test_btn_matrix_clear_btn_ctrl_all_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);

    lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_HIDDEN);
    lv_btnmatrix_clear_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_HIDDEN);

    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 2, LV_BTNMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 3, LV_BTNMATRIX_CTRL_HIDDEN));
}

void test_btn_matrix_set_btn_width_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);

    lv_btnmatrix_set_btn_width(btnm, 1, 3);
    lv_btnmatrix_set_btn_width(btnm, 2, 2);

    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 1, 3));
    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 2, 2));
}

void test_btn_matrix_set_one_checked_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);

    lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_one_checked(btnm, true);

    lv_btnmatrix_set_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_CHECKED);
    lv_btnmatrix_set_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_CHECKED);
    lv_btnmatrix_set_btn_ctrl(btnm, 2, LV_BTNMATRIX_CTRL_CHECKED);
    lv_btnmatrix_set_btn_ctrl(btnm, 3, LV_BTNMATRIX_CTRL_CHECKED);

    TEST_ASSERT_TRUE(lv_btnmatrix_get_one_checked(btnm));

    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_CHECKED));
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_CHECKED));
    TEST_ASSERT_FALSE(lv_btnmatrix_has_btn_ctrl(btnm, 2, LV_BTNMATRIX_CTRL_CHECKED));
    TEST_ASSERT_TRUE(lv_btnmatrix_has_btn_ctrl(btnm, 3, LV_BTNMATRIX_CTRL_CHECKED));
}

void test_btn_matrix_get_btn_text_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);

    const char * lv_btnmatrix_get_btn_text(const lv_obj_t * obj, uint16_t btn_id);

    TEST_ASSERT_EQUAL_STRING("A", lv_btnmatrix_get_btn_text(btnm, 0));
    TEST_ASSERT_EQUAL_STRING("B", lv_btnmatrix_get_btn_text(btnm, 1));
    TEST_ASSERT_EQUAL_STRING("C", lv_btnmatrix_get_btn_text(btnm, 2));
    TEST_ASSERT_EQUAL_STRING("D", lv_btnmatrix_get_btn_text(btnm, 3));
}

/* Common event handler for all the consecutive test cases. */
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == exp_evt_code) {
        event_triggered = true;
    }
}

void test_btn_matrix_pressed_event_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_PRESSED, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_PRESSED;
    /* Click button index 0. */
    lv_test_mouse_click_at(10, 10);
    TEST_ASSERT_TRUE(event_triggered);
}

void test_btn_matrix_release_event_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);
    lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_RELEASED, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_RELEASED;

    /* Click button index 0. */
    lv_test_mouse_click_at(65, 35);
    TEST_ASSERT_TRUE(event_triggered);
    event_triggered = false;
    /* This will increase test coverage by unchecking the
     same button. */
    lv_btnmatrix_set_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_POPOVER);
    /* Click button index 0. */
    lv_test_mouse_click_at(65, 35);
    TEST_ASSERT_TRUE(event_triggered);
}

void test_btn_matrix_size_changed_event_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_SIZE_CHANGED, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_SIZE_CHANGED;
    lv_event_send(btnm, LV_EVENT_SIZE_CHANGED, NULL);
    TEST_ASSERT_TRUE(event_triggered);
}

void test_btn_matrix_key_event_works(void)
{
    char keyCode;
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);
    lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_KEY, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_KEY;

    keyCode = LV_KEY_RIGHT;
    lv_event_send(btnm, LV_EVENT_KEY, &keyCode);
    TEST_ASSERT_TRUE(event_triggered);
    event_triggered = false;

    keyCode = LV_KEY_LEFT;
    lv_event_send(btnm, LV_EVENT_KEY, &keyCode);
    TEST_ASSERT_TRUE(event_triggered);
    event_triggered = false;

    keyCode = LV_KEY_DOWN;
    lv_event_send(btnm, LV_EVENT_KEY, &keyCode);
    TEST_ASSERT_TRUE(event_triggered);
    event_triggered = false;

    keyCode = LV_KEY_UP;
    lv_event_send(btnm, LV_EVENT_KEY, &keyCode);
    TEST_ASSERT_TRUE(event_triggered);

    /* Added this code to increase code coverage. */
    lv_btnmatrix_t * btnmObj = (lv_btnmatrix_t *)btnm;
    btnmObj->btn_id_sel = LV_BTNMATRIX_BTN_NONE;
    lv_btnmatrix_set_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_HIDDEN);
    keyCode = LV_KEY_DOWN;
    lv_event_send(btnm, LV_EVENT_KEY, &keyCode);
    TEST_ASSERT_TRUE(event_triggered);
    event_triggered = false;
}

void test_btn_matrix_pressing_event_works(void)
{
    lv_btnmatrix_t * btnmObj = (lv_btnmatrix_t *)btnm;
    lv_indev_t dummyIndev = {0};
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_btnmatrix_set_map(btnm, btn_map);
    lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_PRESSING, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_PRESSING;
    /* Select a button before raising a simulated event.
     * This is done to increase code coverage. */
    btnmObj->btn_id_sel = 3;
    /* Send a dummy lv_indev_t object as param to avoid crashing during build. */
    lv_event_send(btnm, LV_EVENT_PRESSING, &dummyIndev);
    TEST_ASSERT_TRUE(event_triggered);
}

void test_btn_matrix_long_press_repeat_event_works(void)
{
    lv_btnmatrix_t * btnmObj = (lv_btnmatrix_t *)btnm;
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};

    lv_btnmatrix_set_map(btnm, btn_map);
    lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_LONG_PRESSED_REPEAT, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_LONG_PRESSED_REPEAT;
    /* Select a button before raising a simulated event.
     * This is done to increase code coverage. */
    btnmObj->btn_id_sel = 0;
    lv_event_send(btnm, LV_EVENT_LONG_PRESSED_REPEAT, NULL);
    TEST_ASSERT_TRUE(event_triggered);
}

void test_btn_matrix_press_lost_event_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};

    lv_btnmatrix_set_map(btnm, btn_map);
    lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_PRESS_LOST, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_PRESS_LOST;
    lv_event_send(btnm, LV_EVENT_PRESS_LOST, NULL);
    TEST_ASSERT_TRUE(event_triggered);
}

void test_btn_matrix_defocused_event_works(void)
{
    lv_btnmatrix_t * btnmObj = (lv_btnmatrix_t *)btnm;
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};

    lv_btnmatrix_set_map(btnm, btn_map);
    lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_DEFOCUSED, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_DEFOCUSED;
    /* Select a button before raising a simulated event.
     * This is done to increase code coverage. */
    btnmObj->btn_id_sel = 0;
    lv_event_send(btnm, LV_EVENT_DEFOCUSED, NULL);
    TEST_ASSERT_TRUE(event_triggered);
}

void test_btn_matrix_focused_event_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};

    lv_btnmatrix_set_map(btnm, btn_map);
    lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_FOCUSED, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_FOCUSED;
    lv_event_send(btnm, LV_EVENT_FOCUSED, NULL);
    TEST_ASSERT_TRUE(event_triggered);
}


#endif
