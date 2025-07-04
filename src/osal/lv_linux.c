/**
 * @file lv_linux.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_os.h"

#if LV_USE_OS != LV_OS_NONE && defined(__linux__)

#include "../core/lv_global.h"
#include "../misc/lv_log.h"
#include "lv_linux_private.h"
#include <stdio.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/

#define LV_UPTIME_MONITOR_FILE              "/proc/stat"
#define LV_UPTIME_MONITOR_SELF_FILE         "/proc/self/stat"

#define LV_PROC_STAT_VAR_FORMAT        " %" PRIu32
#define LV_PROC_STAT_IGNORE_VAR_FORMAT " %*" PRIu32

#define last_proc_stat LV_GLOBAL_DEFAULT()->linux_last_proc_stat
#define last_self_ticks LV_GLOBAL_DEFAULT()->linux_last_self_proc_time_ticks
#define last_system_total_ticks_stat LV_GLOBAL_DEFAULT()->linux_last_system_total_ticks_stat

#define LV_SELF_PROC_STAT_BUFFER_SIZE 512
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t lv_read_proc_stat(lv_proc_stat_t * result);
static uint32_t lv_proc_stat_get_total(const lv_proc_stat_t * p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint32_t lv_os_get_idle_percent(void)
{
    lv_proc_stat_t proc_stat;
    {
        lv_result_t err = lv_read_proc_stat(&proc_stat);

        if(err == LV_RESULT_INVALID) {
            return UINT32_MAX;
        }

        for(size_t i = 0; i < LV_PROC_STAT_PARAMS_LEN; ++i) {
            uint32_t delta =
                proc_stat.buffer[i] - last_proc_stat.buffer[i];
            /* Update old for next call*/
            last_proc_stat.buffer[i] = proc_stat.buffer[i];
            /* Store delta in new */
            proc_stat.buffer[i] = delta;
        }
    }

    /* From here onwards, there's no risk of overflowing as long as we call this function regularly */
    const uint32_t total = lv_proc_stat_get_total(&proc_stat);

    if(total == 0) {
        return 0;
    }

    return (proc_stat.fields.idle * 100) / total;
}

uint32_t lv_os_get_proc_idle_percent(void)
{
    uint64_t self_current_time_ticks = 0;
    lv_proc_stat_t stat_current_system_total_ticks;
    lv_proc_stat_t stat_delta_system_ticks;

    FILE * self = fopen(LV_UPTIME_MONITOR_SELF_FILE, "r");
    if(!self) {
        LV_LOG_ERROR("Failed to open " LV_UPTIME_MONITOR_SELF_FILE);
        return UINT32_MAX;
    }

    char self_stat_buffer[LV_SELF_PROC_STAT_BUFFER_SIZE];

    if(!fgets(self_stat_buffer, sizeof(self_stat_buffer), self)) {
        fclose(self);
        LV_LOG_ERROR("Failed to read " LV_UPTIME_MONITOR_SELF_FILE);
        return UINT32_MAX;
    }

    fclose(self);

    /* The comm field can contain spaces and parentheses, so we find the last ')'
     * Skip the whitespace after finding the last ')' */
    char * p = strrchr(self_stat_buffer, ')');
    if(!p) {
        LV_LOG_ERROR(LV_UPTIME_MONITOR_SELF_FILE " is missing the closing ')'");
        return UINT32_MAX;
    }
    p++; /* move past the ')' */
    while(*p && (*p == ' ' || *p == '\t'))
        p++; /* skip whitespace after ')' */
    if(!*p) {
        LV_LOG_ERROR(LV_UPTIME_MONITOR_SELF_FILE " unexpectedly ends after the closing ')'");
        return UINT32_MAX;
    }

    uint64_t utime = 0;
    uint64_t stime = 0;

    int scanned_items = sscanf(p,
                               "%*c "                         // state (field 3)
                               "%*d %*d %*d %*d %*d "         // ppid, pgrp, session, tty_nr, tpgid (fields 4-8)
                               "%*u "                         // flags (field 9)
                               "%*u %*u %*u %*u "             // minflt, cminflt, majflt, cmajflt (fields 10-13)
                               "%" SCNu64 " %" SCNu64,        // utime, stime (fields 14-15)
                               &utime, &stime);

    if(scanned_items != 2) {
        LV_LOG_ERROR("Failed to parse utime/stime");
        return UINT32_MAX;
    }

    self_current_time_ticks = utime + stime;

    if(lv_read_proc_stat(&stat_current_system_total_ticks) != LV_RESULT_OK) {
        LV_LOG_ERROR("lv_read_proc_stat failed");
        return UINT32_MAX;
    }

    /* no delta on the first call so return 0, next call will have actual values*/
    if(last_self_ticks == 0) {
        last_self_ticks = self_current_time_ticks;
        last_system_total_ticks_stat = stat_current_system_total_ticks;

        return 100; /* 100% idle = 0% CPU usage*/
    }

    uint64_t delta_self_proc_ticks = self_current_time_ticks - last_self_ticks;

    for(size_t i = 0; i < LV_PROC_STAT_PARAMS_LEN; ++i) {
        stat_delta_system_ticks.buffer[i] = stat_current_system_total_ticks.buffer[i] - last_system_total_ticks_stat.buffer[i];
    }

    uint64_t delta_total_system_ticks = lv_proc_stat_get_total(&stat_delta_system_ticks);

    last_self_ticks = self_current_time_ticks;
    last_system_total_ticks_stat = stat_current_system_total_ticks;

    if(delta_total_system_ticks == 0) return 100;

    uint32_t cpu_usage_percent = (uint32_t)((delta_self_proc_ticks * 100ULL) / delta_total_system_ticks);

    return 100 - cpu_usage_percent;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_result_t lv_read_proc_stat(lv_proc_stat_t * result)
{
    FILE * fp = fopen(LV_UPTIME_MONITOR_FILE, "r");

    if(!fp) {
        LV_LOG_ERROR("Failed to open " LV_UPTIME_MONITOR_FILE);
        return LV_RESULT_INVALID;
    }
    const char * fmt = "cpu " LV_PROC_STAT_VAR_FORMAT LV_PROC_STAT_VAR_FORMAT
                       LV_PROC_STAT_VAR_FORMAT LV_PROC_STAT_VAR_FORMAT
                       LV_PROC_STAT_IGNORE_VAR_FORMAT LV_PROC_STAT_VAR_FORMAT
                       LV_PROC_STAT_VAR_FORMAT LV_PROC_STAT_VAR_FORMAT;

    int err = fscanf(fp, fmt, &result->fields.user, &result->fields.nice,
                     &result->fields.system, &result->fields.idle,
                     &result->fields.irq, &result->fields.softirq,
                     &result->fields.steal);

    fclose(fp);

    if(err != LV_PROC_STAT_PARAMS_LEN) {
        LV_LOG_ERROR("Failed to parse " LV_UPTIME_MONITOR_FILE);
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}
static uint32_t lv_proc_stat_get_total(const lv_proc_stat_t * p)
{
    uint32_t sum = 0;
    for(size_t i = 0; i < LV_PROC_STAT_PARAMS_LEN; ++i) {
        sum += p->buffer[i];
    }
    return sum;
}

#endif
