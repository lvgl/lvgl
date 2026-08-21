#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * btnm;
static bool event_triggered = false;
lv_event_code_t exp_evt_code;

void setUp(void)
{
    active_screen = lv_screen_active();
    btnm = lv_buttonmatrix_create(active_screen);
    TEST_ASSERT_NOT_NULL(btnm);
    event_triggered = false;
    exp_evt_code = 0;
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_button_matrix_creation(void)
{
    const char * const * map;

    /* Verify the default map. */
    map = lv_buttonmatrix_get_map(btnm);
    TEST_ASSERT_EQUAL_STRING(map[0], "Btn1");
    TEST_ASSERT_EQUAL_STRING(map[1], "Btn2");
    TEST_ASSERT_EQUAL_STRING(map[2], "Btn3");
    TEST_ASSERT_EQUAL_STRING(map[3], "\n");
    TEST_ASSERT_EQUAL_STRING(map[4], "Btn4");
    TEST_ASSERT_EQUAL_STRING(map[5], "Btn5");
}

void test_button_matrix_set_map_works(void)
{
    const char * const * ret_map;
    static const char * exp_map[] = {"A", "B", "\n", "C", "D", ""};

    lv_buttonmatrix_set_map(btnm, exp_map);
    /* Verify if the map was set correctly. */
    ret_map = lv_buttonmatrix_get_map(btnm);
    TEST_ASSERT_EQUAL_STRING(exp_map[0], ret_map[0]);
    TEST_ASSERT_EQUAL_STRING(exp_map[1], ret_map[1]);
    TEST_ASSERT_EQUAL_STRING(exp_map[2], ret_map[2]);
    TEST_ASSERT_EQUAL_STRING(exp_map[3], ret_map[3]);
    TEST_ASSERT_EQUAL_STRING(exp_map[4], ret_map[4]);
    TEST_ASSERT_EQUAL_STRING(exp_map[5], ret_map[5]);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/btnm_1.png");
}

void test_button_matrix_render_2(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);
    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(btnm, 3, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_obj_set_pos(btnm, 10, 10);

    static const char * btn_map2[] = {"A", "\n", "B", "\n", "C", "\n", "D", ""};
    btnm = lv_buttonmatrix_create(active_screen);
    TEST_ASSERT_NOT_NULL(btnm);
    lv_obj_set_width(btnm, 150);
    lv_obj_set_height(btnm, 250);
    lv_buttonmatrix_set_map(btnm, btn_map2);
    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_one_checked(btnm, true);
    lv_buttonmatrix_set_button_ctrl(btnm, 1, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(btnm, 2, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_obj_set_pos(btnm, 10, 160);

    static const char * btn_map3[] = {"A", "B", "C", "\n", "D", "E", "F", ""};
    btnm = lv_buttonmatrix_create(active_screen);
    TEST_ASSERT_NOT_NULL(btnm);
    lv_buttonmatrix_set_map(btnm, btn_map3);
    lv_obj_set_width(btnm, 400);
    lv_buttonmatrix_set_button_width(btnm, 0, 3);
    lv_buttonmatrix_set_button_width(btnm, 1, 2);
    lv_buttonmatrix_set_button_width(btnm, 2, 1);
    lv_buttonmatrix_set_button_width(btnm, 3, 1);
    lv_buttonmatrix_set_button_width(btnm, 4, 2);
    lv_buttonmatrix_set_button_width(btnm, 5, 3);
    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(btnm, 1, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(btnm, 4, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_obj_set_pos(btnm, 300, 10);

    static const char * btn_map4[] = {"A", "B", "C", "D", "E", "F", "G", "\n",
                                      "G", "F", "E", "D", "C", "B", "A", ""
                                     };
    btnm = lv_buttonmatrix_create(active_screen);
    lv_buttonmatrix_set_map(btnm, btn_map4);
    lv_obj_set_width(btnm, 600);
    lv_obj_set_height(btnm, 150);
    lv_buttonmatrix_set_button_width(btnm, 0, 1);
    lv_buttonmatrix_set_button_width(btnm, 1, 2);
    lv_buttonmatrix_set_button_width(btnm, 2, 3);
    lv_buttonmatrix_set_button_width(btnm, 3, 4);
    lv_buttonmatrix_set_button_width(btnm, 4, 5);
    lv_buttonmatrix_set_button_width(btnm, 5, 6);
    lv_buttonmatrix_set_button_width(btnm, 6, 7);

    lv_buttonmatrix_set_button_width(btnm, 7, 7);
    lv_buttonmatrix_set_button_width(btnm, 8, 6);
    lv_buttonmatrix_set_button_width(btnm, 9, 5);
    lv_buttonmatrix_set_button_width(btnm, 10, 4);
    lv_buttonmatrix_set_button_width(btnm, 11, 3);
    lv_buttonmatrix_set_button_width(btnm, 12, 2);
    lv_buttonmatrix_set_button_width(btnm, 13, 1);

    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_button_ctrl(btnm, 1, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(btnm, 3, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(btnm, 5, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(btnm, 7, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(btnm, 9, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(btnm, 11, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_obj_set_pos(btnm, 180, 160);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/btnm_2.png");
}

void test_button_matrix_set_ctrl_map_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);

    lv_buttonmatrix_ctrl_t ctrl_map[4];
    ctrl_map[0] = 1 | LV_BUTTONMATRIX_CTRL_DISABLED;
    ctrl_map[1] = 1 | LV_BUTTONMATRIX_CTRL_CHECKABLE | LV_BUTTONMATRIX_CTRL_CHECKED;
    ctrl_map[2] = 1 | LV_BUTTONMATRIX_CTRL_HIDDEN;
    ctrl_map[3] = 1 | LV_BUTTONMATRIX_CTRL_CHECKABLE;
    lv_buttonmatrix_set_ctrl_map(btnm, ctrl_map);

    /* Verify if the ctrl map was set correctly. */
    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_DISABLED));
    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 1,
                                                     LV_BUTTONMATRIX_CTRL_CHECKABLE | LV_BUTTONMATRIX_CTRL_CHECKED));
    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 2, LV_BUTTONMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 3, LV_BUTTONMATRIX_CTRL_CHECKABLE));

    /* Also checking randomly that no other flags are set. */
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_CHECKABLE));
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 1, LV_BUTTONMATRIX_CTRL_DISABLED));
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 3, LV_BUTTONMATRIX_CTRL_CHECKED));
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 4, LV_BUTTONMATRIX_CTRL_HIDDEN));
}

