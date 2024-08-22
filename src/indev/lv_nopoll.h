#ifndef LV_NOPOLL_H
#define LV_NOPOLL_H

/* lvgl pointer input device without polling */

#include <stdint.h>

void lv_nopoll_create();
void lv_nopoll_click(int32_t x, int32_t y);
void lv_nopoll_swipe_up(int32_t x, int32_t y);
void lv_nopoll_swipe_down(int32_t x, int32_t y);
void lv_nopoll_swipe_left(int32_t x, int32_t y);
void lv_nopoll_swipe_right(int32_t x, int32_t y);
void lv_nopoll_long_press(int32_t x, int32_t y);

#endif
