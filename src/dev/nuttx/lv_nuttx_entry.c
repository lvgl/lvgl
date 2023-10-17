/**
 * @file lv_nuttx_entry.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_nuttx_entry.h"

#if LV_USE_NUTTX

#include <time.h>
#include <nuttx/tls.h>
#include <syslog.h>
#include <lvgl/lvgl.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint32_t millis(void);
static void syslog_print(lv_log_level_t level, const char * buf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if LV_ENABLE_GLOBAL_CUSTOM

/****************************************************************************
 * Name: lv_global_free
 ****************************************************************************/

static void lv_global_free(void * data)
{
    if(data) {
        free(data);
    }
}

/****************************************************************************
 * Name: lv_global_default
 ****************************************************************************/

lv_global_t * lv_global_default(void)
{
    static int index = -1;
    lv_global_t * data;

    if(index < 0) {
        index = task_tls_alloc(lv_global_free);
    }

    if(index >= 0) {
        data = (lv_global_t *)task_tls_get_value(index);
        if(data == NULL) {
            data = (lv_global_t *)calloc(1, sizeof(lv_global_t));
            task_tls_set_value(index, (uintptr_t)data);
        }
    }
    return data;
}
#endif

void lv_nuttx_info_init(lv_nuttx_t * info)
{
    lv_memzero(info, sizeof(lv_nuttx_t));
    info->fb_path = "/dev/fb0";
    info->input_path = "/dev/input0";
}

lv_display_t * lv_nuttx_init(const lv_nuttx_t * info)
{
    lv_display_t * disp = NULL;

    lv_log_register_print_cb(syslog_print);
    lv_tick_set_cb(millis);

#if !LV_USE_NUTTX_CUSTOM_INIT

    if(info && info->fb_path) {
#if LV_USE_NUTTX_LCD
        disp = lv_nuttx_lcd_create(info->fb_path);
#else
        disp = lv_nuttx_fbdev_create();
        if(lv_nuttx_fbdev_set_file(disp, info->fb_path) != 0) {
            lv_display_remove(disp);
            disp = NULL;
        }
#endif
    }

    if(info && info->input_path) {
#if LV_USE_NUTTX_TOUCHSCREEN
        lv_nuttx_touchscreen_create(info->input_path);
#endif
    }

#else

    disp = lv_nuttx_init_custom(info);
#endif

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint32_t millis(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint32_t tick = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

    return tick;
}

static void syslog_print(lv_log_level_t level, const char * buf)
{
    static const int priority[_LV_LOG_LEVEL_NUM] = {
        LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERR, LOG_CRIT
    };

    syslog(priority[level], "[LVGL] %s", buf);
}

#endif /*LV_USE_NUTTX*/
