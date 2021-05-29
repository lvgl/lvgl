#include "../lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include "lv_test_core/lv_test_core.h"
#include "lv_test_widgets/lv_test_label.h"

#if LV_BUILD_TEST && !defined(LV_BUILD_TEST_NO_MAIN)
#include <sys/time.h>

#define HOR_RES 800
#define VER_RES 480

static void hal_init(void);
static void dummy_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

lv_color_t test_fb[HOR_RES * VER_RES];

int main(void)
{
    printf("Call lv_init...\n");
    lv_init();

    hal_init();

    lv_test_core();
//    lv_test_label();

    printf("Exit with success!\n");
    return 0;
}

static void * open_cb(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    (void) drv;
    (void) mode;

    FILE * fp = fopen(path, "rb"); // only reading is supported

    return fp;
}

static lv_fs_res_t close_cb(lv_fs_drv_t * drv, void * file_p)
{
    (void) drv;

    fclose(file_p);
    return LV_FS_RES_OK;
}

static lv_fs_res_t read_cb(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    (void) drv;

    *br = fread(buf, 1, btr, file_p);
    return (*br <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

static lv_fs_res_t seek_cb(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t w)
{
    (void) drv;

    uint32_t w2;
    switch(w) {
    case LV_FS_SEEK_SET:
        w2 = SEEK_SET;
        break;
    case LV_FS_SEEK_CUR:
        w2 = SEEK_CUR;
        break;
    case LV_FS_SEEK_END:
        w2 = SEEK_END;
        break;
    default:
        w2 = SEEK_SET;
    }

    fseek (file_p, pos, w2);

    return LV_FS_RES_OK;
}

static lv_fs_res_t tell_cb(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    (void) drv;

    *pos_p = ftell(file_p);

    return LV_FS_RES_OK;
}


static void hal_init(void)
{
    static lv_disp_draw_buf_t draw_buf;
    lv_color_t * disp_buf1 = (lv_color_t *)malloc(LV_HOR_RES * LV_VER_RES * sizeof(lv_color_t));

    lv_disp_draw_buf_init(&draw_buf, disp_buf1, NULL, LV_HOR_RES * LV_VER_RES);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &draw_buf;
    disp_drv.flush_cb = dummy_flush_cb;
    disp_drv.hor_res = HOR_RES;
    disp_drv.ver_res = VER_RES;
    lv_disp_drv_register(&disp_drv);

    static lv_fs_drv_t drv;
    lv_fs_drv_init(&drv);                     /*Basic initialization*/

    drv.letter = 'F';                         /*An uppercase letter to identify the drive*/
    drv.open_cb = open_cb;                 /*Callback to open a file*/
    drv.close_cb = close_cb;               /*Callback to close a file*/
    drv.read_cb = read_cb;                 /*Callback to read a file*/
    drv.seek_cb = seek_cb;                 /*Callback to seek in a file (Move cursor)*/
    drv.tell_cb = tell_cb;                 /*Callback to tell the cursor position*/

    lv_fs_drv_register(&drv);                 /*Finally register the drive*/
}
#include <stdio.h>

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

#endif
