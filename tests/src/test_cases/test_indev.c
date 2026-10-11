#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static void indev_reset_event_cb(lv_event_t * e)
{
    lv_obj_t * scroll_obj = lv_event_get_target(e);
    lv_obj_t * act_obj = lv_obj_get_child(scroll_obj, 0);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SCROLL && act_obj) {
        lv_obj_delete(act_obj);
    }
    else if(code == LV_EVENT_INDEV_RESET) {
        uint32_t * indev_reset_count = lv_event_get_user_data(e);
        (*indev_reset_count)++;
    }
}

void test_indev_obj_delete_reset(void)
{
    lv_obj_t * scroll_obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(scroll_obj, 300, 300);
    lv_obj_align(scroll_obj, LV_ALIGN_LEFT_MID, 0, 0);

    uint32_t indev_reset_count = 0;
    lv_obj_add_event_cb(scroll_obj, indev_reset_event_cb, LV_EVENT_ALL, &indev_reset_count);

    lv_obj_t * act_obj = lv_obj_create(scroll_obj);
    lv_obj_set_size(act_obj, 400, 200);
    lv_obj_align(act_obj, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_update_layout(scroll_obj);

    lv_test_mouse_move_to(200, 200);
    lv_test_mouse_press();
    lv_test_wait(50);

    lv_test_mouse_move_by(-20, 0);
    lv_test_mouse_press();
    lv_test_wait(50);

    lv_test_mouse_move_by(-20, 0);
    lv_test_mouse_press();
    lv_test_wait(50);

    lv_test_mouse_release();

    TEST_ASSERT_EQUAL_UINT32(1, indev_reset_count);
}

static void indev_wait_release_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    uint32_t * pressed_count = lv_event_get_user_data(e);

    switch(lv_event_get_code(e)) {
        case LV_EVENT_PRESSED:
            (*pressed_count)++;
            lv_indev_wait_release(lv_indev_active());
            break;
        case LV_EVENT_PRESS_LOST:
            lv_indev_reset(lv_indev_active(), obj);
            break;
        default:
            break;
    }
}

void test_indev_wait_release(void)
{
    uint32_t pressed_count = 0;
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 100);
    lv_obj_add_event_cb(btn, indev_wait_release_event_cb, LV_EVENT_ALL, &pressed_count);

    lv_test_mouse_release();
    lv_test_wait(50);
    lv_test_mouse_move_to(50, 50);

    lv_test_mouse_press();
    lv_test_wait(50);
    lv_test_mouse_release();
    lv_test_wait(50);

    lv_test_mouse_press();
    lv_test_wait(50);
    lv_test_mouse_release();
    lv_test_wait(50);

    TEST_ASSERT_EQUAL_UINT32(2, pressed_count);
}

static void indev_long_pressed_event_cb(lv_event_t * e)
{
    uint32_t * long_pressed_cnt = lv_event_get_user_data(e);
    (*long_pressed_cnt)++;
}

void test_indev_long_pressed(void)
{
    uint32_t long_pressed_cnt = 0;
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 100);
    lv_obj_add_event_cb(btn, indev_long_pressed_event_cb, LV_EVENT_LONG_PRESSED, &long_pressed_cnt);
    lv_test_mouse_release();
    lv_test_wait(50);
    lv_test_mouse_move_to(50, 50);
    lv_test_mouse_press();
    lv_test_wait(500);
    lv_test_mouse_release();
    lv_test_wait(50);
    TEST_ASSERT_EQUAL_UINT32(1, long_pressed_cnt);
}

void test_indev_long_press_time(void)
{
    uint32_t long_pressed_cnt = 0;
    lv_indev_t * mouse = lv_test_indev_get_indev(LV_INDEV_TYPE_POINTER);

    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 100);
    lv_obj_add_event_cb(btn, indev_long_pressed_event_cb, LV_EVENT_LONG_PRESSED, &long_pressed_cnt);

    lv_test_mouse_release();
    lv_test_wait(50);
    lv_test_mouse_move_to(50, 50);

    /*With a short long press time 250 ms is enough to trigger*/
    lv_indev_set_long_press_time(mouse, 200);
    lv_test_mouse_press();
    lv_test_wait(150);
    TEST_ASSERT_EQUAL_UINT32(0, long_pressed_cnt);
    lv_test_wait(100);
    TEST_ASSERT_EQUAL_UINT32(1, long_pressed_cnt);
    lv_test_mouse_release();
    lv_test_wait(50);

    /*With a long long press time even 450 ms (more than the default 400 ms) is not enough*/
    lv_indev_set_long_press_time(mouse, 600);
    lv_test_mouse_press();
    lv_test_wait(450);
    TEST_ASSERT_EQUAL_UINT32(1, long_pressed_cnt);
    lv_test_wait(250);
    TEST_ASSERT_EQUAL_UINT32(2, long_pressed_cnt);
    lv_test_mouse_release();
    lv_test_wait(50);

    /*Restore the default value*/
    lv_indev_set_long_press_time(mouse, 400);
}