void test_button_matrix_set_button_ctrl_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);

    /* Set btn control map using individual APIs. */
    lv_buttonmatrix_set_button_ctrl(btnm, 0, 1 | LV_BUTTONMATRIX_CTRL_DISABLED);
    lv_buttonmatrix_set_button_ctrl(btnm, 1, 1 | LV_BUTTONMATRIX_CTRL_CHECKABLE | LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(btnm, 2, 1 | LV_BUTTONMATRIX_CTRL_HIDDEN);
    lv_buttonmatrix_set_button_ctrl(btnm, 3, 1 | LV_BUTTONMATRIX_CTRL_CHECKABLE);

    /* Verify if the ctrl map was set correctly. */
    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_DISABLED));
    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 1,
                                                     LV_BUTTONMATRIX_CTRL_CHECKABLE | LV_BUTTONMATRIX_CTRL_CHECKED));
    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 2, LV_BUTTONMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 3, LV_BUTTONMATRIX_CTRL_CHECKABLE));

    /* Also checking randomly that no other flags are set. */
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_CHECKABLE));
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 1, LV_BUTTONMATRIX_CTRL_DISABLED));
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 3, LV_BUTTONMATRIX_CTRL_CHECKED));
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 4, LV_BUTTONMATRIX_CTRL_HIDDEN));
}

