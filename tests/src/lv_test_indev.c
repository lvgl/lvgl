#if LV_BUILD_TEST
#include "../lvgl.h"
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include "lv_test_indev.h"
#include "lv_test_init.h"

static int32_t x_act;
static int32_t y_act;
static uint32_t key_act;
static int32_t diff_act;
static bool mouse_pressed;
static bool key_pressed;
static bool enc_pressed;

void lv_test_mouse_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    LV_UNUSED(indev);
    lv_point_set(&data->point, x_act, y_act);
    data->state = mouse_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

void lv_test_mouse_move_to(int32_t x, int32_t y)
{
    x_act = x;
    y_act = y;
}

void lv_test_mouse_move_by(int32_t x, int32_t y)
{
    x_act += x;
    y_act += y;
}

void lv_test_mouse_press(void)
{
    mouse_pressed = true;
}

void lv_test_mouse_release(void)
{
    mouse_pressed = false;
}

void lv_test_mouse_click_at(int32_t x, int32_t y)
{
    lv_test_mouse_release();
    lv_test_indev_wait(50);
    lv_test_mouse_move_to(x, y);
    lv_test_mouse_press();
    lv_test_indev_wait(50);
    lv_test_mouse_release();
    lv_test_indev_wait(50);
}

void lv_test_keypad_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    LV_UNUSED(indev);
    data->key = key_act;
    data->state = key_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

void lv_test_key_press(uint32_t k)
{
    key_act = k;
    key_pressed = true;
}

void lv_test_key_release(void)
{
    key_pressed = false;
}

void lv_test_key_hit(uint32_t k)
{
    lv_test_key_release();
    lv_test_indev_wait(50);
    lv_test_key_press(k);
    lv_test_mouse_press();
    lv_test_indev_wait(50);
    lv_test_key_release();
    lv_test_indev_wait(50);
}

void lv_test_encoder_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    LV_UNUSED(indev);
    data->enc_diff = diff_act;
    data->state = enc_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    diff_act = 0;
}

void lv_test_encoder_add_diff(int32_t d)
{
    diff_act += d;
}

void lv_test_encoder_turn(int32_t d)
{
    diff_act += d;
    lv_test_indev_wait(50);
}

void lv_test_encoder_press(void)
{
    enc_pressed = true;
}

void lv_test_encoder_release(void)
{
    enc_pressed = false;
}

void lv_test_encoder_click(void)
{
    lv_test_encoder_release();
    lv_test_indev_wait(50);
    lv_test_encoder_press();
    lv_test_indev_wait(50);
    lv_test_encoder_release();
    lv_test_indev_wait(50);
}

void lv_test_indev_wait(uint32_t ms)
{
    uint32_t t = lv_tick_get();
    while(lv_tick_elaps(t) < ms) {
        lv_timer_handler();
        lv_tick_inc(1);
    }
}

#endif
