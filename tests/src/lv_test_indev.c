#include "../lvgl.h"
#include <stdio.h>
#include <stdlib.h>

#if LV_BUILD_TEST
#include <sys/time.h>
#include "lv_test_indev.h"
#include "lv_test_init.h"

static lv_coord_t x_act;
static lv_coord_t y_act;
static bool pressed;

void lv_test_mouse_read_cb(lv_indev_drv_t * drv, lv_indev_data_t * data) 
{
  LV_UNUSED(drv);
  data->point.x = x_act;
  data->point.y = y_act;
  data->state = pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

void lv_test_mouse_move_to(lv_coord_t x, lv_coord_t y)
{
  x_act = x;
  y_act = y;  
}

void lv_test_mouse_move_by(lv_coord_t x, lv_coord_t y)
{
  x_act += x;
  y_act += y;
}

void lv_test_mouse_press(void)
{
  pressed = true;
}

void lv_test_mouse_release(void)
{
  pressed = false;
}

void lv_test_mouse_click_at(lv_coord_t x, lv_coord_t y)
{ 
  lv_test_mouse_release();
  lv_test_mouse_wait(50);
  lv_test_mouse_move_to(x, y);
  lv_test_mouse_press();
  lv_test_mouse_wait(50);
  lv_test_mouse_release();
  lv_test_mouse_wait(50);
}

void lv_test_mouse_wait(uint32_t ms)
{
  uint32_t t = lv_tick_get();
  while(lv_tick_elaps(t) < ms) {
    lv_timer_handler();
    lv_tick_inc(1);
  }
}


#endif