void test_button_matrix_clear_button_ctrl_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);

    /* Set btn control map using individual APIs. */
    lv_buttonmatrix_set_button_ctrl(btnm, 0, 1 | LV_BUTTONMATRIX_CTRL_DISABLED);
    lv_buttonmatrix_set_button_ctrl(btnm, 1, 1 | LV_BUTTONMATRIX_CTRL_CHECKABLE | LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(btnm, 2, 1 | LV_BUTTONMATRIX_CTRL_HIDDEN);
    lv_buttonmatrix_set_button_ctrl(btnm, 3, 1 | LV_BUTTONMATRIX_CTRL_CHECKABLE);

    lv_buttonmatrix_clear_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_DISABLED);
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_DISABLED));
    lv_buttonmatrix_clear_button_ctrl(btnm, 1, LV_BUTTONMATRIX_CTRL_CHECKED);
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 1, LV_BUTTONMATRIX_CTRL_CHECKED));
    lv_buttonmatrix_clear_button_ctrl(btnm, 2, LV_BUTTONMATRIX_CTRL_HIDDEN);
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 2, LV_BUTTONMATRIX_CTRL_HIDDEN));
    lv_buttonmatrix_clear_button_ctrl(btnm, 3, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 3, LV_BUTTONMATRIX_CTRL_CHECKABLE));
}

void test_button_matrix_set_selected_button_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);

    lv_buttonmatrix_set_selected_button(btnm, 2);
    TEST_ASSERT_EQUAL_UINT16(2, lv_buttonmatrix_get_selected_button(btnm));

    lv_buttonmatrix_set_selected_button(btnm, 0);
    TEST_ASSERT_EQUAL_UINT16(0, lv_buttonmatrix_get_selected_button(btnm));

    lv_buttonmatrix_set_selected_button(btnm, 3);
    TEST_ASSERT_EQUAL_UINT16(3, lv_buttonmatrix_get_selected_button(btnm));

    lv_buttonmatrix_set_selected_button(btnm, 1);
    TEST_ASSERT_EQUAL_UINT16(1, lv_buttonmatrix_get_selected_button(btnm));
}

void test_button_matrix_set_button_ctrl_all_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);

    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_HIDDEN);

    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 1, LV_BUTTONMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 2, LV_BUTTONMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 3, LV_BUTTONMATRIX_CTRL_HIDDEN));
}

void test_button_matrix_clear_button_ctrl_all_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);

    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_HIDDEN);
    lv_buttonmatrix_clear_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_HIDDEN);

    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 1, LV_BUTTONMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 2, LV_BUTTONMATRIX_CTRL_HIDDEN));
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 3, LV_BUTTONMATRIX_CTRL_HIDDEN));
}

void test_button_matrix_set_button_width_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);

    lv_buttonmatrix_set_button_width(btnm, 1, 3);
    lv_buttonmatrix_set_button_width(btnm, 2, 2);

    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 1, 3));
    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 2, 2));
}

void test_button_matrix_set_one_checked_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);

    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_one_checked(btnm, true);

    lv_buttonmatrix_set_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(btnm, 1, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(btnm, 2, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_buttonmatrix_set_button_ctrl(btnm, 3, LV_BUTTONMATRIX_CTRL_CHECKED);

    TEST_ASSERT_TRUE(lv_buttonmatrix_get_one_checked(btnm));

    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_CHECKED));
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 1, LV_BUTTONMATRIX_CTRL_CHECKED));
    TEST_ASSERT_FALSE(lv_buttonmatrix_has_button_ctrl(btnm, 2, LV_BUTTONMATRIX_CTRL_CHECKED));
    TEST_ASSERT_TRUE(lv_buttonmatrix_has_button_ctrl(btnm, 3, LV_BUTTONMATRIX_CTRL_CHECKED));
}

void test_button_matrix_get_button_text_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);

    TEST_ASSERT_EQUAL_STRING("A", lv_buttonmatrix_get_button_text(btnm, 0));
    TEST_ASSERT_EQUAL_STRING("B", lv_buttonmatrix_get_button_text(btnm, 1));
    TEST_ASSERT_EQUAL_STRING("C", lv_buttonmatrix_get_button_text(btnm, 2));
    TEST_ASSERT_EQUAL_STRING("D", lv_buttonmatrix_get_button_text(btnm, 3));
}

