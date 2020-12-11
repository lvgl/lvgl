#include "../lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include "lv_test_core/lv_test_core.h"
#include "lv_test_widgets/lv_test_label.h"

#if LV_BUILD_TEST
#include <sys/time.h>

static void hal_init(void);
static void dummy_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

lv_color_t test_fb[LV_HOR_RES_MAX * LV_VER_RES_MAX];

int main(void)
{
    printf("Call lv_init...\n");
    lv_init();

    hal_init();

    lv_test_core();
    lv_test_label();

    printf("Exit with success!\n");
    return 0;
}


#if LV_USE_FILESYSTEM
static lv_fs_res_t open_cb(struct _lv_fs_drv_t * drv, void * file_p, const char * path, lv_fs_mode_t mode)
{
    (void) drv;
    (void) mode;

    FILE * fp = fopen(path, "rb"); // only reading is supported

    *((FILE **)file_p) = fp;
    return NULL == fp ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

static lv_fs_res_t close_cb(struct _lv_fs_drv_t * drv, void * file_p)
{
    (void) drv;

    FILE * fp = *((FILE **) file_p);
    fclose(fp);
    return LV_FS_RES_OK;
}

static lv_fs_res_t read_cb(struct _lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    (void) drv;

    FILE * fp = *((FILE **) file_p);
    *br = fread(buf, 1, btr, fp);
    return (*br <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

static lv_fs_res_t seek_cb(struct _lv_fs_drv_t * drv, void * file_p, uint32_t pos)
{
    (void) drv;

    FILE * fp = *((FILE **) file_p);
    fseek (fp, pos, SEEK_SET);

    return LV_FS_RES_OK;
}

static lv_fs_res_t tell_cb(struct _lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    (void) drv;

    FILE * fp = *((FILE **) file_p);
    *pos_p = ftell(fp);

    return LV_FS_RES_OK;
}

static bool ready_cb(struct _lv_fs_drv_t * drv)
{
    (void) drv;
    return true;
}
#endif

static void hal_init(void)
{
    static lv_disp_buf_t disp_buf;
    lv_color_t * disp_buf1 = (lv_color_t *)malloc(LV_HOR_RES * LV_VER_RES * sizeof(lv_color_t));

    lv_disp_buf_init(&disp_buf, disp_buf1, NULL, LV_HOR_RES * LV_VER_RES);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = dummy_flush_cb;
    lv_disp_drv_register(&disp_drv);

#if LV_USE_FILESYSTEM
    lv_fs_drv_t drv;
    lv_fs_drv_init(&drv);                     /*Basic initialization*/

    drv.letter = 'f';                         /*An uppercase letter to identify the drive */
    drv.file_size = sizeof(FILE *);   /*Size required to store a file object*/
    drv.ready_cb = ready_cb;               /*Callback to tell if the drive is ready to use */
    drv.open_cb = open_cb;                 /*Callback to open a file */
    drv.close_cb = close_cb;               /*Callback to close a file */
    drv.read_cb = read_cb;                 /*Callback to read a file */
    drv.seek_cb = seek_cb;                 /*Callback to seek in a file (Move cursor) */
    drv.tell_cb = tell_cb;                 /*Callback to tell the cursor position  */

    lv_fs_drv_register(&drv);                 /*Finally register the drive*/
#endif
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

