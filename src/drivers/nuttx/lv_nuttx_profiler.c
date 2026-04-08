/**
 * @file lv_nuttx_profiler.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_nuttx_profiler.h"

#if LV_USE_NUTTX && LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN

#include "../../misc/lv_profiler_builtin_private.h"
#include "../../misc/lv_log.h"
#include "../../core/lv_global.h"
#include "../../stdlib/lv_sprintf.h"
#include "../../stdlib/lv_string.h"
#include "lv_nuttx_entry.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#ifdef __NuttX__
    #include <nuttx/arch.h>
#else
    #include "mock/nuttx_arch.h"
#endif

/*********************
 *      DEFINES
 *********************/

#define trace_fd           (LV_GLOBAL_DEFAULT()->nuttx_ctx->trace_fd)

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

#if LV_USE_NUTTX_TRACE_FILE
    trace_fd = -1;
#endif

    lv_profiler_builtin_config_t config;
    lv_profiler_builtin_config_init(&config);
    config.tick_per_sec = 1000000000; /* 1 sec = 1000000000 nsec */
    config.tick_get_cb = tick_get_cb;
    config.flush_cb = flush_cb;
    lv_profiler_builtin_init(&config);
}

void lv_nuttx_profiler_set_file(const char * file)
{
#if LV_USE_NUTTX_TRACE_FILE
    if(trace_fd >= 0) {
        close(trace_fd);
    }

    trace_fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(trace_fd < 0) {
        LV_LOG_ERROR("Failed to open trace file %s, error: %d", file, errno);
    }
#endif
}

void lv_nuttx_profiler_deinit(void)
{
    lv_profiler_builtin_uninit();
#if LV_USE_NUTTX_TRACE_FILE
    if(trace_fd >= 0) {
        close(trace_fd);
    }
    trace_fd = -1;
#endif
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
#if LV_USE_NUTTX_TRACE_FILE
    if(trace_fd >= 0) {
        write(trace_fd, buf, lv_strlen(buf));
        return;
    }
#endif
    printf("%s", buf);
}

#endif
