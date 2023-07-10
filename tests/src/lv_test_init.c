
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
static void dummy_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

lv_indev_t * lv_test_mouse_indev;
lv_indev_t * lv_test_keypad_indev;
lv_indev_t * lv_test_encoder_indev;

lv_color_t test_fb[HOR_RES * VER_RES];
static lv_color_t disp_buf1[HOR_RES * VER_RES];

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
    static lv_disp_draw_buf_t draw_buf;

    lv_disp_draw_buf_init(&draw_buf, disp_buf1, NULL, HOR_RES * VER_RES);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &draw_buf;
    disp_drv.flush_cb = dummy_flush_cb;
    disp_drv.hor_res = HOR_RES;
    disp_drv.ver_res = VER_RES;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_mouse_drv;
    lv_indev_drv_init(&indev_mouse_drv);
    indev_mouse_drv.type = LV_INDEV_TYPE_POINTER;
    indev_mouse_drv.read_cb = lv_test_mouse_read_cb;
    lv_test_mouse_indev = lv_indev_drv_register(&indev_mouse_drv);

    static lv_indev_drv_t indev_keypad_drv;
    lv_indev_drv_init(&indev_keypad_drv);
    indev_keypad_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_keypad_drv.read_cb = lv_test_keypad_read_cb;
    lv_test_keypad_indev = lv_indev_drv_register(&indev_keypad_drv);

    static lv_indev_drv_t indev_encoder_drv;
    lv_indev_drv_init(&indev_encoder_drv);
    indev_encoder_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_encoder_drv.read_cb = lv_test_encoder_read_cb;
    lv_test_encoder_indev = lv_indev_drv_register(&indev_encoder_drv);
}

static void dummy_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    LV_UNUSED(area);
    LV_UNUSED(color_p);

    memcpy(test_fb, color_p, lv_area_get_size(area) * sizeof(lv_color_t));

    lv_disp_flush_ready(disp_drv);
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
    TEST_FAIL();
}

#endif
