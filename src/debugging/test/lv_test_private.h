/**
 * @file lv_test_private.h
 *
 */

#ifndef LV_TEST_PRIVATE_H
#define LV_TEST_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_TEST

#include "../../misc/lv_types.h"
#include "../../indev/lv_indev_gesture.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_indev_t * mouse_indev;
    lv_indev_t * keypad_indev;
    lv_indev_t * encoder_indev;

    lv_draw_buf_t draw_buf;

    int32_t x_act;
    int32_t y_act;
    uint32_t key_act;
    int32_t diff_act;
    bool mouse_pressed;
    bool key_pressed;
    bool enc_pressed;

#if LV_USE_GESTURE_RECOGNITION
    lv_indev_t * gesture_indev;
    lv_indev_touch_data_t * touch_data;
    uint8_t max_touch_cnt;
#endif
} lv_test_state_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEST*/

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif /*LV_TEST_PRIVATE_H*/
