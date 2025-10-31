/**
 * @file lv_test_indev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_test.h"
#if LV_USE_TEST

#include "../../core/lv_global.h"
#include "../../lvgl_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_test_mouse_read_cb(lv_indev_t * indev, lv_indev_data_t * data);
static void lv_test_keypad_read_cb(lv_indev_t * indev, lv_indev_data_t * data);
static void lv_test_encoder_read_cb(lv_indev_t * indev, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#define _state LV_GLOBAL_DEFAULT()->test_state

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_test_indev_create_all(void)
{
    _state.mouse_indev = lv_indev_create();
    lv_indev_set_type(_state.mouse_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(_state.mouse_indev,  lv_test_mouse_read_cb);

    _state.keypad_indev = lv_indev_create();
    lv_indev_set_type(_state.keypad_indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(_state.keypad_indev,  lv_test_keypad_read_cb);

    _state.encoder_indev = lv_indev_create();
    lv_indev_set_type(_state.encoder_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(_state.encoder_indev,  lv_test_encoder_read_cb);
}

void lv_test_indev_delete_all(void)
{
    if(_state.mouse_indev) {
        lv_indev_delete(_state.mouse_indev);
        _state.mouse_indev = NULL;
    }

    if(_state.keypad_indev) {
        lv_indev_delete(_state.keypad_indev);
        _state.keypad_indev = NULL;
    }

    if(_state.encoder_indev) {
        lv_indev_delete(_state.encoder_indev);
        _state.encoder_indev = NULL;
    }
}

lv_indev_t * lv_test_indev_get_indev(lv_indev_type_t type)
{
    switch(type) {
        case LV_INDEV_TYPE_POINTER:
            return _state.mouse_indev;
        case LV_INDEV_TYPE_KEYPAD:
            return _state.keypad_indev;
        case LV_INDEV_TYPE_ENCODER:
            return _state.encoder_indev;
        default:
            return NULL;

    }
}

void lv_test_mouse_move_to(int32_t x, int32_t y)
{
    _state.x_act = x;
    _state.y_act = y;
}


void lv_test_mouse_move_to_obj(lv_obj_t * obj)
{
    int32_t x = obj->coords.x1 + lv_obj_get_width(obj) / 2;
    int32_t y = obj->coords.y1 + lv_obj_get_height(obj) / 2;
    lv_test_mouse_move_to(x, y);
}

void lv_test_mouse_move_by(int32_t x, int32_t y)
{
    _state.x_act += x;
    _state.y_act += y;
}

void lv_test_mouse_press(void)
{
    _state.mouse_pressed = true;
}

void lv_test_mouse_release(void)
{
    _state.mouse_pressed = false;
}

void lv_test_mouse_click_at(int32_t x, int32_t y)
{
    lv_test_mouse_release();
    lv_test_wait(50);
    lv_test_mouse_move_to(x, y);
    lv_test_mouse_press();
    lv_test_wait(50);
    lv_test_mouse_release();
    lv_test_wait(50);
}

void lv_test_key_press(uint32_t k)
{
    _state.key_act = k;
    _state.key_pressed = true;
}

void lv_test_key_release(void)
{
    _state.key_pressed = false;
}

void lv_test_key_hit(uint32_t k)
{
    lv_test_key_release();
    lv_test_wait(50);
    lv_test_key_press(k);
    lv_test_wait(50);
    lv_test_key_release();
    lv_test_wait(50);
}

void lv_test_encoder_add_diff(int32_t d)
{
    _state.diff_act += d;
}

void lv_test_encoder_turn(int32_t d)
{
    _state.diff_act += d;
    lv_test_wait(50);
}

void lv_test_encoder_press(void)
{
    _state.enc_pressed = true;
}

void lv_test_encoder_release(void)
{
    _state.enc_pressed = false;
}

void lv_test_encoder_click(void)
{
    lv_test_encoder_release();
    lv_test_wait(50);
    lv_test_encoder_press();
    lv_test_wait(50);
    lv_test_encoder_release();
    lv_test_wait(50);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_test_mouse_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    LV_UNUSED(indev);
    lv_point_set(&data->point, _state.x_act, _state.y_act);
    data->state = _state.mouse_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}


static void lv_test_keypad_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    LV_UNUSED(indev);
    data->key = _state.key_act;
    data->state = _state.key_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}


static void lv_test_encoder_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    LV_UNUSED(indev);
    data->enc_diff = _state.diff_act;
    data->state = _state.enc_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    _state.diff_act = 0;
}

#endif /*LV_USE_TEST*/