/* Common event handler for all the consecutive test cases. */
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == exp_evt_code) {
        event_triggered = true;
    }
}

void test_button_matrix_pressed_event_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_PRESSED, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_PRESSED;
    /* Click button index 0. */
    lv_test_mouse_click_at(10, 10);
    TEST_ASSERT_TRUE(event_triggered);
}

void test_button_matrix_release_event_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);
    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_RELEASED, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_RELEASED;

    /* Click button index 0. */
    lv_test_mouse_click_at(65, 35);
    TEST_ASSERT_TRUE(event_triggered);
    event_triggered = false;
    /* This will increase test coverage by unchecking the
     same button. */
    lv_buttonmatrix_set_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_POPOVER);
    /* Click button index 0. */
    lv_test_mouse_click_at(65, 35);
    TEST_ASSERT_TRUE(event_triggered);
}

void test_button_matrix_key_event_works(void)
{
    uint32_t keyCode;
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);
    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_obj_update_layout(btnm);         /*The force calculating the button areas*/
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_KEY, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_KEY;

    lv_buttonmatrix_t * btnmObj = (lv_buttonmatrix_t *)btnm;
    /* Select the first button. */
    lv_buttonmatrix_set_selected_button(btnm, 0);
    keyCode = LV_KEY_RIGHT;
    lv_obj_send_event(btnm, LV_EVENT_KEY, &keyCode);
    TEST_ASSERT_TRUE(event_triggered);
    uint16_t btnId = lv_buttonmatrix_get_selected_button(btnm);
    TEST_ASSERT_EQUAL_INT(1, btnId);

    event_triggered = false;
    keyCode = LV_KEY_LEFT;
    lv_obj_send_event(btnm, LV_EVENT_KEY, &keyCode);
    TEST_ASSERT_TRUE(event_triggered);
    btnId = lv_buttonmatrix_get_selected_button(btnm);
    TEST_ASSERT_EQUAL_INT(0, btnId);

    event_triggered = false;
    keyCode = LV_KEY_DOWN;
    lv_obj_send_event(btnm, LV_EVENT_KEY, &keyCode);
    TEST_ASSERT_TRUE(event_triggered);
    btnId = lv_buttonmatrix_get_selected_button(btnm);
    TEST_ASSERT_EQUAL_INT(2, btnId);

    event_triggered = false;
    keyCode = LV_KEY_UP;
    lv_obj_send_event(btnm, LV_EVENT_KEY, &keyCode);
    TEST_ASSERT_TRUE(event_triggered);
    btnId = lv_buttonmatrix_get_selected_button(btnm);
    TEST_ASSERT_EQUAL_INT(0, btnId);

    /* Added this code to increase code coverage. */
    btnmObj->btn_id_sel = LV_BUTTONMATRIX_BUTTON_NONE;
    lv_buttonmatrix_set_button_ctrl(btnm, 0, LV_BUTTONMATRIX_CTRL_HIDDEN);
    keyCode = LV_KEY_DOWN;
    lv_obj_send_event(btnm, LV_EVENT_KEY, &keyCode);
    TEST_ASSERT_TRUE(event_triggered);
    event_triggered = false;
}

void test_button_matrix_pressing_event_works(void)
{
    lv_buttonmatrix_t * btnmObj = (lv_buttonmatrix_t *)btnm;
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};
    lv_buttonmatrix_set_map(btnm, btn_map);
    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_PRESSING, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_PRESSING;
    /* Select a button before raising a simulated event.
     * This is done to increase code coverage. */
    btnmObj->btn_id_sel = 3;
    /* Send a dummy lv_indev_t object as param to avoid crashing during build. */
    lv_obj_send_event(btnm, LV_EVENT_PRESSING, lv_test_indev_get_indev(LV_INDEV_TYPE_POINTER));
    TEST_ASSERT_TRUE(event_triggered);
}

