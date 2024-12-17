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

#define TICK_PER_SEC_MAX 1000000000 /* 1GHz */
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
static uint64_t tick_nsec_get_cb(void);
static void flush_cb(const char * buf);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_nuttx_profiler_init(void)
{
    const uint32_t tick_per_sec = (uint32_t)up_perf_getfreq();
    cpu_freq = tick_per_sec / 1000000;
    if(cpu_freq == 0) {
        LV_LOG_ERROR("Failed to get CPU frequency");
        return;
    }
    LV_LOG_USER("CPU frequency: %" LV_PRIu32 " MHz", cpu_freq);

    lv_profiler_builtin_config_t config;
    lv_profiler_builtin_config_init(&config);

    if(tick_per_sec <= TICK_PER_SEC_MAX) {
        LV_LOG_USER("Use the original tick source directly");
        config.tick_per_sec = tick_per_sec;
        config.tick_get_cb = tick_get_cb;
    }
    else {
        LV_LOG_USER("Use tick to nanosecond time source");
        config.tick_per_sec = TICK_PER_SEC_MAX;
        config.tick_get_cb = tick_nsec_get_cb;
    }

    config.flush_cb = flush_cb;
    lv_profiler_builtin_init(&config);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline uint32_t tick_get_elaps(void)
{
    static uint32_t prev_tick = 0;
    uint32_t act_time = up_perf_gettime();
    uint32_t elaps;

    /*If there is no overflow in sys_time simple subtract*/
    if(act_time >= prev_tick) {
        elaps = act_time - prev_tick;
    }
    else {
        elaps = UINT32_MAX - prev_tick + 1;
        elaps += act_time;
    }

    return elaps;
}

static uint64_t tick_get_cb(void)
{
    static uint64_t cur_tick = 0;
    cur_tick += tick_get_elaps();
    return cur_tick;
}

static uint64_t tick_nsec_get_cb(void)
{
    static uint64_t cur_tick_ns = 0;
    cur_tick_ns += TICK_TO_NSEC(tick_get_elaps());
    return cur_tick_ns;
}

static void flush_cb(const char * buf)
{
    printf("%s", buf);
}

#endif