static void indev_double_clicked_event_cb(lv_event_t * e)
{
    uint32_t * double_clicked_cnt = lv_event_get_user_data(e);
    (*double_clicked_cnt)++;
}

static void indev_short_click(void)
{
    lv_test_mouse_press();
    lv_test_wait(30);
    lv_test_mouse_release();
    lv_test_wait(30);
}

void test_indev_double_click_time(void)
{
    uint32_t double_clicked_cnt = 0;
    lv_indev_t * mouse = lv_test_indev_get_indev(LV_INDEV_TYPE_POINTER);

    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 100);
    lv_obj_add_event_cb(btn, indev_double_clicked_event_cb, LV_EVENT_DOUBLE_CLICKED, &double_clicked_cnt);

    lv_test_mouse_release();
    lv_test_wait(50);
    lv_test_mouse_move_to(50, 50);

    lv_indev_set_double_click_time(mouse, 300);

    /*Two clicks ~160 ms apart are within the 300 ms double click time*/
    indev_short_click();
    lv_test_wait(100);
    indev_short_click();
    TEST_ASSERT_EQUAL_UINT32(1, double_clicked_cnt);

    /*Wait long enough to reset the click streak*/
    lv_test_wait(400);

    /*Two clicks ~410 ms apart are outside of the 300 ms double click time*/
    indev_short_click();
    lv_test_wait(350);
    indev_short_click();
    TEST_ASSERT_EQUAL_UINT32(1, double_clicked_cnt);

    /*Wait long enough to reset the click streak*/
    lv_test_wait(700);

    /*With a larger double click time the same ~410 ms gap results in a double click*/
    lv_indev_set_double_click_time(mouse, 600);
    indev_short_click();
    lv_test_wait(350);
    indev_short_click();
    TEST_ASSERT_EQUAL_UINT32(2, double_clicked_cnt);

    /*Restore the default value*/
    lv_indev_set_double_click_time(mouse, 400);
}

static void indev_scroll_press_event_cb(lv_event_t * e)
{
    uint32_t * pressed_count = lv_event_get_user_data(e);

    switch(lv_event_get_code(e)) {
        case LV_EVENT_PRESSED:
            (*pressed_count)++;
            break;
        default:
            break;
    }
}

static void indev_scroll_press_lost_event_cb(lv_event_t * e)
{
    uint32_t * press_lost_count = lv_event_get_user_data(e);

    switch(lv_event_get_code(e)) {
        case LV_EVENT_PRESS_LOST:
            (*press_lost_count)++;
            break;
        default:
            break;
    }
}

void test_indev_scroll_between_two_buttons_with_and_without_press_lock(void)
{
    uint32_t pressed_count_1 = 0;
    uint32_t pressed_lost_count_1 = 0;
    uint32_t pressed_count_2 = 0;
    uint32_t pressed_lost_count_2 = 0;

    lv_obj_t * btn1 = lv_button_create(lv_screen_active());
    lv_obj_set_press_lock(btn1, false);
    lv_obj_set_size(btn1, 120, 100);
    lv_obj_set_pos(btn1, 300, 200);
    lv_obj_add_event_cb(btn1, indev_scroll_press_event_cb, LV_EVENT_PRESSED,
                        &pressed_count_1);
    lv_obj_add_event_cb(btn1, indev_scroll_press_lost_event_cb,
                        LV_EVENT_PRESS_LOST, &pressed_lost_count_1);

    lv_obj_t * btn2 = lv_button_create(lv_screen_active());
    lv_obj_set_style_bg_color(btn2, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_size(btn2, 120, 100);
    lv_obj_set_pos(btn2, 900, 200);
    lv_obj_add_event_cb(btn2, indev_scroll_press_event_cb, LV_EVENT_PRESSED,
                        &pressed_count_2);
    lv_obj_add_event_cb(btn2, indev_scroll_press_lost_event_cb,
                        LV_EVENT_PRESS_LOST, &pressed_lost_count_2);

    TEST_ASSERT_EQUAL_SCREENSHOT("scroll_initial.png");

    lv_test_mouse_move_to(350, 250);
    lv_test_mouse_press();
    lv_test_wait(50);
    TEST_ASSERT_EQUAL_UINT32(1, pressed_count_1);

    lv_test_mouse_move_by(-300, 0);
    lv_test_wait(50);
    lv_test_mouse_release();
    lv_test_wait(1000);
    TEST_ASSERT_EQUAL_UINT32(1, pressed_lost_count_1);
    TEST_ASSERT_EQUAL_SCREENSHOT("scroll_after.png");

    lv_test_mouse_move_to(750, 250);
    lv_test_mouse_press();
    lv_test_wait(50);
    TEST_ASSERT_EQUAL_UINT32(1, pressed_count_2);

    lv_test_mouse_move_by(300, 0);
    lv_test_wait(50);
    lv_test_mouse_release();
    lv_test_wait(1000);
    TEST_ASSERT_EQUAL_UINT32(0, pressed_lost_count_2);
    TEST_ASSERT_EQUAL_SCREENSHOT("scroll_initial.png");
}

void test_indev_ccw_pointer(void)
{
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);

    TEST_ASSERT_EQUAL(LV_ROTATION_DIR_CW, lv_indev_get_rotation_dir(indev));

    lv_indev_set_rotation_dir(indev, LV_ROTATION_DIR_CCW);

    TEST_ASSERT_EQUAL(LV_ROTATION_DIR_CCW, lv_indev_get_rotation_dir(indev));

    lv_indev_set_rotation_dir(indev, LV_ROTATION_DIR_CW);

    TEST_ASSERT_EQUAL(LV_ROTATION_DIR_CW, lv_indev_get_rotation_dir(indev));
}