void test_button_matrix_long_press_repeat_event_works(void)
{
    lv_buttonmatrix_t * btnmObj = (lv_buttonmatrix_t *)btnm;
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};

    lv_buttonmatrix_set_map(btnm, btn_map);
    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_LONG_PRESSED_REPEAT, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_LONG_PRESSED_REPEAT;
    /* Select a button before raising a simulated event.
     * This is done to increase code coverage. */
    btnmObj->btn_id_sel = 0;
    lv_obj_send_event(btnm, LV_EVENT_LONG_PRESSED_REPEAT, NULL);
    TEST_ASSERT_TRUE(event_triggered);
}

void test_button_matrix_press_lost_event_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};

    lv_buttonmatrix_set_map(btnm, btn_map);
    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_PRESS_LOST, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_PRESS_LOST;
    lv_obj_send_event(btnm, LV_EVENT_PRESS_LOST, NULL);
    TEST_ASSERT_TRUE(event_triggered);
}

void test_button_matrix_defocused_event_works(void)
{
    lv_buttonmatrix_t * btnmObj = (lv_buttonmatrix_t *)btnm;
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};

    lv_buttonmatrix_set_map(btnm, btn_map);
    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_DEFOCUSED, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_DEFOCUSED;
    /* Select a button before raising a simulated event.
     * This is done to increase code coverage. */
    btnmObj->btn_id_sel = 0;
    lv_obj_send_event(btnm, LV_EVENT_DEFOCUSED, NULL);
    TEST_ASSERT_TRUE(event_triggered);
}

