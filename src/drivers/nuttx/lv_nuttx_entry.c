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
#include <pthread.h>
#include "lv_nuttx_cache.h"
#include "lv_nuttx_image_cache.h"
#include "../../core/lv_global.h"
#include "lv_nuttx_profiler.h"

#include "../../../lvgl.h"

/*********************
 *      DEFINES
 *********************/

#define nuttx_ctx_p (LV_GLOBAL_DEFAULT()->nuttx_ctx)

#if (LV_USE_FREETYPE || LV_USE_THORVG)
    #define LV_NUTTX_MIN_STACK_SIZE (32 * 1024)
#else
    #define LV_NUTTX_MIN_STACK_SIZE (8 * 1024)
#endif

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
static void check_stack_size(void);

#ifdef CONFIG_LV_USE_NUTTX_LIBUV
    static void lv_nuttx_uv_loop(lv_nuttx_result_t * result);
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

    check_stack_size();

    lv_nuttx_cache_init();

    lv_nuttx_image_cache_init();

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

void lv_nuttx_run(lv_nuttx_result_t * result)
{
#ifdef CONFIG_LV_USE_NUTTX_LIBUV
    lv_nuttx_uv_loop(&ui_loop, result);
#else
    while(1) {
        uint32_t idle;
        idle = lv_timer_handler();

        /* Minimum sleep of 1ms */
        idle = idle ? idle : 1;
        usleep(idle * 1000);
    }
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

void lv_nuttx_deinit(lv_nuttx_result_t * result)
{
#if !LV_USE_NUTTX_CUSTOM_INIT
    if(result) {
        if(result->disp) {
            lv_display_delete(result->disp);
            result->disp = NULL;
        }

        if(result->indev) {
            lv_indev_delete(result->indev);
            result->indev = NULL;
        }

        if(result->utouch_indev) {
            lv_indev_delete(result->utouch_indev);
            result->utouch_indev = NULL;
        }
    }
#else
    lv_nuttx_deinit_custom(result);
#endif

    if(nuttx_ctx_p) {
        lv_nuttx_cache_deinit();
        lv_nuttx_image_cache_deinit();

        lv_free(nuttx_ctx_p);
        nuttx_ctx_p = NULL;
    }
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

#if LV_USE_LOG
static void syslog_print(lv_log_level_t level, const char * buf)
{
    static const int priority[LV_LOG_LEVEL_NUM] = {
        LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERR, LOG_CRIT
    };

    syslog(priority[level], "[LVGL] %s", buf);
}
#endif

#ifdef CONFIG_LV_USE_NUTTX_LIBUV
static void lv_nuttx_uv_loop(lv_nuttx_result_t * result)
{
    uv_loop_t loop;
    lv_nuttx_uv_t uv_info;
    void * data;

    uv_loop_init(&loop);

    lv_memzero(&uv_info, sizeof(uv_info));
    uv_info.loop = &loop;
    uv_info.disp = result->disp;
    uv_info.indev = result->indev;
#ifdef CONFIG_UINPUT_TOUCH
    uv_info.uindev = result->utouch_indev;
#endif

    data = lv_nuttx_uv_init(&uv_info);
    uv_run(loop, UV_RUN_DEFAULT);
    lv_nuttx_uv_deinit(&data);
}
#endif

static void check_stack_size(void)
{
    pthread_t tid = pthread_self();
    ssize_t stack_size = pthread_get_stacksize_np(tid);
    LV_LOG_USER("tid: %d, Stack size : %zd", (int)tid, stack_size);

    if(stack_size < LV_NUTTX_MIN_STACK_SIZE) {
        LV_LOG_ERROR("Stack size is too small. Please increase it to %d bytes or more.",
                     LV_NUTTX_MIN_STACK_SIZE);
    }
}

#endif /*LV_USE_NUTTX*/

