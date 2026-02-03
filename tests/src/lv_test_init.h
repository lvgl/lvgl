
#ifndef LV_TEST_INIT_H
#define LV_TEST_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <../lvgl.h>

#define LV_TEST_DISPLAY_HOR_RES 800
#define LV_TEST_DISPLAY_VER_RES 480

void lv_test_init(void);
void lv_test_deinit(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_INIT_H*/
