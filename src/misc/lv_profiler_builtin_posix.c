/**
 * @file lv_profiler_builtin_posix.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_profiler_builtin_private.h"

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN && LV_USE_PROFILER_BUILTIN_POSIX

#include <pthread.h>
#include <stdio.h>
#include <time.h>

#if defined(__linux__)
    #include <sys/syscall.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint64_t tick_get_cb(void);
static void flush_cb(const char * buf);
static int tid_get_cb(void);
static int cpu_get_cb(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_profiler_builtin_posix_init(void)
{
    lv_profiler_builtin_config_t config;
    lv_profiler_builtin_config_init(&config);

    /* One second is equal to 1000000000 nanoseconds */
    config.tick_per_sec = 1000000000;
    config.tick_get_cb = tick_get_cb;
    config.flush_cb = flush_cb;
    config.tid_get_cb = tid_get_cb;
    config.cpu_get_cb = cpu_get_cb;
    lv_profiler_builtin_init(&config);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint64_t tick_get_cb(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

static void flush_cb(const char * buf)
{
    printf("%s", buf);
}

static int tid_get_cb(void)
{
#if defined(__linux__)
    return (int)syscall(SYS_gettid);
#else
    return (int)pthread_self();
#endif
}

static int cpu_get_cb(void)
{
#if defined(__linux__)
    return (int)syscall(SYS_getcpu);
#else
    return 0;
#endif
}

#endif
