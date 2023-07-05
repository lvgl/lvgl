#if LV_BUILD_TEST
#include "lv_test_init.h"
#include "lv_test_indev.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "../unity/unity.h"

#define HOR_RES 800
#define VER_RES 480

static void hal_init(void);
static void dummy_flush_cb(lv_disp_t * disp, const lv_area_t * area, uint8_t * color_p);

lv_indev_t * lv_test_mouse_indev;
lv_indev_t * lv_test_keypad_indev;
lv_indev_t * lv_test_encoder_indev;

lv_color32_t test_fb[HOR_RES * VER_RES];

void lv_test_init(void)
{
    lv_init();
    hal_init();
}

void lv_test_deinit(void)
{
    lv_mem_deinit();
}

static void hal_init(void)
{
    lv_disp_t * disp = lv_disp_create(HOR_RES, VER_RES);
    lv_disp_set_draw_buffers(disp, test_fb, NULL, HOR_RES * VER_RES, LV_DISP_RENDER_MODE_DIRECT);
    lv_disp_set_flush_cb(disp, dummy_flush_cb);

    lv_test_mouse_indev = lv_indev_create();
    lv_indev_set_type(lv_test_mouse_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(lv_test_mouse_indev,  lv_test_mouse_read_cb);

    lv_test_keypad_indev = lv_indev_create();
    lv_indev_set_type(lv_test_keypad_indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(lv_test_keypad_indev,  lv_test_keypad_read_cb);

    lv_test_encoder_indev = lv_indev_create();
    lv_indev_set_type(lv_test_encoder_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(lv_test_encoder_indev,  lv_test_encoder_read_cb);
}

static void dummy_flush_cb(lv_disp_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    LV_UNUSED(area);
    LV_UNUSED(color_p);

    lv_disp_flush_ready(disp);
}

uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}

void lv_test_assert_fail(void)
{
    // Handle error on test
}

#endif
