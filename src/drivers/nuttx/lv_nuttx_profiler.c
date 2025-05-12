/**
 * @file lv_nuttx_profiler.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_nuttx_profiler.h"
#include "../../../lvgl.h"

#if LV_USE_NUTTX && LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN

#include <nuttx/arch.h>
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/

#define TICK_TO_NSEC(tick) ((tick) * 1000 / cpu_freq)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint32_t cpu_freq = 0; /* MHz */

/**********************
 *  STATIC VARIABLES
 **********************/

static uint64_t tick_get_cb(void);
static void flush_cb(const char * buf);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_nuttx_profiler_init(void)
{
    cpu_freq = (uint32_t)up_perf_getfreq() / 1000000;
    if(cpu_freq == 0) {
        LV_LOG_ERROR("Failed to get CPU frequency");
        return;
    }
    LV_LOG_USER("CPU frequency: %" LV_PRIu32 " MHz", cpu_freq);

    lv_profiler_builtin_config_t config;
    lv_profiler_builtin_config_init(&config);
    config.tick_per_sec = 1000000000; /* 1 sec = 1000000000 nsec */
    config.tick_get_cb = tick_get_cb;
    config.flush_cb = flush_cb;
    lv_profiler_builtin_init(&config);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint64_t tick_get_cb(void)
{
    static uint32_t prev_tick = 0;
    static uint64_t cur_tick_ns = 0;
    uint32_t act_time = up_perf_gettime();
    uint64_t elaps;

    /*If there is no overflow in sys_time simple subtract*/
    if(act_time >= prev_tick) {
        elaps = act_time - prev_tick;
    }
    else {
        elaps = UINT32_MAX - prev_tick + 1;
        elaps += act_time;
    }

    cur_tick_ns += TICK_TO_NSEC(elaps);
    prev_tick = act_time;
    return cur_tick_ns;
}

static void flush_cb(const char * buf)
{
    printf("%s", buf);
}

#endif