void test_button_matrix_focused_event_works(void)
{
    static const char * btn_map[] = {"A", "B", "\n", "C", "D", ""};

    lv_buttonmatrix_set_map(btnm, btn_map);
    lv_buttonmatrix_set_button_ctrl_all(btnm, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_obj_add_event_cb(btnm, event_handler, LV_EVENT_FOCUSED, NULL);
    /* Set expected event code before the event is raised. */
    exp_evt_code = LV_EVENT_FOCUSED;
    lv_obj_send_event(btnm, LV_EVENT_FOCUSED, NULL);
    TEST_ASSERT_TRUE(event_triggered);
}

void test_buttonmatrix_properties(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * obj = lv_buttonmatrix_create(lv_screen_active());

    lv_property_t prop = { };

    /* Test SELECTED_BUTTON property */
    prop.id = LV_PROPERTY_BUTTONMATRIX_SELECTED_BUTTON;
    prop.num = 2;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(2, lv_obj_get_property(obj, LV_PROPERTY_BUTTONMATRIX_SELECTED_BUTTON).num);

    /* Test ONE_CHECKED property */
    lv_buttonmatrix_set_button_ctrl_all(obj, LV_BUTTONMATRIX_CTRL_CHECKABLE);

    prop.id = LV_PROPERTY_BUTTONMATRIX_ONE_CHECKED;
    prop.num = 1;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(1, lv_obj_get_property(obj, LV_PROPERTY_BUTTONMATRIX_ONE_CHECKED).num);

    prop.num = 0;
    TEST_ASSERT_TRUE(lv_obj_set_property(obj, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(0, lv_obj_get_property(obj, LV_PROPERTY_BUTTONMATRIX_ONE_CHECKED).num);

    lv_obj_delete(obj);
#endif
}

void test_button_matrix_partial_clip_render(void)
{
    /*Create a small container that clips its content*/
    lv_obj_t * cont = lv_obj_create(active_screen);
    lv_obj_set_size(cont, 300, 120);
    lv_obj_center(cont);
    lv_obj_set_style_pad_all(cont, 0, 0);

    /*Create a buttonmatrix taller than the container*/
    static const char * map[] = {"A", "B", "C", "\n",
                                 "D", "E", "F", "\n",
                                 "G", "H", "I", "\n",
                                 "J", "K", "L", ""
                                };
    lv_obj_t * btnm_clip = lv_buttonmatrix_create(cont);
    lv_buttonmatrix_set_map(btnm_clip, map);
    lv_obj_set_size(btnm_clip, 280, 300);

    /*Scroll so the middle rows are visible, top and bottom rows clipped*/
    lv_obj_scroll_to_y(cont, 80, LV_ANIM_OFF);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/btnm_partial_clip.png");
}

static uint32_t drawn_buttons_mask;

static void partial_clip_draw_task_cb(lv_event_t * e)
{
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * base_dsc = lv_draw_task_get_draw_dsc(draw_task);
    if(base_dsc->part != LV_PART_ITEMS) return;
    if(lv_draw_task_get_fill_dsc(draw_task) == NULL) return;
    drawn_buttons_mask |= 1UL << base_dsc->id1;
}

void test_button_matrix_draw_task_culling(void)
{
    /*Buttons farther outside the clip area than this margin must not create
     *draw tasks.  The margin must be larger than the widget's internal clip
     *margin (row/column gap with a dpi/10 floor) so buttons whose shadow or
     *outline could reach the clip area are exempt from the check.*/
    const int32_t margin = 40;

    /*Create a small container that clips its content*/
    lv_obj_t * cont = lv_obj_create(active_screen);
    lv_obj_set_size(cont, 300, 150);
    lv_obj_center(cont);
    lv_obj_set_style_pad_all(cont, 0, 0);

    /*Create a buttonmatrix much taller than the container so entire rows
     *fall far outside the clip area on both sides*/
    static const char * map[] = {"A", "B", "C", "\n",
                                 "D", "E", "F", "\n",
                                 "G", "H", "I", "\n",
                                 "J", "K", "L", ""
                                };
    lv_obj_t * btnm_clip = lv_buttonmatrix_create(cont);
    lv_buttonmatrix_set_map(btnm_clip, map);
    lv_obj_set_size(btnm_clip, 280, 600);
    lv_obj_set_send_draw_task_events(btnm_clip, true);
    lv_obj_add_event_cb(btnm_clip, partial_clip_draw_task_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);

    /*Scroll so the middle rows are visible, top and bottom rows clipped*/
    lv_obj_update_layout(active_screen);
    lv_obj_scroll_to_y(cont, 225, LV_ANIM_OFF);

    drawn_buttons_mask = 0;
    lv_obj_invalidate(active_screen);
    lv_refr_now(NULL);

    lv_area_t clip_area;
    lv_obj_get_coords(cont, &clip_area);

    lv_area_t obj_coords;
    lv_obj_get_coords(btnm_clip, &obj_coords);

    lv_buttonmatrix_t * btnm_priv = (lv_buttonmatrix_t *)btnm_clip;
    bool culled_above = false;
    bool culled_below = false;
    uint32_t i;
    for(i = 0; i < btnm_priv->btn_cnt; i++) {
        lv_area_t btn_area = btnm_priv->button_areas[i];
        lv_area_move(&btn_area, obj_coords.x1, obj_coords.y1);
        bool drawn = (drawn_buttons_mask >> i) & 1;

        if(lv_area_is_on(&btn_area, &clip_area)) {
            TEST_ASSERT_TRUE_MESSAGE(drawn, "Button intersecting the clip area was not drawn");
        }
        else {
            lv_area_t enlarged = btn_area;
            lv_area_increase(&enlarged, margin, margin);
            if(!lv_area_is_on(&enlarged, &clip_area)) {
                TEST_ASSERT_FALSE_MESSAGE(drawn, "Button far outside the clip area created draw tasks");
                if(btn_area.y2 < clip_area.y1) culled_above = true;
                if(btn_area.y1 > clip_area.y2) culled_below = true;
            }
        }
    }

    /*Self-check: the geometry must exercise culling on both sides of the
     *clip area, otherwise the assertions above are vacuous*/
    TEST_ASSERT_TRUE_MESSAGE(culled_above, "No button far above the clip area, adjust the test geometry");
    TEST_ASSERT_TRUE_MESSAGE(culled_below, "No button far below the clip area, adjust the test geometry");
}

#endif
