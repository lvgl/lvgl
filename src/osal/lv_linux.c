#include "lv_os.h"

#if LV_USE_OS != LV_OS_NONE && defined(__linux__)

#include "../misc/lv_log.h"
#include <stdio.h>

#define LV_UPTIME_MONITOR_FILE "/proc/uptime"

static void lv_proc_get_delta(uint32_t now_s, int now_ms, uint32_t original_s,
                              int original_ms, uint32_t * delta_s, int * delta_ms);

static uint32_t last_uptime_s, last_idletime_s;
static int last_uptime_ms, last_idletime_ms;

uint32_t lv_os_get_idle_percent(void)
{
    FILE * fp = fopen(LV_UPTIME_MONITOR_FILE, "r");

    if(!fp) {
        LV_LOG_WARN("Failed to open " LV_UPTIME_MONITOR_FILE);
        return UINT_MAX;
    }
    // UINT32_MAX seconds > 136 years
    uint32_t uptime_s, idletime_s;

    // Range is [0:100[
    int uptime_ms, idletime_ms;

    int err = fscanf(fp,
                     "%" PRIu32 ".%d"
                     " %" PRIu32 ".%d",
                     &uptime_s, &uptime_ms, &idletime_s, &idletime_ms);

    fclose(fp);
    if(err != 4) {
        LV_LOG_WARN("Failed to parse " LV_UPTIME_MONITOR_FILE);
        return UINT_MAX;
    }

    uint32_t delta_uptime_s, delta_idletime_s;
    int delta_uptime_ms, delta_idletime_ms;

    /* Calculate the delta first to avoid overflowing */
    lv_proc_get_delta(uptime_s, uptime_ms, last_uptime_s,
                      last_uptime_ms, &delta_uptime_s, &delta_uptime_ms);

    lv_proc_get_delta(idletime_s, idletime_ms, last_idletime_s,
                      last_idletime_ms, &delta_idletime_s,
                      &delta_idletime_ms);

    /* From here onwards, there's no risk of overflowing as long as we call this function regularly */

    /* Update for next call */
    last_uptime_s = uptime_s;
    last_uptime_ms = uptime_ms;
    last_idletime_s = idletime_s;
    last_idletime_ms = idletime_ms;

    uint32_t total_ms = delta_uptime_ms + delta_idletime_ms;
    uint32_t total_s = delta_uptime_s + delta_idletime_s;

    if(total_ms >= 100) {
        total_s += 1;
        total_ms -= 100;
    }

    return ((delta_idletime_s * 100 + delta_idletime_ms) * 100) /
           (total_s * 100 + total_ms);
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

#endif
