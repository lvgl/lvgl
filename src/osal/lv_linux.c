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

/*********************
 *      DEFINES
 *********************/

#define LV_UPTIME_MONITOR_FILE         "/proc/stat"

#define LV_PROC_STAT_VAR_FORMAT        " %" PRIu32
#define LV_PROC_STAT_IGNORE_VAR_FORMAT " %*" PRIu32

#define last_proc_stat LV_GLOBAL_DEFAULT()->linux_last_proc_stat

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
