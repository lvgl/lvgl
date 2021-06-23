
#ifndef LV_TEST_INDEV_H
#define LV_TEST_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "../lvgl.h"

void lv_test_mouse_read_cb(lv_indev_drv_t * drv, lv_indev_data_t * data);

void lv_test_mouse_move_to(lv_coord_t x, lv_coord_t y);
void lv_test_mouse_move_by(lv_coord_t x, lv_coord_t y);
void lv_test_mouse_press(void);
void lv_test_mouse_release(void);
void lv_test_mouse_click_at(lv_coord_t x, lv_coord_t y);
void lv_test_mouse_wait(uint32_t ms);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_INDEV_H*/