static lv_point_t rotation_raw_point;

static void rotation_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    LV_UNUSED(indev);
    data->point = rotation_raw_point;
    data->state = LV_INDEV_STATE_RELEASED;
}

static void rotation_never_flushed(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    LV_UNUSED(disp);
    LV_UNUSED(area);
    LV_UNUSED(color_p);
    TEST_FAIL();
}

void test_indev_pointer_rotation(void)
{
    const int32_t width = 480;
    const int32_t height = 320;
    static const struct {
        lv_rotation_t rotation;
        lv_rotation_dir_t rotation_dir;
        int32_t x;
        int32_t y;
    } cases[] = {
        {LV_ROTATION_0, LV_ROTATION_DIR_CW, 10, 20},
        {LV_ROTATION_90, LV_ROTATION_DIR_CW, 299, 10},
        {LV_ROTATION_180, LV_ROTATION_DIR_CW, 469, 299},
        {LV_ROTATION_270, LV_ROTATION_DIR_CW, 20, 469},

        {LV_ROTATION_0, LV_ROTATION_DIR_CCW, 10, 20},
        {LV_ROTATION_90, LV_ROTATION_DIR_CCW, 20, 469},
        {LV_ROTATION_180, LV_ROTATION_DIR_CCW, 469, 299},
        {LV_ROTATION_270, LV_ROTATION_DIR_CCW, 299, 10},
    };

    lv_display_t * disp = lv_display_create(width, height);
    TEST_ASSERT_NOT_NULL(disp);
    lv_display_set_flush_cb(disp, rotation_never_flushed);

    lv_indev_t * indev = lv_indev_create();
    TEST_ASSERT_NOT_NULL(indev);
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, rotation_read_cb);
    lv_indev_set_display(indev, disp);

    for(uint32_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        lv_display_set_rotation(disp, cases[i].rotation);
        lv_indev_set_rotation_dir(indev, cases[i].rotation_dir);

        rotation_raw_point.x = 10;
        rotation_raw_point.y = 20;
        lv_indev_read(indev);

        lv_point_t point;
        lv_indev_get_point(indev, &point);
        TEST_ASSERT_EQUAL_INT32(cases[i].x, point.x);
        TEST_ASSERT_EQUAL_INT32(cases[i].y, point.y);
    }

    lv_indev_delete(indev);
    lv_display_delete(disp);
}

static void forget_pressed_obj_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) != LV_EVENT_PRESSED) return;

    lv_indev_t * indev = lv_indev_active();
    lv_indev_wait_release(indev);
    /*Drop the pressed object while the release is still pending*/
    lv_indev_reset(indev, lv_event_get_target(e));
}

void test_release_without_pressed_object(void)
{
    lv_obj_t * btn = lv_obj_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 100);
    lv_obj_set_pos(btn, 0, 0);
    lv_obj_add_event_cb(btn, forget_pressed_obj_cb, LV_EVENT_ALL, NULL);

    lv_test_mouse_move_to(50, 50);
    lv_test_mouse_press();
    lv_test_wait(50);
    lv_test_mouse_release();
    lv_test_wait(50);
    /* if no assertion fired we're good*/
    TEST_PASS();
}

#endif
