#include "../lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "lv_test_core/lv_test_core.h"

#if LV_BUILD_TEST

static void hal_init(void);
static void dummy_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

int main(void)
{
    printf("Call lv_init...\n");
    lv_init();

    hal_init();

    lv_test_core();

    printf("Exit with success!\n");
    return 0;
}


static void hal_init(void)
{
    static lv_disp_buf_t disp_buf;
    lv_color_t * disp_buf1 = (lv_color_t *)malloc(LV_HOR_RES * LV_VER_RES * sizeof(lv_color_t));

    lv_disp_buf_init(&disp_buf, disp_buf1, NULL, LV_HOR_RES* LV_VER_RES);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = dummy_flush_cb;
    lv_disp_drv_register(&disp_drv);
}


static void dummy_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
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

#endif

