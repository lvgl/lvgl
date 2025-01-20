#include "lv_os.h"

#if LV_USE_OS != LV_OS_NONE && defined(__linux__)

#include "../misc/lv_log.h"
#include <stdio.h>

#define LV_UPTIME_MONITOR_FILE "/proc/uptime"

static void lv_proc_get_delta(uint32_t now_s, int now_ms, uint32_t original_s,
                              int original_ms, uint32_t * delta_s, int * delta_ms);

static int lv_proc_get_uptime(uint32_t * now_s, int * now_ms, uint32_t * idle_s, int * idle_ms);

static uint32_t last_uptime_s, last_idletime_s;
static int last_uptime_ms, last_idletime_ms;

uint32_t lv_os_get_idle_percent(void)
{

    uint32_t delta_active_s, delta_idle_s;
    int delta_active_ms, delta_idle_ms;
    {

        /* UINT32_MAX seconds > 136 years */
        uint32_t active_s, idletime_s;
        /* Range is [0: 99[ */
        int active_ms, idletime_ms;

        int err = lv_proc_get_uptime(&active_s, &active_ms, &idletime_s, &idletime_ms);

        if(err < 0) {
            return UINT_MAX;
        }

        /* Calculate the delta first to avoid overflowing */
        lv_proc_get_delta(active_s, active_ms, last_uptime_s,
                          last_uptime_ms, &delta_active_s, &delta_active_ms);

        lv_proc_get_delta(idletime_s, idletime_ms, last_idletime_s,
                          last_idletime_ms, &delta_idle_s,
                          &delta_idle_ms);

        /* Update for next call */
        last_uptime_s = active_s;
        last_uptime_ms = active_ms;
        last_idletime_s = idletime_s;
        last_idletime_ms = idletime_ms;
    }

    /* From here onwards, there's no risk of overflowing as long as we call this function regularly */

    uint32_t total_ms = delta_active_ms + delta_idle_ms;
    uint32_t total_s = delta_active_s + delta_idle_s;

    if(total_ms >= 100) {
        total_s += 1;
        total_ms -= 100;
    }

    const uint32_t total = total_s * 100 + total_ms;

    if(total == 0) {
        return 0;
    }

    return ((delta_idle_s * 100 + delta_idle_ms) * 100) / total;
}

static void lv_proc_get_delta(uint32_t now_s, int now_ms, uint32_t original_s,
                              int original_ms, uint32_t * delta_s, int * delta_ms)
{
    *delta_s = now_s - original_s;
    *delta_ms = now_ms - original_ms;

    if(*delta_ms < 0) {
        *delta_s -= 1;
        *delta_ms += 100;
    }
}

static int lv_proc_get_uptime(uint32_t * active_s, int * active_ms, uint32_t * idle_s, int * idle_ms)
{

    FILE * fp = fopen(LV_UPTIME_MONITOR_FILE, "r");

    if(!fp) {
        LV_LOG_ERROR("Failed to open " LV_UPTIME_MONITOR_FILE);
        return -1;
    }

    int err = fscanf(fp,
                     "%" PRIu32 ".%d"
                     " %" PRIu32 ".%d",
                     active_s, active_ms, idle_s, idle_ms);

    fclose(fp);

    if(err != 4) {
        LV_LOG_ERROR("Failed to parse " LV_UPTIME_MONITOR_FILE);
        return -1;
    }
    return 0;
}

#endif
