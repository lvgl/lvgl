
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

void lv_test_keypad_read_cb(lv_indev_drv_t * drv, lv_indev_data_t * data);

void lv_test_key_press(uint32_t k);
void lv_test_key_release(void);
void lv_test_key_hit(uint32_t k);

/* encoder read callback */
void lv_test_encoder_read_cb(lv_indev_drv_t * drv, lv_indev_data_t * data) ;

/* Simulate encoder rotation, use positive parameter to rotate to the right
 * and negative to rotate to the left */
void lv_test_encoder_add_diff(int32_t d);
/* Same as lv_test_encoder_add_diff but with additional delay */
void lv_test_encoder_turn(int32_t d);
/* Set encoder to pressed */
void lv_test_encoder_press(void);
/* Set encoder to released */
void lv_test_encoder_release(void);
/* Simulate release+press+release (including delays) */
void lv_test_encoder_click(void);

/* Simulate delay */
void lv_test_indev_wait(uint32_t ms);

extern lv_indev_t * lv_test_mouse_indev;
extern lv_indev_t * lv_test_keypad_indev;
extern lv_indev_t * lv_test_encoder_indev;


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_INDEV_H*/

