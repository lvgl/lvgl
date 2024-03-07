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
#include <nuttx/clock.h>
#include <syslog.h>
#include "lv_nuttx_cache.h"
#include "lv_nuttx_image_cache.h"
#include "lv_nuttx_profiler.h"

#include "../../../lvgl.h"

/*********************
 *      DEFINES
 *********************/
#define nuttx_ctx_p (LV_GLOBAL_DEFAULT()->nuttx_ctx)
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint32_t millis(void);
#if LV_USE_LOG
    static void syslog_print(lv_log_level_t level, const char * buf);
#endif

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

static void lv_global_free(void * data)
{
    if(data) {
        free(data);
    }
}

lv_global_t * lv_global_default(void)
{
    static int index = -1;
    lv_global_t * data = NULL;

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

void lv_nuttx_dsc_init(lv_nuttx_dsc_t * dsc)
{
    if(dsc == NULL)
        return;

    lv_memzero(dsc, sizeof(lv_nuttx_dsc_t));
    dsc->fb_path = "/dev/fb0";
    dsc->input_path = "/dev/input0";

#ifdef CONFIG_UINPUT_TOUCH
    dsc->utouch_path = "/dev/utouch";
#endif
}

void lv_nuttx_init(const lv_nuttx_dsc_t * dsc, lv_nuttx_result_t * result)
{
    nuttx_ctx_p = lv_malloc_zeroed(sizeof(lv_nuttx_ctx_t));
    LV_ASSERT_MALLOC(nuttx_ctx_p);

#if LV_USE_LOG
    lv_log_register_print_cb(syslog_print);
#endif
    lv_tick_set_cb(millis);

    lv_nuttx_cache_init();

#if LV_CACHE_DEF_SIZE > 0
    lv_nuttx_image_cache_init();
#endif

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN
    lv_nuttx_profiler_init();
#endif

    if(result) {
        lv_memzero(result, sizeof(lv_nuttx_result_t));
    }

#if !LV_USE_NUTTX_CUSTOM_INIT

    if(dsc && dsc->fb_path) {
        lv_display_t * disp = NULL;

#if LV_USE_NUTTX_LCD
        disp = lv_nuttx_lcd_create(dsc->fb_path);
#else
        disp = lv_nuttx_fbdev_create();
        if(lv_nuttx_fbdev_set_file(disp, dsc->fb_path) != 0) {
            lv_display_delete(disp);
            disp = NULL;
        }
#endif
        if(result) {
            result->disp = disp;
        }
    }

    if(dsc) {
#if LV_USE_NUTTX_TOUCHSCREEN
        if(dsc->input_path) {
            lv_indev_t * indev = lv_nuttx_touchscreen_create(dsc->input_path);
            if(result) {
                result->indev = indev;
            }
        }

        if(dsc->utouch_path) {
            lv_indev_t * indev = lv_nuttx_touchscreen_create(dsc->utouch_path);
            if(result) {
                result->utouch_indev = indev;
            }
        }
#endif
    }

#else

    lv_nuttx_init_custom(dsc, result);
#endif
}

#ifdef CONFIG_SCHED_CPULOAD

uint32_t lv_nuttx_get_idle(void)
{
    struct cpuload_s cpuload;
    int ret = clock_cpuload(0, &cpuload);
    if(ret < 0) {
        LV_LOG_WARN("clock_cpuload failed: %d", ret);
        return 0;
    }

    uint32_t idle = cpuload.active * 100 / cpuload.total;
    LV_LOG_TRACE("active = %" LV_PRIu32 ", total = %" LV_PRIu32,
                 cpuload.active, cpuload.total);

    return idle;
}

#endif

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

#if LV_USE_LOG
static void syslog_print(lv_log_level_t level, const char * buf)
{
    static const int priority[_LV_LOG_LEVEL_NUM] = {
        LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERR, LOG_CRIT
    };

    syslog(priority[level], "[LVGL] %s", buf);
}
#endif

#endif /*LV_USE_NUTTX*/
